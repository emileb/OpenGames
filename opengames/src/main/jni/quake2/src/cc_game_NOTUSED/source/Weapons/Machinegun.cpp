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
// cc_machinegun.cpp
// Machinegun
//

#include "Local.h"
#include "Weapons/WeaponMain.h"

CMachinegun::CMachinegun() :
IWeaponBase(3, 0, "models/weapons/v_machn/tris.md2", 0, 3, 4, 5,
		6, 45, 46, 49)
{
}

bool CMachinegun::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 4:
	case 5:
		return true;
	}
	return false;
}

bool CMachinegun::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 23:
	case 45:
		return true;
	}
	return false;
}

void CMachinegun::FireAnimation (CPlayerEntity *Player)
{
	Player->Client.Anim.Priority = ANIM_ATTACK;
	if (Player->Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED)
	{
		Player->State.GetFrame() = (CPlayerEntity::FRAME_crattak1 - (sint32) (frand()+0.25));
		Player->Client.Anim.EndFrame = CPlayerEntity::FRAME_crattak9;
	}
	else
	{
		Player->State.GetFrame() = (CPlayerEntity::FRAME_attack1 - (sint32) (frand()+0.25));
		Player->Client.Anim.EndFrame = CPlayerEntity::FRAME_attack8;
	}
}

void CMachinegun::Fire (CPlayerEntity *Player)
{
	if (!(Player->Client.Buttons & BUTTON_ATTACK))
	{
		Player->Client.Timers.MachinegunShots = 0;
		Player->Client.PlayerState.GetGunFrame()++;
		return;
	}

	if (Player->Client.PlayerState.GetGunFrame() == 5)
		Player->Client.PlayerState.GetGunFrame() = 4;
	else
		Player->Client.PlayerState.GetGunFrame() = 5;

	if (!AttemptToFire(Player))
	{
		Player->Client.PlayerState.GetGunFrame() = 6;
		OutOfAmmo(Player);
		NoAmmoWeaponChange (Player);
		return;
	}

	const sint32		damage = CalcQuadVal(8),
						kick = CalcQuadVal(2);

	for (uint8 i = 1; i < 3; i++)
	{
		Player->Client.KickOrigin[i] = crand() * 0.35;
		Player->Client.KickAngles[i] = crand() * 0.7;
	}

	Player->Client.KickOrigin.X = crand() * 0.35;
	Player->Client.KickAngles.X = Player->Client.Timers.MachinegunShots * -1.5;

	// raise the gun as it is firing
	if (!(Game.GameMode & GAME_DEATHMATCH))
	{
		Player->Client.Timers.MachinegunShots++;
		if (Player->Client.Timers.MachinegunShots > 9)
			Player->Client.Timers.MachinegunShots = 9;
	}

	// get start / end positions
	vec3f start, offset (0, 8, Player->ViewHeight-8); 

	anglef angles = (Player->Client.ViewAngle + Player->Client.KickAngles).ToVectors ();
	Player->P_ProjectSource (offset, angles, start);

	CBullet::Fire (Player, start, angles.Forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_MACHINEGUN);

	Muzzle (Player, MZ_MACHINEGUN);
	AttackSound (Player);

	Player->PlayerNoiseAt (start, PNOISE_WEAPON);
	DepleteAmmo(Player, 1);

	FireAnimation (Player);
}

WEAPON_DEFS (CMachinegun);

LINK_ITEM_TO_CLASS (weapon_machinegun, CItemEntity);

void CMachinegun::CreateItem (CItemList *List)
{
	NItems::Machinegun = QNew (TAG_GENERIC) CWeaponItem
		("weapon_machinegun", "models/weapons/g_machn/tris.md2", FX_ROTATE, "misc/w_pkup.wav", "w_machinegun",
		"Machinegun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE,
		&Weapon, NItems::Bullets, 1, "#w_machinegun.md2");
};

