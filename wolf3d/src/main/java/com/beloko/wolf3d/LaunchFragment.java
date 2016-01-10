package com.beloko.wolf3d;

import android.app.AlertDialog;
import android.app.Fragment;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.RadioGroup.OnCheckedChangeListener;
import android.widget.TextView;
import android.widget.Toast;

import com.beloko.opengames.AppSettings;
import com.beloko.opengames.GD;
import com.beloko.opengames.ServerAPI;
import com.beloko.opengames.Utils;
import com.beloko.opengames.wolf3d.Game;
import com.beloko.opengames.wolf3d.NativeLib;

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

	TextView gameArgsTextView;       
	EditText argsEditText;  
	//ListView listview;

	LinearLayout  wolf_layout; 
	LinearLayout sod_layout;         
	TextView sod_misson_textview;
	FrameLayout frame;

	String demoBaseDir;   
	String fullBaseDir;

	ArrayList<String> argsHistory;

	int game;

	int lowRes;

	int disableAlphaFix;
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		AppSettings.setGame(GD.IDGame.Wolf3d);
		demoBaseDir = AppSettings.getQuakeDemoDir();
		fullBaseDir = AppSettings.getQuakeFullDir();

		AppSettings.createDirectories(getActivity());


		//String sig = PackageVerif.bytesToString((PackageVerif.packageSig(getActivity())).sig);
		//Log.d(LOG,"SIG = " + sig);
		//Log.d(LOG,"base = " + Base64.encodeToString((PackageVerif.packageSig(getActivity())).sig, 0));


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

		wolf_layout = (LinearLayout)mainView.findViewById(R.id.wolf_linearlayout);
		sod_layout = (LinearLayout)mainView.findViewById(R.id.sod_linearlayout);
		sod_misson_textview = (TextView)mainView.findViewById(R.id.sod_misson_textview);
		frame = (FrameLayout)mainView.findViewById(R.id.main_frameLayout);

		argsEditText = (EditText)mainView.findViewById(R.id.extra_args_edittext);
		gameArgsTextView = (TextView)mainView.findViewById(R.id.extra_args_textview);
		//listview = (ListView)mainView.findViewById(R.id.listView);

		//listview.setBackgroundDrawable(new BitmapDrawable(getResources(),decodeSampledBitmapFromResource(getResources(),R.drawable.rtcw,400,200)));


		wolf_layout.setClickable(true);
		wolf_layout.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				selectGame(NativeLib.WOLF_GAME);
			}
		}); 

		sod_layout.setClickable(true);
		sod_layout.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (game == NativeLib.WOLF_GAME)
				{
					Toast.makeText(getActivity(),"Press again to select mission packs", Toast.LENGTH_LONG).show();
					selectGame(NativeLib.SOD_GAME);
				}
				else if (game == NativeLib.SOD_GAME)
					selectGame(NativeLib.SD2_GAME);
				else if (game == NativeLib.SD2_GAME)
					selectGame(NativeLib.SD3_GAME);
				else if (game == NativeLib.SD3_GAME)
					selectGame(NativeLib.SOD_GAME);
			}
		});
		Button googlePlay = (Button)mainView.findViewById(R.id.googlePlay_button);
		googlePlay.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				try {
					startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://www.amazon.com/s/ref=cm_cr_pr_pdt_bl_sr?ie=UTF8&field-keywords=Beloko+Games")));
				} catch (android.content.ActivityNotFoundException anfe) {
					 
				}
			}
		});
		
		/*
		listview.setOnItemClickListener(new OnItemClickListener() {          

			@Override
			public void onItemClick(AdapterView<?> arg0, View arg1, int pos,
					long arg3) {
				Advert a = adverts.get(pos);

				try {
					startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("market://details?id="+a.link)));
				} catch (android.content.ActivityNotFoundException anfe) {
					startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://play.google.com/store/apps/details?id="+a.link)));
				}
			}
		});     
		 */     
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

		CheckBox disableAlphaCheckbox = ((CheckBox)mainView.findViewById(R.id.disable_alpha_checkbox));
		disableAlphaFix  = AppSettings.getIntOption(getActivity(), "disable_alpha_fix", 0);
		if (disableAlphaFix == 0)
			disableAlphaCheckbox.setChecked(false);
		else
			disableAlphaCheckbox.setChecked(true);

		disableAlphaCheckbox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				if (isChecked)
					disableAlphaFix = 1;
				else
					disableAlphaFix = 0;
				AppSettings.setIntOption(getActivity(), "disable_alpha_fix", disableAlphaFix);
			}
		});

		Button startdemo = (Button)mainView.findViewById(R.id.start_demo);
		startdemo.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				if (Utils.checkFiles(demoBaseDir, new String[]{"vswap.wl1"}) != null)
				{  
					AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
					builder.setMessage("Download Wolf 3D Shareware Data (1MB)?")
					.setCancelable(true)
					.setPositiveButton("OK", new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int id) {
							//ServerAPI.downloadFileSecure(getActivity(),BASE64_PUBLIC_KEY,"wolf3d_demo.zip",demoBaseDir);
							ServerAPI.downloadFile(getActivity(), "wolf3d_demo.zip", demoBaseDir);
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
					startQuake(demoBaseDir,true);
				}
			}  
		});                
		Button startfull = (Button)mainView.findViewById(R.id.start_full);
		startfull.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				if (game == NativeLib.WOLF_GAME)
				{
					String missingFiles;
					if ((missingFiles = Utils.checkFiles(fullBaseDir , new String[] {"VSWAP.WL6","VGAHEAD.WL6","VGAGRAPH.WL6","VGADICT.WL6"
							,"MAPHEAD.WL6","GAMEMAPS.WL6","AUDIOT.WL6","AUDIOHED.WL6"})) != null)
					{
						AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
						builder.setMessage("Please copy:\n" + missingFiles + "From the full version of Wolf 3D in to:\n" + fullBaseDir)
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
						startQuake(fullBaseDir,false);
					}
				}
				else //SOD
				{
					String[] req_files=null;

					if (game == NativeLib.SOD_GAME)
						req_files = new String[] {"VSWAP.SOD","VGAHEAD.SOD","VGAGRAPH.SOD","VGADICT.SOD"
							,"MAPHEAD.SOD","GAMEMAPS.SOD","AUDIOT.SOD","AUDIOHED.SOD"};
					else if (game == NativeLib.SD2_GAME)
						req_files = new String[] {"VSWAP.SD2","VGAHEAD.SOD","VGAGRAPH.SOD","VGADICT.SOD"
							,"MAPHEAD.SD2","GAMEMAPS.SD2","AUDIOT.SOD","AUDIOHED.SOD"};
					else if (game == NativeLib.SD3_GAME)
						req_files = new String[] {"VSWAP.SD3","VGAHEAD.SOD","VGAGRAPH.SOD","VGADICT.SOD"
							,"MAPHEAD.SD3","GAMEMAPS.SD3","AUDIOT.SOD","AUDIOHED.SOD"};

					String missingFiles;
					if ((missingFiles = Utils.checkFiles(fullBaseDir , req_files)) != null)
					{
						AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
						builder.setMessage("Please copy:\n" + missingFiles + "From the full version of Wolf 3D Spear Of Destiny in to:\n" + fullBaseDir)
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
						startQuake(fullBaseDir,false);
					}
				}
			}
		});

		ImageView history = (ImageView)mainView.findViewById(R.id.args_history_imageview);
		history.setOnClickListener(new View.OnClickListener() {
			//@Override
			public void onClick(View v) {

				String[] servers = new String[argsHistory.size()];
				for (int n = 0; n < argsHistory.size(); n++) servers[n] = argsHistory.get(n);

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

		int r = AppSettings.getIntOption(getActivity(), "last_game", NativeLib.WOLF_GAME);
		selectGame(r);

		//selectGame(0);
		return mainView;
	}


	private void selectGame(int r){
		game = r;
		AppSettings.setIntOption(getActivity(), "last_game", r);

		frame.setAlpha(0.3f);
		if (r == NativeLib.SOD_GAME)
		{
			wolf_layout.setBackgroundResource(0);
			sod_layout.setBackgroundResource(R.drawable.layout_sel_background);
			sod_misson_textview.setText("SOD");
			//frame.setBackgroundResource(R.drawable.sod);
		}
		else if  (r == NativeLib.SD2_GAME)
		{
			wolf_layout.setBackgroundResource(0);
			sod_layout.setBackgroundResource(R.drawable.layout_sel_background);
			sod_misson_textview.setText("SD2");
			//frame.setBackgroundResource(R.drawable.sod);
		}
		else if  (r == NativeLib.SD3_GAME)
		{
			wolf_layout.setBackgroundResource(0);
			sod_layout.setBackgroundResource(R.drawable.layout_sel_background);
			sod_misson_textview.setText("SD3");
			//frame.setBackgroundResource(R.drawable.sod);
		}
		else
		{
			sod_layout.setBackgroundResource(0);
			wolf_layout.setBackgroundResource(R.drawable.layout_sel_background);
			//frame.setBackgroundResource(R.drawable.wolf3d);
		}
	}
	void startQuake(final String base,boolean demo)
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
		intent.putExtra("demo", demo);
		String gamea = "";
		if (game == NativeLib.WOLF_GAME)
			gamea =  " --data wl6 ";
		else if (game == NativeLib.SOD_GAME)
			gamea =  " --data sod ";  
		else if (game == NativeLib.SD2_GAME)
			gamea =  " --data sd2 ";  
		else if (game == NativeLib.SD3_GAME)
			gamea =  " --data sd3 ";  

		intent.putExtra("low_res",lowRes);
		intent.putExtra("disable_alpha_fix",disableAlphaFix);

		intent.putExtra("args"," --samplerate 11250 --bits 32" + gamea + args + " ");                                                
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
