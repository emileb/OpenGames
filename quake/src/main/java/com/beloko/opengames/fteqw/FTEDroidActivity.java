package com.beloko.opengames.fteqw;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Vibrator;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;

import com.bda.controller.Controller;
import com.bda.controller.ControllerListener;
import com.bda.controller.StateEvent;
import com.beloko.opengames.AppSettings;
import com.beloko.opengames.CDAudioPlayer;
import com.beloko.opengames.FPSLimit;
import com.beloko.opengames.GD;
import com.beloko.opengames.Utils;
import com.beloko.touchcontrols.ControlInterpreter;
import com.beloko.touchcontrols.CustomCommands;
import com.beloko.touchcontrols.MogaHack;
import com.beloko.touchcontrols.TouchControlsSettings;
import com.beloko.touchcontrols.TouchSettings;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class FTEDroidActivity extends Activity
{

	private FTEView view;

	//Activity act;

	int screenWidth, screenHeight;
	private String args;

	private boolean QUAKE_QUIT = false;

	private ControlInterpreter controlInterp;

	private final MogaControllerListener mogaListener = new MogaControllerListener();
	Controller mogaController = null;
	
	private class FTERenderer implements GLSurfaceView.Renderer 
	{
		private boolean inited;
		public int glesversion;
		FTEDroidActivity act;
		FTEView theview;

		FPSLimit fpsLimit;
		
		int notifiedflags;

		void updateGLESVersion()
		{
			if (FTEDroidEngine.getpreferedglesversion() < 2)
			{
				android.util.Log.i("FTEDroid", "Using GLES1");
				this.glesversion = 1;
			}
		}

		FTERenderer(FTEView view, FTEDroidActivity parent)
		{
			act = parent;
			theview = view;
			fpsLimit = new FPSLimit(getApplicationContext());
			updateGLESVersion();
		}

		boolean threadCatcher = false;
		
		@Override
		public void onDrawFrame(GL10 gl) 
		{
			if (inited == true)
			{
				fpsLimit.tick();
				
				int flags;
				flags = FTEDroidEngine.frame(0,0,0);
				if (flags != notifiedflags)
				{
					if (((flags ^ notifiedflags) & 1) != 0)
					{
						final int fl = flags;
						Runnable r = new Runnable() 
						{	//doing this on the ui thread because android sucks.
							public void run()
							{
								InputMethodManager im = (InputMethodManager) act.getSystemService(Context.INPUT_METHOD_SERVICE);
								if (im != null)
								{
									if ((fl & 1) != 0)
									{
										//								getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
										im.showSoftInput(theview, 0);//InputMethodManager.SHOW_FORCED);
									}
									else
									{
										//								getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);
										im.hideSoftInputFromWindow(theview.getWindowToken(), 0);
									}
								}
								else
									android.util.Log.i("FTEDroid", "IMM failed");
							}
						};
						act.runOnUiThread(r);
					}
					if (((flags ^ notifiedflags) & 2) != 0)
					{
						int dur = FTEDroidEngine.getvibrateduration();
						flags &= ~2;
						if (AppSettings.vibrate)
						{
							Vibrator vib = (Vibrator) act.getSystemService(Context.VIBRATOR_SERVICE);
							if (vib != null)
							{
								android.util.Log.i("FTEDroid", "Vibrate " + dur + "ms");
								vib.vibrate(dur);
							}
							else
								android.util.Log.i("FTEDroid", "No vibrator device");
						}
					}
					if (((flags ^ notifiedflags) & 4) != 0)
					{
						final int fl = flags;
						Runnable r = new Runnable() 
						{
							public void run()
							{
								if ((fl & 4) != 0)
									act.getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
								else
									act.getWindow().setFlags(0, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
							}
						};
						act.runOnUiThread(r);
					}
					if (((flags ^ notifiedflags) & 8) != 0)
					{
						final String errormsg = FTEDroidEngine.geterrormessage();

						inited = false;

						if (errormsg == "")
						{
							finish();
							System.exit(0);
						}

						//8 means sys error
						Runnable r = new Runnable() 
						{
							public void run()
							{
								theview.setVisibility(theview.GONE);
								AlertDialog ad = new AlertDialog.Builder(act).create();
								ad.setTitle("FTE ERROR");
								ad.setMessage(errormsg);
								ad.setCancelable(false);
								ad.setButton("Ok", new DialogInterface.OnClickListener()
								{
									public void onClick(DialogInterface dialog, int which)
									{
										finish();
										System.exit(0);
									}
								}
										);
								ad.show();
							}
						};
						act.runOnUiThread(r);
					}
					if (((flags ^ notifiedflags) & 16) != 0)//16 means orientation cvar change	
					{								
						//NOT USED
					}
					if (((flags ^ notifiedflags) & 32) != 0)
					{
						if ((flags & 32) != 0)
							view.audioInit(FTEDroidEngine.audioinfo(0), FTEDroidEngine.audioinfo(1), FTEDroidEngine.audioinfo(2));
						else
							view.audioStop();
					}
					if (((flags ^ notifiedflags) & 128) != 0)
					{
						QUAKE_QUIT = true;
						view.audioStop();
						finish();
					}
					//clear anything which is an impulse
					notifiedflags = flags;
				}
			}        
		} 

		@Override
		public void onSurfaceChanged(GL10 gl, int width, int height)
		{
			android.util.Log.i("FTEDroid", "Surface changed, now " + width + " by " + height + ".");
			screenWidth = width;   
			screenHeight = height;
			controlInterp.setScreenSize(screenWidth, screenHeight);

			
			String[] args_array = Utils.creatArgs(args);

			Log.d("FTE","args = " + args);

			FTEDroidEngine.init(width, height, glesversion,AppSettings.graphicsDir,0,64,args_array);
			inited = true;                   
		}
		@Override
		public void onSurfaceCreated(GL10 gl, EGLConfig config)
		{
			FTEDroidEngine.newglcontext();
		}
	}

	private class FTEView extends GLSurfaceView
	{
		private final FTERenderer rndr;

		private audiothreadclass audiothread;


		private class audiothreadclass extends Thread
		{
			boolean timetodie;
			int schannels;
			int sspeed;
			int sbits;
			@Override
			public void run()
			{
				byte[] audbuf = new byte[2048];
				int avail;
				AudioTrack at;

				int chans;
				try
				{
					if (schannels >= 8)	//the OUT enumeration allows specific speaker control. but also api level 5+
						chans = AudioFormat.CHANNEL_OUT_7POINT1;
					else if (schannels >= 6)
						chans = AudioFormat.CHANNEL_OUT_5POINT1;
					else if (schannels >= 4)
						chans = AudioFormat.CHANNEL_OUT_QUAD;
					else if (schannels >= 2)
						chans = AudioFormat.CHANNEL_CONFIGURATION_STEREO;
					else
						chans = AudioFormat.CHANNEL_CONFIGURATION_MONO;
					int enc = (sbits == 8)?AudioFormat.ENCODING_PCM_8BIT:AudioFormat.ENCODING_PCM_16BIT;

					int sz = 2*AudioTrack.getMinBufferSize(sspeed, chans, enc);

					//				if (sz < sspeed * 0.05)
					//					sz = sspeed * 0.05;

					at = new AudioTrack(AudioManager.STREAM_MUSIC, sspeed, chans, enc, sz, AudioTrack.MODE_STREAM);
				}
				catch(IllegalArgumentException e)
				{
					//fixme: tell the engine that its bad and that it should configure some different audio attributes, instead of simply muting.
					return;
				}

				at.setStereoVolume(1, 1);
				at.play();

				while(!timetodie)
				{
					avail = FTEDroidEngine.paintaudio(audbuf, audbuf.length);
					at.write(audbuf, 0, avail);
				}

				at.stop();
			}
			public void killoff()
			{
				timetodie = true;
				try
				{
					join();
				}
				catch(InterruptedException e)
				{
				}
				timetodie = false;
			}
		};

		private void audioInit(int sspeed, int schannels, int sbits)
		{
			if (audiothread == null)
			{
				audiothread = new audiothreadclass();
				audiothread.schannels = schannels;
				audiothread.sspeed = sspeed;
				audiothread.sbits = sbits;
				audiothread.start();
			}
		}
		public void audioStop()
		{
			if (audiothread != null)
			{
				audiothread.killoff();
				audiothread = null;
			}
		}
		public void audioResume()
		{
			if (audiothread != null)
			{
				audiothread.killoff();
				audiothread.start();
			}
		}


		public FTEView(FTEDroidActivity context)
		{
			super(context);

			FTEDroidEngine.preInit();   
			//Create engine, just used to link to the control interpreter
			FTEDroidEngine engine = new FTEDroidEngine();
			controlInterp = new ControlInterpreter(engine,Utils.getGameGamepadConfig(AppSettings.game), TouchSettings.gamePadControlsFile, TouchSettings.gamePadEnabled);

			mogaController = Controller.getInstance(getApplicationContext());
			MogaHack.init(mogaController,getContext());
			mogaController.setListener(mogaListener, new Handler());
			
			
			TouchControlsSettings.setup(context, engine);
			TouchControlsSettings.loadSettings(context);
			TouchControlsSettings.sendToQuake();

			CustomCommands.setup(context, engine, getIntent().getStringExtra("main_qc"), getIntent().getStringExtra("mod_qc"));
			
			rndr = new FTERenderer(this, context);
			setRenderer(rndr);
			setFocusable(true);
			setFocusableInTouchMode(true);
		}

		@Override
		public boolean onGenericMotionEvent(MotionEvent event) {
			return controlInterp.onGenericMotionEvent(event);
		}
		@Override
		public boolean onTouchEvent(MotionEvent event)
		{
			return controlInterp.onTouchEvent(event);
		}

		@Override
		public boolean onKeyDown(int keyCode, KeyEvent event)
		{
			return controlInterp.onKeyDown(keyCode, event);
		}

		@Override
		public boolean onKeyUp(int keyCode, KeyEvent event)
		{
			return controlInterp.onKeyUp(keyCode, event);
		} 

	}

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		android.util.Log.i("FTEDroid", "onCreate");

		GD.IDGame game = GD.IDGame.valueOf(getIntent().getStringExtra("game"));

		AppSettings.setGame(game);
		
		AppSettings.reloadSettings(getApplication());

		if (CDAudioPlayer.checkHasAudioFiles(getIntent().getStringExtra("mod_dir")))
			CDAudioPlayer.initFiles(getIntent().getStringExtra("mod_dir"));
		
		Utils.copyPNGAssets(this,AppSettings.graphicsDir);   


		args = getIntent().getStringExtra("args");


		//go full-screen		
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);    	
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		super.onCreate(savedInstanceState);
		android.util.Log.i("FTEDroid", "create view");
		view = new FTEView(this);
		setContentView(view);
		
		Utils.setImmersionMode(this);

		android.util.Log.i("FTEDroid", "done");
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		Utils.onWindowFocusChanged(this, hasFocus);
	}
	 
	
	@Override
	protected void onResume()
	{
		super.onResume();
		CDAudioPlayer.onResume();
		mogaController.onResume();
		view.audioResume();
	}

	@Override
	protected void onStop()
	{
		view.audioStop();
		super.onStop();
	}

	@Override
	protected void onPause()
	{
		view.audioStop();
		CDAudioPlayer.onPause();

		
		super.onPause();
	}

	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		mogaController.exit();
		//Wait for any config files to be written
		try {
			Thread.sleep(500);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		System.exit(0);
	}
	
	class MogaControllerListener implements ControllerListener {


		@Override
		public void onKeyEvent(com.bda.controller.KeyEvent event) {
			//Log.d(LOG,"onKeyEvent " + event.getKeyCode());
			controlInterp.onMogaKeyEvent(event,mogaController.getState(Controller.STATE_CURRENT_PRODUCT_VERSION));
		}

		@Override
		public void onMotionEvent(com.bda.controller.MotionEvent event) {
			controlInterp.onGenericMotionEvent(event);
		}

		@Override
		public void onStateEvent(StateEvent event) {
			
		}
	}

}
