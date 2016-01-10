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
// cc_arg.cpp
// Wrapper for the argv/s/c system
//

#include "Local.h"

const int MAX_ARG_CONCAT	= 256;
const int MAX_ARG			= 64;

uint8							numArgv;
String						argvConcatString;
std::vector<String>		argvStringArray;
std::vector<sint32>				argvIntegerArray;
std::vector<float>				argvFloatArray;

void SetupArg ()
{
	numArgv = 0;

	argvStringArray.clear();
	argvIntegerArray.clear();
	argvFloatArray.clear();
	argvConcatString.Clear();
}

// Called at the beginning of an arg session
void InitArg ()
{
CC_DISABLE_DEPRECATION
	if (numArgv)
	{
		DebugPrint ("CleanCode Warning: InitArg() called before previous arg session completed!\n");
		EndArg ();
	}

	numArgv = (uint8)gi.argc();

	if (numArgv == 0)
		return;

	argvConcatString = String(gi.args());

	for (uint8 i = 0; i < numArgv; i++)
	{
		argvStringArray.push_back (String(gi.argv(i)));
		argvFloatArray.push_back ((float)atof (argvStringArray[i].CString()));
		argvIntegerArray.push_back ((sint32)argvFloatArray[i]);
	}
CC_ENABLE_DEPRECATION
}

// Called at the end of an arg session
void EndArg ()
{
	SetupArg();
}

// Actual functions
uint8 ArgCount ()
{
	return numArgv;
}

String ArgGetConcatenatedString ()
{
	return argvConcatString;
}

String ArgGets (uint32 Index)
{
	if (Index >= argvStringArray.size())
		return String::Empty();

	return argvStringArray[Index];
}

sint32 ArgGeti (uint32 Index)
{
	if (CC_ASSERT_EXPR (!(Index >= argvStringArray.size()), "ArgGeti(n) index out of bounds"))
		return 0;

	return argvIntegerArray[Index];
}

float ArgGetf (uint32 Index)
{
	if (CC_ASSERT_EXPR (!(Index >= argvStringArray.size()), "ArgGetf(n) index out of bounds"))
		return 0;

	return argvFloatArray[Index];
}
