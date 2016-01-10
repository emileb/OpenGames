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
// cc_items.h
// New, improved, better, stable item system!
//

#if !defined(CC_GUARD_ITEMS_H) || !INCLUDE_GUARDS
#define CC_GUARD_ITEMS_H

void InvalidateItemMedia ();

class IBaseItem
{
	friend void InvalidateItemMedia ();
private:
	// The index of this item in the item list
	uint8			Index;

	// Must be friends with itemlist so it can set the item.
	friend class CItemList;
public:
	IBaseItem (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags);

	IBaseItem ();

	// Classname (for maps)
	const char		*Classname;

	// World model
	const char		*WorldModel;

	// Effect flags (FX_ROTATE, etc)
	sint32			EffectFlags;

	// The sound on pickup
	const char		*PickupSound;

private:
	MediaIndex	PickupSoundIndex;

public:
	inline MediaIndex GetPickupSound ()
	{
		return (PickupSound && !PickupSoundIndex) ? (PickupSoundIndex = SoundIndex(PickupSound)) : PickupSoundIndex;
	}

	/// HUD Icon
	const char		*Icon;

private:
	MediaIndex	IconIndex;

public:
	inline MediaIndex GetIconIndex ()
	{
		return (Icon && !IconIndex) ? (IconIndex = ImageIndex(Icon)) : IconIndex;
	}

	// Name on pickup
	const char		*Name;

	// Item flags
	EItemFlags	Flags;

	// \brief	Attempts to pickup the item. 
	virtual	bool	Pickup (class CItemEntity *Item, CPlayerEntity *Other) = 0;

	// Attempts to uses the item. 
	virtual	void	Use (CPlayerEntity *Player) = 0;

	// Attempts to drops the item. 
	virtual	void	Drop (CPlayerEntity *Player) = 0;
	virtual class CItemEntity	*DropItem (IBaseEntity *Entity);

	virtual void	SetRespawn (class CItemEntity *Item, FrameNumber delay);

	inline uint8		GetIndex ()
	{
		return Index;
	};
	inline sint32		GetConfigStringNumber ()
	{
		return CS_ITEMS+Index;
	};

	virtual void	Add (CPlayerEntity *Player, sint32 quantity);
};

#include "Items/Weapons.h"
#include "Items/Health.h"
#include "Items/Armor.h"
#include "Items/Keys.h"
#include "Items/Powerups.h"

#if CLEANCTF_ENABLED
#include "CTF/CtfItems.h"
#endif
#include "Items/Techs.h"
#if XATRIX_FEATURES
#include "Xatrix/XatrixItems.h"
#endif
#if ROGUE_FEATURES
#include "Rogue/RogueItems.h"
#endif

#include "Items/Itemlist.h"
#include "Entities/ItemEntity.h"

#else
FILE_WARNING
#endif
