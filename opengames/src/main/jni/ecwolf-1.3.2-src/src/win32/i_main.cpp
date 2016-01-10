/*
** i_main.cpp
** System-specific startup code. Uses much code from ZDoom:
**
**---------------------------------------------------------------------------
** Copyright 1998-2009 Randy Heit
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
*/

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <wincrypt.h>
#include <commctrl.h>
#include <cstdlib>
#include <ctime>
#include <SDL_syswm.h>

#define USE_WINDOWS_DWORD
#include "version.h"
#include "w_wad.h"
#include "zstring.h"

HINSTANCE		g_hInst;
HWND			ConWindow;
HANDLE			MainThread;
DWORD			MainThreadID;

extern EXCEPTION_POINTERS CrashPointers;
void CreateCrashLog (char *custominfo, DWORD customsize, HWND richedit);
void DisplayCrashLog ();

//==========================================================================
//
// DoomSpecificInfo
//
// Called by the crash logger to get application-specific information.
//
//==========================================================================

void DoomSpecificInfo (char *buffer, size_t bufflen)
{
	const char *arg;
	char *const buffend = buffer + bufflen - 2;	// -2 for CRLF at end
	int i;

	buffer += mysnprintf (buffer, buffend - buffer, GAMENAME " version " DOTVERSIONSTR " (" __DATE__ ")\r\n");
	buffer += mysnprintf (buffer, buffend - buffer, "\r\nCommand line: %s\r\n", GetCommandLine());

	for (i = 0; (arg = Wads.GetWadName (i)) != NULL; ++i)
	{
		buffer += mysnprintf (buffer, buffend - buffer, "\r\nWad %d: %s", i, arg);
	}

#if 0
	if (gamestate != GS_LEVEL && gamestate != GS_TITLELEVEL)
	{
		buffer += mysnprintf (buffer, buffend - buffer, "\r\n\r\nNot in a level.");
	}
	else
	{
		char name[9];

		strncpy (name, level.mapname, 8);
		name[8] = 0;
		buffer += mysnprintf (buffer, buffend - buffer, "\r\n\r\nCurrent map: %s", name);

		if (!viewactive)
		{
			buffer += mysnprintf (buffer, buffend - buffer, "\r\n\r\nView not active.");
		}
		else
		{
			buffer += mysnprintf (buffer, buffend - buffer, "\r\n\r\nviewx = %d", viewx);
			buffer += mysnprintf (buffer, buffend - buffer, "\r\nviewy = %d", viewy);
			buffer += mysnprintf (buffer, buffend - buffer, "\r\nviewz = %d", viewz);
			buffer += mysnprintf (buffer, buffend - buffer, "\r\nviewangle = %x", viewangle);
		}
	}
#endif
	*buffer++ = '\r';
	*buffer++ = '\n';
	*buffer++ = '\0';
}

// Here is how the error logging system works.
//
// To catch exceptions that occur in secondary threads, CatchAllExceptions is
// set as the UnhandledExceptionFilter for this process. It records the state
// of the thread at the time of the crash using CreateCrashLog and then queues
// an APC on the primary thread. When the APC executes, it raises a software
// exception that gets caught by the __try/__except block in WinMain.
// I_GetEvent calls SleepEx to put the primary thread in a waitable state
// periodically so that the APC has a chance to execute.
//
// Exceptions on the primary thread are caught by the __try/__except block in
// WinMain. Not only does it record the crash information, it also shuts
// everything down and displays a dialog with the information present. If a
// console log is being produced, the information will also be appended to it.
//
// If a debugger is running, CatchAllExceptions never executes, so secondary
// thread exceptions will always be caught by the debugger. For the primary
// thread, IsDebuggerPresent is called to determine if a debugger is present.
// Note that this function is not present on Windows 95, so we cannot
// statically link to it.
//
// To make this work with MinGW, you will need to use inline assembly
// because GCC offers no native support for Windows' SEH.

//==========================================================================
//
// SleepForever
//
//==========================================================================

void SleepForever ()
{
	Sleep (INFINITE);
}

//==========================================================================
//
// ExitMessedUp
//
// An exception occurred while exiting, so don't do any standard processing.
// Just die.
//
//==========================================================================

LONG WINAPI ExitMessedUp (LPEXCEPTION_POINTERS foo)
{
	ExitProcess (1000);
}

//==========================================================================
//
// ExitFatally
//
//==========================================================================

void CALLBACK ExitFatally (ULONG_PTR dummy)
{
	SetUnhandledExceptionFilter (ExitMessedUp);
	DisplayCrashLog ();
	exit(-1);
}

//==========================================================================
//
// CatchAllExceptions
//
//==========================================================================

LONG WINAPI CatchAllExceptions (LPEXCEPTION_POINTERS info)
{
#ifdef _DEBUG
	if (info->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
#endif

	static bool caughtsomething = false;

	if (caughtsomething) return EXCEPTION_EXECUTE_HANDLER;
	caughtsomething = true;

	char *custominfo = (char *)HeapAlloc (GetProcessHeap(), 0, 16384);

	CrashPointers = *info;
	DoomSpecificInfo (custominfo, 16384);
	CreateCrashLog (custominfo, (DWORD)strlen(custominfo), ConWindow);

	// If the main thread crashed, then make it clean up after itself.
	// Otherwise, put the crashing thread to sleep and signal the main thread to clean up.
	if (GetCurrentThreadId() == MainThreadID)
	{
#ifndef _M_X64
		info->ContextRecord->Eip = (DWORD_PTR)ExitFatally;
#else
		info->ContextRecord->Rip = (DWORD_PTR)ExitFatally;
#endif
	}
	else
	{
#ifndef _M_X64
		info->ContextRecord->Eip = (DWORD_PTR)SleepForever;
#else
		info->ContextRecord->Rip = (DWORD_PTR)SleepForever;
#endif
		QueueUserAPC (ExitFatally, MainThread, 0);
	}
	return EXCEPTION_CONTINUE_EXECUTION;
}

void StartupWin32()
{
	// Get console window and hinstance.
	char title[512];
	GetConsoleTitleA(title, 512);
	ConWindow = FindWindow(NULL, title);
	g_hInst = NULL;
	// Apparently the console window will prevent windows from being created if this doesn't return 0.
	//g_hInst = (HINSTANCE)GetWindowLong(ConWindow, GWL_HINSTANCE);

	InitCommonControls ();			// Load some needed controls and be pretty under XP
	// We need to load riched20.dll so that we can create the control.
	if (NULL == LoadLibrary ("riched20.dll"))
	{
		// This should only happen on basic Windows 95 installations, but since we
		// don't support Windows 95, we have no obligation to provide assistance in
		// getting it installed.
		MessageBoxA(NULL, "Could not load riched20.dll", "ZDoom Error", MB_OK | MB_ICONSTOP);
		exit(0);
	}

	MainThread = INVALID_HANDLE_VALUE;
	DuplicateHandle (GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &MainThread,
		0, FALSE, DUPLICATE_SAME_ACCESS);
	MainThreadID = GetCurrentThreadId();

#ifndef _DEBUG
	if (MainThread != INVALID_HANDLE_VALUE)
	{
		SetUnhandledExceptionFilter(CatchAllExceptions);
	}
#endif
}

bool CheckIsRunningFromCommandPrompt()
{
	HANDLE stdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	if(!GetConsoleScreenBufferInfo(stdOutput, &info))
		return false;
	return info.dwCursorPosition.X != 0 || info.dwCursorPosition.Y != 0;
}

#if SDL_VERSION_ATLEAST(2,0,0)
// If we have a console application then the console will be on a separate thread.
// What this means that when the IWAD picker finishes its job, the focus will go to
// the console thread and not to the SDL window we eventually create. We'll call this
// function to bring it into focus.
// https://forums.libsdl.org/viewtopic.php?p=42799
void ForceSDLFocus(SDL_Window *win)
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (!SDL_GetWindowWMInfo(win, &info))
		Printf("Failed to focus window.\n");
	SetForegroundWindow(info.info.win.window);
}
#endif

//==========================================================================
//
// I_MakeRNGSeed
//
// Returns a 32-bit random seed, preferably one with lots of entropy.
//
/*
** i_system.cpp
** Timers, pre-console output, IWAD selection, and misc system routines.
**
**---------------------------------------------------------------------------
** Copyright 1998-2009 Randy Heit
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/
//
//==========================================================================

unsigned int I_MakeRNGSeed()
{
	unsigned int seed;

	// If RtlGenRandom is available, use that to avoid increasing the
	// working set by pulling in all of the crytographic API.
	HMODULE advapi = GetModuleHandle("advapi32.dll");
	if (advapi != NULL)
	{
		BOOLEAN (APIENTRY *RtlGenRandom)(void *, ULONG) =
			(BOOLEAN (APIENTRY *)(void *, ULONG))GetProcAddress(advapi, "SystemFunction036");
		if (RtlGenRandom != NULL)
		{
			if (RtlGenRandom(&seed, sizeof(seed)))
			{
				return seed;
			}
		}
	}

	// Use the full crytographic API to produce a seed. If that fails,
	// time() is used as a fallback.
	HCRYPTPROV prov;

	if (!CryptAcquireContext(&prov, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		return (unsigned int)time(NULL);
	}
	if (!CryptGenRandom(prov, sizeof(seed), (BYTE *)&seed))
	{
		seed = (unsigned int)time(NULL);
	}
	CryptReleaseContext(prov, 0);
	return seed;
}
