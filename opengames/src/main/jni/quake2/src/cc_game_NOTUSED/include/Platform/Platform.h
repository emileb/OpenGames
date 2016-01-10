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
// cc_platform.h
// Main header file. Contains the common definitions.
//

#if !defined(CC_GUARD_PLATFORM_H) || !INCLUDE_GUARDS
#define CC_GUARD_PLATFORM_H

void Sys_FindFiles (TFindFilesType &files, const String &path, const String &pattern, bool recurse, bool addFiles, bool addDirs);
void CC_OutputDebugString (const char *text);
void CC_ReportGameError (const char *text);

// A class to simplify calls that will be ultimately different on
// OSes
#define TIMER_STRING "%.2fms"

class CTimer
{
public:
	double StartCycles;

	CTimer (bool StartNow = true)
	{
		if (StartNow)
			Start ();
	};

	void Start ();
	double Get (); // Automatically resets the timer as well
};

// Class to load and use dynamic libraries in respective operating systems
class CDynamicLibrary
{
private:
	String			LoadedLibFileName;
	void			*Lib;

	void	*OS_LoadLibrary (const char *FileName);
	void	OS_CloseLibrary ();
	void	*OS_GetProcAddress (const char *Symbol);

public:
	CDynamicLibrary ()
	{
	}; // Can be loaded empty

	CDynamicLibrary (const char *FileName)
	{
		Load (FileName);
	};

	virtual ~CDynamicLibrary ()
	{
		Close ();
	};

	// Returns true if the lib is valid and loaded.
	inline bool Valid ()
	{
		return (!LoadedLibFileName.IsNullOrEmpty() && Lib);
	};

	inline String GetLoadedFileName ()
	{
		return LoadedLibFileName;
	};

	inline bool Load (const char *FileName)
	{
		Lib = OS_LoadLibrary (FileName);
		
		if (!Lib)
			return false;
	
		LoadedLibFileName = FileName;
		return true;
	};

	inline void Close ()
	{
		if (!Lib)
			return;

		OS_CloseLibrary ();
		Lib = NULL;
		LoadedLibFileName.Clear();
	};

	template <typename TFuncType>
	inline TFuncType GetFunction (const char *Symbol)
	{
		return (TFuncType) OS_GetProcAddress(Symbol);
	};
};

#else
FILE_WARNING
#endif

