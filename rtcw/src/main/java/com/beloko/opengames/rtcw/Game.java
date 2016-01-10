/*
 * Copyright (C) 2009 jeyries@yahoo.fr
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.beloko.opengames.rtcw;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;

import com.bda.controller.Controller;
import com.bda.controller.ControllerListener;
import com.bda.controller.StateEvent;
import com.beloko.libsdl.SDLLib;
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

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class Game extends Activity
implements Handler.Callback
{
	String LOG = "RTCW";

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

		AppSettings.setGame(GD.IDGame.RTCW);
		AppSettings.reloadSettings(getApplication());

		args = getIntent().getStringExtra("args");
		gamePath  = getIntent().getStringExtra("game_path");

		handlerUI  = new Handler(this);         

		mogaController = Controller.getInstance(this);
		MogaHack.init(mogaController,this);
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
	
		start_game();   
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




	public void start_game() {

	
		boolean demo = getIntent().getBooleanExtra("demo",false);

		NativeLib engine = new NativeLib();
		
		NativeLib.loadLibraries(demo);


        controlInterp = new ControlInterpreter(engine,Utils.getGameGamepadConfig(AppSettings.game), TouchSettings.gamePadControlsFile, TouchSettings.gamePadEnabled);

        TouchControlsSettings.setup(act, engine);
		TouchControlsSettings.loadSettings(act);
		TouchControlsSettings.sendToQuake();

		CustomCommands.setup(act, engine, getIntent().getStringExtra("main_qc"), getIntent().getStringExtra("mod_qc"));


		// Create our Preview view and set it as the content of our
		// Activity
		mGLSurfaceView = new QuakeView(this);


		//mGLSurfaceView.setEGLConfigChooser( new QuakeEGLConfigChooser() );
		mGLSurfaceView.setEGLConfigChooser(new BestEglChooser(getApplicationContext()));
		mGLSurfaceView.setRenderer(mRenderer);

		// This will keep the screen on, while your view is visible. 
		mGLSurfaceView.setKeepScreenOn(true);

        ShowKeyboard.setup(act, mGLSurfaceView);

        setContentView(mGLSurfaceView);
		mGLSurfaceView.requestFocus();
		mGLSurfaceView.setFocusableInTouchMode(true);

	}

		boolean audioIsPaused = false;
	@Override
	protected void onPause() {
		Log.i( LOG, "onPause" );
		CDAudioPlayer.onPause();
		SDLLib.onPause();
		mogaController.onPause();
		super.onPause();
		
		if (!audioIsPaused)
		{
			NativeLib.pauseAudio(1);
			audioIsPaused = true;
		}
	}

	@Override
	protected void onResume() {

		Log.i( LOG, "onResume" );
		CDAudioPlayer.onResume();
		SDLLib.onResume();
		mogaController.onResume();

		super.onResume();
		mGLSurfaceView.onResume();

		if (audioIsPaused)
		{
			NativeLib.pauseAudio(0);
			audioIsPaused = false;
		}
	}

	@Override
	protected void onRestart() {
		Log.i( LOG, "onRestart" );
		super.onRestart();
	}




	@Override
	protected void onStop() {
		Log.i( LOG, "onStop" );
		super.onStop();	
	}

	@Override
	protected void onDestroy() {
		Log.i( LOG, "onDestroy" ); 
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
		//	 controlInterp.onTouchEvent(event);
		//	return false;
		//}

		@Override
		public boolean onGenericMotionEvent(MotionEvent event) {
			return controlInterp.onGenericMotionEvent(event);
		}
		
		@Override
		public boolean onTouchEvent(MotionEvent event)
		{
			controlInterp.onTouchEvent(event);
			return true;
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

			int ret = NativeLib.init(AppSettings.graphicsDir,64,args_array,0,gamePath,getApplicationInfo().nativeLibraryDir);

			Log.i(LOG, "Quake2Init done");

		}



		//// new Renderer interface
		int notifiedflags;

		FPSLimit fpsLimit;

		boolean inited = false;

		public void onDrawFrame(GL10 gl) {



			//Log.d(LOG,"onDrawFrame");

			if (!inited)
			{
				AppSettings.setIntOption(getApplicationContext(), "max_fps", 0);  
				 
				Thread.currentThread().setPriority(Thread.MAX_PRIORITY);
				 
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


