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
// cc_rogue_heatbeam.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES

#include "Weapons/WeaponMain.h"
#include "Rogue/RogueHeatbeam.h"
#include "Rogue/RogueWeaponry.h"

// The launcher
CHeatBeamWeapon::CHeatBeamWeapon() :
IWeaponBase(8, 2, "models/weapons/v_beamer/tris.md2", 0, 8, 9, 12,
		13, 39, 40, 44)
{
}

bool CHeatBeamWeapon::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 9:
	case 10:
	case 11:
	case 12:
		return true;
	}
	return false;
}

bool CHeatBeamWeapon::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 35:
		return true;
	}
	return false;
}

void CHeatBeamWeapon::Fire (CPlayerEntity *Player)
{
	vec3f offset (7, 2, Player->ViewHeight-3);
	const sint32		damage = CalcQuadVal(15),
						kick = CalcQuadVal((Game.GameMode & GAME_DEATHMATCH) ? 75 : 30);

	Player->Client.PlayerState.GetGunIndex() = ModelIndex ("models/weapons/v_beamer2/tris.md2");

	// get start / end positions
	vec3f start;

	anglef angles = Player->Client.ViewAngle.ToVectors();
	Player->P_ProjectSource (offset, angles, start);

	CHeatBeam::Fire(Player, start, angles.Forward, damage, kick, MOD_HEATBEAM, false);

	// send muzzle flash
	Muzzle (Player, MZ_HEATBEAM);
	AttackSound (Player);

	DepleteAmmo(Player, 2);
	FireAnimation (Player);

	Player->Client.PlayerState.GetGunFrame()++;
}

void CHeatBeamWeapon::WeaponGeneric (CPlayerEntity *Player)
{
	if (Player->Client.WeaponState == WS_FIRING)
	{
		Player->Client.WeaponSound = SoundIndex ("weapons/bfg__l1a.wav");
		if ((Player->Client.Persistent.Inventory.Has(Weapon.Item->Ammo) >= 2) && ((Player->Client.Buttons) & BUTTON_ATTACK))
		{
			if (Player->Client.PlayerState.GetGunFrame() >= 13)
			{
				Player->Client.PlayerState.GetGunFrame() = 9;
				Player->Client.PlayerState.GetGunIndex() = ModelIndex ("models/weapons/v_beamer2/tris.md2");
			}
			else
				Player->Client.PlayerState.GetGunIndex() = ModelIndex ("models/weapons/v_beamer2/tris.md2");
		}
		else
		{
			Player->Client.PlayerState.GetGunFrame() = 13;
			Player->Client.PlayerState.GetGunIndex() = ModelIndex ("models/weapons/v_beamer/tris.md2");
		}
	}
	else
	{
		Player->Client.PlayerState.GetGunIndex() = ModelIndex ("models/weapons/v_beamer/tris.md2");
		Player->Client.WeaponSound = 0;
	}

	IWeaponBase::WeaponGeneric (Player);
}

WEAPON_DEFS (CHeatBeamWeapon);

LINK_ITEM_TO_CLASS (weapon_plasmabeam, CItemEntity);
LINK_EXISTING_ITEM_TO_NEW_CLASS (weapon_heatbeam, "weapon_plasmabeam", CItemEntity);

void CHeatBeamWeapon::CreateItem (CItemList *List)
{
	QNew (TAG_GENERIC) CWeaponItem
		("weapon_plasmabeam", "models/weapons/g_beamer/tris.md2", FX_ROTATE, "misc/w_pkup.wav", "w_heatbeam", "Plasma Beam",
		ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, &Weapon,
		NItems::Cells, 2, "#w_plasma.md2");
};

#endif
