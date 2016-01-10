#include <SDL.h>
#include "qcommon/qcommon.h"
#include "../rd-gles/tr_local.h"
//#include "sdl_qgl.h"
#include "sys/sys_local.h"

#include "EGL/egl.h"

static float displayAspect;
cvar_t *r_allowSoftwareGL; // Don't abort out if a hardware visual can't be obtained
cvar_t *r_allowResize; // make window resizable
cvar_t *r_sdlDriver;

typedef enum
{
	RSERR_OK,

	RSERR_INVALID_FULLSCREEN,
	RSERR_INVALID_MODE,

	RSERR_UNKNOWN
} rserr_t;

static SDL_Window *screen = NULL;
static SDL_GLContext opengl_context;

bool g_bTextureRectangleHack = false;


void ( APIENTRY * qglMultiTexCoord2fARB )( GLenum texture, GLfloat s, GLfloat t );
void ( APIENTRY * qglActiveTextureARB )( GLenum texture );
void ( APIENTRY * qglClientActiveTextureARB )( GLenum texture );

void ( APIENTRY * qglPointParameterfEXT)( GLenum, GLfloat);
void ( APIENTRY * qglPointParameterfvEXT)( GLenum, GLfloat const *);

void (* qglLockArraysEXT) (GLint first, GLsizei count);
void (* qglUnlockArraysEXT) (void);

void myglMultiTexCoord2f( GLenum texture, GLfloat s, GLfloat t )
{
	glMultiTexCoord4f(texture, s, t, 0, 1);
}

/*
 *
void (APIENTRYP qglActiveTextureARB) (GLenum texture);
void (APIENTRYP qglClientActiveTextureARB) (GLenum texture);
void (APIENTRYP qglMultiTexCoord2fARB) (GLenum target, GLfloat s, GLfloat t);

void (APIENTRYP qglCombinerParameterfvNV) (GLenum pname,const GLfloat *params);
void (APIENTRYP qglCombinerParameterivNV) (GLenum pname,const GLint *params);
void (APIENTRYP qglCombinerParameterfNV) (GLenum pname,GLfloat param);
void (APIENTRYP qglCombinerParameteriNV) (GLenum pname,GLint param);
void (APIENTRYP qglCombinerInputNV) (GLenum stage,GLenum portion,GLenum variable,GLenum input,GLenum mapping,
		GLenum componentUsage);
void (APIENTRYP qglCombinerOutputNV) (GLenum stage,GLenum portion,GLenum abOutput,GLenum cdOutput,GLenum sumOutput,
		GLenum scale, GLenum bias,GLboolean abDotProduct,GLboolean cdDotProduct,
		GLboolean muxSum);

void (APIENTRYP qglFinalCombinerInputNV) (GLenum variable,GLenum input,GLenum mapping,GLenum componentUsage);
void (APIENTRYP qglGetCombinerInputParameterfvNV) (GLenum stage,GLenum portion,GLenum variable,GLenum pname,GLfloat *params);
void (APIENTRYP qglGetCombinerInputParameterivNV) (GLenum stage,GLenum portion,GLenum variable,GLenum pname,GLint *params);
void (APIENTRYP qglGetCombinerOutputParameterfvNV) (GLenum stage,GLenum portion,GLenum pname,GLfloat *params);
void (APIENTRYP qglGetCombinerOutputParameterivNV) (GLenum stage,GLenum portion,GLenum pname,GLint *params);
void (APIENTRYP qglGetFinalCombinerInputParameterfvNV) (GLenum variable,GLenum pname,GLfloat *params);
void (APIENTRYP qglGetFinalCombinerInputParameterivNV) (GLenum variable,GLenum pname,GLint *params);

PFNGLPROGRAMSTRINGARBPROC qglProgramStringARB = NULL;
PFNGLBINDPROGRAMARBPROC qglBindProgramARB = NULL;
PFNGLDELETEPROGRAMSARBPROC qglDeleteProgramsARB = NULL;
PFNGLGENPROGRAMSARBPROC qglGenProgramsARB = NULL;
PFNGLPROGRAMENVPARAMETER4DARBPROC qglProgramEnvParameter4dARB = NULL;
PFNGLPROGRAMENVPARAMETER4DVARBPROC qglProgramEnvParameter4dvARB = NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC qglProgramEnvParameter4fARB = NULL;
PFNGLPROGRAMENVPARAMETER4FVARBPROC qglProgramEnvParameter4fvARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4DARBPROC qglProgramLocalParameter4dARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4DVARBPROC qglProgramLocalParameter4dvARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC qglProgramLocalParameter4fARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC qglProgramLocalParameter4fvARB = NULL;
PFNGLGETPROGRAMENVPARAMETERDVARBPROC qglGetProgramEnvParameterdvARB = NULL;
PFNGLGETPROGRAMENVPARAMETERFVARBPROC qglGetProgramEnvParameterfvARB = NULL;
PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC qglGetProgramLocalParameterdvARB = NULL;
PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC qglGetProgramLocalParameterfvARB = NULL;
PFNGLGETPROGRAMIVARBPROC qglGetProgramivARB = NULL;
PFNGLGETPROGRAMSTRINGARBPROC qglGetProgramStringARB = NULL;
PFNGLISPROGRAMARBPROC qglIsProgramARB = NULL;

void ( APIENTRY * qglTexImage3DEXT) (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
void ( APIENTRY * qglTexSubImage3DEXT) (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);

void ( APIENTRY * qglPointParameterfEXT)( GLenum param, GLfloat value );
void ( APIENTRY * qglPointParameterfvEXT)( GLenum param, GLfloat *value );

void ( * qglLockArraysEXT)( int, int);
void ( * qglUnlockArraysEXT) ( void );
 */

/*
===============
GLimp_Minimize

Minimize the game so that user is back at the desktop
===============
 */
void GLimp_Minimize(void)
{
	SDL_MinimizeWindow( screen );
}


extern int androidSwapped; //If loading, then draw frame does not return, so detect this
/*
 ** GLimp_EndFrame
 */
void GLimp_EndFrame (void)
{
	if (!androidSwapped)
		eglSwapBuffers( eglGetCurrentDisplay(), eglGetCurrentSurface( EGL_DRAW ) );

	androidSwapped = false;
}

/*
===============
GLimp_CompareModes
===============
 */
static int GLimp_CompareModes( const void *a, const void *b )
{
	const float ASPECT_EPSILON = 0.001f;
	SDL_Rect *modeA = (SDL_Rect *)a;
	SDL_Rect *modeB = (SDL_Rect *)b;
	float aspectA = (float)modeA->w / (float)modeA->h;
	float aspectB = (float)modeB->w / (float)modeB->h;
	int areaA = modeA->w * modeA->h;
	int areaB = modeB->w * modeB->h;
	float aspectDiffA = fabs( aspectA - displayAspect );
	float aspectDiffB = fabs( aspectB - displayAspect );
	float aspectDiffsDiff = aspectDiffA - aspectDiffB;

	if( aspectDiffsDiff > ASPECT_EPSILON )
		return 1;
	else if( aspectDiffsDiff < -ASPECT_EPSILON )
		return -1;
	else
		return areaA - areaB;
}

/*
===============
GLimp_DetectAvailableModes
===============
 */
static bool GLimp_DetectAvailableModes(void)
{
	int i;
	char buf[ MAX_STRING_CHARS ] = { 0 };
	SDL_Rect modes[ 128 ];
	int numModes = 0;

	int display = SDL_GetWindowDisplayIndex( screen );
	SDL_DisplayMode windowMode;

	if( SDL_GetWindowDisplayMode( screen, &windowMode ) < 0 )
	{
		Com_Printf( "Couldn't get window display mode, no resolutions detected (%s).\n", SDL_GetError() );
		return false;
	}

	int numDisplayModes = SDL_GetNumDisplayModes( display );
	for( i = 0; i < numDisplayModes; i++ )
	{
		SDL_DisplayMode mode;

		if( SDL_GetDisplayMode( display, i, &mode ) < 0 )
			continue;

		if( !mode.w || !mode.h )
		{
			Com_Printf( "Display supports any resolution\n" );
			return true;
		}

		if( windowMode.format != mode.format )
			continue;

		modes[ numModes ].w = mode.w;
		modes[ numModes ].h = mode.h;
		numModes++;
	}

	if( numModes > 1 )
		qsort( modes, numModes, sizeof( SDL_Rect ), GLimp_CompareModes );

	for( i = 0; i < numModes; i++ )
	{
		const char *newModeString = va( "%ux%u ", modes[ i ].w, modes[ i ].h );

		if( strlen( newModeString ) < (int)sizeof( buf ) - strlen( buf ) )
			Q_strcat( buf, sizeof( buf ), newModeString );
		else
			Com_Printf( "Skipping mode %ux%x, buffer too small\n", modes[ i ].w, modes[ i ].h );
	}

	if( *buf )
	{
		buf[ strlen( buf ) - 1 ] = 0;
		Com_Printf( "Available modes: '%s'\n", buf );
		ri->Cvar_Set( "r_availableModes", buf );
	}

	return true;
}


/*
===============
GLimp_SetMode
===============
 */
static rserr_t GLimp_SetMode(int mode, qboolean fullscreen, qboolean noborder)
{

	SDL_GetAndroidScreenSize(&glConfig.vidWidth,&glConfig.vidHeight);

	glConfig.colorBits = 16;
	glConfig.depthBits = 16;
	glConfig.stencilBits = 0;
	glConfig.isFullscreen = qtrue;
	glConfig.stereoEnabled = qfalse;
	//glConfig.windowAspect = glConfig.vidWidth / glConfig.vidHeight;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	return RSERR_OK;
	const char *glstring;
	int perChannelColorBits;
	int colorBits, depthBits, stencilBits;
	//int samples;
	int i = 0;
	Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
	SDL_DisplayMode desktopMode;
	int display = 0;
	int x = SDL_WINDOWPOS_UNDEFINED, y = SDL_WINDOWPOS_UNDEFINED;

	Com_Printf( "Initializing OpenGL display\n");

	if ( r_allowResize->integer && !fullscreen )
		flags |= SDL_WINDOW_RESIZABLE;

	// If a window exists, note its display index
	if( screen != NULL )
		display = SDL_GetWindowDisplayIndex( screen );

	if( SDL_GetDesktopDisplayMode( display, &desktopMode ) == 0 )
	{
		displayAspect = (float)desktopMode.w / (float)desktopMode.h;

		Com_Printf( "Display aspect: %.3f\n", displayAspect );
	}
	else
	{
		Com_Memset( &desktopMode, 0, sizeof( SDL_DisplayMode ) );

		Com_Printf( "Cannot determine display aspect, assuming 1.333\n" );
	}

	Com_Printf( "...setting mode %d:", mode );

	if (mode == -2)
	{
		// use desktop video resolution
		if( desktopMode.h > 0 )
		{
			glConfig.vidWidth = desktopMode.w;
			glConfig.vidHeight = desktopMode.h;
		}
		else
		{
			glConfig.vidWidth = 640;
			glConfig.vidHeight = 480;
			Com_Printf( "Cannot determine display resolution, assuming 640x480\n" );
		}

		//glConfig.windowAspect = (float)glConfig.vidWidth / (float)glConfig.vidHeight;
	}
	else if ( !R_GetModeInfo( &glConfig.vidWidth, &glConfig.vidHeight, /*&glConfig.windowAspect,*/ mode ) )
	{
		Com_Printf( " invalid mode\n" );
		return RSERR_INVALID_MODE;
	}
	Com_Printf( " %d %d\n", glConfig.vidWidth, glConfig.vidHeight);

	// Center window
	if( r_centerWindow->integer && !fullscreen )
	{
		x = ( desktopMode.w / 2 ) - ( glConfig.vidWidth / 2 );
		y = ( desktopMode.h / 2 ) - ( glConfig.vidHeight / 2 );
	}

	// Destroy existing state if it exists
	if( opengl_context != NULL )
	{
		SDL_GL_DeleteContext( opengl_context );
		opengl_context = NULL;
	}

	if( screen != NULL )
	{
		SDL_GetWindowPosition( screen, &x, &y );
		ri->Printf( PRINT_DEVELOPER, "Existing window at %dx%d before being destroyed\n", x, y );
		SDL_DestroyWindow( screen );
		screen = NULL;
	}

	if( fullscreen )
	{
		flags |= SDL_WINDOW_FULLSCREEN;
		glConfig.isFullscreen = qtrue;
	}
	else
	{
		if( noborder )
			flags |= SDL_WINDOW_BORDERLESS;

		glConfig.isFullscreen = qfalse;
	}

	colorBits = r_colorbits->value;
	if ((!colorBits) || (colorBits >= 32))
		colorBits = 24;

	if (!r_depthbits->value)
		depthBits = 24;
	else
		depthBits = r_depthbits->value;

	stencilBits = r_stencilbits->value;
	//samples = r_ext_multisample->value;

	for (i = 0; i < 16; i++)
	{
		int testColorBits, testDepthBits, testStencilBits;

		// 0 - default
		// 1 - minus colorBits
		// 2 - minus depthBits
		// 3 - minus stencil
		if ((i % 4) == 0 && i)
		{
			// one pass, reduce
			switch (i / 4)
			{
			case 2 :
				if (colorBits == 24)
					colorBits = 16;
				break;
			case 1 :
				if (depthBits == 24)
					depthBits = 16;
				else if (depthBits == 16)
					depthBits = 8;
			case 3 :
				if (stencilBits == 24)
					stencilBits = 16;
				else if (stencilBits == 16)
					stencilBits = 8;
			}
		}

		testColorBits = colorBits;
		testDepthBits = depthBits;
		testStencilBits = stencilBits;

		if ((i % 4) == 3)
		{ // reduce colorBits
			if (testColorBits == 24)
				testColorBits = 16;
		}

		if ((i % 4) == 2)
		{ // reduce depthBits
			if (testDepthBits == 24)
				testDepthBits = 16;
			else if (testDepthBits == 16)
				testDepthBits = 8;
		}

		if ((i % 4) == 1)
		{ // reduce stencilBits
			if (testStencilBits == 24)
				testStencilBits = 16;
			else if (testStencilBits == 16)
				testStencilBits = 8;
			else
				testStencilBits = 0;
		}

		if (testColorBits == 24)
			perChannelColorBits = 8;
		else
			perChannelColorBits = 4;

#ifdef __sgi /* Fix for SGIs grabbing too many bits of color */
		if (perChannelColorBits == 4)
			perChannelColorBits = 0; /* Use minimum size for 16-bit color */

		/* Need alpha or else SGIs choose 36+ bit RGB mode */
		SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 1);
#endif

		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, perChannelColorBits );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, perChannelColorBits );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, perChannelColorBits );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, testDepthBits );
		SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, testStencilBits );

		/*SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, samples ? 1 : 0 );
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, samples );*/

		if(r_stereo->integer)
		{
			glConfig.stereoEnabled = qtrue;
			SDL_GL_SetAttribute(SDL_GL_STEREO, 1);
		}
		else
		{
			glConfig.stereoEnabled = qfalse;
			SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
		}

		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

		// If not allowing software GL, demand accelerated
		if( !r_allowSoftwareGL->integer )
			SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );

		if( ( screen = SDL_CreateWindow( CLIENT_WINDOW_TITLE, x, y,
				glConfig.vidWidth, glConfig.vidHeight, flags ) ) == NULL )
		{
			ri->Printf( PRINT_DEVELOPER, "SDL_CreateWindow failed: %s\n", SDL_GetError( ) );
			continue;
		}

		if( fullscreen )
		{
			SDL_DisplayMode mode;

			switch( testColorBits )
			{
			case 16: mode.format = SDL_PIXELFORMAT_RGB565; break;
			case 24: mode.format = SDL_PIXELFORMAT_RGB24;  break;
			default: ri->Printf( PRINT_DEVELOPER, "testColorBits is %d, can't fullscreen\n", testColorBits ); continue;
			}

			mode.w = glConfig.vidWidth;
			mode.h = glConfig.vidHeight;
			mode.refresh_rate = glConfig.displayFrequency = ri->Cvar_VariableIntegerValue( "r_displayRefresh" );
			mode.driverdata = NULL;

			if( SDL_SetWindowDisplayMode( screen, &mode ) < 0 )
			{
				ri->Printf( PRINT_DEVELOPER, "SDL_SetWindowDisplayMode failed: %s\n", SDL_GetError( ) );
				continue;
			}
		}

		SDL_SetWindowTitle( screen, CLIENT_WINDOW_TITLE );

		if( ( opengl_context = SDL_GL_CreateContext( screen ) ) == NULL )
		{
			Com_Printf( "SDL_GL_CreateContext failed: %s\n", SDL_GetError( ) );
			continue;
		}

		SDL_GL_SetSwapInterval( r_swapInterval->integer );

		glConfig.colorBits = testColorBits;
		glConfig.depthBits = testDepthBits;
		glConfig.stencilBits = testStencilBits;

		Com_Printf( "Using %d color bits, %d depth, %d stencil display.\n",
				glConfig.colorBits, glConfig.depthBits, glConfig.stencilBits );
		break;
	}

	if (!GLimp_DetectAvailableModes())
	{
		return RSERR_UNKNOWN;
	}

	glstring = (char *) qglGetString (GL_RENDERER);
	Com_Printf( "GL_RENDERER: %s\n", glstring );

	return RSERR_OK;
}

/*
===============
GLimp_StartDriverAndSetMode
===============
 */
static qboolean GLimp_StartDriverAndSetMode(int mode, qboolean fullscreen, qboolean noborder)
{
	rserr_t err;

	if (!SDL_WasInit(SDL_INIT_VIDEO))
	{
		const char *driverName;

		if (SDL_Init(SDL_INIT_VIDEO) == -1)
		{
			Com_Printf( "SDL_Init( SDL_INIT_VIDEO ) FAILED (%s)\n",
					SDL_GetError());
			return qfalse;
		}

		driverName = SDL_GetCurrentVideoDriver();

		if (!driverName)
		{
			Com_Error( ERR_FATAL, "No video driver initialized" );
			return qfalse;
		}

		Com_Printf( "SDL using driver \"%s\"\n", driverName );
		ri->Cvar_Set( "r_sdlDriver", driverName );
	}

	if (SDL_GetNumVideoDisplays() <= 0)
	{
		Com_Error( ERR_FATAL, "SDL_GetNumVideoDisplays() FAILED (%s)", SDL_GetError() );
	}

	if (fullscreen && ri->Cvar_VariableIntegerValue( "in_nograb" ) )
	{
		Com_Printf( "Fullscreen not allowed with in_nograb 1\n");
		ri->Cvar_Set( "r_fullscreen", "0" );
		r_fullscreen->modified = qfalse;
		fullscreen = qfalse;
	}

	err = GLimp_SetMode(mode, fullscreen, noborder);

	switch ( err )
	{
	case RSERR_INVALID_FULLSCREEN:
		Com_Printf( "...WARNING: fullscreen unavailable in this mode\n" );
		return qfalse;
	case RSERR_INVALID_MODE:
		Com_Printf( "...WARNING: could not set the given mode (%d)\n", mode );
		return qfalse;
	case RSERR_UNKNOWN:
		Com_Printf( "...ERROR: no display modes could be found.\n" );
		return qfalse;
	default:
		break;
	}

	return qtrue;
}

/*
 ** GLW_CheckForExtension

  Cannot use strstr directly to differentiate between (for eg) reg_combiners and reg_combiners2
 */

bool GL_CheckForExtension(const char *ext)
{
	const char *ptr = Q_stristr( glConfig.extensions_string, ext );
	if (ptr == NULL)
		return false;
	ptr += strlen(ext);
	return ((*ptr == ' ') || (*ptr == '\0'));  // verify it's complete string.
}

static void GLW_InitTextureCompression( void )
{
	glConfig.textureCompression = TC_NONE;
}

/*
===============
GLimp_InitExtensions
===============
 */
extern bool g_bDynamicGlowSupported;
static void GLimp_InitExtensions( void )
{
	if ( !r_allowExtensions->integer )
	{
		Com_Printf ("*** IGNORING OPENGL EXTENSIONS ***\n" );
		g_bDynamicGlowSupported = false;
		ri->Cvar_Set( "r_DynamicGlow","0" );
		return;
	}

	Com_Printf ("Initializing OpenGL extensions\n" );

	// Select our tc scheme
	GLW_InitTextureCompression();

	glConfig.textureEnvAddAvailable = qtrue;
	//glConfig.textureFilterAnisotropicAvailable = qtrue;




	// GL_EXT_clamp_to_edge
	glConfig.clampToEdgeAvailable = qtrue;
	Com_Printf ("...using GL_EXT_texture_edge_clamp\n" );


	// GL_ARB_multitexture
	qglMultiTexCoord2fARB = NULL;
	qglActiveTextureARB = NULL;
	qglClientActiveTextureARB = NULL;

	qglGetIntegerv( GL_MAX_TEXTURE_UNITS, &glConfig.maxActiveTextures );
	//ri.Printf( PRINT_ALL, "...not using GL_ARB_multitexture, %i texture units\n", glConfig.maxActiveTextures );
	//glConfig.maxActiveTextures=4;
	qglMultiTexCoord2fARB = myglMultiTexCoord2f;
	qglActiveTextureARB = glActiveTexture;
	qglClientActiveTextureARB = glClientActiveTexture;
	if ( glConfig.maxActiveTextures > 1 )
	{
		Com_Printf(  "...using GL_ARB_multitexture (%i texture units)\n", glConfig.maxActiveTextures );
	}
	else
	{
		qglMultiTexCoord2fARB = NULL;
		qglActiveTextureARB = NULL;
		qglClientActiveTextureARB = NULL;
		Com_Printf(  "...not using GL_ARB_multitexture, < 2 texture units\n" );
	}



	// GL_EXT_compiled_vertex_array
	qglLockArraysEXT = NULL;
	qglUnlockArraysEXT = NULL;


	qglPointParameterfEXT = NULL;
	qglPointParameterfvEXT = NULL;

	//3d textures -rww
	//	qglTexImage3DEXT = NULL;
	//	qglTexSubImage3DEXT = NULL;

	qglPointParameterfEXT = &glPointParameterf;
	qglPointParameterfvEXT = &glPointParameterfv;
	Com_Printf(  "...using GL_EXT_point_parameters\n" );


	//bNVRegisterCombiners = false;
	//qglCombinerParameterfvNV = NULL;
	//qglCombinerParameteriNV = NULL;
	Com_Printf ("...ignoring GL_NV_register_combiners\n" );



	// NOTE: Vertex and Fragment Programs are very dependant on each other - this is actually a
	// good thing! So, just check to see which we support (one or the other) and load the shared
	// function pointers. ARB rocks!

	// Vertex Programs.
	bool bARBVertexProgram = false;


	// Fragment Programs.
	bool bARBFragmentProgram = false;




	// Figure out which texture rectangle extension to use.
	bool bTexRectSupported = false;



	g_bDynamicGlowSupported = false;
	ri->Cvar_Set( "r_DynamicGlow","0" );
}


// Truncates the GL extensions string by only allowing up to 'maxExtensions' extensions in the string.
static const char *TruncateGLExtensionsString (const char *extensionsString, int maxExtensions)
{
	const char *p = extensionsString;
	const char *q;
	int numExtensions = 0;
	size_t extensionsLen = strlen (extensionsString);

	char *truncatedExtensions;

	while ( (q = strchr (p, ' ')) != NULL && numExtensions <= maxExtensions )
	{
		p = q + 1;
		numExtensions++;
	}

	if ( q != NULL )
	{
		// We still have more extensions. We'll call this the end

		extensionsLen = p - extensionsString - 1;
	}

	truncatedExtensions = (char *)ri->Hunk_Alloc (extensionsLen + 1, h_low);
	Q_strncpyz (truncatedExtensions, extensionsString, extensionsLen + 1);

	return truncatedExtensions;
}

void 		GLimp_Init( void )
{
	ri->Cvar_Get( "r_restartOnResize", "1", CVAR_ARCHIVE );
	ri->Cvar_Get( "r_resizeDelay", "1000", CVAR_ARCHIVE );
	r_allowSoftwareGL = ri->Cvar_Get( "r_allowSoftwareGL", "0", CVAR_LATCH );
	r_sdlDriver = ri->Cvar_Get( "r_sdlDriver", "", CVAR_ROM );
	r_allowResize = ri->Cvar_Get( "r_allowResize", "0", CVAR_ARCHIVE );

	/*	if( Cvar_VariableIntegerValue( "com_abnormalExit" ) )
	{
		Cvar_Set( "r_mode", va( "%d", R_MODE_FALLBACK ) );
		Cvar_Set( "r_picmap", "1" );
		Cvar_Set( "r_texturebits", "0" );
		Cvar_Set( "r_textureMode", "GL_LINEAR_MIPMAP_NEAREST" );
		Cvar_Set( "r_fullscreen", "0" );
		Cvar_Set( "r_centerWindow", "0" );
		Cvar_Set( "com_abnormalExit", "0" );
		}*/

	// Create the window and set up the context
	if(GLimp_StartDriverAndSetMode(r_mode->integer, (qboolean)r_fullscreen->integer, (qboolean)r_noborder->integer))
		goto success;

	// Try again, this time in a platform specific "safe mode"
	/* Sys_GLimpSafeInit( );

	if(GLimp_StartDriverAndSetMode(r_mode->integer, r_fullscreen->integer, qfalse))
		goto success; */

	/*	// Finally, try the default screen resolution
	if( r_mode->integer != R_MODE_FALLBACK )
	{
		Com_Printf( "Setting r_mode %d failed, falling back on r_mode %d\n",
				r_mode->integer, R_MODE_FALLBACK );

		if(GLimp_StartDriverAndSetMode(R_MODE_FALLBACK, qfalse, qfalse))
			goto success;
			}*/

	// Nothing worked, give up
	//Com_Error( ERR_FATAL, "GLimp_Init() - could not load OpenGL subsystem" );

	success:
	// This values force the UI to disable driver selection
	//	glConfig.driverType = GLDRV_ICD;
	//	glConfig.hardwareType = GLHW_GENERIC;
	glConfig.deviceSupportsGamma = (qboolean)(!r_ignorehwgamma->integer &&
			SDL_SetWindowBrightness( screen, 1.0f ) >= 0);

	// get our config strings
	glConfig.vendor_string = (const char *) qglGetString (GL_VENDOR);
	glConfig.renderer_string = (const char *) qglGetString (GL_RENDERER);
	glConfig.version_string = (const char *) qglGetString (GL_VERSION);
	glConfig.extensions_string = (const char *) qglGetString (GL_EXTENSIONS);

	glConfigExt.originalExtensionString = glConfig.extensions_string;
	glConfig.extensions_string = TruncateGLExtensionsString (glConfigExt.originalExtensionString, 128);

	// OpenGL driver constants
	qglGetIntegerv( GL_MAX_TEXTURE_SIZE, &glConfig.maxTextureSize );
	// stubbed or broken drivers may have reported 0...
	if ( glConfig.maxTextureSize <= 0 )
	{
		glConfig.maxTextureSize = 0;
	}

	// initialize extensions
	GLimp_InitExtensions( );

	ri->Cvar_Get( "r_availableModes", "", CVAR_ROM );

	ri->Cvar_Set( "r_colorbits", "16");
	ri->Cvar_Set( "r_texturebits", "16");
	ri->Cvar_Set( "r_mode", "3");	//force 640

	// This depends on SDL_INIT_VIDEO, hence having it here
	ri->IN_Init( screen );
}

/*
===============
GLimp_Shutdown
===============
 */
void 		GLimp_Shutdown( void )
{
	ri->IN_Shutdown();

	SDL_QuitSubSystem( SDL_INIT_VIDEO );

	Com_Memset( &glConfig, 0, sizeof( glConfig ) );
	Com_Memset( &glState, 0, sizeof( glState ) );
}

void		GLimp_EnableLogging( qboolean enable )
{
}

void 		GLimp_LogComment( char *comment )
{
}

void GLimp_SetGamma( unsigned char red[256], unsigned char green[256], unsigned char blue[256] )
{
	Uint16 table[3][256];
	int i, j;

	if( !glConfig.deviceSupportsGamma || r_ignorehwgamma->integer > 0 )
		return;

	for (i = 0; i < 256; i++)
	{
		table[0][i] = ( ( ( Uint16 ) red[i] ) << 8 ) | red[i];
		table[1][i] = ( ( ( Uint16 ) green[i] ) << 8 ) | green[i];
		table[2][i] = ( ( ( Uint16 ) blue[i] ) << 8 ) | blue[i];
	}

#ifdef _WIN32
#include <windows.h>

	// Win2K and newer put this odd restriction on gamma ramps...
	{
		OSVERSIONINFO	vinfo;

		vinfo.dwOSVersionInfoSize = sizeof( vinfo );
		GetVersionEx( &vinfo );
		if( vinfo.dwMajorVersion >= 5 && vinfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
		{
			ri->Printf( PRINT_DEVELOPER, "performing gamma clamp.\n" );
			for( j = 0 ; j < 3 ; j++ )
			{
				for( i = 0 ; i < 128 ; i++ )
				{
					if( table[ j ] [ i] > ( ( 128 + i ) << 8 ) )
						table[ j ][ i ] = ( 128 + i ) << 8;
				}

				if( table[ j ] [127 ] > 254 << 8 )
					table[ j ][ 127 ] = 254 << 8;
			}
		}
	}
#endif

	// enforce constantly increasing
	for (j = 0; j < 3; j++)
	{
		for (i = 1; i < 256; i++)
		{
			if (table[j][i] < table[j][i-1])
				table[j][i] = table[j][i-1];
		}
	}

	SDL_SetWindowGammaRamp(screen, table[0], table[1], table[2]);
}
