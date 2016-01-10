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
// cc_platform_win32.cpp
// Win32/64 specific functions.
//

#include "Local.h"

#ifdef WIN32
#define _DECL_DLLMAIN   /* enable prototypes for DllMain and _CRT_INIT */
#include <process.h>
#include <crtdbg.h>

char		sys_findBase[MAX_OSPATH];
char		sys_findPath[MAX_OSPATH];
HANDLE		sys_findHandle = INVALID_HANDLE_VALUE;

static bool Sys_CompareFileAttributes (sint32 found, uint32 mustHave, uint32 cantHave)
{
	// read only
	if (found & FILE_ATTRIBUTE_READONLY) {
		if (cantHave & SFF_RDONLY)
			return false;
	}
	else if (mustHave & SFF_RDONLY)
		return false;

	// hidden
	if (found & FILE_ATTRIBUTE_HIDDEN) {
		if (cantHave & SFF_HIDDEN)
			return false;
	}
	else if (mustHave & SFF_HIDDEN)
		return false;

	// system
	if (found & FILE_ATTRIBUTE_SYSTEM) {
		if (cantHave & SFF_SYSTEM)
			return false;
	}
	else if (mustHave & SFF_SYSTEM)
		return false;

	// subdir
	if (found & FILE_ATTRIBUTE_DIRECTORY) {
		if (cantHave & SFF_SUBDIR)
			return false;
	}
	else if (mustHave & SFF_SUBDIR)
		return false;

	// arch
	if (found & FILE_ATTRIBUTE_ARCHIVE) {
		if (cantHave & SFF_ARCH)
			return false;
	}
	else if (mustHave & SFF_ARCH)
		return false;

	return true;
}

/*
================
Sys_FindClose
================
*/
void Sys_FindClose ()
{
	if (sys_findHandle != INVALID_HANDLE_VALUE)
		FindClose (sys_findHandle);

	sys_findHandle = INVALID_HANDLE_VALUE;
}

/*
============
Com_FilePath

Returns the path up to, but not including the last /
============
*/
void Com_FilePath(char *path, char *out, size_t size)
{
	if (size)
	{
		char *s = path + strlen(path) - 1;
		while (s != path && *s != '/')
			s--;

		Q_strncpyz(out, path, size);
		if (s-path < (sint32)size) // FIXME
			out[s-path] = '\0';
	}
}

/*
================
Sys_FindFirst
================
*/
char *Sys_FindFirst (char *path, uint32 mustHave, uint32 cantHave)
{
	WIN32_FIND_DATAA	findinfo;

	if (sys_findHandle != INVALID_HANDLE_VALUE)
		DebugPrint ("Sys_BeginFind without close\n");

	Com_FilePath (path, sys_findBase, sizeof(sys_findBase));
	sys_findHandle = FindFirstFileA (path, &findinfo);

	if (sys_findHandle == INVALID_HANDLE_VALUE)
		return NULL;

	if (!Sys_CompareFileAttributes (findinfo.dwFileAttributes, mustHave, cantHave))
		return NULL;

	Q_snprintfz (sys_findPath, sizeof(sys_findPath), "%s/%s", sys_findBase, findinfo.cFileName);
	return sys_findPath;
}


/*
================
Sys_FindNext
================
*/
char *Sys_FindNext (uint32 mustHave, uint32 cantHave)
{
	WIN32_FIND_DATA	findinfo;

	if (sys_findHandle == INVALID_HANDLE_VALUE)
		return NULL;

	if (!FindNextFile (sys_findHandle, &findinfo))
		return NULL;

	if (!Sys_CompareFileAttributes (findinfo.dwFileAttributes, mustHave, cantHave))
		return NULL;

	Q_snprintfz (sys_findPath, sizeof(sys_findPath), "%s/%s", sys_findBase, findinfo.cFileName);
	return sys_findPath;
}


/*
================
Sys_FindFiles
================
*/

void Sys_FindFiles (TFindFilesType &files, const String &path, const String &pattern, bool recurse, bool addFiles, bool addDirs)
{
	WIN32_FIND_DATAA	findInfo;
	HANDLE			findHandle;
	BOOL			findRes = TRUE;

	String searchPath = String::Format("%s*", path.CString());

	findHandle = FindFirstFileA (searchPath.CString(), &findInfo);
	if (findHandle == INVALID_HANDLE_VALUE)
		return;

	while (findRes == TRUE)
	{
		// Check for invalid file name
		if (findInfo.cFileName[strlen(findInfo.cFileName)-1] == '.')
		{
			findRes = FindNextFileA (findHandle, &findInfo);
			continue;
		}

		String findPath = String::Format("%s%s", path.CString(), findInfo.cFileName);

		if (findInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// Add a directory
			if (addDirs)
				files.push_back (findPath);

			// Recurse down the next directory
			if (recurse)
				Sys_FindFiles (files, findPath, pattern, recurse, addFiles, addDirs);
		}
		else
		{
			// Match pattern
			if (!Q_WildcardMatch (pattern.CString(), findPath.CString(), 1))
			{
				findRes = FindNextFileA (findHandle, &findInfo);
				continue;
			}

			// Add a file
			if (addFiles)
				files.push_back (findPath);
		}

		findRes = FindNextFileA (findHandle, &findInfo);
	}

	FindClose (findHandle);
	return;
}

/*
================
CC_OutputDebugString

Output the string to debuggers and such.
================
*/
void CC_OutputDebugString (const char *text)
{
#if defined(_DEBUG)
	// Pipe to visual studio in debug mode
	//OutputDebugStringA(text);
#endif
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
#if defined(_DEBUG)
	// Pipe to visual studio
	OutputDebugStringA(text);
#endif
	_CrtDbgBreak ();
}

static sint32 sys_timeBase;
static double sys_msPerCycle;

/*
================
Sys_Cycles
================
*/
uint32 Sys_Cycles()
{
	LARGE_INTEGER PC;

	QueryPerformanceCounter(&PC);

	return PC.QuadPart;
}


/*
================
Sys_MSPerCycle
================
*/
double Sys_MSPerCycle()
{
	return sys_msPerCycle;
}

bool TimeInitiated = false;

// Timer class
void CTimer::Start ()
{
	if (!TimeInitiated)
	{
		// Make sure the timer is high precision, otherwise NT gets 18ms resolution
		timeBeginPeriod(1);
		sys_timeBase = timeGetTime() & 0xffff0000;

		// This is later multiplied times a cycle duration to get the CPU time in MS
		{
			LARGE_INTEGER PF;
			QueryPerformanceFrequency(&PF);
			sys_msPerCycle = (1.0 / (double)PF.QuadPart) * 1000.0;
		}

		TimeInitiated = true;
	}

	StartCycles = Sys_Cycles();
};

double CTimer::Get ()
{
	// Grab the value
	double value = (Sys_Cycles()-StartCycles) * Sys_MSPerCycle();

	// Restart the timer
	Start ();

	return value;
};

void *CDynamicLibrary::OS_LoadLibrary (const char *FileName)
{
	return LoadLibraryA (FileName);
};

void CDynamicLibrary::OS_CloseLibrary ()
{
	FreeLibrary ((HMODULE)Lib);
};

void *CDynamicLibrary::OS_GetProcAddress (const char *Symbol)
{
	return GetProcAddress((HMODULE)Lib, Symbol);
};

#endif
