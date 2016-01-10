package com.beloko.rtcw;

import android.app.AlertDialog;
import android.app.Fragment;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.beloko.opengames.AppSettings;
import com.beloko.opengames.GD;
import com.beloko.opengames.NoticeDialog;
import com.beloko.opengames.ServerAPI;
import com.beloko.opengames.Utils;
import com.beloko.opengames.rtcw.Game;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Iterator;


public class LaunchFragment extends Fragment{

	String LOG = "LaunchFragment";
         
	private static final String BASE64_PUBLIC_KEY = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqJLgDqnhFXxKGt+3V7F6hzYWk0ET7CKdeIJa/pejyiIonX5X5BIPFpV/HHiDE2a3em0rGK9eC91K72RqxSMjcnjo6iiUSJJKQ+I8sXgOGfJzwP/53ky+PtFf6yMuYPnozZZTGpNlCizXdhpbCCK0cI04h36booqtZjVvH1DcyAdjRkvhVNLuxwglSlM902w+FRKwKlmlT9pn+3x6qfeN9NiRDT3CAuQ03INxqMTF4fcO9OpFYcni376igzlRT7eGgFtkVWVgVcfHg6rfsVHB+ehmaQmLCL1cuV2H18kkCgOYG8BHMwuYXr8Y8/jWyVuS+8t3gVyiHBN8aTuOteS13wIDAQAB";
 
	TextView gameArgsTextView;  
	EditText argsEditText;      
	ListView listview;
        	  
	TextView copyWadsTextView;                     
     
	String demoBaseDir;   
	String fullBaseDir;
           
	ArrayList<String> argsHistory;

	int renderer;
 
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		AppSettings.setGame(GD.IDGame.RTCW);
		demoBaseDir = AppSettings.getQuakeDemoDir();
		fullBaseDir = AppSettings.getQuakeFullDir();

		AppSettings.createDirectories(getActivity());

		loadArgs();
	}                                           


	@Override
	public void onHiddenChanged(boolean hidden) {
		if (GD.DEBUG) Log.d(LOG,"onHiddenChanged");
		demoBaseDir = AppSettings.getQuakeDemoDir();
		fullBaseDir = AppSettings.getQuakeFullDir();
		
		super.onHiddenChanged(hidden);
	}


	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		View mainView = inflater.inflate(R.layout.fragment_launch, null);

		argsEditText = (EditText)mainView.findViewById(R.id.extra_args_edittext);
		gameArgsTextView = (TextView)mainView.findViewById(R.id.extra_args_textview);
		listview = (ListView)mainView.findViewById(R.id.listView);

		listview.setBackgroundDrawable(new BitmapDrawable(getResources(),decodeSampledBitmapFromResource(getResources(),R.drawable.rtcw,400,200)));

		copyWadsTextView = (TextView)mainView.findViewById(R.id.copy_wads_textview);

	

		Button startdemo = (Button)mainView.findViewById(R.id.start_demo);
		startdemo.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				if (Utils.checkFiles(demoBaseDir + "/demomain/", new String[]{"pak0.pk3", "hunkusage.dat"})!=null)
				{
					AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
					builder.setMessage("Download RTCW Shareware Data (110MB)?")
					.setCancelable(true)
					.setPositiveButton("OK", new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int id) {
							ServerAPI.downloadFile(getActivity(), "rtcw_demo.zip", demoBaseDir);
							//ServerAPI.downloadFile(getActivity(),"rtcw_demo.zip",demoBaseDir);
						}
					});
					builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {

						@Override
						public void onClick(DialogInterface dialog, int which) {
							dialog.cancel();  
						}
					});                                                   
					AlertDialog alert = builder.create();
					alert.show(); 

				}
				else                 
				{ 
					startGame(demoBaseDir,true);
				}
			}
		});   
		Button startfull = (Button)mainView.findViewById(R.id.start_full);
		startfull.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				if (Utils.checkFiles(fullBaseDir + "/main/", new String[] {"pak0.pk3","sp_pak1.pk3","sp_pak2.pk3"})!=null)
				{
					AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
					builder.setMessage("Please copy:\npak0.pk3\nsp_pak1.pk3\nsp_pak2.pk3\nfrom the registered version of RTCW in to:\n" + fullBaseDir + "/main/")
					.setCancelable(true)
					.setPositiveButton("OK", new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int id) {

						}
					});

					AlertDialog alert = builder.create();
					alert.show();

				}
				else
					startGame(fullBaseDir,false);
			}
		});

		Button download_hd = (Button)mainView.findViewById(R.id.hd_download_button);
		download_hd.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				if (Utils.checkFiles(fullBaseDir + "/main/", new String[] {"rtw_pak0.pk3"})!=null)
				{
					AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
					builder.setMessage("Download Unofficial HD Texture Pack? (567MB)?")
					.setCancelable(true)
					.setPositiveButton("OK", new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int id) {
							ServerAPI.downloadFile(getActivity(), "rtw_pak0.pk3", fullBaseDir + "/main/");
							//ServerAPI.downloadFile(getActivity(),"rtw_pak0.pk3",fullBaseDir + "/main/");
							NoticeDialog.show(getActivity(), "HD Pack Info", R.raw.hd_info);
						}
					});

					AlertDialog alert = builder.create();
					alert.show(); 

				}
				else  
				{
					AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
					builder.setMessage("Already installed. Delete file 'rtw_pak0.pk3' to remove.")
					.setCancelable(true)
					.setPositiveButton("OK", new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int id) {
							
						}
					});

					AlertDialog alert = builder.create();
					alert.show();
				}
			}
		});

		
		
		ImageView history = (ImageView)mainView.findViewById(R.id.args_history_imageview);
		history.setOnClickListener(new View.OnClickListener() {
			//@Override
			public void onClick(View v) {

				String[] servers = new String[ argsHistory.size()];
				for (int n=0;n<argsHistory.size();n++) servers[n] = argsHistory.get(n);

				AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
				builder.setTitle("Extra Args History");
				builder.setItems(servers, new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int which) {
						argsEditText.setText(argsHistory.get(which));
					}
				});
				builder.show();
			}        
		});


		return mainView;
	}


	void startGame(final String base,boolean demo)
	{

		//Check for sp_pak4.pk3
		if (!demo)
		{
			if (Utils.checkFiles(fullBaseDir + "/main/", new String[] {"sp_pak4.pk3","sp_pak3.pk3"})!=null)
			{
				Utils.copyAsset(getActivity(),"sp_pak3.pk3",fullBaseDir + "/main/");
				Utils.copyAsset(getActivity(), "sp_pak4.pk3", fullBaseDir + "/main/");
			}
		}
		

		String extraArgs = argsEditText.getText().toString().trim();

		if (extraArgs.length() > 0)
		{
			Iterator<String> it = argsHistory.iterator();
			while (it.hasNext()) {
				String s = it.next();
				if (s.contentEquals(extraArgs))
					it.remove();
			}

			while (argsHistory.size()>50)  
				argsHistory.remove(argsHistory.size()-1);

			argsHistory.add(0, extraArgs);
			saveArgs();  
		} 

		String args =  gameArgsTextView.getText().toString() + " " + argsEditText.getText().toString();


		Intent intent = new Intent(getActivity(), Game.class);
		intent.setAction(Intent.ACTION_MAIN);
		intent.addCategory(Intent.CATEGORY_LAUNCHER);


		intent.putExtra("game_path",base);    
		intent.putExtra("demo", demo);
		
		intent.putExtra("args",args + " +set fs_basepath " + base + " " +args);                                                
		startActivity(intent);
	}                 



	void loadArgs()                      
	{ 
		File cacheDir = getActivity().getFilesDir();

		FileInputStream fis = null;
		ObjectInputStream in = null;
		try
		{
			fis = new FileInputStream(new File(cacheDir,"args_hist.dat"));
			in = new ObjectInputStream(fis);                
			argsHistory = (ArrayList<String>)in.readObject();
			in.close();
			return;
		}
		catch(IOException ex)
		{

		}  
		catch(ClassNotFoundException ex)
		{

		}

		//failed load, load default
		argsHistory = new ArrayList<String>();
	}


	void saveArgs()
	{
		File cacheDir =  getActivity().getFilesDir();

		if (!cacheDir.exists())
			cacheDir.mkdirs();

		FileOutputStream fos = null;
		ObjectOutputStream out = null;
		try
		{
			fos = new FileOutputStream(new File(cacheDir,"args_hist.dat"));
			out = new ObjectOutputStream(fos);
			out.writeObject(argsHistory);
			out.close();
		}
		catch(IOException ex)         
		{
			Toast.makeText(getActivity(),"Error saving args History list: " + ex.toString(), Toast.LENGTH_LONG).show();
		}
	}              

	
	
	public static int calculateInSampleSize(
			BitmapFactory.Options options, int reqWidth, int reqHeight) {
		// Raw height and width of image
		final int height = options.outHeight;
		final int width = options.outWidth;  
		int inSampleSize = 1;

		if (height > reqHeight || width > reqWidth) {

			// Calculate ratios of height and width to requested height and width
			final int heightRatio = Math.round((float) height / (float) reqHeight);
			final int widthRatio = Math.round((float) width / (float) reqWidth);

			// Choose the smallest ratio as inSampleSize value, this will guarantee
			// a final image with both dimensions larger than or equal to the
			// requested height and width.
			inSampleSize = heightRatio < widthRatio ? heightRatio : widthRatio;
		}

		return inSampleSize;
	}
	public static Bitmap decodeSampledBitmapFromResource(Resources res, int resId,
			int reqWidth, int reqHeight) {

		// First decode with inJustDecodeBounds=true to check dimensions
		final BitmapFactory.Options options = new BitmapFactory.Options();
		options.inJustDecodeBounds = true;
		BitmapFactory.decodeResource(res, resId, options);

		// Calculate inSampleSize
		options.inSampleSize = calculateInSampleSize(options, reqWidth, reqHeight);

		// Decode bitmap with inSampleSize set
		options.inJustDecodeBounds = false;
		return BitmapFactory.decodeResource(res, resId, options);
	}
	
}
