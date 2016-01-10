package com.beloko.jk2touch;

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

import com.beloko.jk2.R;
import com.beloko.opengames.AppSettings;
import com.beloko.opengames.GD;
import com.beloko.opengames.Utils;
import com.beloko.opengames.jk2.Game;

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
	     
	private static final String BASE64_PUBLIC_KEY = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAmj6wauxC704N1bXOwHVrf6XOGUdxQ7otJ4g8QFkfvVc2kJUN3n3A8M/qwVwXHBg/thu7Xv++3sRLc1KIVjTtUrOuyl3NIqtyO5DObEh1tlIiVqQv6YNxJZlOVCxw5v2vgZZgWoEZNvYfLznfJVvS0LIgmYVaNCPSsrqY7b8yoD//Rm4q6ilanJyW4OC8qZ1nkzwaN88ALRcqItHOGHhhJkxRyh341HU6zdvq7WHoEd6gdJFMQzdx0gRjuX8o0PurPRDmhmTmHOCMfynSQcxu68UT1pbhXy/YyL544Mrd5dlcIwCq8bPq3Dvmhtzgl0xt34jwgt6u03GtSQIzJ8kjxwIDAQAB";
   
	TextView gameArgsTextView;                                      
	EditText argsEditText;                       
	ImageView imageView;
                                             
                                                                
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
		imageView = (ImageView)mainView.findViewById(R.id.listView);
		imageView.setImageResource(R.drawable.bg);

		/*
		Button startdemo = (Button)mainView.findViewById(R.id.start_demo);
		startdemo.setOnClickListener(new OnClickListener() {
    
			@Override
			public void onClick(View v) {
        
				if (Utils.checkFiles(demoBaseDir + "/demo/", new String[]{"assets0.pk3"})!=null)
				{          
					AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
					builder.setMessage("Download Jedi Knight 2 Shareware Data (63MB)?")
					.setCancelable(true)
					.setPositiveButton("OK", new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int id) {
							ServerAPI.downloadFileSecure(getActivity(), BASE64_PUBLIC_KEY, "jk2_demo.zip", demoBaseDir);
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
					startGame(demoBaseDir);
				} 
			}
		});
		*/

		Button startfull = (Button)mainView.findViewById(R.id.start_full);
		startfull.setOnClickListener(new OnClickListener() {
 
			@Override
			public void onClick(View v) {
				String missingFiles;
				if ((missingFiles = Utils.checkFiles(fullBaseDir + "/base/", new String[] {"assets0.pk3","assets1.pk3"})) != null)
				{
					AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
					builder.setMessage("Please copy:\n" + missingFiles + "From the full version of JK2 in to:\n" + fullBaseDir + "/base/")
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
					startGame(fullBaseDir);
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
  

	void startGame(String base)
	{
		if (Utils.checkFiles(fullBaseDir + "/base/", new String[] {"assets2.pk3"}) != null)
		{
			Utils.copyAsset(getActivity(), "assets2.pk3", fullBaseDir + "/base/");
			Utils.copyAsset(getActivity(), "assets5.pk3", fullBaseDir + "/base/");
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
 
  
		//Intent intent = new Intent(getActivity(), FTEDroidActivity.class);
		Intent intent = new Intent(getActivity(), Game.class);
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
