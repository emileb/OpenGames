/*
Copyright (C) 1996-1997 Id Software, Inc.

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

// System functions for Android OS.
// Based on sys_linux.c

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
// #include <sys/ipc.h>
// #include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <errno.h>
#include <dirent.h>


#include "quakedef.h"

extern bool QUAKE_QUIT;
qboolean			isDedicated;

int noconinput = 0;
int nostdout = 0;

// Look for data on either the sdcard or the internal data store.
// (We look at the sdcard first.)

// These paths must match equivalent paths in QuakeActivity.java

static const char *basedir;
static const char *cachedir;

cvar_t  sys_linerefresh = CVAR2("sys_linerefresh","0");// set for entity display

// =======================================================================
// General routines
// =======================================================================

void Sys_DebugNumber(int y, int val)
{
}

/*
void Sys_Printf (char *fmt, ...)
{
  va_list		argptr;
  char		text[1024];

  va_start (argptr,fmt);
  vsprintf (text,fmt,argptr);
  va_end (argptr);
  fprintf(stderr, "%s", text);

  Con_Print (text);
}

void Sys_Printf (char *fmt, ...)
{

    va_list     argptr;
    char        text[1024], *t_p;
    int         l, r;

    if (nostdout)
        return;

    va_start (argptr,fmt);
    vsprintf (text,fmt,argptr);
    va_end (argptr);

    l = strlen(text);
    t_p = text;

// make sure everything goes through, even though we are non-blocking
    while (l)
    {
        r = write (1, text, l);
        if (r != l)
            sleep (0);
        if (r > 0)
        {
            t_p += r;
            l -= r;
        }
    }

}
 */

#define USE_PMPEVENT

void Sys_Printf (const char *fmt, ...)
{
	va_list		argptr;
	char		text[2048];
	unsigned char		*p;

	va_start (argptr,fmt);
	vsnprintf (text, sizeof(text), fmt,argptr);
	va_end (argptr);

	text[sizeof(text)-1] = 0;
	LOGI("%s", text);

#ifdef USE_PMPEVENT
	PMPEVENT(("%s", text));
#else
	if (nostdout)
		return;

	for (p = (unsigned char *)text; *p; p++)
		if ((*p > 128 || *p < 32) && *p != 10 && *p != 13 && *p != 9)
			printf("[%02x]", *p);
		else
			putc(*p, stdout);
#endif
}

qboolean soft_quit;

void Sys_Quit (void)
{
	QUAKE_QUIT = true;
	/*
	Host_Shutdown();
#ifdef USE_PMPEVENT
	PMPERROR(("Sys_Quit - exiting."));
#else
	printf("Sys_Quit - exiting.\n");
#endif
	// fcntl (0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY);
	if (soft_quit) {
		return;
	}
	LOGI("Sys_Quit - exiting." );
	exit(0);
	*/
}

void Sys_Init(void)
{

}

void Sys_Error (const char *error, ...)
{
	va_list     argptr;
	char        string[1024];

	// change stdin to non blocking
	// fcntl (0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY);

	va_start (argptr,error);
	vsprintf (string,error,argptr);
	va_end (argptr);
#ifdef USE_PMPEVENT
	PMPERROR(("Error: %s\n", string));
#else
	fprintf(stderr, "Error: %s\n", string);
#endif
	Host_Shutdown ();
#ifdef USE_PMPEVENT
	PMPERROR(("Sys_Error - exiting."));
#else
	printf("Sys_Error - exiting.\n");
#endif
	LOGI("QUAKE ERROR: %s", string);
	//exit (1);

}

void Sys_Warn (const char *warning, ...)
{
	va_list     argptr;
	char        string[1024];

	va_start (argptr,warning);
	vsprintf (string,warning,argptr);
	va_end (argptr);
#ifdef USE_PMPEVENT
	PMPWARNING(("Warning: %s", string));
#else
	fprintf(stderr, "Warning: %s\n", string);
#endif
}

/*
============
Sys_FileTime

returns -1 if not present
============
 */
int	Sys_FileTime (const char *path)
{
	struct	stat	buf;

	if (stat (path,&buf) == -1)
		return -1;

	return buf.st_mtime;
}


void Sys_mkdir (const char *path)
{
	mkdir (path, 0777);
}

int Sys_FileOpenRead (const char *path, int *handle)
{
	int	h;
	struct stat	fileinfo;


	h = open (path, O_RDONLY, 0666);
	*handle = h;
	if (h == -1)
		return -1;

	if (fstat (h,&fileinfo) == -1)
		Sys_Error ("Error fstating %s", path);

	return fileinfo.st_size;
}

int Sys_FileOpenWrite (const char *path)
{
	int     handle;

	umask (0);

	handle = open(path,O_RDWR | O_CREAT | O_TRUNC
			, 0666);

	if (handle == -1)
		Sys_Error ("Error opening %s: %s", path,strerror(errno));

	return handle;
}

int Sys_FileWrite (int handle, const void *src, int count)
{
	return write (handle, src, count);
}

void Sys_FileClose (int handle)
{
	close (handle);
}

void Sys_FileSeek (int handle, int position)
{
	lseek (handle, position, SEEK_SET);
}

int Sys_FileRead (int handle, void *dest, int count)
{
	return read (handle, dest, count);
}

void Sys_DebugLog(const char *file, char *fmt, ...)
{
	va_list argptr;
	static char data[1024];
	int fd;

	va_start(argptr, fmt);
	vsprintf(data, fmt, argptr);
	va_end(argptr);
	//    fd = open(file, O_WRONLY | O_BINARY | O_CREAT | O_APPEND, 0666);
	fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0666);
	write(fd, data, strlen(data));
	close(fd);
}

void Sys_EditFile(const char *filename)
{

	char cmd[256];
	char *term;
	const char *editor;

	term = getenv("TERM");
	if (term && !strcmp(term, "xterm"))
	{
		editor = getenv("VISUAL");
		if (!editor)
			editor = getenv("EDITOR");
		if (!editor)
			editor = getenv("EDIT");
		if (!editor)
			editor = "vi";
		sprintf(cmd, "xterm -e %s %s", editor, filename);
		system(cmd);
	}

}

double Sys_FloatTime (void)
{
	struct timeval tp;
	struct timezone tzp;
	static int      secbase;

	gettimeofday(&tp, &tzp);

	if (!secbase)
	{
		secbase = tp.tv_sec;
		return tp.tv_usec/1000000.0;
	}

	return (tp.tv_sec - secbase) + tp.tv_usec/1000000.0;
}

// =======================================================================
// Sleeps for microseconds
// =======================================================================

static volatile int oktogo;

void alarm_handler(int x)
{
	oktogo=1;
}

void Sys_LineRefresh(void)
{
}

void floating_point_exception_handler(int whatever)
{
	//	Sys_Warn("floating point exception\n");
	//signal(SIGFPE, floating_point_exception_handler);
}

char *Sys_ConsoleInput(void)
{
#if 0
	static char text[256];
	int     len;

	if (cls.state == ca_dedicated) {
		len = read (0, text, sizeof(text));
		if (len < 1)
			return NULL;
		text[len-1] = 0;    // rip off the /n and terminate

		return text;
	}
#endif
	return NULL;
}

#if !id386
void Sys_HighFPPrecision (void)
{
}

void Sys_LowFPPrecision (void)
{
}
#endif

int		skipframes;

// The following APIs are called from the Java activity

double g_oldtime;

extern int scr_width;
extern int scr_height;

qboolean direxists(const char* path)
{
	struct stat sb;
	if(stat(path, &sb))
	{
		return 0;	// error
	}
	if(sb.st_mode & S_IFDIR)
	{
		return 1;
	}
	return 0;
}

// Remove all files in path. Recurses into subdirectories

void rmDir(const char* path) {
	DIR* dir = opendir(path);
	if(!dir) {
		return;
	}
	struct dirent * dp;
	while((dp = readdir(dir)) != NULL) {
		const char* name = dp->d_name;
		if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
			continue;
		}
		char filePath[1024];
		if ((int) (sizeof(filePath)-1) < snprintf(filePath, sizeof(filePath), "%s/%s", path, name)) {
			continue; // buffer overflow
		}
		if(direxists(filePath)) {
			rmDir(filePath);
		}
		else {
			unlink(filePath);
		}
	}
	closedir(dir);
	rmdir(path);
}

// Increment this number whenever the data format of any of the files stored in glquake changes:

typedef unsigned long GLCacheVersion;

static const GLCacheVersion kCurrentCacheVersion = 0x3a914000; // The numbers mean nothing special

// #define FORCE_INVALIDATE_CACHE // Useful for testing

#define GLQUAKE_RELPATH "/id1/glquake"
void CheckGLCacheVersion(const char* baseDir)
{
	char cachePath[1024];
	if ((int) (sizeof(cachePath)-1) < snprintf(cachePath, sizeof(cachePath), "%s" GLQUAKE_RELPATH "/cacheversion", baseDir)) {
		return; // buffer overflow
	}
	bool validCache = false;
	{
		GLCacheVersion vernum = 0;
		FILE* f = fopen(cachePath, "rb");
		if (f) {
			if (1 == fread(&vernum, sizeof(vernum), 1, f)) {
				if (vernum == kCurrentCacheVersion) {
					validCache = true;
				}
			}
			fclose(f);
		}
	}

#ifdef FORCE_INVALIDATE_CACHE
	validCache = false;
#endif

	if(!validCache) {
		PMPLOG(("Invalidating glquake cache."));
		char cacheDirPath[1024];
		if ( (int)(sizeof(cacheDirPath)-1) < snprintf(cacheDirPath, sizeof(cacheDirPath), "%s" GLQUAKE_RELPATH, baseDir)) {
			return; // Ran out ot memory
		}
		rmDir(cacheDirPath);
		Sys_mkdir(cacheDirPath);
		FILE* f = fopen(cachePath, "wb");
		if (f) {
			GLCacheVersion vernum = kCurrentCacheVersion;
			fwrite(&vernum, sizeof(vernum), 1, f);
			fclose(f);
		} else {
			PMPLOG(("Could not write %s %d.\n", cachePath, errno));
		}
	}
}



static int gArgc;
static const char** gArgv;

void PortableInitArgs(int argc,const char** argv) {
	gArgc = argc;
	gArgv = argv;
}

qboolean gDoubleInitializeGuard;
qboolean gInitialized;
void GL_ReInit();

bool PortableInit()
{
	PMPLOG(("PortableInit"));
	LOGI(("PortableInit"));

	PMPLOG(("This function was compiled on " __DATE__ " at " __TIME__));

	if (gDoubleInitializeGuard && gInitialized)
	{
		GL_ReInit();
	}

	gDoubleInitializeGuard = true;
	return true;
}

void PortableSetBasePath(const char *path){
	basedir = path;
}

void PortableSetCacheBasePath(const char *path){
	cachedir = path;
}
// Note: Needs a valid OpenGL context

void PortableInit2(int width, int height)
{
	PMPLOG(("PortableInit2 %d,%d", width, height));
	LOGI(("PortableInit2"));

	gInitialized = true;
	PMPBEGIN(("PortableInit2"));
	quakeparms_t parms;
	int j;
	int c = 0;
	const char* v[] = {"quake", (char*) 0};
	//int c = 3;
	//const char* v[] = {"quake", "-game", "shadows" ,(char*) 0};




	scr_width = width;
	scr_height = height;

	//	static char cwd[1024];

	//	signal(SIGFPE, floating_point_exception_handler);
	//  signal(SIGFPE, SIG_IGN);

	memset(&parms, 0, sizeof(parms));

	if (gArgc) {
		COM_InitArgv(gArgc, (const char**) gArgv);
	}
	else {
		COM_InitArgv(c, (const char**) v);
	}

	parms.argc = com_argc;
	parms.argv = com_argv;

	parms.memsize = 16*1024*1024;

	j = COM_CheckParm("-mem");
	if (j)
		parms.memsize = (int) (Q_atof(com_argv[j+1]) * 1024 * 1024);

	parms.membase = malloc (parms.memsize);

/*
	if(direxists(basedir))
	{

	}
	else
	{
		Sys_Error("Could not find data directory %s", basedir);
	}
	*/

	parms.basedir = basedir;

	CheckGLCacheVersion(basedir);

	// caching is disabled by default, use -cachedir to enable
	//	parms.cachedir = cachedir;

#if 0 // FNDELAY not implemented
	noconinput = COM_CheckParm("-noconinput");
	if (!noconinput)
		fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) | FNDELAY);
#endif

	if (COM_CheckParm("-nostdout"))
		nostdout = 1;

	Sys_Init();

	Host_Init(&parms);

	g_oldtime = Sys_FloatTime ();
	PMPEND(("AndroidInit2"));
}

static int currentFrame;
frameTime fastestFrame;
frameTime slowestFrame;

void InitFrameTimes()
{
	currentFrame = 0;
	fastestFrame.time = 1e6;
	fastestFrame.frame = 0;
	slowestFrame.time = -1;
	slowestFrame.frame = 0;
}

static void UpdateFrameTimes(float time)
{
	if (currentFrame > 0) {

		if (fastestFrame.time > time) {
			fastestFrame.time = time;
			fastestFrame.frame = currentFrame;
		}
		if (slowestFrame.time < time) {
			slowestFrame.time = time;
			slowestFrame.frame = currentFrame;
		}
	}
	currentFrame++;
}

int PortableStep(int width, int height)
{
	// PMPBEGIN(("AndroidStep"));
	double time, newtime;

//	LOGI("PortableStep %d",gInitialized);

	if(!gInitialized)
		PortableInit2(width, height);

	scr_width = width;
	scr_height = height;

	//scr_width = 500;
	//scr_height = 300;

	// find time spent rendering last frame
	newtime = Sys_FloatTime ();
	time = newtime - g_oldtime;
	//LOGI("time = %f", time);
	//Cap otherwise flickers on speedy devices
	//if (time < 0.017) time = 0.017;

	UpdateFrameTimes(time);

	Host_Frame(time);
	g_oldtime = newtime;
	// PMPEND(("AndroidStep"));
	int kd = 0;
	if ((!cls.demoplayback) && (key_dest == key_game))
		kd = 1;
	return kd;
}

extern void Host_Quit();
void PortableQuit() {
	soft_quit = true;
	Host_Quit();
	soft_quit = false; // In case we live on after returning.
}
