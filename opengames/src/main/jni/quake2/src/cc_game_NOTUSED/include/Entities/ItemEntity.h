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
// cc_itementity.h
// The entity that lets you pick items up.
// Moved here because it's seperate from the item system.
//

#if !defined(CC_GUARD_ITEMENTITY_H) || !INCLUDE_GUARDS
#define CC_GUARD_ITEMENTITY_H

// item spawnflags
/**
\enum	

\brief	Values that represent spawnflags pertaining to CItemEntity. 
**/
enum
{
	ITEM_TRIGGER_SPAWN			= BIT(0),
	ITEM_NO_TOUCH				= BIT(1),

	DROPPED_ITEM				= BIT(16),
	DROPPED_PLAYER_ITEM			= BIT(17),
	ITEM_TARGETS_USED			= BIT(18)
};

/**
\typedef	uint8 EItemThinkState

\brief	Defines an alias representing think state of an item.
**/
typedef uint8 EItemThinkState;

/**
\enum	

\brief	Values that represent item think states.
		Everything from ITS_NONE to ITS_CUSTOM is reserved.
**/
enum
{
	ITS_NONE,
	ITS_DROPTOFLOOR,
	ITS_RESPAWN,
	ITS_FREE,

	ITS_CUSTOM
};

// Item entity
class CItemEntity : public IMapEntity, public ITossProjectile, public ITouchableEntity, public IThinkableEntity, public IUsableEntity
{
public:
	String				Model;
	uint32				AmmoCount;
	EItemThinkState		ThinkState;
	bool				ShouldRespawn;
	IBaseItem			*LinkedItem;

	CItemEntity ();
	CItemEntity (sint32 Index);

	virtual void Spawn (IBaseItem *item);

	ENTITYFIELD_DEFS
	void SaveFields (CFile &File);
	void LoadFields (CFile &File);
	const char *SAVE_GetName () { return LinkedItem->Classname; }

	// Returns a random team member of ent
	CItemEntity *GetRandomTeamMember (CItemEntity *Master);

	virtual void Think ();
	virtual void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator);

	bool Run ();

	void Spawn ();
};

class CAmmoEntity : public CItemEntity
{
public:
	CAmmoEntity();
	CAmmoEntity (sint32 Index);

	void Spawn (IBaseItem *item);
};

class CPowerupEntity : public CItemEntity
{
public:
	CPowerupEntity();
	CPowerupEntity (sint32 Index);

	void Spawn (IBaseItem *item);
};

#define LINK_ITEM_TO_CLASS(mapClassName,DLLClassName) \
	IMapEntity *LINK_RESOLVE_CLASSNAME(mapClassName, _Spawn) (sint32 Index) \
	{ \
		DLLClassName *newClass = QNewEntityOf DLLClassName(Index); \
		IBaseItem *Item = FindItemByClassname(#mapClassName); \
		if (Item) \
		{	\
			newClass->ParseFields (); \
			\
			if (newClass->CheckValidity()) \
				newClass->Spawn (Item); \
		}	\
		return newClass; \
	} \
	CClassnameToClassIndex LINK_RESOLVE_CLASSNAME(mapClassName, _Linker) \
	(LINK_RESOLVE_CLASSNAME(mapClassName, _Spawn), #mapClassName); \
	IMPLEMENT_SAVE_STRUCTURE (mapClassName,DLLClassName)

#define LINK_EXISTING_ITEM_TO_NEW_CLASS(DLLItemName, mapClassName,DLLClassName) \
	IMapEntity *LINK_RESOLVE_CLASSNAME(DLLItemName, _Spawn) (sint32 Index) \
	{ \
		DLLClassName *newClass = QNewEntityOf DLLClassName(Index); \
		IBaseItem *Item = FindItemByClassname(mapClassName); \
		if (Item) \
		{	\
			newClass->ParseFields (); \
			\
			if (newClass->CheckValidity()) \
				newClass->Spawn (Item); \
		}	\
		return newClass; \
	} \
	CClassnameToClassIndex LINK_RESOLVE_CLASSNAME(DLLItemName, _Linker) \
	(LINK_RESOLVE_CLASSNAME(DLLItemName, _Spawn), #DLLItemName); \
	IMPLEMENT_SAVE_STRUCTURE (DLLItemName,DLLClassName)

#if CLEANCTF_ENABLED
#include "CTF/CtfItemEntities.h"
#endif

#else
FILE_WARNING
#endif
