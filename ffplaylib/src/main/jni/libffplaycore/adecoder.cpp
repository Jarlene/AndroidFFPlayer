#include <unistd.h>
#include "playcore.h"
#include "adecoder.h"
#include "audioresample.h"

CAudioDecoder::CAudioDecoder()
{
	mAudioResample		= NULL;
	memset((void*)&mInAudioFormat,0,sizeof(AudioFormat));
	memset((void*)&mOutAudioFormat,0,sizeof(AudioFormat));
	INFO("0x%x",this);
}

CAudioDecoder::~CAudioDecoder()
{
	if(mAudioResample != NULL)
	{
		delete mAudioResample;
		mAudioResample = NULL;
	}
	INFO("0x%x",this);
}

void CAudioDecoder::SetOutputFormat(int SampleRate,int nChannel,int BitPersample)
{
	mOutAudioFormat.Bits		= BitPersample;
	mOutAudioFormat.SampleRate 	= SampleRate;
	mOutAudioFormat.Channels	= nChannel;
}

void CAudioDecoder::ThreadEntry () 
{
    CDemux* 		pDemux 			= mpDemux;
    AVStream* 		pStream 		= mpStream;
    CAudioStubRender* pAStubRender	= (CAudioStubRender*)mpStubRender;
    CMasterClock* 	pClock 			= mpClock;
	uint8_t* 		FrameBuf 		= CAudioDecoderImp::mBuffer;
    int 			FrameSize 		= CAudioDecoderImp::miBufferSize;

	uint8_t* 		FrameBufResample = CAudioDecoderImp::mBufferResample;
    int 			FrameSizeResample= CAudioDecoderImp::miBufferSizeResample;

	uint8_t* 		FrameBufOut 	= NULL;
	int		 		FrameSizeOut	= 0;

	int				OSize 			= 0;
	int 			ISize 			= 0;
    int64_t 		PTS 			= 0;
    int 			IgnoreSize		= 0;
	int				Ret				= ERR_NONE;
    AVCodecContext *pCodecContent	= NULL;
	bool			bPktQueueEmpty	= false;
	
	AVPacket 		AVPkt;
	AudioSample		SrcAudioSample;
	int 			Len;
	
    if (pDemux == NULL || pStream == NULL || pAStubRender == NULL || pStream->codec == NULL) 
	{
        ERROR ("mpDemux || mpStream || mpStubRender || mpStream->codec== NULL!");
        return;
    }

	memset((void*)&AVPkt,0,sizeof(AVPacket));
	memset((void*)&SrcAudioSample,0,sizeof(AudioSample));
	
    pCodecContent = pStream->codec;
		
    INFO ("start audio decoder loop!");

    for(;;) 
	{
        if(mbQuit) 
		{
			INFO("mbQuit");
			PlayCore::GetInstance()->avcodec_flush_buffers(pCodecContent);
			pAStubRender->Stop();
			pAStubRender->Flush();
            break;
        }
		//�Ƿ�����Ƶ������Ƶ��һ֡�Ѿ����ͬ����Ƶʱ��,û����Ƶ�������Ѿ��趨����Ƶ���ĵ�SyncPTS����ִ��
        if(pDemux->GetVideoStreamIndex() >= 0&&pDemux->GetSyncPTS()<0)
        {
        	//����Ƶ��������û�б���Ƶ������SyncPTS
        	usleep(25*1000);
			continue;
        }
		//����Ƶ�Ѿ�ͬ����ͬ���㵫�ǲ���������ͣ״̬	
		if(pDemux->IsSynced())
		{
			if(!pDemux->IsAudioReadyForPlay() && Ret == ERR_BUFFER_FULL)//��Ƶ�豸д����
			{
				usleep(1*1000);

				pDemux->SetAudioReadForPlay();
				
			}
			//���ʱ�ӷ���ת״̬
			if(!pClock->IsRun())
			{
				//��Ƶ������Ǵ򿪲���״̬����ͣ��
				if(pAStubRender->IsStart())
				{
					pAStubRender->Pause();
				}
				usleep(1*1000);//Reduce CPU usage
			}
			else if(!pAStubRender->IsStart())//ʱ����ת״̬��Ƶ�豸д���������������Ƶ���
			{
				pAStubRender->Start();
			}
			else//ʱ����ת״̬����Ƶ�������
			{
				if(Ret == ERR_BUFFER_FULL)//��ʱд������sleepʱ���Գ�
				{
					usleep(25*1000);//Reduce CPU usage
				}
			}
		}

		//�����ǰ���Ѿ��������ȡ�°��Ҷ���δ��ʾΪ��
		if (AVPkt.size == 0 && !bPktQueueEmpty)
        {
    		AVPkt = pDemux->GetAudioPacket ();
			if(AVPkt.size == 0)
				bPktQueueEmpty = true;
			else
				SrcAudioSample.PTS = AVPkt.pts * av_q2d (pStream->time_base);
			
			//���ò���
			IgnoreSize = 0;
			FrameSize  = CAudioDecoderImp::miBufferSize;
		}

		//�����ǰ���Ѿ���������Ҷ��б�ʾΪ��
		if (AVPkt.size == 0 && bPktQueueEmpty)
		{			
			if(pAStubRender->IsRenderFinish())
			{
	            break;
			}
			else
			{
				usleep(10*1000);
				Ret = ERR_WAIT;
				continue;
			}
        }

        do 
		{
			if(Ret == ERR_NONE)
            {
            	PTS = 0;
				Len = 0;
				
				if(IgnoreSize != 0)
				{
					SrcAudioSample.PTS = TIME_UNKNOWN;
				}
				
	            Len = CAudioDecoderImp::Decode (pCodecContent, (int16_t*)FrameBuf, &FrameSize, &AVPkt, &PTS, IgnoreSize);
	            IgnoreSize += Len;
				
	            if (Len < 0 || FrameSize <= 0) 
				{
	                DEBUG ("%s",(FrameSize <= 0)?"no data! begin next frame!":"decode frame error!");
					PlayCore::GetInstance()->av_free_packet (&AVPkt);
					memset((void*)&AVPkt,0,sizeof(AVPacket));
	                break;
	            }

			   	AudioFormat InAudioFormat;
				InAudioFormat.SampleRate = pCodecContent->sample_rate;
				InAudioFormat.Channels	 = pCodecContent->channels;	
				InAudioFormat.Bits		 = PlayCore::GetInstance()->av_get_bits_per_sample_fmt(pCodecContent->sample_fmt);

				if(!CAudioResample::IsEqualAudioFormat(InAudioFormat,mOutAudioFormat))
				{
					if(mAudioResample && !CAudioResample::IsEqualAudioFormat(InAudioFormat,mInAudioFormat) )
				    {
		    			INFO("del mAudioResample 0x%x",mAudioResample);
				    	delete mAudioResample;
					    mAudioResample = NULL;
				    }

					if(!mAudioResample)
				    {	
				    	mInAudioFormat = InAudioFormat;
						
				      	mAudioResample = new CAudioResample(InAudioFormat,mOutAudioFormat);
						INFO("new mAudioResample 0x%x",mAudioResample);
						if(mAudioResample->Init() != ERR_NONE)
						{
							ERROR("mAudioResample.Init() %d", pCodecContent->sample_fmt);
							break;
						}
				    }

					FrameSizeResample=mAudioResample->Resample((short*)FrameBufResample, (short*)FrameBuf,FrameSize);
					
					if(FrameSizeResample <= 0)
				    {
				       	WARN("av_audio_convert - Unable to convert %d to SAMPLE_FMT_S16", (int)pCodecContent->sample_fmt);
				       	continue;
				    }
					
					SrcAudioSample.Ptr	= FrameBufResample;
					SrcAudioSample.Size= FrameSizeResample;
				}
				else
				{
					SrcAudioSample.Ptr	= FrameBuf;
					SrcAudioSample.Size= FrameSize;
				}
				
	            if (pClock) 
				{
	                double FramePTS 	= PTS * av_q2d (pStream->time_base);            
					double Diff 		= 0.0f;
					double SyncPTS 		= pDemux->GetSyncPTS();
					bool   IsSynced 	= pDemux->IsSynced();
					int    VStreamIndex = pDemux->GetVideoStreamIndex();
									
					if((VStreamIndex<0 || (VStreamIndex>=0 && SyncPTS>=0.0)) && !IsSynced)
					{					
						if(VStreamIndex < 0)
						{
							pDemux->SetSyncPTS(FramePTS);
							
							pClock->SetOriginClock (FramePTS);

							Diff = CMasterClock::AVThresholdSync;
						}
						else
						{
							Diff = (FramePTS - SyncPTS);
						}

						DEBUG("A:SyncPTS=%f, FramePTS=%f, Diff=%f", SyncPTS, FramePTS, Diff);
						
						if (Diff>=CMasterClock::AVThresholdSync) 
						{						
							pDemux->SetSynced();//�趨Sync
						}
					}
				}
				else 
				{
	                ERROR ("you have not set Master Clock!!! will not show audio!");
	            }
            } 

			if(pDemux->IsSynced())
			{
				Ret = pAStubRender->Write (SrcAudioSample);
				
				if(Ret == ERR_BUFFER_FULL)
				{
					DEBUG ("Write AudioSample to Device Ret == ERR_BUFFER_FULL");
					break;
				}
				else if(Ret == ERR_NONE)
				{

				}
				else
				{
					//other error
				}
			}
			
        } while(1);

    }

	pAStubRender->Stop();
	pAStubRender->Flush();
			
	if(mAudioResample != NULL)
	{
		INFO("del mAudioResample 0x%x",mAudioResample);
		delete mAudioResample;
		mAudioResample = NULL;
	}
	if (AVPkt.data != NULL) 
	{
		PlayCore::GetInstance()->av_free_packet (&AVPkt);
	}
    DEBUG ("end of audio out thread!");
}
