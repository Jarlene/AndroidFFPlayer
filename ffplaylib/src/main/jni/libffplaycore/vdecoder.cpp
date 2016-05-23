#include "common.h"
#include "playcore.h"
#include "vdecoder.h"

CVideoDecoder::CVideoDecoder()
{
	Reset();
	mpFrame				= PlayCore::GetInstance()->avcodec_alloc_frame();
    INFO ("0x%x",this);
}

CVideoDecoder::~CVideoDecoder()
{
    INFO ("0x%x",this);
	PlayCore::GetInstance()->av_free(mpFrame);
}

//�˴��������������ڸմ򿪺�seek�󣬲��ʺ�pause��play
void CVideoDecoder::Reset()
{
	mbFirstKeyFrame = true;
	mbNeedReSync	= false;
	mbNextFrame 	= true;
}

void CVideoDecoder::ReDrawPictrue()
{
	if(mpStubRender)
	{
		((CVideoStubRender*)mpStubRender)->ShowPicture (mpFrame);
	}
}

void CVideoDecoder::ThreadEntry () 
{
    CDemux* pDemux 					= mpDemux;
    AVStream* pStream 				= mpStream;
    CVideoStubRender* pVStubRender 	= (CVideoStubRender*)mpStubRender;
    CMasterClock* pClock 			= mpClock;
    int Ret 						= ERR_NONE;
	int bFrameFinished				= 0;
	AVCodecContext *pCodecContext	= NULL;
	AVPacket AVPkt;
	int64_t  PTS;
	
    if (pDemux == NULL || pStream == NULL || pVStubRender == NULL || pStream->codec == NULL) 
	{
        ERROR ("mpDecoder || mpStream || mpStubRender || mpStream->codec== NULL!");
        return;
    }
    pCodecContext = pStream->codec;

	memset((void*)&AVPkt,0,sizeof(AVPacket));

	DEBUG ("start video decoder loop!");	
	
    for(;;) 
	{
        if(mbQuit) 
		{
			INFO("mbQuit");
			PlayCore::GetInstance()->avcodec_flush_buffers(pCodecContext);
            break;
        }

		if(pDemux->GetSyncPTS()>=0.0)//�Ѿ��趨��SyncPTS
		{						
			if(!pDemux->IsSynced())//û�б���Ƶ��Sync
			{
				DEBUG("NO FOUND SYNCED !!");
				usleep(25*1000);
				continue;
			}
			else if(!pClock->IsRun())//�Ѿ��趨SyncPTs���ұ�������sync���ҵ�ǰ״̬���ǲ���״̬
			{
				usleep(1*1000);//Reduce CPU usage
				continue;
			}
		}
		else 
		{	
			DEBUG("TRY SHOW FIRST KEY VIDEO FRAME mbFirstKeyFrame %s",mbFirstKeyFrame?"true":"false");
		}
		
		if(mbNextFrame)
		{
			AVPkt = pDemux->GetVideoPacket ();
			if (AVPkt.data == 0 || AVPkt.size == 0) 
			{
				INFO ("get NULL packet! continue! must be end of pStream!!! stop thread!");
				break;
			}
		
			//���A/V�Ѿ�ͬ��������ʱ��Ƶʱ������������ϵͳʱ��
			if(pDemux->IsSynced()&&mbNeedReSync)
			{
				double CurClock = pClock->GetCurrentClock ();
				double FramePTS = AVPkt.pts * av_q2d (pStream->time_base);
	    		double Diff = FramePTS - CurClock;
	            DEBUG ("V:SyncOut CurClock=%f, FramePTS=%f, Diff=%f", CurClock, FramePTS, Diff);
								
				if (Diff >= CMasterClock::AVThresholdSync) 
				{
		            DEBUG ("V:Sync CurClock=%f, FramePTS=%f, Diff=%f", CurClock, FramePTS, Diff);
				}
				else
				{
					PlayCore::GetInstance()->av_free_packet (&AVPkt);
					memset((void*)&AVPkt,0,sizeof(AVPacket));
					continue;
				}
				
			}
			
	        int len = CVideoDecoderImp::Decode (pCodecContext, mpFrame, &bFrameFinished, &AVPkt, &PTS);
			
	        if (len < 0 || bFrameFinished == 0) 
			{
	            WARN ("CVideoDecoderImp::Decode fail! continue to next packet!");
	            PlayCore::GetInstance()->av_free_packet (&AVPkt);
				memset((void*)&AVPkt,0,sizeof(AVPacket));
	            continue;
	        }

			DEBUG("mbFirstKeyFrame %d,key_frame %d,pict_type %d mbNeedReSync %d AV_PICTURE_TYPE_I %d",mbFirstKeyFrame,mpFrame->key_frame,mpFrame->pict_type,mbNeedReSync,AV_PICTURE_TYPE_I);
			//��Ҫ�ؼ�֡
			if(!pDemux->IsSynced())
			{
				//���ǹؼ�֡continue
				if(!(mpFrame->key_frame && mpFrame->pict_type == AV_PICTURE_TYPE_I))
				{
					DEBUG("Not Key frame continue");
					PlayCore::GetInstance()->av_free_packet (&AVPkt);
					memset((void*)&AVPkt,0,sizeof(AVPacket));
					continue;
				}
				else //��һ������ƵSync
				{
					INFO("First Sync after opened");
					mbFirstKeyFrame = true;
				}
					
			}
			else if(mbNeedReSync) //��Ƶʱ������������ϵͳʱ��,��һ�����ʵĹؼ�֡
			{
				if(!(mpFrame->key_frame && mpFrame->pict_type == AV_PICTURE_TYPE_I))
				{
					DEBUG("Not Key frame continue");
					PlayCore::GetInstance()->av_free_packet (&AVPkt);
					memset((void*)&AVPkt,0,sizeof(AVPacket));
					continue;
				}
				else
				{
					INFO("mbNeedReSync %d",mbNeedReSync);
					mbNeedReSync 	= false;
					mbFirstKeyFrame = false;
				}
			}
			else//pDemux go on
			{
				mbFirstKeyFrame = false;
			}
		}
        if (pClock) 
		{
			double FramePTS = PTS * av_q2d (pStream->time_base);
			//��Ⱦ��Ƶ�׸��ؼ�֡���Ҳ��ǲ��Ź�������Ƶ���������ϵͳʱ�ӡ��趨��Ƶͬ��ʱ���
			if(mbFirstKeyFrame)
			{							
				Ret = pVStubRender->ShowPicture (mpFrame);

				INFO("IS Key frame mbFirstKeyFrame %d FramePTS %f ShowPicture Ret %d",mbFirstKeyFrame,FramePTS,Ret);
				
				//�ɹ���Ⱦ��Ƶ
				if(Ret>=0 || Ret == ERR_DEVICE_NOSET)
				{
					//�õ�ͬ��ʱ��㣬
					double SyncPTS = pDemux->GetSyncPTS();
					INFO("pDemux->GetSyncPTS() %f",SyncPTS);
					if(SyncPTS<0.0)
					{
						//�趨ͬ��ʱ���
						pDemux->SetSyncPTS(FramePTS);
						//�趨ʱ�ӵ���ʼʱ���
						pClock->SetOriginClock (FramePTS);

						//surface fixed  
						pVStubRender->ShowPicture (mpFrame);

						//���û����Ƶ�������趨ΪSynced;
						if(pDemux->GetAudioStreamIndex()<0)
						{
							pDemux->SetSynced(true);
						}

						mbFirstKeyFrame = false;
						mbNextFrame = true;
					}
				}
			}
            else 
			{
				double CurClock = pClock->GetCurrentClock ();
	            double Diff = FramePTS - CurClock;

				DEBUG ("V:CurClock=%f, FramePTS=%f, Diff=%f", CurClock, FramePTS, Diff);
				
				if (fabs(Diff) < CMasterClock::AVThresholdNoSync) 
				{
					mbNextFrame = true;
					
	                if (Diff <= 0) 
					{
	                    VERBOSE ("show it at once.");
	                } 
					else 
					{
	                    unsigned int usec = Diff * 1000 * 1000;
	                    VERBOSE ("wait %d usec", usec);
	                    usleep (usec);
	                }

	                Ret = pVStubRender->ShowPicture (mpFrame);
	            } 
				else
				{
	                if (Diff < 0) 
					{
						mbNextFrame = true;

						//���ϵͳʱ��������Ƶʱ��Ϊ��׼�ģ�����ϵͳʱ��
						if(pClock->GetClockType()==CMasterClock::CLOCK_VIDEO)
						{
							WARN("we reset master timer to video FramePTS");
                    		double ClockTime = PlayCore::GetInstance()->av_gettime() / 1000000.0;
                   			pClock->SetOriginClock (FramePTS, ClockTime);
                   			Ret = pVStubRender->ShowPicture (mpFrame);
						}
						else//������������(��Ƶ��ϵͳʱ��)Ϊϵͳʱ�ӣ���ʱ��Ƶ���������ͬ����ʱ�䷶Χֵ��
						{	//����ֱ������Ҫ�趨��Ҫ��������Ĺؼ�֡��ʶ

							DEBUG("we need ReSync video FramePTS Diff %f",Diff);
						}
	                } 
					else 
					{
						WARN ("video FramePTS far early than curr_pts Diff %f",Diff);
						unsigned int usec = Diff * 1000 * 1000;
						mbNextFrame  = false;
						usleep (1*1000);
	                }
	            }
				DEBUG("ShowPicture Ret %d",Ret);
            }
        } 
		else 
		{
            ERROR ("you have not set Master Clock!!! will not show pictures!");
        }
		
        PlayCore::GetInstance()->av_free_packet (&AVPkt);
		memset((void*)&AVPkt,0,sizeof(AVPacket));
    }
	
	Reset();
	
	if (AVPkt.data != 0) 
	{
		PlayCore::GetInstance()->av_free_packet (&AVPkt);
	}
    DEBUG ("end of video out thread!");
}

