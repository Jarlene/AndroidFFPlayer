package com.baidu.music.ffplaylib.view;

/*
 * Copyright (C) 2006 The Android Open Source Project
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

import android.content.Context;
import android.net.Uri;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.MediaController;
import android.widget.MediaController.MediaPlayerControl;

import com.baidu.music.ffplaylib.jni.LivePlayer;

/**
 * Displays a video file.  The VideoView class
 * can load images from various sources (such as resources or content
 * providers), takes care of computing its measurement from the video so that
 * it can be used in any layout manager, and provides various display options
 * such as scaling and tinting.
 */
public class VideoView extends SurfaceView implements MediaPlayerControl {
       
    private String TAG = "VideoView";
    
    private Context mContext;
    
    // settable by the client
    private Uri         mUri;
    
    // All the stuff we need for playing and showing a video
    private SurfaceHolder mSurfaceHolder = null;
    private LivePlayer mLivePlayer = null;
    private int         mVideoWidth;
    private int         mVideoHeight;
    private MediaController mMediaController;


    public int getVideoWidth(){
    	return mVideoWidth;
    }
    
    public int getVideoHeight(){
    	return mVideoHeight;
    }
    
    public void setVideoScale(int width , int height){
    	LayoutParams lp = getLayoutParams();
    	lp.height = height;
		lp.width = width;
		setLayoutParams(lp);
    }
    
    public VideoView(Context context) {
        super(context);
        mContext = context;
        initVideoView();
    }

    public VideoView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
        mContext = context;
        initVideoView();
    }

    public VideoView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        mContext = context;
        initVideoView();
    }
    
    private void initVideoView() {
        mVideoWidth = 0;
        mVideoHeight = 0;
        getHolder().addCallback(mSHCallback);
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
    }

    public void setVideoPath(String path) {
    	Log.d(TAG, "setVideoPath: "+path);
        setVideoURI(Uri.parse(path));
    }

    public void setVideoURI(Uri uri) {
    	Log.d(TAG, "setVideoURI: "+uri);
        mUri = uri;
        openVideo();
        requestLayout();
        invalidate();
    }

    public void stopPlayback() {
        if (mLivePlayer != null) {
        	mLivePlayer.reset();
            mLivePlayer.release();
            mLivePlayer = null;
        }
    }

    private void openVideo() {
    	Log.d(TAG, "openVideo");
        if (mUri == null ) {
            return;
        }
        if (mLivePlayer != null) {
            mLivePlayer.reset();
        } else {
            mLivePlayer = new LivePlayer();
        }
        try {
            mLivePlayer.setDisplay(mSurfaceHolder);
            
            if(mLivePlayer.setDataSource(mContext, mUri)<0) {
                return;
            }
            mLivePlayer.setScreenOnWhilePlaying(true);
         
            attachMediaController();
        } catch (Exception ex) {
            Log.w(TAG, "Unable to open content: " + mUri, ex);
            return;
        }
    }

    public void setMediaController(MediaController controller) {
        if (mMediaController != null) {
            mMediaController.hide();
        }
        mMediaController = controller;
        attachMediaController();
    }

    private void attachMediaController() {
        if (mLivePlayer != null) {
        	mMediaController = new MediaController(getContext());
            mMediaController.setMediaPlayer(this);
            View anchorView = this.getParent() instanceof View ?
                    (View)this.getParent() : this;
            mMediaController.setAnchorView(anchorView);
            mMediaController.setEnabled(true);
        }
    }

    SurfaceHolder.Callback mSHCallback = new SurfaceHolder.Callback() {
        @Override
        public void surfaceChanged(SurfaceHolder holder, int format,
                                    int w, int h) {
        	Log.d(TAG, "surfaceChanged, format="+format+", w="+w+", h="+h);
            if (mLivePlayer != null && mVideoWidth == w && mVideoHeight == h) {
                //mLivePlayer.start();
                if (mMediaController != null) {
                    mMediaController.show();
                }
            }
        }

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
        	Log.d(TAG, "surfaceCreated, holder="+holder);
            mSurfaceHolder = holder;
            if(mLivePlayer != null)
            {
            	mLivePlayer.setDisplay(mSurfaceHolder);
            }
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            // after we return from this we can't use the surface any more
        	Log.d(TAG, "surfaceDestroyed, holder="+holder);
            mSurfaceHolder = null;
            if (mMediaController != null) mMediaController.hide();
            if (mLivePlayer != null) {
            	mLivePlayer.setDisplay(null);
            }
        }
    };
    
    static int sDisplayType = 0;
    @Override
    public boolean onTouchEvent(MotionEvent ev) {
        if (mLivePlayer != null && mMediaController != null) {
            toggleMediaControlsVisiblity();
            Log.d(TAG, "x "+ev.getRawX()+ " y "+ev.getRawY());
        }
        return false;
    }

    @Override
    public boolean onTrackballEvent(MotionEvent ev) {
        if (mLivePlayer != null && mMediaController != null) {
            toggleMediaControlsVisiblity();
        }
        return false;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
    	if ( keyCode == KeyEvent.KEYCODE_BACK )
    	{
			stopPlayback();
			mLivePlayer = null;
    	}
        return super.onKeyDown(keyCode, event);
    }

    private void toggleMediaControlsVisiblity() {
        if (mMediaController.isShowing()) {
            mMediaController.hide();
        } else {
            mMediaController.show();
        }
    }

    @Override
    public void start() {
        if (mLivePlayer != null) {
        	mLivePlayer.start();
        }
    }

    @Override
    public void pause() {
        if (mLivePlayer != null) {
            if (mLivePlayer.isPlaying()) {
                mLivePlayer.pause();
            }
        }
    }

    @Override
    public int getDuration() {
        if (mLivePlayer != null) {
            return mLivePlayer.getDuration();
        }
        return -1;
    }

    @Override
    public int getCurrentPosition() {
       if (mLivePlayer != null) {
            return mLivePlayer.getCurrentPosition();
        }
        return 0;
    }

    @Override
    public void seekTo(int msec) {
        if (mLivePlayer != null) {
        	if(mLivePlayer.isSeekable())
            {
        		mLivePlayer.seekTo(msec);
            }
        }
    }

    @Override
    public boolean isPlaying() {
        if (mLivePlayer != null) {
            return mLivePlayer.isPlaying();
        }
        return false;
    }

    @Override
    public int getBufferPercentage() {
        return 0;
    }

    @Override
    public boolean canPause() {
    	if (mLivePlayer != null) {
            return mLivePlayer.isCanPause();
        }
        return true;
	}

    @Override
    public boolean canSeekBackward() {
    	if (mLivePlayer != null) {
            mLivePlayer.isSeekable();
        }
        return true;
	}

    @Override
    public boolean canSeekForward() {
        if (mLivePlayer != null) {
            mLivePlayer.isSeekable();
        }
        return true;
	}

	@Override
	public int getAudioSessionId() {
		return 0;
	}

}
