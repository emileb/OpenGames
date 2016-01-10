package com.beloko.abuse;

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
import android.widget.ListView;
import android.widget.TextView;

import com.beloko.opengames.AppSettings;
import com.beloko.opengames.GD;
import com.beloko.opengames.Utils;

import java.util.ArrayList;
import java.util.Iterator;


public class LaunchFragment extends Fragment{                           
  
	String LOG = "LaunchFragment";               

	TextView gameArgsTextView;                  
	EditText argsEditText;                     
	ListView listview;            
	TextView copyWadsTextView;                                          
             
	String demoBaseDir;                    
	String fullBaseDir;
                                 
	ArrayList<String> argsHistory = new ArrayList<String>();  

	boolean playExpansion = false;  
                                                        
	@Override       
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);  

		demoBaseDir = AppSettings.getQuakeDemoDir();
		fullBaseDir = AppSettings.getQuakeFullDir();

		AppSettings.createDirectories(getActivity());

		Utils.loadArgs(getActivity(), argsHistory);
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


		copyWadsTextView = (TextView)mainView.findViewById(R.id.copy_wads_textview);

		Button startfull = (Button)mainView.findViewById(R.id.start_full);
		startfull.setOnClickListener(new OnClickListener() {
 
			@Override
			public void onClick(View v) {

				{
					startGame(fullBaseDir,false);  
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

	
		//selectGame(0);
		return mainView;
	}


	
	void startGame(final String base,boolean ignoreMusic)
	{
		

		


		
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
			Utils.saveArgs(getActivity(),argsHistory);  
		}
 
	
		String args =  gameArgsTextView.getText().toString() + " " + argsEditText.getText().toString();


		Intent intent = new Intent(getActivity(), com.beloko.opengames.abuse.Game.class);

		intent.setAction(Intent.ACTION_MAIN);
		intent.addCategory(Intent.CATEGORY_LAUNCHER);

		intent.putExtra("game_path",base);    
		intent.putExtra("game",AppSettings.game.toString());       
		
		intent.putExtra("args",args + "");                                                                   
		startActivity(intent);   
	}                  

	
}
