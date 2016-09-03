package com.baidu.music.ffplaylib.jni;

import android.content.Context;
import android.net.Uri;
import android.os.PowerManager;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;

public class LivePlayer {

    private final static String TAG = "LivePlayer";

    static {
        try {
            System.loadLibrary("swscale-2");
            System.loadLibrary("avutil-51");
            System.loadLibrary("avfilter-3");
            System.loadLibrary("avformat-54");
            System.loadLibrary("swresample-0");
            System.loadLibrary("avdevice-54");
            System.loadLibrary("avcodec-54");
            System.loadLibrary("FFPlayer");
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    public static final int DType_Auto_Scale = 0;
    public static final int DType_4_3_Scale = 1;
    public static final int DType_16_9_Scale = 2;

    public static final int MEDIA_PLAY_NOP = 0;
    public static final int MEDIA_PLAY_LOCAL = 1;
    public static final int MEDIA_PLAY_LIVE = 2;
    public static final int MEDIA_PLAY_TSHIFT = 3;
    public static final int MEDIA_PLAY_ONDEMAND = 4;

    private Surface mSurface; // accessed by native methods
    private SurfaceHolder mSurfaceHolder;
    private PowerManager.WakeLock mWakeLock = null;
    private boolean mScreenOnWhilePlaying;
    private boolean mStayAwake;

    public LivePlayer() {

    }

    public int setDisplayType(int DType) {
        if (DType >= DType_Auto_Scale && DType <= DType_16_9_Scale) {
            return setDisplayType(DType);
        }
        return -1;
    }

    public void setDisplay(SurfaceHolder sh) {
        if (sh == null) {
            throw new IllegalArgumentException("the param is null");
        }
        mSurfaceHolder = sh;
        mSurface = sh.getSurface();
        int ret = nativeSetVideoSurface(mSurface);
        if (ret < 0) {
            Log.e(TAG, "setVideoSurface fail!");
        }
        updateSurfaceScreenOn();
    }

    /**
     * Sets the data source as a content Uri.
     *
     * @param context the Context to use when resolving the Uri
     * @param uri     the Content URI of the data you want to play
     * @throws IllegalStateException if it is called in an invalid state
     */
    public int setDataSource(Context context, Uri uri) {
        String scheme = uri.getScheme();
        if (scheme == null || scheme.equals("file")) {
            int ret = 0;

            if (uri.getPath().endsWith("mpg")) {
                ret = nativeSetDataSource("");
            } else {
                ret = nativeSetDataSource(uri.toString());
            }
            if (ret < 0) {
                Log.d(TAG, "setDataSource fail!");
            }
            return ret;
        } else {
            Log.d(TAG, "unrecognized file" + uri.toString());
            return -1;
        }
    }

    /**
     * Starts or resumes playback. If playback had previously been paused,
     * playback will continue from where it was paused. If playback had
     * been stopped, or never started before, playback will start at the
     * beginning.
     *
     * @throws IllegalStateException if it is called in an invalid state
     */
    public void start() throws IllegalStateException {
        stayAwake(true);
        nativeStart();
    }

    /**
     * Stops playback after playback has been stopped or paused.
     *
     * @throws IllegalStateException if the internal player engine has not been
     *                               initialized.
     */
    public void stop() throws IllegalStateException {
        stayAwake(false);
        nativeStop();
    }

    /**
     * Pauses playback. Call start() to resume.
     *
     * @throws IllegalStateException if the internal player engine has not been
     *                               initialized.
     */
    public void pause() throws IllegalStateException {
        stayAwake(false);
        nativePause();
    }

    /**
     * Seek playback.
     *
     * @throws IllegalStateException if the internal player engine has not been
     *                               initialized.
     */
    public void seekTo(int msec) throws IllegalStateException {
        stayAwake(false);
        if (isSeekable()) {
            nativeSeek(msec);
        }
    }

    public int getDuration() {
        int duration = nativeGetDuration();
        if (duration < 0) {
            return -1;
        }
        return duration;
    }

    /**
     * Releases resources associated with this LivePlayer object.
     * It is considered good practice to call this method when you're
     * done using the LivePlayer.
     */
    public void release() {
        stayAwake(false);
        updateSurfaceScreenOn();
        nativeRelease();
    }

    /**
     * Resets the LivePlayer to its uninitialized state. After calling
     * this method, you will have to initialize it again by setting the
     * data source and calling prepare().
     */
    public void reset() {
        stayAwake(false);
        nativeReset();
    }

    public int setVolume(float leftVolume, float rightVolume) {
        return nativeSetVolume(leftVolume, rightVolume);
    }

    /**
     * Set the low-level power management behavior for this LivePlayer.  This
     * can be used when the LivePlayer is not playing through a SurfaceHolder
     * set with {@link #setDisplay(SurfaceHolder)} and thus can use the
     * high-level {@link #setScreenOnWhilePlaying(boolean)} feature.
     * <p/>
     * <p>This function has the LivePlayer access the low-level power manager
     * service to control the device's power usage while playing is occurring.
     * The parameter is a combination of {@link PowerManager} wake flags.
     * Use of this method requires {@link android.Manifest.permission#WAKE_LOCK}
     * permission.
     * By default, no attempt is made to keep the device awake during playback.
     *
     * @param context the Context to use
     * @param mode    the power/wake mode to set
     * @see PowerManager
     */
    public void setWakeMode(Context context, int mode) {
        boolean washeld = false;
        if (mWakeLock != null) {
            if (mWakeLock.isHeld()) {
                washeld = true;
                mWakeLock.release();
            }
            mWakeLock = null;
        }

        PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(mode | PowerManager.ON_AFTER_RELEASE, LivePlayer.class.getName());
        mWakeLock.setReferenceCounted(false);
        if (washeld) {
            mWakeLock.acquire();
        }
    }

    /**
     * Control whether we should use the attached SurfaceHolder to keep the
     * screen on while video playback is occurring.  This is the preferred
     * method over {@link #setWakeMode} where possible, since it doesn't
     * require that the application have permission for low-level wake lock
     * access.
     *
     * @param screenOn Supply true to keep the screen on, false to allow it
     *                 to turn off.
     */
    public void setScreenOnWhilePlaying(boolean screenOn) {
        if (mScreenOnWhilePlaying != screenOn) {
            mScreenOnWhilePlaying = screenOn;
            updateSurfaceScreenOn();
        }
    }

    private void stayAwake(boolean awake) {
        if (mWakeLock != null) {
            if (awake && !mWakeLock.isHeld()) {
                mWakeLock.acquire();
            } else if (!awake && mWakeLock.isHeld()) {
                mWakeLock.release();
            }
        }
        mStayAwake = awake;
        updateSurfaceScreenOn();
    }

    private void updateSurfaceScreenOn() {
        if (mSurfaceHolder != null) {
            mSurfaceHolder.setKeepScreenOn(mScreenOnWhilePlaying && mStayAwake);
        }
    }

    private native int nativeSetDisplayType(int DType);
    private native int nativeSetVideoSurface(Surface surface);
    private native int nativeSetDataSource(String path);
    private native int nativeStart();
    private native int nativeStop();
    private native int nativePause();
    private native int nativeSeek(long msec);
    private native int nativeRelease();
    private native int nativeReset();
    private native int nativeSetVolume(float leftVolume, float rightVolume);
    private native int nativeGetDuration();
    public native int nativeGetVideoWidth();
    public native int nativeGetVideoHeight();
    public native boolean isPlaying();
    public native int getCurrentPosition();
    public native boolean isSeekable();
    public native boolean isCanPause();
    public native int getPlayMode();
}
