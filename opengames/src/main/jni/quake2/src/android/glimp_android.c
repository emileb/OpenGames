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
#include "../ref_gl/gl_local.h"



void drawTouchControls();

void		GLimp_BeginFrame( float camera_separation )
{
}

static int framecount;


void quake2_jni_reset_framecount()
{
	framecount = 0;
}

int quake2_jni_get_framecount()
{
	return framecount;
}

static int overlay = 0;

void quake2_jni_set_overlay( int value )
{
	overlay = value;
}

void		GLimp_EndFrame( void )
{
	int width, height;

	width = quake2_jni_get_width();
	height = quake2_jni_get_height();
	

	qglDisable (GL_DEPTH_TEST);
	qglDisable (GL_CULL_FACE);
	qglEnable  (GL_BLEND);
	qglDisable (GL_ALPHA_TEST);
	qglColor4f (1,1,1,1);
	
	drawTouchControls();


	//qglFinish();
	qglFlush(); // needed
	


	framecount ++;
}

int 		GLimp_Init( void *hinstance, void *hWnd )
{
	return true; // better
}

void		GLimp_Shutdown( void )
{
}

int     	GLimp_SetMode( int *pwidth, int *pheight, int mode, qboolean fullscreen )
{
	extern int quake2_jni_get_width();
	extern int quake2_jni_get_height();

	int width, height;

	width = quake2_jni_get_width();
	height = quake2_jni_get_height();

	ri.Con_Printf (PRINT_ALL, "setting mode %dx%d", width, height );

	// let the sound and input subsystems know about the new window
	ri.Vid_NewWindow (width, height);

	*pwidth = width ;
	*pheight = height ;

	return rserr_ok; //bettter
}

void		GLimp_AppActivate( qboolean active )
{
}

// defined in qgl_android.c
#if 0
void		GLimp_EnableLogging( qboolean enable )
{
}

void		GLimp_LogNewFrame( void )
{
}
#endif

