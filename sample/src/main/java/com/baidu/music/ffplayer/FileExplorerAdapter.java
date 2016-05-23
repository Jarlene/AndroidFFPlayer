package com.baidu.music.ffplayer;

import java.io.File;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class FileExplorerAdapter extends BaseAdapter {
	
	private File[] 							mFiles;
	private LayoutInflater 					mInflater;
	private Context 						mContext;
	private boolean							isTop;

	public FileExplorerAdapter(Context context, File[] files, boolean root) {
		mFiles = files;
        mInflater = LayoutInflater.from(context);
        mContext = context;
        isTop = root;
	}
	
	public int getCount() {
		return mFiles.length;
	}

	public Object getItem(int position) {
		return mFiles[position];
	}

	public long getItemId(int position) {
		return position;
	}
	
	private void setRow(ViewHolder holder, int position) {
		File file = mFiles[position];
		holder.text.setText(file.getName());
		if(file.isDirectory())
		{
			holder.icon.setImageDrawable(mContext.getResources().getDrawable(R.mipmap.folder_48));
		} 
		else if (file.isFile()) 
		{
			holder.icon.setImageDrawable(mContext.getResources().getDrawable(R.mipmap.file_48));
		} 
		else {
			
		}
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		// A ViewHolder keeps references to children views to avoid unneccessary calls
        // to findViewById() on each row.
        ViewHolder holder;

        // When convertView is not null, we can reuse it directly, there is no need
        // to reinflate it. We only inflate a new View when the convertView supplied
        // by ListView is null.
        if (convertView == null) {
            convertView = mInflater.inflate(R.layout.ffplay_file_row, null);

            // Creates a ViewHolder and store references to the two children views
            // we want to bind data to.
            holder = new ViewHolder();
            holder.text = (TextView) convertView.findViewById(R.id.textview_rowtext);
            holder.icon = (ImageView) convertView.findViewById(R.id.imageview_rowicon);

            convertView.setTag(holder);
        } else {
            // Get the ViewHolder back to get fast access to the TextView
            // and the ImageView.
            holder = (ViewHolder) convertView.getTag();
        }

		// Bind the data efficiently with the holder.
		setRow(holder, position);
		
        return convertView;
	}
	
	private static class ViewHolder {
	    TextView text;
	    ImageView icon;
	}
}