/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Engine implementation */

#include "sles_allinclusive.h"


static SLresult IEngine_CreateLEDDevice(SLEngineItf self, SLObjectItf *pDevice, SLuint32 deviceID,
    SLuint32 numInterfaces, const SLInterfaceID *pInterfaceIds, const SLboolean *pInterfaceRequired)
{
    SL_ENTER_INTERFACE

#if USE_PROFILES & USE_PROFILES_OPTIONAL
    if ((NULL == pDevice) || (SL_DEFAULTDEVICEID_LED != deviceID)) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pDevice = NULL;
        unsigned exposedMask;
        const ClassTable *pCLEDDevice_class = objectIDtoClass(SL_OBJECTID_LEDDEVICE);
        if (NULL == pCLEDDevice_class) {
            result = SL_RESULT_FEATURE_UNSUPPORTED;
        } else {
            result = checkInterfaces(pCLEDDevice_class, numInterfaces, pInterfaceIds,
                pInterfaceRequired, &exposedMask);
        }
        if (SL_RESULT_SUCCESS == result) {
            CLEDDevice *this = (CLEDDevice *) construct(pCLEDDevice_class, exposedMask, self);
            if (NULL == this) {
                result = SL_RESULT_MEMORY_FAILURE;
            } else {
                this->mDeviceID = deviceID;
                IObject_Publish(&this->mObject);
                // return the new LED object
                *pDevice = &this->mObject.mItf;
            }
        }
    }
#else
    result = SL_RESULT_FEATURE_UNSUPPORTED;
#endif

    SL_LEAVE_INTERFACE
}


static SLresult IEngine_CreateVibraDevice(SLEngineItf self, SLObjectItf *pDevice, SLuint32 deviceID,
    SLuint32 numInterfaces, const SLInterfaceID *pInterfaceIds, const SLboolean *pInterfaceRequired)
{
    SL_ENTER_INTERFACE

#if USE_PROFILES & USE_PROFILES_OPTIONAL
    if ((NULL == pDevice) || (SL_DEFAULTDEVICEID_VIBRA != deviceID)) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pDevice = NULL;
        unsigned exposedMask;
        const ClassTable *pCVibraDevice_class = objectIDtoClass(SL_OBJECTID_VIBRADEVICE);
        if (NULL == pCVibraDevice_class) {
            result = SL_RESULT_FEATURE_UNSUPPORTED;
        } else {
            result = checkInterfaces(pCVibraDevice_class, numInterfaces,
                pInterfaceIds, pInterfaceRequired, &exposedMask);
        }
        if (SL_RESULT_SUCCESS == result) {
            CVibraDevice *this = (CVibraDevice *) construct(pCVibraDevice_class, exposedMask, self);
            if (NULL == this) {
                result = SL_RESULT_MEMORY_FAILURE;
            } else {
                this->mDeviceID = deviceID;
                IObject_Publish(&this->mObject);
                // return the new vibra object
                *pDevice = &this->mObject.mItf;
            }
        }
    }
#else
    result = SL_RESULT_FEATURE_UNSUPPORTED;
#endif

    SL_LEAVE_INTERFACE
}


static SLresult IEngine_CreateAudioPlayer(SLEngineItf self, SLObjectItf *pPlayer,
    SLDataSource *pAudioSrc, SLDataSink *pAudioSnk, SLuint32 numInterfaces,
    const SLInterfaceID *pInterfaceIds, const SLboolean *pInterfaceRequired)
{
    SL_ENTER_INTERFACE

    if (NULL == pPlayer) {
       result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pPlayer = NULL;
        unsigned exposedMask;
        const ClassTable *pCAudioPlayer_class = objectIDtoClass(SL_OBJECTID_AUDIOPLAYER);
        assert(NULL != pCAudioPlayer_class);
        result = checkInterfaces(pCAudioPlayer_class, numInterfaces,
            pInterfaceIds, pInterfaceRequired, &exposedMask);
        if (SL_RESULT_SUCCESS == result) {

            // Construct our new AudioPlayer instance
            CAudioPlayer *this = (CAudioPlayer *) construct(pCAudioPlayer_class, exposedMask, self);
            if (NULL == this) {
                result = SL_RESULT_MEMORY_FAILURE;
            } else {

                do {

                    // Initialize private fields not associated with an interface

                    // Default data source in case of failure in checkDataSource
                    this->mDataSource.mLocator.mLocatorType = SL_DATALOCATOR_NULL;
                    this->mDataSource.mFormat.mFormatType = SL_DATAFORMAT_NULL;

                    // Default data sink in case of failure in checkDataSink
                    this->mDataSink.mLocator.mLocatorType = SL_DATALOCATOR_NULL;
                    this->mDataSink.mFormat.mFormatType = SL_DATAFORMAT_NULL;

                    // Default is no per-channel mute or solo
                    this->mMuteMask = 0;
                    this->mSoloMask = 0;

                    // Will be set soon for PCM buffer queues, or later by platform-specific code
                    // during Realize or Prefetch
                    this->mNumChannels = 0;
                    this->mSampleRateMilliHz = 0;

                    // More default values, in case destructor needs to be called early
                    this->mDirectLevel = 0;
#ifdef USE_OUTPUTMIXEXT
                    this->mTrack = NULL;
                    this->mGains[0] = 1.0f;
                    this->mGains[1] = 1.0f;
                    this->mDestroyRequested = SL_BOOLEAN_FALSE;
#endif
#ifdef USE_SNDFILE
                    this->mSndFile.mPathname = NULL;
                    this->mSndFile.mSNDFILE = NULL;
                    memset(&this->mSndFile.mSfInfo, 0, sizeof(SF_INFO));
                    memset(&this->mSndFile.mMutex, 0, sizeof(pthread_mutex_t));
                    this->mSndFile.mEOF = SL_BOOLEAN_FALSE;
                    this->mSndFile.mWhich = 0;
                    memset(this->mSndFile.mBuffer, 0, sizeof(this->mSndFile.mBuffer));
#endif
#ifdef ANDROID
                    // extra safe initializations of pointers, in case of incomplete construction
                    this->mpLock = NULL;
                    this->mAudioTrack = NULL;
                    // placement new (explicit constructor)
                    (void) new (&this->mSfPlayer) android::sp<android::SfPlayer>();
                    (void) new (&this->mAuxEffect) android::sp<android::AudioEffect>();
#endif

                    // Check the source and sink parameters against generic constraints,
                    // and make a local copy of all parameters in case other application threads
                    // change memory concurrently.

                    result = checkDataSource(pAudioSrc, &this->mDataSource);
                    if (SL_RESULT_SUCCESS != result) {
                        break;
                    }

                    result = checkDataSink(pAudioSnk, &this->mDataSink, SL_OBJECTID_AUDIOPLAYER);
                    if (SL_RESULT_SUCCESS != result) {
                        break;
                    }

                    // It would be unsafe to ever refer to the application pointers again
                    pAudioSrc = NULL;
                    pAudioSnk = NULL;

                    // Check that the requested interfaces are compatible with the data source
                    result = checkSourceFormatVsInterfacesCompatibility(&this->mDataSource,
                            pCAudioPlayer_class, exposedMask);
                    if (SL_RESULT_SUCCESS != result) {
                        break;
                    }

                    // copy the buffer queue count from source locator to the buffer queue interface
                    // we have already range-checked the value down to a smaller width

                    switch (this->mDataSource.mLocator.mLocatorType) {
                    case SL_DATALOCATOR_BUFFERQUEUE:
#ifdef ANDROID
                    case SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE:
#endif
                        this->mBufferQueue.mNumBuffers =
                                (SLuint16) this->mDataSource.mLocator.mBufferQueue.numBuffers;
                        assert(SL_DATAFORMAT_PCM == this->mDataSource.mFormat.mFormatType);
                        this->mNumChannels = this->mDataSource.mFormat.mPCM.numChannels;
                        this->mSampleRateMilliHz = this->mDataSource.mFormat.mPCM.samplesPerSec;
                        break;
                    default:
                        this->mBufferQueue.mNumBuffers = 0;
                        break;
                    }

                    // check the audio source and sink parameters against platform support
#ifdef ANDROID
                    result = android_audioPlayer_checkSourceSink(this);
                    if (SL_RESULT_SUCCESS != result) {
                        break;
                    }
#endif

#ifdef USE_SNDFILE
                    result = SndFile_checkAudioPlayerSourceSink(this);
                    if (SL_RESULT_SUCCESS != result) {
                        break;
                    }
#endif

#ifdef USE_OUTPUTMIXEXT
                    result = IOutputMixExt_checkAudioPlayerSourceSink(this);
                    if (SL_RESULT_SUCCESS != result) {
                        break;
                    }
#endif

                    // FIXME move to dedicated function
                    // Allocate memory for buffer queue

                    //if (0 != this->mBufferQueue.mNumBuffers) {
                        // inline allocation of circular mArray, up to a typical max
                        if (BUFFER_HEADER_TYPICAL >= this->mBufferQueue.mNumBuffers) {
                            this->mBufferQueue.mArray = this->mBufferQueue.mTypical;
                        } else {
                            // Avoid possible integer overflow during multiplication; this arbitrary
                            // maximum is big enough to not interfere with real applications, but
                            // small enough to not overflow.
                            if (this->mBufferQueue.mNumBuffers >= 256) {
                                result = SL_RESULT_MEMORY_FAILURE;
                                break;
                            }
                            this->mBufferQueue.mArray = (BufferHeader *) malloc((this->mBufferQueue.
                                mNumBuffers + 1) * sizeof(BufferHeader));
                            if (NULL == this->mBufferQueue.mArray) {
                                result = SL_RESULT_MEMORY_FAILURE;
                                break;
                            }
                        }
                        this->mBufferQueue.mFront = this->mBufferQueue.mArray;
                        this->mBufferQueue.mRear = this->mBufferQueue.mArray;
                        //}

                        // used to store the data source of our audio player
                        this->mDynamicSource.mDataSource = &this->mDataSource.u.mSource;

                        // platform-specific initialization
#ifdef ANDROID
                        android_audioPlayer_create(this);
#endif

                } while (0);

                if (SL_RESULT_SUCCESS != result) {
                    IObject_Destroy(&this->mObject.mItf);
                } else {
                    IObject_Publish(&this->mObject);
                    // return the new audio player object
                    *pPlayer = &this->mObject.mItf;
                }

            }
        }

    }

    SL_LEAVE_INTERFACE
}


static SLresult IEngine_CreateAudioRecorder(SLEngineItf self, SLObjectItf *pRecorder,
    SLDataSource *pAudioSrc, SLDataSink *pAudioSnk, SLuint32 numInterfaces,
    const SLInterfaceID *pInterfaceIds, const SLboolean *pInterfaceRequired)
{
    SL_ENTER_INTERFACE

#if (USE_PROFILES & USE_PROFILES_OPTIONAL) || defined(ANDROID)
    if (NULL == pRecorder) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pRecorder = NULL;
        unsigned exposedMask;
        const ClassTable *pCAudioRecorder_class = objectIDtoClass(SL_OBJECTID_AUDIORECORDER);
        if (NULL == pCAudioRecorder_class) {
            result = SL_RESULT_FEATURE_UNSUPPORTED;
        } else {
            result = checkInterfaces(pCAudioRecorder_class, numInterfaces,
                    pInterfaceIds, pInterfaceRequired, &exposedMask);
        }

        if (SL_RESULT_SUCCESS == result) {

            // Construct our new AudioRecorder instance
            CAudioRecorder *this = (CAudioRecorder *) construct(pCAudioRecorder_class, exposedMask,
                    self);
            if (NULL == this) {
                result = SL_RESULT_MEMORY_FAILURE;
            } else {

                do {

                    // Initialize fields not associated with any interface

                    // Default data source in case of failure in checkDataSource
                    this->mDataSource.mLocator.mLocatorType = SL_DATALOCATOR_NULL;
                    this->mDataSource.mFormat.mFormatType = SL_DATAFORMAT_NULL;

                    // Default data sink in case of failure in checkDataSink
                    this->mDataSink.mLocator.mLocatorType = SL_DATALOCATOR_NULL;
                    this->mDataSink.mFormat.mFormatType = SL_DATAFORMAT_NULL;

                    // These fields are set to real values by
                    // android_audioRecorder_checkSourceSinkSupport.  Note that the data sink is
                    // always PCM buffer queue, so we know the channel count and sample rate early.
                    this->mNumChannels = 0;
                    this->mSampleRateMilliHz = 0;
#ifdef ANDROID
                    this->mAudioRecord = NULL;
                    this->mRecordSource = android::AUDIO_SOURCE_DEFAULT;
#endif

                    // Check the source and sink parameters, and make a local copy of all parameters
                    result = checkDataSource(pAudioSrc, &this->mDataSource);
                    if (SL_RESULT_SUCCESS != result) {
                        break;
                    }
                    result = checkDataSink(pAudioSnk, &this->mDataSink, SL_OBJECTID_AUDIORECORDER);
                    if (SL_RESULT_SUCCESS != result) {
                        break;
                    }

                    // It would be unsafe to ever refer to the application pointers again
                    pAudioSrc = NULL;
                    pAudioSnk = NULL;

                    // check the audio source and sink parameters against platform support
#ifdef ANDROID
                    result = android_audioRecorder_checkSourceSinkSupport(this);
                    if (SL_RESULT_SUCCESS != result) {
                        SL_LOGE("Cannot create AudioRecorder: invalid source or sink");
                        break;
                    }
#endif

#ifdef ANDROID
                    // Allocate memory for buffer queue
                    SLuint32 locatorType = this->mDataSink.mLocator.mLocatorType;
                    if (locatorType == SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE) {
                        this->mBufferQueue.mNumBuffers =
                            this->mDataSink.mLocator.mBufferQueue.numBuffers;
                        // inline allocation of circular Buffer Queue mArray, up to a typical max
                        if (BUFFER_HEADER_TYPICAL >= this->mBufferQueue.mNumBuffers) {
                            this->mBufferQueue.mArray = this->mBufferQueue.mTypical;
                        } else {
                            // Avoid possible integer overflow during multiplication; this arbitrary
                            // maximum is big enough to not interfere with real applications, but
                            // small enough to not overflow.
                            if (this->mBufferQueue.mNumBuffers >= 256) {
                                result = SL_RESULT_MEMORY_FAILURE;
                                break;
                            }
                            this->mBufferQueue.mArray = (BufferHeader *) malloc((this->mBufferQueue.
                                    mNumBuffers + 1) * sizeof(BufferHeader));
                            if (NULL == this->mBufferQueue.mArray) {
                                result = SL_RESULT_MEMORY_FAILURE;
                                break;
                            }
                        }
                        this->mBufferQueue.mFront = this->mBufferQueue.mArray;
                        this->mBufferQueue.mRear = this->mBufferQueue.mArray;
                    }
#endif

                    // platform-specific initialization
#ifdef ANDROID
                    android_audioRecorder_create(this);
#endif

                } while (0);

                if (SL_RESULT_SUCCESS != result) {
                    IObject_Destroy(&this->mObject.mItf);
                } else {
                    IObject_Publish(&this->mObject);
                    // return the new audio recorder object
                    *pRecorder = &this->mObject.mItf;
                }
            }

        }

    }
#else
    result = SL_RESULT_FEATURE_UNSUPPORTED;
#endif

    SL_LEAVE_INTERFACE
}


static SLresult IEngine_CreateMidiPlayer(SLEngineItf self, SLObjectItf *pPlayer,
    SLDataSource *pMIDISrc, SLDataSource *pBankSrc, SLDataSink *pAudioOutput,
    SLDataSink *pVibra, SLDataSink *pLEDArray, SLuint32 numInterfaces,
    const SLInterfaceID *pInterfaceIds, const SLboolean *pInterfaceRequired)
{
    SL_ENTER_INTERFACE

#if USE_PROFILES & (USE_PROFILES_GAME | USE_PROFILES_PHONE)
    if ((NULL == pPlayer) || (NULL == pMIDISrc) || (NULL == pAudioOutput)) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pPlayer = NULL;
        unsigned exposedMask;
        const ClassTable *pCMidiPlayer_class = objectIDtoClass(SL_OBJECTID_MIDIPLAYER);
        if (NULL == pCMidiPlayer_class) {
            result = SL_RESULT_FEATURE_UNSUPPORTED;
        } else {
            result = checkInterfaces(pCMidiPlayer_class, numInterfaces,
                pInterfaceIds, pInterfaceRequired, &exposedMask);
        }
        if (SL_RESULT_SUCCESS == result) {
            CMidiPlayer *this = (CMidiPlayer *) construct(pCMidiPlayer_class, exposedMask, self);
            if (NULL == this) {
                result = SL_RESULT_MEMORY_FAILURE;
            } else {
                // FIXME a fake value - why not use value from IPlay_init? what does CT check for?
                this->mPlay.mDuration = 0;
                IObject_Publish(&this->mObject);
                // return the new MIDI player object
                *pPlayer = &this->mObject.mItf;
            }
        }
    }
#else
    result = SL_RESULT_FEATURE_UNSUPPORTED;
#endif

    SL_LEAVE_INTERFACE
}


static SLresult IEngine_CreateListener(SLEngineItf self, SLObjectItf *pListener,
    SLuint32 numInterfaces, const SLInterfaceID *pInterfaceIds, const SLboolean *pInterfaceRequired)
{
    SL_ENTER_INTERFACE

#if USE_PROFILES & USE_PROFILES_GAME
    if (NULL == pListener) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pListener = NULL;
        unsigned exposedMask;
        const ClassTable *pCListener_class = objectIDtoClass(SL_OBJECTID_LISTENER);
        if (NULL == pCListener_class) {
            result = SL_RESULT_FEATURE_UNSUPPORTED;
        } else {
            result = checkInterfaces(pCListener_class, numInterfaces,
                pInterfaceIds, pInterfaceRequired, &exposedMask);
        }
        if (SL_RESULT_SUCCESS == result) {
            CListener *this = (CListener *) construct(pCListener_class, exposedMask, self);
            if (NULL == this) {
                result = SL_RESULT_MEMORY_FAILURE;
            } else {
                IObject_Publish(&this->mObject);
                // return the new 3D listener object
                *pListener = &this->mObject.mItf;
            }
        }
    }
#else
    result = SL_RESULT_FEATURE_UNSUPPORTED;
#endif

    SL_LEAVE_INTERFACE
}


static SLresult IEngine_Create3DGroup(SLEngineItf self, SLObjectItf *pGroup, SLuint32 numInterfaces,
    const SLInterfaceID *pInterfaceIds, const SLboolean *pInterfaceRequired)
{
    SL_ENTER_INTERFACE

#if USE_PROFILES & USE_PROFILES_GAME
    if (NULL == pGroup) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pGroup = NULL;
        unsigned exposedMask;
        const ClassTable *pC3DGroup_class = objectIDtoClass(SL_OBJECTID_3DGROUP);
        if (NULL == pC3DGroup_class) {
            result = SL_RESULT_FEATURE_UNSUPPORTED;
        } else {
            result = checkInterfaces(pC3DGroup_class, numInterfaces,
                pInterfaceIds, pInterfaceRequired, &exposedMask);
        }
        if (SL_RESULT_SUCCESS == result) {
            C3DGroup *this = (C3DGroup *) construct(pC3DGroup_class, exposedMask, self);
            if (NULL == this) {
                result = SL_RESULT_MEMORY_FAILURE;
            } else {
                this->mMemberMask = 0;
                IObject_Publish(&this->mObject);
                // return the new 3D group object
                *pGroup = &this->mObject.mItf;
            }
        }
    }
#else
    result = SL_RESULT_FEATURE_UNSUPPORTED;
#endif

    SL_LEAVE_INTERFACE
}


static SLresult IEngine_CreateOutputMix(SLEngineItf self, SLObjectItf *pMix, SLuint32 numInterfaces,
    const SLInterfaceID *pInterfaceIds, const SLboolean *pInterfaceRequired)
{
    SL_ENTER_INTERFACE

    if (NULL == pMix) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pMix = NULL;
        unsigned exposedMask;
        const ClassTable *pCOutputMix_class = objectIDtoClass(SL_OBJECTID_OUTPUTMIX);
        assert(NULL != pCOutputMix_class);
        result = checkInterfaces(pCOutputMix_class, numInterfaces,
            pInterfaceIds, pInterfaceRequired, &exposedMask);
        if (SL_RESULT_SUCCESS == result) {
            COutputMix *this = (COutputMix *) construct(pCOutputMix_class, exposedMask, self);
            if (NULL == this) {
                result = SL_RESULT_MEMORY_FAILURE;
            } else {
#ifdef ANDROID
                android_outputMix_create(this);
#endif
#ifdef USE_SDL
                IEngine *thisEngine = this->mObject.mEngine;
                interface_lock_exclusive(thisEngine);
                bool unpause = false;
                if (NULL == thisEngine->mOutputMix) {
                    thisEngine->mOutputMix = this;
                    unpause = true;
                }
                interface_unlock_exclusive(thisEngine);
#endif
                IObject_Publish(&this->mObject);
#ifdef USE_SDL
                if (unpause) {
                    // Enable SDL_callback to be called periodically by SDL's internal thread
                    SDL_PauseAudio(0);
                }
#endif
                // return the new output mix object
                *pMix = &this->mObject.mItf;
            }
        }
    }

    SL_LEAVE_INTERFACE
}


static SLresult IEngine_CreateMetadataExtractor(SLEngineItf self, SLObjectItf *pMetadataExtractor,
    SLDataSource *pDataSource, SLuint32 numInterfaces, const SLInterfaceID *pInterfaceIds,
    const SLboolean *pInterfaceRequired)
{
    SL_ENTER_INTERFACE

#if USE_PROFILES & (USE_PROFILES_GAME | USE_PROFILES_MUSIC)
    if (NULL == pMetadataExtractor) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pMetadataExtractor = NULL;
        unsigned exposedMask;
        const ClassTable *pCMetadataExtractor_class =
            objectIDtoClass(SL_OBJECTID_METADATAEXTRACTOR);
        if (NULL == pCMetadataExtractor_class) {
            result = SL_RESULT_FEATURE_UNSUPPORTED;
        } else {
            result = checkInterfaces(pCMetadataExtractor_class, numInterfaces,
                pInterfaceIds, pInterfaceRequired, &exposedMask);
        }
        if (SL_RESULT_SUCCESS == result) {
            CMetadataExtractor *this = (CMetadataExtractor *)
                construct(pCMetadataExtractor_class, exposedMask, self);
            if (NULL == this) {
                result = SL_RESULT_MEMORY_FAILURE;
            } else {
                IObject_Publish(&this->mObject);
                // return the new metadata extractor object
                *pMetadataExtractor = &this->mObject.mItf;
                result = SL_RESULT_SUCCESS;
            }
        }
    }
#else
    result = SL_RESULT_FEATURE_UNSUPPORTED;
#endif

    SL_LEAVE_INTERFACE
}


static SLresult IEngine_CreateExtensionObject(SLEngineItf self, SLObjectItf *pObject,
    void *pParameters, SLuint32 objectID, SLuint32 numInterfaces,
    const SLInterfaceID *pInterfaceIds, const SLboolean *pInterfaceRequired)
{
    SL_ENTER_INTERFACE

    if (NULL == pObject) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pObject = NULL;
        result = SL_RESULT_FEATURE_UNSUPPORTED;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IEngine_QueryNumSupportedInterfaces(SLEngineItf self,
    SLuint32 objectID, SLuint32 *pNumSupportedInterfaces)
{
    SL_ENTER_INTERFACE

    if (NULL == pNumSupportedInterfaces) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        const ClassTable *class__ = objectIDtoClass(objectID);
        if (NULL == class__) {
            result = SL_RESULT_FEATURE_UNSUPPORTED;
        } else {
            SLuint32 count = 0;
            SLuint32 i;
            for (i = 0; i < class__->mInterfaceCount; ++i) {
                switch (class__->mInterfaces[i].mInterface) {
                case INTERFACE_IMPLICIT:
                case INTERFACE_IMPLICIT_PREREALIZE:
                case INTERFACE_EXPLICIT:
                case INTERFACE_EXPLICIT_PREREALIZE:
                case INTERFACE_DYNAMIC:
                    ++count;
                    break;
                case INTERFACE_UNAVAILABLE:
                    break;
                default:
                    assert(false);
                    break;
                }
            }
            *pNumSupportedInterfaces = count;
            result = SL_RESULT_SUCCESS;
        }
    }

    SL_LEAVE_INTERFACE;
}


static SLresult IEngine_QuerySupportedInterfaces(SLEngineItf self,
    SLuint32 objectID, SLuint32 index, SLInterfaceID *pInterfaceId)
{
    SL_ENTER_INTERFACE

    if (NULL == pInterfaceId) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pInterfaceId = NULL;
        const ClassTable *class__ = objectIDtoClass(objectID);
        if (NULL == class__) {
            result = SL_RESULT_FEATURE_UNSUPPORTED;
        } else {
            result = SL_RESULT_PARAMETER_INVALID; // will be reset later
            SLuint32 i;
            for (i = 0; i < class__->mInterfaceCount; ++i) {
                switch (class__->mInterfaces[i].mInterface) {
                case INTERFACE_IMPLICIT:
                case INTERFACE_IMPLICIT_PREREALIZE:
                case INTERFACE_EXPLICIT:
                case INTERFACE_EXPLICIT_PREREALIZE:
                case INTERFACE_DYNAMIC:
                    break;
                case INTERFACE_UNAVAILABLE:
                    continue;
                default:
                    assert(false);
                    break;
                }
                if (index == 0) {
                    *pInterfaceId = &SL_IID_array[class__->mInterfaces[i].mMPH];
                    result = SL_RESULT_SUCCESS;
                    break;
                }
                --index;
            }
        }
    }

    SL_LEAVE_INTERFACE
};


static const char * const extensionNames[] = {
#ifdef ANDROID
    "ANDROID_SDK_LEVEL_9",  // Android 2.3 aka "Gingerbread"
    // in the future, add more entries for each SDK level here, and
    // don't delete the entries for previous SDK levels unless support is removed
#else
    "WILHELM_DESKTOP",
#endif
};


static SLresult IEngine_QueryNumSupportedExtensions(SLEngineItf self, SLuint32 *pNumExtensions)
{
    SL_ENTER_INTERFACE

    if (NULL == pNumExtensions) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pNumExtensions = sizeof(extensionNames) / sizeof(extensionNames[0]);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IEngine_QuerySupportedExtension(SLEngineItf self,
    SLuint32 index, SLchar *pExtensionName, SLint16 *pNameLength)
{
    SL_ENTER_INTERFACE

    if (NULL == pNameLength) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        size_t actualNameLength;
        unsigned numExtensions = sizeof(extensionNames) / sizeof(extensionNames[0]);
        if (index >= numExtensions) {
            actualNameLength = 0;
            result = SL_RESULT_PARAMETER_INVALID;
        } else {
            const char *extensionName = extensionNames[index];
            actualNameLength = strlen(extensionName) + 1;
            if (NULL == pExtensionName) {
                // application is querying the name length in order to allocate a buffer
                result = SL_RESULT_SUCCESS;
            } else {
                SLint16 availableNameLength = *pNameLength;
                if (0 >= availableNameLength) {
                    // there is not even room for the terminating NUL
                    result = SL_RESULT_BUFFER_INSUFFICIENT;
                } else if (actualNameLength > (size_t) availableNameLength) {
                    // "no invalid strings are written. That is, the null-terminator always exists"
                    memcpy(pExtensionName, extensionName, (size_t) availableNameLength - 1);
                    pExtensionName[(size_t) availableNameLength - 1] = '\0';
                    result = SL_RESULT_BUFFER_INSUFFICIENT;
                } else {
                    memcpy(pExtensionName, extensionName, actualNameLength);
                    result = SL_RESULT_SUCCESS;
                }
            }
        }
        *pNameLength = actualNameLength;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IEngine_IsExtensionSupported(SLEngineItf self,
    const SLchar *pExtensionName, SLboolean *pSupported)
{
    SL_ENTER_INTERFACE

    if (NULL == pSupported) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        SLboolean isSupported = SL_BOOLEAN_FALSE;
        if (NULL == pExtensionName) {
            result = SL_RESULT_PARAMETER_INVALID;
        } else {
            unsigned numExtensions = sizeof(extensionNames) / sizeof(extensionNames[0]);
            unsigned i;
            for (i = 0; i < numExtensions; ++i) {
                if (!strcmp((const char *) pExtensionName, extensionNames[i])) {
                    isSupported = SL_BOOLEAN_TRUE;
                    break;
                }
            }
            result = SL_RESULT_SUCCESS;
        }
        *pSupported = isSupported;
    }

    SL_LEAVE_INTERFACE
}


static const struct SLEngineItf_ IEngine_Itf = {
    IEngine_CreateLEDDevice,
    IEngine_CreateVibraDevice,
    IEngine_CreateAudioPlayer,
    IEngine_CreateAudioRecorder,
    IEngine_CreateMidiPlayer,
    IEngine_CreateListener,
    IEngine_Create3DGroup,
    IEngine_CreateOutputMix,
    IEngine_CreateMetadataExtractor,
    IEngine_CreateExtensionObject,
    IEngine_QueryNumSupportedInterfaces,
    IEngine_QuerySupportedInterfaces,
    IEngine_QueryNumSupportedExtensions,
    IEngine_QuerySupportedExtension,
    IEngine_IsExtensionSupported
};

void IEngine_init(void *self)
{
    IEngine *this = (IEngine *) self;
    this->mItf = &IEngine_Itf;
    // mLossOfControlGlobal is initialized in slCreateEngine
#ifdef USE_SDL
    this->mOutputMix = NULL;
#endif
    this->mInstanceCount = 1; // ourself
    this->mInstanceMask = 0;
    this->mChangedMask = 0;
    unsigned i;
    for (i = 0; i < MAX_INSTANCE; ++i) {
        this->mInstances[i] = NULL;
    }
    this->mShutdown = SL_BOOLEAN_FALSE;
    this->mShutdownAck = SL_BOOLEAN_FALSE;
    // mThreadPool is initialized in CEngine_Realize
    memset(&this->mThreadPool, 0, sizeof(ThreadPool));
#if defined(ANDROID) && !defined(USE_BACKPORT)
    this->mEqNumPresets = 0;
    this->mEqPresetNames = NULL;
#endif
}

void IEngine_deinit(void *self)
{
#if defined(ANDROID) && !defined(USE_BACKPORT)
    IEngine *this = (IEngine *) self;
    // free equalizer preset names
    if (NULL != this->mEqPresetNames) {
        for (unsigned i = 0; i < this->mEqNumPresets; ++i) {
            if (NULL != this->mEqPresetNames[i]) {
                delete[] this->mEqPresetNames[i];
                this->mEqPresetNames[i] = NULL;
            }
        }
        delete[] this->mEqPresetNames;
        this->mEqPresetNames = NULL;
    }
    this->mEqNumPresets = 0;
#endif
}
