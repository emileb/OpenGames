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
// cc_trace.cpp
// A wrapper for the ugly gi.trace system.
//

#include "Local.h"

void CTrace::Copy (STrace tr)
{
	Fraction = tr.Fraction;
	Plane = tr.Plane;
	AllSolid = (tr.AllSolid != 0);
	StartSolid = (tr.StartSolid != 0);
	EndPosition = tr.EndPosition;
	Surface = tr.Surface;
	Contents = tr.Contents;
	Entity = (tr.Entity) ? tr.Entity->Entity : NULL;
};

CTrace::CTrace ()
{
}

CC_DISABLE_DEPRECATION

CTrace::CTrace (vec3f Start, vec3f Mins, vec3f Maxs, vec3f End, IBaseEntity *Ignore, EBrushContents ContentMask)
{
	operator() (Start, Mins, Maxs, End, Ignore, ContentMask);
};

CTrace::CTrace (vec3f Start, vec3f End, IBaseEntity *Ignore, EBrushContents ContentMask)
{
	operator() (Start, End, Ignore, ContentMask);
};

CTrace::CTrace (vec3f Start, vec3f End, EBrushContents ContentMask)
{
	operator() (Start, End, ContentMask);
}

void CTrace::operator () (vec3f Start, vec3f Mins, vec3f Maxs, vec3f End, IBaseEntity *Ignore, EBrushContents ContentMask)
{
	STrace temp = gi.trace(Start, Mins, Maxs, End, (Ignore) ? Ignore->GetGameEntity() : NULL, ContentMask);
	Copy(temp);
};

void CTrace::operator () (vec3f Start, vec3f End, IBaseEntity *Ignore, EBrushContents ContentMask)
{
	STrace temp = gi.trace(Start, vec3fOrigin, vec3fOrigin, End, (Ignore) ? Ignore->GetGameEntity() : NULL, ContentMask);
	Copy(temp);
};

void CTrace::operator () (vec3f Start, vec3f End, EBrushContents contentMask)
{
	STrace temp = gi.trace(Start, vec3fOrigin, vec3fOrigin, End, NULL, contentMask);
	Copy(temp);
}

CC_ENABLE_DEPRECATION

