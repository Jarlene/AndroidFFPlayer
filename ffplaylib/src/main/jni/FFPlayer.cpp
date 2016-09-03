//
// Created by Jarlene on 16/6/8.
//
//    encode \\\
//    av_register_all();
//    avformat_alloc_output_context2();
//    avio_open2();
//    avformat_new_stream();
//    avcodec_find_encoder();
//    avcodec_open2();
//    avformat_write_header();
//    avcodec_encode_video2();
//    av_write_frame();
//    av_write_trailer();
//    avcodec_close();
//    avformat_free_context();
//    avio_close();
//
//    decode \\\\
//    av_register_all();
//    avformat_alloc_context();
//    avformat_open_input();
//    avformat_find_stream_info();
//    avcodec_find_decoder();
//    avcodec_open2();
//    av_read_frame();
//    avcodec_decode_video2();
//    avcodec_close();
//    avformat_close_input();

#include <jni.h>
#include <stddef.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "base/NativeException.h"
#include "base/log.h"
extern "C" {
#include "include/libswscale/swscale.h"
#include "include/libavutil/dict.h"
#include "include/libavformat/avformat.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/avutil.h"
#include "include/libavutil/imgutils.h"
}


#define LOG_TAG "FFPlayer"
#define MAIN_CLASS "com/baidu/music/ffplaylib/jni/LivePlayer"
static jclass mainClass;

AVFormatContext *pFormatCtx;
AVCodecContext *pVideoCodecCtx;
AVCodecContext *pAudioCodecCtx;
AVCodec *pVideoCodec;
AVCodec *pAudioCodec;
AVFrame *pFrame;
AVFrame *pFrameRGB;
AVDictionary *dictionary;
AVPacket packet;
struct SwsContext *pSwsCtx;
uint8_t *buffer;
int width, height, videoStream, audioStream;
const char *pathStr;

ANativeWindow* nativeWindow;
ANativeWindow_Buffer windowBuffer;

jboolean isPause = JNI_TRUE;

jint nativeSetDisplayType(JNIEnv *env, jclass clazz, jint type) {
}

jint nativeSetVideoSurface(JNIEnv *env, jclass clazz, jobject surface) {
    LOGD("nativeSetVideoSurface");
    if (surface == NULL) {
        ANativeWindow_release(nativeWindow);
    } else {
        nativeWindow = ANativeWindow_fromSurface(env, surface);
        // set format and size of window buffer
        ANativeWindow_setBuffersGeometry(nativeWindow, 640, 480, WINDOW_FORMAT_RGBA_8888);
    }
}

jint nativeSetDataSource(JNIEnv *env, jclass clazz, jstring path) {
    if (path == NULL) {
        NativeException::throwNullPointerException(env, "the video path is null");
        return -1;
    }
    jboolean iscopy = JNI_FALSE;
    pathStr = (char *) env->GetStringUTFChars(path, &iscopy);
    if (pathStr == NULL) {
        NativeException::throwRuntimeException(env, "Out of memory");
        return -1;
    }
    LOGD("the path is %s", pathStr);
    return 0;
}

void finish() {
    av_free(pFrame);
    av_free(buffer);
    av_free(pFrameRGB);
    avcodec_close(pVideoCodecCtx);
    avformat_close_input(&pFormatCtx);
}

void  decodeAndRender() {
    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    if (avformat_open_input(&pFormatCtx, pathStr, NULL, NULL) != 0) {
        LOGE("Couldn't open file: %s\n", pathStr);
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, &dictionary) < 0) {
        LOGE("Couldn't find stream information.");
        return;
    }
    av_dump_format(pFormatCtx, 0, pathStr, 0);
    videoStream = -1;
    audioStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
        }
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = i;
        }
    }

    if (videoStream == -1) {
        LOGE("Didn't find a video stream.");
        return;
    }

    pVideoCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pAudioCodecCtx = pFormatCtx->streams[audioStream]->codec;
    pVideoCodec = avcodec_find_decoder(pVideoCodecCtx->codec_id);
    pAudioCodec = avcodec_find_encoder(pAudioCodecCtx->codec_id);
    if (pVideoCodec == NULL) {
        LOGE("Codec not found.");
        return;
    }
    if (avcodec_open2(pVideoCodecCtx, pVideoCodec, NULL) < 0) {
        LOGE("Could not open codec.");
        return;
    }
    width = pVideoCodecCtx->width;
    height = pVideoCodecCtx->height;

    ANativeWindow_setBuffersGeometry(nativeWindow,  width, height, WINDOW_FORMAT_RGBA_8888);

    LOGD("the width is %d, the height is %d", width, height);
    pFrame = avcodec_alloc_frame();
    pFrameRGB = avcodec_alloc_frame();
    if (pFrameRGB == NULL || pFrameRGB == NULL) {
        LOGE("Could not allocate video frame.");
        return;
    }
    int numBytes = avpicture_get_size(PIX_FMT_RGBA, pVideoCodecCtx->width,
                                      pVideoCodecCtx->height);
    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    if (buffer == NULL) {
        LOGE("buffer is null");
        return;
    }

    avpicture_fill((AVPicture *) pFrameRGB, buffer, PIX_FMT_RGBA,
                   pVideoCodecCtx->width, pVideoCodecCtx->height);

    pSwsCtx = sws_getContext(pVideoCodecCtx->width,
                             pVideoCodecCtx->height,
                             pVideoCodecCtx->pix_fmt,
                             width,
                             height,
                             PIX_FMT_RGBA,
                             SWS_BILINEAR,
                             NULL,
                             NULL,
                             NULL);

    int frameFinished = 0;
    while (!isPause && av_read_frame(pFormatCtx, &packet) >= 0) {
        if (packet.stream_index == videoStream) {
            avcodec_decode_video2(pVideoCodecCtx, pFrame, &frameFinished, &packet);
            if (frameFinished) {
                LOGD("av_read_frame");
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
                sws_scale(pSwsCtx, (uint8_t const * const *)pFrame->data,
                          pFrame->linesize, 0, pVideoCodecCtx->height,
                          pFrameRGB->data, pFrameRGB->linesize);
                uint8_t * dst = (uint8_t *)windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t * src =  (pFrameRGB->data[0]);
                int srcStride = pFrameRGB->linesize[0];
                for (int h = 0; h < height; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }
                ANativeWindow_unlockAndPost(nativeWindow);
            }

        }
        av_free_packet(&packet);
    }
    finish();
}


jint nativeStart(JNIEnv *env, jclass clazz) {
    LOGD("native start");
    if (isPause) {
        pthread_t decodeThread;
        if (0 != pthread_create(&decodeThread, NULL, (void *(*)(void *)) (void *)decodeAndRender, NULL)) {
            LOGD("yyyy");
        }
        isPause = JNI_FALSE;
    }
}

jint nativePause(JNIEnv *env, jclass clazz) {
    isPause = JNI_TRUE;
}

jint nativeStop(JNIEnv *env, jclass clazz) {
    isPause = JNI_TRUE;
    finish();
}

jint nativeSeek(JNIEnv *env, jclass clazz, jlong msec) {

}

jint nativeRelease(JNIEnv *env, jclass clazz) {

}


jint nativeReset(JNIEnv *env, jclass clazz) {
    finish();
    isPause = JNI_TRUE;
}


jint nativeSetVolume(JNIEnv *env, jclass clazz, jlong left, jlong right) {

}

jlong nativeGetDuration(JNIEnv *env, jclass clazz) {

}

jint nativeGetVideoWidth(JNIEnv *env, jclass clazz) {
    return width;
}

jint nativeGetVideoHeight(JNIEnv *env, jclass clazz) {
    return height;
}

jboolean isPlaying(JNIEnv *env, jclass clazz) {
    return !isPause;
}

jlong getCurrentPosition(JNIEnv *env, jclass clazz) {
}

jboolean isSeekable(JNIEnv *env, jclass clazz) {

}

jboolean isCanPause(JNIEnv *env, jclass clazz) {

}


jint getPlayMode(JNIEnv *env, jclass clazz) {

}

/*
 * native方法映射
 */
static JNINativeMethod nativeMethods[] = {
        {"nativeSetDisplayType",  "(I)I",                     (void *) nativeSetDisplayType},
        {"nativeSetVideoSurface", "(Landroid/view/Surface;)I", (void *) nativeSetVideoSurface},
        {"nativeSetDataSource",   "(Ljava/lang/String;)I",     (void *) nativeSetDataSource},
        {"nativeStart",           "()I",                     (void *) nativeStart},
        {"nativePause",           "()I",                     (void *) nativePause},
        {"nativeStop",            "()I",                     (void *) nativeStop},
        {"nativeSeek",            "(J)I",                     (void *) nativeSeek},
        {"nativeRelease",         "()I",                     (void *) nativeRelease},
        {"nativeReset",           "()I",                     (void *) nativeReset},
        {"nativeSetVolume",       "(FF)I",                    (void *) nativeSetVolume},
        {"nativeGetDuration",     "()I",                     (void *) nativeGetDuration},
        {"nativeGetVideoWidth",   "()I",                     (void *) nativeGetVideoWidth},
        {"nativeGetVideoHeight",  "()I",                     (void *) nativeGetVideoHeight},
        {"isPlaying",             "()Z",                     (void *) isPlaying},
        {"getCurrentPosition",    "()I",                     (void *) getCurrentPosition},
        {"isSeekable",            "()Z",                     (void *) isSeekable},
        {"isCanPause",            "()Z",                     (void *) isCanPause},
        {"getPlayMode",           "()I",                     (void *) getPlayMode}
};


static int registerNativeMethods(JNIEnv *env, const char *className,
                                 JNINativeMethod *gMethods, int numMethods) {
    jclass clazz = env->FindClass(className);
    mainClass = (jclass) env->NewGlobalRef(clazz);
    if (mainClass == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(mainClass, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    LOGD("registerNativeMethods");
    return JNI_TRUE;
}


/**
 * 注册native方法
 */
static int registerNatives(JNIEnv* env) {
    if (!registerNativeMethods(env, MAIN_CLASS, nativeMethods,
                               sizeof(nativeMethods) / sizeof(nativeMethods[0])))
        return JNI_FALSE;

    return JNI_TRUE;
}

/*
 * Set some test stuff up.
 *
 * Returns the JNI version on success, -1 on failure.
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = -1;
    LOGD(LOG_TAG, "JNI_OnLoad");
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    assert(env != NULL);
    if (!registerNatives(env)) { //注册
        return -1;
    }
    /* success -- return valid version number */
    result = JNI_VERSION_1_6;
    return result;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
    LOGI("JNI_OnUnload");
    JNIEnv *env;
    int nJNIVersionOK = vm->GetEnv((void **)&env, JNI_VERSION_1_6) ;
    if (nJNIVersionOK == JNI_OK) {
        if(mainClass) {
            env->DeleteGlobalRef(mainClass);
        }
    }
}
