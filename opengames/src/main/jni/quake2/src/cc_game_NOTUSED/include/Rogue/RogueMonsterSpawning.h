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
// cc_rogue_monster_spawning.h
// 
//

#if !defined(CC_GUARD_CC_ROGUE_MONSTER_SPAWNING_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ROGUE_MONSTER_SPAWNING_H

class CReinforcement
{
public:
	const char	*Classname;
	vec3f		Mins, Maxs, Position;

	CReinforcement (const char *Classname, vec3f Mins, vec3f Maxs, vec3f Position) :
	  Classname(Classname),
	  Mins(Mins),
	  Maxs(Maxs),
	  Position(Position)
	  {
	  };
};

CMonsterEntity *CreateGroundMonster (vec3f origin, vec3f angles, vec3f entMins, vec3f entMaxs, const char *classname, int height);
CMonsterEntity *CreateFlyMonster (vec3f origin, vec3f angles, vec3f mins, vec3f maxs, char *classname);
void DetermineBBox (const char *classname, vec3f &mins, vec3f &maxs);
bool CheckGroundSpawnPoint (vec3f origin, vec3f entMins, vec3f entMaxs, float height, float gravity);
bool CheckSpawnPoint (vec3f origin, vec3f mins, vec3f maxs);
bool FindSpawnPoint (vec3f startpoint, vec3f mins, vec3f maxs, vec3f &spawnpoint, float maxMoveUp);
CMonsterEntity *CreateMonster(vec3f origin, vec3f angles, const char *classname);
void PredictAim (IBaseEntity *target, vec3f start, float bolt_speed, bool eye_height, float offset, vec3f *aimdir, vec3f *aimpoint);
CPlayerEntity *PickCoopTarget (IBaseEntity *Entity);
int CountPlayers ();

class CSpawnGrow : public IThinkableEntity
{
public:
	FrameNumber		Wait;

	CSpawnGrow ();
	CSpawnGrow (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CSpawnGrow)

	void SaveFields (CFile &File);
	void LoadFields (CFile &File);

	void Think ();

	static void Spawn (vec3f Origin, sint8 Size);
};

#else
FILE_WARNING
#endif