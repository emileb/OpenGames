/*
** GLW_IMP.C
**
** This file contains ALL Linux specific stuff having to do with the
** OpenGL refresh.
**
*/

#include "../game/g_headers.h"

#include "../game/b_local.h"
#include "../game/q_shared.h"

#include <termios.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/vt.h>
#include <stdarg.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <dlfcn.h>

#include "../renderer/tr_local.h"
#include "../client/client.h"

#ifdef HAVE_GLES
#include "EGL/egl.h"
//#include <SDL/SDL.h>
//#include "../es/eglport.h"
#else
#include "android_glw.h"
#endif
#include "android_local.h"

#ifndef HAVE_GLES
#include <GL/glx.h>
#endif
/*
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>

#ifdef USE_XF86DGA
#include <X11/extensions/xf86dga.h>
#endif
#include <X11/extensions/xf86vmode.h>
#include <X11/extensions/Xrandr.h>
*/

typedef enum {
	RSERR_OK,

	RSERR_INVALID_FULLSCREEN,
	RSERR_INVALID_MODE,

	RSERR_UNKNOWN
} rserr_t;


glwstate_t glw_state;
/*
static Display *dpy = NULL;
static int scrnum;
static Window win = 0;
*/
#ifdef HAVE_GLES


void myglMultiTexCoord2f( GLenum texture, GLfloat s, GLfloat t )
{
	glMultiTexCoord4f(texture, s, t, 0, 1);
}

static EGLDisplay   g_EGLDisplay;
static EGLConfig    g_EGLConfig;
static EGLContext   g_EGLContext;
static NativeWindowType	g_EGLWindow;
static EGLSurface   g_EGLWindowSurface;

#else	// HAVE_GLES
static GLXContext ctx = NULL;
#endif  // HAVE_GLES

static qboolean autorepeaton = qtrue;

#define KEY_MASK (KeyPressMask | KeyReleaseMask)
#define MOUSE_MASK (ButtonPressMask | ButtonReleaseMask | \
		    PointerMotionMask | ButtonMotionMask )
#define X_MASK (KEY_MASK | MOUSE_MASK | VisibilityChangeMask | StructureNotifyMask )

static qboolean        mouse_avail;
static qboolean        mouse_active;
static int   mx, my;

static cvar_t	*in_mouse;
static cvar_t	*in_dgamouse;

static cvar_t	*r_fakeFullscreen;

//Window root;
qboolean dgamouse = qfalse;
qboolean vidmode_ext = qfalse;

static int win_x, win_y;
/*
static bool default_safed = false;
static XF86VidModeModeLine default_vidmode;
static XF86VidModeModeInfo default_vidmodeInfo;
static XF86VidModeModeInfo **vidmodes;
static int default_dotclock_vidmode;
static int num_vidmodes;
static qboolean vidmode_active = qfalse;
static qboolean vidmode_xrandr = qfalse;
static qboolean vidmode_fullscreen = qfalse;
*/

static int mouse_accel_numerator;
static int mouse_accel_denominator;
static int mouse_threshold;

static void		GLW_InitExtensions( void );
int GLW_SetMode( const char *drivername, int mode, qboolean fullscreen );

//
// function declaration
//
void	 QGL_EnableLogging( qboolean enable );
qboolean QGL_Init( const char *dllname );
void     QGL_Shutdown( void );
#ifdef HAVE_GLES
void	 QGL_EnableLogging( qboolean enable )
{
	(void)enable;
}
#endif

/*****************************************************************************/

static qboolean signalcaught = qfalse;

static void signal_handler(int sig)
{
	if (signalcaught) {
		printf("DOUBLE SIGNAL FAULT: Received signal %d, exiting...\n", sig);
		exit(1);
	}

	signalcaught = qtrue;
	printf("Received signal %d, exiting...\n", sig);
	GLimp_Shutdown();
	exit(1);
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
** GLW_StartDriverAndSetMode
*/
static qboolean GLW_StartDriverAndSetMode( const char *drivername,
	int mode, qboolean fullscreen )
{
	rserr_t err;


	err = (rserr_t) GLW_SetMode( drivername, mode, fullscreen );

	switch ( err )
	{
	case RSERR_INVALID_FULLSCREEN:
		ri.Printf( PRINT_ALL, "...WARNING: fullscreen unavailable in this mode\n" );
		return qfalse;
	case RSERR_INVALID_MODE:
		ri.Printf( PRINT_ALL, "...WARNING: could not set the given mode (%d)\n", mode );
		return qfalse;
	default:
		break;
	}
	return qtrue;
}


extern int android_screen_width;
extern int android_screen_height;

/*
** GLW_SetMode
*/
int GLW_SetMode( const char *drivername, int mode, qboolean fullscreen )
{


	//TODO
	glConfig.vidWidth = android_screen_width;
	glConfig.vidHeight = android_screen_height;

	//glConfig.vidWidth = 800;
	//glConfig.vidHeight = 480;

	glConfig.colorBits = 16;
	glConfig.depthBits = 16;
	glConfig.stencilBits = 0;
	glConfig.windowAspect = glConfig.vidWidth / glConfig.vidHeight;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	return RSERR_OK;
}

//--------------------------------------------
static void GLW_InitTextureCompression( void )
{
	qboolean newer_tc, old_tc;

	#ifdef HAVE_GLES
	newer_tc = qfalse;
	old_tc = qfalse;
	glConfig.textureCompression = TC_NONE;
	ri.Printf( PRINT_ALL, "...ignoring texture compression\n" );
	#else
	// Check for available tc methods.
	newer_tc = ( strstr( glConfig.extensions_string, "ARB_texture_compression" )
		&& strstr( glConfig.extensions_string, "EXT_texture_compression_s3tc" )) ? qtrue : qfalse;
	old_tc = ( strstr( glConfig.extensions_string, "GL_S3_s3tc" )) ? qtrue : qfalse;
	if ( old_tc )
	{
		ri.Printf( PRINT_ALL, "...GL_S3_s3tc available\n" );
	}

	if ( newer_tc )
	{
		ri.Printf( PRINT_ALL, "...GL_EXT_texture_compression_s3tc available\n" );
	}

	if ( !r_ext_compressed_textures->value )
	{
		// Compressed textures are off
		glConfig.textureCompression = TC_NONE;
		ri.Printf( PRINT_ALL, "...ignoring texture compression\n" );
	}
	else if ( !old_tc && !newer_tc )
	{
		// Requesting texture compression, but no method found
		glConfig.textureCompression = TC_NONE;
		ri.Printf( PRINT_ALL, "...no supported texture compression method found\n" );
		ri.Printf( PRINT_ALL, ".....ignoring texture compression\n" );
	}
	else
	{
		// some form of supported texture compression is avaiable, so see if the user has a preference
		if ( r_ext_preferred_tc_method->integer == TC_NONE )
		{
			// No preference, so pick the best
			if ( newer_tc )
			{
				ri.Printf( PRINT_ALL, "...no tc preference specified\n" );
				ri.Printf( PRINT_ALL, ".....using GL_EXT_texture_compression_s3tc\n" );
				glConfig.textureCompression = TC_S3TC_DXT;
			}
			else
			{
				ri.Printf( PRINT_ALL, "...no tc preference specified\n" );
				ri.Printf( PRINT_ALL, ".....using GL_S3_s3tc\n" );
				glConfig.textureCompression = TC_S3TC;
			}
		}
		else
		{
			// User has specified a preference, now see if this request can be honored
			if ( old_tc && newer_tc )
			{
				// both are avaiable, so we can use the desired tc method
				if ( r_ext_preferred_tc_method->integer == TC_S3TC )
				{
					ri.Printf( PRINT_ALL, "...using preferred tc method, GL_S3_s3tc\n" );
					glConfig.textureCompression = TC_S3TC;
				}
				else
				{
					ri.Printf( PRINT_ALL, "...using preferred tc method, GL_EXT_texture_compression_s3tc\n" );
					glConfig.textureCompression = TC_S3TC_DXT;
				}
			}
			else
			{
				// Both methods are not available, so this gets trickier
				if ( r_ext_preferred_tc_method->integer == TC_S3TC )
				{
					// Preferring to user older compression
					if ( old_tc )
					{
						ri.Printf( PRINT_ALL, "...using GL_S3_s3tc\n" );
						glConfig.textureCompression = TC_S3TC;
					}
					else
					{
						// Drat, preference can't be honored
						ri.Printf( PRINT_ALL, "...preferred tc method, GL_S3_s3tc not available\n" );
						ri.Printf( PRINT_ALL, ".....falling back to GL_EXT_texture_compression_s3tc\n" );
						glConfig.textureCompression = TC_S3TC_DXT;
					}
				}
				else
				{
					// Preferring to user newer compression
					if ( newer_tc )
					{
						ri.Printf( PRINT_ALL, "...using GL_EXT_texture_compression_s3tc\n" );
						glConfig.textureCompression = TC_S3TC_DXT;
					}
					else
					{
						// Drat, preference can't be honored
						ri.Printf( PRINT_ALL, "...preferred tc method, GL_EXT_texture_compression_s3tc not available\n" );
						ri.Printf( PRINT_ALL, ".....falling back to GL_S3_s3tc\n" );
						glConfig.textureCompression = TC_S3TC;
					}
				}
			}
		}
	}
	#endif
}

/*
** GLW_InitExtensions
*/
cvar_t *r_ATI_NPATCH_available = NULL;
static void GLW_InitExtensions( void )
{
	//return;

	if ( !r_allowExtensions->integer )
	{
		ri.Printf( PRINT_ALL, "*** IGNORING OPENGL EXTENSIONS ***\n" );
		return;
	}

	ri.Printf( PRINT_ALL, "Initializing OpenGL extensions\n" );

	// Select our tc scheme
	GLW_InitTextureCompression();
	#ifdef HAVE_GLES
	glConfig.textureEnvAddAvailable = qtrue;
	ri.Printf( PRINT_ALL, "...using GL_EXT_texture_env_add\n" );
	#else
	// GL_EXT_texture_env_add
	glConfig.textureEnvAddAvailable = qfalse;
	if ( strstr( glConfig.extensions_string, "EXT_texture_env_add" ) )
	{
		if ( r_ext_texture_env_add->integer )
		{
			glConfig.textureEnvAddAvailable = qtrue;
			ri.Printf( PRINT_ALL, "...using GL_EXT_texture_env_add\n" );
		}
		else
		{
		#endif
			glConfig.textureEnvAddAvailable = qfalse;
			ri.Printf( PRINT_ALL, "...ignoring GL_EXT_texture_env_add\n" );
		#ifndef HAVE_GLES
		}
	}
	else
	{
		ri.Printf( PRINT_ALL, "...GL_EXT_texture_env_add not found\n" );
	}
	#endif
	// GL_EXT_texture_filter_anisotropic
	#ifdef HAVE_GLES
	glConfig.textureFilterAnisotropicAvailable = qtrue;
	#else
	glConfig.textureFilterAnisotropicAvailable = qfalse;

	if ( strstr( glConfig.extensions_string, "EXT_texture_filter_anisotropic" ) )
	{
		glConfig.textureFilterAnisotropicAvailable = qtrue;
		ri.Printf( PRINT_ALL, "...GL_EXT_texture_filter_anisotropic available\n" );

		if ( r_ext_texture_filter_anisotropic->integer )
		{
			ri.Printf( PRINT_ALL, "...using GL_EXT_texture_filter_anisotropic\n" );
		}
		else
		{
			ri.Printf( PRINT_ALL, "...ignoring GL_EXT_texture_filter_anisotropic\n" );
		}
		ri.Cvar_Set( "r_ext_texture_filter_anisotropic_avail", "1" );
	}
	else
	{
		ri.Printf( PRINT_ALL, "...GL_EXT_texture_filter_anisotropic not found\n" );
		ri.Cvar_Set( "r_ext_texture_filter_anisotropic_avail", "0" );
	}
	#endif
	// GL_EXT_clamp_to_edge
	#ifdef HAVE_GLES
	glConfig.clampToEdgeAvailable = qtrue;
	ri.Printf( PRINT_ALL, "...Using GL_EXT_texture_edge_clamp\n" );
	#else
	glConfig.clampToEdgeAvailable = qfalse;
	if ( strstr( glConfig.extensions_string, "GL_EXT_texture_edge_clamp" ) )
	{
		glConfig.clampToEdgeAvailable = qtrue;
		ri.Printf( PRINT_ALL, "...Using GL_EXT_texture_edge_clamp\n" );
	}
	#endif

	// WGL_EXT_swap_control
	// LAvaPort - what is this - do we need it?
//	qwglSwapIntervalEXT = ( BOOL (WINAPI *)(int)) qwglGetProcAddress( "wglSwapIntervalEXT" );
//	if ( qwglSwapIntervalEXT )
//	{
//		ri.Printf( PRINT_ALL, "...using WGL_EXT_swap_control\n" );
//		r_swapInterval->modified = qtrue;	// force a set next frame
//	}
//	else
//	{
//		ri.Printf( PRINT_ALL, "...WGL_EXT_swap_control not found\n" );
//	}

	// GL_ARB_multitexture
	qglMultiTexCoord2fARB = NULL;
	qglActiveTextureARB = NULL;
	qglClientActiveTextureARB = NULL;
	#ifdef HAVE_GLES
	glConfig.maxActiveTextures = 2;	///*SEB*/
	qglMultiTexCoord2fARB = myglMultiTexCoord2f;
	qglActiveTextureARB = &glActiveTexture;
	qglClientActiveTextureARB = &glClientActiveTexture;
	#else
	if ( strstr( glConfig.extensions_string, "GL_ARB_multitexture" )  )
	{
		if ( r_ext_multitexture->integer )
		{
			qglMultiTexCoord2fARB = ( PFNGLMULTITEXCOORD2FARBPROC ) dlsym( glw_state.OpenGLLib, "glMultiTexCoord2fARB" );
			qglActiveTextureARB = ( PFNGLACTIVETEXTUREARBPROC ) dlsym( glw_state.OpenGLLib, "glActiveTextureARB" );
			qglClientActiveTextureARB = ( PFNGLCLIENTACTIVETEXTUREARBPROC ) dlsym( glw_state.OpenGLLib, "glClientActiveTextureARB" );

			if ( qglActiveTextureARB )
			{
				qglGetIntegerv( GL_MAX_ACTIVE_TEXTURES_ARB, &glConfig.maxActiveTextures );

				if ( glConfig.maxActiveTextures > 1 )
				{
					ri.Printf( PRINT_ALL, "...using GL_ARB_multitexture\n" );
				}
				else
				{
					qglMultiTexCoord2fARB = NULL;
					qglActiveTextureARB = NULL;
					qglClientActiveTextureARB = NULL;
					ri.Printf( PRINT_ALL, "...not using GL_ARB_multitexture, < 2 texture units\n" );
				}
			}
		}
		else
		{
			ri.Printf( PRINT_ALL, "...ignoring GL_ARB_multitexture\n" );
		}
	}
	else
	{
		ri.Printf( PRINT_ALL, "...GL_ARB_multitexture not found\n" );
	}
	#endif
	// GL_EXT_compiled_vertex_array
	qglLockArraysEXT = NULL;
	qglUnlockArraysEXT = NULL;
	#ifndef HAVE_GLES
	if ( strstr( glConfig.extensions_string, "GL_EXT_compiled_vertex_array" ) )
	{
		if ( r_ext_compiled_vertex_array->integer )
		{
			ri.Printf( PRINT_ALL, "...using GL_EXT_compiled_vertex_array\n" );
			qglLockArraysEXT = ( void ( APIENTRY * )( int, int ) ) dlsym( glw_state.OpenGLLib, "glLockArraysEXT" );
			qglUnlockArraysEXT = ( void ( APIENTRY * )( void ) ) dlsym( glw_state.OpenGLLib, "glUnlockArraysEXT" );
			if (!qglLockArraysEXT || !qglUnlockArraysEXT) {
				ri.Error (ERR_FATAL, "bad getprocaddress");
			}
		}
		else
		{
			ri.Printf( PRINT_ALL, "...ignoring GL_EXT_compiled_vertex_array\n" );
		}
	}
	else
	{
		ri.Printf( PRINT_ALL, "...GL_EXT_compiled_vertex_array not found\n" );
	}
	#endif
	#ifdef HAVE_GLES
	qglPointParameterfEXT = &glPointParameterf;
	qglPointParameterfvEXT = &glPointParameterfv;
	ri.Printf( PRINT_ALL, "...using GL_EXT_point_parameters\n" );
	#else
	qglPointParameterfEXT = NULL;
	qglPointParameterfvEXT = NULL;
	if ( strstr( glConfig.extensions_string, "GL_EXT_point_parameters" ) )
	{
		if ( r_ext_compiled_vertex_array->integer || 1)
		{
			ri.Printf( PRINT_ALL, "...using GL_EXT_point_parameters\n" );
			qglPointParameterfEXT = ( void ( APIENTRY * )( GLenum, GLfloat) ) dlsym( glw_state.OpenGLLib, "glPointParameterfEXT" );
			qglPointParameterfvEXT = ( void ( APIENTRY * )( GLenum, GLfloat *) ) dlsym( glw_state.OpenGLLib, "glPointParameterfvEXT" );
			if (!qglPointParameterfEXT || !qglPointParameterfvEXT)
			{
				ri.Error (ERR_FATAL, "bad getprocaddress");
			}
		}
		else
		{
			ri.Printf( PRINT_ALL, "...ignoring GL_EXT_point_parameters\n" );
		}
	}
	else
	{
		ri.Printf( PRINT_ALL, "...GL_EXT_point_parameters not found\n" );
	}
	#endif
#ifdef _NPATCH
	// GL_ATI_pn_triangles
	qglPNTrianglesiATI = NULL;
	r_ATI_NPATCH_available = ri.Cvar_Get( "r_ATI_NPATCH_available", "0",CVAR_ROM );
/*	if ( strstr( glConfig.extensions_string, "GL_ATI_pn_triangles" ) )
	{
		ri.Cvar_Set( "r_ATI_NPATCH_available", "1" );
		if ( r_ati_pn_triangles->integer )
		{
			ri.Printf( PRINT_ALL, "...using GL_ATI_pn_triangles\n" );
			qglPNTrianglesiATI = ( void ( APIENTRY * )( GLenum, GLint ) ) qwglGetProcAddress( "glPNTrianglesiATI" );
			if (!qglPNTrianglesiATI) {
				ri.Error (ERR_FATAL, "bad getprocaddress");
			}
		}
		else
		{
			ri.Printf( PRINT_ALL, "...ignoring GL_ATI_pn_triangles\n" );
		}
	}
	else
	{
		ri.Printf( PRINT_ALL, "...GL_ATI_pn_triangles not found\n" );
	}
*/
#endif // _NPATCH
}


/*
** GLW_LoadOpenGL
**
** GLimp_win.c internal function that that attempts to load and use
** a specific OpenGL DLL.
*/
static qboolean GLW_LoadOpenGL()
{
	char buffer[1024];
	qboolean fullscreen;

	#ifdef HAVE_GLES
	strcpy( buffer, "libGLES_CM.so" );
	#else
	strcpy( buffer, OPENGL_DRIVER_NAME );
	#endif

	ri.Printf( PRINT_ALL, "...loading %s: ", buffer );

	// load the QGL layer
	if ( QGL_Init( buffer ) )
	{
		#ifdef HAVE_GLES
		fullscreen = qtrue;
		#else
		fullscreen = r_fullscreen->integer;
		#endif

		// create the window and set up the context
		if ( !GLW_StartDriverAndSetMode( buffer, r_mode->integer, fullscreen ) )
		{
			if (r_mode->integer != 3) {
				if ( !GLW_StartDriverAndSetMode( buffer, 3, fullscreen ) ) {
					goto fail;
				}
			} else
				goto fail;
		}

		return qtrue;
	}
	else
	{
		ri.Printf( PRINT_ALL, "failed\n" );
	}
fail:

	QGL_Shutdown();

	return qfalse;
}


bool androidSwapped = true; //If loading, then draw frame does not return, so detect this
/*
** GLimp_EndFrame
*/
void GLimp_EndFrame (void)
{
	  if (!androidSwapped)
	    	eglSwapBuffers( eglGetCurrentDisplay(), eglGetCurrentSurface( EGL_DRAW ) );

	    androidSwapped = false;

	// check logging
	QGL_EnableLogging( r_logFile->integer );
}

static void GLW_StartOpenGL( void )
{
	//
	// load and initialize the specific OpenGL driver
	//
	if ( !GLW_LoadOpenGL() )
	{
		ri.Error( ERR_FATAL, "GLW_StartOpenGL() - could not load OpenGL subsystem\n" );
	}


}

/*
** GLimp_Init
**
** This is the platform specific OpenGL initialization function.  It
** is responsible for loading OpenGL, initializing it, setting
** extensions, creating a window of the appropriate size, doing
** fullscreen manipulations, etc.  Its overall responsibility is
** to make sure that a functional OpenGL subsystem is operating
** when it returns to the ref.
*/
void GLimp_Init( void )
{
	char	buf[1024];
	cvar_t *lastValidRenderer = ri.Cvar_Get( "r_lastValidRenderer", "(uninitialized)", CVAR_ARCHIVE );
	cvar_t	*cv;

	ri.Printf( PRINT_ALL, "Initializing OpenGL subsystem\n" );

	glConfig.deviceSupportsGamma = qfalse;

	//InitSig();



	//r_allowSoftwareGL = ri.Cvar_Get( "r_allowSoftwareGL", "0", CVAR_LATCH );

	// load appropriate DLL and initialize subsystem
	GLW_StartOpenGL();

	// get our config strings
	glConfig.vendor_string = (const char *) qglGetString (GL_VENDOR);
	glConfig.renderer_string = (const char *) qglGetString (GL_RENDERER);
	glConfig.version_string = (const char *) qglGetString (GL_VERSION);
	glConfig.extensions_string = (const char *) qglGetString (GL_EXTENSIONS);

	if (!glConfig.vendor_string || !glConfig.renderer_string || !glConfig.version_string || !glConfig.extensions_string)
	{
		ri.Error( ERR_FATAL, "GLimp_Init() - Invalid GL Driver\n" );
	}

	// OpenGL driver constants
	qglGetIntegerv( GL_MAX_TEXTURE_SIZE, &glConfig.maxTextureSize );
	// stubbed or broken drivers may have reported 0...
	if ( glConfig.maxTextureSize <= 0 )
	{
		glConfig.maxTextureSize = 1024;
	}

	//
	// chipset specific configuration
	//
	strcpy( buf, glConfig.renderer_string );
	strlwr( buf );

	//
	// NOTE: if changing cvars, do it within this block.  This allows them
	// to be overridden when testing driver fixes, etc. but only sets
	// them to their default state when the hardware is first installed/run.
	//

	if ( Q_stricmp( lastValidRenderer->string, glConfig.renderer_string ) )
	{
		//reset to defaults
		ri.Cvar_Set( "r_picmip", "2" );

		if ( strstr( buf, "matrox" )) {
            ri.Cvar_Set( "r_allowExtensions", "0");
		}
		else
		// Savage3D and Savage4 should always have trilinear enabled
		if ( strstr( buf, "savage3d" ) || strstr( buf, "s3 savage4" ) || strstr( buf, "geforce" ))
		{
			ri.Cvar_Set( "r_texturemode", "GL_LINEAR_MIPMAP_LINEAR" );
		}
		else
		{
			ri.Cvar_Set( "r_textureMode", "GL_LINEAR_MIPMAP_NEAREST" );
		}

		if ( strstr( buf, "kyro" ) )
		{
			ri.Cvar_Set( "r_ext_texture_filter_anisotropic", "0");	//KYROs have it avail, but suck at it!
			ri.Cvar_Set( "r_ext_preferred_tc_method", "1");			//(Use DXT1 instead of DXT5 - same quality but much better performance on KYRO)
		}

		GLW_InitExtensions();

		//this must be a really sucky card!
		if ( (glConfig.textureCompression == TC_NONE) || (glConfig.maxActiveTextures < 2)  || (glConfig.maxTextureSize <= 512) )
		{
			ri.Cvar_Set( "r_picmip", "2");
			ri.Cvar_Set( "r_colorbits", "16");
			ri.Cvar_Set( "r_texturebits", "16");
			ri.Cvar_Set( "r_mode", "3");	//force 640
			Cmd_ExecuteString ("exec low.cfg\n");	//get the rest which can be pulled in after init
		}

	}

	ri.Cvar_Set( "r_lastValidRenderer", glConfig.renderer_string );


	//ri.Cvar_Set( "r_picmip", "2");
	//ri.Cvar_Set( "r_colorbits", "16");
	//ri.Cvar_Set( "r_texturebits", "16");
	//ri.Cvar_Set( "r_dlightBacks", "0");
	//ri.Cvar_Set( "r_dynamiclight", "0");

	GLW_InitExtensions();
	//InitSig();
}


/*
** GLimp_SetGamma
**
** This routine should only be called if glConfig.deviceSupportsGamma is TRUE
*/
void GLimp_SetGamma( unsigned char red[256], unsigned char green[256], unsigned char blue[256] )
{
}


/*
** GLimp_Shutdown
**
** This routine does all OS specific shutdown procedures for the OpenGL
** subsystem.
*/
void GLimp_Shutdown( void )
{
//	const char *strings[] = { "soft", "hard" };
	const char *success[] = { "failed", "success" };
	int retVal;

	//*(int *)0 = 0;

	ri.Printf( PRINT_ALL, "Shutting down OpenGL subsystem\n" );

	// shutdown QGL subsystem
	QGL_Shutdown();

	memset( &glConfig, 0, sizeof( glConfig ) );
	memset( &glState, 0, sizeof( glState ) );
}

/*
** GLimp_LogComment
*/
void GLimp_LogComment( char *comment )
{
	if ( glw_state.log_fp ) {
		fprintf( glw_state.log_fp, "%s", comment );
	}
}


/*
===========================================================

SMP acceleration

===========================================================
*/

sem_t	renderCommandsEvent;
sem_t	renderCompletedEvent;
sem_t	renderActiveEvent;

void (*glimpRenderThread)( void );

void* GLimp_RenderThreadWrapper( void *stub ) {
	glimpRenderThread();

#if 0
	// unbind the context before we die
	qglXMakeCurrent(dpy, None, NULL);
#endif
}
/*
=======================
GLimp_SpawnRenderThread
=======================
*/
pthread_t	renderThreadHandle;
qboolean GLimp_SpawnRenderThread( void (*function)( void ) ) {

	sem_init( &renderCommandsEvent, 0, 0 );
	sem_init( &renderCompletedEvent, 0, 0 );
	sem_init( &renderActiveEvent, 0, 0 );

	glimpRenderThread = function;

	if (pthread_create( &renderThreadHandle, NULL,
		GLimp_RenderThreadWrapper, NULL)) {
		return qfalse;
	}

	return qtrue;
}

static	void	*smpData;
static	int		glXErrors;

void *GLimp_RendererSleep( void ) {
	void	*data;

#if 0
	if ( !qglXMakeCurrent(dpy, None, NULL) ) {
		glXErrors++;
	}
#endif

//	ResetEvent( renderActiveEvent );

	// after this, the front end can exit GLimp_FrontEndSleep
	sem_post ( &renderCompletedEvent );

	sem_wait ( &renderCommandsEvent );

#if 0
	if ( !qglXMakeCurrent(dpy, win, ctx) ) {
		glXErrors++;
	}
#endif

//	ResetEvent( renderCompletedEvent );
//	ResetEvent( renderCommandsEvent );

	data = smpData;

	// after this, the main thread can exit GLimp_WakeRenderer
	sem_post ( &renderActiveEvent );

	return data;
}


void GLimp_FrontEndSleep( void ) {
	sem_wait ( &renderCompletedEvent );

#if 0
	if ( !qglXMakeCurrent(dpy, win, ctx) ) {
		glXErrors++;
	}
#endif
}


void GLimp_WakeRenderer( void *data ) {
	smpData = data;

#if 0
	if ( !qglXMakeCurrent(dpy, None, NULL) ) {
		glXErrors++;
	}
#endif

	// after this, the renderer can continue through GLimp_RendererSleep
	sem_post( &renderCommandsEvent );

	sem_wait( &renderActiveEvent );
}



qboolean			inputActive;
qboolean			inputSuspended;

#define KEY_MASK (KeyPressMask | KeyReleaseMask)
#define MOUSE_MASK (ButtonPressMask | ButtonReleaseMask | \
		    PointerMotionMask | ButtonMotionMask )
#define X_MASK (KEY_MASK | MOUSE_MASK | VisibilityChangeMask | StructureNotifyMask )




void Input_Init(void);
void Input_GetState( void );


/*****************************************************************************/
/* KEYBOARD                                                                  */
/*****************************************************************************/

static unsigned int	keyshift[256];		// key to map to if shift held down in console
static qboolean shift_down=qfalse;

static void HandleEvents(void)
{

}

void KBD_Init(void)
{
}

void KBD_Close(void)
{
}

void IN_ActivateMouse( void )
{

}

void IN_DeactivateMouse( void )
{

}

/*****************************************************************************/
/* MOUSE                                                                     */
/*****************************************************************************/

void IN_Init(void)
{
	// mouse variables
    in_mouse = Cvar_Get ("in_mouse", "1", CVAR_ARCHIVE);
    in_dgamouse = Cvar_Get ("in_dgamouse", "1", CVAR_ARCHIVE);

	if (in_mouse->value)
		mouse_avail = qtrue;
	else
		mouse_avail = qfalse;
}

void IN_Shutdown(void)
{
	mouse_avail = qfalse;
}

void IN_MouseMove(void)
{
	/*
	if (!mouse_avail || !dpy || !win)
		return;

#if 0
	if (!dgamouse) {
		Window root, child;
		int root_x, root_y;
		int win_x, win_y;
		unsigned int mask_return;
		int mwx = glConfig.vidWidth/2;
		int mwy = glConfig.vidHeight/2;

		XQueryPointer(dpy, win, &root, &child,
			&root_x, &root_y, &win_x, &win_y, &mask_return);

		mx = win_x - mwx;
		my = win_y - mwy;

		XWarpPointer(dpy,None,win,0,0,0,0, mwx, mwy);
	}
#endif

	if (mx || my)
		Sys_QueEvent( 0, SE_MOUSE, mx, my, 0, NULL );
	mx = my = 0;
	*/
}

void IN_Frame (void)
{
	/*
	if ( cls.keyCatchers || cls.state != CA_ACTIVE ) {
		// temporarily deactivate if not in the game and
		// running on the desktop
		// voodoo always counts as full screen
//		if (Cvar_VariableValue ("r_fullscreen") == 0
//			&& strcmp( Cvar_VariableString("r_glDriver"), _3DFX_DRIVER_NAME ) )	{
//			IN_DeactivateMouse ();
//			return;
//		}
		if (dpy && !autorepeaton) {
			XAutoRepeatOn(dpy);
			autorepeaton = qtrue;
		}
	} else if (dpy && autorepeaton) {
		XAutoRepeatOff(dpy);
		autorepeaton = qfalse;
	}
*/
	IN_ActivateMouse();

	// post events to the system que
	IN_MouseMove();
}

void IN_Activate(void)
{
}

void Sys_SendKeyEvents (void)
{
	/*
	XEvent event;

	if (!dpy)
		return;

	HandleEvents();
//	while (XCheckMaskEvent(dpy,KEY_MASK|MOUSE_MASK,&event))
//		HandleEvent(&event);
 * */
}

