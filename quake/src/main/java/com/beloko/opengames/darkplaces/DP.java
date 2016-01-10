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
package com.beloko.opengames.darkplaces;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;

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
import com.beloko.touchcontrols.ShowKeyboard;
import com.beloko.touchcontrols.TouchControlsSettings;
import com.beloko.touchcontrols.TouchSettings;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class DP extends Activity 
{
	String LOG = "DP";

	private ControlInterpreter controlInterp;

	private final MogaControllerListener mogaListener = new MogaControllerListener();
	Controller mogaController = null;

	private String args;


	private QuakeView mGLSurfaceView = null;
	private QuakeRenderer mRenderer = new QuakeRenderer();
	Activity act;

	int surfaceWidth,surfaceHeight;


	private String error_message;
	private int overlay = 0;

	public static final String version = "1.91" ;


	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);           

		Log.i( "Quake.java", "onCreate " + version);

		act = this;

		AppSettings.setGame(GD.IDGame.Quake);//Always Quake
		AppSettings.reloadSettings(getApplication());

		if (CDAudioPlayer.checkHasAudioFiles(getIntent().getStringExtra("mod_dir")))
			CDAudioPlayer.initFiles(getIntent().getStringExtra("mod_dir"));
				
		args = getIntent().getStringExtra("args");

	
		mogaController = Controller.getInstance(this);
		MogaHack.init(mogaController,this);
		mogaController.setListener(mogaListener,new Handler());

		//Log.i( "Quake2", "version : " + getVersion());

		// fullscreen
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);

		// keep screen on 
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		/*
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {

			getWindow().getDecorView().setSystemUiVisibility(
					View.SYSTEM_UI_FLAG_LAYOUT_STABLE
					| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
					| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
					| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
					| View.SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
					| View.SYSTEM_UI_FLAG_IMMERSIVE
					| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY );
		}
*/
		Utils.setImmersionMode(this);

		DPLib.loadLibraries();
		start_quake2();   

	}
	
	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		Utils.onWindowFocusChanged(this, hasFocus);
	}
	 
	

	public void start_quake2() {


		DPLib engine = new DPLib();
		controlInterp = new ControlInterpreter(engine,Utils.getGameGamepadConfig(AppSettings.game), TouchSettings.gamePadControlsFile, TouchSettings.gamePadEnabled);

		TouchControlsSettings.setup(act, engine);
		TouchControlsSettings.loadSettings(act);
		TouchControlsSettings.sendToQuake();

		CustomCommands.setup(act, engine, getIntent().getStringExtra("main_qc"), getIntent().getStringExtra("mod_qc"));


		// Create our Preview view and set it as the content of our
		// Activity
		mGLSurfaceView = new QuakeView(this);

		ShowKeyboard.setup(this,mGLSurfaceView);

		mGLSurfaceView.setEGLContextClientVersion(2); // enable OpenGL 2.0
		mGLSurfaceView.setPreserveEGLContextOnPause(true);
		//mGLSurfaceView.setGLWrapper( new MyWrapper());
		//mGLSurfaceView.setDebugFlags(GLSurfaceView.DEBUG_CHECK_GL_ERROR | GLSurfaceView.DEBUG_LOG_GL_CALLS);
		//setEGLConfigChooser  (int redSize, int greenSize, int blueSize, int alphaSize, int depthSize, int stencilSize)
		//mGLSurfaceView.setEGLConfigChooser(8,8,8,0,16,0);
		mGLSurfaceView.setEGLConfigChooser( new QuakeEGLConfigChooser());
		//mGLSurfaceView.setEGLConfigChooser( new BestEglChooser(getApplicationContext()) );

		mGLSurfaceView.setRenderer(mRenderer);

		// This will keep the screen on, while your view is visible. 
		mGLSurfaceView.setKeepScreenOn(true);

		setContentView(mGLSurfaceView);
		mGLSurfaceView.requestFocus();
		mGLSurfaceView.setFocusableInTouchMode(true);
	}


	@Override
	protected void onPause() {
		Log.i( "Quake2.java", "onPause" );
		CDAudioPlayer.onPause();
		mGLSurfaceView.onPause();
		mogaController.onPause();
		super.onPause();
	}

	@Override
	protected void onResume() {

		Log.i( "Quake2.java", "onResume" );
		CDAudioPlayer.onResume();
		mGLSurfaceView.onResume();
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
			//Log.d(LOG,"onKeyDown " + keyCode);
			return controlInterp.onKeyDown(keyCode, event);
		}

		@Override
		public boolean onKeyUp(int keyCode, KeyEvent event)
		{
			return controlInterp.onKeyUp(keyCode, event);
		} 

	}  // end of QuakeView

	///////////// GLSurfaceView.Renderer implementation ///////////

	class QuakeRenderer implements GLSurfaceView.Renderer {



		public void onSurfaceCreated(GL10 gl, EGLConfig config) {

			Log.d("Renderer", "onSurfaceCreated");

		}





		private void init( int width, int height ){

			Log.i( "Quake2", "screen size : " + width + "x"+ height);

			DPLib.setScreenSize(width,height);

			Utils.copyPNGAssets(getApplicationContext(),AppSettings.graphicsDir);   

			fpsLimit = new FPSLimit(getApplicationContext());

			Log.i("Quake2", "Quake2Init start");

			int game = getIntent().getIntExtra("game", 0);
			
			String[] args_array = Utils.creatArgs(args);

			int ret = DPLib.init(AppSettings.graphicsDir,64,args_array,game);

			Log.i("Quake2", "Quake2Init done");

		}



		//// new Renderer interface
		int notifiedflags;

		FPSLimit fpsLimit;

		boolean inited = false;

		public void onDrawFrame(GL10 gl) {



			//Log.d(LOG,"onDrawFrame");

			if (!inited)
			{
				inited = true;
				init( surfaceWidth, surfaceHeight );
			}

			fpsLimit.tick();

			int flags =  DPLib.frame();

			if (flags != notifiedflags)
			{
				if (((flags ^ notifiedflags) & 1) != 0)
				{
					
					Log.d(LOG,"show keyboard");
					/*
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
									im.hideSoftInputFromWindow(mGLSurfaceView.getWindowToken(), 0);
								}
							}
							else
								android.util.Log.i("FTEDroid", "IMM failed");
						}
					};
					act.runOnUiThread(r);
				*/
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

	class QuakeEGLConfigChooser implements GLSurfaceView.EGLConfigChooser {

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {

			Log.i( "Quake2.java", "chooseConfig");

			int EGL_OPENGL_ES2_BIT = 4;

			int[] mConfigSpec  = {
					EGL10.EGL_RED_SIZE, 8,
					EGL10.EGL_GREEN_SIZE, 8,
					EGL10.EGL_BLUE_SIZE, 8,
					EGL10.EGL_ALPHA_SIZE, 8,
					EGL10.EGL_DEPTH_SIZE, 16,
					EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
					//EGL10.EGL_STENCIL_SIZE, 0,
					EGL10.EGL_NONE};



			int[] num_config = new int[1];
			egl.eglChooseConfig(display, mConfigSpec, null, 0, num_config);

			int numConfigs = num_config[0];

			Log.i( "Quake2.java", "numConfigs="+numConfigs);

			if (numConfigs <= 0) {
				throw new IllegalArgumentException(
						"No EGL configs match configSpec");
			}

			EGLConfig[] configs = new EGLConfig[numConfigs];
			egl.eglChooseConfig(display, mConfigSpec, configs, numConfigs,
					num_config);


			for(EGLConfig config : configs) {
				Log.i( "Quake2.java", "found EGL config : " + printConfig(egl,display,config));
			}


			// best choice : select first config
			Log.i( "Quake2.java", "selected EGL config : " + printConfig(egl,display,configs[0]));

			return configs[0];
		}


		private  String printConfig(EGL10 egl, EGLDisplay display,
									EGLConfig config) {

			int r = findConfigAttrib(egl, display, config,
					EGL10.EGL_RED_SIZE, 0);
			int g = findConfigAttrib(egl, display, config,
					EGL10.EGL_GREEN_SIZE, 0);
			int b = findConfigAttrib(egl, display, config,
					EGL10.EGL_BLUE_SIZE, 0);
			int a = findConfigAttrib(egl, display, config,
					EGL10.EGL_ALPHA_SIZE, 0);
			int d = findConfigAttrib(egl, display, config,
					EGL10.EGL_DEPTH_SIZE, 0);
			int s = findConfigAttrib(egl, display, config,
					EGL10.EGL_STENCIL_SIZE, 0);

			return String.format("EGLConfig rgba=%d%d%d%d depth=%d stencil=%d", r,g,b,a,d,s)
					+ " native=" + findConfigAttrib(egl, display, config, EGL10.EGL_NATIVE_RENDERABLE, 0)
					+ " buffer=" + findConfigAttrib(egl, display, config, EGL10.EGL_BUFFER_SIZE, 0)
					+ String.format(" caveat=0x%04x" , findConfigAttrib(egl, display, config, EGL10.EGL_CONFIG_CAVEAT, 0))
					;




		}

		private int findConfigAttrib(EGL10 egl, EGLDisplay display,
									 EGLConfig config, int attribute, int defaultValue) {

			int[] mValue = new int[1];
			if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
				return mValue[0];
			}
			return defaultValue;
		}

	}
}


