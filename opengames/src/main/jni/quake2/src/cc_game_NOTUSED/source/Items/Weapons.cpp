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
// cc_weapons.cpp
// New item system code
//

#include "Local.h"
#include "Weapons/WeaponMain.h"

CWeaponItem::CWeaponItem (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   IWeaponBase *Weapon, CAmmo *Ammo, sint32 Amount, const char *VWepModel) :
IBaseItem(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags),
Weapon(Weapon),
Ammo(Ammo),
Amount(Amount),
VWepModel(VWepModel)
{
	if (!Weapon)
		DebugPrint ("Warning: Weapon with no weapon!\n");
	else
		Weapon->Item = this;

	if (!Ammo && Amount)
		DebugPrint ("Warning: Weapon with no ammo has quantity!\n");
}

CWeaponItem::CWeaponItem ()
{
}

CAmmo::CAmmo (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   sint32 Quantity, EAmmoTag Tag) :
IBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags),
Quantity(Quantity),
Tag(Tag)
{
}

CAmmo::CAmmo (const char *Classname, const char *WorldModel, const char *Icon, const char *Name,
				sint32 Quantity, EAmmoTag Tag) :
IBaseItem (Classname, WorldModel, 0, "misc/am_pkup.wav", Icon, Name, ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE),
Quantity(Quantity),
Tag(Tag)
{
};

CAmmo::CAmmo ()
{
}

CAmmoWeapon::CAmmoWeapon (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   IWeaponBase *Weapon, CAmmo *Ammo, sint32 Amount, const char *VWepModel,
			   sint32 Quantity, EAmmoTag Tag) :
IBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags)
{
	this->Weapon = Weapon;
	this->Ammo = Ammo;
	this->Amount = Amount;
	this->VWepModel = VWepModel;
	this->Quantity = Quantity;
	this->Tag = Tag;

	if (!Weapon)
		DebugPrintf ("Warning: Weapon with no weapon!\n");
	else
		Weapon->Item = this;

	if (!Ammo && Amount)
		DebugPrint ("Warning: Weapon with no ammo has quantity!\n");
}

CAmmoWeapon::CAmmoWeapon (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   IWeaponBase *Weapon, sint32 Amount, const char *VWepModel,
			   sint32 Quantity, EAmmoTag Tag) :
IBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags)
{
	this->Weapon = Weapon;
	this->Ammo = this;
	this->Amount = Amount;
	this->VWepModel = VWepModel;
	this->Quantity = Quantity;
	this->Tag = Tag;

	if (!Weapon)
		DebugPrint ("Warning: Weapon with no weapon!\n");
	else
		Weapon->Item = this;

	if (!Ammo && Amount)
		DebugPrint ("Warning: Weapon with no ammo has quantity!\n");
}

#ifndef NO_AUTOSWITCH
bool CheckAutoSwitch (CPlayerEntity *Other)
{
	sint32 val = atoi(Other->Client.Persistent.UserInfo.GetValueFromKey (String("cc_autoswitch")).CString());
	return (val == 1);
}
#endif

bool CWeaponItem::Pickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	if ( (DeathmatchFlags.dfWeaponsStay.IsEnabled() || Game.GameMode & GAME_COOPERATIVE) 
		&& Other->Client.Persistent.Inventory.Has(GetIndex()))
	{
		if (!(Item->SpawnFlags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM) ) )
			return false;	// leave the weapon for others to pickup
	}

	Other->Client.Persistent.Inventory += this;

	if (!(Item->SpawnFlags & DROPPED_ITEM) )
	{
		// give them some ammo with it
		if (Ammo)
		{
			if (DeathmatchFlags.dfInfiniteAmmo.IsEnabled())
				Ammo->AddAmmo (Other, 1000);
			else
				Ammo->AddAmmo (Other, Ammo->Quantity);
		}

		if (! (Item->SpawnFlags & DROPPED_PLAYER_ITEM) )
		{
			if (Game.GameMode & GAME_DEATHMATCH)
			{
				if (DeathmatchFlags.dfWeaponsStay.IsEnabled())
					Item->ShouldRespawn = true;
				else
					SetRespawn (Item, 300);
			}
			if (Game.GameMode & GAME_COOPERATIVE)
				Item->ShouldRespawn = true;
		}
	}
	else if (Item->AmmoCount)
		Ammo->AddAmmo (Other, Item->AmmoCount);

	if (Weapon)
	{
		if (Other->Client.Persistent.Weapon != Weapon && 
#ifndef NO_AUTOSWITCH
			(CheckAutoSwitch(Other) || 
#endif
			((Other->Client.Persistent.Inventory.Has(this) == 1)) &&
			( !(Game.GameMode & GAME_DEATHMATCH) || (Other->Client.Persistent.Weapon && Other->Client.Persistent.Weapon->Item == NItems::Blaster) ) )
#ifndef NO_AUTOSWITCH
			)
#endif
			Other->Client.NewWeapon = Weapon;
	}


	return true;
}

void CWeaponItem::Use (CPlayerEntity *Player)
{
	Weapon->Use (this, Player);
}

void CWeaponItem::Drop (CPlayerEntity *Player)
{
	if ((Weapon == Player->Client.Persistent.Weapon) && (Player->Client.WeaponState != WS_IDLE))
		return;

	DropItem(Player);

	Player->Client.Persistent.Inventory -= this;

	if (Weapon == Player->Client.Persistent.Weapon)
		Player->Client.Persistent.Weapon->NoAmmoWeaponChange(Player);
}

sint32 maxBackpackAmmoValues[AMMOTAG_MAX] =
{
	200,
	300,
	100,
	100,
	300,
	100,

#if XATRIX_FEATURES
	100,
	5,
#endif

#if ROGUE_FEATURES
	50,
	250,
	50,
	200,
#endif
};
sint32 maxBandolierAmmoValues[AMMOTAG_MAX] =
{
	150,
	250,
	50,
	50,
	250,
	75,

#if XATRIX_FEATURES
	100,
	5,
#endif

#if ROGUE_FEATURES
	50,
	200,
	50,
	150,
#endif
};

sint32 CAmmo::GetMax (CPlayerEntity *Player)
{
	return Player->Client.Persistent.MaxAmmoValues[Tag];
}

bool CAmmo::Pickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	sint32			count = Quantity;

	if (Item->AmmoCount)
		count = Item->AmmoCount;

	if (!AddAmmo (Other, count))
		return false;

	if (!(Item->SpawnFlags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)) && (Game.GameMode & GAME_DEATHMATCH))
		SetRespawn (Item, 300);
	return true;
}

void CAmmo::Use (CPlayerEntity *Player)
{
}

void CAmmo::Drop (CPlayerEntity *Player)
{
	sint32 count = Quantity;
	CItemEntity *dropped = DropItem(Player);

	if (count > Player->Client.Persistent.Inventory.Has(this))
		count = Player->Client.Persistent.Inventory.Has(this);

	dropped->AmmoCount = count;

	Player->Client.Persistent.Inventory.Remove (this, count);
}

bool CAmmoWeapon::Pickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	sint32			oldcount = Other->Client.Persistent.Inventory.Has(this);
	sint32			count = Quantity;
	bool		weapon = (Flags & ITEMFLAG_WEAPON);

	if (weapon && DeathmatchFlags.dfInfiniteAmmo.IsEnabled())
		count = 1000;
	else if (Item->AmmoCount)
		count = Item->AmmoCount;

	if (!AddAmmo (Other, count))
		return false;

	if (weapon && !oldcount)
	{
		if (Other->Client.Persistent.Weapon != Weapon && (!(Game.GameMode & GAME_DEATHMATCH) ||
			(Other->Client.Persistent.Weapon && Other->Client.Persistent.Weapon->Item == NItems::Blaster)))
			Other->Client.NewWeapon = Weapon;
	}

	if (!(Item->SpawnFlags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)) && (Game.GameMode & GAME_DEATHMATCH))
		SetRespawn (Item, 300);
	return true;
}

void CAmmoWeapon::Use (CPlayerEntity *Player)
{
/*	if (!(Flags & ITEMFLAG_WEAPON))
		return;

	// see if we're already using it
	if (Weapon == Player->Client.Persistent.Weapon)
		return;

	if (!Player->Client.Persistent.Inventory.Has(GetIndex()))
	{
		Player->PrintToClient (PRINT_HIGH, "Out of Item: %s.\n", Name);
		return;
	}

	// change to this weapon when down
	Player->Client.NewWeapon = Weapon;*/
	CAmmoWeapon *Wanted = this;
	bool UsingItOrChain = !Player->Client.Persistent.Inventory.Has(Wanted);

	while (!UsingItOrChain)
	{
		Wanted = dynamic_cast<CAmmoWeapon*>(Wanted->Weapon->GetNextWeapon()->Item);

		if (Player->Client.Persistent.Weapon == Wanted->Weapon)
		{
			UsingItOrChain = true;
			break;
		}

		if (Wanted->Weapon == Weapon)
			break;
	};

	// see if we're already using it
	if (UsingItOrChain)
	{
		if (!(Wanted->Weapon->GetNextWeapon() == Wanted->Weapon && Wanted->Weapon->GetPrevWeapon() == Wanted->Weapon))
		{
			while (true)
			{
				Wanted = dynamic_cast<CAmmoWeapon*>(Wanted->Weapon->GetNextWeapon()->Item);

				if (Wanted->Weapon == Weapon)
					break; // nothing

				if (!Player->Client.Persistent.Inventory.Has(Wanted))
					continue;

				break;
			};
		}
	}

	if (!Player->Client.Persistent.Inventory.Has(Wanted))
	{
		Player->PrintToClient (PRINT_HIGH, "Out of Item: %s.\n", Name);
		return;
	}

	// change to this weapon when down
	Player->Client.NewWeapon = Wanted->Weapon;
}

void CAmmoWeapon::Drop (CPlayerEntity *Player)
{
	sint32 count = Quantity;
	CItemEntity *dropped = DropItem(Player);

	if (count > Player->Client.Persistent.Inventory.Has(this))
		count = Player->Client.Persistent.Inventory.Has(this);

	dropped->AmmoCount = count;

	Player->Client.Persistent.Inventory.Remove (this, count);

	if (Weapon && Player->Client.Persistent.Weapon && (Player->Client.Persistent.Weapon == Weapon) &&
		!Player->Client.Persistent.Inventory.Has(this))
		Player->Client.Persistent.Weapon->NoAmmoWeaponChange(Player);
}

bool CAmmo::AddAmmo (CPlayerEntity *Player, sint32 count)
{
	// YUCK
	sint32 max = GetMax(Player);

	if (!max)
		return false;

	if (Player->Client.Persistent.Inventory.Has(this) < max)
	{
		Player->Client.Persistent.Inventory.Add (this, count);

		if (Player->Client.Persistent.Inventory.Has(this) > max)
			Player->Client.Persistent.Inventory.Set(this, max);

		return true;
	}
	return false;
}

CAmmoEntity::CAmmoEntity() :
  IBaseEntity(),
  CItemEntity ()
  {
  };

CAmmoEntity::CAmmoEntity (sint32 Index) :
  IBaseEntity(Index),
  CItemEntity (Index)
  {
  };

void CAmmoEntity::Spawn (IBaseItem *item)
{
	if ((Game.GameMode & GAME_DEATHMATCH) && DeathmatchFlags.dfInfiniteAmmo.IsEnabled())
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

LINK_ITEM_TO_CLASS (ammo_shells, CAmmoEntity);
LINK_ITEM_TO_CLASS (ammo_bullets, CAmmoEntity);
LINK_ITEM_TO_CLASS (ammo_slugs, CAmmoEntity);
LINK_ITEM_TO_CLASS (ammo_grenades, CAmmoEntity);
LINK_ITEM_TO_CLASS (ammo_rockets, CAmmoEntity);
LINK_ITEM_TO_CLASS (ammo_cells, CAmmoEntity);

void AddWeapons (CItemList *List);
void AddAmmoToList ()
{
	NItems::Shells = QNew (TAG_GENERIC) CAmmo("ammo_shells", "models/items/ammo/shells/medium/tris.md2", 0, "misc/am_pkup.wav", "a_shells", "Shells", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, 10, AMMOTAG_SHELLS);
	NItems::Bullets = QNew (TAG_GENERIC) CAmmo("ammo_bullets", "models/items/ammo/bullets/medium/tris.md2", 0, "misc/am_pkup.wav", "a_bullets", "Bullets", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, 50, AMMOTAG_BULLETS);
	NItems::Slugs = QNew (TAG_GENERIC) CAmmo("ammo_slugs", "models/items/ammo/slugs/medium/tris.md2", 0, "misc/am_pkup.wav", "a_slugs", "Slugs", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, 10, AMMOTAG_SLUGS);
	NItems::Grenades = QNew (TAG_GENERIC) CAmmoWeapon("ammo_grenades", "models/items/ammo/grenades/medium/tris.md2", 0, "misc/am_pkup.wav", "a_grenades", "Grenades", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_USABLE|ITEMFLAG_GRABBABLE|ITEMFLAG_WEAPON, &CHandGrenade::Weapon, 1, "#a_grenades.md2", 5, AMMOTAG_GRENADES);
	NItems::Rockets = QNew (TAG_GENERIC) CAmmo("ammo_rockets", "models/items/ammo/rockets/medium/tris.md2", 0, "misc/am_pkup.wav", "a_rockets", "Rockets", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, 5, AMMOTAG_ROCKETS);
	NItems::Cells = QNew (TAG_GENERIC) CAmmo("ammo_cells", "models/items/ammo/cells/medium/tris.md2", 0, "misc/am_pkup.wav", "a_cells", "Cells", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, 50, AMMOTAG_CELLS);

#if XATRIX_FEATURES
	AddXatrixItemsToList ();
#endif

#if ROGUE_FEATURES
	AddRogueItemsToList ();
#endif

	AddWeapons (ItemList);
}

