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
// cc_rogue_chainfist.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES

#include "Weapons/WeaponMain.h"
#include "Rogue/RogueChainfist.h"
#include "Utility/TemporaryEntities.h"

CChainfist::CChainfist() :
IWeaponBase(0, 1, "models/weapons/v_chainf/tris.md2", 0, 4, 5, 32,
		33, 57, 58, 60)
{
}

bool CChainfist::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 8:
	case 9:

	case 16:
	case 17:
	case 18:

	case 30:
	case 31:
		return true;
	}
	return false;
}

bool CChainfist::CanStopFidgetting (CPlayerEntity *Player)
{
	return false;
}

void CChainfist::Smoke (CPlayerEntity *Player)
{
	vec3f			tempVec, forward, right;
	const vec3f		offset (8, 8, Player->ViewHeight - 4);

	Player->P_ProjectSource (offset, Player->Client.ViewAngle.ToVectors(), tempVec);

	CChainfistSmoke(tempVec).SendTo (Player);
}

void CChainfist::Fire (CPlayerEntity *Player)
{
	vec3f			offset (0, 8, Player->ViewHeight - 4), forward, right, start;
	const sint32	damage = CalcQuadVal((Game.GameMode & GAME_DEATHMATCH) ? 30 : 15);

	FireAnimation (Player);

	anglef angles = Player->Client.ViewAngle.ToVectors();
	Player->P_ProjectSource (offset, angles, start);

	Player->Client.KickOrigin = angles.Forward * -2;
	Player->Client.KickAngles.X = -1;

	CPlayerMeleeWeapon::Fire (Player, start, angles.Forward, 64, damage, 100, MOD_CHAINFIST);

	AttackSound (Player);

	Player->PlayerNoiseAt (start, PNOISE_WEAPON);
	Player->Client.PlayerState.GetGunFrame()++;
}

bool CChainfist::AttemptToFire (CPlayerEntity *Player)
{
	return true;
}

void CChainfist::WeaponGeneric (CPlayerEntity *Player)
{
	if (Player->Client.PlayerState.GetGunFrame() == 13 || Player->Client.PlayerState.GetGunFrame() == 23)			// end of attack, go idle
		Player->Client.PlayerState.GetGunFrame() = 32;

	if (randomMT() & 7)
	{
		switch (Player->Client.PlayerState.GetGunFrame())
		{
		case 42:
		case 51:
			Smoke (Player);
			break;
		}
	}

	// set the appropriate weapon sound.
	switch (Player->Client.WeaponState)
	{
	case WS_FIRING:
		Player->Client.WeaponSound = SoundIndex ("weapons/sawhit.wav");
		break;
	case WS_DEACTIVATING:
		Player->Client.WeaponSound = 0;
		break;
	default:
		Player->Client.WeaponSound = SoundIndex ("weapons/sawidle.wav");
		break;
	};

	IWeaponBase::WeaponGeneric (Player);

	int LastSequence = 0;
	if ((Player->Client.Buttons) & BUTTON_ATTACK)
	{
		if (Player->Client.PlayerState.GetGunFrame() == 13 ||
			Player->Client.PlayerState.GetGunFrame() == 23 ||
			Player->Client.PlayerState.GetGunFrame() == 32)
		{
			LastSequence = Player->Client.PlayerState.GetGunFrame();
			Player->Client.PlayerState.GetGunFrame() = 6;
		}
	}

	if (Player->Client.PlayerState.GetGunFrame() == 6)
	{
		float Chance = frand();

		switch (LastSequence)
		{
		case 13:
			Chance -= 0.34f;
			break;
		case 23:
			Chance += 0.33f;
			break;
		case 32:
			if (Chance >= 0.33f)
				Chance += 0.34f;
			break;
		};

		if (Chance < 0.33)
			Player->Client.PlayerState.GetGunFrame() = 14;
		else if (Chance < 0.66)
			Player->Client.PlayerState.GetGunFrame() = 24;
	}}

WEAPON_DEFS (CChainfist);

LINK_ITEM_TO_CLASS (weapon_chainfist, CItemEntity);

void CChainfist::CreateItem (CItemList *List)
{
	QNew (TAG_GENERIC) CWeaponItem
		("weapon_chainfist", "models/weapons/g_chainf/tris.md2", FX_ROTATE, "misc/w_pkup.wav", "w_chainfist", "Chainfist",
		ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, &Weapon,
		NULL, 0, "#w_chainfist.md2");
};

#endif