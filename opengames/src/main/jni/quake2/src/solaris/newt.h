/*
 * Brian Schmidt
 *
 * File:     newt.h
 * Date:     03/16/99
 * Revision: 1.4
 *
 * Header file for NewT Quake routines and variables.
 */
#pragma ident "@(#)File: newt.h, Rev: 1.4, Date: 99/03/16 12:04:16, Author: Brian Schmidt"

#ifndef _NEWT_H_
#define _NEWT_H_

/*============================== INCLUDE FILES ==============================*/
#include "ndga.h"

/*================================ CONSTANTS ================================*/
#define DEF_YUV_XSUBSAMP	(2)	     /* default horiz chroma subsamp */
#define DEF_YUV_YSUBSAMP	(1)	     /* default vert chroma subsamp */
#ifndef TRUE
#define TRUE			(1)	     /* boolean vals */
#endif
#ifndef FALSE
#define FALSE			(0)
#endif
#define CLAMP(c)		((c) < 0 ? 0 : ((c) > 255 ? 255 : (c)))

/*================================= MACROS ==================================*/

/*============================= DATA STRUCTURES =============================*/

/*============================ EXPORTED GLOBALS =============================*/
extern int yuv_dpcm;			     /* use 4-bit DPCM on luma vals */
extern int yuv_subx;			     /* horizontal chroma subsample */
extern int yuv_suby;			     /* vertical chroma subsample */
extern int yuv_scale;			     /* image scale factor */
extern int req_max_bw;			     /* request max BW to NewT */

/*============================= PREDECLARATIONS =============================*/
    
/*============================= LOCAL ROUTINES ==============================*/

/*============================ EXPORTED ROUTINES ============================*/
int send_newt_video(Display *disp, Window win,
		    void *data, int width, int height);
void newt_process_event(XEvent *xev);
void load_newt_cmaps(u_char *rgb_map);
void newt_cleanup( void );

#endif /* _NEWT_H_ */
