#include "quakedef.h"

#include "in_android.h"

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
		return NULL;
	return &eventlist[events_avail & (EVENTQUEUELENGTH-1)];
}

static void in_finishevent(void)
{
	events_avail++;
}
///////////////////////



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
	LOGI("PortableKeyEvent %d %d",state,code);
	struct eventlist_s *ev = in_newevent();
	if (!ev)
		return;

	ev->scancode = code;
	ev->unicode = code;
	ev->state = state;
	in_finishevent();
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
extern kbutton_t	in_strafe, in_speed, in_use, in_jump, in_attack;
extern kbutton_t	in_up, in_down;
void PortableAction(int state, int action)
{
	LOGI("PortableAction %d %d",state, action);

	if (PortableGetScreenMode() == TS_MENU)
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
	case PORT_ACT_JUMP:
		(state)?KeyDownPort(&in_jump):KeyUpPort(&in_jump);
		break;
	case PORT_ACT_ATTACK:
		(state)?KeyDownPort(&in_attack):KeyUpPort(&in_attack);
		break;
	case PORT_ACT_UP:
		(state)?KeyDownPort(&in_up):KeyUpPort(&in_up);
		break;
	case PORT_ACT_DOWN:
		(state)?KeyDownPort(&in_down):KeyUpPort(&in_down);
		break;
		//TODO make fifo, possibly not thread safe!!
	case PORT_ACT_NEXT_WEP:
		if (state)
			PortableCommand("impulse 10\n");
		break;
	case PORT_ACT_PREV_WEP:
		if (state)
			PortableCommand("impulse 12\n");
		break;
	case PORT_MALICE_USE:
		if (state)
			PortableCommand("impulse 13\n");
		break;
	case PORT_MALICE_RELOAD:
		if (state)
			PortableCommand("impulse 12\n");
		break;
	case PORT_MALICE_CYCLE:
		if (state)
			PortableCommand("impulse 14\n");
		break;
	}
}

char * quickCommand = 0;
void PortableCommand(char * cmd)
{
	quickCommand = cmd;
}

/////////////////////
// Movement handling
////

void IN_Move( void )
{
	if (quickCommand)
	{
		Cmd_ExecuteString(quickCommand,src_command,true);
		quickCommand = 0;
	}


	cl.cmd.forwardmove  += forwardmove * cl_forwardspeed.value;
	cl.cmd.sidemove  += sidemove   * cl_sidespeed.value;

	switch(look_pitch_mode)
	{
	case LOOK_MODE_MOUSE:
		cl.viewangles[0] += look_pitch_mouse * 200;
		look_pitch_mouse = 0;
		break;
	case LOOK_MODE_ABSOLUTE:
		cl.viewangles[0] = look_pitch_abs * 80;
		break;
	case LOOK_MODE_JOYSTICK:
		cl.viewangles[0] += look_pitch_joy * 6;
		break;
	}


	switch(look_yaw_mode)
	{
	case LOOK_MODE_MOUSE:
		cl.viewangles[1] += look_yaw_mouse * 300;
		look_yaw_mouse = 0;
		break;
	case LOOK_MODE_JOYSTICK:
		cl.viewangles[1] += look_yaw_joy * 6;
		break;
	}

	if (cl.viewangles[0] > 80)
		cl.viewangles[0] = 80;
	if (cl.viewangles[0]< -70)
		cl.viewangles[0] = -70;
}


extern	keydest_t	key_dest;
touchscreemode_t PortableGetScreenMode()
{
	if ((key_dest != key_game) || key_consoleactive)
		return TS_MENU;
	else
	{
		if (cl.intermission || cls.demoplayback)
			return TS_BLANK;
		else
			return TS_GAME;
	}
}

int PortableShowKeyboard()
{
	//LOGI("key_dest = %d",key_dest);
	if (key_dest == key_console || key_dest == key_message || (key_dest == key_game && cls.state == ca_disconnected) || key_consoleactive)
		return 1;
	else
		return 0;
}


void Host_Init(void);
void PortableInit(int argc,const char ** argv)
{
	//LOGI("PortableInit");
	events_avail = 0;
	events_used = 0;

	com_argc = argc;
	com_argv = (const char **)argv;
	Sys_ProvideSelfFD();

	Host_Init();
	//Host_Main();
}

void Host_Frame();
void PortableFrame()
{
	//LOGI("PortableFrame");
	Host_Frame();
}


void Sys_SendKeyEvents( void )
{
	struct eventlist_s *ev;

	while (events_used != events_avail)
	{
		ev = &eventlist[events_used & (EVENTQUEUELENGTH-1)];
		Key_Event(ev->scancode, ev->unicode < 128? ev->unicode: 0, ev->state);
		events_used++;
	}
}



