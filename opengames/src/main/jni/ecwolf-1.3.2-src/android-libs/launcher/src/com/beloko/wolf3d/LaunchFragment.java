package com.beloko.wolf3d;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Iterator;

import android.app.AlertDialog;
import android.app.Fragment;
import android.content.DialogInterface;
import android.content.Intent;
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
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.RadioGroup.OnCheckedChangeListener;
import android.widget.TextView;
import android.widget.Toast;

import com.beloko.idtech.AppSettings;
import com.beloko.idtech.GD;
import com.beloko.idtech.GD.IDGame;
import com.beloko.idtech.Utils;
import com.beloko.idtech.wolf3d.Game;
import com.beloko.idtech.R;

public class LaunchFragment extends Fragment{                           

	String LOG = "LaunchFragment";     

	TextView gameArgsTextView;       
	EditText argsEditText;       
	ListView listview;

	TextView copyWadsTextView;                                          

	String demoBaseDir;
	String fullBaseDir;

	ArrayList<String> argsHistory;


	int lowRes;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		AppSettings.setGame(IDGame.Wolf3d);
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

		lowRes = 0;

		RadioGroup resRadio =  (RadioGroup) mainView.findViewById(R.id.res_radiogroup);

		resRadio.setOnCheckedChangeListener(new OnCheckedChangeListener() 
		{
			public void onCheckedChanged(RadioGroup group, int checkedId) {
				// checkedId is the RadioButton selected

				switch(checkedId) {
				case R.id.high_res_radiobutton:
					AppSettings.setIntOption(getActivity(), "low_res", 0);
					lowRes = 0;
					break;
				case R.id.low_res_radiobutton:
					AppSettings.setIntOption(getActivity(), "low_res", 1);
					lowRes = 1;
					break;
				}   
			}
		}); 

		lowRes = AppSettings.getIntOption(getActivity(), "low_res", 0);
		if (lowRes == 0)
			((RadioButton)mainView.findViewById(R.id.high_res_radiobutton)).setChecked(true);
		else
			((RadioButton)mainView.findViewById(R.id.low_res_radiobutton)).setChecked(true);


		Button startfull = (Button)mainView.findViewById(R.id.start_full);
		startfull.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String missingFiles;
				if ((missingFiles = Utils.checkFiles(fullBaseDir , new String[] {"ecwolf.pk3"})) != null)
					Utils.copyAsset(getActivity(), "ecwolf.pk3", fullBaseDir);
				startGame(fullBaseDir);
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



	void startGame(final String base)
	{
		//Check prboom wad exists
		//File ecwolfpk3 = new File(base + "/ecwolf.pk3"  );
		//if (!ecwolfpk3.exists())
		{
			Utils.copyAsset(getActivity(),"ecwolf.pk3",base);
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


		intent.putExtra("low_res",lowRes);
		intent.putExtra("args"," --samplerate 11250 --bits 32"  + args + " ");                                                
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
