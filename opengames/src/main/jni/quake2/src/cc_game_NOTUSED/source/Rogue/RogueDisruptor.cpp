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
// cc_rogue_disruptor.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES

#include "Weapons/WeaponMain.h"
#include "Rogue/RogueDisruptor.h"
#include "Rogue/RogueWeaponry.h"

// The launcher
CDisruptor::CDisruptor() :
IWeaponBase(10, 1, "models/weapons/v_dist/tris.md2", 0, 4, 5, 9,
		10, 29, 30, 34)
{
}

bool CDisruptor::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 5:
		return true;
	}
	return false;
}

bool CDisruptor::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 14:
	case 19:
	case 23:
		return true;
	}
	return false;
}

void CDisruptor::Fire (CPlayerEntity *Player)
{
	vec3f		offset (24, 8, Player->ViewHeight-8), start;
	const int	Damage = CalcQuadVal((Game.GameMode & GAME_DEATHMATCH) ? 30 : 45);

	FireAnimation (Player);

	anglef angles = Player->Client.ViewAngle.ToVectors();
	Player->P_ProjectSource (offset, angles, start);

	IBaseEntity *Enemy = NULL;

	vec3f end = start.MultiplyAngles (8192, angles.Forward);
	CTrace tr (start, end, Player, CONTENTS_MASK_SHOT);
	
	if (tr.Entity != World)
	{
		if (tr.Entity->EntityFlags & EF_HURTABLE)
		{
			if (entity_cast<IHurtableEntity>(tr.Entity)->Health > 0)
				Enemy = tr.Entity;
		}
	}
	else
	{
		tr (start, vec3f(-16), vec3f(16), end, Player, CONTENTS_MASK_SHOT);

		if(tr.Entity != World)
		{
			if (tr.Entity->EntityFlags & EF_HURTABLE)
			{
				if (entity_cast<IHurtableEntity>(tr.Entity)->Health > 0)
					Enemy = tr.Entity;
			}
		}
	}

	Player->Client.KickOrigin = angles.Forward * -2;
	Player->Client.KickAngles.X = -1;

	CDisruptorTracker::Spawn (Player, start, angles.Forward, Damage, 1000, Enemy);

	Muzzle (Player, MZ_TRACKER);
	AttackSound (Player);

	Player->PlayerNoiseAt (start, PNOISE_WEAPON);
	DepleteAmmo(Player, 1);

	Player->Client.PlayerState.GetGunFrame()++;
}

WEAPON_DEFS (CDisruptor);

LINK_ITEM_TO_CLASS (weapon_disintegrator, CItemEntity);

void CDisruptor::CreateItem (CItemList *List)
{
	QNew (TAG_GENERIC) CWeaponItem
		("weapon_disintegrator", "models/weapons/g_dist/tris.md2", FX_ROTATE, "misc/w_pkup.wav", "w_disintegrator", "Disruptor",
		ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, &Weapon,
		NItems::Rounds, 1, "#w_disrupt.md2");
};

#endif
