package com.beloko.jk3touch;

import android.app.AlertDialog;
import android.app.Fragment;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.beloko.jk3.R;
import com.beloko.opengames.AppSettings;
import com.beloko.opengames.GD;
import com.beloko.opengames.Utils;

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
   
	private static final String BASE64_PUBLIC_KEY = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA06//Vew/xgFFoT9Y5Sev2NorgazcwzTIovRH7LX33TYI/3BRDdoL5b2h6+IHyZtK8SRpH7SwxyDpcbUMF3ctWgVTmHDmuhvbvbUHcD7jV+ywwI5qVdJObbxONv3z7Kx/ws3i+ec4W/K/NA37PGxHiXOdVGeXHKXfYs9cQTqPT4d8hdA0UxxWjJjdVnGmR4l2jLSn34R67o5ZnlIOZ0mTtG4ICHNVWfPTeW89SgfdVrbM6VWwRcVS3NEAPPQumNRqKTyNcf8p6YBrSE9gLFfFtHabHY0bdwrhfXkTNOsoACNA0EVu9HtamqzyMwr0rCh+A78znYPdF4GVezrVHzfCsQIDAQAB";
	             
	TextView gameArgsTextView;                  
	EditText argsEditText;                              
	ImageView listview;
  
	String demoBaseDir;
	String fullBaseDir;   

	ArrayList<String> argsHistory;         

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);


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
		//refreshGames();
		super.onHiddenChanged(hidden);
	}


	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		View mainView = inflater.inflate(R.layout.fragment_launch, null);

		argsEditText = (EditText)mainView.findViewById(R.id.extra_args_edittext);
		gameArgsTextView = (TextView)mainView.findViewById(R.id.extra_args_textview);
		listview = (ImageView)mainView.findViewById(R.id.listView);
		listview.setImageResource(R.drawable.bg);

		Button startdemo = (Button)mainView.findViewById(R.id.start_demo);
		startdemo.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
/*
				if (Utils.checkFiles(demoBaseDir + "/Demo/", new String[]{"assets0.pk3"})!=null)
				{          
					AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
					builder.setMessage("Download Jedi Knight : Jedi Academy Shareware Data (188MB)?")
					.setCancelable(true)
					.setPositiveButton("OK", new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int id) {
							ServerAPI.downloadFileSecure(getActivity(), BASE64_PUBLIC_KEY, "jk3_demo.zip", demoBaseDir);
							//ServerAPI.downloadFile(getActivity(),"jk2_demo.zip",demoBaseDir);

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
					startGame(demoBaseDir,false);
				}
				*/
			}

		});
		Button startfull = (Button)mainView.findViewById(R.id.start_full);
		startfull.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String missingFiles;
				if ((missingFiles = Utils.checkFiles(fullBaseDir + "/base/", new String[] {"assets0.pk3","assets1.pk3"})) != null)
				{
					AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
					builder.setMessage("Please copy:\n" + missingFiles + "From the full version of JK3 in to:\n" + fullBaseDir + "/base/")
					.setCancelable(true)
					.setPositiveButton("OK", new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int id) {

						}
					});

					AlertDialog alert = builder.create();
					alert.show();
				}
				else
				{
					startGame(fullBaseDir,false);
				}
			}
		});

		Button startfullmp = (Button)mainView.findViewById(R.id.start_full_mp);
		startfullmp.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String missingFiles;
				if ((missingFiles = Utils.checkFiles(fullBaseDir + "/base/", new String[] {"assets0.pk3","assets1.pk3"})) != null)
				{
					AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
					builder.setMessage("Please copy:\n" + missingFiles + "From the full version of JK3 in to:\n" + fullBaseDir + "/base/")
					.setCancelable(true)
					.setPositiveButton("OK", new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int id) {

						}
					});

					AlertDialog alert = builder.create();
					alert.show();
				}
				else
				{
					startGame(fullBaseDir,true);
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


	void startGame(String base,boolean mp)
	{
		if (Utils.checkFiles(fullBaseDir + "/base/", new String[] {"assets2.pk3"}) != null)
		{
			Utils.copyAsset(getActivity(), "assets2.pk3", fullBaseDir + "/base/");
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

		Intent intent ;
		if (mp)
			intent = new Intent(getActivity(),com.beloko.opengames.jk3mp.Game.class);
		else
			intent = new Intent(getActivity(),com.beloko.opengames.jk3.Game.class);

		intent.setAction(Intent.ACTION_MAIN);
		intent.addCategory(Intent.CATEGORY_LAUNCHER);

		intent.putExtra("game_path",base);                 
		intent.putExtra("args","+set fs_basepath " + base + "/ "  + args);

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

}
