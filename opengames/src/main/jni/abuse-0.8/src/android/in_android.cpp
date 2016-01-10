
int main_android(int argc, char *argv[]);
#include "TouchControls.h"

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



extern "C"
{
extern int SDL_SendKeyboardKey(Uint8 state, SDL_Scancode scancode);
/* Send a mouse motion event */
extern int SDL_SendMouseMotion(SDL_Window * window, int relative, int x, int y);

/* Send a mouse button event */
extern int SDL_SendMouseButton(SDL_Window * window, Uint8 state, Uint8 button);

}

int PortableKeyEvent(int state, int code, int unicode){
	LOGI("PortableKeyEvent %d %d %d",state,code,unicode);

	if (state)
		SDL_SendKeyboardKey(SDL_PRESSED, (SDL_Scancode)code);
	else
		SDL_SendKeyboardKey(SDL_RELEASED, (SDL_Scancode) code);

	return 0;

}

float look_pitch_abs,look_yaw_joy;

int attackButton=0;

int menuControl;
void PortableAction(int state, int action)
{
	LOGI("PortableAction %d   %d",state,action);
	switch (action)
	{
		case PORT_ACT_MOVE_LEFT:
			SDL_SendKeyboardKey(state?SDL_PRESSED:SDL_RELEASED, SDL_SCANCODE_LEFT);
		break;
		case PORT_ACT_MOVE_RIGHT:
			SDL_SendKeyboardKey(state?SDL_PRESSED:SDL_RELEASED, SDL_SCANCODE_RIGHT);
		break;
		case PORT_ACT_JUMP:
			SDL_SendKeyboardKey(state?SDL_PRESSED:SDL_RELEASED, SDL_SCANCODE_UP);
		break;
		case PORT_ACT_LOOK_DOWN:
			SDL_SendKeyboardKey(state?SDL_PRESSED:SDL_RELEASED, SDL_SCANCODE_DOWN);
		break;
		case PORT_ACT_ATTACK:
			attackButton = state?SDL_BUTTON(1):0;
		break;
		case PORT_ACT_NEXT_WEP:
			SDL_SendKeyboardKey(state?SDL_PRESSED:SDL_RELEASED, SDL_SCANCODE_INSERT);
        break;
		case PORT_ACT_PREV_WEP:
			SDL_SendKeyboardKey(state?SDL_PRESSED:SDL_RELEASED,SDL_SCANCODE_HOME );
			break;
		case PORT_ACT_MENU_UP:
			if (state)
				menuControl = 1;
			break;
		case PORT_ACT_MENU_DOWN:
			if (state)
				menuControl = 2;
			break;
		case PORT_ACT_MENU_LEFT:
			if (state)
				menuControl = 3;
			break;
		case PORT_ACT_MENU_RIGHT:
			if (state)
				menuControl = 4;
			break;
		case PORT_ACT_MENU_SELECT:
			if (state)
				menuControl = 5;
			break;
		break;
	}

	//send event
	SDL_SendMouseButton(NULL,1,0);
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
	SDL_SendMouseMotion(NULL,0,x,y);
}

int android_buttons;
void PortableMouseButton(int buttons)
{
	android_buttons |= buttons;
	SDL_SendMouseButton(NULL,1,buttons);
	SDL_SendMouseButton(NULL,0,buttons);

}

int Android_GetMouseState(int *x, int *y)
{
	*x = absx;
	*y = absy;
	//LOGI("x = %d y = %d",*x,*y);

	//LOGI("%f    %f",look_yaw_joy,look_pitch_abs);
	if ((fabs(look_yaw_joy) > 0.1f) || (fabs(look_pitch_abs) > 0.1f))
	{
		//LOGI("attack on");
		attackButton = SDL_BUTTON(1);
	}
	else
	{
		//LOGI("attack off");
		attackButton = 0;
	}
	int b = android_buttons;
	android_buttons = 0;
	return b | attackButton;
}

int Android_GetMenuControl()
{
	int ret = menuControl;
	menuControl = 0;
	return ret;
}

#define PI 3.14159265

float getAngle(float a,float o)
{
	a = a * ((float)touchcontrols::ScaleX / (float)touchcontrols::ScaleY);
	//LOGTOUCH("len: %f   %f",o,a);

	//float angle = asin(o/a);
	float angle = atan2(o,a);

	angle += PI/2;

	if (angle < 0)
		angle = (2*PI) + angle;

	angle *= 180/PI;
	return angle;
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


void PortableLookPitch(int mode, float pitch)
{
//Dummy event
	SDL_SendMouseButton(NULL,1,0);
	look_pitch_abs = pitch;
}


void PortableLookYaw(int mode, float yaw)
{
	look_yaw_joy = yaw;
}





void PortableInit(int argc,const char ** argv){
	main_android(argc,(char **)argv);
}


void PortableFrame(void){
}

int PortableInMenu(void){
}


int PortableShowKeyboard(void){
	return 0;
}



int Android_GetAngle()
{

	if ((fabs(look_yaw_joy) < 0.1f) && (fabs(look_pitch_abs) < 0.1f))
	{
		return -1; //Special return to know if stick not touched
	}

	int angle = getAngle(look_yaw_joy,look_pitch_abs) + 90;
	if (angle >= 360)
		angle -= 360;
	return angle;
}

}


