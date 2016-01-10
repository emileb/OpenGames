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
** RW_X11.C
**
** This file contains ALL Linux specific stuff having to do with the
** software refresh.  When a port is being made the following functions
** must be implemented by the port:
**
** SWimp_EndFrame
** SWimp_Init
** SWimp_InitGraphics
** SWimp_SetPalette
** SWimp_Shutdown
** SWimp_SwitchFullscreen
*/

#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#ifdef Joystick
#include <fcntl.h>
//#include "joystick.h"
#endif
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/xf86dga.h>
#ifdef OPENGL
#include <X11/extensions/xf86vmode.h>
#endif
#ifdef Joystick
# if defined (__linux__)
#include <linux/joystick.h>
# elif defined (__FreeBSD__)
#include <sys/joystick.h>
# endif
#include <glob.h>
#endif

#ifdef OPENGL
#include "../ref_gl/gl_local.h"
#include <dlfcn.h>
#else
#include "../ref_soft/r_local.h"
#endif

#include "../client/keys.h"
#include "../linux/rw_linux.h"

#ifdef OPENGL
#include <GL/glx.h>
#include "../linux/glw_linux.h"

glwstate_t glw_state;
static int scrnum;
static GLXContext ctx = NULL;
static cvar_t	*r_fakeFullscreen;
static qboolean vidmode_ext = false;

static XF86VidModeModeInfo **vidmodes;
static int num_vidmodes;
static qboolean vidmode_active = false;
static XF86VidModeGamma oldgamma;

//GLX Functions
static XVisualInfo * (*qglXChooseVisual)( Display *dpy, int screen, int *attribList );
static GLXContext (*qglXCreateContext)( Display *dpy, XVisualInfo *vis, GLXContext shareList, Bool direct );
static void (*qglXDestroyContext)( Display *dpy, GLXContext ctx );
static Bool (*qglXMakeCurrent)( Display *dpy, GLXDrawable drawable, GLXContext ctx);
static void (*qglXCopyContext)( Display *dpy, GLXContext src, GLXContext dst, GLuint mask );
static void (*qglXSwapBuffers)( Display *dpy, GLXDrawable drawable );
#endif

/*****************************************************************************/

static qboolean			doShm;
static Display			*dpy;
#ifndef OPENGL
static Colormap			x_cmap;
static GC			x_gc;
#endif

#ifdef REDBLUE
static GC				x_gc;
#endif

static Window			win;
static Visual			*x_vis;
static XVisualInfo		*x_visinfo;
static int win_x, win_y;
static Atom wmDeleteWindow;

qboolean mouse_active;
int mx, my, mouse_buttonstate;

#define KEY_MASK (KeyPressMask | KeyReleaseMask)
#define MOUSE_MASK (ButtonPressMask | ButtonReleaseMask | \
		    PointerMotionMask | ButtonMotionMask )
#define X_MASK (KEY_MASK | MOUSE_MASK | VisibilityChangeMask | StructureNotifyMask | ExposureMask )

static int				x_shmeventtype;
//static XShmSegmentInfo	x_shminfo;

static qboolean			oktodraw = false;
static qboolean			ignorefirst = false;
static qboolean			exposureflag = false;
#ifndef OPENGL
static qboolean			X11_active = false;
#endif

qboolean have_stencil = false;

int XShmQueryExtension(Display *);
int XShmGetEventBase(Display *);

int current_framebuffer;
static XImage			*x_framebuffer[2] = { 0, 0 };
static XShmSegmentInfo	x_shminfo[2];

int config_notify=0;
int config_notify_width;
int config_notify_height;
						      
typedef unsigned short PIXEL16;
typedef unsigned long PIXEL24;
#ifdef REDBLUE
static PIXEL16 st2d_8to16table_s[2][256];
static PIXEL24 st2d_8to24table_s[2][256];
#endif
static PIXEL16 st2d_8to16table[256];
static PIXEL24 st2d_8to24table[256];

static int shiftmask_fl=0;
static long r_shift,g_shift,b_shift;
static unsigned long r_mask,g_mask,b_mask;

void shiftmask_init(void)
{
    unsigned int x;
    r_mask=x_vis->red_mask;
    g_mask=x_vis->green_mask;
    b_mask=x_vis->blue_mask;
    for(r_shift=-8,x=1;x<r_mask;x=x<<1)r_shift++;
    for(g_shift=-8,x=1;x<g_mask;x=x<<1)g_shift++;
    for(b_shift=-8,x=1;x<b_mask;x=x<<1)b_shift++;
    shiftmask_fl=1;
}

PIXEL16 xlib_rgb16(int r,int g,int b)
{
    PIXEL16 p;
    if(shiftmask_fl==0) shiftmask_init();
    p=0;

    if(r_shift>0) {
        p=(r<<(r_shift))&r_mask;
    } else if(r_shift<0) {
        p=(r>>(-r_shift))&r_mask;
    } else p|=(r&r_mask);

    if(g_shift>0) {
        p|=(g<<(g_shift))&g_mask;
    } else if(g_shift<0) {
        p|=(g>>(-g_shift))&g_mask;
    } else p|=(g&g_mask);

    if(b_shift>0) {
        p|=(b<<(b_shift))&b_mask;
    } else if(b_shift<0) {
        p|=(b>>(-b_shift))&b_mask;
    } else p|=(b&b_mask);

    return p;
}

PIXEL24 xlib_rgb24(int r,int g,int b)
{
    PIXEL24 p;
    if(shiftmask_fl==0) shiftmask_init();
    p=0;

    if(r_shift>0) {
        p=(r<<(r_shift))&r_mask;
    } else if(r_shift<0) {
        p=(r>>(-r_shift))&r_mask;
    } else p|=(r&r_mask);

    if(g_shift>0) {
        p|=(g<<(g_shift))&g_mask;
    } else if(g_shift<0) {
        p|=(g>>(-g_shift))&g_mask;
    } else p|=(g&g_mask);

    if(b_shift>0) {
        p|=(b<<(b_shift))&b_mask;
    } else if(b_shift<0) {
        p|=(b>>(-b_shift))&b_mask;
    } else p|=(b&b_mask);

    return p;
}


void st2_fixup( XImage *framebuf, int x, int y, int width, int height)
{
  int yi;
	byte *src;
	PIXEL16 *dest;
	register int count, n;

	if( (x<0)||(y<0) )return;

	for (yi = y; yi < (y+height); yi++) {
		src = (byte *)&framebuf->data [yi * framebuf->bytes_per_line];

		// Duff's Device
		count = width;
		n = (count + 7) / 8;
		dest = ((PIXEL16 *)src) + x+width - 1;
		src += x+width - 1;

		switch (count % 8) {
		case 0:	do {	*dest-- = st2d_8to16table[*src--];
		case 7:			*dest-- = st2d_8to16table[*src--];
		case 6:			*dest-- = st2d_8to16table[*src--];
		case 5:			*dest-- = st2d_8to16table[*src--];
		case 4:			*dest-- = st2d_8to16table[*src--];
		case 3:			*dest-- = st2d_8to16table[*src--];
		case 2:			*dest-- = st2d_8to16table[*src--];
		case 1:			*dest-- = st2d_8to16table[*src--];
				} while (--n > 0);
		}

//		for(xi = (x+width-1); xi >= x; xi--) {
//			dest[xi] = st2d_8to16table[src[xi]];
//		}
	}
}

#ifdef REDBLUE
void st2_fixup_stereo( XImage *framebuf1, XImage *framebuf2, 
		       int x, int y, int width, int height)
{
  int yi;
  unsigned char *src;
  PIXEL16 *dest;
  register int count, n;
 
  if( (x<0)||(y<0) )return;
  
  for (yi = y; yi < (y+height); yi++) {
    src = &framebuf1->data [yi * framebuf1->bytes_per_line];
    
    // Duff's Device
    count = width;
    n = (count + 7) / 8;
    dest = ((PIXEL16 *)src) + x+width - 1;
    src += x+width - 1;
    
    switch (count % 8) {
    case 0:	do {	*dest-- = st2d_8to16table_s[0][*src--];
    case 7:			*dest-- = st2d_8to16table_s[0][*src--];
    case 6:			*dest-- = st2d_8to16table_s[0][*src--];
    case 5:			*dest-- = st2d_8to16table_s[0][*src--];
    case 4:			*dest-- = st2d_8to16table_s[0][*src--];
    case 3:			*dest-- = st2d_8to16table_s[0][*src--];
    case 2:			*dest-- = st2d_8to16table_s[0][*src--];
    case 1:			*dest-- = st2d_8to16table_s[0][*src--];
    } while (--n > 0);
    }
  }
  
  for (yi = y; yi < (y+height); yi++) {
    src = &framebuf1->data [yi * framebuf1->bytes_per_line];
    
    // Duff's Device
    count = width;
    n = (count + 7) / 8;
    dest = ((PIXEL16 *)src) + x+width - 1;
    src = &framebuf2->data [yi * framebuf2->bytes_per_line];
    src += x+width - 1;
    
    switch (count % 8) {
    case 0:	do {	*dest-- += st2d_8to16table_s[1][*src--];
    case 7:			*dest-- += st2d_8to16table_s[1][*src--];
    case 6:			*dest-- += st2d_8to16table_s[1][*src--];
    case 5:			*dest-- += st2d_8to16table_s[1][*src--];
    case 4:			*dest-- += st2d_8to16table_s[1][*src--];
    case 3:			*dest-- += st2d_8to16table_s[1][*src--];
    case 2:			*dest-- += st2d_8to16table_s[1][*src--];
    case 1:			*dest-- += st2d_8to16table_s[1][*src--];
    } while (--n > 0);
    }
    
  }
}
#endif

void st3_fixup( XImage *framebuf, int x, int y, int width, int height)
{
	int yi;
	byte *src;
	PIXEL24 *dest;
	register int count, n;

	if( (x<0)||(y<0) )return;

	for (yi = y; yi < (y+height); yi++) {
		src = (byte *)&framebuf->data [yi * framebuf->bytes_per_line];

		// Duff's Device
		count = width;
		n = (count + 7) / 8;
		dest = ((PIXEL24 *)src) + x+width - 1;
		src += x+width - 1;

		switch (count % 8) {
		case 0:	do {	*dest-- = st2d_8to24table[*src--];
		case 7:			*dest-- = st2d_8to24table[*src--];
		case 6:			*dest-- = st2d_8to24table[*src--];
		case 5:			*dest-- = st2d_8to24table[*src--];
		case 4:			*dest-- = st2d_8to24table[*src--];
		case 3:			*dest-- = st2d_8to24table[*src--];
		case 2:			*dest-- = st2d_8to24table[*src--];
		case 1:			*dest-- = st2d_8to24table[*src--];
				} while (--n > 0);
		}

//		for(xi = (x+width-1); xi >= x; xi--) {
//			dest[xi] = st2d_8to16table[src[xi]];
//		}
	}
}

#ifdef REDBLUE
void st3_fixup_stereo( XImage *framebuf1, XImage *framebuf2, 
		       int x, int y, int width, int height)
{
  int yi;
  unsigned char *src;
  PIXEL24 *dest;
  register int count, n;

  
  if( (x<0)||(y<0) )return;
  
  for (yi = y; yi < (y+height); yi++) {
    src = &framebuf1->data [yi * framebuf1->bytes_per_line];
    
    // Duff's Device
    count = width;
    n = (count + 7) / 8;
    dest = ((PIXEL24 *)src) + x+width - 1;
    src += x+width - 1;
    
    switch (count % 8) {
    case 0:	do {	*dest-- = st2d_8to24table_s[0][*src--];
    case 7:			*dest-- = st2d_8to24table_s[0][*src--];
    case 6:			*dest-- = st2d_8to24table_s[0][*src--];
    case 5:			*dest-- = st2d_8to24table_s[0][*src--];
    case 4:			*dest-- = st2d_8to24table_s[0][*src--];
    case 3:			*dest-- = st2d_8to24table_s[0][*src--];
    case 2:			*dest-- = st2d_8to24table_s[0][*src--];
    case 1:			*dest-- = st2d_8to24table_s[0][*src--];
    } while (--n > 0);
    }
  }
  
  for (yi = y; yi < (y+height); yi++) {
    src = &framebuf1->data [yi * framebuf1->bytes_per_line];
    
    // Duff's Device
    count = width;
    n = (count + 7) / 8;
    dest = ((PIXEL24 *)src) + x+width - 1;
    src = &framebuf2->data [yi * framebuf2->bytes_per_line];
    src += x+width - 1;
    
    switch (count % 8) {
    case 0:	do {	*dest-- += st2d_8to24table_s[1][*src--];
    case 7:			*dest-- += st2d_8to24table_s[1][*src--];
    case 6:			*dest-- += st2d_8to24table_s[1][*src--];
    case 5:			*dest-- += st2d_8to24table_s[1][*src--];
    case 4:			*dest-- += st2d_8to24table_s[1][*src--];
    case 3:			*dest-- += st2d_8to24table_s[1][*src--];
    case 2:			*dest-- += st2d_8to24table_s[1][*src--];
    case 1:			*dest-- += st2d_8to24table_s[1][*src--];
    } while (--n > 0);
    }
  }
}
#endif


// Console variables that we need to access from this module

/*****************************************************************************/
/* MOUSE                                                                     */
/*****************************************************************************/

// this is inside the renderer shared lib, so these are called from vid_so

static qboolean dgamouse = false;

static cvar_t	*in_dgamouse;

static cvar_t	*vid_xpos;		     // X coordinate of window position
static cvar_t	*vid_ypos;		     // Y coordinate of window position

#ifdef Joystick
static int joy_fd;
#endif

static Time myxtime;

#ifdef Joystick
qboolean OpenJoystick(cvar_t *joy_dev) {
  int i, err;
  glob_t pglob;
  struct js_event e;

  err = glob(joy_dev->string, 0, NULL, &pglob);

  if (err) {
    switch (err) {
    case GLOB_NOSPACE:
      ri.Con_Printf(PRINT_ALL, "Error, out of memory while looking for joysticks\n");
      break;
    case GLOB_NOMATCH:
      ri.Con_Printf(PRINT_ALL, "No joysticks found\n");
      break;
    default:
      ri.Con_Printf(PRINT_ALL, "Error #%d while looking for joysticks\n",err);
    }
    return false;
  }  
  
  for (i=0;i<pglob.gl_pathc;i++) {
    ri.Con_Printf(PRINT_ALL, "Trying joystick dev %s\n", pglob.gl_pathv[i]);
    joy_fd = open (pglob.gl_pathv[i], O_RDONLY | O_NONBLOCK);
    if (joy_fd == -1) {
      ri.Con_Printf(PRINT_ALL, "Error opening joystick dev %s\n", 
		    pglob.gl_pathv[i]);
      return false;
    }
    else {
      while (read(joy_fd, &e, sizeof(struct js_event))!=-1 &&
	     (e.type & JS_EVENT_INIT))
	ri.Con_Printf(PRINT_ALL, "Read init event\n");
      ri.Con_Printf(PRINT_ALL, "Using joystick dev %s\n", pglob.gl_pathv[i]);
      return true;
    }
  }
  globfree(&pglob);
  return false;
}
#endif

void RW_IN_PlatformInit() {
  in_dgamouse = ri.Cvar_Get ("in_dgamouse", "1", CVAR_ARCHIVE);
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

    cursormask = XCreatePixmap(display, root, 1, 1, 1/*depth*/);
    xgc.function = GXclear;
    gc =  XCreateGC(display, cursormask, GCFunction, &xgc);
    XFillRectangle(display, cursormask, gc, 0, 0, 1, 1);
    dummycolour.pixel = 0;
    dummycolour.red = 0;
    dummycolour.flags = 04;
    cursor = XCreatePixmapCursor(display, cursormask, cursormask,
          &dummycolour,&dummycolour, 0,0);
    XFreePixmap(display,cursormask);
    XFreeGC(display,gc);
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

	if (in_dgamouse->value) {
		int MajorVersion, MinorVersion;

		if (!XF86DGAQueryVersion(dpy, &MajorVersion, &MinorVersion)) { 
			// unable to query, probalby not supported
			ri.Con_Printf( PRINT_ALL, "Failed to detect XF86DGA Mouse\n" );
			ri.Cvar_Set( "in_dgamouse", "0" );
		} else {
			dgamouse = true;
			XF86DGADirectVideo(dpy, DefaultScreen(dpy), XF86DGADirectMouse);
			XWarpPointer(dpy, None, win, 0, 0, 0, 0, 0, 0);
		}
	} else
		XWarpPointer(dpy, None, win, 0, 0, 0, 0, vid.width / 2, vid.height / 2);

	XGrabKeyboard(dpy, win,
		      False,
		      GrabModeAsync, GrabModeAsync,
		      CurrentTime);

	mouse_active = true;

	ignorefirst = true;

//	XSync(dpy, True);
}

static void uninstall_grabs(void)
{
	if (!dpy || !win)
		return;

	if (dgamouse) {
		dgamouse = false;
		XF86DGADirectVideo(dpy, DefaultScreen(dpy), 0);
	}

	XUngrabPointer(dpy, CurrentTime);
	XUngrabKeyboard(dpy, CurrentTime);

// inviso cursor
	XUndefineCursor(dpy, win);

	mouse_active = false;
}

static void IN_DeactivateMouse( void ) 
{
  //***BAD***
  //if (!mouse_avail || !dpy || !win)
  //return;

  if (mouse_active) {
    uninstall_grabs();
    mouse_active = false;
  }
}

static void IN_ActivateMouse( void ) 
{
  //***BAD***
  //if (!mouse_avail || !dpy || !win)
  //return;
  
  if (!mouse_active) {
    mx = my = 0; // don't spazz
    install_grabs();
    mouse_active = true;
  }
}

void getMouse(int *x, int *y, int *state) {
  *x = mx;
  *y = my;
  *state = mouse_buttonstate;
}

void doneMouse() {
  mx = my = 0;
}

void RW_IN_Activate(qboolean active)
{
  if (active)
    IN_ActivateMouse();
  else
    IN_DeactivateMouse();
}

#ifdef Joystick
qboolean CloseJoystick(void) {
  if (close(joy_fd))
    ri.Con_Printf(PRINT_ALL, "Error, Problem closing joystick.");
  return true;
}
#endif

/*****************************************************************************/

char *RW_Sys_GetClipboardData()
{
  Window sowner;
  Atom type, property;
  unsigned long len, bytes_left, tmp;
  unsigned char *data;
  int format, result;
  char *ret = NULL;
  
  sowner = XGetSelectionOwner(dpy, XA_PRIMARY);
  
  if (sowner != None) {
    property = XInternAtom(dpy,
			   "GETCLIPBOARDDATA_PROP",
			   False);
    
    XConvertSelection(dpy,
		      XA_PRIMARY, XA_STRING,
		      property, win, myxtime); /* myxtime == time of last X event */
    XFlush(dpy);
    
    XGetWindowProperty(dpy,
		       win, property,
		       0, 0, False, AnyPropertyType,
		       &type, &format, &len,
		       &bytes_left, &data);
    if (bytes_left > 0) {
      result =
	XGetWindowProperty(dpy,
			   win, property,
			   0, bytes_left, True, AnyPropertyType,
			   &type, &format, &len,
			   &tmp, &data);
      if (result == Success) {
	ret = strdup(data);
      }
      XFree(data);
    }
  }
  return ret;
}

/*****************************************************************************/
#ifndef OPENGL
void ResetFrameBuffer(void)
{
  int mem;
  int pwidth;
  
  if (x_framebuffer[0]) {
    free(x_framebuffer[0]->data);
    free(x_framebuffer[0]);
  }
  
#ifdef REDBLUE
  if (x_framebuffer[1]) {
    free(x_framebuffer[1]->data);
    free(x_framebuffer[1]);
  }
#endif
  
  // alloc an extra line in case we want to wrap, and allocate the z-buffer
  pwidth = x_visinfo->depth / 8;
  if (pwidth == 3) pwidth = 4;
  mem = ((vid.width*pwidth+7)&~7) * vid.height;
  
  x_framebuffer[0] = XCreateImage(dpy,
				  x_vis,
				  x_visinfo->depth,
				  ZPixmap,
				  0,
				  malloc(mem),
				  vid.width, vid.height,
				  32,
				  0);
  
  if (!x_framebuffer[0])
    Sys_Error("VID: XCreateImage failed\n");
  
#ifdef REDBLUE
  x_framebuffer[1] = XCreateImage(dpy,
				  x_vis,
				  x_visinfo->depth,
				  ZPixmap,
				  0,
				  malloc(mem),
				  vid.width, vid.height,
				  32,
				  0);
  
  if (!x_framebuffer[1])
    Sys_Error("VID: XCreateImage failed\n");
#endif
  
  vid.buffer = (byte*) (x_framebuffer[0]->data);
}
#endif

void ResetSharedFrameBuffers(void)
{
  int size;
  int key;
  int minsize = getpagesize();
  int frm;
  
  for (frm=0 ; frm<2 ; frm++)
    {
      // free up old frame buffer memory
      if (x_framebuffer[frm])
	{
	  XShmDetach(dpy, &x_shminfo[frm]);
	  free(x_framebuffer[frm]);
	  shmdt(x_shminfo[frm].shmaddr);
	}
      
      // create the image
      x_framebuffer[frm] = XShmCreateImage(	dpy,
						x_vis,
						x_visinfo->depth,
						ZPixmap,
						0,
						&x_shminfo[frm],
						vid.width,
						vid.height );

	// grab shared memory

		size = x_framebuffer[frm]->bytes_per_line
			* x_framebuffer[frm]->height;
		if (size < minsize)
			Sys_Error("VID: Window must use at least %d bytes\n", minsize);

		key = random();
		x_shminfo[frm].shmid = shmget((key_t)key, size, IPC_CREAT|0777);
		if (x_shminfo[frm].shmid==-1)
			Sys_Error("VID: Could not get any shared memory\n");

		// attach to the shared memory segment
		x_shminfo[frm].shmaddr =
			(void *) shmat(x_shminfo[frm].shmid, 0, 0);

		ri.Con_Printf(PRINT_DEVELOPER, "MITSHM shared memory (id=%d, addr=0x%lx)\n", 
			x_shminfo[frm].shmid, (long) x_shminfo[frm].shmaddr);

		x_framebuffer[frm]->data = x_shminfo[frm].shmaddr;

	// get the X server to attach to it

		if (!XShmAttach(dpy, &x_shminfo[frm]))
			Sys_Error("VID: XShmAttach() failed\n");
		XSync(dpy, 0);
		shmctl(x_shminfo[frm].shmid, IPC_RMID, 0);
	}

}

// ========================================================================
// Tragic death handler
// ========================================================================

void TragicDeath(int signal_num)
{
//	XAutoRepeatOn(dpy);
	XCloseDisplay(dpy);
	Sys_Error("This death brought to you by the number %d\n", signal_num);
}

int XLateKey(XKeyEvent *ev)
{

	int key;
	char buf[64];
	KeySym keysym;

	key = 0;

	XLookupString(ev, buf, sizeof buf, &keysym, 0);

	switch(keysym)
	{
		case XK_KP_Page_Up:	 key = K_KP_PGUP; break;
		case XK_Page_Up:	 key = K_PGUP; break;

		case XK_KP_Page_Down: key = K_KP_PGDN; break;
		case XK_Page_Down:	 key = K_PGDN; break;

		case XK_KP_Home: key = K_KP_HOME; break;
		case XK_Home:	 key = K_HOME; break;

		case XK_KP_End:  key = K_KP_END; break;
		case XK_End:	 key = K_END; break;

		case XK_KP_Left: key = K_KP_LEFTARROW; break;
		case XK_Left:	 key = K_LEFTARROW; break;

		case XK_KP_Right: key = K_KP_RIGHTARROW; break;
		case XK_Right:	key = K_RIGHTARROW;		break;

		case XK_KP_Down: key = K_KP_DOWNARROW; break;
		case XK_Down:	 key = K_DOWNARROW; break;

		case XK_KP_Up:   key = K_KP_UPARROW; break;
		case XK_Up:		 key = K_UPARROW;	 break;

		case XK_Escape: key = K_ESCAPE;		break;

		case XK_KP_Enter: key = K_KP_ENTER;	break;
		case XK_Return: key = K_ENTER;		 break;

		case XK_Tab:		key = K_TAB;			 break;

		case XK_F1:		 key = K_F1;				break;

		case XK_F2:		 key = K_F2;				break;

		case XK_F3:		 key = K_F3;				break;

		case XK_F4:		 key = K_F4;				break;

		case XK_F5:		 key = K_F5;				break;

		case XK_F6:		 key = K_F6;				break;

		case XK_F7:		 key = K_F7;				break;

		case XK_F8:		 key = K_F8;				break;

		case XK_F9:		 key = K_F9;				break;

		case XK_F10:		key = K_F10;			 break;

		case XK_F11:		key = K_F11;			 break;

		case XK_F12:		key = K_F12;			 break;

		case XK_BackSpace: key = K_BACKSPACE; break;

		case XK_KP_Delete: key = K_KP_DEL; break;
		case XK_Delete: key = K_DEL; break;

		case XK_Pause:	key = K_PAUSE;		 break;

		case XK_Shift_L:
		case XK_Shift_R:	key = K_SHIFT;		break;

		case XK_Execute: 
		case XK_Control_L: 
		case XK_Control_R:	key = K_CTRL;		 break;

		case XK_Alt_L:	
		case XK_Meta_L: 
		case XK_Alt_R:	
		case XK_Meta_R: key = K_ALT;			break;

		case XK_KP_Begin: key = K_KP_5;	break;

		case XK_Insert:key = K_INS; break;
		case XK_KP_Insert: key = K_KP_INS; break;

		case XK_KP_Multiply: key = '*'; break;
		case XK_KP_Add:  key = K_KP_PLUS; break;
		case XK_KP_Subtract: key = K_KP_MINUS; break;
		case XK_KP_Divide: key = K_KP_SLASH; break;

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
			if (key >= 1 && key <= 26) /* ctrl+alpha */
				key = key + 'a' - 1;
			break;
	} 

	return key;
}

/* Check to see if this is a repeated key.
   (idea shamelessly lifted from SDL who...)
   (idea shamelessly lifted from GII -- thanks guys! :)
   This has bugs if two keys are being pressed simultaneously and the
   events start getting interleaved.
*/
int X11_KeyRepeat(Display *display, XEvent *event)
{
	XEvent peekevent;
	int repeated;

	repeated = 0;
	if ( XPending(display) ) {
		XPeekEvent(display, &peekevent);
		if ( (peekevent.type == KeyPress) &&
(peekevent.xkey.keycode == event->xkey.keycode) &&
((peekevent.xkey.time-event->xkey.time) < 2) ) {
			repeated = 1;
			XNextEvent(display, &peekevent);
		}
	}
	return(repeated);
}

void HandleEvents(void)
{
  XEvent event;
  int b;
  qboolean dowarp = false;
  int mwx = vid.width/2;
  int mwy = vid.height/2;
  
  in_state_t *in_state = getState();
  
  while (XPending(dpy)) {
    XNextEvent(dpy, &event);
    
    switch(event.type) {
    case KeyPress:
      myxtime = event.xkey.time;
      if (in_state && in_state->Key_Event_fp)
	in_state->Key_Event_fp (XLateKey(&event.xkey), true);
      break;
    case KeyRelease:
      if (! X11_KeyRepeat(dpy, &event)) {
	if (in_state && in_state->Key_Event_fp)
	  in_state->Key_Event_fp (XLateKey(&event.xkey), false);
      }
      break;
      
    case MotionNotify:
      if (ignorefirst) {
	ignorefirst = false;
	break;
      }
      if (mouse_active) {
	if (dgamouse) {
	  mx += (event.xmotion.x + win_x)*2;
	  my += (event.xmotion.y + win_y)*2;
	} 
	else 
	  {
	    mx += ((int)event.xmotion.x - mwx)*2;
	    my -= ((int)event.xmotion.y - mwy)*2;
	    mwx = event.xmotion.x;
	    mwy = event.xmotion.y;
	    
	    if (mx || my)
	      dowarp = true;
	  }
      }
      break;
      
    case ButtonPress:
      myxtime = event.xbutton.time;
      
      b=-1;
      if (event.xbutton.button == 1)
	b = 0;
      else if (event.xbutton.button == 2)
	b = 2;
      else if (event.xbutton.button == 3)
	b = 1;
      else if (event.xbutton.button == 4)
	in_state->Key_Event_fp (K_MWHEELUP, 1);
      else if (event.xbutton.button == 5)
	in_state->Key_Event_fp (K_MWHEELDOWN, 1);
      if (b>=0)
	mouse_buttonstate |= 1<<b;
      break;
      
    case ButtonRelease:
      b=-1;
      if (event.xbutton.button == 1)
	b = 0;
      else if (event.xbutton.button == 2)
	b = 2;
      else if (event.xbutton.button == 3)
	b = 1;
      else if (event.xbutton.button == 4)
	in_state->Key_Event_fp (K_MWHEELUP, 0);
      else if (event.xbutton.button == 5)
	in_state->Key_Event_fp (K_MWHEELDOWN, 0);
      if (b>=0)
	mouse_buttonstate &= ~(1<<b);
      break;
      
    case CreateNotify :
      ri.Cvar_Set( "vid_xpos", va("%d", event.xcreatewindow.x));
      ri.Cvar_Set( "vid_ypos", va("%d", event.xcreatewindow.y));
      vid_xpos->modified = false;
      vid_ypos->modified = false;
      win_x = event.xcreatewindow.x;
      win_y = event.xcreatewindow.y;
      break;
      
    case ConfigureNotify :
      ri.Cvar_Set( "vid_xpos", va("%d", event.xcreatewindow.x));
      ri.Cvar_Set( "vid_ypos", va("%d", event.xcreatewindow.y));
      vid_xpos->modified = false;
      vid_ypos->modified = false;
      win_x = event.xconfigure.x;
      win_y = event.xconfigure.y;
      config_notify_width = event.xconfigure.width;
      config_notify_height = event.xconfigure.height;
      if (config_notify_width != vid.width ||
	  config_notify_height != vid.height)
	XMoveResizeWindow(dpy, win, win_x, win_y, vid.width, vid.height);
      config_notify = 1;
      break;
      
    case ClientMessage:
      if (event.xclient.data.l[0] == wmDeleteWindow)
	ri.Cmd_ExecuteText(EXEC_NOW, "quit");
      break;
    default:
      if (doShm && event.type == x_shmeventtype)
	oktodraw = true;
      if (event.type == Expose && !event.xexpose.count)
	exposureflag = true;
    }
  }
  
  if (dowarp) {
    /* move the mouse to the window center again */
    XWarpPointer(dpy,None,win,0,0,0,0, vid.width/2,vid.height/2);
  }
}

/*****************************************************************************/

#ifndef OPENGL
/*
** SWimp_Init
**
** This routine is responsible for initializing the implementation
** specific stuff in a software rendering subsystem.
*/
int SWimp_Init( void *hInstance, void *wndProc )
{

  vid_xpos = ri.Cvar_Get ("vid_xpos", "3", CVAR_ARCHIVE);
  vid_ypos = ri.Cvar_Get ("vid_ypos", "22", CVAR_ARCHIVE);
  
  // open the display
  dpy = XOpenDisplay(NULL);
  if (!dpy)
    {
      if (getenv("DISPLAY"))
	Sys_Error("VID: Could not open display [%s]\n",
		  getenv("DISPLAY"));
      else
	Sys_Error("VID: Could not open local display\n");
    }
  
  // catch signals so i can turn on auto-repeat
  
  {
    struct sigaction sa;
    sigaction(SIGINT, 0, &sa);
    sa.sa_handler = TragicDeath;
    sigaction(SIGINT, &sa, 0);
    sigaction(SIGTERM, &sa, 0);
  }
  
  return true;
}
#endif

#ifndef OPENGL
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
	int i;
	XVisualInfo template;
	int num_visuals;
	int template_mask;
	Window root;
	//int pnum;

	srandom(getpid());

	// free resources in use
	SWimp_Shutdown ();

	// let the sound and input subsystems know about the new window
	ri.Vid_NewWindow (vid.width, vid.height);

//	XAutoRepeatOff(dpy);

// for debugging only
	XSynchronize(dpy, True);

// check for command-line window size
	template_mask = 0;

#if 0
// specify a visual id
	if ((pnum=COM_CheckParm("-visualid")))
	{
		if (pnum >= com_argc-1)
			Sys_Error("VID: -visualid <id#>\n");
		template.visualid = Q_atoi(com_argv[pnum+1]);
		template_mask = VisualIDMask;
	}

// If not specified, use default visual
	else
#endif
	{
		int screen;
		screen = XDefaultScreen(dpy);
		template.visualid =
			XVisualIDFromVisual(XDefaultVisual(dpy, screen));
		template_mask = VisualIDMask;
	}

// pick a visual- warn if more than one was available
	x_visinfo = XGetVisualInfo(dpy, template_mask, &template, &num_visuals);
	if (num_visuals > 1)
	{
		printf("Found more than one visual id at depth %d:\n", template.depth);
		for (i=0 ; i<num_visuals ; i++)
			printf("	-visualid %d\n", (int)(x_visinfo[i].visualid));
	}
	else if (num_visuals == 0)
	{
		if (template_mask == VisualIDMask)
			Sys_Error("VID: Bad visual id %d\n", template.visualid);
		else
			Sys_Error("VID: No visuals at depth %d\n", template.depth);
	}

#if 0
	printf("Using visualid %d:\n", (int)(x_visinfo->visualid));
	printf("	screen %d\n", x_visinfo->screen);
	printf("	red_mask 0x%x\n", (int)(x_visinfo->red_mask));
	printf("	green_mask 0x%x\n", (int)(x_visinfo->green_mask));
	printf("	blue_mask 0x%x\n", (int)(x_visinfo->blue_mask));
	printf("	colormap_size %d\n", x_visinfo->colormap_size);
	printf("	bits_per_rgb %d\n", x_visinfo->bits_per_rgb);
#endif

	x_vis = x_visinfo->visual;
	root = XRootWindow(dpy, x_visinfo->screen);

// setup attributes for main window
	{
	   int attribmask = CWEventMask  | CWColormap | CWBorderPixel;
	   XSetWindowAttributes attribs;
	   XSizeHints *sizehints;
	   XWMHints *wmhints;
	   Colormap tmpcmap;
	   
	   tmpcmap = XCreateColormap(dpy, root, x_vis, AllocNone);
	   
	   attribs.event_mask = X_MASK;
	   attribs.border_pixel = 0;
	   attribs.colormap = tmpcmap;

// create the main window
		win = XCreateWindow(dpy, root, (int)vid_xpos->value, (int)vid_ypos->value, 
			vid.width, vid.height, 0, x_visinfo->depth, InputOutput, x_vis, 
			attribmask, &attribs );
		
		sizehints = XAllocSizeHints();
		if (sizehints) {
			sizehints->min_width = vid.width;
			sizehints->min_height = vid.height;
			sizehints->max_width = vid.width;
			sizehints->max_height = vid.height;
			sizehints->base_width = vid.width;
			sizehints->base_height = vid.height;
			
			sizehints->flags = PMinSize | PMaxSize | PBaseSize;
		}
		
		wmhints = XAllocWMHints();
		if (wmhints) {
			#include "q2icon.xbm"

			Pixmap icon_pixmap, icon_mask;
			unsigned long fg, bg;
			int i;
		
			fg = BlackPixel(dpy, x_visinfo->screen);
			bg = WhitePixel(dpy, x_visinfo->screen);
			icon_pixmap = XCreatePixmapFromBitmapData(dpy, win, (char *)q2icon_bits, q2icon_width, q2icon_height, fg, bg, x_visinfo->depth);
			for (i = 0; i < sizeof(q2icon_bits); i++)
				q2icon_bits[i] = ~q2icon_bits[i];
			icon_mask = XCreatePixmapFromBitmapData(dpy, win, (char *)q2icon_bits, q2icon_width, q2icon_height, bg, fg, x_visinfo->depth); 
		
			wmhints->flags = IconPixmapHint|IconMaskHint;
			wmhints->icon_pixmap = icon_pixmap;
			wmhints->icon_mask = icon_mask;
		}

		XSetWMProperties(dpy, win, NULL, NULL, NULL, 0,
			sizehints, wmhints, None);
		if (sizehints)
			XFree(sizehints);
		if (wmhints)
			XFree(wmhints);
			
		XStoreName(dpy, win, "Quake II");

		wmDeleteWindow = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(dpy, win, &wmDeleteWindow, 1);
		
		if (x_visinfo->class != TrueColor)
			XFreeColormap(dpy, tmpcmap);
	}

	if (x_visinfo->depth == 8)
	{
	// create and upload the palette
		if (x_visinfo->class == PseudoColor)
		{
			x_cmap = XCreateColormap(dpy, win, x_vis, AllocAll);
			XSetWindowColormap(dpy, win, x_cmap);
		}

	}

// create the GC
	{
		XGCValues xgcvalues;
		int valuemask = GCGraphicsExposures;
		xgcvalues.graphics_exposures = False;
		x_gc = XCreateGC(dpy, win, valuemask, &xgcvalues );
	}

	XMapWindow(dpy, win);
	XMoveWindow(dpy, win, (int)vid_xpos->value, (int)vid_ypos->value);

// wait for first exposure event
	{
	  exposureflag = false;
	  do
	    {
	      HandleEvents();
	    } while (!exposureflag);
	}
	// now safe to draw
#ifdef REDBLUE
	doShm = false;
#endif
// even if MITSHM is available, make sure it's a local connection
#ifdef REDBLUE
	/*
#endif
	if (XShmQueryExtension(dpy))
	{
		char *displayname;
		doShm = true;
		displayname = (char *) getenv("DISPLAY");
		if (displayname)
		{
			char *dptr = strdup(displayname);
			char *d;
			
			d = dptr;
			while (*d && (*d != ':')) d++;
			if (*d) *d = 0;
			if (!(!strcasecmp(displayname, "unix") || !*displayname))
				doShm = false;
			
			free(dptr);
		}
	}
#ifdef REDBLUE
	*/
#endif
	if (doShm)
	{
		x_shmeventtype = XShmGetEventBase(dpy) + ShmCompletion;
		ResetSharedFrameBuffers();
	}
	else
		ResetFrameBuffer();

	current_framebuffer = 0;
	vid.rowbytes = x_framebuffer[0]->bytes_per_line;
	vid.buffer = (byte *)x_framebuffer[0]->data;

//	XSynchronize(dpy, False);

	X11_active = true;

	return true;
}
#endif

#ifndef OPENGL
/*
** SWimp_EndFrame
**
** This does an implementation specific copy from the backbuffer to the
** front buffer.  In the Win32 case it uses BitBlt or BltFast depending
** on whether we're using DIB sections/GDI or DDRAW.
*/
void SWimp_EndFrame (void)
{
// if the window changes dimension, skip this frame
#if 0
	if (config_notify)
	{
		fprintf(stderr, "config notify\n");
		config_notify = 0;
		vid.width = config_notify_width & ~7;
		vid.height = config_notify_height;
		if (doShm)
			ResetSharedFrameBuffers();
		else
			ResetFrameBuffer();
		vid.rowbytes = x_framebuffer[0]->bytes_per_line;
		vid.buffer = x_framebuffer[current_framebuffer]->data;
		vid.recalc_refdef = 1;				// force a surface cache flush
		Con_CheckResize();
		Con_Clear_f();
		return;
	}
#endif

	if (doShm)
	{
	  if (x_visinfo->depth == 16)
	    st2_fixup( x_framebuffer[current_framebuffer], 0, 0, vid.width, vid.height);
	  else if (x_visinfo->depth == 24)
	    st3_fixup( x_framebuffer[current_framebuffer], 0, 0, vid.width, vid.height);
		if (!XShmPutImage(dpy, win, x_gc,
			x_framebuffer[current_framebuffer], 0, 0, 0, 0, vid.width, vid.height, True))
			Sys_Error("VID_Update: XShmPutImage failed\n");
		oktodraw = false;
		while (!oktodraw) 
			HandleEvents();
		current_framebuffer = !current_framebuffer;
		vid.buffer = (byte *)x_framebuffer[current_framebuffer]->data;
		XSync(dpy, False);
	}
	else
	{

#ifdef REDBLUE
	  if (x_visinfo->depth == 16)
	    st2_fixup_stereo( x_framebuffer[0], x_framebuffer[1], 
			      0, 0, vid.width, vid.height);
	  else if (x_visinfo->depth == 24)
	    st3_fixup_stereo( x_framebuffer[0], x_framebuffer[1], 
			      0, 0, vid.width, vid.height);
#else	
	  if (x_visinfo->depth == 16)
	    st2_fixup( x_framebuffer[current_framebuffer], 
		       0, 0, vid.width, vid.height);
	  else if (x_visinfo->depth == 24)
	    st3_fixup( x_framebuffer[current_framebuffer], 
		       0, 0, vid.width, vid.height);
#endif
	  XPutImage(dpy, win, x_gc, x_framebuffer[0], 0, 
		    0, 0, 0, vid.width, vid.height);
	  XSync(dpy, False);
	}
}
#endif

#ifndef OPENGL
/*
** SWimp_SetMode
*/
rserr_t SWimp_SetMode( int *pwidth, int *pheight, int mode, qboolean fullscreen )
{
	rserr_t retval = rserr_ok;

	ri.Con_Printf (PRINT_ALL, "setting mode %d:", mode );

	if ( !ri.Vid_GetModeInfo( pwidth, pheight, mode ) )
	{
		ri.Con_Printf( PRINT_ALL, " invalid mode\n" );
		return rserr_invalid_mode;
	}

	ri.Con_Printf( PRINT_ALL, " %d %d\n", *pwidth, *pheight);

	if ( !SWimp_InitGraphics( false ) ) {
		// failed to set a valid mode in windowed mode
		return rserr_invalid_mode;
	}

	R_GammaCorrectAndSetPalette( ( const unsigned char * ) d_8to24table );

	return retval;
}
#endif

#ifndef OPENGL
/*
** SWimp_SetPalette
**
** System specific palette setting routine.  A NULL palette means
** to use the existing palette.  The palette is expected to be in
** a padded 4-byte xRGB format.
*/
void SWimp_SetPalette( const unsigned char *palette )
{
	int i;
	XColor colors[256];

	if (!X11_active)
		return;

	if ( !palette )
		palette = ( const unsigned char * ) sw_state.currentpalette;
 
	for(i=0;i<256;i++) {
#ifdef REDBLUE
	  int tmp = (30*palette[i*4] + 59*palette[i*4+1] + 11*palette[i*4+2]) / 100;
	  st2d_8to16table_s[0][i]= xlib_rgb16(tmp,0,0);
	  st2d_8to24table_s[0][i]= xlib_rgb24(tmp,0,0);
	  st2d_8to16table_s[1][i]= xlib_rgb16(0,0,tmp);
	  st2d_8to24table_s[1][i]= xlib_rgb24(0,0,tmp);
#endif
	  st2d_8to16table[i]= xlib_rgb16(palette[i*4], 
					 palette[i*4+1],palette[i*4+2]);
	  st2d_8to24table[i]= xlib_rgb24(palette[i*4], 
					 palette[i*4+1],palette[i*4+2]);
	}

	if (x_visinfo->class == PseudoColor && x_visinfo->depth == 8)
	{
		for (i=0 ; i<256 ; i++)
		{
			colors[i].pixel = i;
			colors[i].flags = DoRed|DoGreen|DoBlue;
			colors[i].red = palette[i*4] * 257;
			colors[i].green = palette[i*4+1] * 257;
			colors[i].blue = palette[i*4+2] * 257;
		}
		XStoreColors(dpy, x_cmap, colors, 256);
	}
}
#endif

#ifndef OPENGL
/*
** SWimp_Shutdown
**
** System specific graphics subsystem shutdown routine.  Destroys
** DIBs or DDRAW surfaces as appropriate.
*/
void SWimp_Shutdown( void )
{
	int i;

	if (!X11_active)
		return;

	if (doShm) {
		for (i = 0; i < 2; i++)
			if (x_framebuffer[i]) {
				XShmDetach(dpy, &x_shminfo[i]);
				free(x_framebuffer[i]);
				shmdt(x_shminfo[i].shmaddr);
				x_framebuffer[i] = NULL;
			}
	} else {
	  if (x_framebuffer[0]) {
	    free(x_framebuffer[0]->data);
	    free(x_framebuffer[0]);
	    x_framebuffer[0] = NULL;
	  }
	  if (x_framebuffer[1]) {
	    free(x_framebuffer[1]->data);
	    free(x_framebuffer[1]);
	    x_framebuffer[1] = NULL;
	  }
	}

	XDestroyWindow(	dpy, win );

	win = 0;
	
//	XAutoRepeatOn(dpy);
//	XCloseDisplay(dpy);

	X11_active = false;
}
#endif

#ifndef OPENGL
/*
** SWimp_AppActivate
*/
void SWimp_AppActivate( qboolean active )
{
}

#ifdef REDBLUE
void SetStereoBuffer(int buf)
{
  if (x_framebuffer[buf])
    vid.buffer = (byte*) (x_framebuffer[buf]->data);
  else
    printf("SetStereoBuffer: x_framebuffer[%d] is NULL\n", buf);
}
#endif
#endif
//===============================================================================

/*
================
Sys_MakeCodeWriteable
================
*/
void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{

	int r;
	unsigned long addr;
	int psize = getpagesize();

	addr = (startaddr & ~(psize-1)) - psize;

//	fprintf(stderr, "writable code %lx(%lx)-%lx, length=%lx\n", startaddr,
//			addr, startaddr+length, length);

	r = mprotect((char*)addr, length + startaddr - addr + psize, 7);

	if (r < 0)
    		Sys_Error("Protection change failed\n");

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
  HandleEvents();
}

void KBD_Close(void)
{
}

#ifdef OPENGL

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
** GLimp_AppActivate
*/
void GLimp_AppActivate( qboolean active )
{
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
		
		return true;
	}
	
	return false;
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
		None
	};
	Window root;
	XVisualInfo *visinfo;
	XSetWindowAttributes attr;
	XSizeHints *sizehints;
	XWMHints *wmhints;
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

	// destroy the existing window
	GLimp_Shutdown ();

#if 0 // this breaks getenv()? - sbf
	// Mesa VooDoo hacks
	if (fullscreen)
		putenv("MESA_GLX_FX=fullscreen");
	else
		putenv("MESA_GLX_FX=window");
#endif

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "Error couldn't open the X display\n");
		return rserr_invalid_mode;
	}

	scrnum = DefaultScreen(dpy);
	root = RootWindow(dpy, scrnum);

	// Get video mode list
	MajorVersion = MinorVersion = 0;
	if (!XF86VidModeQueryVersion(dpy, &MajorVersion, &MinorVersion)) { 
		vidmode_ext = false;
	} else {
		ri.Con_Printf(PRINT_ALL, "Using XFree86-VidModeExtension Version %d.%d\n",
			MajorVersion, MinorVersion);
		vidmode_ext = true;
	}

	visinfo = qglXChooseVisual(dpy, scrnum, attrib);
	if (!visinfo) {
		fprintf(stderr, "Error couldn't get an RGB, Double-buffered, Depth visual\n");
		return rserr_invalid_mode;
	}

	gl_state.hwgamma = false;

	if (vidmode_ext) {
		int best_fit, best_dist, dist, x, y;
		
		XF86VidModeGetAllModeLines(dpy, scrnum, &num_vidmodes, &vidmodes);

		// Are we going fullscreen?  If so, let's change video mode
		if (fullscreen && !r_fakeFullscreen->value) {
			best_dist = 9999999;
			best_fit = -1;

			for (i = 0; i < num_vidmodes; i++) {
				if (width > vidmodes[i]->hdisplay ||
					height > vidmodes[i]->vdisplay)
					continue;

				x = width - vidmodes[i]->hdisplay;
				y = height - vidmodes[i]->vdisplay;
				dist = (x * x) + (y * y);
				if (dist < best_dist) {
					best_dist = dist;
					best_fit = i;
				}
			}

			if (best_fit != -1) {
				actualWidth = vidmodes[best_fit]->hdisplay;
				actualHeight = vidmodes[best_fit]->vdisplay;

				// change to the mode
				XF86VidModeSwitchToMode(dpy, scrnum, vidmodes[best_fit]);
				vidmode_active = true;

				if (XF86VidModeGetGamma(dpy, scrnum, &oldgamma)) {
					gl_state.hwgamma = true;
					/* We can not reliably detect hardware gamma
					   changes across software gamma calls, which
					   can reset the flag, so change it anyway */
					vid_gamma->modified = true;
					ri.Con_Printf( PRINT_ALL, "Using hardware gamma\n");
				}

				// Move the viewport to top left
				XF86VidModeSetViewPort(dpy, scrnum, 0, 0);
			} else
				fullscreen = 0;
		}
	}

	/* window attributes */
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);
	attr.event_mask = X_MASK;
	if (vidmode_active) {
		mask = CWBackPixel | CWColormap | CWSaveUnder | CWBackingStore | 
			CWEventMask | CWOverrideRedirect;
		attr.override_redirect = True;
		attr.backing_store = NotUseful;
		attr.save_under = False;
	} else
		mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	win = XCreateWindow(dpy, root, 0, 0, width, height,
						0, visinfo->depth, InputOutput,
						visinfo->visual, mask, &attr);
	
	sizehints = XAllocSizeHints();
	if (sizehints) {
		sizehints->min_width = width;
		sizehints->min_height = height;
		sizehints->max_width = width;
		sizehints->max_height = height;
		sizehints->base_width = width;
		sizehints->base_height = vid.height;
		
		sizehints->flags = PMinSize | PMaxSize | PBaseSize;
	}
	
	wmhints = XAllocWMHints();
	if (wmhints) {
		#include "q2icon.xbm"

		Pixmap icon_pixmap, icon_mask;
		unsigned long fg, bg;
		int i;
		
		fg = BlackPixel(dpy, visinfo->screen);
		bg = WhitePixel(dpy, visinfo->screen);
		icon_pixmap = XCreatePixmapFromBitmapData(dpy, win, (char *)q2icon_bits, q2icon_width, q2icon_height, fg, bg, visinfo->depth);
		for (i = 0; i < sizeof(q2icon_bits); i++)
			q2icon_bits[i] = ~q2icon_bits[i];
		icon_mask = XCreatePixmapFromBitmapData(dpy, win, (char *)q2icon_bits, q2icon_width, q2icon_height, bg, fg, visinfo->depth); 
	
		wmhints->flags = IconPixmapHint|IconMaskHint;
		wmhints->icon_pixmap = icon_pixmap;
		wmhints->icon_mask = icon_mask;
	}

	XSetWMProperties(dpy, win, NULL, NULL, NULL, 0,
			sizehints, wmhints, None);
	if (sizehints)
		XFree(sizehints);
	if (wmhints)
		XFree(wmhints);
	
	XStoreName(dpy, win, "Quake II");
	
	wmDeleteWindow = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, win, &wmDeleteWindow, 1);
	
	XMapWindow(dpy, win);

	if (vidmode_active) {
		XMoveWindow(dpy, win, 0, 0);
		XRaiseWindow(dpy, win);
		XWarpPointer(dpy, None, win, 0, 0, 0, 0, 0, 0);
		XFlush(dpy);
		// Move the viewport to top left
		XF86VidModeSetViewPort(dpy, scrnum, 0, 0);
	}

	XFlush(dpy);

	ctx = qglXCreateContext(dpy, visinfo, NULL, True);

	qglXMakeCurrent(dpy, win, ctx);

	*pwidth = width;
	*pheight = height;

	// let the sound and input subsystems know about the new window
	ri.Vid_NewWindow (width, height);

	qglXMakeCurrent(dpy, win, ctx);

	return rserr_ok;
}

void Fake_glColorTableEXT( GLenum target, GLenum internalformat,
                             GLsizei width, GLenum format, GLenum type,
                             const GLvoid *table )
{
	byte temptable[256][4];
	byte *intbl;
	int i;

	for (intbl = (byte *)table, i = 0; i < 256; i++) {
		temptable[i][2] = *intbl++;
		temptable[i][1] = *intbl++;
		temptable[i][0] = *intbl++;
		temptable[i][3] = 255;
	}
	qgl3DfxSetPaletteEXT((GLuint *)temptable);
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
	dgamouse = false;

	if (dpy) {
		if (ctx)
			qglXDestroyContext(dpy, ctx);
		if (win)
			XDestroyWindow(dpy, win);
		if (gl_state.hwgamma) {
			XF86VidModeSetGamma(dpy, scrnum, &oldgamma);
			/* The gamma has changed, but SetMode will change it
			   anyway, so why bother?
			vid_gamma->modified = true; */
		}
		if (vidmode_active)
			XF86VidModeSwitchToMode(dpy, scrnum, vidmodes[0]);
		XUngrabKeyboard(dpy, CurrentTime);
		XCloseDisplay(dpy);
	}
	ctx = NULL;
	dpy = NULL;
	win = 0;
	ctx = NULL;
/*	
	qglXChooseVisual             = NULL;
	qglXCreateContext            = NULL;
	qglXDestroyContext           = NULL;
	qglXMakeCurrent              = NULL;
	qglXCopyContext              = NULL;
	qglXSwapBuffers              = NULL;
*/	
}
#endif

#ifdef Joystick
void PlatformJoyCommands(int *axis_vals, int *axis_map) {
  struct js_event e;
  int key_index;
  in_state_t *in_state = getState();
  
  while (read(joy_fd, &e, sizeof(struct js_event))!=-1) {
    if (JS_EVENT_BUTTON & e.type) {
      key_index = (e.number < 4) ? K_JOY1 : K_AUX1;
      if (e.value) {
	in_state->Key_Event_fp (key_index + e.number, true);
      }
      else {
	in_state->Key_Event_fp (key_index + e.number, false);
      }
    }
    else if (JS_EVENT_AXIS & e.type) {
      axis_vals[axis_map[e.number]] = e.value;
    }
  }
}
#endif
