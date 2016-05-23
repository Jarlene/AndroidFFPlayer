package com.baidu.music.ffplayer;

import android.content.ContentResolver;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;

import com.baidu.music.ffplaylib.jni.LivePlayer;
import com.baidu.music.ffplaylib.view.VideoView;

import java.util.LinkedList;

public class MainActivity extends AppCompatActivity {

    private final static String TAG = "VideoPlayerActivity";

    private VideoView vv = null;

    public static LinkedList<MovieInfo> playList = new LinkedList<MovieInfo>();
    public class MovieInfo{
        String displayName;
        String path;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);


        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        processIntentData(getIntent());
    }

    protected String getSchemePath(Uri uri) {
        if(uri != null)
        {
            if("content".equals(uri.getScheme()))
            {
                ContentResolver contentResolver = getContentResolver();
                Cursor c = contentResolver.query(uri, null, null, null, null);
                if(c.moveToFirst())
                {
                    return c.getString(c.getColumnIndex(MediaStore.Video.Media.DATA));
                }
            }
            else if("file".equals(uri.getScheme()))
            {
                return uri.getPath();
//				String path = uri.toString();
//				//�ȶ��ַ���н���룬�����?�Ǳ��뷽ʽԭ�����
//				path = Uri.decode(path);
//				path = this.encodeFileName(path, "/");
//				URI fileURI = URI.create(path);
//				File file = new File(fileURI);
//
//				return file.getPath();
            }
            else if("http".equals(uri.getScheme()))
            {
                return uri.toString();
            }
        }

        return "";
    }

    /**
     * ���ļ��л����ļ������ַ���б������
     * @author song.lj
     * @return String
     */
    private String encodeFileName(String path, String regular){
        String[] parts = path.split(regular);
        StringBuffer sb = new StringBuffer();
        sb.append(parts[0]);
        for(int i=1; i<parts.length; i++){
            sb.append(regular);
            sb.append(Uri.encode(parts[i]));
        }

        return sb.toString();
    }

    @SuppressWarnings("null")
    protected void processIntentData(Intent intent)
    {
        String path = null;
        if (intent.getAction() != null&& intent.getAction().equals(Intent.ACTION_VIEW ))
        {
            /* Started from external application */
            path = getSchemePath(intent.getData());

            if (path == null || path.length() <= 1)
            {
                //finish();
                return ;
            }
        }
        else
        {
            Bundle bundle = intent.getExtras();
            if(bundle == null)
            {
//				this.finish();
                return ;
            }
            path = bundle.getString("file");
        }

        if(vv == null)
        {
            vv = (VideoView)findViewById(R.id.vv);
            LivePlayer.TimeShiftInfo TShiftInfo = new LivePlayer.TimeShiftInfo();

            TShiftInfo.mbAllowTimeShift = true;
            TShiftInfo.mFileName		= "/sdcard/ptvlive.cache";
            TShiftInfo.mFileSize		= 100*1024*1024;

            vv.SetTimeShiftInfo(TShiftInfo);
            vv.setVideoPath(path);
        }
        else
        {
            LivePlayer.TimeShiftInfo TShiftInfo = new LivePlayer.TimeShiftInfo();

            TShiftInfo.mbAllowTimeShift = true;
            TShiftInfo.mFileName		= "/sdcard/ptvlive.cache";
            TShiftInfo.mFileSize		= 100*1024*1024;
            vv.setVideoPath(path);
        }
    }
}
