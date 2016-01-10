//Generic input code.
//mostly mouse support, but can also handle a few keyboard events.

#include "quakedef.h"

extern qboolean mouse_active;

static cvar_t m_filter = CVARF("m_filter", "0", CVAR_ARCHIVE);
static cvar_t m_accel = CVARF("m_accel", "0", CVAR_ARCHIVE);
static cvar_t m_forcewheel = CVARD("m_forcewheel", "1", "0: ignore mousewheels in apis where it is abiguous.\n1: Use mousewheel when it is treated as a third axis. Motion above a threshold is ignored, to avoid issues with an unknown threshold.\n2: Like 1, but excess motion is retained. The threshold specifies exact z-axis distance per notice.");
static cvar_t m_forcewheel_threshold = CVARD("m_forcewheel_threshold", "32", "Mousewheel graduations smaller than this will not trigger mousewheel deltas.");
static cvar_t m_strafeonright = CVARFD("m_strafeonright", "1", CVAR_ARCHIVE, "If 1, touching the right half of the touchscreen will strafe/move, while the left side will turn.");
static cvar_t m_fatpressthreshold = CVARFD("m_fatpressthreshold", "0.5", CVAR_ARCHIVE, "How fat your thumb has to be to register a fat press (touchscreens).");
static cvar_t m_slidethreshold = CVARFD("m_slidethreshold", "5", CVAR_ARCHIVE, "How far your finger needs to move to be considered a slide event (touchscreens).");

extern cvar_t cl_forcesplitclient;	//all devices claim to be a single player
extern cvar_t _windowed_mouse;


#define EVENTQUEUELENGTH 128
struct eventlist_s
{
	enum
	{
		IEV_KEYDOWN,
		IEV_KEYRELEASE,
		IEV_MOUSEABS,
		IEV_MOUSEDELTA
	} type;
	int devid;

	union
	{
		struct
		{
			float x, y, z;
			float tsize;	//the size of the touch
		} mouse;
		struct
		{
			int scancode, unicode;
		} keyboard;
	};
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

#define MAXPOINTERS 8
struct mouse_s
{
	enum
	{
		M_INVALID,
		M_MOUSE,	//using deltas
		M_TOUCH		//using absolutes
	} type;
	int qdeviceid;
	vec2_t oldpos;
	vec2_t downpos;
	float moveddist;	//how far it has moved while held. this provides us with our emulated mouse1 when they release the press
	vec2_t delta;		//how far its moved recently
	vec2_t old_delta;	//how far its moved previously, for mouse smoothing
	float wheeldelta;
	int down;
} ptr[MAXPOINTERS];



void IN_Shutdown(void)
{
	INS_Shutdown();
}

void IN_ReInit(void)
{
	int i;

	events_avail = 0;
	events_used = 0;

	for (i = 0; i < MAXPOINTERS; i++)
	{
		ptr[i].type = M_INVALID;
		ptr[i].qdeviceid = i;
	}

	INS_ReInit();
}

void IN_Init(void)
{
	Cvar_Register (&m_filter, "input controls");
	Cvar_Register (&m_accel, "input controls");
	Cvar_Register (&m_forcewheel, "Input Controls");
	Cvar_Register (&m_forcewheel_threshold, "Input Controls");
	Cvar_Register (&m_strafeonright, "input controls");
	Cvar_Register (&m_fatpressthreshold, "input controls");
	Cvar_Register (&m_slidethreshold, "input controls");

	INS_Init();
}

/*a 'pointer' is either a multitouch pointer, or a separate device
note that mice use the keyboard button api, but separate devices*/
void IN_Commands(void)
{
	struct eventlist_s *ev;

	INS_Commands();

	while (events_used != events_avail)
	{
		ev = &eventlist[events_used & (EVENTQUEUELENGTH-1)];
		switch(ev->type)
		{
		case IEV_KEYDOWN:
		case IEV_KEYRELEASE:
			//on touchscreens, mouse1 is used as up/down state. we have to emulate actual mouse clicks based upon distance moved, so we can get movement events.
			if (ev->keyboard.scancode == K_MOUSE1 && ev->devid < MAXPOINTERS && (ptr[ev->devid].type == M_TOUCH))
			{
				if (Key_MouseShouldBeFree())
					ptr[ev->devid].down = 0;
				else
				{
					if (ev->type == IEV_KEYDOWN)
					{
						ptr[ev->devid].down = 1;
						ptr[ev->devid].moveddist = 0;
						ptr[ev->devid].downpos[0] = ptr[ev->devid].oldpos[0];
						ptr[ev->devid].downpos[1] = ptr[ev->devid].oldpos[1];
						ptr[ev->devid].delta[0] = 0;
						ptr[ev->devid].delta[1] = 0;

						if (ev->mouse.tsize > m_fatpressthreshold.value)
						{
							int key = (m_strafeonright.ival && ptr[ev->devid].downpos[0] > vid.pixelwidth/2)?K_MOUSE2:K_MOUSE1;
							Key_Event(ev->devid, key, 0, true);
							ptr[ev->devid].down = 2;
						}
					}
					else
					{
						if (ptr[ev->devid].down > 1)
						{
							int key = (m_strafeonright.ival && ptr[ev->devid].downpos[0] > vid.pixelwidth/2)?K_MOUSE2:K_MOUSE1;
							Key_Event(ev->devid, key, 0, false);
							ptr[ev->devid].down = 1;
						}
						if (ptr[ev->devid].down)
						{
							if (ptr[ev->devid].moveddist < m_slidethreshold.value)
							{
								/*if its on the right, make it a mouse2*/
								int key = (m_strafeonright.ival && ptr[ev->devid].downpos[0] > vid.pixelwidth/2)?K_MOUSE2:K_MOUSE1;
								Key_Event(ev->devid, key, 0, true);
								Key_Event(ev->devid, key, 0, false);
							}
						}
						ptr[ev->devid].down = 0;
					}
					break;
				}
			}
			Key_Event(ev->devid, ev->keyboard.scancode, ev->keyboard.unicode, ev->type == IEV_KEYDOWN);
			break;
		case IEV_MOUSEDELTA:
			if (ev->devid < MAXPOINTERS)
			{
				if (ptr[ev->devid].type != M_MOUSE)
				{
					ptr[ev->devid].type = M_MOUSE;
				}
				ptr[ev->devid].delta[0] += ev->mouse.x;
				ptr[ev->devid].delta[1] += ev->mouse.y;

				if (m_forcewheel.value >= 2)
					ptr[ev->devid].wheeldelta -= ev->mouse.z;
				else if (m_forcewheel.value)
				{
					int mfwt = (int)m_forcewheel_threshold.value;

					if (ev->mouse.z > mfwt)
						ptr[ev->devid].wheeldelta -= mfwt;
					else if (ev->mouse.z < -mfwt)
						ptr[ev->devid].wheeldelta += mfwt;
				}
			}
			break;
		case IEV_MOUSEABS:
			/*mouse cursors only really work with one pointer*/
			if (ev->devid == 0)
			{
				float fl;
				fl = ev->mouse.x * vid.width / vid.pixelwidth;
				mousecursor_x = bound(0, fl, vid.width-1);
				fl = ev->mouse.y * vid.height / vid.pixelheight;
				mousecursor_y = bound(0, fl, vid.height-1);
			}

			if (ev->devid < MAXPOINTERS)
			{
				if (ptr[ev->devid].type != M_TOUCH)
				{
					//if its now become an absolute device, clear stuff so we don't get confused.
					ptr[ev->devid].type = M_TOUCH;
					ptr[ev->devid].down = 0;
					ptr[ev->devid].moveddist = 0;
					ptr[ev->devid].oldpos[0] = ev->mouse.x;
					ptr[ev->devid].oldpos[1] = ev->mouse.y;
				}

				if (ptr[ev->devid].down)
				{
					ptr[ev->devid].delta[0] += ev->mouse.x - ptr[ev->devid].oldpos[0];
					ptr[ev->devid].delta[1] += ev->mouse.y - ptr[ev->devid].oldpos[1];

					ptr[ev->devid].moveddist += fabs(ev->mouse.x - ptr[ev->devid].oldpos[0]) + fabs(ev->mouse.y - ptr[ev->devid].oldpos[1]);
				}

				ptr[ev->devid].oldpos[0] = ev->mouse.x;
				ptr[ev->devid].oldpos[1] = ev->mouse.y;


				if (ptr[ev->devid].down > 1 && ev->mouse.tsize < m_fatpressthreshold.value)
				{
					int key = (m_strafeonright.ival && ptr[ev->devid].downpos[0] > vid.pixelwidth/2)?K_MOUSE2:K_MOUSE1;
					Key_Event(ev->devid, key, 0, false);
					ptr[ev->devid].down = 1;
				}
				if (ptr[ev->devid].down == 1 && ev->mouse.tsize > m_fatpressthreshold.value)
				{
					int key = (m_strafeonright.ival && ptr[ev->devid].downpos[0] > vid.pixelwidth/2)?K_MOUSE2:K_MOUSE1;
					Key_Event(ev->devid, key, 0, true);
					ptr[ev->devid].down = 2;
				}
			}
			break;
		}
		events_used++;
	}
}

void IN_MoveMouse(struct mouse_s *mouse, float *movements, int pnum)
{

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
	IN_KeyEvent(0,state,code,0);
	return 0;
}

void KeyUpPort (kbutton_t *b)
{
	b->state[0] = 4; 		// impulse up
}
void KeyDownPort (kbutton_t *b)
{
	b->state[0] |= 1 + 2; // down + impulse down
}
extern kbutton_t	in_mlook, in_klook;
extern kbutton_t	in_left, in_right, in_forward, in_back;
extern kbutton_t	in_lookup, in_lookdown, in_moveleft, in_moveright;
extern kbutton_t	in_strafe, in_speed, in_use, in_jump, in_attack;
extern kbutton_t	in_up, in_down;
void PortableAction(int state, int action)
{
	LOGI("PortableAction %d %d",state, action);

	if (key_dest != key_game)
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
			Cmd_ExecuteString("impulse 10\n", RESTRICT_LOCAL);
		break;
	case PORT_ACT_PREV_WEP:
		if (state)
			Cmd_ExecuteString("impulse 12\n", RESTRICT_LOCAL);
		break;
	}
}

char * quickCommand = 0;
void PortableCommand(char * cmd)
{
	quickCommand = cmd;
}

void IN_Move (float *movements, int pnum)
{
	if (quickCommand)
	{
		Cmd_ExecuteString(quickCommand, RESTRICT_LOCAL);
		quickCommand = 0;
	}

	//Sys_Printf("IN_Move");
	int i;

	if (movements)
	{
		movements[0]  += forwardmove * cl_forwardspeed.value;
		movements[1]  += sidemove   * cl_forwardspeed.value;
	}



	V_StopPitchDrift (pnum);

	switch(look_pitch_mode)
	{
	case LOOK_MODE_MOUSE:
		cl.playerview[pnum].viewanglechange[PITCH] += look_pitch_mouse * 200;
		look_pitch_mouse = 0;
		break;
	case LOOK_MODE_ABSOLUTE:
		cl.playerview[pnum].viewangles[PITCH] = look_pitch_abs * 80;
		break;
	case LOOK_MODE_JOYSTICK:
		cl.playerview[pnum].viewanglechange[PITCH] = look_pitch_joy * 6;
		break;
	}


	switch(look_yaw_mode)
	{
	case LOOK_MODE_MOUSE:
		cl.playerview[pnum].viewanglechange[YAW] += look_yaw_mouse * 200;
		look_yaw_mouse = 0;
		break;
	case LOOK_MODE_JOYSTICK:
		cl.playerview[pnum].viewanglechange[YAW] = look_yaw_joy * 6;
		break;
	}


	if (cl.playerview[pnum].viewanglechange[PITCH] > 80)
		cl.playerview[pnum].viewanglechange[PITCH] = 80;
	if (cl.playerview[pnum].viewanglechange[PITCH]< -70)
		cl.playerview[pnum].viewanglechange[PITCH] = -70;
}

void IN_KeyEvent(int devid, int down, int keycode, int unicode)
{
	struct eventlist_s *ev = in_newevent();
	if (!ev)	
		return;
	ev->type = down?IEV_KEYDOWN:IEV_KEYRELEASE;
	ev->devid = devid;
	ev->keyboard.scancode = keycode;
	ev->keyboard.unicode = unicode;
	in_finishevent();
}

/*
devid is the mouse device id. generally idependant from keyboards.
for multitouch, devid might be the touch identifier, which will persist until released.
x is horizontal, y is vertical.
z is height... generally its used as a mousewheel instead, but there are some '3d' mice out there, so its provided in this api.
 */
void IN_MouseMove(int devid, int abs, float x, float y, float z, float size)
{

}
