package com.baidu.music.ffplayer;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.View;
import android.widget.Button;

import com.baidu.music.ffplaylib.jni.LivePlayer;
import com.baidu.music.ffplaylib.view.VideoView;

import java.util.LinkedList;

public class MainActivity extends Activity implements View.OnClickListener{

    private final static String TAG = "VideoPlayerActivity";

    private VideoView vv = null;
    private Button play, pause, stop;
    String path = null;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        vv = (VideoView) findViewById(R.id.vv);
        play = (Button) findViewById(R.id.play);
        pause = (Button) findViewById(R.id.pause);
        stop = (Button) findViewById(R.id.stop);

        play.setOnClickListener(this);
        stop.setOnClickListener(this);
        pause.setOnClickListener(this);
        processIntentData(getIntent());
    }

    @Override
    protected void onStart() {
        super.onStart();

    }

    @Override
    protected void onResume() {
        super.onResume();

    }

    protected void processIntentData(Intent intent) {
        if (intent == null) {
            return;
        }
        path = intent.getStringExtra("file");

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.play:
            vv.setVideoPath(path);
            vv.start();
                break;
            case R.id.pause:
                vv.pause();
                break;
            case R.id.stop:
                vv.stop();
                break;
            default:
                break;
        }
    }
}
