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

/*
** GLW_IMP.C
**
** This file contains ALL Solaris specific stuff having to do with the
** OpenGL refresh.  When a port is being made the following functions
** must be implemented by the port:
**
** GLimp_EndFrame
** GLimp_Init
** GLimp_Shutdown
** GLimp_SwitchFullscreen
**
*/

#include <termios.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <signal.h>
#include <dlfcn.h>

#include <GL/glx.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/Sunkeysym.h>

#include "../ref_gl/gl_local.h"

#include "../client/keys.h"

#include "../solaris/rw_solaris.h"
#include "../solaris/glw_solaris.h"
#include "../solaris/gl_glx.h"

glwstate_t glw_state;

static Display *dpy = NULL;
static int scrnum;
static Window win;
static GLXContext ctx = NULL;

#define KEY_MASK (KeyPressMask | KeyReleaseMask)
#define MOUSE_MASK (ButtonPressMask | ButtonReleaseMask | \
		    PointerMotionMask | ButtonMotionMask )
#define X_MASK (KEY_MASK | MOUSE_MASK | VisibilityChangeMask | StructureNotifyMask )


struct
{
  int key;
  int down;
} keyq[64];
static int keyq_head=0;
static int keyq_tail=0;

static int window_width;
static int window_height;

extern char display_name[];

/*****************************************************************************/
/* MOUSE                                                                     */
/*****************************************************************************/

// this is inside the renderer shared lib, so these are called from vid_so

static qboolean mouse_avail;
static int mouse_buttonstate;
static int mouse_oldbuttonstate;
static int mouse_x, mouse_y;
static int old_mouse_x, old_mouse_y;
static int mx, my;
static int old_windowed_mouse;
static int p_mouse_x, p_mouse_y;

static cvar_t	*_windowed_mouse;
static cvar_t	*m_filter;
static cvar_t	*in_mouse;

static qboolean	mlooking;
static qboolean mouse_active = false;

// stencilbuffer shadows   
qboolean have_stencil = false;

// state struct passed in Init
static in_state_t	*in_state;

static cvar_t *sensitivity;
static cvar_t *lookstrafe;
static cvar_t *m_side;
static cvar_t *m_yaw;
static cvar_t *m_pitch;
static cvar_t *m_forward;
static cvar_t *freelook;
static cvar_t   *r_fakeFullscreen;

static Cursor CreateNullCursor(Display *display, Window root)
{
  Pixmap cursormask; 
  XGCValues xgc;
  GC gc;
  XColor dummycolour;
  Cursor cursor;

  cursormask = XCreatePixmap( display, root, 1, 1, 1 );
  xgc.function = GXclear;
  gc =  XCreateGC( display, cursormask, GCFunction, &xgc );
  XFillRectangle( display, cursormask, gc, 0, 0, 1, 1 );
  dummycolour.pixel = 0;
  dummycolour.red = 0;
  dummycolour.flags = DoRed;
  cursor = XCreatePixmapCursor( display, cursormask, cursormask,
				&dummycolour, &dummycolour, 0,0 );
  XFreePixmap( display, cursormask );
  XFreeGC( display, gc );
  return cursor;
}

static void install_grabs(void)
{

// inviso cursor
	XDefineCursor(dpy, win, CreateNullCursor(dpy, win));

	XGrabPointer(dpy, win,
				 True,
				 0,
				 GrabModeAsync, GrabModeAsync,
				 win,
				 None,
				 CurrentTime);


#ifdef __linux__
	if (in_dgamouse->value) {
		int MajorVersion, MinorVersion;

		if (!XF86DGAQueryVersion(dpy, &MajorVersion, &MinorVersion)) { 
			// unable to query, probalby not supported
			VID_Printf( PRINT_ALL, "Failed to detect XF86DGA Mouse\n" );
			Cvar_Set( "in_dgamouse", "0" );
		} else {
			dgamouse = true;
			XF86DGADirectVideo(dpy, DefaultScreen(dpy), XF86DGADirectMouse);
			XWarpPointer(dpy, None, win, 0, 0, 0, 0, 0, 0);
		}
	} else
#endif /*__linux__ */
	{
		XWarpPointer(dpy, None, win,
					 0, 0, 0, 0,
					 vid.width / 2, vid.height / 2);
	}

	XGrabKeyboard(dpy, win,
				  False,
				  GrabModeAsync, GrabModeAsync,
				  CurrentTime);

	mouse_active = true;

//	XSync(dpy, True);
}

static void uninstall_grabs(void)
{
	if (!dpy || !win)
		return;

#ifdef __linux__
	if (dgamouse) {
		dgamouse = false;
		XF86DGADirectVideo(dpy, DefaultScreen(dpy), 0);
	}
#endif

	XUngrabPointer(dpy, CurrentTime);
	XUngrabKeyboard(dpy, CurrentTime);

// inviso cursor
	XUndefineCursor(dpy, win);

	mouse_active = false;
}

static void Force_CenterView_f (void)
{
  in_state->viewangles[PITCH] = 0;
}

static void RW_IN_MLookDown (void) 
{ 
  mlooking = true; 
}

static void RW_IN_MLookUp (void) 
{
  mlooking = false;
  in_state->IN_CenterView_fp ();
}

void RW_IN_Init(in_state_t *in_state_p)
{
  int mtype;
  int i;

  in_state = in_state_p;

  // mouse variables
  _windowed_mouse = ri.Cvar_Get( "_windowed_mouse", "0", CVAR_ARCHIVE );
  m_filter = ri.Cvar_Get ("m_filter", "0", 0);
  in_mouse = ri.Cvar_Get ("in_mouse", "1", CVAR_ARCHIVE);
  freelook = ri.Cvar_Get( "freelook", "0", 0 );
  lookstrafe = ri.Cvar_Get ("lookstrafe", "0", 0);
  sensitivity = ri.Cvar_Get ("sensitivity", "3", 0);
  m_pitch = ri.Cvar_Get ("m_pitch", "0.022", 0);
  m_yaw = ri.Cvar_Get ("m_yaw", "0.022", 0);
  m_forward = ri.Cvar_Get ("m_forward", "1", 0);
  m_side = ri.Cvar_Get ("m_side", "0.8", 0);

  ri.Cmd_AddCommand ("+mlook", RW_IN_MLookDown);
  ri.Cmd_AddCommand ("-mlook", RW_IN_MLookUp);

  ri.Cmd_AddCommand ("force_centerview", Force_CenterView_f);

  mx = my = 0.0;
  mouse_avail = true;
}

void RW_IN_Shutdown(void)
{
  mouse_avail = false;

  ri.Cmd_RemoveCommand( "+mlook" );
  ri.Cmd_RemoveCommand( "-mlook" );
  ri.Cmd_RemoveCommand( "force_centerview" );}

/*
===========
IN_Commands
===========
*/
void RW_IN_Commands (void)
{
  int i;
   
  if (!mouse_avail) 
    return;
   
  for( i = 0 ; i < 3 ; i++ ) {
    if( (mouse_buttonstate & (1<<i)) && !(mouse_oldbuttonstate & (1<<i)) ) {
      in_state->Key_Event_fp (K_MOUSE1 + i, true);
    }

    if( !(mouse_buttonstate & (1<<i)) && (mouse_oldbuttonstate & (1<<i)) ) {
      in_state->Key_Event_fp (K_MOUSE1 + i, false);
    }
  }
  mouse_oldbuttonstate = mouse_buttonstate;
}

/*
===========
IN_Move
===========
*/
void RW_IN_Move (usercmd_t *cmd)
{
  if (!mouse_avail)
    return;
   
  if( m_filter->value )
  {
    mx = (mx + old_mouse_x) * 0.5;
    my = (my + old_mouse_y) * 0.5;
  }

  old_mouse_x = mx;
  old_mouse_y = my;

  mx *= sensitivity->value;
  my *= sensitivity->value;

  // add mouse X/Y movement to cmd
  if( (*in_state->in_strafe_state & 1) || 
      (lookstrafe->value && mlooking ) ) {
    cmd->sidemove += m_side->value * mx;
  }
  else {
    in_state->viewangles[YAW] -= m_yaw->value * mx;
  }

  if( (mlooking || freelook->value) && 
      !(*in_state->in_strafe_state & 1) ) {
    in_state->viewangles[PITCH] += m_pitch->value * my;
  }
  else {
    cmd->forwardmove -= m_forward->value * my;
  }

  mx = 0;
  my = 0;
}

static void IN_DeactivateMouse( void ) 
{
	if (!mouse_avail || !dpy || !win)
		return;

	if (mouse_active) {
		uninstall_grabs();
		mouse_active = false;
	}
}

static void IN_ActivateMouse( void ) 
{
	if (!mouse_avail || !dpy || !win)
		return;

	if (!mouse_active) {
		mx = my = 0; // don't spazz
		install_grabs();
		mouse_active = true;
	}
}

void RW_IN_Frame (void)
{
}

void RW_IN_Activate(qboolean active)
{
}

/*****************************************************************************/
/* KEYBOARD                                                                  */
/*****************************************************************************/

int XLateKey(XKeyEvent *ev)
{

  int key;
  char buf[64];
  KeySym keysym;

  key = 0;

  XLookupString( ev, buf, sizeof buf, &keysym, 0 );

  switch( keysym ) {
  case XK_KP_9:
  case XK_F29:
  case XK_KP_Page_Up:	key = K_KP_PGUP;	break;
  case XK_Page_Up:	key = K_PGUP;		break;

  case XK_KP_3:
  case XK_F35:
  case XK_KP_Page_Down:	key = K_KP_PGDN;	break;
  case XK_Page_Down:	key = K_PGDN;		break;

  case XK_KP_7:
  case XK_F27:
  case XK_KP_Home:	key = K_KP_HOME;	break;
  case XK_Home:		key = K_HOME;		break;

  case XK_KP_1:
  case XK_F33:
  case XK_KP_End:	key = K_KP_END;		break;
  case XK_End:		key = K_END;		break;

  case XK_KP_4:
  case XK_KP_Left:	key = K_KP_LEFTARROW;	break;
  case XK_Left:		key = K_LEFTARROW;	break;

  case XK_KP_6:
  case XK_KP_Right:	key = K_KP_RIGHTARROW;	break;
  case XK_Right:	key = K_RIGHTARROW;	break;

  case XK_KP_2:
  case XK_KP_Down:	key = K_KP_DOWNARROW;	break;
  case XK_Down:		key = K_DOWNARROW;	break;

  case XK_KP_8:
  case XK_KP_Up:	key = K_KP_UPARROW;	break;
  case XK_Up:		key = K_UPARROW;	break;

  case XK_Escape:	key = K_ESCAPE;		break;

  case XK_KP_Enter:	key = K_KP_ENTER;	break;
  case XK_Return:	key = K_ENTER;		break;

  case XK_Tab:		key = K_TAB;		break;

  case XK_F1:		key = K_F1;		break;
  case XK_F2:		key = K_F2;		break;
  case XK_F3:		key = K_F3;		break;
  case XK_F4:		key = K_F4;		break;
  case XK_F5:		key = K_F5;		break;
  case XK_F6:		key = K_F6;		break;
  case XK_F7:		key = K_F7;		break;
  case XK_F8:		key = K_F8;		break;
  case XK_F9:		key = K_F9;		break;
  case XK_F10:		key = K_F10;		break;
  case SunXK_F36:	key = K_F11;		break;
  case SunXK_F37:	key = K_F12;		break;

  case XK_BackSpace:	key = K_BACKSPACE;	break;

  case XK_KP_Separator:
  case XK_KP_Delete:	key = K_KP_DEL;		break;
  case XK_Delete:	key = K_DEL;		break;

  case XK_Pause:	key = K_PAUSE;		break;

  case XK_Shift_L:
  case XK_Shift_R:	key = K_SHIFT;		break;

  case XK_Execute:
  case XK_Control_L: 
  case XK_Control_R:	key = K_CTRL;		break;

  case XK_Alt_L:	
  case XK_Meta_L: 
  case XK_Alt_R:	
  case XK_Meta_R:	key = K_ALT;		break;

  case XK_KP_5:
  case XK_F31:		key = K_KP_5;		break;

  case XK_Insert:	key = K_INS;		break;

  case XK_KP_0:
  case XK_KP_Insert:	key = K_KP_INS;		break;

  case XK_F26:
  case XK_KP_Multiply:	key = '*';		break;

  case XK_KP_Add:	key = K_KP_PLUS;	break;
  
  case XK_F24:
  case XK_KP_Subtract:	key = K_KP_MINUS;	break;

  case XK_F25:
  case XK_KP_Divide:	key = K_KP_SLASH;	break;

  case XK_F11:
    if( ev->type == KeyPress ) {
      if( _windowed_mouse->value ) {
	Cvar_SetValue( "_windowed_mouse", 0 );
      }
      else {
	Cvar_SetValue( "_windowed_mouse", 1 );
      }
    }
    break;

#if 0
  case 0x021: key = '1';break;/* [!] */
  case 0x040: key = '2';break;/* [@] */
  case 0x023: key = '3';break;/* [#] */
  case 0x024: key = '4';break;/* [$] */
  case 0x025: key = '5';break;/* [%] */
  case 0x05e: key = '6';break;/* [^] */
  case 0x026: key = '7';break;/* [&] */
  case 0x02a: key = '8';break;/* [*] */
  case 0x028: key = '9';;break;/* [(] */
  case 0x029: key = '0';break;/* [)] */
  case 0x05f: key = '-';break;/* [_] */
  case 0x02b: key = '=';break;/* [+] */
  case 0x07c: key = '\'';break;/* [|] */
  case 0x07d: key = '[';break;/* [}] */
  case 0x07b: key = ']';break;/* [{] */
  case 0x022: key = '\'';break;/* ["] */
  case 0x03a: key = ';';break;/* [:] */
  case 0x03f: key = '/';break;/* [?] */
  case 0x03e: key = '.';break;/* [>] */
  case 0x03c: key = ',';break;/* [<] */
#endif

  default:
    key = *(unsigned char*)buf;
    if (key >= 'A' && key <= 'Z')
      key = key - 'A' + 'a';
    break;
  } 

  return key;
}





void GetEvent(void)
{
  XEvent x_event;
  int b;
   
  XNextEvent(dpy, &x_event);
  switch(x_event.type) {
  case KeyPress:
    keyq[keyq_head].key = XLateKey(&x_event.xkey);
    keyq[keyq_head].down = true;
    keyq_head = (keyq_head + 1) & 63;
    break;

  case KeyRelease:
    /*
     *  This is a hack in order to avoid disabling key repeat, which
     *  would cause a lot of problems when changing to other windows
     *  or when the program crashes.
     *
     *  Whenever a key release event occurs, we check to see if the
     *  next event in the queue is a press event of the same key
     *  with the same time stamp. If it is, we simply discard this
     *  and the next event.
     */
    if( XPending( dpy ) > 0 ) {
      XEvent tmp_event;
      XPeekEvent( dpy, &tmp_event );
      if( tmp_event.type == KeyPress &&
	  tmp_event.xkey.keycode == x_event.xkey.keycode &&
	  tmp_event.xkey.time == x_event.xkey.time ) {
	XNextEvent( dpy, &tmp_event );
	break;
      }
    }
    keyq[keyq_head].key = XLateKey(&x_event.xkey);
    keyq[keyq_head].down = false;
    keyq_head = (keyq_head + 1) & 63;
    break;

  case MotionNotify:
    if( _windowed_mouse->value ) {
      int xoffset = ((int)x_event.xmotion.x - (int)(window_width / 2));
      int yoffset = ((int)x_event.xmotion.y - (int)(window_height / 2));

      if( xoffset != 0 || yoffset != 0 ) {

	mx += xoffset;
	my += yoffset;

	/* move the mouse to the window center again */
	XSelectInput( dpy, win, X_MASK & ~PointerMotionMask );
	XWarpPointer( dpy, None, win, 0, 0, 0, 0, 
		      (window_width / 2), (window_height / 2) );
	XSelectInput( dpy, win, X_MASK );
      }
    }
    else {
      mx = ((int)x_event.xmotion.x - (int)p_mouse_x);
      my = ((int)x_event.xmotion.y - (int)p_mouse_y);
      p_mouse_x = x_event.xmotion.x;
      p_mouse_y = x_event.xmotion.y;
    }
    break;

  case ButtonPress:
    b = -1;
    if( x_event.xbutton.button == 1 ) {
      b = 0;
    }
    else if( x_event.xbutton.button == 2 ) {
      b = 2;
    }
    else if( x_event.xbutton.button == 3 ) {
      b = 1;
    }
    if( b >= 0 ) {
      mouse_buttonstate |= 1 << b;
    }
    break;

  case ButtonRelease:
    b = -1;
    if( x_event.xbutton.button == 1 ) {
      b = 0;
    }
    else if( x_event.xbutton.button == 2 ) {
      b = 2;
    }
    else if( x_event.xbutton.button == 3 ) {
      b = 1;
    }
    if( b >= 0 ) {
      mouse_buttonstate &= ~(1 << b);
    }
    break;
	
#if 0
  case ConfigureNotify:
    config_notify_width = x_event.xconfigure.width;
    config_notify_height = x_event.xconfigure.height;
    if( xil_keepaspect->value && !force_resize ) {
      double aspect = (double)vid.width / vid.height;
      if( config_notify_height > config_notify_width / aspect ) {
	force_resize = 1;
	XResizeWindow( dpy, win,
		       config_notify_width,
		       (int)(config_notify_width / aspect) );
      }
      else if( config_notify_width > config_notify_height * aspect ) {
	force_resize = 1;
	XResizeWindow( dpy, win,
		       (int)(config_notify_height * aspect),
		       config_notify_height );
      }
    }
    else {
      force_resize = 0;
    }
    if( !force_resize ) {
      config_notify = 1;
    }
    break;
#endif

  case MapNotify:
    if( _windowed_mouse->value ) {
      XGrabPointer( dpy, win, True, 0, GrabModeAsync,
		    GrabModeAsync, win, None, CurrentTime);
    }
    break;

  case UnmapNotify:
    if( _windowed_mouse->value ) {
      XUngrabPointer( dpy, CurrentTime );
    }
    break;

  default:
    ;
  }
   
  if( old_windowed_mouse != _windowed_mouse->value ) {
    old_windowed_mouse = _windowed_mouse->value;

    if( !_windowed_mouse->value ) {
      /* ungrab the pointer */
      XUngrabPointer( dpy, CurrentTime );
    } else {
      /* grab the pointer */
      XGrabPointer( dpy, win, True, 0, GrabModeAsync,
		    GrabModeAsync, win, None, CurrentTime );
    }
  }
}




Key_Event_fp_t Key_Event_fp;

void KBD_Init(Key_Event_fp_t fp)
{
  Key_Event_fp = fp;
}

void KBD_Update(void)
{
  // get events from x server
  if( dpy ) {
    while( XPending( dpy ) ) {
      GetEvent();
    }
    while( keyq_head != keyq_tail ) {
      Key_Event_fp(keyq[keyq_tail].key, keyq[keyq_tail].down);
      keyq_tail = (keyq_tail + 1) & 63;
    }
  }
}

void KBD_Close(void)
{
}

/*****************************************************************************/

static qboolean GLimp_SwitchFullscreen( int width, int height );
qboolean GLimp_InitGL (void);

static void signal_handler(int sig)
{
  printf("Received signal %d, exiting...\n", sig);
  GLimp_Shutdown();
  _exit(0);
}

static void InitSig(void)
{
  signal(SIGHUP, signal_handler);
  signal(SIGQUIT, signal_handler);
  signal(SIGILL, signal_handler);
  signal(SIGTRAP, signal_handler);
  signal(SIGIOT, signal_handler);
  signal(SIGBUS, signal_handler);
  signal(SIGFPE, signal_handler);
  signal(SIGSEGV, signal_handler);
  signal(SIGTERM, signal_handler);
}

/*
** GLimp_SetMode
*/
int GLimp_SetMode( int *pwidth, int *pheight, int mode, qboolean fullscreen )
{
  int width, height;
  int attrib[] = {
    GLX_RGBA,
    GLX_RED_SIZE, 1,
    GLX_GREEN_SIZE, 1,
    GLX_BLUE_SIZE, 1,
    GLX_DOUBLEBUFFER,
    GLX_DEPTH_SIZE, 1,
    GLX_STENCIL_SIZE, 1,
    None
  };
  int attrib_nostencil[] = {
    GLX_RGBA,
    GLX_RED_SIZE, 1,
    GLX_GREEN_SIZE, 1,
    GLX_BLUE_SIZE, 1,
    GLX_DOUBLEBUFFER,
    GLX_DEPTH_SIZE, 1,
    None
  };
  Window root;
  XVisualInfo *visinfo;
  XSetWindowAttributes attr;
  unsigned long mask;
  int MajorVersion, MinorVersion;
  int actualWidth, actualHeight;
  int i;

  r_fakeFullscreen = ri.Cvar_Get( "r_fakeFullscreen", "0", CVAR_ARCHIVE);

  ri.Con_Printf( PRINT_ALL, "Initializing OpenGL display\n");

  if (fullscreen)
    ri.Con_Printf (PRINT_ALL, "...setting fullscreen mode %d:", mode );
  else
    ri.Con_Printf (PRINT_ALL, "...setting mode %d:", mode );

  if ( !ri.Vid_GetModeInfo( &width, &height, mode ) )
    {
      ri.Con_Printf( PRINT_ALL, " invalid mode\n" );
      return rserr_invalid_mode;
    }

  ri.Con_Printf( PRINT_ALL, " %d %d\n", width, height );

  window_width = width;
  window_height = height;

  // destroy the existing window (Commented out on Mon Sep 20 15:09:02 MEST 2004)
  GLimp_Shutdown ();

  if (!(dpy = XOpenDisplay(display_name))) {
    fprintf(stderr, "Error couldn't open the X display\n");
    return rserr_invalid_mode;
  }

  scrnum = DefaultScreen(dpy);
  root = RootWindow(dpy, scrnum);

  // Get video mode list
  MajorVersion = MinorVersion = 0;

  visinfo = qglXChooseVisual(dpy, scrnum, attrib);
  if (!visinfo) {
    fprintf(stderr, "Error couldn't get an RGBA, DOUBLEBUFFER, DEPTH, STENCIL visual\n");
    visinfo = qglXChooseVisual(dpy, scrnum, attrib_nostencil);
    if (!visinfo) {
      fprintf(stderr, "E: couldn't get an RGBA, DOUBLEBUFFER, DEPTH visual\n");
      return rserr_invalid_mode;
    }
  }

	/* do some pantsness */
	if ( qglXGetConfig )
	{
		int red_bits, blue_bits, green_bits, depth_bits, alpha_bits;

		qglXGetConfig(dpy, visinfo, GLX_RED_SIZE, &red_bits);
		qglXGetConfig(dpy, visinfo, GLX_BLUE_SIZE, &blue_bits);
		qglXGetConfig(dpy, visinfo, GLX_GREEN_SIZE, &green_bits);
		qglXGetConfig(dpy, visinfo, GLX_DEPTH_SIZE, &depth_bits);
		qglXGetConfig(dpy, visinfo, GLX_ALPHA_SIZE, &alpha_bits);

		ri.Con_Printf(PRINT_ALL, "I: got %d bits of red\n", red_bits);
		ri.Con_Printf(PRINT_ALL, "I: got %d bits of blue\n", blue_bits);
		ri.Con_Printf(PRINT_ALL, "I: got %d bits of green\n", green_bits);
		ri.Con_Printf(PRINT_ALL, "I: got %d bits of depth\n", depth_bits);
		ri.Con_Printf(PRINT_ALL, "I: got %d bits of alpha\n", alpha_bits);
	}

	/* stencilbuffer shadows */
	if ( qglXGetConfig )
	{
		int stencil_bits;

		if (!qglXGetConfig(dpy, visinfo, GLX_STENCIL_SIZE, &stencil_bits)) {
			ri.Con_Printf(PRINT_ALL, "I: got %d bits of stencil\n", stencil_bits);
			if (stencil_bits >= 1) {
				have_stencil = true;
			}
		}
	}



  /* window attributes */
  attr.background_pixel = 0;
  attr.border_pixel = 0;
  attr.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);
  attr.event_mask = X_MASK;
  puts("DEBUG: GLimp_SetMode() IS setting X_MASK");

  mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

  win = XCreateWindow(dpy, root, 0, 0, width, height,
		      0, visinfo->depth, InputOutput,
		      visinfo->visual, mask, &attr);
  XMapWindow(dpy, win);

  XFlush(dpy);

  ctx = qglXCreateContext(dpy, visinfo, NULL, True);

  qglXMakeCurrent(dpy, win, ctx);

  *pwidth = width;
  *pheight = height;

  // let the sound and input subsystems know about the new window
  ri.Vid_NewWindow (width, height);

  qglXMakeCurrent(dpy, win, ctx);


  // inviso cursor
  XDefineCursor( dpy, win, CreateNullCursor( dpy, win ) );


  return rserr_ok;
}

/*
** GLimp_Shutdown
**
** This routine does all OS specific shutdown procedures for the OpenGL
** subsystem.  Under OpenGL this means NULLing out the current DC and
** HGLRC, deleting the rendering context, and releasing the DC acquired
** for the window.  The state structure is also nulled out.
**
*/
void GLimp_Shutdown( void )
{
	uninstall_grabs();
	mouse_active = false;
#ifdef __linux__
	dgamouse = false;
#endif

	if (dpy) {
		if (ctx)
			qglXDestroyContext(dpy, ctx);
		if (win)
			XDestroyWindow(dpy, win);
		XUngrabKeyboard(dpy, CurrentTime);
		XCloseDisplay(dpy);
	}
	ctx = NULL;
	dpy = NULL;
	win = 0;
/*	
	qglXChooseVisual             = NULL;
	qglXCreateContext            = NULL;
	qglXDestroyContext           = NULL;
	qglXMakeCurrent              = NULL;
	qglXCopyContext              = NULL;
	qglXSwapBuffers              = NULL;
*/	
}

/*
** GLimp_Init
**
** This routine is responsible for initializing the OS specific portions
** of OpenGL.  
*/
int GLimp_Init( void *hinstance, void *wndproc )
{
  InitSig();

	if ( glw_state.OpenGLLib) {
		#define GPA( a ) dlsym( glw_state.OpenGLLib, a )

		qglXChooseVisual             =  GPA("glXChooseVisual");
		qglXCreateContext            =  GPA("glXCreateContext");
		qglXDestroyContext           =  GPA("glXDestroyContext");
		qglXMakeCurrent              =  GPA("glXMakeCurrent");
		qglXCopyContext              =  GPA("glXCopyContext");
		qglXSwapBuffers              =  GPA("glXSwapBuffers");
		qglXGetConfig                =  GPA("glXGetConfig");
		
		return true;
	}
	
	return false;
}

/*
** GLimp_BeginFrame
*/
void GLimp_BeginFrame( float camera_seperation )
{
}

/*
** GLimp_EndFrame
** 
** Responsible for doing a swapbuffers and possibly for other stuff
** as yet to be determined.  Probably better not to make this a GLimp
** function and instead do a call to GLimp_SwapBuffers.
*/
void GLimp_EndFrame (void)
{
  qglFlush();
  qglXSwapBuffers(dpy, win);
}

/*
** GLimp_AppActivate
*/
void GLimp_AppActivate( qboolean active )
{
}


/*------------------------------------------------*/
/* X11 Input Stuff */
/*------------------------------------------------*/

