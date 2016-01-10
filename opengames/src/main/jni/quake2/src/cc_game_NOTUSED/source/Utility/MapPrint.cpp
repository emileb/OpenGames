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
// cc_mapprint.cpp
// Contains a system to more easily-er warn about map errors
//

#include "Local.h"

static sint32 mapWarnings, mapErrors;

void InitMapCounter ()
{
	mapWarnings = mapErrors = 0;
	ServerPrint ("======================\nSpawning entities...\n");
}

void EndMapCounter ()
{
	ServerPrintf ("...entities spawned with %i error(s), %i warning(s).\n======================\n", mapErrors, mapWarnings);
}

static String PrintBuffer;

void Map_Print (EMapPrintType printType, vec3f &origin)
{
	switch (printType)
	{
	case MAPPRINT_WARNING:
		mapWarnings++;
		PrintBuffer += String::Format("Warning %i (%s #%i)", mapWarnings, Level.ClassName.CString(), Level.EntityNumber);
		break;
	case MAPPRINT_ERROR:
		mapErrors++;
		PrintBuffer += String::Format("Error %i (%s #%i)", mapErrors, Level.ClassName.CString(), Level.EntityNumber);
		break;
	default:
		PrintBuffer += String::Format("Message (%s #%i)", Level.ClassName.CString(), Level.EntityNumber);
		break;
	}

	if (origin)
		PrintBuffer += String::Format(" @ (%f %f %f)", origin.X, origin.Y, origin.Z);
	PrintBuffer += ":\n";
}

void MapPrint (EMapPrintType printType, IBaseEntity *Entity, vec3f &origin, const char *fmt, ...)
{
	if (Entity && !Entity->ClassName.IsNullOrEmpty() && Level.ClassName != Entity->ClassName)
		Level.ClassName = Entity->ClassName;

	Map_Print (printType, origin);

	va_list		argptr;
	CTempMemoryBlock text = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, fmt);
	vsnprintf (text.GetBuffer<char>(), text.GetSize() - 1, fmt, argptr);
	va_end (argptr);

	PrintBuffer += String::Format("   %s", text.GetBuffer<char>());
	ServerPrintf ("%s", PrintBuffer.CString());

	PrintBuffer.Clear();
}

// To speed the process up a bit, and to reduce problematic newlines,
// this goes by entire lines.
char *CC_ParseSpawnEntities (char *ServerLevelName, char *entities)
{
	CFileBuffer FileBuffer(String::Format("maps/ents/%s.ccent", ServerLevelName).CString(), true);
	String finalString;
	char *realEntities;

	if (!FileBuffer.Valid())
		return entities;
	else
		entities = FileBuffer.GetBuffer<char> ();

	realEntities = entities;
	finalString = realEntities;

	/*while ((token = CC_ParseLine(&realEntities)) != NULL)
	{
		if (token == NULL)
			break; // ever happen?

		if (token[0] == '#' && (realEntities = ParsePound(token, realEntities)) != NULL)
			continue;

		// Basically if we reach here, this part makes it into the final compilation.
		finalString += std::string(token) + "\n";

		if (TokenEnd(token))
			break;
	}*/

	return Mem_TagStrDup (finalString.CString(), TAG_LEVEL);
}
