
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

#include "code/client/client.h"
#include "code/renderergl1/tr_local.h"


#ifndef DEDICATED
#ifdef USE_LOCAL_HEADERS
#	include "SDL.h"
#	include "SDL_cpuinfo.h"
#else
#	include <SDL.h>
#	include <SDL_cpuinfo.h>
#endif
#endif

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"JNI", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "JNI", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"JNI", __VA_ARGS__))


#include "../qcommon/q_shared.h"
#include "../qcommon/qcommon.h"


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



int PortableKeyEvent(int state, int code, int unicode){

	LOGI("PortableKeyEvent state = %d, code =  %d, unicode = %d",state,code,unicode);
	struct eventlist_s *ev = in_newevent();
	if (!ev)
		return 0;

	ev->scancode = code;
	ev->unicode = unicode;
	ev->state = state;
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

char* postedCommand = 0;
void postCommand(const char * cmd)
{
	postedCommand = cmd;
}

extern kbutton_t	in_left, in_right, in_forward, in_back;
extern kbutton_t	in_lookup, in_lookdown, in_moveleft, in_moveright;
extern kbutton_t	in_strafe, in_speed;
extern kbutton_t	in_up, in_down;
extern  kbutton_t	in_buttons[16];

static zoomed = 0; //toggle zoom
void PortableAction(int state, int action)
{
	LOGI("PortableAction %d   %d",state,action);

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
	case PORT_ACT_ZOOM_IN:
		if (state)
		{
			if (zoomed)
				postCommand("-zoom\n");
			else
				postCommand("+zoom\n");
			zoomed = !zoomed;
		}
		break;
	case PORT_ACT_USE:
		(state)?KeyDownPort(&in_buttons[2]):KeyUpPort(&in_buttons[2]);
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
		//TODO make fifo, possibly not thread safe!!
	case PORT_ACT_NEXT_WEP:
		if (state)
			postCommand("weapnext\n");
		break;
	case PORT_ACT_PREV_WEP:
		if (state)
			postCommand("weapprev\n");
		break;
	case PORT_ACT_CUSTOM_0:
		PortableKeyEvent(state,K_F1,0);
		break;
	case PORT_ACT_CUSTOM_1:
		PortableKeyEvent(state,K_F2,0);
		break;
	case PORT_ACT_CUSTOM_2:
		PortableKeyEvent(state,K_F3,0);
		break;
	case PORT_ACT_CUSTOM_3:
		PortableKeyEvent(state,K_F4,0);
		break;

		break;
	}
}

void PortableMouse(float dx,float dy)
{
	//LOGI("dx = %f, dy = %f",dx,dy);
	dx *= 1500;
	dy *= 1200;

	Com_QueueEvent( 0, SE_MOUSE, -dx, -dy, 0, NULL );

}

int absx=0,absy=0;
void PortableMouseAbs(float x,float y)
{
	absx = x;
	absy = y;
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
	//Copied form sys_main.c
	int   i;
	char  commandLine[ MAX_STRING_CHARS ] = { 0 };
	Sys_PlatformInit( );

	// Set the initial time base
	Sys_Milliseconds( );

	Sys_ParseArgs( argc, argv );
	Sys_SetBinaryPath( Sys_Dirname( argv[ 0 ] ) );
	Sys_SetDefaultInstallPath(  Sys_BinaryPath() );

	// Concatenate the command line for passing to Com_Init
	for( i = 1; i < argc; i++ )
	{
		const qboolean containsSpaces = strchr(argv[i], ' ') != NULL;
		if (containsSpaces)
			Q_strcat( commandLine, sizeof( commandLine ), "\"" );

		Q_strcat( commandLine, sizeof( commandLine ), argv[ i ] );

		if (containsSpaces)
			Q_strcat( commandLine, sizeof( commandLine ), "\"" );

		Q_strcat( commandLine, sizeof( commandLine ), " " );
	}

	Com_Init( commandLine );
	NET_Init( );

	CON_Init( );
}

void pumpEvents(void)
{
	struct eventlist_s *ev;

	if (events_used != events_avail)
	{
		ev = &eventlist[events_used & (EVENTQUEUELENGTH-1)];

		LOGI("Queue event");
		Com_QueueEvent( 0, SE_KEY, ev->scancode, ev->state?qtrue:qfalse, 0, NULL );

		if( ev->unicode &&  ev->state)
			Com_QueueEvent( 0, SE_CHAR,ev->unicode, 0, 0, NULL );

		events_used++;
	}

	//Ok can not issue commands more than 60 times/sec!
	if (postedCommand)
	{
		Cmd_ExecuteString(postedCommand);
		postedCommand = 0;
	}

	if ( Key_GetCatcher( ) & KEYCATCH_UI ) {
		if (absx || absy)
		{
			//LOGI("Mouse %d %d",absx,absy);
			VM_Call( uivm, UI_MOUSE_EVENT_ABS, absx, absy );
		}
		absx = 0;
		absy = 0;
	}

}

void CL_AndroidMove( usercmd_t *cmd )
{

	//cmd->forwardmove += forwardmove * cl_forwardspeed->value * 2; //Always run! (x2)
	//cmd->rightmove  += sidemove   * cl_sidespeed->value * 2;
	cmd->forwardmove = ClampChar(cmd->forwardmove + forwardmove * 127 );
	cmd->rightmove = ClampChar(cmd->rightmove + sidemove * 127 );

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

void PortableFrame(void){

	qglBindTexture (GL_TEXTURE_2D, glState.currenttextures[glState.currenttmu]);

	pumpEvents();
	IN_Frame( );
	Com_Frame( );
}

int PortableInMenu(void){

	if ((Key_GetCatcher( ) & KEYCATCH_UI) ||
			(Key_GetCatcher( ) & KEYCATCH_CGAME) )
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

