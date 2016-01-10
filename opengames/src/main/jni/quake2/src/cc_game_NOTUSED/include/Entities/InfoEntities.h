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
// cc_info_entities.h
// Info_ and some Target_ entities
//

#if !defined(CC_GUARD_CC_INFOENTITIES_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_INFOENTITIES_H

class CPathCorner : public IMapEntity, public IThinkableEntity, public ITouchableEntity, public IUsableEntity
{
public:
	TTargetList		NextTargets;
	FrameNumber		Wait;
#if ROGUE_FEATURES
	float			Accel, Decel, Speed;
#endif

	CPathCorner ();
	CPathCorner (sint32 Index);

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CPathCorner)

	virtual void Think ();
	virtual void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);
	virtual void Spawn ();
};

class CSpotBase : public IMapEntity
{
public:
	CSpotBase ();
	CSpotBase(sint32 Index);

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return IMapEntity::ParseField (Key, Value);
	};

	virtual void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
	};

	virtual void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
	};

	virtual void Spawn ();
};

class CPlayerIntermission : public CSpotBase
{
public:
	typedef std::vector<CPlayerIntermission*> TSpawnPointsType;
	static inline TSpawnPointsType &SpawnPoints ()
	{
		static TSpawnPointsType Points;
		return Points;
	}

	static void ClearSpawnPoints ()
	{
		SpawnPoints().clear();
	}

	CPlayerIntermission ();
	CPlayerIntermission (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CPlayerIntermission)

	void Spawn ();
};

class CPlayerStart : public CSpotBase, public IThinkableEntity
{
public:
	typedef std::vector<CPlayerStart*> TSpawnPointsType;
	static inline TSpawnPointsType &SpawnPoints ()
	{
		static TSpawnPointsType Points;
		return Points;
	}

	static void ClearSpawnPoints ()
	{
		SpawnPoints().clear();
	}

	CPlayerStart ();
	CPlayerStart (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CPlayerStart)

	void SaveFields (CFile &File);
	void LoadFields (CFile &File);

	bool ParseField (const char *Key, const char *Value);

	// some maps don't have any coop spots at all, so we need to create them
	// where they should have been
	virtual void Think ();

	virtual void Spawn ();
};

class CPlayerCoop : public CSpotBase, public IThinkableEntity
{
public:
	typedef std::vector<CPlayerCoop*> TSpawnPointsType;
	static inline TSpawnPointsType &SpawnPoints ()
	{
		static TSpawnPointsType Points;
		return Points;
	}

	static void ClearSpawnPoints ()
	{
		SpawnPoints().clear();
	}

	CPlayerCoop ();
	CPlayerCoop (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CPlayerCoop)

	void SaveFields (CFile &File);
	void LoadFields (CFile &File);

	bool ParseField (const char *Key, const char *Value);

	// this function is an ugly as hell hack to fix some map flaws
	//
	// the coop spawn spots on some maps are SNAFU.  There are coop spots
	// with the wrong targetname as well as spots with no name at all
	//
	// we use carnal knowledge of the maps to fix the coop spot targetnames to match
	// that of the nearest named single player spot
	virtual void Think ();

	virtual void Spawn ();
};

class CPlayerDeathmatch : public CSpotBase
{
public:
	typedef std::vector<CPlayerDeathmatch*> TSpawnPointsType;
	static inline TSpawnPointsType &SpawnPoints ()
	{
		static TSpawnPointsType Points;
		return Points;
	}

	static void ClearSpawnPoints ()
	{
		SpawnPoints().clear();
	}

	CPlayerDeathmatch ();
	CPlayerDeathmatch (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CPlayerDeathmatch)

	void LoadFields (CFile &File);

	virtual void Spawn ();
};

#if ROGUE_FEATURES
class CPlayerCoopLava : public CSpotBase
{
public:
	typedef std::vector<CPlayerCoopLava*> TSpawnPointsType;
	static inline TSpawnPointsType &SpawnPoints ()
	{
		static TSpawnPointsType Points;
		return Points;
	}

	static void ClearSpawnPoints ()
	{
		SpawnPoints().clear();
	}

	CPlayerCoopLava ();
	CPlayerCoopLava (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CPlayerCoopLava)

	void SaveFields (CFile &File);
	void LoadFields (CFile &File);

	bool ParseField (const char *Key, const char *Value);

	void Spawn ();
};
#endif

#else
FILE_WARNING
#endif