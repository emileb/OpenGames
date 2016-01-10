/*
 Copyright (C) 1997-2001 Id Software, Inc.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

 */

#include <termios.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>

#if defined (__linux__)
#include <asm/io.h>
#include <sys/vt.h>
#endif

//#include "../ref_soft/r_local.h"
//#include "../client/keys.h"
#include "../client/client.h"

#include "rw_android.h"
#include "quake2-jni.h"

#include <android/log.h>

/*****************************************************************************/
/* KEYBOARD                                                                  */
/*****************************************************************************/

static Key_Event_fp_t Key_Event_fp;



#define MAX_KEY_EVENTS 128

static int keyEvents[MAX_KEY_EVENTS];
static int eventId ;

void KBD_Init(Key_Event_fp_t fp) {
	//__android_log_print(ANDROID_LOG_DEBUG, "KBD", "KBD_Init\n");
	Key_Event_fp = fp;
	eventId = 0;
}


void KBD_Update(void) {

	int k;

	//__android_log_print(ANDROID_LOG_DEBUG, "rw_in_android.c", "KBD_Update\n");

	for (k=0;k<eventId;k++){
		int event = keyEvents[k];

		int key = event & 0x00ff;
		int down = (event>>8) & 0x00ff;

		Key_Event_fp(key, down);
	}

	eventId = 0;
}



void quake2_jni_key_event( int key, int down) 
{

	if ( eventId < MAX_KEY_EVENTS )
		keyEvents[eventId++] = key | down<<8;

}


void KBD_Close(void) {

}

/*****************************************************************************/
/* MOUSE                                                                     */
/*****************************************************************************/


static in_state_t *in_state;


static struct {
	int mode;
	int forwardmove, sidemove, upmove;
	float pitch, yaw, roll;
} event;



// this is inside the renderer shared lib, so these are called from vid_so


void RW_IN_Init(in_state_t *in_state_p) {

	in_state = in_state_p;

	event.mode = 0;

	//__android_log_print(ANDROID_LOG_DEBUG, "KBD", "RW_IN_Init\n");
}

void RW_IN_Shutdown(void) {
	//__android_log_print(ANDROID_LOG_DEBUG, "KBD", "RW_IN_Shutdown\n");
}

/*
 ===========
 IN_Commands
 ===========
 */
void RW_IN_Commands(void) {
	//__android_log_print(ANDROID_LOG_DEBUG, "KBD", "RW_IN_Commands\n");
}

/*
 ===========
 IN_Move
 ===========
 */

extern client_static_t	cls;

keydest_t getKeyDest()
{
	return cls.key_dest;
}

#include "in_android.h"
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

int PortableKeyEvent(int state, int code)
{
	if ( eventId < MAX_KEY_EVENTS )
		keyEvents[eventId++] = (code&0xFF) | state<<8;

	return 0;
}

void KeyUpPort (kbutton_t *b)
{
	b->state = 4; 		// impulse up
}
void KeyDownPort (kbutton_t *b)
{
	b->state |= 1 + 2; // down + impulse down
}
extern kbutton_t	in_mlook, in_klook;
extern kbutton_t	in_left, in_right, in_forward, in_back;
extern kbutton_t	in_lookup, in_lookdown, in_moveleft, in_moveright;
extern kbutton_t	in_strafe, in_speed, in_use, in_attack;
extern kbutton_t	in_up, in_down;
void PortableAction(int state, int action)
{
	if ((getKeyDest() != key_game))
	{
		if (action >= PORT_ACT_MENU_UP && action <= PORT_ACT_MENU_BACK)
		{

			int sdl_code [] = { K_UPARROW, K_DOWNARROW, K_LEFTARROW,
					K_RIGHTARROW, K_ENTER, K_ESCAPE };
			PortableKeyEvent(state, sdl_code[action-PORT_ACT_MENU_UP]);
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
		(state)?KeyDownPort(&in_use):KeyUpPort(&in_use);
		break;
	case PORT_ACT_ATTACK:
		(state)?KeyDownPort(&in_attack):KeyUpPort(&in_attack);
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
			Cmd_ExecuteString("weapnext\n");
		break;
	case PORT_ACT_PREV_WEP:
		if (state)
			Cmd_ExecuteString("weapprev\n");
		break;
	case PORT_ACT_INVEN:
		if (state)
			Cmd_ExecuteString("inven\n");
		break;
	case PORT_ACT_INVUSE:
		if (state)
			Cmd_ExecuteString("invuse\n");
		break;
	case PORT_ACT_INVDROP:
		if (state)
			Cmd_ExecuteString("invdrop\n");
		break;
	case PORT_ACT_INVPREV:
		if (state)
			Cmd_ExecuteString("invprev\n");
		break;
	case PORT_ACT_INVNEXT:
		if (state)
			Cmd_ExecuteString("invnext\n");
		break;
	case PORT_ACT_HELPCOMP:
		if (state)
			Cmd_ExecuteString("cmd help\n");
		break;
		break;
	}
}

char * quickCommand = 0;
void PortableCommand(char * cmd)
{
	LOGI("PortableCommand %s",cmd);
	quickCommand = cmd;
}

void quake2_jni_move_event( int mode, 
		int forwardmove, int sidemove, int upmove,
		float pitch, float yaw, float roll
) {

	/*
	__android_log_print(ANDROID_LOG_DEBUG, "rw_in_android.c", 
					"quake2_jni_move_event forwardmove= %d yaw= %.0f\n", forwardmove, yaw);
	 */
	event.mode = mode;

	event.forwardmove = forwardmove;
	event.sidemove = sidemove;
	event.upmove = upmove;


	event.pitch = pitch;
	event.yaw = yaw;
	event.roll = roll;
}

extern	cvar_t	*cl_forwardspeed;
extern	cvar_t	*cl_sidespeed;
void RW_IN_Move(usercmd_t *cmd) {

	//__android_log_print(ANDROID_LOG_DEBUG, "rw_in_android.c", "RW_IN_Move\n");
	if (quickCommand)
	{
		Cmd_ExecuteString(quickCommand);
		quickCommand = 0;
	}

	cmd->forwardmove += forwardmove * cl_forwardspeed->value * 2; //Always run! (x2)
	cmd->sidemove  += sidemove   * cl_sidespeed->value * 2;

	switch(look_pitch_mode)
	{
	case LOOK_MODE_MOUSE:
		in_state->viewangles[PITCH] += look_pitch_mouse * 200;
		look_pitch_mouse = 0;
		break;
	case LOOK_MODE_ABSOLUTE:
		in_state->viewangles[PITCH] = look_pitch_abs * 80;
		break;
	case LOOK_MODE_JOYSTICK:
		in_state->viewangles[PITCH] += look_pitch_joy * 6;
		break;
	}


	switch(look_yaw_mode)
	{
	case LOOK_MODE_MOUSE:
		in_state->viewangles[YAW] += look_yaw_mouse * 200;
		look_yaw_mouse = 0;
		break;
	case LOOK_MODE_JOYSTICK:
		in_state->viewangles[YAW] += look_yaw_joy * 6;
		break;
	}

	/*
	if ( event.mode == 1 ){ // delta mode


		cmd->forwardmove += event.forwardmove;
		cmd->sidemove += event.sidemove;
		cmd->upmove += event.upmove;

		in_state->viewangles[PITCH] += event.pitch;
		in_state->viewangles[YAW] += event.yaw ;
		in_state->viewangles[ROLL] += event.roll ;

	} else if ( event.mode == 2 ){

		cmd->forwardmove += event.forwardmove;
		cmd->sidemove += event.sidemove;
		cmd->upmove += event.upmove;

		in_state->viewangles[PITCH] = event.pitch;
		in_state->viewangles[YAW] += event.yaw ;
		in_state->viewangles[ROLL] += event.roll ;
	}

	event.mode = 0; // no event 
	 */


}

void RW_IN_Frame(void) {
	//__android_log_print(ANDROID_LOG_DEBUG, "KBD", "RW_IN_Frame\n");
}

void RW_IN_Activate(void) {
	//__android_log_print(ANDROID_LOG_DEBUG, "KBD", "RW_IN_Activate\n");
}

