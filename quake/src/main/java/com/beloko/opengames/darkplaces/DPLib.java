package com.beloko.opengames.darkplaces;

import android.util.Log;
import android.view.KeyEvent;

import com.beloko.touchcontrols.ControlInterface;


public class DPLib implements ControlInterface {

	public static void loadLibraries()
	{
		try {
			Log.i("JNI", "Trying to load darkplaces.so");
			System.loadLibrary("touchcontrols_gles2");
			System.loadLibrary("darkplaces");
		}
		catch (UnsatisfiedLinkError ule) {
			Log.e("JNI", "WARNING: Could not load darkplaces.so: " + ule.toString());
		}
	}

	
	public static native int init(String graphics_dir,int mem,String[] args,int game);

	public static native void setScreenSize( int width, int height );
	
	public static native int frame();

	public static native boolean touchEvent( int action, int pid, float x, float y);
	public static native void keypress(int down, int qkey, int unicode);
	public static native void doAction(int state, int action);
	public static native void analogFwd(float v);
	public static native void analogSide(float v);
	public static native void analogPitch(int mode,float v);
	public static native void analogYaw(int mode,float v);
	public static native void setTouchSettings(float alpha,float strafe,float fwd,float pitch,float yaw,int other);
	
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
		keypress(down,qkey,unicode);
		
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
	
	public static final int KEY_PRESS = 1;
	public static final int KEY_RELEASE = 0;

	
	private static final int    K_ESCAPE = 27;
	private static final int    K_ENTER = 13;
	private static final int    K_TAB = 9;
	private static final int    K_SPACE = 32;
	
	private static final int    K_BACKSPACE = 127;
	private static final int    K_UPARROW = 128;
	private static final int    K_DOWNARROW = 129;
	private static final int    K_LEFTARROW = 130;
	private static final int    K_RIGHTARROW = 131;
	
	private static final int    K_LALT = 132;
	private static final int    K_RALT = 132;
	private static final int    K_LCTRL = 133;
	private static final int    K_RCTRL = 133;
	private static final int    K_LSHIFT = 134;
	private static final int    K_RSHIFT = 134;
	
	private static final int    K_F1 = 135;
	private static final int    K_F2 = 136;
	private static final int    K_F3 = 137;
	private static final int    K_F4 = 138;
	private static final int    K_F5 = 139;
	private static final int    K_F6 = 140;
	private static final int    K_F7 = 141;
	private static final int    K_F8 = 142;
	private static final int    K_F9 = 143;
	private static final int    K_F10 = 144;
	private static final int    K_F11 = 145;
	private static final int    K_F12 = 146;

	
	private static final int    K_INSERT = 147;
	private static final int    K_DELETE = 148;
	private static final int    K_PAGEDOWN = 149;
	private static final int    K_PAGEUP = 150;
	
	
	private static final int    K_HOME = 151;
	private static final int    K_END = 152;
	

	private static final int    K_CAPSLOCK = 155;

	
	
	
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
			return K_LALT;
		case KeyEvent.KEYCODE_ALT_RIGHT:
			return K_RALT;
		case KeyEvent.KEYCODE_CTRL_LEFT:
			return K_LCTRL;
		case KeyEvent.KEYCODE_CTRL_RIGHT:
			return K_RCTRL;
		case KeyEvent.KEYCODE_SHIFT_LEFT:
			return K_LSHIFT;
		case KeyEvent.KEYCODE_SHIFT_RIGHT:
			return K_RSHIFT;
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
			return K_DELETE;
		case KeyEvent.KEYCODE_INSERT:
			return K_INSERT;
		case KeyEvent.KEYCODE_PAGE_UP:
			return K_PAGEUP;
		case KeyEvent.KEYCODE_PAGE_DOWN:
			return K_PAGEDOWN;
		case KeyEvent.KEYCODE_MOVE_HOME:
			return K_HOME;
		case KeyEvent.KEYCODE_MOVE_END:
			return K_END;
		case KeyEvent.KEYCODE_BREAK:
			return 0;
		
		default:
			if (unicode < 128)
				return Character.toLowerCase(unicode);
		}
		return 0;
	} 
}
