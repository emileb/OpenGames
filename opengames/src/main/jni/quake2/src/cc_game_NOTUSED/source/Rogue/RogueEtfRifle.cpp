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
// cc_rogue_etf_rifle.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES

#include "Weapons/WeaponMain.h"
#include "Rogue/RogueEtfRifle.h"
#include "Rogue/RogueWeaponry.h"

CETFRifle::CETFRifle() :
IWeaponBase(4, 1, "models/weapons/v_etf_rifle/tris.md2", 0, 4, 5, 7,
		8, 37, 38, 41)
{
}

bool CETFRifle::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 6:
	case 7:
		return true;
	}
	return false;
}

bool CETFRifle::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 18:
	case 28:
		return true;
	}
	return false;
}

void CETFRifle::Fire (CPlayerEntity *Player)
{
	vec3f	offset (15, (Player->Client.PlayerState.GetGunFrame() == 6) ? 8 : 6, Player->ViewHeight - 8), start;
	const sint32	damage = CalcQuadVal(10),
					kick = CalcQuadVal(3);

	FireAnimation (Player);

	anglef angles = Player->Client.ViewAngle.ToVectors();
	Player->P_ProjectSource (offset, angles, start);

	Player->Client.KickOrigin.Set (crand() * 0.85f, crand() * 0.85f, crand() * 0.85f);
	Player->Client.KickAngles.Set (crand() * 0.85f, crand() * 0.85f, crand() * 0.85f);

	CFlechette::Spawn (Player, start, angles.Forward, damage, kick, 750);

	Muzzle (Player, MZ_ETF_RIFLE);
	AttackSound (Player);

	Player->PlayerNoiseAt (start, PNOISE_WEAPON);
	DepleteAmmo(Player, 1);

	Player->Client.PlayerState.GetGunFrame()++;
}

bool CETFRifle::AttemptToFire (CPlayerEntity *Player)
{
	if (!IWeaponBase::AttemptToFire(Player))
	{
		Player->Client.KickOrigin.Clear();
		Player->Client.KickAngles.Clear();
		Player->Client.PlayerState.GetGunFrame() = 8;

		return false;
	}

	return true;
}

void CETFRifle::WeaponGeneric (CPlayerEntity *Player)
{
	if (Player->Client.WeaponState == WS_FIRING)
	{
		if (Player->Client.Persistent.Inventory.Has(Item->Ammo) <= 0)
			Player->Client.PlayerState.GetGunFrame() = 8;
	}

	IWeaponBase::WeaponGeneric (Player);

	if (Player->Client.PlayerState.GetGunFrame() == 8 && (Player->Client.Buttons & BUTTON_ATTACK))
		Player->Client.PlayerState.GetGunFrame() = 6;
}

WEAPON_DEFS (CETFRifle);

LINK_ITEM_TO_CLASS (weapon_etf_rifle, CItemEntity);
LINK_EXISTING_ITEM_TO_NEW_CLASS (weapon_nailgun, "weapon_etf_rifle", CItemEntity);

void CETFRifle::CreateItem (CItemList *List)
{
	QNew (TAG_GENERIC) CWeaponItem
		("weapon_etf_rifle", "models/weapons/g_etf_rifle/tris.md2", FX_ROTATE, "misc/w_pkup.wav", "w_etf_rifle", "ETF Rifle",
		ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE|ITEMFLAG_MELEE, &Weapon,
		NItems::Flechettes, 1, "#w_etfrifle.md2");
};

#endif
