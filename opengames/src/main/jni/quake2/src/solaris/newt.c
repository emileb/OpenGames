/*
 * Brian Schmidt
 *
 * File:     newt.c
 * Date:     03/18/99
 * Revision: 1.9
 *
 * This file contains the routines necessary to send Quake video to
 * a NewT.
 */
#pragma ident "@(#)File: newt.c, Rev: 1.9, Date: 99/03/18 13:12:08, Author: Brian Schmidt"

/*============================== INCLUDE FILES ==============================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <newtvideo.h>
#include <nvsession.h>
#include <macros.h>
#include <signal.h>
#include <thread.h>
#include <sched.h>
#include "newt.h"

/*================================ CONSTANTS ================================*/

/*================================= MACROS ==================================*/

/*============================ EXPORTED GLOBALS =============================*/
int yuv_dpcm = FALSE;			     /* use 4-bit DPCM on luma vals */
int yuv_subx = DEF_YUV_XSUBSAMP;	     /* horizontal chroma subsample */
int yuv_suby = DEF_YUV_YSUBSAMP;	     /* vertical chroma subsample */
int req_max_bw = TRUE;			     /* request max BW to NewT */
int newt_initialize_next = 1;

/*============================= STATIC GLOBALS ==============================*/
static ndga_t *ndga = NULL;		     /* NewT DGA interface */
static char *target = NULL;		     /* NewT target */
static nvSession nvsession = NULL;	     /* NewT session */
static nv_t newtnv = NULL;		     /* NewT video conn */
static u_char *line = NULL;		     /* line of pixels */
static struct {				     /* video state info */
    int x, y;				     /* location */
    int dw, dh;				     /* destination width/height */
    int sw, sh;				     /* source width/height */
    int clipEvent;			     /* clip event type */
    NRegion clip;			     /* clip info */
} vinfo;
static u_char ymap[256];		     /* color maps */
static u_char umap[256];
static u_char vmap[256];

/*============================= PREDECLARATIONS =============================*/
static void setup_video(Display *disp, Window win);
static void nvSessionCheck(nvSession sess);

extern char display_name[];


/*============================ EXPORTED ROUTINES ============================*/

void newt_set_palette( unsigned char *palette )
{
  int i, j;
  int y, u, v;
  int r, g, b;

  for (i = j = 0 ; i < 256 ; i++) {
    /* get the RGB pixel values */
    b = palette[i*4 + 2];
    g = palette[i*4 + 1];
    r = palette[i*4 + 0];

    /* convert to YUV */
    y = (77*r + 150*g + 29*b)/256;
    u = (-44*r - 87*g + 131*b)/256 + 128;
    v = (131*r - 110*g - 21*b)/256 + 128;
	
    /* clamp the values */
    ymap[i] = CLAMP(y);
    umap[i] = CLAMP(u);
    vmap[i] = CLAMP(v);
  }
}


/*========================= newt_process_event =========================
 * Process events to watch for window changes.
 */
void newt_process_event(XEvent *xev)
{
    if (ndga != NULL) ndga_process_event(ndga, xev);
}

/*========================= send_newt_video =========================
 * Given an image consisting of 8-bit color index pixels, convert to
 * YUV format and send to a NewT for scaling and display.  The width
 * and height are for the source image. Return 1 on success,
 * 0 if we could not blast straight to the NewT screen.
 */
int send_newt_video(Display *disp, Window win, 
		    void *data, int sw, int sh)
{
    int x, y, w, h;			     /* window coordinates */
    register int i, j, k, l, m;		     /* loop indexes */
    u_char *frame = (u_char *)data;	     /* 8-bit pix for frame */
    tile_t tile;			     /* video tile */
    u_char *ydata, *udata, *vdata;	     /* yuv data */
    int tx, ty, tw, th;			     /* tile location/size */
    int ret = 1;			     /* return code */
    int is_unobscured;			     /* true if window is unobscured */
    int clipEvent;			     /* clip event type */

    
    /* Set things up on the first time through */
    if( newt_initialize_next ) {
      newt_initialize_next = FALSE;
      if( ndga != NULL ) {
	ndga_destroy( ndga );
	ndga = NULL;
      }
      memset( &vinfo, 0, sizeof( vinfo ) );
      setup_video( disp, win );
    }

    /* make sure we have a conn to the NewT and can
     * draw straight to the screen.
     */
    nvSessionCheck(nvsession);
    if ((target == NULL) ||
	(!ndga_start(ndga, &x, &y, &w, &h, &is_unobscured, &clipEvent))) {
	return 0;
    }
    if ((newtnv == NULL) ||
	(vinfo.x != x) || (vinfo.y != y) ||
	(vinfo.dw != w) || (vinfo.dh != h) ||
	(vinfo.sw != sw) || (vinfo.sh != sh)) {

	vinfo.x = x;
	vinfo.y = y;
	vinfo.sw = sw;
	vinfo.sh = sh;
	vinfo.dw = w;
	vinfo.dh = h;

	if (line != NULL) free(line);
	if ((line = (u_char *)malloc(sw)) == NULL) {
	    fprintf(stderr, "send_newt_video: malloc faild\n");
	    ret = 0;
	    goto send_done;
	}
	
	if (newtnv != NULL) nv_destroy(newtnv);
	if ((newtnv = nv_new(target, x, y, w, h, yuv_dpcm)) == NULL) {
	    ret = 0;
	    goto send_done;
	}
    }
    
    /* set up the clip region if necessary */
    if (clipEvent != vinfo.clipEvent) {
	vinfo.clip = ndga_clip(ndga, vinfo.clip);
	nv_set_clip(newtnv, vinfo.clip);
    }

    /* Set up the video tiles */
    if (nv_setup(newtnv, sw, sh, yuv_subx, yuv_suby) == 0) {
	ret = 0;
	goto send_done;
    }
    if (req_max_bw) nv_bwreq(newtnv, sw * sh * 3 * 40);
    
    /* Convert 8-bit colormap data to yuv data.  Copy or compress
     * data into each tile
     */
    tile = nv_next_tile(newtnv, TRUE,
			&ydata, &udata, &vdata, &tx, &ty, &tw, &th);
    while (tile != NULL) {
	/* luma data */
	for (j = 0 ; j < th ; j++) {
	    if (yuv_dpcm) {
		int ywidth = (tw + 1) / 2;
		
		for (i = 0 ; i < tw ; i++) {
		    line[i] = ymap[frame[(ty + j)*sw + tx + i]];
		}
		nv_compress(ydata, line, ywidth);
		ydata += ywidth;
	    } else {
		for (i = 0 ; i < tw ; i++) {
		    ydata[j*tw + i] = 
			ymap[frame[(ty + j)*sw + tx + i]];
		}
	    }
	}
	
	/* chroma data */
	if ((yuv_subx > 0) && (yuv_suby > 0)) {
	    int uvx = tx/yuv_subx, uvy = ty/yuv_suby,
		uvw = (tw + yuv_subx - 1)/yuv_subx,
		uvh = (th + yuv_suby - 1)/yuv_suby;
	
	    for (i = j = 0 ; j < uvh ; j++) {
		for (k = 0 ; k < uvw ; k++) {
		    u_int u = 0, v = 0;
		    
		    for (l = 0 ; l < yuv_suby ; l++) {
			for (m = 0 ; m < yuv_subx ; m++) {
			    u_int pel = frame[(ty + j*yuv_suby + l)*sw +
					     tx + k*yuv_subx + m];
			    
			    u += umap[pel];
			    v += vmap[pel];
			}
		    }
		    
		    udata[i] = u / (yuv_subx * yuv_suby);
		    vdata[i] = v / (yuv_subx * yuv_suby);
		    i++;
		}
	    }
	}
	
	/* ship it off */
	nv_send_tile(newtnv, tile);
	
	/* get next tile */
	tile = nv_next_tile(newtnv, FALSE,
			    &ydata, &udata, &vdata, &tx, &ty, &tw, &th);
    }

send_done:				     /* leave */
    ndga_done(ndga);
    return ret;
}

/*============================= LOCAL ROUTINES ==============================*/

/*========================= setup_video =========================
 * Setup a connection to a NewT so we can send video.
 */
static void setup_video(Display *disp, Window win)
{
    char *session;
    
    printf("Using NewT video: %d-bit luma, chroma subsamp W/%d H/%d\n",
	   yuv_dpcm ? 4 : 8, yuv_subx, yuv_suby);
    
    /* Get the NewT target */
    session = nvGetSessionId( display_name );
    if ((session != NULL) && ((nvsession = nvSessionSetup(session)) != NULL)) {
	struct sigaction sa;
	
	target = NULL;
	sa.sa_handler = exit;
	if ((sigaction(SIGINT, &sa, NULL) < 0) ||
	    (sigaction(SIGHUP, &sa, NULL) < 0) ||
	    (sigaction(SIGTERM, &sa, NULL) < 0)) {
	    perror("sigaction");
	    newt_cleanup();
	    exit(1);
	}
	/*	atexit(newt_cleanup);*/
    } else {
	nvsession = NULL;
	target = getenv("NEWT_TARGET");
    }

    if ((nvsession == NULL) && (target == NULL)) {
	Sys_Error("Could not open connection to NewT");
    }

    /* Create NewT DGA context */
    ndga = ndga_new(disp, win, nvsession, 1, 1);
}

/*========================= newt_cleanup =========================
 * Clean up on exit.  Kill the NewT video session.
 */
void newt_cleanup( void )
{
  if( ndga != NULL ) {
    ndga_destroy( ndga );
  }

  if( newtnv != NULL ) {
    nv_destroy(newtnv);
    newtnv = NULL;
  }
}

/*========================= nvSessionCheck =========================
 * Check the status of the given NewT session and set the target.
 */
static void nvSessionCheck(nvSession sess)
{
    char *new_target;
    register int i;
    
    if (sess == NULL) return;
    
    new_target = nvSessionTarget(sess);

    /* Status has changed; break everything down and start again */
    if ((new_target==NULL) || (target==NULL) || strcmp(new_target,target)) {
	if (newtnv != NULL) {
	    /* Lost the connection. Destroy it */
	    nv_destroy(newtnv);
	    newtnv = NULL;
	}
	
	/* Set new target */
	if (target) {
	    free(target);
	    target = NULL;
	}
	if (new_target) target = strdup(new_target);
    }
}
