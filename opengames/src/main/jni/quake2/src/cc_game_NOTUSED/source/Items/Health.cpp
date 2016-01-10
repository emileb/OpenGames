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
// cc_health.cpp
// New, improved, better, stable item system!
//

#include "Local.h"
#include "Items/Items.h"

CHealth::CHealth (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   sint32 Amount, EHealthFlags HealthFlags) :
IBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags),
Amount(Amount),
HealthFlags(HealthFlags)
{
};

bool CHealth::Pickup (CItemEntity *Item, CPlayerEntity *Other)
{
	if (!(HealthFlags & HEALTHFLAG_IGNOREMAX) && (Other->Health >= Other->MaxHealth))
		return false;

#if CLEANCTF_ENABLED
//ZOID
	if (Other->Health >= 250 && Amount > 25)
		return false;
//ZOID
#endif

	Other->Health += Amount;

#if CLEANCTF_ENABLED
//ZOID
	if (Other->Health > 250 && Amount > 25)
		Other->Health = 250;
//ZOID
#endif

	if (!(HealthFlags & HEALTHFLAG_IGNOREMAX))
	{
		if (Other->Health > Other->MaxHealth)
			Other->Health = Other->MaxHealth;
	}

	if (!(Item->SpawnFlags & DROPPED_ITEM) && (Game.GameMode & GAME_DEATHMATCH))
		SetRespawn (Item, 300);

	return true;
}

class CHealthEntity : public CItemEntity
{
public:
	CHealthEntity() :
	  IBaseEntity(),
	  CItemEntity ()
	  {
	  };

	CHealthEntity (sint32 Index) :
	  IBaseEntity(Index),
	  CItemEntity (Index)
	  {
	  };

	void Spawn (IBaseItem *item)
	{
		if ((Game.GameMode & GAME_DEATHMATCH) && DeathmatchFlags.dfNoHealth.IsEnabled())
		{
			Free ();
			return;
		}

		LinkedItem = item;
		NextThink = Level.Frame + 2;    // items start after other solids
		ThinkState = ITS_DROPTOFLOOR;
		PhysicsType = PHYSICS_NONE;

		State.GetEffects() = item->EffectFlags;
		State.GetRenderEffects() = RF_GLOW | RF_IR_VISIBLE;
	};
};

LINK_ITEM_TO_CLASS (item_health_small, CHealthEntity);
LINK_ITEM_TO_CLASS (item_health, CHealthEntity);
LINK_ITEM_TO_CLASS (item_health_large, CHealthEntity);

void AddHealthToList ()
{
	NItems::StimPack = QNew (TAG_GENERIC) CHealth("item_health_small", "models/items/healing/stimpack/tris.md2", 0, "items/s_health.wav", "i_health", "Stimpack", ITEMFLAG_HEALTH|ITEMFLAG_GRABBABLE, 2, HEALTHFLAG_IGNOREMAX);
	NItems::SmallHealth = QNew (TAG_GENERIC) CHealth("item_health", "models/items/healing/medium/tris.md2", 0, "items/n_health.wav", "i_health", "Medium Health", ITEMFLAG_HEALTH|ITEMFLAG_GRABBABLE, 10, HEALTHFLAG_NONE);
	NItems::LargeHealth = QNew (TAG_GENERIC) CHealth("item_health_large", "models/items/healing/large/tris.md2", 0, "items/l_health.wav", "i_health", "Large Health", ITEMFLAG_HEALTH|ITEMFLAG_GRABBABLE, 25, HEALTHFLAG_NONE);
}
