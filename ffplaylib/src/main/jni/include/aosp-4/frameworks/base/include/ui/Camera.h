/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (C) 2008 HTC Inc.
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

#ifndef ANDROID_HARDWARE_CAMERA_H
#define ANDROID_HARDWARE_CAMERA_H

#include <utils/Timers.h>
#include <ui/ICameraClient.h>

namespace android {

/*
 * A set of bit masks for specifying how the received preview frames are
 * handled before the previewCallback() call.
 *
 * The least significant 3 bits of an "int" value are used for this purpose:
 *
 * ..... 0 0 0
 *       ^ ^ ^
 *       | | |---------> determine whether the callback is enabled or not
 *       | |-----------> determine whether the callback is one-shot or not
 *       |-------------> determine whether the frame is copied out or not
 *
 * WARNING:
 * When a frame is sent directly without copying, it is the frame receiver's
 * responsiblity to make sure that the frame data won't get corrupted by
 * subsequent preview frames filled by the camera. This flag is recommended
 * only when copying out data brings significant performance price and the
 * handling/processing of the received frame data is always faster than
 * the preview frame rate so that data corruption won't occur.
 *
 * For instance,
 * 1. 0x00 disables the callback. In this case, copy out and one shot bits
 *    are ignored.
 * 2. 0x01 enables a callback without copying out the received frames. A
 *    typical use case is the Camcorder application to avoid making costly
 *    frame copies.
 * 3. 0x05 is enabling a callback with frame copied out repeatedly. A typical
 *    use case is the Camera application.
 * 4. 0x07 is enabling a callback with frame copied out only once. A typical use
 *    case is the Barcode scanner application.
 */
#define FRAME_CALLBACK_FLAG_ENABLE_MASK              0x01
#define FRAME_CALLBACK_FLAG_ONE_SHOT_MASK            0x02
#define FRAME_CALLBACK_FLAG_COPY_OUT_MASK            0x04

// Typical use cases
#define FRAME_CALLBACK_FLAG_NOOP                     0x00
#define FRAME_CALLBACK_FLAG_CAMCORDER                0x01
#define FRAME_CALLBACK_FLAG_CAMERA                   0x05
#define FRAME_CALLBACK_FLAG_BARCODE_SCANNER          0x07

// msgType in notifyCallback and dataCallback functions
enum {
    CAMERA_MSG_ERROR = 0,
    CAMERA_MSG_SHUTTER,
    CAMERA_MSG_FOCUS,
    CAMERA_MSG_ZOOM,
    CAMERA_MSG_PREVIEW_FRAME,
    CAMERA_MSG_VIDEO_FRAME,
    CAMERA_MSG_POSTVIEW_FRAME,
    CAMERA_MSG_RAW_IMAGE,
    CAMERA_MSG_COMPRESSED_IMAGE
};

// camera fatal errors
enum {
    CAMERA_ERROR_UKNOWN  = 1,
    CAMERA_ERROR_SERVER_DIED = 100
};

class ICameraService;
class ICamera;
class Surface;
class Mutex;
class String8;

// ref-counted object for callbacks
class CameraListener: virtual public RefBase
{
public:
    virtual void notify(int32_t msgType, int32_t ext1, int32_t ext2) = 0;
    virtual void postData(int32_t msgType, const sp<IMemory>& dataPtr) = 0;
    virtual void postDataTimestamp(nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr) = 0;
};

class Camera : public BnCameraClient, public IBinder::DeathRecipient
{
public:
            // construct a camera client from an existing remote
    static  sp<Camera>  create(const sp<ICamera>& camera);
    static  sp<Camera>  connect();
                        ~Camera();
            void        init();

            status_t    reconnect();
            void        disconnect();
            status_t    lock();
            status_t    unlock();

            status_t    getStatus() { return mStatus; }

            // pass the buffered ISurface to the camera service
            status_t    setPreviewDisplay(const sp<Surface>& surface);
            status_t    setPreviewDisplay(const sp<ISurface>& surface);

            // start preview mode, must call setPreviewDisplay first
            status_t    startPreview();

            // stop preview mode
            void        stopPreview();

            // get preview state
            bool        previewEnabled();

            // start recording mode, must call setPreviewDisplay first
            status_t    startRecording();

            // stop recording mode
            void        stopRecording();

            // get recording state
            bool        recordingEnabled();

            // release a recording frame
            void        releaseRecordingFrame(const sp<IMemory>& mem);

            // autoFocus - status returned from callback
            status_t    autoFocus();

            // take a picture - picture returned from callback
            status_t    takePicture();

            // set preview/capture parameters - key/value pairs
            status_t    setParameters(const String8& params);

            // get preview/capture parameters - key/value pairs
            String8     getParameters() const;

            void        setListener(const sp<CameraListener>& listener);
            void        setPreviewCallbackFlags(int preview_callback_flag);

    // ICameraClient interface
    virtual void        notifyCallback(int32_t msgType, int32_t ext, int32_t ext2);
    virtual void        dataCallback(int32_t msgType, const sp<IMemory>& dataPtr);
    virtual void        dataCallbackTimestamp(nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr);

    sp<ICamera>         remote();

private:
                        Camera();
                        Camera(const Camera&);
                        Camera& operator=(const Camera);
                        virtual void binderDied(const wp<IBinder>& who);

            class DeathNotifier: public IBinder::DeathRecipient
            {
            public:
                DeathNotifier() {
                }

                virtual void binderDied(const wp<IBinder>& who);
            };

            static sp<DeathNotifier> mDeathNotifier;

            // helper function to obtain camera service handle
            static const sp<ICameraService>& getCameraService();

            sp<ICamera>         mCamera;
            status_t            mStatus;

            sp<CameraListener>  mListener;

            friend class DeathNotifier;

            static  Mutex               mLock;
            static  sp<ICameraService>  mCameraService;

};

}; // namespace android

#endif

