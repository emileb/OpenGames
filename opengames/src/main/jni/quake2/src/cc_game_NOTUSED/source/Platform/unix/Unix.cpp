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
This source file is contained as part of CleanCode Quake2, a project maintained
by Paril, to 'clean up' and make Quake2 an easier source base to read and work with.

You may use any part of this code to help create your own bases and own mods off
this code if you wish. It is under the same license as Quake 2 source (as above),
therefore you are free to have to fun with it. All I ask is you email me so I can
list the mod on my page for CleanCode Quake2 to help get the word around. Thanks.
*/

//
// cc_platform_unix.cpp
// Unix/GCC Compiler Platform
//

#include "Local.h"
#include <sys/time.h>
#include <dlfcn.h>


// Functions
/*
================
Sys_FindFiles

OS-dependent function.
This function must calculate a list of
files starting in "path" that match "pattern",
optionally "recurse"ing directories. If
addFiles is true, individual files will be 
added, if addDirs is true a folder can be added.
Files will be put into "files".
================
*/

void Sys_FindFiles (TFindFilesType &files, const char *path, const char *pattern, bool recurse, bool addFiles, bool addDirs)
{
}

/*
================
CC_OutputDebugString

Output the string to debuggers and such.
You can modify this to suit your platform/debugger/compiler.
================
*/
void CC_OutputDebugString (const char *text)
{
}

/*
================
CC_ReportGameError

Use this to properly break the debuggers
so you can debug fatal game errors
================
*/
void CC_ReportGameError (const char *text)
{
}


// Classes

// Timer

/*
================
Sys_UMilliseconds
================
*/
uint32 Sys_UMilliseconds ()
{
	struct timeval	tp;
	struct timezone	tzp;
	static int		secbase;

	gettimeofday (&tp, &tzp);

	if (!secbase) {
		secbase = tp.tv_sec;
		return tp.tv_usec/1000;
	}
	
	return (tp.tv_sec - secbase)*1000 + tp.tv_usec/1000;
}

/*
================
CTimer::Start

This function starts the timer. Generally you would
return the milliseconds and store in StartCycles.
================
*/
void CTimer::Start ()
{
	StartCycles = Sys_UMilliseconds();
};

/*
================
CTimer::Get

This function ends the timer, calculates difference,
restarts timer, and returns the difference.
================
*/
double CTimer::Get ()
{
	// Grab the value
	double value = (Sys_UMilliseconds()-StartCycles);

	// Restart the timer
	Start ();

	return value;
};

// Dynamic libraries

/*
================
CDynamicLibrary::OS_LoadLibrary

This function loads a library from a file and
returns it. Return NULL if the library is not found.
================
*/
void *CDynamicLibrary::OS_LoadLibrary (const char *FileName)
{
	return dlopen(FileName, RTLD_LAZY);
};

/*
================
CDynamicLibrary::OS_CloseLibrary

Expected to close and free library handles.
================
*/
void CDynamicLibrary::OS_CloseLibrary ()
{
	dlclose (Lib);
};

/*
================
CDynamicLibrary::OS_GetProcAddress

Returns pointer to symbol located at "Symbol".
NULL if error.
================
*/
void *CDynamicLibrary::OS_GetProcAddress (const char *Symbol)
{
	return dlsym(Lib, Symbol);
};
