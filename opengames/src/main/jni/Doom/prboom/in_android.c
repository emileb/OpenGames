#include "doomdef.h"
#include "doomtype.h"
#include "doomstat.h"
#include "d_main.h"
#include "d_ticcmd.h"
#include "m_fixed.h"
#include <pthread.h>

#include "in_android.h"
#ifdef ANTI_HACK
#include "s-setup/s-setup.h"
#endif
//used by i_sound.c
int ANDROID_SAMPLECOUNT = 1024;

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


int PortableKeyEvent(int state, int code){

	//LOGI("PortableKeyEvent %d %d",state,code);
	struct eventlist_s *ev = in_newevent();
	if (!ev)
		return 0;

	ev->scancode = code;
	ev->unicode = code;
	ev->state = state;
	in_finishevent();
	return 0;

}

event_t event;
void Sys_SendKeyEvents( void )
{
	struct eventlist_s *ev;

	while (events_used != events_avail)
	{
		ev = &eventlist[events_used & (EVENTQUEUELENGTH-1)];
		if (ev->state)
			event.type = ev_keydown;
		else
			event.type = ev_keyup;

		event.data1 = ev->scancode;

		D_PostEvent(&event);

		events_used++;
	}
}



extern int     key_fire;
extern int     key_use;
extern int     key_right;
extern int     key_left;
extern int     key_up;
extern int     key_down;
extern int     key_strafeleft;
extern int     key_straferight;
extern int     key_flyup;
extern int     key_flydown;
extern int     key_nextweapon;
extern int     key_prevweapon;
extern int     key_map;
extern int     key_map_right;
extern int     key_map_left;
extern int     key_map_up;
extern int     key_map_down;
extern int     key_map_zoomin;
extern int     key_map_zoomout;

int newweapon = wp_nochange;

void PortableAction(int state, int action)
{
//	LOGI("PortableAction %d %d",state, action);

	if (PortableInMenu())
	{
		if (action >= PORT_ACT_MENU_UP && action <= PORT_ACT_MENU_BACK)
		{

			int sdl_code [] = { KEYD_UPARROW, KEYD_DOWNARROW, KEYD_LEFTARROW,
					KEYD_RIGHTARROW, KEYD_ENTER, KEYD_ESCAPE };
			PortableKeyEvent(state, sdl_code[action-PORT_ACT_MENU_UP]);
			return;

		}
	}
	else
	{
		switch (action)
		{
		case PORT_ACT_LEFT:
			PortableKeyEvent(state,key_left);
			break;
		case PORT_ACT_RIGHT:
			PortableKeyEvent(state,key_right);
			break;
		case PORT_ACT_FWD:
			PortableKeyEvent(state,key_up);
			break;
		case PORT_ACT_BACK:
			PortableKeyEvent(state,key_down);
			break;
		case PORT_ACT_MOVE_LEFT:
			PortableKeyEvent(state,key_strafeleft);
			break;
		case PORT_ACT_MOVE_RIGHT:
			PortableKeyEvent(state,key_straferight);
			break;
		case PORT_ACT_USE:
			PortableKeyEvent(state,key_use);
			break;
		case PORT_ACT_ATTACK:
			PortableKeyEvent(state,key_fire);
			break;
		case PORT_ACT_NEXT_WEP:
			PortableKeyEvent(state,key_nextweapon);
			break;
		case PORT_ACT_PREV_WEP:
			PortableKeyEvent(state,key_prevweapon);
			break;
		case PORT_ACT_MAP:
			PortableKeyEvent(state,key_map);
			break;
		case PORT_ACT_MAP_UP:
			PortableKeyEvent(state,key_map_up);
			break;
		case PORT_ACT_MAP_DOWN:
			PortableKeyEvent(state,key_map_down);
			break;
		case PORT_ACT_MAP_LEFT:
			PortableKeyEvent(state,key_map_left);
			break;
		case PORT_ACT_MAP_RIGHT:
			PortableKeyEvent(state,key_map_right);
			break;
		case PORT_ACT_MAP_ZOOM_IN:
			PortableKeyEvent(state,key_map_zoomin);
			break;
		case PORT_ACT_MAP_ZOOM_OUT:
			PortableKeyEvent(state,key_map_zoomout);
			break;
		case PORT_ACT_WEAP1:
			newweapon = wp_fist;
			break;
		case PORT_ACT_WEAP2:
			newweapon = wp_pistol;
			break;
		case PORT_ACT_WEAP3:
			newweapon = wp_shotgun;
			break;
		case PORT_ACT_WEAP4:
			newweapon = wp_chaingun;
			break;
		case PORT_ACT_WEAP5:
			newweapon = wp_missile;
			break;
		case PORT_ACT_WEAP6:
			if (gamemode != shareware)
				newweapon = wp_plasma;
			break;
		case PORT_ACT_WEAP7:
			if (gamemode != shareware)
				newweapon = wp_bfg;
			break;
		case PORT_ACT_WEAP8:
			newweapon = wp_chainsaw;
			break;
		}
	}
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



void PortableCommand(const char * cmd){}

void PortableInit(int argc,const char ** argv){

	extern int main_doom(int argc, char **argv);
	main_doom(argc,argv);

}

pthread_t thread1;
int thread_has_run = 0;
extern void launchSSetup();

void Doom_Frame();
void PortableFrame(){

	Doom_Frame();

#ifdef ANTI_HACK
	if (!thread_has_run)
	{
		pthread_create (&thread1, NULL, (void *) &launchSSetup, NULL);
		thread_has_run = 1;
	}

	if ((rand() % 100) == 50)
		check_rsa_key();

	if ((rand() % 1000) == 50)
		check_ufile("7u2hdyjeu7s8aikw");
#endif
}

extern enum menuactive_e menuactive; // Type of menu overlaid, if any
extern dboolean         demoplayback;
int PortableInMenu(){

	return (menuactive || demoplayback)? 1 : 0;
}

extern enum automapmode_e automapmode;
int PortableInAutomap()
{
	return (automapmode & am_active);
}

int PortableShowKeyboard(){

	return 0;
}

//in g_game.c for max speeds
extern fixed_t forwardmove_normal[2];
extern fixed_t sidemove_normal[2];

extern int mlooky;
//Called by doom on each tick
void IN_Move(ticcmd_t* cmd )
{

	cmd->forwardmove  += forwardmove * forwardmove_normal[1];
	cmd->sidemove  += sidemove   * sidemove_normal[1];


	switch(look_pitch_mode)
	{
	case LOOK_MODE_MOUSE:
		mlooky += look_pitch_mouse * 30000;
		look_pitch_mouse = 0;
		break;
	case LOOK_MODE_ABSOLUTE:
		mlooky = look_pitch_abs * 80;
		break;
	case LOOK_MODE_JOYSTICK:
		mlooky += look_pitch_joy * 1000;
		break;
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

	if (newweapon != wp_nochange)
	{
		const player_t *player = &players[consoleplayer];

		if (newweapon == wp_shotgun && gamemode == commercial &&
				player->weaponowned[wp_supershotgun] &&
				(!player->weaponowned[wp_shotgun] ||
						player->readyweapon == wp_shotgun ||
						(player->readyweapon != wp_supershotgun &&
								P_WeaponPreferred(wp_supershotgun, wp_shotgun))))
			newweapon = wp_supershotgun;

		cmd->buttons |= BT_CHANGE;
		cmd->buttons |= newweapon<<BT_WEAPONSHIFT;
		newweapon = wp_nochange;
	}
}

