package com.beloko.quaketouch;

import android.app.Activity;
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
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.beloko.opengames.AppSettings;
import com.beloko.opengames.GD;
import com.beloko.opengames.Utils;
import com.beloko.opengames.darkplaces.DP;
import com.beloko.opengames.fteqw.FTEDroidActivity;
import com.beloko.opengames.quakegl.QuakeActivity;

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

	LinearLayout gl_layout;
	LinearLayout fte_layout;
	LinearLayout dp_layout;
                   
	TextView gameArgsTextView;
	EditText argsEditText;  
	ListView listview;
 
	GamesListAdapter listAdapter;

	ArrayList<QuakeGameMod> games = new ArrayList<QuakeGameMod>();
	QuakeGameMod selectedMod;

	String demoBaseDir;
	String fullBaseDir;

	ArrayList<String> argsHistory;

	public enum QuakeEngine {
		GL,FTE,DP
	}     
	QuakeEngine engine;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		AppSettings.setGame(GD.IDGame.Quake);
		demoBaseDir = AppSettings.getQuakeDemoDir();
		fullBaseDir = AppSettings.getQuakeFullDir();

		AppSettings.createDirectories(getActivity());

		loadArgs();            
	}     

    
	@Override
	public void onHiddenChanged(boolean hidden) {
		if (GD.DEBUG) Log.d(LOG,"onHiddenChanged");
		AppSettings.setGame(GD.IDGame.Quake);
		demoBaseDir = AppSettings.getQuakeDemoDir();         
		fullBaseDir = AppSettings.getQuakeFullDir();
		refreshGames();
		selectGame(0);

		super.onHiddenChanged(hidden);
	}
 

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		View mainView = inflater.inflate(R.layout.fragment_launch, null);

		dp_layout = (LinearLayout)mainView.findViewById(R.id.quake_dp_linearlayout);
		gl_layout = (LinearLayout)mainView.findViewById(R.id.quake_gl_linearlayout);
		fte_layout = (LinearLayout)mainView.findViewById(R.id.quake_fte_linearlayout);
		argsEditText = (EditText)mainView.findViewById(R.id.extra_args_edittext);
		gameArgsTextView = (TextView)mainView.findViewById(R.id.extra_args_textview);
		listview = (ListView)mainView.findViewById(R.id.listView);

		listAdapter = new GamesListAdapter(getActivity());
		listview.setAdapter(listAdapter);


		listview.setOnItemClickListener(new OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> arg0, View arg1, int pos,
									long arg3) {
				selectGame(pos);
			}
		});

		gl_layout.setClickable(true);
		gl_layout.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				selectEngine(QuakeEngine.GL);
			}
		});

		fte_layout.setClickable(true);
		fte_layout.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				selectEngine(QuakeEngine.FTE);
			}
		});

		dp_layout.setClickable(true);
		dp_layout.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				selectEngine(QuakeEngine.DP);
			}
		});



		Button startfull = (Button)mainView.findViewById(R.id.start_full);
		startfull.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				String missingFiles;

				if ((missingFiles = Utils.checkFiles(fullBaseDir + "/id1", new String[] {"pak0.pak","pak1.pak"})) != null)
				{
					AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
					builder.setMessage("Please copy:\n" + missingFiles + "From the full version of Quake in to:\n" + fullBaseDir + "/id1")
					.setCancelable(true)
					.setPositiveButton("OK", new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int id) {
              
						}          
					});

					AlertDialog alert = builder.create();
					alert.show();
				}
				else
					startQuake(fullBaseDir);
			}
		});

		Button startmalice = (Button)mainView.findViewById(R.id.start_malice);
		startmalice.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				if (Utils.checkFiles(demoBaseDir + "/qt_malice/", new String[]{"pak0.pak","pak1.pak","pak2.pak","pak3.pak","pak4.pak"})!=null)
				{
					Utils.ExtractAsset(getActivity(),"qt_malice.zip",demoBaseDir);

				}
				else
					startMalice(demoBaseDir);
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

		refreshGames();

		QuakeEngine e = QuakeEngine.valueOf(AppSettings.getStringOption(getActivity(), "last_engine", "DP"));
		selectEngine(e);

		selectGame(0);

		return mainView;
	}
 
	private void selectGame(int pos)
	{
		for (QuakeGameMod g: games)
			g.selected = false;

		selectedMod = games.get(pos);

		games.get(pos).selected = true;

		gameArgsTextView.setText(games.get(pos).getArgs());

		listAdapter.notifyDataSetChanged();
	}

	private void selectEngine(QuakeEngine e){
		engine = e;
		AppSettings.setStringOption(getActivity(), "last_engine", e.toString());


		if (e == QuakeEngine.GL)
		{
			dp_layout.setBackgroundResource(0);
			gl_layout.setBackgroundResource(R.drawable.layout_sel_background);
			fte_layout.setBackgroundResource(0);
		}
		else if (e == QuakeEngine.FTE)
		{
			dp_layout.setBackgroundResource(0);
			gl_layout.setBackgroundResource(0);
			fte_layout.setBackgroundResource(R.drawable.layout_sel_background);
		}               
		else if (e == QuakeEngine.DP)
		{
			dp_layout.setBackgroundResource(R.drawable.layout_sel_background);
			gl_layout.setBackgroundResource(0);
			fte_layout.setBackgroundResource(0);
		}

	}

	void startQuake(String base)
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
			saveArgs(); 

		}

		String args = gameArgsTextView.getText().toString() + " " + argsEditText.getText().toString();

		if (engine== QuakeEngine.GL)
		{
			Intent intent = new Intent(getActivity(), QuakeActivity.class);
			intent.setAction(Intent.ACTION_MAIN);
			intent.addCategory(Intent.CATEGORY_LAUNCHER);
			intent.putExtra("args","-basedir " + base + "/ " + args);

			intent.putExtra("main_qc",base + "/" + GD.qc_fn);
			if ((selectedMod.getDirectory() == null)|| (selectedMod.getDirectory().contentEquals("")))
				intent.putExtra("mod_qc",(String)null);
			else	
			{
				intent.putExtra("mod_qc",base + "/" + selectedMod.getDirectory() + "/" + GD.qc_fn);
				intent.putExtra("mod_dir",base + "/" + selectedMod.getDirectory());
			}

			startActivity(intent);
		}
		else if (engine== QuakeEngine.FTE)
		{     
			Intent intent = new Intent(getActivity(), FTEDroidActivity.class);
			intent.setAction(Intent.ACTION_MAIN);
			intent.addCategory(Intent.CATEGORY_LAUNCHER);

			intent.putExtra("game","Quake");
			intent.putExtra("args","-basedir " + base + "/ " + args);

			intent.putExtra("main_qc",base + "/" + GD.qc_fn);
			if ((selectedMod.getDirectory() == null)|| (selectedMod.getDirectory().contentEquals("")))
				intent.putExtra("mod_qc",(String)null);
			else	
			{
				intent.putExtra("mod_qc",base + "/" + selectedMod.getDirectory() + "/" + GD.qc_fn);
				intent.putExtra("mod_dir",base + "/" + selectedMod.getDirectory());
			}  
			
			startActivity(intent);
		}
		else  if (engine== QuakeEngine.DP)
		{
			Intent intent = new Intent(getActivity(), DP.class);
			intent.setAction(Intent.ACTION_MAIN);
			intent.addCategory(Intent.CATEGORY_LAUNCHER);

			intent.putExtra("game", 0);

			intent.putExtra("args","-basedir " + base + "/ " + args);

			intent.putExtra("main_qc",base + "/" + GD.qc_fn);
			if ((selectedMod.getDirectory() == null)|| (selectedMod.getDirectory().contentEquals("")))
				intent.putExtra("mod_qc",(String)null);
			else	
			{
				intent.putExtra("mod_qc",base + "/" + selectedMod.getDirectory() + "/" + GD.qc_fn);
				intent.putExtra("mod_dir",base + "/" + selectedMod.getDirectory());
			}

			startActivity(intent);

		}
	}
	
	void startMalice(String base)
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

		String args = " +set sv_gameplayfix_droptofloorstartsolid 1";


		Intent intent = new Intent(getActivity(), DP.class);
		intent.setAction(Intent.ACTION_MAIN);
		intent.addCategory(Intent.CATEGORY_LAUNCHER);
		intent.putExtra("args","-basedir " + base + "/ -game qt_malice " + args);
		
		intent.putExtra("main_qc",base + "/" + GD.qc_fn);
		intent.putExtra("mod_qc",base + "/qt_malice/" + GD.qc_fn);
		
		intent.putExtra("game",1);
		
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
			if (GD.DEBUG) Log.d(LOG,"newxbin search history loaded ok");
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

	private void refreshGames()
	{
		if (listAdapter == null) //Otherwise can crash on some devices
			return;
		
		games.clear();
		games.add(new QuakeGameMod("Main Campaign",""));

		File files[] = new File(fullBaseDir).listFiles();

		if (files != null)
		{
			for(File f: files)
			{
				if (f.isDirectory())
				{
					String dir = f.getName().toLowerCase();
					if (!dir.contentEquals("id1") && !dir.contentEquals("fte")  && !dir.contentEquals("qt_malice"))
					{
						QuakeGameMod game = new QuakeGameMod(dir, dir);
						game.setArgs("-game " + dir);
						games.add(game);
					}
				}
			}
		}
		listAdapter.notifyDataSetChanged();
	}


	class GamesListAdapter extends BaseAdapter{

		public GamesListAdapter(Activity context){

		}
		public void add(String string){

		}
		public int getCount() {
			return games.size();
		}

		public Object getItem(int arg0) {
			// TODO Auto-generated method stub
			return null;
		}

		public long getItemId(int arg0) {
			// TODO Auto-generated method stub
			return 0;
		}


		public View getView (int position, View convertView, ViewGroup list)  {
			View view = getActivity().getLayoutInflater().inflate(R.layout.games_listview_item, null);

			ImageView iv = (ImageView)view.findViewById(R.id.imageview);
			QuakeGameMod game = games.get(position);

			if (game.selected)
			{
				iv.setImageResource(R.drawable.icon);
			}
			else
				iv.setImageResource(R.drawable.icon_blank);

			TextView title = (TextView)view.findViewById(R.id.title_textview);

			title.setText(game.getTitle());
			return view;
		}

	}
}
