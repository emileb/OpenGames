/*
** rw_xil.c
**
** This file contains the Corona version of the X11 renderer.
*/

#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <xil/xil.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/Sunkeysym.h>

#include "../ref_soft/r_local.h"
#include "../client/keys.h"
#include "../solaris/rw_solaris.h"
#include "newt.h"

#include "q2icon.xbm"


/*****************************************************************************/

extern char display_name[];

static Display *x_disp;
static Window x_win;
static GC x_gc;

#define STD_EVENT_MASK (StructureNotifyMask | KeyPressMask \
	     | KeyReleaseMask | ExposureMask | PointerMotionMask | \
	     ButtonPressMask | ButtonReleaseMask | \
	     VisibilityChangeMask)


static qboolean X11_active = false;

struct
{
  int key;
  int down;
} keyq[64];
static int keyq_head=0;
static int keyq_tail=0;

static int config_notify = 0;
static int config_notify_width;
static int config_notify_height;
static int is_double_buffered;

typedef unsigned short PIXEL;

// Console variables that we need to access from this module

/*****************************************************************************/
/* MOUSE                                                                     */
/*****************************************************************************/

// this is inside the renderer shared lib, so these are called from vid_so

static qboolean        mouse_avail;
static int     mouse_buttonstate;
static int     mouse_oldbuttonstate;
static int   mouse_x, mouse_y;
static int	old_mouse_x, old_mouse_y;
static int		mx, my;
static int old_windowed_mouse;
static int p_mouse_x, p_mouse_y;

static cvar_t	*_windowed_mouse;
static cvar_t	*m_filter;
static cvar_t	*in_mouse;

static qboolean	mlooking;

// state struct passed in Init
static in_state_t	*in_state;

static cvar_t *sensitivity;
static cvar_t *lookstrafe;
static cvar_t *m_side;
static cvar_t *m_yaw;
static cvar_t *m_pitch;
static cvar_t *m_forward;
static cvar_t *freelook;
static cvar_t *xil_keepaspect;



/**************************************************************
 * NewT stuff
 **************************************************************/
static unsigned char *video_buffer = NULL;
static int window_width;
static int window_height;
static int force_resize = 0;
static unsigned char ymap[256];
static unsigned char umap[256];
static unsigned char vmap[256];

extern int newt_initialize_next;


static void Set_Window_Position_f( void );
static void Set_Window_Size_f( void );


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
  m_filter = ri.Cvar_Get( "m_filter", "0", 0 );
  in_mouse = ri.Cvar_Get( "in_mouse", "1", CVAR_ARCHIVE );
  freelook = ri.Cvar_Get( "freelook", "0", 0 );
  lookstrafe = ri.Cvar_Get( "lookstrafe", "0", 0 );
  sensitivity = ri.Cvar_Get( "sensitivity", "3", 0 );
  m_pitch = ri.Cvar_Get( "m_pitch", "0.022", 0 );
  m_yaw = ri.Cvar_Get( "m_yaw", "0.022", 0 );
  m_forward = ri.Cvar_Get( "m_forward", "1", 0 );
  m_side = ri.Cvar_Get( "m_side", "0.8", 0 );
  xil_keepaspect = ri.Cvar_Get( "xil_keepaspect", "1", CVAR_ARCHIVE );

  ri.Cmd_AddCommand( "+mlook", RW_IN_MLookDown );
  ri.Cmd_AddCommand( "-mlook", RW_IN_MLookUp );

  ri.Cmd_AddCommand( "force_centerview", Force_CenterView_f );

  ri.Cmd_AddCommand( "xil_set_window_position", Set_Window_Position_f );
  ri.Cmd_AddCommand( "xil_set_window_size", Set_Window_Size_f );

  mouse_x = mouse_y = 0.0;
  mouse_avail = true;
}

void RW_IN_Shutdown(void)
{
  mouse_avail = false;

  ri.Cmd_RemoveCommand( "+mlook" );
  ri.Cmd_RemoveCommand( "-mlook" );
  ri.Cmd_RemoveCommand( "force_centerview" );
  ri.Cmd_RemoveCommand( "xil_set_window_position" );
  ri.Cmd_RemoveCommand( "xil_set_window_size" );
}

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
  if( !mouse_avail ) {
    return;
  }
   
  if( m_filter->value ) {
    mouse_x = (mx + old_mouse_x) * 0.5;
    mouse_y = (my + old_mouse_y) * 0.5;
  }
  else {
    mouse_x = mx;
    mouse_y = my;
  }

  old_mouse_x = mx;
  old_mouse_y = my;

  if( !mouse_x && !mouse_y ) {
    return;
  }

  mouse_x *= sensitivity->value;
  mouse_y *= sensitivity->value;

  // add mouse X/Y movement to cmd
  if( (*in_state->in_strafe_state & 1) ||
      (lookstrafe->value && mlooking) ) {
    cmd->sidemove += m_side->value * mouse_x;
  }
  else {
    in_state->viewangles[YAW] -= m_yaw->value * mouse_x;
  }

  if( (mlooking || freelook->value) && 
      !(*in_state->in_strafe_state & 1) ) {
    in_state->viewangles[PITCH] += m_pitch->value * mouse_y;
  }
  else {
    cmd->forwardmove -= m_forward->value * mouse_y;
  }
  mx = 0;
  my = 0;
}

void RW_IN_Frame (void)
{
}

void RW_IN_Activate(void)
{
}


// ========================================================================
// makes a null cursor
// ========================================================================

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


void ResetNEWTFrameBuffers( void )
{
  newt_cleanup();
  video_buffer = malloc( vid.width * vid.height );
  if( video_buffer == NULL ) {
    Sys_Error( "Can't allocate video buffer memory" );
  }
  vid.buffer = video_buffer;
  vid.rowbytes = vid.width;
  newt_initialize_next = 1;
}


static void create_xil_window( void )
{
  /* TODO */
}


// ========================================================================
// Tragic death handler
// ========================================================================

void TragicDeath(int signal_num)
{
  XCloseDisplay(x_disp);
  Sys_Error( "This death brought to you by the number %d\n", signal_num );
}

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
   
  XNextEvent(x_disp, &x_event);
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
    if( XPending( x_disp ) > 0 ) {
      XEvent tmp_event;
      XPeekEvent( x_disp, &tmp_event );
      if( tmp_event.type == KeyPress &&
	  tmp_event.xkey.keycode == x_event.xkey.keycode &&
	  tmp_event.xkey.time == x_event.xkey.time ) {
	XNextEvent( x_disp, &tmp_event );
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
	XSelectInput( x_disp, x_win, STD_EVENT_MASK & ~PointerMotionMask );
	XWarpPointer( x_disp, None, x_win, 0, 0, 0, 0, 
		      (window_width / 2), (window_height / 2) );
	XSelectInput( x_disp,x_win, STD_EVENT_MASK );
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
	
  case ConfigureNotify:
    config_notify_width = x_event.xconfigure.width;
    config_notify_height = x_event.xconfigure.height;
    if( xil_keepaspect->value && !force_resize ) {
      double aspect = (double)vid.width / vid.height;
      if( config_notify_height > config_notify_width / aspect ) {
	force_resize = 1;
	XResizeWindow( x_disp, x_win,
		       config_notify_width,
		       (int)(config_notify_width / aspect) );
      }
      else if( config_notify_width > config_notify_height * aspect ) {
	force_resize = 1;
	XResizeWindow( x_disp, x_win,
		       (int)(config_notify_height * aspect),
		       config_notify_height );
      }
    }
    else {
      force_resize = 0;
    }
    if( !force_resize ) {
      window_width = config_notify_width;
      window_height = config_notify_height;
      newt_initialize_next = 1;
    }
    break;

  case MapNotify:
    if( _windowed_mouse->value ) {
      XGrabPointer( x_disp, x_win, True, 0, GrabModeAsync,
		    GrabModeAsync, x_win, None, CurrentTime);
    }
    break;

  case UnmapNotify:
    if( _windowed_mouse->value ) {
      XUngrabPointer( x_disp, CurrentTime );
    }
    break;

  case VisibilityNotify:
    puts("visibility");
    newt_initialize_next = 1;
    break;

  default:
    ;
  }
   
  if( old_windowed_mouse != _windowed_mouse->value ) {
    old_windowed_mouse = _windowed_mouse->value;

    if( !_windowed_mouse->value ) {
      /* ungrab the pointer */
      XUngrabPointer( x_disp, CurrentTime );
    } else {
      /* grab the pointer */
      XGrabPointer( x_disp, x_win, True, 0, GrabModeAsync,
		    GrabModeAsync, x_win, None, CurrentTime );
    }
  }
}

/*****************************************************************************/

/*
** SWimp_Init
**
** This routine is responsible for initializing the implementation
** specific stuff in a software rendering subsystem.
*/
int SWimp_Init( void *hInstance, void *wndProc )
{

  // open the display
  x_disp = XOpenDisplay( display_name );
  if( !x_disp ) {
    if( display_name != NULL ) {
      Sys_Error( "VID: Could not open display [%s]\n", display_name );
    }
    else {
      Sys_Error( "VID: Could not open local display\n" );
    }
  }


  // catch signals so i can turn on auto-repeat

  {
    struct sigaction sa;
    sigaction( SIGINT, 0, &sa );
    sa.sa_handler = TragicDeath;
    sigaction( SIGINT, &sa, 0 );
    sigaction( SIGTERM, &sa, 0 );
  }

  return true;
}

/*
** SWimp_InitGraphics
**
** This initializes the software refresh's implementation specific
** graphics subsystem.  In the case of Windows it creates DIB or
** DDRAW surfaces.
**
** The necessary width and height parameters are grabbed from
** vid.width and vid.height.
*/
static qboolean SWimp_InitGraphics( qboolean fullscreen )
{
  int pnum, i;
  long template_mask;

  srandom(getpid());


  // free resources in use
  SWimp_Shutdown ();

  // let the sound and input subsystems know about the new window
  ri.Vid_NewWindow (vid.width, vid.height);

  // for debugging only
  //	XSynchronize(x_disp, True);

  // check for command-line window size
  template_mask = 0;


  // setup attributes for main window
  {
    int attribmask = CWEventMask | CWBorderPixel;
    XSetWindowAttributes attribs;

    window_width = vid.width;
    window_height = vid.height;
	   
    attribs.event_mask = STD_EVENT_MASK;
    attribs.border_pixel = 0;

    // create the main window
    x_win = XCreateWindow( x_disp,
			   XRootWindow( x_disp, DefaultScreen( x_disp ) ),
			   0, 0,	// x, y
			   window_width, window_height,
			   0, // borderwidth
			   CopyFromParent,
			   InputOutput,
			   CopyFromParent,
			   attribmask,
			   &attribs );
    XStoreName( x_disp, x_win, "Quake II" );

  }

  // inviso cursor
  XDefineCursor( x_disp, x_win, CreateNullCursor( x_disp, x_win ) );

  // create the GC
  {
    XGCValues xgcvalues;
    int valuemask = GCGraphicsExposures;
    xgcvalues.graphics_exposures = False;
    x_gc = XCreateGC( x_disp, x_win, valuemask, &xgcvalues );
  }

  /* set the icon */
  {
    Pixmap icon;
    XWMHints hints;
    icon = XCreateBitmapFromData( x_disp, DefaultRootWindow( x_disp ),
				  q2icon_bits, q2icon_width, q2icon_height );
				  /*				  BlackPixel( x_disp, DefaultScreen( x_disp ) ),
				  WhitePixel( x_disp, DefaultScreen( x_disp ) ),
				  1 );*/
    hints.icon_pixmap = icon;
    hints.flags = IconPixmapHint;
    XSetWMHints( x_disp, x_win, &hints );
  }

  // map the window
  XMapWindow( x_disp, x_win );

  // wait for first exposure event
  {
    XEvent event;
    do {
      XNextEvent( x_disp, &event );
    } while( event.type != Expose || event.xexpose.count != 0 );
  }
  // now safe to draw

  ResetNEWTFrameBuffers();

  //	XSynchronize(x_disp, False);

  old_windowed_mouse = 0;
  X11_active = true;

  return true;
}

/*
** SWimp_EndFrame
**
** This does an implementation specific copy from the backbuffer to the
** front buffer.  In the Win32 case it uses BitBlt or BltFast depending
** on whether we're using DIB sections/GDI or DDRAW.
*/
void SWimp_EndFrame (void)
{
  send_newt_video( x_disp, x_win, vid.buffer, vid.width, vid.height );
}


/*
** SWimp_SetMode
*/
rserr_t SWimp_SetMode( int *pwidth, int *pheight, int mode, qboolean fullscreen )
{
  rserr_t retval = rserr_ok;

  ri.Con_Printf (PRINT_ALL, "setting mode %d:", mode );

  if( !ri.Vid_GetModeInfo( pwidth, pheight, mode ) ) {
    ri.Con_Printf( PRINT_ALL, " invalid mode\n" );
    return rserr_invalid_mode;
  }

  ri.Con_Printf( PRINT_ALL, " %d %d\n", *pwidth, *pheight );

  if( !SWimp_InitGraphics( false ) ) {
    // failed to set a valid mode in windowed mode
    return rserr_invalid_mode;
  }

  R_GammaCorrectAndSetPalette( (const unsigned char *)d_8to24table );

  return retval;
}

/*
** SWimp_SetPalette
**
** System specific palette setting routine.  A NULL palette means
** to use the existing palette.  The palette is expected to be in
** a padded 4-byte xRGB format.
*/
void SWimp_SetPalette( const unsigned char *palette )
{
  newt_set_palette( palette );
}

/*
** SWimp_Shutdown
**
** System specific graphics subsystem shutdown routine.  Destroys
** DIBs or DDRAW surfaces as appropriate.
*/
void SWimp_Shutdown( void )
{
  newt_cleanup();
  if( X11_active ) {
    XDestroyWindow( x_disp, x_win );
    XSync( x_disp, False );
  }
  if( video_buffer != NULL ) {
    free( video_buffer );
  }
}


/*
** SWimp_AppActivate
*/
void SWimp_AppActivate( qboolean active )
{
}


/*****************************************************************************/
/* KEYBOARD                                                                  */
/*****************************************************************************/

Key_Event_fp_t Key_Event_fp;

void KBD_Init(Key_Event_fp_t fp)
{
  Key_Event_fp = fp;
}

void KBD_Update(void)
{
  // get events from x server
  if (x_disp)
    {
      while (XPending(x_disp)) 
	GetEvent();
      while (keyq_head != keyq_tail)
	{
	  Key_Event_fp(keyq[keyq_tail].key, keyq[keyq_tail].down);
	  keyq_tail = (keyq_tail + 1) & 63;
	}
    }
}

void KBD_Close(void)
{
}


/*** User commands *******************************************************/

static void Set_Window_Position_f( void )
{
  int x, y;
  char *arg;
  char *endnum;

  if( Cmd_Argc() != 3 ) {
    Com_Printf( "usage: xil_set_window_position x y\n" );
    return;
  }

  arg = Cmd_Argv( 1 );
  x = strtol( arg, &endnum, 10 );
  if( endnum != arg && *endnum != 0 ) {
    Com_Printf( "illegal x position\n" );
    return;
  }

  arg = Cmd_Argv( 2 );
  y = strtol( arg, &endnum, 10 );
  if( endnum != arg && *endnum != 0 ) {
    Com_Printf( "illegal y position\n" );
    return;
  }

  XMoveWindow( x_disp, x_win, x, y );
}

static void Set_Window_Size_f( void )
{
  int width, height;
  char *arg;
  char *endnum;

  if( Cmd_Argc() != 3 ) {
    Com_Printf( "usage: xil_set_window_size width height\n" );
    return;
  }

  arg = Cmd_Argv( 1 );
  width = strtol( arg, &endnum, 10 );
  if( endnum != arg && *endnum != 0 ) {
    Com_Printf( "illegal width\n" );
    return;
  }

  arg = Cmd_Argv( 2 );
  height = strtol( arg, &endnum, 10 );
  if( endnum != arg && *endnum != 0 ) {
    Com_Printf( "illegal height\n" );
    return;
  }

  XResizeWindow( x_disp, x_win, width, height );
}
