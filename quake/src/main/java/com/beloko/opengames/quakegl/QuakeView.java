/*
 * Copyright (C) 2007 The Android Open Source Project
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

package com.beloko.opengames.quakegl;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.inputmethod.InputMethodManager;

import com.bda.controller.Controller;
import com.bda.controller.ControllerListener;
import com.bda.controller.StateEvent;
import com.beloko.opengames.AppSettings;
import com.beloko.opengames.FPSLimit;
import com.beloko.opengames.Utils;
import com.beloko.touchcontrols.ControlInterpreter;
import com.beloko.touchcontrols.MogaHack;
import com.beloko.touchcontrols.TouchControlsSettings;
import com.beloko.touchcontrols.TouchSettings;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * An implementation of SurfaceView that uses the dedicated surface for
 * displaying an OpenGL animation.  This allows the animation to run in a
 * separate thread, without requiring that it be driven by the update mechanism
 * of the view hierarchy.
 *
 * The application-specific rendering code is delegated to a GLView.Renderer
 * instance.
 */
class QuakeView extends GLSurfaceView {

	final static String LOG =  "QuakeView";

	int screenWidth, screenHeight;
	private ControlInterpreter controlInterp;
	
	private final MogaControllerListener mogaListener = new MogaControllerListener();
	Controller mogaController = null;
	
	QuakeView top;
	Activity activity;

	String args;
	QuakeView(Context context,QuakeLib engine,Activity act) {
		super(context);
		init(engine,act);
	}                                              

	                                

	private void init(QuakeLib engine,Activity act) {
		top = this;
		activity = act;
		// We want events.

		controlInterp = new ControlInterpreter(engine,Utils.getGameGamepadConfig(AppSettings.game), TouchSettings.gamePadControlsFile, TouchSettings.gamePadEnabled);

		mogaController = Controller.getInstance(act);
		MogaHack.init(mogaController, getContext());
		mogaController.setListener(mogaListener,new Handler());
		
		TouchControlsSettings.sendToQuake();
		
		setFocusable(true);
		setFocusableInTouchMode(true);      
		requestFocus();
	}
	public void setArgs(String a)
	{
		args = a;
	}
	
	public void setQuakeLib(QuakeLib quakeLib) {
		mQuakeLib = quakeLib;
		setRenderer(new QuakeRenderer());
	}        
	
	/*
	@Override
	public boolean onGenericMotionEvent(MotionEvent event) {
	  Log.d(LOG,"ev = " + event.toString());

	  return super.onGenericMotionEvent(event);
	}
	 */
	
	@Override
	public boolean onTouchEvent(MotionEvent event) {

		return controlInterp.onTouchEvent(event);
	}   
	
	
	@Override
	public boolean onGenericMotionEvent(MotionEvent event) {
		return controlInterp.onGenericMotionEvent(event);
	}
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		//Log.d ("FTE","key dev id = " + event.getDeviceId());
		return controlInterp.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event)
	{
		return controlInterp.onKeyUp(keyCode, event);
	} 

	@Override
	public void onPause() {
		super.onPause();
		mogaController.onPause();
	}

	@Override
	public void onResume() {
		super.onResume();
		mogaController.onResume();
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
	/*
    @Override
    public boolean dispatchTouchEvent(MotionEvent ev) {
    	Log.d("TEST","touch");
        queueMotionEvent(ev);
        return true;
    }
	 */     



	private class QuakeRenderer implements GLSurfaceView.Renderer {
		FPSLimit fpsLimit;
		
		
		public void onDrawFrame(GL10 gl) {
			if (mWidth != 0 &&  mHeight != 0) {

				fpsLimit.tick();
				
				int key = mQuakeLib.step(mWidth, mHeight);

				if (key == 128)
				{
					 activity.finish(); 
					 
					 return;
				}
				
				if (key == 1)
				{

					InputMethodManager im = (InputMethodManager) getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
					if (im != null)
					{

						im.showSoftInput(top, 0);//InputMethodManager.SHOW_FORCED);
					}
					else
						android.util.Log.i("FTEDroid", "IMM failed");

				}
			}
		}          
		
		public void onSurfaceChanged(GL10 gl, int width, int height) {
			mWidth = width;
			mHeight = height;
			screenHeight = mHeight;
			screenWidth = mWidth;
			
			controlInterp.setScreenSize(width, height);
			
			Log.d("test","onSurfaceChanged");  

			String[] args_array = Utils.creatArgs(args);

			mQuakeLib.init(args_array);
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			fpsLimit = new FPSLimit(activity);
		}
		private int mWidth;
		private int mHeight;
	}

	private QuakeLib mQuakeLib;
	private boolean mGameMode;
}

