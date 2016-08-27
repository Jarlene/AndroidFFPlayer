//
// Created by Jarlene on 16/6/8.
//


#include <jni.h>
#include <stddef.h>
#include <assert.h>

#include "base/NativeException.h"
#include "base/log.h"
extern "C"
{
#include "include/libavformat/avformat.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/avutil.h"
}


#define LOG_TAG "FFPlayer"
#define MAIN_CLASS "com/baidu/music/ffplaylib/jni/LivePlayer"
static jclass mainClass;


void nativeInit(JNIEnv* env, jclass clazz) {
#ifdef PROFILER
    #warning "Profiler enabled"
	setenv("CPUPROFILE_FREQUENCY", "1000", 1);
	monstartup("libffmpeg.so");
#endif
}

void nativeSetup(JNIEnv* env, jclass clazz, jobject obj) {

}

void nativeFinalize(JNIEnv* env, jclass clazz) {

}

jint nativeSetLogLevel(JNIEnv* env, jclass clazz, jint level) {

}

jint nativeSetDisplayType(JNIEnv* env, jclass clazz, jint type) {

}

jint nativeSetVideoSurface(JNIEnv* env, jclass clazz) {

}

jint nativeSetDataSource(JNIEnv* env, jclass clazz, jstring path, jint type) {
    if (path == NULL) {
        NativeException::throwNullPointerException(env, "the video path is null");
        return -1;
    }
    jboolean iscopy;
    const char *pathStr = env->GetStringUTFChars(path, &iscopy);
    if (pathStr == NULL) {
        NativeException::throwRuntimeException(env, "Out of memory");
        return -1;
    }
    AVFormatContext* pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame;
    AVFrame *pFrameRGB;
    AVPacket packet;
    int i, videoStream;
    int frameFinished = 0;
    int numBytes;
    uint8_t *buffer;
    struct SwsContext *pSwsCtx;

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
    pFrameRGB = avcodec_alloc_frame();
    if (pFrameRGB == NULL) {
        return -1;
    }
    numBytes = avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
                                  pCodecCtx->height);
    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, buffer, PIX_FMT_RGB24,
                   pCodecCtx->width, pCodecCtx->height);
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


jint updateTShiftInfo(JNIEnv* env, jclass clazz) {

}

jboolean isTShiftRun(JNIEnv* env, jclass clazz) {

}

jint getPlayMode(JNIEnv* env, jclass clazz) {

}

/*
 * native方法映射
 */
static JNINativeMethod nativeMethods[] = {
        { "nativeInit",            "()V",                           (void*) nativeInit },
        { "nativeSetup",           "(Ljava/lang/Object)V",          (void*) nativeSetup },
        { "nativeFinalize",        "()V",                           (void*) nativeFinalize },
        { "nativeSetLogLevel",     "(I)I",                          (void*) nativeSetLogLevel},
        { "nativeSetDisplayType",  "(I)I",                          (void*) nativeSetDisplayType},
        { "nativeSetVideoSurface", "(V)I",                          (void*) nativeSetVideoSurface},
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
        { "updateTShiftInfo",      "(V)I",                          (void*) updateTShiftInfo},
        { "isTShiftRun",           "(V)Z",                          (void*) isTShiftRun},
        { "getPlayMode",           "(V)I",                          (void*) getPlayMode},

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
