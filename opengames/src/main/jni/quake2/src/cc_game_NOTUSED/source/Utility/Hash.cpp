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
// cc_hash.cpp
// Duh
//

#include "Local.h"

/*
================
Com_HashGeneric
hashSize MUST be a power of two!

From EGL, used to make table lookups
a bit faster.
================
*/
uint32 Com_HashGeneric (const char *name, const sint32 hashSize)
{
	uint32 hashValue = 0;
	for ( ; *name; )
	{
		sint32 ch = Q_tolower(*(name++));
		hashValue = hashValue * 33 + ch;
	}

	return (hashValue + (hashValue >> 5)) & (hashSize-1);
}

uint32 Com_HashGeneric (const String &name, const sint32 hashSize)
{
	uint32 hashValue = 0;
	for (uint32 i = 0; name[i]; i++)
	{
		sint32 ch = Q_tolower(name[i]);
		hashValue = hashValue * 33 + ch;
	}

	return (hashValue + (hashValue >> 5)) & (hashSize-1);
}
