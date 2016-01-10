/*
Copyright (C) 2003  T. Joseph Carter

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

#include <stdio.h>

#include "quakedef.h"
#include "image.h"
#include "dpsoftrast.h"


// Tell startup code that we have a client
int cl_available = true;

qboolean vid_supportrefreshrate = false;

static qboolean vid_usingmouse = false;
static qboolean vid_usingmouse_relativeworks = false; // SDL2 workaround for unimplemented RelativeMouse mode
static qboolean vid_usinghidecursor = false;
static qboolean vid_hasfocus = false;
static qboolean vid_isfullscreen;

static int win_half_width = 50;
static int win_half_height = 50;
static int video_bpp;




#ifdef __IPHONEOS__
qboolean VID_ShowingKeyboard(void)
{
	return SDL_iPhoneKeyboardIsShown(window);
}
#endif

void VID_SetMouse(qboolean fullscreengrab, qboolean relative, qboolean hidecursor)
{

}

// multitouch[10][] represents the mouse pointer
// X and Y coordinates are 0-32767 as per SDL spec
#define MAXFINGERS 11
int multitouch[MAXFINGERS][3];

static qboolean VID_TouchscreenArea(int corner, float px, float py, float pwidth, float pheight, const char *icon, float *resultmove, qboolean *resultbutton, keynum_t key)
{

}

void VID_BuildJoyState(vid_joystate_t *joystate)
{
	VID_Shared_BuildJoyState_Begin(joystate);


	VID_Shared_BuildJoyState_Finish(joystate);
}




/////////////////////
// Message Handling
////


/////////////////
// Video system
////

#ifdef USE_GLES2
#ifndef qglClear
#include <GLES2/gl2.h>
#endif

//#define PRECALL //Con_Printf("GLCALL %s:%i\n", __FILE__, __LINE__)
#define PRECALL
#define POSTCALL
GLboolean wrapglIsBuffer(GLuint buffer) {PRECALL;return glIsBuffer(buffer);POSTCALL;}
GLboolean wrapglIsEnabled(GLenum cap) {PRECALL;return glIsEnabled(cap);POSTCALL;}
GLboolean wrapglIsFramebuffer(GLuint framebuffer) {PRECALL;return glIsFramebuffer(framebuffer);POSTCALL;}
//GLboolean wrapglIsQuery(GLuint qid) {PRECALL;return glIsQuery(qid);POSTCALL;}
GLboolean wrapglIsRenderbuffer(GLuint renderbuffer) {PRECALL;return glIsRenderbuffer(renderbuffer);POSTCALL;}
//GLboolean wrapglUnmapBuffer(GLenum target) {PRECALL;return glUnmapBuffer(target);POSTCALL;}
GLenum wrapglCheckFramebufferStatus(GLenum target) {PRECALL;return glCheckFramebufferStatus(target);POSTCALL;}
GLenum wrapglGetError(void) {PRECALL;return glGetError();POSTCALL;}
GLuint wrapglCreateProgram(void) {PRECALL;return glCreateProgram();POSTCALL;}
GLuint wrapglCreateShader(GLenum shaderType) {PRECALL;return glCreateShader(shaderType);POSTCALL;}
//GLuint wrapglGetHandle(GLenum pname) {PRECALL;return glGetHandle(pname);POSTCALL;}
GLint wrapglGetAttribLocation(GLuint programObj, const GLchar *name) {PRECALL;return glGetAttribLocation(programObj, name);POSTCALL;}
GLint wrapglGetUniformLocation(GLuint programObj, const GLchar *name) {PRECALL;return glGetUniformLocation(programObj, name);POSTCALL;}
//GLvoid* wrapglMapBuffer(GLenum target, GLenum access) {PRECALL;return glMapBuffer(target, access);POSTCALL;}
const GLubyte* wrapglGetString(GLenum name) {PRECALL;return (const GLubyte*)glGetString(name);POSTCALL;}
void wrapglActiveStencilFace(GLenum e) {PRECALL;Con_Printf("glActiveStencilFace(e)\n");POSTCALL;}
void wrapglActiveTexture(GLenum e) {PRECALL;glActiveTexture(e);POSTCALL;}
void wrapglAlphaFunc(GLenum func, GLclampf ref) {PRECALL;Con_Printf("glAlphaFunc(func, ref)\n");POSTCALL;}
void wrapglArrayElement(GLint i) {PRECALL;Con_Printf("glArrayElement(i)\n");POSTCALL;}
void wrapglAttachShader(GLuint containerObj, GLuint obj) {PRECALL;glAttachShader(containerObj, obj);POSTCALL;}
//void wrapglBegin(GLenum mode) {PRECALL;Con_Printf("glBegin(mode)\n");POSTCALL;}
//void wrapglBeginQuery(GLenum target, GLuint qid) {PRECALL;glBeginQuery(target, qid);POSTCALL;}
void wrapglBindAttribLocation(GLuint programObj, GLuint index, const GLchar *name) {PRECALL;glBindAttribLocation(programObj, index, name);POSTCALL;}
//void wrapglBindFragDataLocation(GLuint programObj, GLuint index, const GLchar *name) {PRECALL;glBindFragDataLocation(programObj, index, name);POSTCALL;}
void wrapglBindBuffer(GLenum target, GLuint buffer) {PRECALL;glBindBuffer(target, buffer);POSTCALL;}
void wrapglBindFramebuffer(GLenum target, GLuint framebuffer) {PRECALL;glBindFramebuffer(target, framebuffer);POSTCALL;}
void wrapglBindRenderbuffer(GLenum target, GLuint renderbuffer) {PRECALL;glBindRenderbuffer(target, renderbuffer);POSTCALL;}
void wrapglBindTexture(GLenum target, GLuint texture) {PRECALL;glBindTexture(target, texture);POSTCALL;}
void wrapglBlendEquation(GLenum e) {PRECALL;glBlendEquation(e);POSTCALL;}
void wrapglBlendFunc(GLenum sfactor, GLenum dfactor) {PRECALL;glBlendFunc(sfactor, dfactor);POSTCALL;}
void wrapglBufferData(GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage) {PRECALL;glBufferData(target, size, data, usage);POSTCALL;}
void wrapglBufferSubData(GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data) {PRECALL;glBufferSubData(target, offset, size, data);POSTCALL;}
void wrapglClear(GLbitfield mask) {PRECALL;glClear(mask);POSTCALL;}
void wrapglClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {PRECALL;glClearColor(red, green, blue, alpha);POSTCALL;}
void wrapglClearDepth(GLclampd depth) {PRECALL;/*Con_Printf("glClearDepth(%f)\n", depth);glClearDepthf((float)depth);*/POSTCALL;}
void wrapglClearStencil(GLint s) {PRECALL;glClearStencil(s);POSTCALL;}
void wrapglClientActiveTexture(GLenum target) {PRECALL;Con_Printf("glClientActiveTexture(target)\n");POSTCALL;}
void wrapglColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {PRECALL;Con_Printf("glColor4f(red, green, blue, alpha)\n");POSTCALL;}
void wrapglColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha) {PRECALL;Con_Printf("glColor4ub(red, green, blue, alpha)\n");POSTCALL;}
void wrapglColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) {PRECALL;glColorMask(red, green, blue, alpha);POSTCALL;}
void wrapglColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr) {PRECALL;Con_Printf("glColorPointer(size, type, stride, ptr)\n");POSTCALL;}
void wrapglCompileShader(GLuint shaderObj) {PRECALL;glCompileShader(shaderObj);POSTCALL;}
void wrapglCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border,  GLsizei imageSize, const void *data) {PRECALL;glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);POSTCALL;}
void wrapglCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data) {PRECALL;Con_Printf("glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data)\n");POSTCALL;}
void wrapglCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data) {PRECALL;glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);POSTCALL;}
void wrapglCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data) {PRECALL;Con_Printf("glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data)\n");POSTCALL;}
void wrapglCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) {PRECALL;glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);POSTCALL;}
void wrapglCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) {PRECALL;glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);POSTCALL;}
void wrapglCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) {PRECALL;Con_Printf("glCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height)\n");POSTCALL;}
void wrapglCullFace(GLenum mode) {PRECALL;glCullFace(mode);POSTCALL;}
void wrapglDeleteBuffers(GLsizei n, const GLuint *buffers) {PRECALL;glDeleteBuffers(n, buffers);POSTCALL;}
void wrapglDeleteFramebuffers(GLsizei n, const GLuint *framebuffers) {PRECALL;glDeleteFramebuffers(n, framebuffers);POSTCALL;}
void wrapglDeleteShader(GLuint obj) {PRECALL;glDeleteShader(obj);POSTCALL;}
void wrapglDeleteProgram(GLuint obj) {PRECALL;glDeleteProgram(obj);POSTCALL;}
//void wrapglDeleteQueries(GLsizei n, const GLuint *ids) {PRECALL;glDeleteQueries(n, ids);POSTCALL;}
void wrapglDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers) {PRECALL;glDeleteRenderbuffers(n, renderbuffers);POSTCALL;}
void wrapglDeleteTextures(GLsizei n, const GLuint *textures) {PRECALL;glDeleteTextures(n, textures);POSTCALL;}
void wrapglDepthFunc(GLenum func) {PRECALL;glDepthFunc(func);POSTCALL;}
void wrapglDepthMask(GLboolean flag) {PRECALL;glDepthMask(flag);POSTCALL;}
//void wrapglDepthRange(GLclampd near_val, GLclampd far_val) {PRECALL;glDepthRangef((float)near_val, (float)far_val);POSTCALL;}
void wrapglDepthRangef(GLclampf near_val, GLclampf far_val) {PRECALL;glDepthRangef(near_val, far_val);POSTCALL;}
void wrapglDetachShader(GLuint containerObj, GLuint attachedObj) {PRECALL;glDetachShader(containerObj, attachedObj);POSTCALL;}
void wrapglDisable(GLenum cap) {PRECALL;glDisable(cap);POSTCALL;}
void wrapglDisableClientState(GLenum cap) {PRECALL;Con_Printf("glDisableClientState(cap)\n");POSTCALL;}
void wrapglDisableVertexAttribArray(GLuint index) {PRECALL;glDisableVertexAttribArray(index);POSTCALL;}
void wrapglDrawArrays(GLenum mode, GLint first, GLsizei count) {PRECALL;glDrawArrays(mode, first, count);POSTCALL;}
void wrapglDrawBuffer(GLenum mode) {PRECALL;Con_Printf("glDrawBuffer(mode)\n");POSTCALL;}
void wrapglDrawBuffers(GLsizei n, const GLenum *bufs) {PRECALL;Con_Printf("glDrawBuffers(n, bufs)\n");POSTCALL;}
void wrapglDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) {PRECALL;glDrawElements(mode, count, type, indices);POSTCALL;}
//void wrapglDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices) {PRECALL;glDrawRangeElements(mode, start, end, count, type, indices);POSTCALL;}
//void wrapglDrawRangeElementsEXT(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices) {PRECALL;glDrawRangeElements(mode, start, end, count, type, indices);POSTCALL;}
void wrapglEnable(GLenum cap) {PRECALL;glEnable(cap);POSTCALL;}
void wrapglEnableClientState(GLenum cap) {PRECALL;Con_Printf("glEnableClientState(cap)\n");POSTCALL;}
void wrapglEnableVertexAttribArray(GLuint index) {PRECALL;glEnableVertexAttribArray(index);POSTCALL;}
//void wrapglEnd(void) {PRECALL;Con_Printf("glEnd()\n");POSTCALL;}
//void wrapglEndQuery(GLenum target) {PRECALL;glEndQuery(target);POSTCALL;}
void wrapglFinish(void) {PRECALL;glFinish();POSTCALL;}
void wrapglFlush(void) {PRECALL;glFlush();POSTCALL;}
void wrapglFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {PRECALL;glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);POSTCALL;}
void wrapglFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {PRECALL;glFramebufferTexture2D(target, attachment, textarget, texture, level);POSTCALL;}
void wrapglFramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset) {PRECALL;Con_Printf("glFramebufferTexture3D()\n");POSTCALL;}
void wrapglGenBuffers(GLsizei n, GLuint *buffers) {PRECALL;glGenBuffers(n, buffers);POSTCALL;}
void wrapglGenFramebuffers(GLsizei n, GLuint *framebuffers) {PRECALL;glGenFramebuffers(n, framebuffers);POSTCALL;}
//void wrapglGenQueries(GLsizei n, GLuint *ids) {PRECALL;glGenQueries(n, ids);POSTCALL;}
void wrapglGenRenderbuffers(GLsizei n, GLuint *renderbuffers) {PRECALL;glGenRenderbuffers(n, renderbuffers);POSTCALL;}
void wrapglGenTextures(GLsizei n, GLuint *textures) {PRECALL;glGenTextures(n, textures);POSTCALL;}
void wrapglGenerateMipmap(GLenum target) {PRECALL;glGenerateMipmap(target);POSTCALL;}
void wrapglGetActiveAttrib(GLuint programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLchar *name) {PRECALL;glGetActiveAttrib(programObj, index, maxLength, length, size, type, name);POSTCALL;}
void wrapglGetActiveUniform(GLuint programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLchar *name) {PRECALL;glGetActiveUniform(programObj, index, maxLength, length, size, type, name);POSTCALL;}
void wrapglGetAttachedShaders(GLuint containerObj, GLsizei maxCount, GLsizei *count, GLuint *obj) {PRECALL;glGetAttachedShaders(containerObj, maxCount, count, obj);POSTCALL;}
void wrapglGetBooleanv(GLenum pname, GLboolean *params) {PRECALL;glGetBooleanv(pname, params);POSTCALL;}
void wrapglGetCompressedTexImage(GLenum target, GLint lod, void *img) {PRECALL;Con_Printf("glGetCompressedTexImage(target, lod, img)\n");POSTCALL;}
void wrapglGetDoublev(GLenum pname, GLdouble *params) {PRECALL;Con_Printf("glGetDoublev(pname, params)\n");POSTCALL;}
void wrapglGetFloatv(GLenum pname, GLfloat *params) {PRECALL;glGetFloatv(pname, params);POSTCALL;}
void wrapglGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params) {PRECALL;glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);POSTCALL;}
void wrapglGetShaderInfoLog(GLuint obj, GLsizei maxLength, GLsizei *length, GLchar *infoLog) {PRECALL;glGetShaderInfoLog(obj, maxLength, length, infoLog);POSTCALL;}
void wrapglGetProgramInfoLog(GLuint obj, GLsizei maxLength, GLsizei *length, GLchar *infoLog) {PRECALL;glGetProgramInfoLog(obj, maxLength, length, infoLog);POSTCALL;}
void wrapglGetIntegerv(GLenum pname, GLint *params) {PRECALL;glGetIntegerv(pname, params);POSTCALL;}
void wrapglGetShaderiv(GLuint obj, GLenum pname, GLint *params) {PRECALL;glGetShaderiv(obj, pname, params);POSTCALL;}
void wrapglGetProgramiv(GLuint obj, GLenum pname, GLint *params) {PRECALL;glGetProgramiv(obj, pname, params);POSTCALL;}
//void wrapglGetQueryObjectiv(GLuint qid, GLenum pname, GLint *params) {PRECALL;glGetQueryObjectiv(qid, pname, params);POSTCALL;}
//void wrapglGetQueryObjectuiv(GLuint qid, GLenum pname, GLuint *params) {PRECALL;glGetQueryObjectuiv(qid, pname, params);POSTCALL;}
//void wrapglGetQueryiv(GLenum target, GLenum pname, GLint *params) {PRECALL;glGetQueryiv(target, pname, params);POSTCALL;}
void wrapglGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params) {PRECALL;glGetRenderbufferParameteriv(target, pname, params);POSTCALL;}
void wrapglGetShaderSource(GLuint obj, GLsizei maxLength, GLsizei *length, GLchar *source) {PRECALL;glGetShaderSource(obj, maxLength, length, source);POSTCALL;}
void wrapglGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels) {PRECALL;Con_Printf("glGetTexImage(target, level, format, type, pixels)\n");POSTCALL;}
void wrapglGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params) {PRECALL;Con_Printf("glGetTexLevelParameterfv(target, level, pname, params)\n");POSTCALL;}
void wrapglGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params) {PRECALL;Con_Printf("glGetTexLevelParameteriv(target, level, pname, params)\n");POSTCALL;}
void wrapglGetTexParameterfv(GLenum target, GLenum pname, GLfloat *params) {PRECALL;glGetTexParameterfv(target, pname, params);POSTCALL;}
void wrapglGetTexParameteriv(GLenum target, GLenum pname, GLint *params) {PRECALL;glGetTexParameteriv(target, pname, params);POSTCALL;}
void wrapglGetUniformfv(GLuint programObj, GLint location, GLfloat *params) {PRECALL;glGetUniformfv(programObj, location, params);POSTCALL;}
void wrapglGetUniformiv(GLuint programObj, GLint location, GLint *params) {PRECALL;glGetUniformiv(programObj, location, params);POSTCALL;}
void wrapglHint(GLenum target, GLenum mode) {PRECALL;glHint(target, mode);POSTCALL;}
void wrapglLineWidth(GLfloat width) {PRECALL;glLineWidth(width);POSTCALL;}
void wrapglLinkProgram(GLuint programObj) {PRECALL;glLinkProgram(programObj);POSTCALL;}
void wrapglLoadIdentity(void) {PRECALL;Con_Printf("glLoadIdentity()\n");POSTCALL;}
void wrapglLoadMatrixf(const GLfloat *m) {PRECALL;Con_Printf("glLoadMatrixf(m)\n");POSTCALL;}
void wrapglMatrixMode(GLenum mode) {PRECALL;Con_Printf("glMatrixMode(mode)\n");POSTCALL;}
void wrapglMultiTexCoord1f(GLenum target, GLfloat s) {PRECALL;Con_Printf("glMultiTexCoord1f(target, s)\n");POSTCALL;}
void wrapglMultiTexCoord2f(GLenum target, GLfloat s, GLfloat t) {PRECALL;Con_Printf("glMultiTexCoord2f(target, s, t)\n");POSTCALL;}
void wrapglMultiTexCoord3f(GLenum target, GLfloat s, GLfloat t, GLfloat r) {PRECALL;Con_Printf("glMultiTexCoord3f(target, s, t, r)\n");POSTCALL;}
void wrapglMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) {PRECALL;Con_Printf("glMultiTexCoord4f(target, s, t, r, q)\n");POSTCALL;}
void wrapglNormalPointer(GLenum type, GLsizei stride, const GLvoid *ptr) {PRECALL;Con_Printf("glNormalPointer(type, stride, ptr)\n");POSTCALL;}
void wrapglPixelStorei(GLenum pname, GLint param) {PRECALL;glPixelStorei(pname, param);POSTCALL;}
void wrapglPointSize(GLfloat size) {PRECALL;Con_Printf("glPointSize(size)\n");POSTCALL;}
//void wrapglPolygonMode(GLenum face, GLenum mode) {PRECALL;Con_Printf("glPolygonMode(face, mode)\n");POSTCALL;}
void wrapglPolygonOffset(GLfloat factor, GLfloat units) {PRECALL;glPolygonOffset(factor, units);POSTCALL;}
void wrapglPolygonStipple(const GLubyte *mask) {PRECALL;Con_Printf("glPolygonStipple(mask)\n");POSTCALL;}
void wrapglReadBuffer(GLenum mode) {PRECALL;Con_Printf("glReadBuffer(mode)\n");POSTCALL;}
void wrapglReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) {PRECALL;glReadPixels(x, y, width, height, format, type, pixels);POSTCALL;}
void wrapglRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {PRECALL;glRenderbufferStorage(target, internalformat, width, height);POSTCALL;}
void wrapglScissor(GLint x, GLint y, GLsizei width, GLsizei height) {PRECALL;glScissor(x, y, width, height);POSTCALL;}
void wrapglShaderSource(GLuint shaderObj, GLsizei count, const GLchar **string, const GLint *length) {PRECALL;glShaderSource(shaderObj, count, string, length);POSTCALL;}
void wrapglStencilFunc(GLenum func, GLint ref, GLuint mask) {PRECALL;glStencilFunc(func, ref, mask);POSTCALL;}
void wrapglStencilFuncSeparate(GLenum func1, GLenum func2, GLint ref, GLuint mask) {PRECALL;Con_Printf("glStencilFuncSeparate(func1, func2, ref, mask)\n");POSTCALL;}
void wrapglStencilMask(GLuint mask) {PRECALL;glStencilMask(mask);POSTCALL;}
void wrapglStencilOp(GLenum fail, GLenum zfail, GLenum zpass) {PRECALL;glStencilOp(fail, zfail, zpass);POSTCALL;}
void wrapglStencilOpSeparate(GLenum e1, GLenum e2, GLenum e3, GLenum e4) {PRECALL;Con_Printf("glStencilOpSeparate(e1, e2, e3, e4)\n");POSTCALL;}
void wrapglTexCoord1f(GLfloat s) {PRECALL;Con_Printf("glTexCoord1f(s)\n");POSTCALL;}
void wrapglTexCoord2f(GLfloat s, GLfloat t) {PRECALL;Con_Printf("glTexCoord2f(s, t)\n");POSTCALL;}
void wrapglTexCoord3f(GLfloat s, GLfloat t, GLfloat r) {PRECALL;Con_Printf("glTexCoord3f(s, t, r)\n");POSTCALL;}
void wrapglTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q) {PRECALL;Con_Printf("glTexCoord4f(s, t, r, q)\n");POSTCALL;}
void wrapglTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr) {PRECALL;Con_Printf("glTexCoordPointer(size, type, stride, ptr)\n");POSTCALL;}
void wrapglTexEnvf(GLenum target, GLenum pname, GLfloat param) {PRECALL;Con_Printf("glTexEnvf(target, pname, param)\n");POSTCALL;}
void wrapglTexEnvfv(GLenum target, GLenum pname, const GLfloat *params) {PRECALL;Con_Printf("glTexEnvfv(target, pname, params)\n");POSTCALL;}
void wrapglTexEnvi(GLenum target, GLenum pname, GLint param) {PRECALL;Con_Printf("glTexEnvi(target, pname, param)\n");POSTCALL;}
void wrapglTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) {PRECALL;glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels);POSTCALL;}
void wrapglTexImage3D(GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels) {PRECALL;Con_Printf("glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels)\n");POSTCALL;}
void wrapglTexParameterf(GLenum target, GLenum pname, GLfloat param) {PRECALL;glTexParameterf(target, pname, param);POSTCALL;}
void wrapglTexParameterfv(GLenum target, GLenum pname, GLfloat *params) {PRECALL;glTexParameterfv(target, pname, params);POSTCALL;}
void wrapglTexParameteri(GLenum target, GLenum pname, GLint param) {PRECALL;glTexParameteri(target, pname, param);POSTCALL;}
void wrapglTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) {PRECALL;glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);POSTCALL;}
void wrapglTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels) {PRECALL;Con_Printf("glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels)\n");POSTCALL;}
void wrapglUniform1f(GLint location, GLfloat v0) {PRECALL;glUniform1f(location, v0);POSTCALL;}
void wrapglUniform1fv(GLint location, GLsizei count, const GLfloat *value) {PRECALL;glUniform1fv(location, count, value);POSTCALL;}
void wrapglUniform1i(GLint location, GLint v0) {PRECALL;glUniform1i(location, v0);POSTCALL;}
void wrapglUniform1iv(GLint location, GLsizei count, const GLint *value) {PRECALL;glUniform1iv(location, count, value);POSTCALL;}
void wrapglUniform2f(GLint location, GLfloat v0, GLfloat v1) {PRECALL;glUniform2f(location, v0, v1);POSTCALL;}
void wrapglUniform2fv(GLint location, GLsizei count, const GLfloat *value) {PRECALL;glUniform2fv(location, count, value);POSTCALL;}
void wrapglUniform2i(GLint location, GLint v0, GLint v1) {PRECALL;glUniform2i(location, v0, v1);POSTCALL;}
void wrapglUniform2iv(GLint location, GLsizei count, const GLint *value) {PRECALL;glUniform2iv(location, count, value);POSTCALL;}
void wrapglUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) {PRECALL;glUniform3f(location, v0, v1, v2);POSTCALL;}
void wrapglUniform3fv(GLint location, GLsizei count, const GLfloat *value) {PRECALL;glUniform3fv(location, count, value);POSTCALL;}
void wrapglUniform3i(GLint location, GLint v0, GLint v1, GLint v2) {PRECALL;glUniform3i(location, v0, v1, v2);POSTCALL;}
void wrapglUniform3iv(GLint location, GLsizei count, const GLint *value) {PRECALL;glUniform3iv(location, count, value);POSTCALL;}
void wrapglUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {PRECALL;glUniform4f(location, v0, v1, v2, v3);POSTCALL;}
void wrapglUniform4fv(GLint location, GLsizei count, const GLfloat *value) {PRECALL;glUniform4fv(location, count, value);POSTCALL;}
void wrapglUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) {PRECALL;glUniform4i(location, v0, v1, v2, v3);POSTCALL;}
void wrapglUniform4iv(GLint location, GLsizei count, const GLint *value) {PRECALL;glUniform4iv(location, count, value);POSTCALL;}
void wrapglUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {PRECALL;glUniformMatrix2fv(location, count, transpose, value);POSTCALL;}
void wrapglUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {PRECALL;glUniformMatrix3fv(location, count, transpose, value);POSTCALL;}
void wrapglUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {PRECALL;glUniformMatrix4fv(location, count, transpose, value);POSTCALL;}
void wrapglUseProgram(GLuint programObj) {PRECALL;glUseProgram(programObj);POSTCALL;}
void wrapglValidateProgram(GLuint programObj) {PRECALL;glValidateProgram(programObj);POSTCALL;}
void wrapglVertex2f(GLfloat x, GLfloat y) {PRECALL;Con_Printf("glVertex2f(x, y)\n");POSTCALL;}
void wrapglVertex3f(GLfloat x, GLfloat y, GLfloat z) {PRECALL;Con_Printf("glVertex3f(x, y, z)\n");POSTCALL;}
void wrapglVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {PRECALL;Con_Printf("glVertex4f(x, y, z, w)\n");POSTCALL;}
void wrapglVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) {PRECALL;glVertexAttribPointer(index, size, type, normalized, stride, pointer);POSTCALL;}
void wrapglVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr) {PRECALL;Con_Printf("glVertexPointer(size, type, stride, ptr)\n");POSTCALL;}
void wrapglViewport(GLint x, GLint y, GLsizei width, GLsizei height) {PRECALL;glViewport(x, y, width, height);POSTCALL;}
void wrapglVertexAttrib1f(GLuint index, GLfloat v0) {PRECALL;glVertexAttrib1f(index, v0);POSTCALL;}
//void wrapglVertexAttrib1s(GLuint index, GLshort v0) {PRECALL;glVertexAttrib1s(index, v0);POSTCALL;}
//void wrapglVertexAttrib1d(GLuint index, GLdouble v0) {PRECALL;glVertexAttrib1d(index, v0);POSTCALL;}
void wrapglVertexAttrib2f(GLuint index, GLfloat v0, GLfloat v1) {PRECALL;glVertexAttrib2f(index, v0, v1);POSTCALL;}
//void wrapglVertexAttrib2s(GLuint index, GLshort v0, GLshort v1) {PRECALL;glVertexAttrib2s(index, v0, v1);POSTCALL;}
//void wrapglVertexAttrib2d(GLuint index, GLdouble v0, GLdouble v1) {PRECALL;glVertexAttrib2d(index, v0, v1);POSTCALL;}
void wrapglVertexAttrib3f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2) {PRECALL;glVertexAttrib3f(index, v0, v1, v2);POSTCALL;}
//void wrapglVertexAttrib3s(GLuint index, GLshort v0, GLshort v1, GLshort v2) {PRECALL;glVertexAttrib3s(index, v0, v1, v2);POSTCALL;}
//void wrapglVertexAttrib3d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2) {PRECALL;glVertexAttrib3d(index, v0, v1, v2);POSTCALL;}
void wrapglVertexAttrib4f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {PRECALL;glVertexAttrib4f(index, v0, v1, v2, v3);POSTCALL;}
//void wrapglVertexAttrib4s(GLuint index, GLshort v0, GLshort v1, GLshort v2, GLshort v3) {PRECALL;glVertexAttrib4s(index, v0, v1, v2, v3);POSTCALL;}
//void wrapglVertexAttrib4d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3) {PRECALL;glVertexAttrib4d(index, v0, v1, v2, v3);POSTCALL;}
//void wrapglVertexAttrib4Nub(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w) {PRECALL;glVertexAttrib4Nub(index, x, y, z, w);POSTCALL;}
void wrapglVertexAttrib1fv(GLuint index, const GLfloat *v) {PRECALL;glVertexAttrib1fv(index, v);POSTCALL;}
//void wrapglVertexAttrib1sv(GLuint index, const GLshort *v) {PRECALL;glVertexAttrib1sv(index, v);POSTCALL;}
//void wrapglVertexAttrib1dv(GLuint index, const GLdouble *v) {PRECALL;glVertexAttrib1dv(index, v);POSTCALL;}
void wrapglVertexAttrib2fv(GLuint index, const GLfloat *v) {PRECALL;glVertexAttrib2fv(index, v);POSTCALL;}
//void wrapglVertexAttrib2sv(GLuint index, const GLshort *v) {PRECALL;glVertexAttrib2sv(index, v);POSTCALL;}
//void wrapglVertexAttrib2dv(GLuint index, const GLdouble *v) {PRECALL;glVertexAttrib2dv(index, v);POSTCALL;}
void wrapglVertexAttrib3fv(GLuint index, const GLfloat *v) {PRECALL;glVertexAttrib3fv(index, v);POSTCALL;}
//void wrapglVertexAttrib3sv(GLuint index, const GLshort *v) {PRECALL;glVertexAttrib3sv(index, v);POSTCALL;}
//void wrapglVertexAttrib3dv(GLuint index, const GLdouble *v) {PRECALL;glVertexAttrib3dv(index, v);POSTCALL;}
void wrapglVertexAttrib4fv(GLuint index, const GLfloat *v) {PRECALL;glVertexAttrib4fv(index, v);POSTCALL;}
//void wrapglVertexAttrib4sv(GLuint index, const GLshort *v) {PRECALL;glVertexAttrib4sv(index, v);POSTCALL;}
//void wrapglVertexAttrib4dv(GLuint index, const GLdouble *v) {PRECALL;glVertexAttrib4dv(index, v);POSTCALL;}
//void wrapglVertexAttrib4iv(GLuint index, const GLint *v) {PRECALL;glVertexAttrib4iv(index, v);POSTCALL;}
//void wrapglVertexAttrib4bv(GLuint index, const GLbyte *v) {PRECALL;glVertexAttrib4bv(index, v);POSTCALL;}
//void wrapglVertexAttrib4ubv(GLuint index, const GLubyte *v) {PRECALL;glVertexAttrib4ubv(index, v);POSTCALL;}
//void wrapglVertexAttrib4usv(GLuint index, const GLushort *v) {PRECALL;glVertexAttrib4usv(index, GLushort v);POSTCALL;}
//void wrapglVertexAttrib4uiv(GLuint index, const GLuint *v) {PRECALL;glVertexAttrib4uiv(index, v);POSTCALL;}
//void wrapglVertexAttrib4Nbv(GLuint index, const GLbyte *v) {PRECALL;glVertexAttrib4Nbv(index, v);POSTCALL;}
//void wrapglVertexAttrib4Nsv(GLuint index, const GLshort *v) {PRECALL;glVertexAttrib4Nsv(index, v);POSTCALL;}
//void wrapglVertexAttrib4Niv(GLuint index, const GLint *v) {PRECALL;glVertexAttrib4Niv(index, v);POSTCALL;}
//void wrapglVertexAttrib4Nubv(GLuint index, const GLubyte *v) {PRECALL;glVertexAttrib4Nubv(index, v);POSTCALL;}
//void wrapglVertexAttrib4Nusv(GLuint index, const GLushort *v) {PRECALL;glVertexAttrib4Nusv(index, GLushort v);POSTCALL;}
//void wrapglVertexAttrib4Nuiv(GLuint index, const GLuint *v) {PRECALL;glVertexAttrib4Nuiv(index, v);POSTCALL;}
//void wrapglGetVertexAttribdv(GLuint index, GLenum pname, GLdouble *params) {PRECALL;glGetVertexAttribdv(index, pname, params);POSTCALL;}
void wrapglGetVertexAttribfv(GLuint index, GLenum pname, GLfloat *params) {PRECALL;glGetVertexAttribfv(index, pname, params);POSTCALL;}
void wrapglGetVertexAttribiv(GLuint index, GLenum pname, GLint *params) {PRECALL;glGetVertexAttribiv(index, pname, params);POSTCALL;}
void wrapglGetVertexAttribPointerv(GLuint index, GLenum pname, GLvoid **pointer) {PRECALL;glGetVertexAttribPointerv(index, pname, pointer);POSTCALL;}
#endif


void GLES_Init(void)
{
#ifndef qglClear
	qglIsBufferARB = wrapglIsBuffer;
	qglIsEnabled = wrapglIsEnabled;
	qglIsFramebufferEXT = wrapglIsFramebuffer;
//	qglIsQueryARB = wrapglIsQuery;
	qglIsRenderbufferEXT = wrapglIsRenderbuffer;
//	qglUnmapBufferARB = wrapglUnmapBuffer;
	qglCheckFramebufferStatusEXT = wrapglCheckFramebufferStatus;
	qglGetError = wrapglGetError;
	qglCreateProgram = wrapglCreateProgram;
	qglCreateShader = wrapglCreateShader;
//	qglGetHandleARB = wrapglGetHandle;
	qglGetAttribLocation = wrapglGetAttribLocation;
	qglGetUniformLocation = wrapglGetUniformLocation;
//	qglMapBufferARB = wrapglMapBuffer;
	qglGetString = wrapglGetString;
//	qglActiveStencilFaceEXT = wrapglActiveStencilFace;
	qglActiveTexture = wrapglActiveTexture;
	qglAlphaFunc = wrapglAlphaFunc;
	qglArrayElement = wrapglArrayElement;
	qglAttachShader = wrapglAttachShader;
//	qglBegin = wrapglBegin;
//	qglBeginQueryARB = wrapglBeginQuery;
	qglBindAttribLocation = wrapglBindAttribLocation;
//	qglBindFragDataLocation = wrapglBindFragDataLocation;
	qglBindBufferARB = wrapglBindBuffer;
	qglBindFramebufferEXT = wrapglBindFramebuffer;
	qglBindRenderbufferEXT = wrapglBindRenderbuffer;
	qglBindTexture = wrapglBindTexture;
	qglBlendEquationEXT = wrapglBlendEquation;
	qglBlendFunc = wrapglBlendFunc;
	qglBufferDataARB = wrapglBufferData;
	qglBufferSubDataARB = wrapglBufferSubData;
	qglClear = wrapglClear;
	qglClearColor = wrapglClearColor;
	qglClearDepth = wrapglClearDepth;
	qglClearStencil = wrapglClearStencil;
	qglClientActiveTexture = wrapglClientActiveTexture;
	qglColor4f = wrapglColor4f;
	qglColor4ub = wrapglColor4ub;
	qglColorMask = wrapglColorMask;
	qglColorPointer = wrapglColorPointer;
	qglCompileShader = wrapglCompileShader;
	qglCompressedTexImage2DARB = wrapglCompressedTexImage2D;
	qglCompressedTexImage3DARB = wrapglCompressedTexImage3D;
	qglCompressedTexSubImage2DARB = wrapglCompressedTexSubImage2D;
	qglCompressedTexSubImage3DARB = wrapglCompressedTexSubImage3D;
	qglCopyTexImage2D = wrapglCopyTexImage2D;
	qglCopyTexSubImage2D = wrapglCopyTexSubImage2D;
	qglCopyTexSubImage3D = wrapglCopyTexSubImage3D;
	qglCullFace = wrapglCullFace;
	qglDeleteBuffersARB = wrapglDeleteBuffers;
	qglDeleteFramebuffersEXT = wrapglDeleteFramebuffers;
	qglDeleteProgram = wrapglDeleteProgram;
	qglDeleteShader = wrapglDeleteShader;
//	qglDeleteQueriesARB = wrapglDeleteQueries;
	qglDeleteRenderbuffersEXT = wrapglDeleteRenderbuffers;
	qglDeleteTextures = wrapglDeleteTextures;
	qglDepthFunc = wrapglDepthFunc;
	qglDepthMask = wrapglDepthMask;
	qglDepthRangef = wrapglDepthRangef;
	qglDetachShader = wrapglDetachShader;
	qglDisable = wrapglDisable;
	qglDisableClientState = wrapglDisableClientState;
	qglDisableVertexAttribArray = wrapglDisableVertexAttribArray;
	qglDrawArrays = wrapglDrawArrays;
//	qglDrawBuffer = wrapglDrawBuffer;
//	qglDrawBuffersARB = wrapglDrawBuffers;
	qglDrawElements = wrapglDrawElements;
//	qglDrawRangeElements = wrapglDrawRangeElements;
	qglEnable = wrapglEnable;
	qglEnableClientState = wrapglEnableClientState;
	qglEnableVertexAttribArray = wrapglEnableVertexAttribArray;
//	qglEnd = wrapglEnd;
//	qglEndQueryARB = wrapglEndQuery;
	qglFinish = wrapglFinish;
	qglFlush = wrapglFlush;
	qglFramebufferRenderbufferEXT = wrapglFramebufferRenderbuffer;
	qglFramebufferTexture2DEXT = wrapglFramebufferTexture2D;
	qglFramebufferTexture3DEXT = wrapglFramebufferTexture3D;
	qglGenBuffersARB = wrapglGenBuffers;
	qglGenFramebuffersEXT = wrapglGenFramebuffers;
//	qglGenQueriesARB = wrapglGenQueries;
	qglGenRenderbuffersEXT = wrapglGenRenderbuffers;
	qglGenTextures = wrapglGenTextures;
	qglGenerateMipmapEXT = wrapglGenerateMipmap;
	qglGetActiveAttrib = wrapglGetActiveAttrib;
	qglGetActiveUniform = wrapglGetActiveUniform;
	qglGetAttachedShaders = wrapglGetAttachedShaders;
	qglGetBooleanv = wrapglGetBooleanv;
//	qglGetCompressedTexImageARB = wrapglGetCompressedTexImage;
	qglGetDoublev = wrapglGetDoublev;
	qglGetFloatv = wrapglGetFloatv;
	qglGetFramebufferAttachmentParameterivEXT = wrapglGetFramebufferAttachmentParameteriv;
	qglGetProgramInfoLog = wrapglGetProgramInfoLog;
	qglGetShaderInfoLog = wrapglGetShaderInfoLog;
	qglGetIntegerv = wrapglGetIntegerv;
	qglGetShaderiv = wrapglGetShaderiv;
	qglGetProgramiv = wrapglGetProgramiv;
//	qglGetQueryObjectivARB = wrapglGetQueryObjectiv;
//	qglGetQueryObjectuivARB = wrapglGetQueryObjectuiv;
//	qglGetQueryivARB = wrapglGetQueryiv;
	qglGetRenderbufferParameterivEXT = wrapglGetRenderbufferParameteriv;
	qglGetShaderSource = wrapglGetShaderSource;
	qglGetTexImage = wrapglGetTexImage;
	qglGetTexLevelParameterfv = wrapglGetTexLevelParameterfv;
	qglGetTexLevelParameteriv = wrapglGetTexLevelParameteriv;
	qglGetTexParameterfv = wrapglGetTexParameterfv;
	qglGetTexParameteriv = wrapglGetTexParameteriv;
	qglGetUniformfv = wrapglGetUniformfv;
	qglGetUniformiv = wrapglGetUniformiv;
	qglHint = wrapglHint;
	qglLineWidth = wrapglLineWidth;
	qglLinkProgram = wrapglLinkProgram;
	qglLoadIdentity = wrapglLoadIdentity;
	qglLoadMatrixf = wrapglLoadMatrixf;
	qglMatrixMode = wrapglMatrixMode;
	qglMultiTexCoord1f = wrapglMultiTexCoord1f;
	qglMultiTexCoord2f = wrapglMultiTexCoord2f;
	qglMultiTexCoord3f = wrapglMultiTexCoord3f;
	qglMultiTexCoord4f = wrapglMultiTexCoord4f;
	qglNormalPointer = wrapglNormalPointer;
	qglPixelStorei = wrapglPixelStorei;
	qglPointSize = wrapglPointSize;
//	qglPolygonMode = wrapglPolygonMode;
	qglPolygonOffset = wrapglPolygonOffset;
//	qglPolygonStipple = wrapglPolygonStipple;
	qglReadBuffer = wrapglReadBuffer;
	qglReadPixels = wrapglReadPixels;
	qglRenderbufferStorageEXT = wrapglRenderbufferStorage;
	qglScissor = wrapglScissor;
	qglShaderSource = wrapglShaderSource;
	qglStencilFunc = wrapglStencilFunc;
	qglStencilFuncSeparate = wrapglStencilFuncSeparate;
	qglStencilMask = wrapglStencilMask;
	qglStencilOp = wrapglStencilOp;
	qglStencilOpSeparate = wrapglStencilOpSeparate;
	qglTexCoord1f = wrapglTexCoord1f;
	qglTexCoord2f = wrapglTexCoord2f;
	qglTexCoord3f = wrapglTexCoord3f;
	qglTexCoord4f = wrapglTexCoord4f;
	qglTexCoordPointer = wrapglTexCoordPointer;
	qglTexEnvf = wrapglTexEnvf;
	qglTexEnvfv = wrapglTexEnvfv;
	qglTexEnvi = wrapglTexEnvi;
	qglTexImage2D = wrapglTexImage2D;
	qglTexImage3D = wrapglTexImage3D;
	qglTexParameterf = wrapglTexParameterf;
	qglTexParameterfv = wrapglTexParameterfv;
	qglTexParameteri = wrapglTexParameteri;
	qglTexSubImage2D = wrapglTexSubImage2D;
	qglTexSubImage3D = wrapglTexSubImage3D;
	qglUniform1f = wrapglUniform1f;
	qglUniform1fv = wrapglUniform1fv;
	qglUniform1i = wrapglUniform1i;
	qglUniform1iv = wrapglUniform1iv;
	qglUniform2f = wrapglUniform2f;
	qglUniform2fv = wrapglUniform2fv;
	qglUniform2i = wrapglUniform2i;
	qglUniform2iv = wrapglUniform2iv;
	qglUniform3f = wrapglUniform3f;
	qglUniform3fv = wrapglUniform3fv;
	qglUniform3i = wrapglUniform3i;
	qglUniform3iv = wrapglUniform3iv;
	qglUniform4f = wrapglUniform4f;
	qglUniform4fv = wrapglUniform4fv;
	qglUniform4i = wrapglUniform4i;
	qglUniform4iv = wrapglUniform4iv;
	qglUniformMatrix2fv = wrapglUniformMatrix2fv;
	qglUniformMatrix3fv = wrapglUniformMatrix3fv;
	qglUniformMatrix4fv = wrapglUniformMatrix4fv;
	qglUseProgram = wrapglUseProgram;
	qglValidateProgram = wrapglValidateProgram;
	qglVertex2f = wrapglVertex2f;
	qglVertex3f = wrapglVertex3f;
	qglVertex4f = wrapglVertex4f;
	qglVertexAttribPointer = wrapglVertexAttribPointer;
	qglVertexPointer = wrapglVertexPointer;
	qglViewport = wrapglViewport;
	qglVertexAttrib1f = wrapglVertexAttrib1f;
//	qglVertexAttrib1s = wrapglVertexAttrib1s;
//	qglVertexAttrib1d = wrapglVertexAttrib1d;
	qglVertexAttrib2f = wrapglVertexAttrib2f;
//	qglVertexAttrib2s = wrapglVertexAttrib2s;
//	qglVertexAttrib2d = wrapglVertexAttrib2d;
	qglVertexAttrib3f = wrapglVertexAttrib3f;
//	qglVertexAttrib3s = wrapglVertexAttrib3s;
//	qglVertexAttrib3d = wrapglVertexAttrib3d;
	qglVertexAttrib4f = wrapglVertexAttrib4f;
//	qglVertexAttrib4s = wrapglVertexAttrib4s;
//	qglVertexAttrib4d = wrapglVertexAttrib4d;
//	qglVertexAttrib4Nub = wrapglVertexAttrib4Nub;
	qglVertexAttrib1fv = wrapglVertexAttrib1fv;
//	qglVertexAttrib1sv = wrapglVertexAttrib1sv;
//	qglVertexAttrib1dv = wrapglVertexAttrib1dv;
	qglVertexAttrib2fv = wrapglVertexAttrib2fv;
//	qglVertexAttrib2sv = wrapglVertexAttrib2sv;
//	qglVertexAttrib2dv = wrapglVertexAttrib2dv;
	qglVertexAttrib3fv = wrapglVertexAttrib3fv;
//	qglVertexAttrib3sv = wrapglVertexAttrib3sv;
//	qglVertexAttrib3dv = wrapglVertexAttrib3dv;
	qglVertexAttrib4fv = wrapglVertexAttrib4fv;
//	qglVertexAttrib4sv = wrapglVertexAttrib4sv;
//	qglVertexAttrib4dv = wrapglVertexAttrib4dv;
//	qglVertexAttrib4iv = wrapglVertexAttrib4iv;
//	qglVertexAttrib4bv = wrapglVertexAttrib4bv;
//	qglVertexAttrib4ubv = wrapglVertexAttrib4ubv;
//	qglVertexAttrib4usv = wrapglVertexAttrib4usv;
//	qglVertexAttrib4uiv = wrapglVertexAttrib4uiv;
//	qglVertexAttrib4Nbv = wrapglVertexAttrib4Nbv;
//	qglVertexAttrib4Nsv = wrapglVertexAttrib4Nsv;
//	qglVertexAttrib4Niv = wrapglVertexAttrib4Niv;
//	qglVertexAttrib4Nubv = wrapglVertexAttrib4Nubv;
//	qglVertexAttrib4Nusv = wrapglVertexAttrib4Nusv;
//	qglVertexAttrib4Nuiv = wrapglVertexAttrib4Nuiv;
//	qglGetVertexAttribdv = wrapglGetVertexAttribdv;
	qglGetVertexAttribfv = wrapglGetVertexAttribfv;
	qglGetVertexAttribiv = wrapglGetVertexAttribiv;
	qglGetVertexAttribPointerv = wrapglGetVertexAttribPointerv;
#endif

	gl_renderer = (const char *)qglGetString(GL_RENDERER);
	gl_vendor = (const char *)qglGetString(GL_VENDOR);
	gl_version = (const char *)qglGetString(GL_VERSION);
	gl_extensions = (const char *)qglGetString(GL_EXTENSIONS);
	
	if (!gl_extensions)
		gl_extensions = "";
	if (!gl_platformextensions)
		gl_platformextensions = "";
	
	Con_Printf("GL_VENDOR: %s\n", gl_vendor);
	Con_Printf("GL_RENDERER: %s\n", gl_renderer);
	Con_Printf("GL_VERSION: %s\n", gl_version);
	Con_Printf("GL_EXTENSIONS: %s\n", gl_extensions);
	Con_DPrintf("%s_EXTENSIONS: %s\n", gl_platform, gl_platformextensions);
	
	// LordHavoc: report supported extensions
	Con_DPrintf("\nQuakeC extensions for server and client: %s\nQuakeC extensions for menu: %s\n", vm_sv_extensions, vm_m_extensions );

	// GLES devices in general do not like GL_BGRA, so use GL_RGBA
	vid.forcetextype = TEXTYPE_RGBA;
	//vid.forcetextype = TEXTYPE_BGRA;

	vid.support.gl20shaders = true;
	vid.support.amd_texture_texture4 = false;
	vid.support.arb_depth_texture = false;
	vid.support.arb_draw_buffers = false;
	vid.support.arb_multitexture = false;
	vid.support.arb_occlusion_query = false;
	vid.support.arb_shadow = false;
	vid.support.arb_texture_compression = false; // different (vendor-specific) formats than on desktop OpenGL...
	vid.support.arb_texture_cube_map = true;
	vid.support.arb_texture_env_combine = false;
	vid.support.arb_texture_gather = false;
	vid.support.arb_texture_non_power_of_two = false; //strstr(gl_extensions, "GL_OES_texture_npot") != NULL;
	vid.support.arb_vertex_buffer_object = true;
	vid.support.ati_separate_stencil = false;
	vid.support.ext_blend_minmax = false;
	vid.support.ext_blend_subtract = true;
	vid.support.ext_draw_range_elements = true;
	vid.support.ext_framebuffer_object = false;//true;

	vid.support.arb_framebuffer_object = false;
	vid.support.ext_framebuffer_object = false;

	vid.support.ext_packed_depth_stencil = false;
	vid.support.ext_stencil_two_side = false;
	vid.support.ext_texture_3d = false; //EMILE
	vid.support.ext_texture_compression_s3tc = true; //EMILE
	vid.support.ext_texture_edge_clamp = true;
	vid.support.ext_texture_filter_anisotropic = false; // probably don't want to use it...
	vid.support.ext_texture_srgb = false;

	qglGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&vid.maxtexturesize_2d);
	if (vid.support.ext_texture_filter_anisotropic)
		qglGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLint*)&vid.max_anisotropy);
	if (vid.support.arb_texture_cube_map)
		qglGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, (GLint*)&vid.maxtexturesize_cubemap);
#ifdef GL_MAX_3D_TEXTURE_SIZE
	if (vid.support.ext_texture_3d)
		qglGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, (GLint*)&vid.maxtexturesize_3d);
#endif
	Con_Printf("GL_MAX_CUBE_MAP_TEXTURE_SIZE = %i\n", vid.maxtexturesize_cubemap);
	Con_Printf("GL_MAX_3D_TEXTURE_SIZE = %i\n", vid.maxtexturesize_3d);
	{
#define GL_ALPHA_BITS                           0x0D55
#define GL_RED_BITS                             0x0D52
#define GL_GREEN_BITS                           0x0D53
#define GL_BLUE_BITS                            0x0D54
#define GL_DEPTH_BITS                           0x0D56
#define GL_STENCIL_BITS                         0x0D57
		int fb_r = -1, fb_g = -1, fb_b = -1, fb_a = -1, fb_d = -1, fb_s = -1;
		qglGetIntegerv(GL_RED_BITS    , &fb_r);
		qglGetIntegerv(GL_GREEN_BITS  , &fb_g);
		qglGetIntegerv(GL_BLUE_BITS   , &fb_b);
		qglGetIntegerv(GL_ALPHA_BITS  , &fb_a);
		qglGetIntegerv(GL_DEPTH_BITS  , &fb_d);
		qglGetIntegerv(GL_STENCIL_BITS, &fb_s);
		Con_Printf("Framebuffer depth is R%iG%iB%iA%iD%iS%i\n", fb_r, fb_g, fb_b, fb_a, fb_d, fb_s);
	}

	// verify that cubemap textures are really supported
	if (vid.support.arb_texture_cube_map && vid.maxtexturesize_cubemap < 256)
		vid.support.arb_texture_cube_map = false;
	
	// verify that 3d textures are really supported
	if (vid.support.ext_texture_3d && vid.maxtexturesize_3d < 32)
	{
		vid.support.ext_texture_3d = false;
		Con_Printf("GL_OES_texture_3d reported bogus GL_MAX_3D_TEXTURE_SIZE, disabled\n");
	}

	vid.texunits = 4;
	vid.teximageunits = 8;
	vid.texarrayunits = 5;
	vid.texunits = bound(1, vid.texunits, MAX_TEXTUREUNITS);
	vid.teximageunits = bound(1, vid.teximageunits, MAX_TEXTUREUNITS);
	vid.texarrayunits = bound(1, vid.texarrayunits, MAX_TEXTUREUNITS);
	Con_DPrintf("Using GLES2.0 rendering path - %i texture matrix, %i texture images, %i texcoords%s\n", vid.texunits, vid.teximageunits, vid.texarrayunits, vid.support.ext_framebuffer_object ? ", shadowmapping supported" : "");
	vid.renderpath = RENDERPATH_GLES2;
	vid.useinterleavedarrays = false;
	vid.sRGBcapable2D = false;
	vid.sRGBcapable3D = false;

	// VorteX: set other info (maybe place them in VID_InitMode?)
	extern cvar_t gl_info_vendor;
	extern cvar_t gl_info_renderer;
	extern cvar_t gl_info_version;
	extern cvar_t gl_info_platform;
	extern cvar_t gl_info_driver;
	Cvar_SetQuick(&gl_info_vendor, gl_vendor);
	Cvar_SetQuick(&gl_info_renderer, gl_renderer);
	Cvar_SetQuick(&gl_info_version, gl_version);
	Cvar_SetQuick(&gl_info_platform, gl_platform ? gl_platform : "");
	Cvar_SetQuick(&gl_info_driver, gl_driver);
}


void *GL_GetProcAddress(const char *name)
{

}

static qboolean vid_sdl_initjoysticksystem = false;

void VID_Init (void)
{

}

static int vid_sdljoystickindex = -1;
void VID_EnableJoystick(qboolean enable)
{

}



static void VID_OutputVersion(void)
{

}

static qboolean VID_InitModeGL(viddef_mode_t *mode)
{
	int i;


	win_half_width = mode->width>>1;
	win_half_height = mode->height>>1;

	VID_OutputVersion();

	mode->fullscreen = true;


	// Knghtbrd: should do platform-specific extension string function here

	vid_isfullscreen = false;

	vid_isfullscreen = true;
/*

	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
	if (mode->bitsperpixel >= 32)
	{
		SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute (SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute (SDL_GL_STENCIL_SIZE, 8);
	}
	else
	{
		SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 5);
		SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 5);
		SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 16);
	}
	if (mode->stereobuffer)
		SDL_GL_SetAttribute (SDL_GL_STEREO, 1);
	if (mode->samples > 1)
	{
		SDL_GL_SetAttribute (SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute (SDL_GL_MULTISAMPLESAMPLES, mode->samples);
	}

#if SDL_MAJOR_VERSION == 1
	if (vid_vsync.integer)
		SDL_GL_SetAttribute (SDL_GL_SWAP_CONTROL, 1);
	else
		SDL_GL_SetAttribute (SDL_GL_SWAP_CONTROL, 0);
#else
#ifdef USE_GLES2
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute (SDL_GL_RETAINED_BACKING, 1);
#endif
#endif
*/

	extern int android_screen_width;
	extern int android_screen_height;

	mode->width = android_screen_width;
	mode->height = android_screen_height;
	mode->bitsperpixel=32;

	vid.softpixels = NULL;


	gl_platform = "SDL";
	gl_platformextensions = "";

	GLES_Init();

	vid_hidden = false;
	vid_activewindow = false;
	vid_hasfocus = true;
	vid_usingmouse = false;
	vid_usinghidecursor = false;
		
	return true;
}

extern cvar_t gl_info_extensions;
extern cvar_t gl_info_vendor;
extern cvar_t gl_info_renderer;
extern cvar_t gl_info_version;
extern cvar_t gl_info_platform;
extern cvar_t gl_info_driver;

static qboolean VID_InitModeSoft(viddef_mode_t *mode)
{

	return true;
}

qboolean VID_InitMode(viddef_mode_t *mode)
{

		return VID_InitModeGL(mode);
}

void VID_Shutdown (void)
{
	VID_EnableJoystick(false);
	VID_SetMouse(false, false, false);
	VID_RestoreSystemGamma();

	gl_driver[0] = 0;
	gl_extensions = "";
	gl_platform = "";
	gl_platformextensions = "";
}

int VID_SetGamma (unsigned short *ramps, int rampsize)
{

}

int VID_GetGamma (unsigned short *ramps, int rampsize)
{

}

#include "EGL/egl.h"


void VID_Finish (void)
{
#if SDL_MAJOR_VERSION == 1
	Uint8 appstate;

	//react on appstate changes
	appstate = SDL_GetAppState();

	vid_hidden = !(appstate & SDL_APPACTIVE);
	vid_hasfocus = (appstate & SDL_APPINPUTFOCUS) != 0;
#endif
	vid_activewindow = !vid_hidden && vid_hasfocus;

	VID_UpdateGamma(false, 256);

	if (!vid_hidden)
	{
		switch(vid.renderpath)
		{
		case RENDERPATH_GL11:
		case RENDERPATH_GL13:
		case RENDERPATH_GL20:
		case RENDERPATH_GLES1:
		case RENDERPATH_GLES2:
			CHECKGLERROR
			if (r_speeds.integer == 2 || gl_finish.integer)
				GL_Finish();

			break;
		case RENDERPATH_SOFT:
		case RENDERPATH_D3D9:
		case RENDERPATH_D3D10:
		case RENDERPATH_D3D11:
			if (r_speeds.integer == 2 || gl_finish.integer)
				GL_Finish();
			break;
		}
	}

//eglSwapBuffers( eglGetCurrentDisplay(), eglGetCurrentSurface( EGL_DRAW ) );

}

size_t VID_ListModes(vid_mode_t *modes, size_t maxcount)
{
	/*
	size_t k = 0;
#if SDL_MAJOR_VERSION == 1
	SDL_Rect **vidmodes;
	int bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;

	for(vidmodes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE); vidmodes && vidmodes != (SDL_Rect**)(-1) && *vidmodes; ++vidmodes)
	{
		if(k >= maxcount)
			break;
		modes[k].width = (*vidmodes)->w;
		modes[k].height = (*vidmodes)->h;
		modes[k].bpp = bpp;
		modes[k].refreshrate = 60; // no support for refresh rate in SDL
		modes[k].pixelheight_num = 1;
		modes[k].pixelheight_denom = 1; // SDL does not provide this
		++k;
	}
#else
	int modenum;
	int nummodes = SDL_GetNumDisplayModes(0);
	SDL_DisplayMode mode;
	for (modenum = 0;modenum < nummodes;modenum++)
	{
		if (k >= maxcount)
			break;
		if (SDL_GetDisplayMode(0, modenum, &mode))
			continue;
		modes[k].width = mode.w;
		modes[k].height = mode.h;
		modes[k].refreshrate = mode.refresh_rate;
		modes[k].pixelheight_num = 1;
		modes[k].pixelheight_num = 1;
		modes[k].pixelheight_denom = 1; // SDL does not provide this
		k++;
	}
#endif
*/
	return 0;
}
