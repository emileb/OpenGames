

extern "C"
{

#include "in_android.h"

#include <signal.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "SDL.h"
#include "SDL_keycode.h"


#include <android/log.h>
//#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"JNI", __VA_ARGS__))
//#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "JNI", __VA_ARGS__))
//#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"JNI", __VA_ARGS__))


#include "game/game.h"



unsigned char android_keyboard[323];
unsigned char android_keyboard_sticky[323];

int PortableKeyEvent(int state, int code, int unicode){
	LOGI("PortableKeyEvent %d %d %d",state,code,unicode);
	android_keyboard[code] = state;

	if (state)
		android_keyboard_sticky[code] = 1;

	return 0;

}

void GetAndroidKeyboard(unsigned char*keyboard)
{

	memcpy(keyboard,android_keyboard,323);

	for (int n=0;n<323;n++)
		keyboard[n] |= android_keyboard_sticky[n];

	memset(android_keyboard_sticky,0,323);

}

void PortableAction(int state, int action)
{
	LOGI("PortableAction %d   %d",state,action);

}

int mdx=0,mdy=0;
void PortableMouse(float dx,float dy)
{
	dx *= 1500;
	dy *= 1200;

	mdx += dx;
	mdy += dy;
}

int absx=0,absy=0;
void PortableMouseAbs(float x,float y)
{
	absx = x;
	absy = y;
}
int mouse_buttons=0;
void PortableMouseButton(int buttons)
{

	mouse_buttons |= buttons;
}


// =================== FORWARD and SIDE MOVMENT ==============

float forwardmove_android, sidemove_android; //Joystick mode

void PortableMoveFwd(float fwd)
{
	if (fwd > 1)
		fwd = 1;
	else if (fwd < -1)
		fwd = -1;

	forwardmove_android = fwd;
}

void PortableMoveSide(float strafe)
{
	if (strafe > 1)
		strafe = 1;
	else if (strafe < -1)
		strafe = -1;

	sidemove_android = strafe;
}

void PortableMove(float fwd, float strafe)
{
	PortableMoveFwd(fwd);
	PortableMoveSide(strafe);
}

//======================================================================

//Look up and down
int look_pitch_mode;
float look_pitch_mouse,look_pitch_abs,look_pitch_joy;
void PortableLookPitch(int mode, float pitch)
{
	look_pitch_mode = mode;
	switch(mode)
	{
	case LOOK_MODE_MOUSE:
		look_pitch_mouse += pitch;
		break;
	case LOOK_MODE_ABSOLUTE:
		look_pitch_abs = pitch;
		break;
	case LOOK_MODE_JOYSTICK:
		look_pitch_joy = pitch;
		break;
	}
}

//left right
int look_yaw_mode;
float look_yaw_mouse,look_yaw_joy;
void PortableLookYaw(int mode, float yaw)
{
	look_yaw_mode = mode;
	switch(mode)
	{
	case LOOK_MODE_MOUSE:
		look_yaw_mouse += yaw;
		break;
	case LOOK_MODE_JOYSTICK:
		look_yaw_joy = yaw;
		break;
	}
}




extern int main_android(int argc, char *argv[]);

void PortableInit(int argc,const char ** argv){
	main_android(argc,(char **)argv);
}


void PortableFrame(void){


}
extern int playing_game;
int PortableInMenu(void){

	return !playing_game;



	if (game.exit==GAMEEXIT_NONE && !game.pause && game.dialog==0 && !game.over)
		return false;
	else
		return true;
}



int PortableShowKeyboard(void){

	return 0;
}


int GetAndroidMouse(int *x, int *y)
{
	*x = absx;
	*y = absy;

	int buttons = mouse_buttons;
	mouse_buttons = 0;
	return buttons;
}
}


