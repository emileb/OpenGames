
package com.beloko.opengames.jk3mp;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
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
import com.beloko.opengames.BestEglChooser;
import com.beloko.opengames.CDAudioPlayer;
import com.beloko.opengames.FPSLimit;
import com.beloko.opengames.GD;
import com.beloko.opengames.Utils;
import com.beloko.touchcontrols.ControlInterpreter;
import com.beloko.touchcontrols.CustomCommands;
import com.beloko.touchcontrols.MogaHack;
import com.beloko.touchcontrols.ShowKeyboard;
import com.beloko.touchcontrols.TouchControlsSettings;
import com.beloko.touchcontrols.TouchSettings;

import org.libsdl.app.SDLActivity;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class Game extends Activity 
implements Handler.Callback
{
	String LOG = "Quake3";

	private ControlInterpreter controlInterp;

	private final MogaControllerListener mogaListener = new MogaControllerListener();
	Controller mogaController = null;

	private String args;
	private String gamePath;

	private QuakeView mGLSurfaceView = null;
	private QuakeRenderer mRenderer = new QuakeRenderer();
	Activity act;

	int surfaceWidth,surfaceHeight;


	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);           

		act = this;
		
		AppSettings.setGame(GD.IDGame.JK3);
		AppSettings.reloadSettings(getApplication());

		args = getIntent().getStringExtra("args");
		gamePath  = getIntent().getStringExtra("game_path");

		handlerUI  = new Handler(this);         

		mogaController = Controller.getInstance(this);
		MogaHack.init(mogaController, this);
		mogaController.setListener(mogaListener,new Handler());

		//Log.i( LOG, "version : " + getVersion());

		// fullscreen
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);

		// keep screen on 
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		
		Utils.setImmersionMode(this);

		GD.init(getApplicationContext());
	
		String s = "Please stop hacking this and posting online. It has a MASSIVE effect on sales which means I can not afford to port more games. It costs less than $2 on Google Play!!! Thank you.";
		Log.i(LOG,s);

		
		start_quake2();   
	}


	
	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		Utils.onWindowFocusChanged(this, hasFocus);
	}
	
	/// Handler for asynchronous message
	/// => showDialog

	private Handler handlerUI ;

	public static final int MSG_SHOW_DIALOG = 1;


	// implements Handler.Callback
	@Override
	public boolean handleMessage(Message msg) {

		Log.i( LOG, String.format("handleMessage %d %d", msg.what, msg.arg1));

		switch( msg.what ){

		case MSG_SHOW_DIALOG:
			showDialog(msg.arg1);
			break;

		}

		return true;

	}


	/////////////////////////////




	public void start_quake2() {


		NativeLib engine = new NativeLib();

		NativeLib.loadLibraries(false);


		controlInterp = new ControlInterpreter(engine,Utils.getGameGamepadConfig(AppSettings.game), TouchSettings.gamePadControlsFile, TouchSettings.gamePadEnabled);

		TouchControlsSettings.setup(act, engine);
		TouchControlsSettings.loadSettings(act);
		TouchControlsSettings.sendToQuake();

		CustomCommands.setup(act, engine, getIntent().getStringExtra("main_qc"), getIntent().getStringExtra("mod_qc"));


		// Create our Preview view and set it as the content of our
		// Activity
		mGLSurfaceView = new QuakeView(this);


		mGLSurfaceView.setEGLConfigChooser( new BestEglChooser(getApplicationContext()) );

		mGLSurfaceView.setRenderer(mRenderer);

		// This will keep the screen on, while your view is visible. 
		mGLSurfaceView.setKeepScreenOn(true);

		ShowKeyboard.setup(act, mGLSurfaceView);

		setContentView(mGLSurfaceView);
		mGLSurfaceView.requestFocus();
		mGLSurfaceView.setFocusableInTouchMode(true);

	}


	@Override
	protected void onPause() {
		Log.i( "Quake2.java", "onPause" );
		CDAudioPlayer.onPause();

		mogaController.onPause();
		super.onPause();
	}

	@Override
	protected void onResume() {

		Log.i( "Quake2.java", "onResume" );
		CDAudioPlayer.onResume();

		mogaController.onResume();

		super.onResume();
		mGLSurfaceView.onResume();

	}

	@Override
	protected void onRestart() {
		Log.i( "Quake2.java", "onRestart" );
		super.onRestart();
	}




	@Override
	protected void onStop() {
		Log.i( "Quake2.java", "onStop" );
		super.onStop();	
	}

	@Override
	protected void onDestroy() {
		Log.i( "Quake2.java", "onDestroy" ); 
		super.onDestroy();
		mogaController.exit();
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
			// TODO Auto-generated method stub
			Log.d(LOG,"onGenericMotionEvent " + event.toString());
			controlInterp.onGenericMotionEvent(event);
		}

		@Override
		public void onStateEvent(StateEvent event) {
			Log.d(LOG,"onStateEvent " + event.getState());
		}
	}

	class QuakeView extends GLSurfaceView {

		/*--------------------
		 * Event handling
		 *--------------------*/


		public QuakeView(Context context) {
			super(context);

		}

		//@Override
		//public boolean dispatchTouchEvent(MotionEvent event) {
		//return controlInterp.onTouchEvent(event);
		//}

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
			Log.d(LOG,"onKeyDown " + keyCode);
			//SDLLib.onNativeKeyDown(keyCode);
			return controlInterp.onKeyDown(keyCode, event);
		}

		@Override
		public boolean onKeyUp(int keyCode, KeyEvent event)
		{
			//SDLLib.onNativeKeyUp(keyCode);
			return controlInterp.onKeyUp(keyCode, event);
		} 

	}  // end of QuakeView




	///////////// GLSurfaceView.Renderer implementation ///////////

	class QuakeRenderer implements GLSurfaceView.Renderer {



		public void onSurfaceCreated(GL10 gl, EGLConfig config) {

			Log.d("Renderer", "onSurfaceCreated");

		}


		private void init( int width, int height ){

			Log.i( LOG, "screen size : " + width + "x"+ height);
		
			NativeLib.setScreenSize(width,height);

			Utils.copyPNGAssets(getApplicationContext(),AppSettings.graphicsDir);   

			fpsLimit = new FPSLimit(getApplicationContext());

			Log.i(LOG, "Quake2Init start");

			String[] args_array = Utils.creatArgs(args);
			String lsfn = "DLFlibs";
			lsfn += "-setup.zip";
			//if (!AppSettings.getBoolOption(act, lsfn, false))
			//	gamePath = null;
			
			int ret = NativeLib.init(act,AppSettings.graphicsDir,64,args_array,0,gamePath,getApplicationInfo().nativeLibraryDir);

			Log.i(LOG, "Quake2Init done");

		}



		//// new Renderer interface
		int notifiedflags;

		FPSLimit fpsLimit;

		boolean inited = false;

		private int counter_fps=0;
		private long tprint_fps= 0;

		public void onDrawFrame(GL10 gl) {



			//Log.d(LOG,"onDrawFrame");
			long tnow = SystemClock.uptimeMillis(); 


			// compute FPS
			if ( (tnow-tprint_fps) >= 1000){

				//Log.i(LOG,String.format( "FPS= %d",counter_fps));

				tprint_fps = tnow;
				counter_fps = 0;        	
			}
			counter_fps ++;

			if (!inited)
			{
				SDLActivity.nativeInit();
				
				AppSettings.setIntOption(getApplicationContext(), "max_fps", 0);  

				inited = true;
				init( surfaceWidth, surfaceHeight );
			}

			fpsLimit.tick();

			int flags =  NativeLib.frame();

			if (flags != notifiedflags)
			{
				if (((flags ^ notifiedflags) & 1) != 0)
				{
					Log.d(LOG,"show keyboard");
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
									im.showSoftInput(mGLSurfaceView, 0);//InputMethodManager.SHOW_FORCED);
								}
								else
								{
									//								getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);
									//im.hideSoftInputFromWindow(mGLSurfaceView.getWindowToken(), 0);
								}
							}
							else
								android.util.Log.i("FTEDroid", "IMM failed");
						}
					};
					act.runOnUiThread(r);
				}
				else if (((flags ^ notifiedflags) & 128) != 0)
				{
					Log.d(LOG,"QUIT");
					finish();
				}
				notifiedflags = flags;
			}

		}




		public void onSurfaceChanged(GL10 gl, int width, int height) {
			Log.d("Renderer", String.format("onSurfaceChanged %dx%d", width,height) );

			controlInterp.setScreenSize(width, height);

			surfaceWidth = width;
			surfaceHeight = height;

		}


	} // end of QuakeRenderer


}


