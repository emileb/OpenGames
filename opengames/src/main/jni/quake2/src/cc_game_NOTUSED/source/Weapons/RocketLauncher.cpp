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
// cc_grenadelauncher.cpp
// Grenade Launcher
//

#include "Local.h"
#include "Weapons/WeaponMain.h"

CRocketLauncher::CRocketLauncher() :
IWeaponBase(7, 0, "models/weapons/v_rocket/tris.md2", 0, 4, 5, 12,
		13, 50, 51, 54)
{
}

bool CRocketLauncher::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 5:
		return true;
	}
	return false;
}

bool CRocketLauncher::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 25:
	case 33:
	case 42:
	case 50:
		return true;
	}
	return false;
}

void CRocketLauncher::Fire (CPlayerEntity *Player)
{
	vec3f	offset (8, 8, Player->ViewHeight-8), start;
	const sint32	damage = CalcQuadVal(100 + (sint32)(frand() * 20.0)),
					radius_damage = CalcQuadVal(120);
	const float		damage_radius = 120;

	anglef angles = Player->Client.ViewAngle.ToVectors ();

	Player->Client.KickOrigin = angles.Forward * -2;
	Player->Client.KickAngles.X = -1;

	Player->P_ProjectSource (offset, angles, start);
	CRocket::Spawn (Player, start, angles.Forward, damage, 650, damage_radius, radius_damage);

	// send muzzle flash
	Muzzle (Player, MZ_ROCKET);
	AttackSound (Player);

	Player->PlayerNoiseAt (start, PNOISE_WEAPON);
	FireAnimation (Player);
	DepleteAmmo(Player, 1);

	Player->Client.PlayerState.GetGunFrame()++;
}

WEAPON_DEFS (CRocketLauncher);

LINK_ITEM_TO_CLASS (weapon_rocketlauncher, CItemEntity);

void CRocketLauncher::CreateItem (CItemList *List)
{
	NItems::RocketLauncher = QNew (TAG_GENERIC) CWeaponItem
		("weapon_rocketlauncher", "models/weapons/g_rocket/tris.md2", FX_ROTATE, "misc/w_pkup.wav", "w_rlauncher", "Rocket Launcher",
		ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE,
		&Weapon, NItems::Rockets, 1, "#w_rlauncher.md2");
};

