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
// cc_armor.cpp
// New, improved, better, stable item system!
//

#include "Local.h"
#include "Items/Items.h"


CArmor::CArmor (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   sint16 baseCount, sint16 maxCount, sint16 normalProtection,
			   sint16 energyProtection) :
IBaseItem(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags),
baseCount(baseCount),
maxCount(maxCount),
normalProtection(normalProtection),
energyProtection(energyProtection)
{
};

bool CArmor::Pickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	if (normalProtection == -1)
	{
		if (Other->Client.Persistent.Armor == NULL)
		{
			Other->Client.Persistent.Inventory.Set (NItems::JacketArmor, 2);
			Other->Client.Persistent.Armor = dynamic_cast<CArmor*>(NItems::JacketArmor);
		}
		else
		{
			if (maxCount != -1 && (Other->Client.Persistent.Inventory.Has(Other->Client.Persistent.Armor) >= maxCount))
				return false;

			Other->Client.Persistent.Inventory.Add (Other->Client.Persistent.Armor, 2);
			if (maxCount != -1 && (Other->Client.Persistent.Inventory.Has(Other->Client.Persistent.Armor) > maxCount))
				Other->Client.Persistent.Inventory.Set(Other->Client.Persistent.Armor, maxCount);
		}
	}
	else if (Other->Client.Persistent.Armor != NULL)
	{
		if (normalProtection > Other->Client.Persistent.Armor->normalProtection)
		{
			// calc new armor values
			sint32 newCount = baseCount + (((float)Other->Client.Persistent.Armor->normalProtection / (float)normalProtection) * Other->Client.Persistent.Inventory.Has(Other->Client.Persistent.Armor));
			if (newCount > maxCount)
				newCount = maxCount;

			// zero count of old armor so it goes away
			Other->Client.Persistent.Inventory.Set(Other->Client.Persistent.Armor, 0);

			// change armor to new item with computed value
			Other->Client.Persistent.Inventory.Set(this, newCount);
			Other->Client.Persistent.Armor = this;
		}
		else
		{
			// calc new armor values
			sint32 newCount = Other->Client.Persistent.Inventory.Has(Other->Client.Persistent.Armor) + (((float)normalProtection / (float)Other->Client.Persistent.Armor->normalProtection) * baseCount);
			if (newCount > Other->Client.Persistent.Armor->maxCount)
				newCount = Other->Client.Persistent.Armor->maxCount;

			// if we're already maxed out then we don't need the new armor
			if (Other->Client.Persistent.Inventory.Has(Other->Client.Persistent.Armor) >= newCount)
				return false;

			// update current armor value
			Other->Client.Persistent.Inventory.Set(Other->Client.Persistent.Armor, newCount);
		}
	}
	// Player has no other armor, just use it
	else
	{
		Other->Client.Persistent.Armor = this;
		Other->Client.Persistent.Inventory.Set(this, baseCount);
	}

	if (!(Item->SpawnFlags & DROPPED_ITEM) && (Game.GameMode & GAME_DEATHMATCH))
		SetRespawn (Item, 200);

	return true;
}

// No dropping or using armor.
void CArmor::Use(CPlayerEntity *Player)
{
}

void CArmor::Drop (CPlayerEntity *Player)
{
}

#include "Utility/TemporaryEntities.h"

sint32 CArmor::CheckArmor (CPlayerEntity *Player, vec3f &Point, vec3f &Normal, sint32 Damage, EDamageFlags DamageFlags)
{
	if (!Damage || DamageFlags & (DAMAGE_NO_ARMOR | DAMAGE_NO_REG_ARMOR))
		return 0;

	sint32 save = ceil (((DamageFlags & DAMAGE_ENERGY) ? ((float)energyProtection / 100) : ((float)normalProtection / 100)) * Damage);
	if (save >= Player->Client.Persistent.Inventory.Has(this))
		save = Player->Client.Persistent.Inventory.Has(this);

	if (!save)
		return 0;

	Player->Client.Persistent.Inventory.Remove(GetIndex(), save);
	CSparks(Point, Normal, (DamageFlags & DAMAGE_BULLET) ? ST_BULLET_SPARKS : ST_SPARKS, SPT_SPARKS).Send();

	// Ran out of armor?
	if (!Player->Client.Persistent.Inventory.Has(this))
		Player->Client.Persistent.Armor = NULL;

	return save;
}

class CArmorEntity : public CItemEntity
{
public:
	CArmorEntity() :
	  IBaseEntity(),
	  CItemEntity ()
	  {
	  };

	CArmorEntity (sint32 Index) :
	  IBaseEntity(Index),
	  CItemEntity (Index)
	  {
	  };

	void Spawn (IBaseItem *item)
	{
		if ((Game.GameMode & GAME_DEATHMATCH) && DeathmatchFlags.dfNoArmor.IsEnabled())
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

LINK_ITEM_TO_CLASS (item_armor_jacket, CArmorEntity);
LINK_ITEM_TO_CLASS (item_armor_combat, CArmorEntity);
LINK_ITEM_TO_CLASS (item_armor_body, CArmorEntity);
LINK_ITEM_TO_CLASS (item_armor_shard, CArmorEntity);

void AddArmorToList ()
{
	NItems::JacketArmor = QNew (TAG_GENERIC) CArmor ("item_armor_jacket", "models/items/armor/jacket/tris.md2", FX_ROTATE, "misc/ar1_pkup.wav", "i_jacketarmor", "Jacket Armor", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, 25, 50, 30, 0);
	NItems::CombatArmor = QNew (TAG_GENERIC) CArmor ("item_armor_combat", "models/items/armor/combat/tris.md2", FX_ROTATE, "misc/ar1_pkup.wav", "i_combatarmor", "Combat Armor", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, 50, 100, 60, 30);
	NItems::BodyArmor = QNew (TAG_GENERIC) CArmor ("item_armor_body", "models/items/armor/body/tris.md2", FX_ROTATE, "misc/ar1_pkup.wav", "i_bodyarmor", "Body Armor", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, 100, 200, 80, 60);
	NItems::ArmorShard = QNew (TAG_GENERIC) CArmor ("item_armor_shard", "models/items/armor/shard/tris.md2", FX_ROTATE, "misc/ar2_pkup.wav", "i_jacketarmor", "Armor Shard", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, 2, -1, -1, -1);
}

