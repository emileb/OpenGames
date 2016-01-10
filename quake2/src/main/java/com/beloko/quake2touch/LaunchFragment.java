package com.beloko.quake2touch;

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
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.beloko.opengames.AppSettings;
import com.beloko.opengames.GD;
import com.beloko.opengames.ServerAPI;
import com.beloko.opengames.quake2.Quake2;
import com.beloko.opengames.quake2.Quake2Lib;

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
 
	private static final String BASE64_PUBLIC_KEY = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAiGdXOQp9cy4UOXoKIpEP5EKb0xe98GgopZelYcsEMcXTsE9G2wleZglOmEiVXcfb9hjWBrN59R6Ed8t0owoRQ8WcPa86caAtnaKBDyvKy8oPa6Ou+8lid81vANMlJu6E1aEVXG0bvElW4OL666iI6LoORtFwh7lxddFp4f3JrthQ5aOp5/B78T6RoNGD1mYCcOZChDyBqkP4kCWEA/iYmWwZzxGaMjCg9UJqTAwNOXhBeAU2qc87FsxE1bFHJtoDk3BQzrW1QHyeARuEqAMLxeGCwJhcMup9dyKWLDfEy8MbAEqecVwctXwusPA/d+TLbsjSr77Yu47facdq0SGaYQIDAQAB"; 
	    
	TextView gameArgsTextView;
	EditText argsEditText; 
	ListView listview;
             
	GamesListAdapter listAdapter;  

	ArrayList<QuakeGameMod> games = new ArrayList<QuakeGameMod>();
	QuakeGameMod selectedMod;
	  
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
       
		Button startdemo = (Button)mainView.findViewById(R.id.start_demo);
		startdemo.setOnClickListener(new OnClickListener() {
    
			@Override 
			public void onClick(View v) {

				if (!Utils.checkFiles(demoBaseDir + "/baseq2/", true))
				{
					AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
					builder.setMessage("Download Quake 2 Shareware Data (33MB)?")
					.setCancelable(true)
					.setPositiveButton("OK", new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int id) {
							//utils.downloadData("http://www.beloko.com/QUAKE/quake_demo.zip", demoBaseDir);
							ServerAPI.downloadFile(getActivity(), "quake2_demo.zip", demoBaseDir);
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
					startQuake(demoBaseDir);
			}
		});
		Button startfull = (Button)mainView.findViewById(R.id.start_full);
		startfull.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
 
				if (!Utils.checkFiles(fullBaseDir + "/baseq2/" , false))
				{
					AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
					builder.setMessage("Please copy PAK0.PAK from the registered version of Quake 2 in to:\n" + fullBaseDir + "/baseq2")
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
		selectGame(0);
		return mainView;
	}
  

	void startQuake(String base)
	{
		File deleteConfig = new File(base + "/baseq2/config.cfg");
		deleteConfig.delete();

        //Delete any config files in mods
        if (selectedMod.getDirectory() != null) {
            deleteConfig = new File(base + "/" + selectedMod.getDirectory() + "/config.cfg");
            deleteConfig.delete();
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
		Intent intent = new Intent(getActivity(), Quake2.class);
		intent.setAction(Intent.ACTION_MAIN);
		intent.addCategory(Intent.CATEGORY_LAUNCHER);
		
		
		intent.putExtra("main_qc",base + "/" + GD.qc_fn);
		
		intent.putExtra("game_dll",selectedMod.getGameDLL());
		
		intent.putExtra("game","Quake2");
		intent.putExtra("args","+set basedir " + base + "/ "  + args);
		intent.putExtra("args",args + " +set basedir " + base + "/");
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
	
	private void selectGame(int pos)
	{
		QuakeGameMod game =  games.get(pos);
		if (game.getDirectory() != null)
		{
			if (!Utils.checkFiles(fullBaseDir + "/" + game.getDirectory() + "/" , false))
			{
				AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
				builder.setMessage("Please copy PAK0.PAK from the Mission Pack in to:\n" + fullBaseDir + "/"  + game.getDirectory())
				.setCancelable(true)
				.setPositiveButton("OK", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {

					}
				});

				AlertDialog alert = builder.create();
				alert.show();
				return;
			} 
		}
		
		for (QuakeGameMod g: games)
			g.selected = false;

		selectedMod = games.get(pos);
		
		games.get(pos).selected = true;

		gameArgsTextView.setText(games.get(pos).getArgs());
		
		listAdapter.notifyDataSetChanged();
	}
	
	private void refreshGames()
	{
		games.clear();
		
		QuakeGameMod game = new QuakeGameMod("Main Campaign",null);
		game.setImage(R.drawable.quake2);
		game.setGameDLL(Quake2Lib.Q2DLL_GAME);
		games.add(game);

		game = new QuakeGameMod("The Reckoning","xatrix");
		game.setImage(R.drawable.the_reckoning);
		game.setGameDLL(Quake2Lib.Q2DLL_XATRIX);
		game.setArgs("+set game xatrix");
		games.add(game);
		
		game = new QuakeGameMod("Ground Zero","rogue");
		game.setImage(R.drawable.ground_zero);
		game.setGameDLL(Quake2Lib.Q2DLL_ROGUE);
		game.setArgs("+set game rogue");
		games.add(game); 
		
		game = new QuakeGameMod("Capture The Flag","ctf");
		game.setImage(R.drawable.ctf);
		game.setGameDLL(Quake2Lib.Q2DLL_CTF);    
		game.setArgs("+set game ctf");
		games.add(game);
		
		
		
		//game = new QuakeGameMod("CRBOT",null);
		//game.setImage(R.drawable.ctf);
		//game.setGameDLL(Quake2Lib.Q2DLL_CRBOT);
		//game.setArgs("+set game ctf");
		//games.add(game);
		
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
				view.setBackgroundResource(R.drawable.layout_sel_background);
			}

            //Only show image if game is installed, otherwise show icon
            if (Utils.checkFiles(fullBaseDir + "/" + game.getDirectory() + "/" , false))
			    iv.setImageResource(game.getImage());
            else
                iv.setImageResource(R.drawable.ic_launcher);

			TextView title = (TextView)view.findViewById(R.id.title_textview);

			title.setText(game.getTitle());
			return view;
		}

	}
}
