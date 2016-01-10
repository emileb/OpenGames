#ifdef OPENGL
#include "../ref_gl/gl_local.h"
#include <dlfcn.h>
#else
#include "../ref_soft/r_local.h"
#endif

#include "rw_linux.h"

// state struct passed in Init

static cvar_t   *in_joystick;
static cvar_t	*joy_name;
static cvar_t   *joy_dev;
static cvar_t	*joy_advanced;
static cvar_t	*joy_advaxisx;
static cvar_t	*joy_advaxisy;
static cvar_t	*joy_advaxisz;
static cvar_t	*joy_advaxisr;
static cvar_t	*joy_advaxisu;
static cvar_t	*joy_advaxisv;
static cvar_t	*joy_forwardthreshold;
static cvar_t	*joy_sidethreshold;
static cvar_t	*joy_pitchthreshold;
static cvar_t	*joy_yawthreshold;
static cvar_t	*joy_forwardsensitivity;
static cvar_t	*joy_sidesensitivity;
static cvar_t	*joy_pitchsensitivity;
static cvar_t	*joy_yawsensitivity;
static cvar_t	*joy_upthreshold;
static cvar_t	*joy_upsensitivity;

static cvar_t  *cl_upspeed;
static cvar_t  *cl_forwardspeed;
static cvar_t  *cl_sidespeed;
static cvar_t  *cl_yawspeed;
static cvar_t  *cl_pitchspeed;
static cvar_t  *cl_anglespeedkey;

static cvar_t  *cl_run;

static qboolean joy_avail = false;
static int *axis_vals = NULL;
static int *axis_map = NULL;

qboolean OpenJoystick(cvar_t *);
void PlatformJoyCommands(int *, int *);

void RW_IN_JoystickCommands() {
  PlatformJoyCommands(axis_vals, axis_map);
}

void Joy_AdvancedUpdate_f () {
  // called once by IN_ReadJoystick and by user whenever an update is needed
  // cvars are now available
  if (joy_advanced->value != 0.0) {
    if (strcmp (joy_name->string, "joystick")) {
      // notify user of advanced controller
      ri.Con_Printf (PRINT_ALL,"\n%s configured\n\n", joy_name->string);
    }
    
    // advanced initialization here
    // data supplied by user via joy_axisn cvars
    axis_map[0] = atoi(joy_advaxisx->string);
    axis_map[1] = atoi(joy_advaxisy->string);
    axis_map[2] = atoi(joy_advaxisz->string);
    axis_map[3] = atoi(joy_advaxisr->string);
    axis_map[4] = atoi(joy_advaxisu->string);
    axis_map[5] = atoi(joy_advaxisv->string);
  }
}


void RW_IN_InitJoystick() {
  in_joystick = ri.Cvar_Get ("in_joystick", "1", CVAR_ARCHIVE);
  if (in_joystick->value) {
    joy_name = ri.Cvar_Get ("joy_name", "joystick", 0);
    joy_dev = ri.Cvar_Get ("joy_dev","/dev/js*",CVAR_ARCHIVE);
    joy_advanced = ri.Cvar_Get ("joy_advanced","0",0);
    joy_advaxisx = ri.Cvar_Get ("joy_advaxisx","4",0);
    joy_advaxisy = ri.Cvar_Get ("joy_advaxisy","2",0);
    joy_advaxisz = ri.Cvar_Get ("joy_advaxisz","0",0);
    joy_advaxisr = ri.Cvar_Get ("joy_advaxisr","0",0);
    joy_advaxisu = ri.Cvar_Get ("joy_advaxisu","0",0);
    joy_advaxisv = ri.Cvar_Get ("joy_advaxisv","0",0);
    joy_forwardthreshold = ri.Cvar_Get ("joy_forwardthreshold","0.15",0);
    joy_sidethreshold = ri.Cvar_Get ("joy_sidethreshold","0.15",0);
    joy_upthreshold	= ri.Cvar_Get ("joy_upthreshold", "0.15",0);
    joy_pitchthreshold = ri.Cvar_Get ("joy_pitchthreshold","0.15",0);
    joy_yawthreshold = ri.Cvar_Get ("joy_yawthreshold","0.15",0);
    joy_forwardsensitivity = ri.Cvar_Get ("joy_forwardsensitivity","-1",0);
    joy_sidesensitivity = ri.Cvar_Get ("joy_sidesensitivity","1",0);
    joy_upsensitivity = ri.Cvar_Get ("joy_upsensitivity","-1",0);
    joy_pitchsensitivity = ri.Cvar_Get ("joy_pitchsensitivity","1",0);
    joy_yawsensitivity = ri.Cvar_Get ("joy_yawsensitivity","-1",0);
    
    cl_upspeed = ri.Cvar_Get ("cl_upspeed", "200", 0);
    cl_forwardspeed = ri.Cvar_Get ("cl_forwardspeed", "200", 0);
    cl_sidespeed = ri.Cvar_Get ("cl_sidespeed", "200", 0);
    cl_yawspeed = ri.Cvar_Get ("cl_yawspeed", "140", 0);
    cl_pitchspeed = ri.Cvar_Get ("cl_pitchspeed", "150", 0);
    
    cl_run = ri.Cvar_Get ("cl_run", "0", CVAR_ARCHIVE);

    if (!axis_map) {
      int i;
      axis_map = (int *)malloc(sizeof(int)*6);
      axis_vals = (int *)malloc(sizeof(int)*6);
      for (i=0;i<6;i++) {
	axis_map[i] = 0;
	axis_vals[i] = 0;
      }
      axis_map[0]=3;
      axis_map[1]=1;
    }
    
    ri.Cmd_AddCommand ("joy_advancedupdate", Joy_AdvancedUpdate_f);
    if ((joy_avail = OpenJoystick(joy_dev))) {
      ri.Con_Printf(PRINT_ALL, "Joystick Activated");
      Joy_AdvancedUpdate_f();
    }
  }
}

void RW_IN_JoystickMove(usercmd_t *cmd, qboolean mlooking, 
			cvar_t *lookstrafe, cvar_t *m_pitch) {
  in_state_t *in_state = getState();
  
  if (joy_avail) {
    // Start
    float speed, aspeed;
    float jforward = ((float)axis_vals[1])/32768.0;
    float jside = ((float)axis_vals[3])/32768.0;
    float jup = ((float)axis_vals[5])/32768.0;
    float jturn = ((float)axis_vals[4])/32768.0;
    float jlook = ((float)axis_vals[2])/32768.0;

    //ri.Con_Printf(PRINT_ALL, "%g %g %g %g %g\n", 
    //jforward, jside, jup, jturn, jlook);

    if (((*in_state->in_speed_state) & 1) || cl_run->value!=0.0)
      speed = 2;
    else
      speed = 1;
    aspeed = 0.025;
    
    // This is the forward/backward code
    if ((joy_advanced->value == 0.0) && mlooking) {
      // user wants forward control to become look control
      // if mouse invert is on, invert the joystick pitch value
      // only absolute control support here (joy_advanced is false)
      if (fabs(jforward) > joy_pitchthreshold->value) {
	if (m_pitch->value < 0.0) {
	  in_state->viewangles[PITCH] -= 
	    jforward*joy_pitchsensitivity->value*aspeed*cl_pitchspeed->value;
	}
	else {
	  in_state->viewangles[PITCH] += 
	    jforward*joy_pitchsensitivity->value*aspeed*cl_pitchspeed->value;
	}
      }
    }
    else if (fabs(jforward) > joy_forwardthreshold->value) {
      // user wants forward control to be forward control
      cmd->forwardmove += 
	jforward*joy_forwardsensitivity->value*speed*cl_forwardspeed->value;
    }
    
    if (fabs(jside)>joy_sidethreshold->value) {
      cmd->sidemove += 
	(jside*joy_sidesensitivity->value)*speed*cl_sidespeed->value;
    }
    
    if (fabs(jup) > joy_upthreshold->value) {
      cmd->upmove += 
	(jup * joy_upsensitivity->value) * speed*cl_upspeed->value;
    }

    if (((*in_state->in_strafe_state) & 1)||(lookstrafe->value && mlooking)) {
      // user wants turn control to become side control
      if (fabs(jturn) > joy_sidethreshold->value) {
	cmd->sidemove -= 
	  (jturn*joy_sidesensitivity->value)*speed*cl_sidespeed->value;
      }
    }
    else {
      // user wants turn control to be turn control
      if (fabs(jturn) > joy_yawthreshold->value) {
	in_state->viewangles[YAW] += 
	  (jturn*joy_yawsensitivity->value)*aspeed*cl_yawspeed->value;
      }
    }
    
    //    if (mlooking) {
      if (fabs(jlook) > joy_pitchthreshold->value) {
	// pitch movement detected and pitch movement desired by user
	in_state->viewangles[PITCH] += 
	  (jlook*joy_pitchsensitivity->value) * aspeed*cl_pitchspeed->value;
      }
      //}

    // End
  }
}
