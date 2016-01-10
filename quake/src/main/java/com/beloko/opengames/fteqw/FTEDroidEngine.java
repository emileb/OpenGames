package com.beloko.opengames.fteqw;

import android.view.KeyEvent;

import com.beloko.touchcontrols.ControlInterface;


public class FTEDroidEngine implements ControlInterface
{   
	public static native void preInit();
	public static native void init(int w, int h, int gles2,String graphic_dir, int game,int mem_mb,String[] args); /* init/reinit */
	public static native int frame(float ax, float ay, float az);
	public static native int getvibrateduration();	//in ms
	
	public static native int  paintaudio(byte[] stream, int len);
	public static native int audioinfo(int arg);
	public static native String geterrormessage();
	public static native String getpreferedorientation();
	public static native int getpreferedglesversion();
	public static native void newglcontext();
	
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
	static  
	{     
		System.loadLibrary("touchcontrols");
		System.loadLibrary("ftedroid");
	}
	
	private static final int K_TAB			= 9;
	private static final int K_ENTER		= 13;
	private static final int K_ESCAPE		= 27;
	private static final int K_SPACE		= 32;
	
	private static final int K_BACKSPACE    = 127;
	
	private static final int K_SCRLCK       = 128;
	private static final int K_CAPSLOCK     = 129;
	private static final int K_POWER        = 130;
	private static final int K_PAUSE        = 131;
	

	private static final int K_UPARROW		= 132;
	private static final int K_DOWNARROW	= 133;
	private static final int K_LEFTARROW	= 134;
	private static final int K_RIGHTARROW	= 135;
	
	private static final int K_ALT          = 136;
	private static final int K_CTRL         = 137;
	private static final int K_SHIFT        = 138;
	private static final int K_INS          = 139;
	private static final int K_DEL          = 140;
	private static final int K_PGDN         = 141;
	private static final int K_PGUP         = 142;
	private static final int K_HOME         = 143;
	private static final int K_END          = 144;
	

	private static final int K_F1           = 145;
	private static final int K_F2           = 146;
	private static final int K_F3           = 147;
	private static final int K_F4           = 148;
	private static final int K_F5           = 149;
	private static final int K_F6           = 150;
	private static final int K_F7           = 151;
	private static final int K_F8           = 152;
	private static final int K_F9           = 153;
	private static final int K_F10          = 154;
	private static final int K_F11          = 155;
	private static final int K_F12          = 156;
	private static final int K_F13          = 157;
	private static final int K_F14          = 158;
	private static final int K_F15          = 159;
	
	private static final int K_APP			= 241;
	private static final int K_SEARCH		= 242;
	private static final int K_VOLUP		= 243;
	private static final int K_VOLDOWN		= 244;
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
		case KeyEvent.KEYCODE_CAPS_LOCK:
			return K_CAPSLOCK;
		case KeyEvent.KEYCODE_SCROLL_LOCK:
			return K_SCRLCK;
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
	
	
	
}
