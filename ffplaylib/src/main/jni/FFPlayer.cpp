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

#include "base/NativeException.h"
#include "base/log.h"
extern "C" {
#include "include/libavformat/avformat.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/avutil.h"
#include "SDL/src/core/android/SDL_android.h"
}


#define LOG_TAG "FFPlayer"
#define MAIN_CLASS "com/baidu/music/ffplaylib/jni/LivePlayer"
static jclass mainClass;

AVFormatContext *pFormatCtx;
AVCodecContext *pCodecCtx;
AVCodec *pCodec;
AVFrame *pFrame;
AVFrame *pFrameRGB;
AVPacket packet;
struct SwsContext *pSwsCtx;

int disPlayerType;
jclass surfaceClass;

jint nativeSetDisplayType(JNIEnv* env, jclass clazz, jint type) {
    disPlayerType = type;
}

jint nativeSetVideoSurface(JNIEnv* env, jclass clazz, jobject surface) {
    surfaceClass = (jclass)env->NewGlobalRef(surface);
    if (surfaceClass == NULL) {
        return -1;
    } else {
        return 0;
    }
}

jint nativeSetDataSource(JNIEnv* env, jclass clazz, jstring path, jint type) {
    if (path == NULL) {
        NativeException::throwNullPointerException(env, "the video path is null");
        return -1;
    }
    jboolean iscopy = JNI_FALSE;
    const char *pathStr = env->GetStringUTFChars(path, &iscopy);
    if (pathStr == NULL) {
        NativeException::throwRuntimeException(env, "Out of memory");
        return -1;
    }
    int i, videoStream;
    int frameFinished = 0;

    av_register_all();
    pFormatCtx=avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, pathStr, NULL, NULL) != 0) {
        return -1;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        return -1;
    }
    av_dump_format(pFormatCtx, 0, pathStr, 0);
    videoStream = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    if (videoStream == -1) {
        return -1;
    }

    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0) {
        return -1;
    }

    pFrame = avcodec_alloc_frame();
    i=0;
    while(av_read_frame(pFormatCtx, &packet)) {
        if(packet.stream_index==videoStream) {
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
            if (frameFinished) {
                ++i;
            }

        }
    }


    av_free(pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

//    pFrameRGB = avcodec_alloc_frame();
//    if (pFrameRGB == NULL) {
//        return -1;
//    }
//    numBytes = avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
//                                  pCodecCtx->height);
//    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
//    avpicture_fill((AVPicture *) pFrameRGB, buffer, PIX_FMT_RGB24,
//                   pCodecCtx->width, pCodecCtx->height);
    delete pathStr;

}

jint nativeStart(JNIEnv* env, jclass clazz) {

}

jint nativePause(JNIEnv* env, jclass clazz) {

}

jint nativeStop(JNIEnv* env, jclass clazz) {

}

jint nativeSeek(JNIEnv* env, jclass clazz, jlong msec) {

}

jint nativeRelease(JNIEnv* env, jclass clazz) {

}


jint nativeReset(JNIEnv* env, jclass clazz) {

}


jint nativeSetVolume(JNIEnv* env, jclass clazz, jlong left, jlong right) {

}

jlong nativeGetDuration(JNIEnv* env, jclass clazz, jlong left, jlong right) {

}

jint nativeGetVideoWidth(JNIEnv* env, jclass clazz) {

}

jint nativeGetVideoHeight(JNIEnv* env, jclass clazz) {

}

jboolean isPlaying(JNIEnv* env, jclass clazz) {

}

jlong getCurrentPosition(JNIEnv* env, jclass clazz) {

}

jboolean isSeekable(JNIEnv* env, jclass clazz) {

}

jboolean isCanPause(JNIEnv* env, jclass clazz) {

}


jint getPlayMode(JNIEnv* env, jclass clazz) {

}

/*
 * native方法映射
 */
static JNINativeMethod nativeMethods[] = {
        { "nativeSetDisplayType",  "(I)I",                          (void*) nativeSetDisplayType},
        { "nativeSetVideoSurface", "(Landroid/view/Surface)I",      (void*) nativeSetVideoSurface},
        { "nativeSetDataSource",   "(Ljava/lang/String;)I",         (void*) nativeSetDataSource},
        { "nativeStart",           "(V)I",                          (void*) nativeStart},
        { "nativePause",           "(V)I",                          (void*) nativePause},
        { "nativeStop",            "(V)I",                          (void*) nativeStop},
        { "nativeSeek",            "(J)I",                          (void*) nativeSeek},
        { "nativeRelease",         "(V)I",                          (void*) nativeRelease},
        { "nativeReset",           "(V)I",                          (void*) nativeReset},
        { "nativeSetVolume",       "(FF)I",                         (void*) nativeSetVolume},
        { "nativeGetDuration",     "(V)I",                          (void*) nativeGetDuration},
        { "nativeGetVideoWidth",   "(V)I",                          (void*) nativeGetVideoWidth},
        { "nativeGetVideoHeight",  "(V)I",                          (void*) nativeGetVideoHeight},
        { "isPlaying",             "(V)Z",                          (void*) isPlaying},
        { "getCurrentPosition",    "(V)I",                          (void*) getCurrentPosition},
        { "isSeekable",            "(V)Z",                          (void*) isSeekable},
        { "isCanPause",            "(V)Z",                          (void*) isCanPause},
        { "getPlayMode",           "(V)I",                          (void*) getPlayMode}
};


static int registerNativeMethods(JNIEnv* env, const char* className,
                                 JNINativeMethod* gMethods, int numMethods) {
    jclass clazz= env->FindClass(className);
    mainClass = (jclass) env->NewGlobalRef(clazz);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }

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

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    assert(env != NULL);
    if (!registerNatives(env)) { //注册
        return -1;
    }
    /* success -- return valid version number */
    result = JNI_VERSION_1_6;
    JniOnload(vm, reserved);
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
