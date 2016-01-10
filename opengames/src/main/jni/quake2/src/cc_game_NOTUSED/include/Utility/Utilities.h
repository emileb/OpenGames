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
// cc_utils.h
// g_utils, CleanCode style
//

#if !defined(CC_GUARD_UTILS_H) || !INCLUDE_GUARDS
#define CC_GUARD_UTILS_H

typedef std::vector<IBaseEntity*> TTargetList;
IBaseEntity *CC_PickTarget (String targetname);
TTargetList CC_GetTargets (String targetname);

template <class TEntityType, uint32 EntityFlags, size_t FieldOfs>
TEntityType *CC_Find (IBaseEntity *From, const char *Match)
{
	SEntity *gameEnt;
	if (!From)
		gameEnt = Game.Entities;
	else
	{
		gameEnt = From->GetGameEntity();
		gameEnt++;
	}

	for ( ; gameEnt < &Game.Entities[GameAPI.GetNumEdicts()]; gameEnt++)
	{
		if (!gameEnt->Entity || !gameEnt->Entity->GetInUse())
			continue;
		if (!(gameEnt->Entity->EntityFlags & EntityFlags))
			continue;

		// Special case
		// If we're only looking for EF_BASE
		// we don't need to cast just yet
		if (EntityFlags & EF_BASE)
		{
			String s = *(String *) ((uint8 *)gameEnt->Entity + FieldOfs);
			if (s.IsNullOrEmpty())
				continue;
			if (s == Match)
				return entity_cast<TEntityType>(gameEnt->Entity);
		}
		else
		{
			TEntityType *Check = entity_cast<TEntityType>(gameEnt->Entity);
			String s = *(String*) ((uint8 *)Check + FieldOfs);
			if (s.IsNullOrEmpty())
				continue;
			if (s == Match)
				return Check;
		}
	}

	return NULL;
}

template <class TEntityType, uint32 EntityFlags>
TEntityType *CC_FindByClassName (IBaseEntity *From, const char *Match)
{
	SEntity *gameEnt;
	if (!From)
		gameEnt = Game.Entities;
	else
	{
		gameEnt = From->GetGameEntity();
		gameEnt++;
	}

	for ( ; gameEnt < &Game.Entities[GameAPI.GetNumEdicts()]; gameEnt++)
	{
		if (!gameEnt->Entity || !gameEnt->Entity->GetInUse())
			continue;
		if (!(gameEnt->Entity->EntityFlags & EntityFlags))
			continue;

		if (gameEnt->Entity->ClassName.IsNullOrEmpty())
			continue;
		if (!Q_stricmp (gameEnt->Entity->ClassName.CString(), Match))
			return entity_cast<TEntityType>(gameEnt->Entity);
	}

	return NULL;
}

// stored in cc_infoentities
CSpotBase *SelectFarthestDeathmatchSpawnPoint ();
CSpotBase *SelectRandomDeathmatchSpawnPoint ();
float	PlayersRangeFromSpot (IBaseEntity *spot);

inline CSpotBase *SelectDeathmatchSpawnPoint ()
{
	return (DeathmatchFlags.dfSpawnFarthest.IsEnabled()) ? SelectFarthestDeathmatchSpawnPoint () : SelectRandomDeathmatchSpawnPoint ();
}

class CForEachTeamChainCallback
{
public:
	virtual void Callback (IBaseEntity *Entity) = 0;
	virtual void Query (IBaseEntity *Master);
};

class CForEachPlayerCallback
{
public:
	sint32		Index;

	virtual void Callback (CPlayerEntity *Player) = 0;
	virtual void Query (bool MustBeInUse = true);
};

class CForEachEntityCallback
{
public:
	virtual void Callback (IBaseEntity *Entity) = 0;
	virtual void Query (uint32 FlagsCanHave, uint32 FlagsCantHave = 0, bool MustBeInUse = true);
};

/*
class CMyPlayerCallback : public CForEachPlayerCallback
{
public:
	CMyPlayerCallback ()
	{
	};

	void Callback (CPlayerEntity *Player)
	{
	}
};
*/

inline float RangeFrom (vec3f left, vec3f right)
{
	return (left - right).Length();
}

inline ERangeType Range (vec3f left, vec3f right)
{
	float len = RangeFrom (left, right);
	if (len < MELEE_DISTANCE)
		return RANGE_MELEE;
	else if (len < 500)
		return RANGE_NEAR;
	else if (len < 1000)
		return RANGE_MID;
	return RANGE_FAR;
}

ERangeType Range (IBaseEntity *self, IBaseEntity *Other);
bool IsInFront (IBaseEntity *self, IBaseEntity *Other);
bool IsInBack (IBaseEntity *self, IBaseEntity *Other);
bool IsBelow (IBaseEntity *self, IBaseEntity *Other);
bool IsVisible (IBaseEntity *self, IBaseEntity *Other);
bool IsVisible (vec3f left, vec3f right, IBaseEntity *self);

void G_ProjectSource (const vec3f &Point, const vec3f &distance, const anglef angles, vec3f &result, bool up = false);

// Changed to sint32, rarely used as a float..
IBaseEntity *FindRadius (IBaseEntity *From, vec3f &org, sint32 Radius, uint32 EntityFlags, bool CheckNonSolid);

template <class ReturnType, uint32 EntityFlags>
ReturnType *FindRadius (IBaseEntity *From, vec3f &org, sint32 Radius, bool CheckNonSolid = true)
{
	return entity_cast<ReturnType>(FindRadius (From, org, Radius, EntityFlags, CheckNonSolid));
}

template <uint32 EntityFlags>
inline IBaseEntity *FindRadius (IBaseEntity *From, vec3f &org, sint32 Radius, bool CheckNonSolid = true)
{
	return FindRadius (From, org, Radius, EntityFlags, CheckNonSolid);
}

inline vec3f VelocityForDamage (sint32 Damage)
{
	return vec3f(100.0f * crand(), 100.0f * crand(), 200 + 100 * frand()) * ((Damage < 50) ? 0.7f : 1.2f);
}

#else
FILE_WARNING
#endif
