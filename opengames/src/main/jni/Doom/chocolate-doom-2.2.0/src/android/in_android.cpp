

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

#include "i_system.h"
#include "m_fixed.h"

#include <android/log.h>
//#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"JNI", __VA_ARGS__))
//#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "JNI", __VA_ARGS__))
//#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"JNI", __VA_ARGS__))



// FIFO STUFF ////////////////////
// Copied from FTEQW, I don't know if this is thread safe, but it's safe enough for a game :)
#define EVENTQUEUELENGTH 128

//This event_t is from choc Doom d_event.h
event_t  eventlist[EVENTQUEUELENGTH];

volatile int events_avail; /*volatile to make sure the cc doesn't try leaving these cached in a register*/
volatile int events_used;

static  event_t *in_newevent(void)
{
	if (events_avail >= events_used + EVENTQUEUELENGTH)
		return 0;
	return &eventlist[events_avail & (EVENTQUEUELENGTH-1)];
}

static void in_finishevent(void)
{
	events_avail++;
}

int add_choc_event(evtype_t type, int data1, int data2, int data3){

	//LOGI("PortableKeyEvent %d %d",state,code);
	event_t *ev = in_newevent();
	if (!ev)
		return 0;

	ev->type = type;
	ev->data1 = data1;
	ev->data2 = data2;
	ev->data3 = data3;

	in_finishevent();
	return 0;
}
///////////////////////


extern int SDL_SendKeyboardKey(Uint8 state, SDL_Scancode scancode);

int PortableKeyEvent(int state, int code, int unicode){
	LOGI("PortableKeyEvent %d %d %d",state,code,unicode);

	if (state)
		SDL_SendKeyboardKey(SDL_PRESSED, (SDL_Scancode)code);
	else
		SDL_SendKeyboardKey(SDL_RELEASED, (SDL_Scancode) code);

	return 0;

}

extern int key_right;
extern int key_left;

extern int key_up;
extern int key_down;
extern int key_strafeleft;
extern int key_straferight;
extern int key_fire;
extern int key_use;
extern int key_strafe;
extern int key_speed;

extern int key_jump;

extern int key_flyup;
extern int key_flydown;
extern int key_flycenter;
extern int key_lookup;
extern int key_lookdown;
extern int key_lookcenter;
extern int key_invleft;
extern int key_invright;
extern int key_useartifact;
// villsa [STRIFE] strife keys
extern int key_usehealth;
extern int key_invquery;
extern int key_mission;
extern int key_invpop;
extern int key_invkey;
extern int key_invhome;
extern int key_invend;
extern int key_invuse;
extern int key_invdrop;

extern int key_message_refresh;
extern int key_pause;

extern int key_multi_msg;
extern int key_multi_msgplayer[8];

extern int key_weapon1;
extern int key_weapon2;
extern int key_weapon3;
extern int key_weapon4;
extern int key_weapon5;
extern int key_weapon6;
extern int key_weapon7;
extern int key_weapon8;

extern int key_demo_quit;
extern int key_spy;
extern int key_prevweapon;
extern int key_nextweapon;

extern int key_map_north;
extern int key_map_south;
extern int key_map_east;
extern int key_map_west;
extern int key_map_zoomin;
extern int key_map_zoomout;
extern int key_map_toggle;
extern int key_map_maxzoom;
extern int key_map_follow;
extern int key_map_grid;
extern int key_map_mark;
extern int key_map_clearmark;

// menu keys:

extern int key_menu_activate;
extern int key_menu_up;
extern int key_menu_down;
extern int key_menu_left;
extern int key_menu_right;
extern int key_menu_back;
extern int key_menu_forward;
extern int key_menu_confirm;
extern int key_menu_abort;

extern int key_menu_help;
extern int key_menu_save;
extern int key_menu_load;
extern int key_menu_volume;
extern int key_menu_detail;
extern int key_menu_qsave;
extern int key_menu_endgame;
extern int key_menu_messages;
extern int key_menu_qload;
extern int key_menu_quit;
extern int key_menu_gamma;

extern int key_menu_incscreen;
extern int key_menu_decscreen;
void ActionKey(int state,int key)
{
	add_choc_event(state?ev_keydown:ev_keyup,key,0,0);
}

int newweapon = -1;

void PortableAction(int state, int action)
{
	LOGI("PortableAction %d   %d",state,action);

	if (PortableInMenu())
	{
		if (action >= PORT_ACT_MENU_UP && action <= PORT_ACT_MENU_BACK)
		{

			int sdl_code [] = { SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT,
					SDL_SCANCODE_RIGHT, SDL_SCANCODE_RETURN, SDL_SCANCODE_ESCAPE };
			PortableKeyEvent(state, sdl_code[action-PORT_ACT_MENU_UP], 0);
			return;

		}
	}
	else
	{

		int key = -1;

		switch (action)
		{
		case PORT_ACT_LEFT:
			key = key_left;
			break;
		case PORT_ACT_RIGHT:
			key = key_right;
			break;
		case PORT_ACT_FWD:
			key = key_up;
			break;
		case PORT_ACT_BACK:
			key = key_down;
			break;
		case PORT_ACT_MOVE_LEFT:
			key = key_strafeleft;
			break;
		case PORT_ACT_MOVE_RIGHT:
			key = key_straferight;
			break;
		case PORT_ACT_USE:
			key = key_use;
			break;
		case PORT_ACT_ATTACK:
			key = key_fire;
			break;
		case PORT_ACT_JUMP:
			key = key_jump;
			break;
		case PORT_ACT_STRAFE:
			key = key_strafe;
			break;
		case PORT_ACT_SPEED:
			key = key_speed;
			break;
		case PORT_ACT_MAP:
			key = key_map_toggle;
			break;
		case PORT_ACT_MAP_ZOOM_IN:
			key = key_map_zoomin;
			break;
		case PORT_ACT_MAP_ZOOM_OUT:
			key = key_map_zoomout;
			break;
		case PORT_ACT_INVUSE:
			key = key_useartifact;
			break;
		case PORT_ACT_INVDROP:
			key = key_invdrop;
			break;
		case PORT_ACT_INVPREV:
			key = key_invleft;
			break;
		case PORT_ACT_INVNEXT:
			key = key_invright;
			break;
		case PORT_ACT_NEXT_WEP:
			key = key_nextweapon;
			break;
		case PORT_ACT_PREV_WEP:
			key = key_prevweapon;
			break;
		case PORT_ACT_QUICKSAVE:
			key = key_menu_qsave;
			break;
		case PORT_ACT_QUICKLOAD:
			key = key_menu_qload;
			break;
		case PORT_ACT_GAMMA:
			key = key_menu_gamma;
			break;
		case PORT_ACT_HELPCOMP:
			key = key_mission;
			break;
		case PORT_ACT_SHOW_WEAPONS:
			key = key_invpop;
			break;
		case PORT_ACT_SHOW_KEYS:
			key = key_invkey;
			break;
		case PORT_ACT_FLY_UP:
			key = key_flyup;
			break;
		case PORT_ACT_FLY_DOWN:
			key = key_flydown;
			break;
		case PORT_ACT_FLY_CENTER:
			key = key_flycenter;
			break;
		case PORT_ACT_WEAP1:
			newweapon = 1;
			break;
		case PORT_ACT_WEAP2:
			newweapon = 2;
			break;
		case PORT_ACT_WEAP3:
			newweapon = 3;
			break;
		case PORT_ACT_WEAP4:
			newweapon = 4;
			break;
		case PORT_ACT_WEAP5:
			newweapon = 5;
			break;
		case PORT_ACT_WEAP6:
			newweapon = 6;
			break;
		case PORT_ACT_WEAP7:
			newweapon = 7;
			break;
		case PORT_ACT_WEAP8:
			newweapon = 8;
			break;
		}

		if (key != -1)
			ActionKey(state,key);
	}
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

#ifndef CHOC_SETUP

#if defined(CHOC_DOOM) || defined(CHOC_STRIFE)
extern boolean menuactive;
extern boolean paused;
extern boolean usergame;

int PortableInMenu(void){

	return (menuactive || paused || !usergame);;
}
#else
extern boolean MenuActive;
extern boolean paused;
extern boolean usergame;
extern boolean askforquit;
int PortableInMenu(void){

	return (MenuActive || paused || !usergame || askforquit);
}
#endif
#else //SETUP ONLY
int PortableInMenu(void){
	return 1;
}
#endif

int PortableInAutomap(void)
{
	return 0;
}

int PortableShowKeyboard(void){

	return 0;
}

#ifndef CHOC_SETUP

void I_UpdateAndroid(void)
{
	event_t *ev;

	while (events_used != events_avail)
	{
		ev = &eventlist[events_used & (EVENTQUEUELENGTH-1)];


		D_PostEvent(ev);

		events_used++;
	}
}


extern fixed_t forwardmove[2];
extern fixed_t sidemove[2];
extern fixed_t angleturn[3];     // + slow turn

static int mlooky = 0;
//Called from the game
void G_AndroidBuildTiccmd(ticcmd_t *cmd)
{
	cmd->forwardmove  += forwardmove_android * forwardmove[1];
	cmd->sidemove  += sidemove_android   * sidemove[1];


	switch(look_pitch_mode)
	{
	case LOOK_MODE_MOUSE:
		mlooky += look_pitch_mouse * 10000;
		look_pitch_mouse = 0;
		break;
	case LOOK_MODE_ABSOLUTE:
		mlooky = look_pitch_abs * 80;
		break;
	case LOOK_MODE_JOYSTICK:
		mlooky += look_pitch_joy * 300;
		break;
	}

	//LOGI("mlooky = %d",mlooky);

#ifdef CHOC_STRIFE
	if (abs(mlooky) > 50) //make look more senstive for strife
#else
	if (abs(mlooky) > 100)
#endif
	{
		int look = -mlooky/100;
		if (look > 7) look = 7;
		if (look < -7) look = -7;

		if (look < 0)
		{
			look += 16;
		}
		cmd->lookfly |= look & 0xF;

		if (mlooky > 0)
			cmd->buttons2 |= BT2_LOOKDOWN;
		else
			cmd->buttons2 |= BT2_LOOKUP;

		mlooky = 0;
	}


	switch(look_yaw_mode)
	{
	case LOOK_MODE_MOUSE:
		cmd->angleturn += look_yaw_mouse * 50000;
		look_yaw_mouse = 0;
		break;
	case LOOK_MODE_JOYSTICK:
		cmd->angleturn += look_yaw_joy * 1000;
		break;
	}
	if (newweapon != -1)
	{
		cmd->buttons |= BT_CHANGE;
		cmd->buttons |= (newweapon-1)<<BT_WEAPONSHIFT;
		newweapon = -1;
	}
}

#endif
}


