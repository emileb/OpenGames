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

import android.view.KeyEvent;

import com.beloko.touchcontrols.ControlInterface;


// Wrapper for native quake application

public class QuakeLib implements ControlInterface {

	public static final int KEY_PRESS = 1;
	public static final int KEY_RELEASE = 0;

	private static final int	K_TAB			=9;
	private static final int	K_ENTER			=13;
	private static final int	K_ESCAPE		=27;
	private static final int	K_SPACE			=32;

	// normal keys should be passed as lowercased ascii

	private static final int	K_BACKSPACE		=127;
	private static final int	K_UPARROW		=128;
	private static final int	K_DOWNARROW		=129;
	private static final int	K_LEFTARROW		=130;
	private static final int	K_RIGHTARROW	=131;

	private static final int	K_ALT			=132;
	private static final int	K_CTRL			=133;
	private static final int	K_SHIFT			=134;
	private static final int	K_F1			=135;
	private static final int	K_F2			=136;
	private static final int	K_F3			=137;
	private static final int	K_F4			=138;
	private static final int	K_F5			=139;
	private static final int	K_F6			=140;
	private static final int	K_F7			=141;
	private static final int	K_F8			=142;
	private static final int	K_F9			=143;
	private static final int	K_F10			=144;
	private static final int	K_F11			=145;
	private static final int	K_F12			=146;
	private static final int	K_INS			=147;
	private static final int	K_DEL			=148;
	private static final int	K_PGDN			=149;
	private static final int	K_PGUP			=150;
	private static final int	K_HOME			=151;
	private static final int	K_END			=152;
	private static final int    K_PAUSE			=255;
	public  int mapKey(int acode,  int unicode)
	{
		switch(acode)
		{
		case KeyEvent.KEYCODE_TAB:
			return K_TAB;
		case KeyEvent.KEYCODE_DPAD_CENTER:
		case KeyEvent.KEYCODE_ENTER:
			return K_ENTER;
		case KeyEvent.KEYCODE_ESCAPE:
		case KeyEvent.KEYCODE_BACK:
			return K_ESCAPE;
		case KeyEvent.KEYCODE_SPACE:
			return K_SPACE;
		case KeyEvent.KEYCODE_DEL:
			return K_BACKSPACE;
		case KeyEvent.KEYCODE_DPAD_UP:
			return K_UPARROW;
		case KeyEvent.KEYCODE_DPAD_DOWN:
			return K_DOWNARROW;
		case KeyEvent.KEYCODE_DPAD_LEFT:
			return K_LEFTARROW;
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			return K_RIGHTARROW;
		case KeyEvent.KEYCODE_ALT_LEFT:
		case KeyEvent.KEYCODE_ALT_RIGHT:
			return K_ALT;
		case KeyEvent.KEYCODE_CTRL_LEFT:
		case KeyEvent.KEYCODE_CTRL_RIGHT:
			return K_CTRL;
		case KeyEvent.KEYCODE_SHIFT_LEFT:
		case KeyEvent.KEYCODE_SHIFT_RIGHT:
			return K_SHIFT;
		case KeyEvent.KEYCODE_F1:
			return K_F1;
		case KeyEvent.KEYCODE_F2:
			return K_F2;
		case KeyEvent.KEYCODE_F3:
			return K_F3;
		case KeyEvent.KEYCODE_F4:
			return K_F4;
		case KeyEvent.KEYCODE_F5:
			return K_F5;
		case KeyEvent.KEYCODE_F6:
			return K_F6;
		case KeyEvent.KEYCODE_F7:
			return K_F7;
		case KeyEvent.KEYCODE_F8:
			return K_F8;
		case KeyEvent.KEYCODE_F9:
			return K_F9;
		case KeyEvent.KEYCODE_F10:
			return K_F10;
		case KeyEvent.KEYCODE_F11:
			return K_F11;
		case KeyEvent.KEYCODE_F12:
			return K_F12;	
		case KeyEvent.KEYCODE_FORWARD_DEL:
			return K_DEL;
		case KeyEvent.KEYCODE_INSERT:
			return K_INS;
		case KeyEvent.KEYCODE_PAGE_UP:
			return K_PGUP;
		case KeyEvent.KEYCODE_PAGE_DOWN:
			return K_PGDN;
		case KeyEvent.KEYCODE_MOVE_HOME:
			return K_HOME;
		case KeyEvent.KEYCODE_MOVE_END:
			return K_END;
		case KeyEvent.KEYCODE_BREAK:
			return K_PAUSE;

		default:
			if (unicode < 128)
				return Character.toLowerCase(unicode);
		}
		return 0;
	} 
	static {
		System.loadLibrary("touchcontrols");
		System.loadLibrary("quake");
	}   

	public QuakeLib() {
	}  

	public native void preInit();

	public native boolean init(String args[]);


	public native boolean touchEvent( int action, int pid, float x, float y);

	public static native void doAction(int state, int action);
	public static native void analogFwd(float v);
	public static native void analogSide(float v);
	public static native void analogPitch(int mode,float v);
	public static native void analogYaw(int mode,float v);
	public static native void setTouchSettings(float alpha,float strafe,float fwd,float pitch,float yaw,int other);

	public native boolean event(int type, int value); //Keypress


	public native void setGraphicsBase( String path);

	public native void setCachePath( String path);
	public native void setBasePath( String path);
	public native void ipAddr( long  ip);   

	public static native void quickCommand(String command);

	@Override
	public void quickCommand_if(String command)
	{
		quickCommand(command);
	}

	@Override
	public void initTouchControls_if(String pngPath, int width, int height) {

	}

	@Override
	public boolean touchEvent_if(int action, int pid, float x, float y) {
		return touchEvent(  action,  pid,  x,  y);
	}
	@Override
	public void keyPress_if(int down, int qkey, int unicode) {
		event(down,qkey);

	}

	@Override
	public void doAction_if(int state, int action) {
		doAction(state,action);
	} 

	@Override
	public void analogFwd_if(float v) {
		analogFwd(v);
	}
	@Override
	public void analogSide_if(float v) {
		analogSide(v);
	}
	@Override
	public void  analogPitch_if(int mode,float v)
	{
		analogPitch(mode,v);
	}
	@Override
	public void  analogYaw_if(int mode,float v)
	{
		analogYaw(mode,v);
	}
	@Override
	public void setTouchSettings_if(float alpha,float strafe, float fwd, float pitch,
			float yaw, int other) {
		setTouchSettings(alpha,strafe, fwd, pitch, yaw, other);

	}

	/**
	 * @param width the current view width
	 * @param height the current view height
	 * @return true if quake is in "game" mode, false if it is in "menu" or
	 * "typing" mode.
	 */
	public native int step(int width, int height);

	/**
	 * Tell Quake to quit. It will write out its config files and so forth.
	 */
	public native void quit();

}
