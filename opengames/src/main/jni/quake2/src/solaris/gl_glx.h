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

#ifndef __GL_GLX_SOLARIS_H__
#define __GL_GLX_SOLARIS_H__

//GLX Functions
XVisualInfo * (APIENTRY * qglXChooseVisual)( Display *dpy, int screen, int *attribList );
GLXContext (APIENTRY * qglXCreateContext)( Display *dpy, XVisualInfo *vis, GLXContext shareList, Bool direct );
void (APIENTRY * qglXDestroyContext)( Display *dpy, GLXContext ctx );
Bool (APIENTRY * qglXMakeCurrent)( Display *dpy, GLXDrawable drawable, GLXContext ctx);
void (APIENTRY * qglXCopyContext)( Display *dpy, GLXContext src, GLXContext dst, GLuint mask );
void (APIENTRY * qglXSwapBuffers)( Display *dpy, GLXDrawable drawable );
int (APIENTRY * qglXGetConfig) (Display *dpy, XVisualInfo *vis, int attrib, int *value);

#endif
