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
// cc_chaingun.cpp
// Chaingun
//

#include "Local.h"
#include "Weapons/WeaponMain.h"

CChaingun::CChaingun() :
IWeaponBase(4, 0, "models/weapons/v_chain/tris.md2", 0, 4, 5, 31,
		32, 61, 62, 64)
{
}

bool CChaingun::CanFire (CPlayerEntity *Player)
{
	if (Player->Client.PlayerState.GetGunFrame() >= 5 && Player->Client.PlayerState.GetGunFrame() <= 21)
		return true;
	return false;
}

bool CChaingun::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 38:
	case 43:
	case 51:
	case 61:
		return true;
	}
	return false;
}

void CChaingun::FireAnimation (CPlayerEntity *Player)
{
	Player->Client.Anim.Priority = ANIM_ATTACK;
	if (Player->Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED)
	{
		Player->State.GetFrame() = (CPlayerEntity::FRAME_crattak1 - (Player->Client.PlayerState.GetGunFrame() & 1));
		Player->Client.Anim.EndFrame = CPlayerEntity::FRAME_crattak9;
	}
	else
	{
		Player->State.GetFrame() = (CPlayerEntity::FRAME_attack1 - (Player->Client.PlayerState.GetGunFrame() & 1));
		Player->Client.Anim.EndFrame = CPlayerEntity::FRAME_attack8;
	}
}

void CChaingun::Fire (CPlayerEntity *Player)
{
	sint32			shots;
	vec3f		start, offset;
	const sint32	damage = (Game.GameMode & GAME_DEATHMATCH) ?
				CalcQuadVal(6)
				:
				CalcQuadVal(8),
				kick = CalcQuadVal(2);

	if (Player->Client.PlayerState.GetGunFrame() == 5)
		Player->PlaySound (CHAN_AUTO, SoundIndex("weapons/chngnu1a.wav"));

	if ((Player->Client.PlayerState.GetGunFrame() == 14) && !(Player->Client.Buttons & BUTTON_ATTACK))
	{
		Player->Client.PlayerState.GetGunFrame() = 31;
		Player->Client.WeaponSound = 0;
		return;
	}
	else if ((Player->Client.PlayerState.GetGunFrame() == 21) && (Player->Client.Buttons & BUTTON_ATTACK)
		&& Player->Client.Persistent.Inventory.Has(Player->Client.Persistent.Weapon->Item->Ammo))
	{
		Player->Client.PlayerState.GetGunFrame() = 15;
	}
	else
		Player->Client.PlayerState.GetGunFrame()++;

	if (Player->Client.PlayerState.GetGunFrame() == 22)
	{
		Player->Client.WeaponSound = 0;
		Player->PlaySound (CHAN_AUTO, SoundIndex("weapons/chngnd1a.wav"));
	}
	else
		Player->Client.WeaponSound = SoundIndex("weapons/chngnl1a.wav");

	FireAnimation (Player);

	if (Player->Client.PlayerState.GetGunFrame() <= 9)
		shots = 1;
	else if (Player->Client.PlayerState.GetGunFrame() <= 14)
	{
		if (Player->Client.Buttons & BUTTON_ATTACK)
			shots = 2;
		else
			shots = 1;
	}
	else
		shots = 3;

	if (Player->Client.Persistent.Inventory.Has(Player->Client.Persistent.Weapon->Item->Ammo) < shots)
		shots = Player->Client.Persistent.Inventory.Has(Player->Client.Persistent.Weapon->Item->Ammo);

	if (!shots)
	{
		OutOfAmmo(Player);
		NoAmmoWeaponChange (Player);
		return;
	}

	for (uint8 i = 0; i < 3; i++)
	{
		Player->Client.KickOrigin[i] = crand() * 0.35;
		Player->Client.KickAngles[i] = crand() * 0.7;
	}

	for (uint8 i = 0; i < shots; i++)
	{
		// get start / end positions
		anglef angles = Player->Client.ViewAngle.ToVectors ();
		offset.Set (0, 7 + crand()*4, crand()*4 + Player->ViewHeight-8);
		Player->P_ProjectSource (offset, angles, start);

		CBullet::Fire (Player, start, angles.Forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_CHAINGUN);
	}

	// send muzzle flash
	Muzzle (Player, MZ_CHAINGUN1 + shots - 1);
	AttackSound (Player);

	Player->PlayerNoiseAt (start, PNOISE_WEAPON);
	DepleteAmmo (Player, shots);
}

WEAPON_DEFS (CChaingun);

LINK_ITEM_TO_CLASS (weapon_chaingun, CItemEntity);

void CChaingun::CreateItem (CItemList *List)
{
	NItems::Chaingun = QNew (TAG_GENERIC) CWeaponItem
		("weapon_chaingun", "models/weapons/g_chain/tris.md2", FX_ROTATE, "misc/w_pkup.wav",
		"w_chaingun", "Chaingun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE,
		&Weapon, NItems::Bullets, 1, "#w_chaingun.md2");
};

