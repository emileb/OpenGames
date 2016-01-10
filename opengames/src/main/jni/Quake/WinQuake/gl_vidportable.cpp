/*
Copyright (C) 2007 The Android Open Source Project

 */

#include "quakedef.h"

unsigned short	d_8to16table[256];
unsigned	d_8to24table[256];

#ifdef SUPPORT_8BIT_MIPMAPGENERATION
unsigned char d_15to8table[65536];
#endif

cvar_t  mouse_button_commands[3] =
{
		CVAR2("mouse1","+attack"),
		CVAR2("mouse2","+strafe"),
		CVAR2("mouse3","+forward"),
};

static const int MOUSE_LEFTBUTTON = 1;
static const int MOUSE_MIDDLEBUTTON = 2;
static const int MOUSE_RIGHTBUTTON = 4;

bool     mouse_tap;
float   mouse_x, mouse_y;
float   old_mouse_x, old_mouse_y;
int     mx, my;
bool    mouse_buttonstate;
bool    mouse_oldbuttonstate;

cvar_t  m_filter = CVAR2("m_filter","1");

int scr_width, scr_height;

cvar_t	_windowed_mouse = CVAR3("_windowed_mouse","0", true);

/*-----------------------------------------------------------------------*/

//int		texture_mode = GL_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_LINEAR;
int		texture_mode = GL_LINEAR;
// int		texture_mode = GL_LINEAR_MIPMAP_NEAREST;
//int		texture_mode = GL_LINEAR_MIPMAP_LINEAR;

int		texture_extension_number = 1;

float		gldepthmin, gldepthmax;

cvar_t	gl_ztrick = CVAR2("gl_ztrick","0");

const char *gl_vendor;
const char *gl_renderer;
const char *gl_version;
const char *gl_extensions;

qboolean is8bit = false;
qboolean isPermedia = false;
qboolean gl_mtexable = false;

/*-----------------------------------------------------------------------*/
void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height)
{
}

void D_EndDirectRect (int x, int y, int width, int height)
{
}

void VID_Shutdown(void)
{
}

void VID_ShiftPalette(unsigned char *p)
{
	//	VID_SetPalette(p);
}

void	VID_SetPalette (unsigned char *palette)
{
	byte	*pal;
	unsigned r,g,b;
	unsigned v;
	int     r1,g1,b1;
	int		k;
	unsigned short i;
	unsigned	*table;
	FILE *f;
	char s[255];
	int dist, bestdist;
	static qboolean palflag = false;

	PMPBEGIN(("VID_SetPalette"));
	//
	// 8 8 8 encoding
	//
	Con_Printf("Converting 8to24\n");

	pal = palette;
	table = d_8to24table;
	for (i=0 ; i<256 ; i++)
	{
		r = pal[0];
		g = pal[1];
		b = pal[2];
		pal += 3;

		//		v = (255<<24) + (r<<16) + (g<<8) + (b<<0);
		//		v = (255<<0) + (r<<8) + (g<<16) + (b<<24);
		v = (255<<24) + (r<<0) + (g<<8) + (b<<16);
		*table++ = v;
	}
	d_8to24table[255] &= 0xffffff;	// 255 is transparent

#ifdef SUPPORT_8BIT_MIPMAPGENERATION

	// JACK: 3D distance calcs - k is last closest, l is the distance.
	// FIXME: Precalculate this and cache to disk.
	if (palflag)
		return;
	palflag = true;

	COM_FOpenFile("glquake/15to8.pal", &f);
	if (f) {
		fread(d_15to8table, 1<<15, 1, f);
		fclose(f);
	} else {
		PMPBEGIN(("Creating 15to8 palette"));
		for (i=0; i < (1<<15); i++) {
			/* Maps
       0000.0000.0000.0000
       0000.0000.0001.1111 = Red   = 0x001F
       0000.0011.1110.0000 = Green = 0x03E0
       0111.1100.0000.0000 = Blue  = 0x7C00
			 */
			r = ((i & 0x1F) << 3)+4;
			g = ((i & 0x03E0) >> (5-3)) +4;
			b = ((i & 0x7C00) >> (10-3))+4;
			pal = (unsigned char *)d_8to24table;
			for (v=0,k=0,bestdist=0x7FFFFFFF; v<256; v++,pal+=4) {
				r1 = (int)r - (int)pal[0];
				g1 = (int)g - (int)pal[1];
				b1 = (int)b - (int)pal[2];
				dist = ((r1*r1)+(g1*g1)+(b1*b1));
				if (dist < bestdist) {
					k=v;
					bestdist = dist;
				}
			}
			d_15to8table[i]=k;
		}
		PMPEND(("Creating 15to8 palette"));
		sprintf(s, "%s/glquake", com_gamedir);
		Sys_mkdir (s);
		sprintf(s, "%s/glquake/15to8.pal", com_gamedir);
		if ((f = fopen(s, "wb")) != NULL) {
			fwrite(d_15to8table, 1<<15, 1, f);
			fclose(f);
		}
		else
		{
			Con_Printf("Could not write %s\n", s);
		}
	}

#endif // SUPPORT_8BIT_MIPMAPGENERATION
	PMPEND(("VID_SetPalette"));
}

/*
===============
GL_Init
===============
 */
void GL_Init (void)
{
	gl_vendor = (char*) glGetString (GL_VENDOR);
	Con_Printf ("GL_VENDOR: %s\n", gl_vendor);
	GLCHECK("glGetString");
	gl_renderer = (char*) glGetString (GL_RENDERER);
	Con_Printf ("GL_RENDERER: %s\n", gl_renderer);
	GLCHECK("glGetString");

	gl_version = (char*) glGetString (GL_VERSION);
	Con_Printf ("GL_VERSION: %s\n", gl_version);
	GLCHECK("glGetString");
	gl_extensions = (char*) glGetString (GL_EXTENSIONS);
	Con_Printf ("GL_EXTENSIONS: %s\n", gl_extensions);
	GLCHECK("glGetString");

	//	Con_Printf ("%s %s\n", gl_renderer, gl_version);

	// Enable/disable multitexture:

	gl_mtexable = true;

	glClearColor (1,0,0,0);
	glCullFace(GL_FRONT);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.666);

#ifdef USE_OPENGLES
#else
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
#endif
	glShadeModel(GL_FLAT);
	glDisable(GL_DITHER);

	// perspective correction don't work well currently
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_DST_ALPHA);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

#ifdef USE_OPENGLES
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
}

/*
=================
GL_BeginRendering

=================
 */
void GL_BeginRendering (int *x, int *y, int *width, int *height)
{
	extern cvar_t gl_clear;

	*x = *y = 0;
	*width = scr_width;
	*height = scr_height;

	//    if (!wglMakeCurrent( maindc, baseRC ))
	//		Sys_Error ("wglMakeCurrent failed");

	//	glViewport (*x, *y, *width, *height);
}


void GL_EndRendering (void)
{
	//glFlush();
	// !!! Swap buffers.
}

void Init_KBD(void)
{
}

// This function controls whether or not 8-bit paletted textures are used:

qboolean VID_Is8bit(void)
{
	return 0;
}

static void Check_Gamma (unsigned char *pal)
{
	float vid_gamma;
	float	f, inf;
	unsigned char	palette[768];
	int		i;

	if ((i = COM_CheckParm("-gamma")) == 0) {
		vid_gamma = 0.5;	// brighten up game.
	} else
		vid_gamma = Q_atof(com_argv[i+1]);

	if(vid_gamma != 1)
	{
		for (i=0 ; i<768 ; i++)
		{
			f = pow ( (pal[i]+1)/256.0 , vid_gamma );
			inf = f*255 + 0.5;
			if (inf < 0)
				inf = 0;
			if (inf > 255)
				inf = 255;
			palette[i] = (unsigned char) inf;
		}
	}

	memcpy (pal, palette, sizeof(palette));
}

void VID_Init(unsigned char *palette)
{
	int i;
	GLint attribs[32];
	char	gldir[MAX_OSPATH];
	int width = scr_width, height = scr_height;

	S_Init();

	Init_KBD();

	Cvar_RegisterVariable (&gl_ztrick);

	vid.maxwarpwidth = scr_width;
	vid.maxwarpheight = height;
	vid.colormap = host_colormap;
	vid.fullbright = 0xffff;
	vid.aspect = (float) scr_width / (float) scr_height;
	vid.numpages = 2;
	vid.rowbytes = 2 * scr_width;
	vid.width = scr_width;
	vid.height = scr_height;

	//vid.conwidth = scr_width;
	//vid.conheight = scr_height;


	vid.conwidth =400;
	vid.conheight = 300;

	vid.width = vid.conwidth;
	vid.height = vid.conheight;

	// interpret command-line params

	// set vid parameters

	GL_Init();

	sprintf (gldir, "%s/glquake", com_gamedir);
	Sys_mkdir (gldir);

	Check_Gamma(palette);
	VID_SetPalette(palette);

	Con_SafePrintf ("Video mode %dx%d initialized.\n", width, height);

	vid.recalc_refdef = 1;				// force a surface cache flush
}




extern  kbutton_t in_attack;
void PortableAttack(int state)
{
	if (state)
		in_attack.state |= 1 + 2;
	else
		in_attack.state = 4 ;
}

extern  kbutton_t in_jump;
void PortableJump(int state)
{
	if (state)
		in_jump.state |= 1 + 2;
	else
		in_jump.state = 4 ;
}


int PortableKeyEvent(int state, int code)
{
	Key_Event(code, state);
	return 0;
}



void Sys_SendKeyEvents(void)
{
	// Used to poll keyboards on systems that need to poll keyboards.
}

void Force_CenterView_f (void)
{
	cl.viewangles[PITCH] = 0;
}

//void IN_AttackDownp(void) {LOGI("IN_AttackDown");}
//void IN_AttackUpp(void) {LOGI("IN_AttackUp");}

void IN_Init(void)
{
	Cvar_RegisterVariable (&mouse_button_commands[0]);
	Cvar_RegisterVariable (&mouse_button_commands[1]);
	Cvar_RegisterVariable (&mouse_button_commands[2]);
	Cmd_AddCommand ("force_centerview", Force_CenterView_f);

	LOGI("IN_Init");

}

void IN_Shutdown(void)
{
}

/*
===========
IN_Commands
===========
 */
void IN_Commands (void)
{
	// perform button actions
	if (mouse_tap) {
		Key_Event (K_MOUSE1, true);
		Key_Event (K_MOUSE1, false);
		mouse_tap = false;
	}
	if (mouse_buttonstate != mouse_oldbuttonstate) {
		Key_Event (K_MOUSE1, mouse_buttonstate ? true : false);
		mouse_oldbuttonstate = mouse_buttonstate;
	}
}

/*
===========
IN_Move
===========
 */
void IN_MouseMove (usercmd_t *cmd)
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
			Cmd_ExecuteString2("impulse 10\n",src_command);
		break;
	case PORT_ACT_PREV_WEP:
		if (state)
			Cmd_ExecuteString2("impulse 12\n",src_command);
		break;
	}
}

char * quickCommand = 0;
void PortableCommand(char * cmd)
{
	quickCommand = cmd;
}

void IN_Move (usercmd_t *cmd)
{

	if (quickCommand)
	{
		Cmd_ExecuteString2(quickCommand,src_command);
		quickCommand = 0;
	}

	cmd->forwardmove += forwardmove * cl_forwardspeed.value;
	cmd->sidemove    += sidemove    * cl_forwardspeed.value;

	V_StopPitchDrift ();

	switch(look_pitch_mode)
	{
	case LOOK_MODE_MOUSE:
		cl.viewangles[PITCH] += look_pitch_mouse * 200;
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
		cl.viewangles[YAW] += look_yaw_mouse * 200;
		look_yaw_mouse = 0;
		break;
	case LOOK_MODE_JOYSTICK:
		cl.viewangles[YAW] += look_yaw_joy * 6;
		break;
	}

	if (cl.viewangles[PITCH] > 80)
		cl.viewangles[PITCH] = 80;
	if (cl.viewangles[PITCH] < -70)
		cl.viewangles[PITCH] = -70;

}


