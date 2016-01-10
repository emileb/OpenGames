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
// cc_shotgun.cpp
// Shotgun
//

#include "Local.h"
#include "Weapons/WeaponMain.h"

CSuperShotgun::CSuperShotgun() :
IWeaponBase(2, 0, "models/weapons/v_shotg2/tris.md2", 0, 6, 7, 17,
		18, 57, 58, 61)
{
}

bool CSuperShotgun::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 7:
		return true;
	}
	return false;
}

bool CSuperShotgun::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 29:
	case 42:
	case 57:
		return true;
	}
	return false;
}

void CSuperShotgun::Fire (CPlayerEntity *Player)
{
	vec3f		start, offset (0, 8, Player->ViewHeight-8);
	const sint32	damage = CalcQuadVal(6),
					kick = CalcQuadVal(12);

	anglef angles = Player->Client.ViewAngle.ToVectors ();

	Player->Client.KickOrigin = angles.Forward * -2;
	Player->Client.KickAngles.X = -2;

	Player->P_ProjectSource (offset, angles, start);

	vec3f v = Player->Client.ViewAngle;
	v.Y -= 5;
	angles = v.ToVectors ();
	CShotgunPellets::Fire (Player, start, angles.Forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT/2, MOD_SSHOTGUN);

	v.Y += 10;
	angles = v.ToVectors ();
	CShotgunPellets::Fire (Player, start, angles.Forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT/2, MOD_SSHOTGUN);

	// send muzzle flash
	Muzzle (Player, MZ_SSHOTGUN);
	FireAnimation (Player);

	AttackSound (Player);

	Player->Client.PlayerState.GetGunFrame()++;
	Player->PlayerNoiseAt (start, PNOISE_WEAPON);

	DepleteAmmo(Player, 2);
}

WEAPON_DEFS (CSuperShotgun);

LINK_ITEM_TO_CLASS (weapon_supershotgun, CItemEntity);

void CSuperShotgun::CreateItem (CItemList *List)
{
	NItems::SuperShotgun = QNew (TAG_GENERIC) CWeaponItem
		("weapon_supershotgun", "models/weapons/g_shotg2/tris.md2", FX_ROTATE, "misc/w_pkup.wav", "w_sshotgun",
		"Super Shotgun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE,
		&Weapon, NItems::Shells, 2, "#w_sshotgun.md2");
};

