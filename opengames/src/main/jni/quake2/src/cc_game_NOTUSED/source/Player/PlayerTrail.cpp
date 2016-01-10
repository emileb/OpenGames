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
// cc_player_trail.cpp
// 
//

#include "Local.h"

/*
==============================================================================

PLAYER TRAIL

==============================================================================

This is a circular list containing the a list of points of where
the player has been recently.  It is used by monsters for pursuit.
*/


const int TRAIL_LENGTH	= 24;

static CPlayerMarker	trail[TRAIL_LENGTH];
static int				trail_head;
static bool				trail_active = false;

inline int NextMarker (int marker)
{
	return ((marker) + 1) & (TRAIL_LENGTH - 1);
}

inline int PrevMarker (int marker)
{
	return ((marker) - 1) & (TRAIL_LENGTH - 1);
}

void PlayerTrail_Init ()
{
	if (Game.GameMode & GAME_DEATHMATCH)
		return;

	trail_head = 0;
	trail_active = true;
}

void PlayerTrail_Add (vec3f spot)
{
	if (!trail_active)
		return;

	trail[trail_head].Origin = spot;
	trail[trail_head].Timestamp = Level.Frame;
	trail[trail_head].Angles = (spot - trail[PrevMarker(trail_head)].Origin).ToYaw();

	trail_head = NextMarker(trail_head);
}

void PlayerTrail_New (vec3f spot)
{
	if (!trail_active)
		return;

	PlayerTrail_Init ();
	PlayerTrail_Add (spot);
}

CPlayerMarker *PlayerTrail_PickFirst (CMonsterEntity *Monster)
{
	if (!trail_active)
		return NULL;

	int marker = trail_head;

	for (int n = TRAIL_LENGTH; n; n--)
	{
		if(trail[marker].Timestamp <= Monster->Monster->TrailTime)
			marker = NextMarker(marker);
		else
			break;
	}

	if (IsVisible (Monster->State.GetOrigin() + vec3f(0, 0, Monster->ViewHeight), trail[marker].Origin, Monster))
		return &trail[marker];

	if (IsVisible (Monster->State.GetOrigin() + vec3f(0, 0, Monster->ViewHeight), trail[PrevMarker(marker)].Origin, Monster))
		return &trail[PrevMarker(marker)];

	return &trail[marker];
}

CPlayerMarker *PlayerTrail_PickNext (CMonsterEntity *Monster)
{
	if (!trail_active)
		return NULL;

	int marker = trail_head;
	for (int n = TRAIL_LENGTH; n; n--)
	{
		if (trail[marker].Timestamp <= Monster->Monster->TrailTime)
			marker = NextMarker(marker);
		else
			break;
	}

	return &trail[marker];
}

CPlayerMarker *PlayerTrail_LastSpot ()
{
	return &trail[PrevMarker(trail_head)];
}
