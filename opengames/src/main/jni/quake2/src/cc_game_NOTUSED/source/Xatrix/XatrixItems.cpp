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
// cc_xatrix_items.cpp
// 
//

#include "Local.h"

#if XATRIX_FEATURES
#include "Weapons/WeaponMain.h"
#include "Xatrix/XatrixTrap.h"

CFoodCube *FoodCubeItem;

static sint32	quad_fire_drop_timeout_hack;

CQuadFire::CQuadFire(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

void CQuadFire::DoPickup (CItemEntity *Item, CPlayerEntity *Other)
{
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		if (!(Item->SpawnFlags & DROPPED_ITEM) )
			SetRespawn (Item, 600);
		if (Item->SpawnFlags & DROPPED_PLAYER_ITEM)
			quad_fire_drop_timeout_hack = (Item->NextThink - Level.Frame);

		if (DeathmatchFlags.dfInstantItems.IsEnabled())
			Use (Other);
	}
}

void CQuadFire::Use (CPlayerEntity *Player)
{
	sint32 timeOut = 300;

	if (quad_fire_drop_timeout_hack)
	{
		timeOut = quad_fire_drop_timeout_hack;
		quad_fire_drop_timeout_hack = 0;
	}

	if (Player->Client.Timers.QuadFire > Level.Frame)
		Player->Client.Timers.QuadFire += timeOut;
	else
		Player->Client.Timers.QuadFire = Level.Frame + timeOut;

	Player->Client.Persistent.Inventory -= this;
	Player->PlaySound (CHAN_ITEM, SoundIndex("items/quadfire1.wav"));
}

LINK_ITEM_TO_CLASS (ammo_magslug, CAmmoEntity);
LINK_ITEM_TO_CLASS (ammo_trap, CAmmoEntity);
LINK_ITEM_TO_CLASS (item_quadfire, CPowerupEntity);
LINK_ITEM_TO_CLASS (key_green_key, CItemEntity);

void AddXatrixItemsToList ()
{
	NItems::MagSlugs = QNew (TAG_GENERIC) CAmmo("ammo_magslug", "models/objects/ammo/tris.md2", 0, "misc/am_pkup.wav", "a_mslugs", "Mag Slug", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, 10, AMMOTAG_MAGSLUGS);
	NItems::Trap = QNew (TAG_GENERIC) CAmmoWeapon("ammo_trap", "models/weapons/g_trap/tris.md2", FX_ROTATE, "misc/am_pkup.wav", "a_trap", "Trap", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_USABLE|ITEMFLAG_GRABBABLE|ITEMFLAG_WEAPON, &CTrap::Weapon, 1, "#a_grenades.md2", 1, AMMOTAG_TRAP);
	FoodCubeItem = QNew (TAG_GENERIC) CFoodCube;

	NItems::QuadFire = QNew (TAG_GENERIC) CQuadFire ("item_quadfire", "models/items/quadfire/tris.md2", FX_ROTATE, "items/pkup.wav", "p_quadfire", "DualFire Damage", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	QNew (TAG_GENERIC) CKey("key_green_key", "models/items/keys/green_key/tris.md2", FX_ROTATE, "items/pkup.wav", "k_greenkey", "Green Key", ITEMFLAG_GRABBABLE|ITEMFLAG_KEY|ITEMFLAG_STAY_COOP);
}
#endif
