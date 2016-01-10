#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__


#ifdef OPENGL
#include "../ref_gl/gl_local.h"
#else
#include "../ref_soft/r_local.h"
#endif

/*
extern cvar_t   *in_joystick;
extern cvar_t	*joy_name;
extern cvar_t   *joy_dev;
extern cvar_t   *joy_advanced;
extern cvar_t	*joy_advaxisx;
extern cvar_t	*joy_advaxisy;
extern cvar_t	*joy_advaxisz;
extern cvar_t	*joy_advaxisr;
extern cvar_t	*joy_advaxisu;
extern cvar_t	*joy_advaxisv;
extern cvar_t	*joy_forwardthreshold;
extern cvar_t	*joy_sidethreshold;
extern cvar_t	*joy_pitchthreshold;
extern cvar_t	*joy_yawthreshold;
extern cvar_t	*joy_forwardsensitivity;
extern cvar_t	*joy_sidesensitivity;
extern cvar_t	*joy_pitchsensitivity;
extern cvar_t	*joy_yawsensitivity;
extern cvar_t	*joy_upthreshold;
extern cvar_t	*joy_upsensitivity;

extern cvar_t  *cl_upspeed;
extern cvar_t  *cl_forwardspeed;
extern cvar_t  *cl_sidespeed;
extern cvar_t  *cl_yawspeed;
extern cvar_t  *cl_pitchspeed;
extern cvar_t  *cl_anglespeedkey;

extern cvar_t  *cl_run;

extern qboolean joystick_avail;
extern int *axis_vals;
extern int *axis_map;
*/
// In joystick.c
void RW_IN_InitJoystick();
void Joy_AdvancedUpdate_f (void);
void RW_IN_JoystickCommands();
void RW_IN_JoystickMove(usercmd_t *, qboolean, cvar_t *, cvar_t *);

// Provided in platform specific manner
qboolean OpenJoystick(cvar_t *);
qboolean CloseJoystick();
void PlatformJoyCommands(int *, int *);

#endif
