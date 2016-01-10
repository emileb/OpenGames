
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

#include "../client/client.h"
#include "../renderer/tr_local.h"


#include "../game/g_headers.h"

#include "../game/b_local.h"
#include "../game/q_shared.h"
#include "../client/keycodes.h"
#include "../qcommon/qcommon.h"

#include "android_local.h"

extern int Key_GetCatcher( void );

#include <android/log.h>
//#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"JNI", __VA_ARGS__))
//#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "JNI", __VA_ARGS__))
//#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"JNI", __VA_ARGS__))


// FIFO STUFF ////////////////////
// Copied from FTEQW, I don't know if this is thread safe, but it's safe enough for a game :)
#define EVENTQUEUELENGTH 128
struct eventlist_s
{
	int scancode, unicode,state;
} eventlist[EVENTQUEUELENGTH];

volatile int events_avail; /*volatile to make sure the cc doesn't try leaving these cached in a register*/
volatile int events_used;

static struct eventlist_s *in_newevent(void)
{
	if (events_avail >= events_used + EVENTQUEUELENGTH)
		return 0;
	return &eventlist[events_avail & (EVENTQUEUELENGTH-1)];
}

static void in_finishevent(void)
{
	events_avail++;
}
///////////////////////
int android_look_scale;
int PortableLookScale()
{
	if (!android_look_scale)
		android_look_scale = 1;

	return android_look_scale;
}

int PortableKeyEvent(int state, int code, int unicode){

	LOGI("PortableKeyEvent state = %d, code =  %d, unicode = %d",state,code,unicode);
	struct eventlist_s *ev = in_newevent();
	if (!ev)
		return 0;

	ev->scancode = code;
	ev->unicode = unicode;
	ev->state = state;

	if (code == A_BACKSPACE)
		ev->unicode = A_BACKSPACE;

	in_finishevent();
	return 0;

}


//extern void KeyDownPort( kbutton_t *b );
//extern void KeyUpPort( kbutton_t *b );

void KeyUpPort (kbutton_t *b)
{
	b->active = qfalse;
}
void KeyDownPort (kbutton_t *b)
{
	b->active = qtrue;
	b->wasPressed = qtrue;
}

char createCommand[256];
char* postedCommand = 0;
void postCommand(const char * cmd)
{
	postedCommand = cmd;
}

extern kbutton_t	in_left, in_right, in_forward, in_back;
extern kbutton_t	in_lookup, in_lookdown, in_moveleft, in_moveright;
extern kbutton_t	in_strafe, in_speed;
extern kbutton_t	in_up, in_down;
extern  kbutton_t	in_buttons[8];

void PortableAction(int state, int action,int param)
{
	LOGI("PortableAction %d   %d",state,action);

	if (PortableInMenu())
	{
		if (action >= PORT_ACT_MENU_UP && action <= PORT_ACT_MENU_BACK)
		{

			int sdl_code [] = { A_CURSOR_UP, A_CURSOR_DOWN, A_CURSOR_LEFT,
					A_CURSOR_RIGHT, A_ENTER, A_ESCAPE };
			PortableKeyEvent(state, sdl_code[action-PORT_ACT_MENU_UP],0);
			return;

		}
	}

	switch (action)
	{
	case PORT_ACT_LEFT:
		(state)?KeyDownPort(&in_left):KeyUpPort(&in_left);
		break;
	case PORT_ACT_RIGHT:
		(state)?KeyDownPort(&in_right):KeyUpPort(&in_right);
		break;
	case PORT_ACT_FWD:
		(state)?KeyDownPort(&in_forward):KeyUpPort(&in_forward);
		break;
	case PORT_ACT_BACK:
		(state)?KeyDownPort(&in_back):KeyUpPort(&in_back);
		break;
	case PORT_ACT_LOOK_UP:
		(state)?KeyDownPort(&in_lookup):KeyUpPort(&in_lookup);
		break;
	case PORT_ACT_LOOK_DOWN:
		(state)?KeyDownPort(&in_lookdown):KeyUpPort(&in_lookdown);
		break;
	case PORT_ACT_MOVE_LEFT:
		(state)?KeyDownPort(&in_moveleft):KeyUpPort(&in_moveleft);
		break;
	case PORT_ACT_MOVE_RIGHT:
		(state)?KeyDownPort(&in_moveright):KeyUpPort(&in_moveright);
		break;
	case PORT_ACT_STRAFE:
		(state)?KeyDownPort(&in_strafe):KeyUpPort(&in_strafe);
		break;
	case PORT_ACT_SPEED:
		(state)?KeyDownPort(&in_speed):KeyUpPort(&in_speed);
		break;
	case PORT_ACT_USE:
		(state)?KeyDownPort(&in_buttons[5]):KeyUpPort(&in_buttons[5]);
		break;
	case PORT_ACT_ATTACK:
		(state)?KeyDownPort(&in_buttons[0]):KeyUpPort(&in_buttons[0]);
		break;
	case PORT_ACT_JUMP:
		//Jump is same as up
	case PORT_ACT_UP:
		(state)?KeyDownPort(&in_up):KeyUpPort(&in_up);
		break;
	case PORT_ACT_DOWN:
		(state)?KeyDownPort(&in_down):KeyUpPort(&in_down);
		break;
	case PORT_ACT_NEXT_WEP:
		if (state)
			postCommand("weapnext\n");
		break;
	case PORT_ACT_PREV_WEP:
		if (state)
			postCommand("weapprev\n");
		break;
	case PORT_ACT_QUICKSAVE:
		if (state)
			postCommand("save quick\n");
		break;
	case PORT_ACT_QUICKLOAD:
		if (state)
			postCommand("load quick\n");
		break;
	case PORT_ACT_SAVE_MENU:
		if (state)
			postCommand("uimenu ingamesavemenu\n");
		break;
	case PORT_ACT_LOAD_MENU:
		if (state)
			postCommand("uimenu ingameloadmenu\n");
		break;


	case PORT_ACT_INVUSE:
		if (state)
			postCommand("invuse\n");
		break;
	case PORT_ACT_INVPREV:
		if (state)
			postCommand("invprev\n");
		break;
	case PORT_ACT_INVNEXT:
		if (state)
			postCommand("invnext\n");
		break;
	case PORT_ACT_NEXT_FORCE:
		if (state)
			postCommand("forcenext\n");
		break;
	case PORT_ACT_PREV_FORCE:
		if (state)
			postCommand("forceprev\n");
		break;
	case PORT_ACT_FORCE_USE:
		if (state)
			postCommand("+useforce\n");
		else
			postCommand("-useforce\n");
		break;
	case PORT_ACT_DATAPAD:
	case PORT_ACT_HELPCOMP:
		if (state)
			postCommand("datapad\n");
		break;

	case PORT_ACT_FORCE_SELECT:
		if (state)
		{
			sprintf(createCommand,"forceselect %d",param);
			postCommand(createCommand);
		}
		break;
	case PORT_ACT_WEAPON_SELECT:
		if (state)
		{
			sprintf(createCommand,"weapon %d",param);
			postCommand(createCommand);
		}
		break;
	case PORT_ACT_SABER_SEL:
		if (state)
			postCommand("weapon 1\n");
		break;
	case PORT_ACT_ALT_ATTACK:
		(state)?KeyDownPort(&in_buttons[7]):KeyUpPort(&in_buttons[7]);
		break;
	case PORT_ACT_SABER_STYLE:
		if (state)
			postCommand("saberAttackCycle\n");
		break;
	case PORT_ACT_THIRD_PERSON:
		if (state)
			postCommand("cg_thirdperson !\n");
		break;
	case PORT_ACT_FORCE_PULL:
		if (state)
			postCommand("force_pull\n");
		break;
	case PORT_ACT_FORCE_MIND:
		if (state)
			postCommand("force_distract\n");
		break;
	case PORT_ACT_FORCE_PUSH:
		if (state)
			postCommand("force_throw\n");
		break;
	case PORT_ACT_FORCE_SPEED:
		if (state)
			postCommand("force_speed\n");
		break;
	case PORT_ACT_FORCE_HEAL:
		if (state)
			postCommand("force_heal\n");
		break;
	case PORT_ACT_FORCE_LIGHT:
		(state)?KeyDownPort(&in_buttons[1]):KeyUpPort(&in_buttons[1]);
		break;
	case PORT_ACT_FORCE_GRIP:
		(state)?KeyDownPort(&in_buttons[6]):KeyUpPort(&in_buttons[6]);
		break;
	case PORT_ACT_FORCE_DRAIN:
		(state)?KeyDownPort(&in_buttons[3]):KeyUpPort(&in_buttons[3]);
		break;
	case PORT_ACT_FORCE_RAGE:
		if (state)
			postCommand("force_rage\n");
		break;
	case PORT_ACT_FORCE_PROTECT:
		if (state)
			postCommand("force_protect\n");
		break;
	case PORT_ACT_FORCE_ABSORB:
		if (state)
			postCommand("force_absorb\n");
		break;
	case PORT_ACT_FORCE_SIGHT:
		if (state)
			postCommand("force_sight\n");
		break;
	}
}

int mdx=0,mdy=0;
void PortableMouse(float dx,float dy)
{
	//LOGI("dx = %f, dy = %f",dx,dy);
	dx *= 1500;
	dy *= 1200;

	mdx += dx;
	mdy += dy;
}

int absx=0,absy=0;
void PortableMouseAbs(float x,float y)
{
	Sys_QueEvent( 0, SE_MOUSE_ABS, x, y, 0, NULL );

}

// =================== FORWARD and SIDE MOVMENT ==============

float forwardmove, sidemove; //Joystick mode

void PortableMoveFwd(float fwd)
{
	if (fwd > 1)
		fwd = 1;
	else if (fwd < -1)
		fwd = -1;

	forwardmove = fwd;
}

void PortableMoveSide(float strafe)
{
	if (strafe > 1)
		strafe = 1;
	else if (strafe < -1)
		strafe = -1;

	sidemove = strafe;
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



void PortableCommand(const char * cmd){
	postCommand(cmd);
}

void PortableInit(int argc,const char ** argv){

	int len, i;
	char  *cmdline;
	// merge the command line, this is kinda silly
	for ( len = 1, i = 1; i < argc; i++ )
		len += strlen( argv[i] ) + 1;
	cmdline = malloc( len );
	*cmdline = 0;
	for ( i = 1; i < argc; i++ )
	{
		if ( i > 1 ) {
			strcat( cmdline, " " );
		}
		strcat( cmdline, argv[i] );
	}
	Com_Init(cmdline);

	srand (time(NULL));


}

void pumpEvents(void)
{
	struct eventlist_s *ev;

	if (events_used != events_avail)
	{
		ev = &eventlist[events_used & (EVENTQUEUELENGTH-1)];

		LOGI("Queue event");
		Sys_QueEvent( 0, SE_KEY, ev->scancode, ev->state?qtrue:qfalse, 0, NULL );

		if( ev->unicode &&  ev->state)
			Sys_QueEvent( 0, SE_CHAR,ev->unicode, 0, 0, NULL );

		events_used++;
	}

	//Ok so can not issue commands more than 60 times/sec, who cares!
	if (postedCommand)
	{
		Cmd_ExecuteString(postedCommand);
		postedCommand = 0;
	}


	if (mdx || mdy)
		Sys_QueEvent( 0, SE_MOUSE, -mdx, -mdy, 0, NULL );
	mdx=mdy=0;


	/*
	if (absx || absy)
		Sys_QueEvent( 0, SE_MOUSE_ABS, absx, absy, 0, NULL );
	absx = 0;
	absy = 0;
	 */


}

void CL_AndroidMove( usercmd_t *cmd )
{

	//cmd->forwardmove += forwardmove * cl_forwardspeed->value * 2; //Always run! (x2)
	//cmd->rightmove  += sidemove   * cl_sidespeed->value * 2;
	cmd->forwardmove = ClampChar(cmd->forwardmove + forwardmove * 127 );
	cmd->rightmove = ClampChar(cmd->rightmove + sidemove * 127 );

	if ((abs(cmd->forwardmove) > 80) || (abs(cmd->rightmove) > 80))
		cmd->buttons &= ~BUTTON_WALKING;
	else
		cmd->buttons |= BUTTON_WALKING;

	switch(look_pitch_mode)
	{
	case LOOK_MODE_MOUSE:
		cl.viewangles[PITCH] += look_pitch_mouse * 300;
		look_pitch_mouse = 0;
		break;
	case LOOK_MODE_ABSOLUTE:
		cl.viewangles[PITCH] = look_pitch_abs * 80;
		break;
	case LOOK_MODE_JOYSTICK:
		cl.viewangles[PITCH] += look_pitch_joy * 6;
		break;
	}


	switch(look_yaw_mode)
	{
	case LOOK_MODE_MOUSE:
		cl.viewangles[YAW] += look_yaw_mouse * 300;
		look_yaw_mouse = 0;
		break;
	case LOOK_MODE_JOYSTICK:
		cl.viewangles[YAW] += look_yaw_joy * 6;
		break;
	}

}

pthread_t thread1;
int thread_has_run = 0;
int ssetup_ufile_bad=0;
extern void launchSSetup();
void PortableFrame(void){

	GL_State( glState.glStateBits);

	qglTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	qglBindTexture (GL_TEXTURE_2D, glState.currenttextures[glState.currenttmu]);

	pumpEvents();
	//IN_Frame( );
	Com_Frame( );
}

int PortableInMenu(void){

	if ((Key_GetCatcher( ) & KEYCATCH_UI) ||
			(Key_GetCatcher( ) & KEYCATCH_CONSOLE) )
		return 1;
	else
		return 0;

}

int PortableInAutomap(void)
{
	return 0;
}

int PortableShowKeyboard(void){

	return 0;
}

