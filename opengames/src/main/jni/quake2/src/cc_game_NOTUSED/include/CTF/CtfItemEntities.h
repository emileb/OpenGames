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
// cc_ctfitementities.h
// CTF Item Entities
//

#if !defined(CC_GUARD_CTFITEMENTITIES_H) || !INCLUDE_GUARDS
#define CC_GUARD_CTFITEMENTITIES_H

#if CLEANCTF_ENABLED

// Forward declaration
class CFlagEntity;

// A simple class to track a single flag.
class CFlagTransponder
{
public:
	CFlagTransponder (const ETeamIndex Team, CFlagEntity *Flag);

	/**
	\typedef	uint8 EFlagLocation
	
	\brief	Defines an alias representing a flag's location.
	**/
	typedef uint8 EFlagLocation;

	/**
	\enum	
	
	\brief	Values that represent a flag's location. 
	**/
	enum
	{
		FLAG_UNKNOWN,
		FLAG_AT_BASE,
		FLAG_TAKEN,
		FLAG_DROPPED,
	};

	ETeamIndex			Team;		// Team that this transponder's flag belongs to.
	EFlagLocation		Location;	// Where are we right now?
	class CFlagEntity	*Base;		// Entity pointing to the flag's base. Never NULL.
	class CFlagEntity	*Flag;		// Entity pointing to the current location of the actual flag.
	class CPlayerEntity	*Holder;	// Entity pointing to the current person holding the flag, if any.
};

CFlagTransponder *FindTransponder (ETeamIndex Team);

enum
{
	FTS_FLAGSETUP = ITS_CUSTOM,
	FTS_FLAGTHINK,
};

class CFlagEntity : public CItemEntity
{
public:
	CFlagTransponder	*Transponder;

	CFlagEntity ();
	CFlagEntity (sint32 Index);

	void Think ();

	bool Run ();
	virtual void BecomeExplosion (bool grenade) {};

	virtual void Spawn (IBaseItem *item, ETeamIndex Team);
	virtual void Spawn (IBaseItem *Item) {};
};

class CRedFlagEntity : public CFlagEntity
{
public:
	CRedFlagEntity ();
	CRedFlagEntity (sint32 Index);

	bool Run ();
	void BecomeExplosion (bool grenade);

	void Spawn (IBaseItem *Item)
	{
		CFlagEntity::Spawn (Item, CTF_TEAM1);
	};
};

class CBlueFlagEntity : public CFlagEntity
{
public:
	CBlueFlagEntity ();
	CBlueFlagEntity (sint32 Index);

	bool Run ();
	void BecomeExplosion (bool grenade);

	void Spawn (IBaseItem *Item)
	{
		CFlagEntity::Spawn (Item, CTF_TEAM2);
	};
};

#endif

#else
FILE_WARNING
#endif
