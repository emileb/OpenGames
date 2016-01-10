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
package com.beloko.opengames.abuse;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.PixelFormat;
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
import com.beloko.libsdl.SDLLib;
import com.beloko.opengames.AppSettings;
import com.beloko.opengames.BestEglChooser;
import com.beloko.opengames.CDAudioPlayer;
import com.beloko.opengames.FPSLimit;
import com.beloko.opengames.GD;
import com.beloko.opengames.MyGLSurfaceView;
import com.beloko.opengames.Utils;
import com.beloko.touchcontrols.ActionInput;
import com.beloko.touchcontrols.ControlConfig;
import com.beloko.touchcontrols.ControlInterpreter;
import com.beloko.touchcontrols.MogaHack;
import com.beloko.touchcontrols.TouchSettings;
import com.beloko.touchcontrols.ShowKeyboard;
import com.beloko.touchcontrols.TouchControlsSettings;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class Game extends Activity
{
	String LOG = "Game";

	private ControlInterpreter controlInterp;

	private final MogaControllerListener mogaListener = new MogaControllerListener();
	Controller mogaController = null;

	private String args;
	private String gamePath;
	private boolean setupLaunch; //True if the native setup program launched this

	private GameView mGLSurfaceView = null;
	private QuakeRenderer mRenderer = new QuakeRenderer();
	Activity act;

	int surfaceWidth,surfaceHeight;



	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		act = this;

		AppSettings.setGame(GD.IDGame.Abuse);

		AppSettings.reloadSettings(getApplication());

		args = getIntent().getStringExtra("args");
		//gamePath  = getIntent().getStringExtra("game_path");

		gamePath = getFilesDir().toString();

		args += " -datadir " + gamePath + "/ ";

		String missingFiles;
		if ((missingFiles = Utils.checkFiles(gamePath, new String[]{"art/dev.spe"})) != null)
		{
			extractFiles("data.zip",gamePath);
		}

		mogaController = Controller.getInstance(this);
		MogaHack.init(mogaController,this);
		mogaController.setListener(mogaListener,new Handler());

		// fullscreen
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);

		// keep screen on 
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);


		Utils.setImmersionMode(this);

		start_game();
	}

	void extractFiles(String file,String basePath)
	{
		try
		{
			AssetManager assetManager = getAssets();
			InputStream ins = assetManager.open(file);
			BufferedInputStream in = new BufferedInputStream(ins);
			ZipInputStream zis = new ZipInputStream(new BufferedInputStream(in));
			ZipEntry entry;
			while ((entry = zis.getNextEntry()) != null) {
				if(entry.isDirectory()) {
					// Assume directories are stored parents first then children.
					System.err.println("Extracting directory: " + entry.getName());
					// This is not robust, just for demonstration purposes.
					(new File(basePath, entry.getName())).mkdirs();
					continue;
				}
				if (GD.DEBUG) Log.d(LOG,"Extracting file: " + entry.getName());
				(new File(basePath, entry.getName())).getParentFile().mkdirs();
				BufferedInputStream zin = new BufferedInputStream(zis);
				OutputStream out =  new FileOutputStream(new File(basePath, entry.getName()));
				Utils.copyFile(zin,out);
			}
		} catch (IOException e) {
			return;
		}
	}

	public static ArrayList<ActionInput> getGameGamepadConfig() {
		ArrayList<ActionInput> actions = new ArrayList<ActionInput>();

		actions.add(new ActionInput("analog_look_pitch", "Look Up/Look Down", ControlConfig.ACTION_ANALOG_PITCH, ControlConfig.Type.ANALOG,
				ControlConfig.Type.ANALOG,MotionEvent.AXIS_RZ));

		actions.add(new ActionInput("analog_look_yaw", "Look Left/Look Right", ControlConfig.ACTION_ANALOG_YAW, ControlConfig.Type.ANALOG,
				ControlConfig.Type.ANALOG,MotionEvent.AXIS_Z));

		actions.add(new ActionInput("left","Left",ControlConfig.PORT_ACT_MOVE_LEFT, ControlConfig.Type.BUTTON,
				ControlConfig.Type.ANALOG,MotionEvent.AXIS_X,false));
		actions.add(new ActionInput("right","Right",ControlConfig.PORT_ACT_MOVE_RIGHT, ControlConfig.Type.BUTTON,
				ControlConfig.Type.ANALOG,MotionEvent.AXIS_X,true));

		actions.add(new ActionInput("look_down","Down",ControlConfig.PORT_ACT_LOOK_DOWN, ControlConfig.Type.BUTTON,
				ControlConfig.Type.ANALOG,MotionEvent.AXIS_Y,true));

		actions.add(new ActionInput("jump","Jump",ControlConfig.PORT_ACT_JUMP, ControlConfig.Type.BUTTON,
				ControlConfig.Type.BUTTON,KeyEvent.KEYCODE_BUTTON_A));

		actions.add(new ActionInput("next_weapon","Next Weapon",ControlConfig.PORT_ACT_NEXT_WEP, ControlConfig.Type.BUTTON,
				ControlConfig.Type.BUTTON,KeyEvent.KEYCODE_BUTTON_R1));
		actions.add(new ActionInput("prev_weapon","Previous Weapon",ControlConfig.PORT_ACT_PREV_WEP, ControlConfig.Type.BUTTON,
				ControlConfig.Type.BUTTON,KeyEvent.KEYCODE_BUTTON_L1));

		actions.add(new ActionInput("menu_up","Menu Up",ControlConfig.MENU_UP, ControlConfig.Type.MENU,
				ControlConfig.Type.ANALOG,MotionEvent.AXIS_HAT_Y,false));
		actions.add(new ActionInput("menu_down","Menu Down",ControlConfig.MENU_DOWN, ControlConfig.Type.MENU,
				ControlConfig.Type.ANALOG,MotionEvent.AXIS_HAT_Y,true));
		actions.add(new ActionInput("menu_left","Menu Left",ControlConfig.MENU_LEFT, ControlConfig.Type.MENU,
				ControlConfig.Type.ANALOG,MotionEvent.AXIS_HAT_X,false));
		actions.add(new ActionInput("menu_right","Menu Right",ControlConfig.MENU_RIGHT, ControlConfig.Type.MENU,
				ControlConfig.Type.ANALOG,MotionEvent.AXIS_HAT_X,true));
		actions.add(new ActionInput("menu_select","Menu Select",ControlConfig.MENU_SELECT, ControlConfig.Type.MENU,
				ControlConfig.Type.BUTTON,KeyEvent.KEYCODE_BUTTON_A));
		actions.add(new ActionInput("menu_back","Menu Back",ControlConfig.MENU_BACK, ControlConfig.Type.MENU,
				ControlConfig.Type.BUTTON,KeyEvent.KEYCODE_BUTTON_B));
		return actions;
	}

		@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		Utils.onWindowFocusChanged(this, hasFocus);
	}

	public void start_game() {

		NativeLib.loadLibraries(false);

		NativeLib engine = new NativeLib();


		controlInterp = new ControlInterpreter(engine,getGameGamepadConfig(), TouchSettings.gamePadControlsFile, TouchSettings.gamePadEnabled);

		TouchControlsSettings.setup(act, engine);
		TouchControlsSettings.loadSettings(act);
		TouchControlsSettings.sendToQuake();


		mGLSurfaceView = new GameView(this);

		NativeLib.gv = mGLSurfaceView;

		ShowKeyboard.setup(act, mGLSurfaceView);

		mGLSurfaceView.setEGLConfigChooser( new BestEglChooser(getApplicationContext()) );

		mGLSurfaceView.setRenderer(mRenderer);

		// This will keep the screen on, while your view is visible. 
		mGLSurfaceView.setKeepScreenOn(true);

		setContentView(mGLSurfaceView);
		mGLSurfaceView.requestFocus();
		mGLSurfaceView.setFocusableInTouchMode(true);
	}




	@Override
	protected void onPause() {
		Log.i(LOG, "onPause" );
		CDAudioPlayer.onPause();
		SDLLib.onPause();
		mogaController.onPause();
		super.onPause();
	}

	@Override
	protected void onResume() {

		Log.i(LOG, "onResume" );
		CDAudioPlayer.onResume();
		SDLLib.onResume();
		mogaController.onResume();
		super.onResume();
		mGLSurfaceView.onResume();
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
			controlInterp.onGenericMotionEvent(event);
		}

		@Override
		public void onStateEvent(StateEvent event) {
			Log.d(LOG,"onStateEvent " + event.getState());
		}
	}

	class GameView extends MyGLSurfaceView {

		/*--------------------
		 * Event handling
		 *--------------------*/


		public GameView(Context context) {
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
			return controlInterp.onKeyDown(keyCode, event);
		}

		@Override
		public boolean onKeyUp(int keyCode, KeyEvent event)
		{
			return controlInterp.onKeyUp(keyCode, event);
		}

	}  // end of QuakeView



	///////////// GLSurfaceView.Renderer implementation ///////////

	class QuakeRenderer implements MyGLSurfaceView.Renderer {



		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			Log.d("Renderer", "onSurfaceCreated");
		}

		private void init( int width, int height ){

			Log.i( LOG, "screen size : " + width + "x"+ height);

			NativeLib.setScreenSize(width,height);

			Utils.copyPNGAssets(getApplicationContext(),AppSettings.graphicsDir);

			Log.i(LOG, "Quake2Init start");


			args += " -gl";

			String[] args_array = Utils.creatArgs(args);

			int ret = NativeLib.init(AppSettings.graphicsDir,0,args_array,0,gamePath);

			Log.i(LOG, "Quake2Init done");

		}

		//// new Renderer interface
		int notifiedflags;

		FPSLimit fpsLimit;

		public void onDrawFrame(GL10 gl) {

			init( surfaceWidth, surfaceHeight );

				System.exit(0);
		}

		boolean SDLinited = false;
		public void onSurfaceChanged(GL10 gl, int width, int height) {
			Log.d("Renderer", String.format("onSurfaceChanged %dx%d", width,height) );

			if (!SDLinited)
			{
				SDLLib.nativeInit(false);
				SDLLib.surfaceChanged(PixelFormat.RGBA_8888, width, height);
				SDLinited = true;
			}

			controlInterp.setScreenSize(width, height);

			surfaceWidth = width;
			surfaceHeight = height;
		}
	} // end of QuakeRenderer
}


