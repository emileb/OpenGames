package com.beloko.idtech;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Build;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.animation.Animation;
import android.view.animation.Transformation;
import android.widget.Toast;

public class Utils {
	static String LOG = "Utils";

	static public void copyFile(InputStream in, OutputStream out) throws IOException {
		byte[] buffer = new byte[1024];
		int read;
		while((read = in.read(buffer)) != -1){
			out.write(buffer, 0, read);
		}
		out.close(); 
	}

	static public void copyFile(InputStream in, OutputStream out,ProgressDialog pb) throws IOException {
		byte[] buffer = new byte[1024];
		int read;
		while((read = in.read(buffer)) != -1){
			out.write(buffer, 0, read);
			pb.setProgress(pb.getProgress() + 1024);
		}
		out.close(); 
	}
	/*static public  void showDownloadDialog(final Activity act,String title,final String KEY,final String directory,final String file)
	{
		AlertDialog.Builder builder = new AlertDialog.Builder(act);
		builder.setMessage(title)
		.setCancelable(true)
		.setPositiveButton("OK", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				ServerAPI.downloadFileSecure(act,KEY,file,directory);
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
	}*/
	
	public static String checkFiles(String basePath,String[] files_to_ceck)
	{
		File[] files = new File(basePath ).listFiles();
		boolean ok=true;

		String filesNotFound = "";

		String[] expected;
		expected = files_to_ceck;

		if (files == null)
			files = new File[0];

		if (files!=null)
		{
			for (File f: files)
			{
				Log.d(LOG,"FILES: " + f.toString());

			}

			for (String e: expected)
			{
				boolean found=false;
				for (File f: files)
				{
					if (f.toString().toLowerCase().endsWith(e.toLowerCase()))
						found = true;
				}
				if (!found)
				{
					Log.d(LOG,"Didnt find " + e);
					filesNotFound +=  e + "\n";
					ok = false;
				}
			}
		}

		if (filesNotFound.contentEquals(""))
			return null;
		else
			return filesNotFound;

	}
	static public void copyPNGAssets(Context ctx,String dir) {
		copyPNGAssets(ctx,dir,"");
	}

	static public void copyPNGAssets(Context ctx,String dir,String prefix) {

		if (prefix == null)
			prefix = "";

		File d = new File(dir);
		if (!d.exists())
			d.mkdirs();

		AssetManager assetManager = ctx.getAssets();
		String[] files = null;
		try {
			files = assetManager.list("");
		} catch (IOException e) {
			Log.e("tag", "Failed to get asset file list.", e);
		}
		for(String filename : files) {
			if (filename.endsWith("png") && filename.startsWith(prefix)){
				InputStream in = null;
				OutputStream out = null;
				//Log.d("test","file = " + filename);
				try {
					in = assetManager.open(filename);
					out = new FileOutputStream(dir + "/" + filename.substring(prefix.length()));
					copyFile(in, out);
					in.close();
					in = null;
					out.flush();
					out.close();
					out = null;
				} catch(IOException e) {
					Log.e("tag", "Failed to copy asset file: " + filename, e);
				}       
			}
		}
	}

	static public String[] creatArgs(String appArgs)
	{
		ArrayList<String> a = new ArrayList<String>(Arrays.asList(appArgs.split(" ")));

		Iterator<String> iter = a.iterator();
		while (iter.hasNext()) {
			if (iter.next().contentEquals("")) {
				iter.remove();
			}
		}

		return a.toArray(new String[a.size()]);
	}


	public static void expand(final View v) {
		v.measure(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
		final int targtetHeight = v.getMeasuredHeight();

		v.getLayoutParams().height = 0;
		v.setVisibility(View.VISIBLE);
		Animation a = new Animation()
		{
			@Override
			protected void applyTransformation(float interpolatedTime, Transformation t) {
				v.getLayoutParams().height = interpolatedTime == 1
						? LayoutParams.WRAP_CONTENT
								: (int)(targtetHeight * interpolatedTime);
				v.requestLayout();
			}

			@Override
			public boolean willChangeBounds() {
				return true;
			}
		};

		// 1dp/ms
		a.setDuration((int)(targtetHeight / v.getContext().getResources().getDisplayMetrics().density));
		v.startAnimation(a);
	}

	public static void collapse(final View v) {
		final int initialHeight = v.getMeasuredHeight();

		Animation a = new Animation()
		{
			@Override
			protected void applyTransformation(float interpolatedTime, Transformation t) {
				if(interpolatedTime == 1){
					v.setVisibility(View.GONE);
				}else{
					v.getLayoutParams().height = initialHeight - (int)(initialHeight * interpolatedTime);
					v.requestLayout();
				}
			}

			@Override
			public boolean willChangeBounds() {
				return true;
			}
		};

		// 1dp/ms
		a.setDuration((int)(initialHeight / v.getContext().getResources().getDisplayMetrics().density));
		v.startAnimation(a);
	}


	static final int BUFFER_SIZE = 1024;

	static public String getLogCat() {
		String[] logcatArgs = new String[] {"logcat", "-d", "-v", "time"};

		Process logcatProc = null;
		try {
			logcatProc = Runtime.getRuntime().exec(logcatArgs);
		}
		catch (IOException e) {
			return null;
		}

		BufferedReader reader = null;
		String response = null;
		try {
			String separator = System.getProperty("line.separator");
			StringBuilder sb = new StringBuilder();
			reader = new BufferedReader(new InputStreamReader(logcatProc.getInputStream()), BUFFER_SIZE);
			String line;
			while ((line = reader.readLine()) != null) {
				sb.append(line);
				sb.append(separator);
			}
			response = sb.toString();
		}
		catch (IOException e) {
		}
		finally {
			if (reader != null) {
				try {
					reader.close();
				}
				catch (IOException e) {}
			}
		}

		return response;
	}


	static public void copyAsset(Context ctx,String file,String destdir) {
		AssetManager assetManager = ctx.getAssets();

		InputStream in = null;
		OutputStream out = null;

		try {
			in = assetManager.open(file);
			out = new FileOutputStream(destdir + "/" + file);
			copyFile(in, out);
			in.close();
			in = null;
			out.flush();
			out.close();
			out = null;
		} catch(IOException e) {
			Log.e("tag", "Failed to copy asset file: " + file);
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

	public static void loadArgs(Context ctx,ArrayList<String> args)                         
	{ 
		File cacheDir = ctx.getFilesDir();

		FileInputStream fis = null;
		ObjectInputStream in = null;
		try
		{
			fis = new FileInputStream(new File(cacheDir,"args_hist.dat"));
			in = new ObjectInputStream(fis);                
			ArrayList<String> argsHistory = (ArrayList<String>)in.readObject();
			args.clear();
			args.addAll(argsHistory);
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
		args.clear();
	}  


	public static void saveArgs(Context ctx,ArrayList<String> args)
	{
		File cacheDir = ctx.getFilesDir();

		if (!cacheDir.exists())
			cacheDir.mkdirs();

		FileOutputStream fos = null;
		ObjectOutputStream out = null;
		try
		{
			fos = new FileOutputStream(new File(cacheDir,"args_hist.dat"));
			out = new ObjectOutputStream(fos);
			out.writeObject(args);
			out.close();
		}
		catch(IOException ex)         
		{
			Toast.makeText(ctx,"Error saving args History list: " + ex.toString(), Toast.LENGTH_LONG).show();
		}
	}              

	public static void copyTimidityFile(Activity act)
	{
		File timiditycfg = new File(AppSettings.getBaseDir() + "/eawpats/timidity.cfg"  );
		File doomtimiditycfg = new File(AppSettings.getGameDir() + "/timidity.cfg"  );

		if (timiditycfg.exists() && !doomtimiditycfg.exists())
		{
			Log.d(LOG,"Copying timidity file");
			try {
				Utils.copyFile(new FileInputStream(timiditycfg),new FileOutputStream(doomtimiditycfg));
			} catch (FileNotFoundException e) {
				Toast.makeText(act,"Error copying timidity.cfg " + e.toString(), Toast.LENGTH_LONG).show();
			} catch (IOException e) {
				Toast.makeText(act,"Error copying timidity.cfg " + e.toString(), Toast.LENGTH_LONG).show();
			}
		}
	}


	public static void setImmersionMode(final Activity act)
	{
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {

			if (AppSettings.immersionMode)
			{
				act.getWindow().getDecorView().setSystemUiVisibility(
						View.SYSTEM_UI_FLAG_LAYOUT_STABLE
						| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
						| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
						| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
						| View.SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
						| View.SYSTEM_UI_FLAG_IMMERSIVE
						| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY );

				View decorView = act.getWindow().getDecorView();
				decorView.setOnSystemUiVisibilityChangeListener
				(new View.OnSystemUiVisibilityChangeListener() {
					@Override
					public void onSystemUiVisibilityChange(int visibility) {
						Log.d(LOG,"onSystemUiVisibilityChange");

						act.getWindow().getDecorView().setSystemUiVisibility(
								View.SYSTEM_UI_FLAG_LAYOUT_STABLE
								| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
								| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
								| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
								| View.SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
								| View.SYSTEM_UI_FLAG_IMMERSIVE
								| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY );

					}
				});
			}
		}
	}

	public static void onWindowFocusChanged(final Activity act,final boolean hasFocus)
	{
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {

			if (AppSettings.immersionMode)
			{
				Handler handler = new Handler();

				handler.postDelayed(new Runnable() {
					public void run() {

						if (hasFocus) {
							act.getWindow().getDecorView().setSystemUiVisibility(
									View.SYSTEM_UI_FLAG_LAYOUT_STABLE
									| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
									| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
									| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
									| View.SYSTEM_UI_FLAG_FULLSCREEN
									| View.SYSTEM_UI_FLAG_IMMERSIVE
									| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);}
					}

				}, 2000);
			}
		}
	}
}
