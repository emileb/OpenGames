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

CShotgun::CShotgun() :
IWeaponBase(1, 0, "models/weapons/v_shotg/tris.md2", 0, 7, 8, 18,
		19, 36, 37, 39)
{
}

bool CShotgun::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 8:
		return true;
	}
	return false;
}

bool CShotgun::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 22:
	case 28:
	case 34:
		return true;
	}
	return false;
}

void CShotgun::Fire (CPlayerEntity *Player)
{
	vec3f		start, offset (0, 8,  Player->ViewHeight-8);
	const sint32	damage = CalcQuadVal(4),
					kick = CalcQuadVal(8);

	anglef angles = Player->Client.ViewAngle.ToVectors ();

	Player->Client.KickOrigin = angles.Forward * -2;
	Player->Client.KickAngles.X = -2;

	Player->P_ProjectSource (offset, angles, start);

	CShotgunPellets::Fire (Player, start, angles.Forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);

	// send muzzle flash
	Muzzle (Player, MZ_SHOTGUN);
	AttackSound (Player);

	Player->Client.PlayerState.GetGunFrame()++;
	Player->PlayerNoiseAt (start, PNOISE_WEAPON);
	FireAnimation (Player);

	DepleteAmmo(Player, 1);
}

WEAPON_DEFS (CShotgun);

LINK_ITEM_TO_CLASS (weapon_shotgun, CItemEntity);

void CShotgun::CreateItem (CItemList *List)
{
	NItems::Shotgun = QNew (TAG_GENERIC) CWeaponItem
		("weapon_shotgun", "models/weapons/g_shotg/tris.md2", FX_ROTATE, "misc/w_pkup.wav",
		"w_shotgun", "Shotgun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE,
		&Weapon, NItems::Shells, 1, "#w_shotgun.md2");
};

