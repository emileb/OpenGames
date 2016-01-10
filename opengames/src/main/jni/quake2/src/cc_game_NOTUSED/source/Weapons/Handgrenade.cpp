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
// cc_handgrenade.cpp
// Handgrenades
//

#include "Local.h"
#include "Weapons/WeaponMain.h"

CHandGrenade::CHandGrenade() :
IWeaponBase(6, 0, "models/weapons/v_handgr/tris.md2", 0, 0, 0, 16,
		17, 48, 0, 0)
{
}

bool CHandGrenade::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 29:
	case 34:
	case 39:
	case 48:
		return true;
	}
	return false;
}

const int GRENADE_TIMER		= 30;
const int GRENADE_MINSPEED	= 400;
const int GRENADE_MAXSPEED	= 800;

void CHandGrenade::Hold (CPlayerEntity *Player)
{
	if (!Player->Client.Grenade.Time)
	{
		Player->Client.Grenade.Time = Level.Frame + GRENADE_TIMER + 2;
		Player->Client.WeaponSound = SoundIndex("weapons/hgrenc1b.wav");
	}

	// they waited too long, detonate it in their hand
	if (!Player->Client.Grenade.BlewUp && (Level.Frame >= Player->Client.Grenade.Time))
	{
		Player->Client.WeaponSound = 0;
		FireGrenade (Player, true);
		Player->Client.Grenade.BlewUp = true;

		Player->Client.PlayerState.GetGunFrame() = 15;
		return;
	}

	if (Player->Client.Buttons & BUTTON_ATTACK)
		return;

	Player->Client.PlayerState.GetGunFrame()++;
}

void CHandGrenade::FireGrenade (CPlayerEntity *Player, bool inHand)
{
	vec3f	offset (8, 8, Player->ViewHeight-8), start;
	const sint32		damage = CalcQuadVal(125);
	const float		radius = 165;

	Player->Client.Grenade.Thrown = true;

	anglef angles = Player->Client.ViewAngle.ToVectors ();
	Player->P_ProjectSource (offset, angles, start);

	FrameNumber timer = (float)(Player->Client.Grenade.Time - Level.Frame) / 10;
	const sint32 speed = (Player->Client.Persistent.Weapon) ? 
		(GRENADE_MINSPEED + ((GRENADE_TIMER/10) - timer) * ((GRENADE_MAXSPEED - GRENADE_MINSPEED) / (GRENADE_TIMER/10)))
		: 25; // If we're dead, don't toss it 5 yards.
	CGrenade::Spawn (Player, start, angles.Forward, damage, speed, (Player->Client.Grenade.Time - Level.Frame), radius, true, inHand);

	Player->Client.Grenade.Time = Level.Frame + ((((
#if CLEANCTF_ENABLED
	(Game.GameMode & GAME_CTF) || 
#endif
	DeathmatchFlags.dfDmTechs.IsEnabled()) && Player->ApplyHaste())
#if XATRIX_FEATURES
	|| isQuadFire
#endif
	) ? 5 : 10);

	DepleteAmmo(Player, 1);

	if (Player->Health <= 0 || Player->IsDead || Player->State.GetModelIndex() != 255) // VWep animations screw up corpses
		return;

	AttackSound (Player);

	if (Player->Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED)
	{
		Player->Client.Anim.Priority = ANIM_ATTACK;
		Player->State.GetFrame() = CPlayerEntity::FRAME_crattak1 - 1;
		Player->Client.Anim.EndFrame = CPlayerEntity::FRAME_crattak3;
	}
	else
	{
		Player->Client.Anim.Priority = ANIM_REVERSE;
		Player->State.GetFrame() = CPlayerEntity::FRAME_wave08;
		Player->Client.Anim.EndFrame = CPlayerEntity::FRAME_wave01;
	}
	Player->Client.PlayerState.GetGunFrame()++;
}

void CHandGrenade::Wait (CPlayerEntity *Player)
{
	Player->Client.Grenade.BlewUp = false;
	if (Level.Frame < Player->Client.Grenade.Time)
		return;

	if (!Player->IsDead)
		Player->Client.Grenade.Thrown = false;
	Player->Client.PlayerState.GetGunFrame()++;
}

void CHandGrenade::Fire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 11:
		Hold (Player);
		break;
	case 12:
		Player->Client.WeaponSound = 0;
		FireGrenade(Player, false);
		break;
	case 15:
		Wait(Player);
		break;
	}
}

bool CHandGrenade::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 5:
		Player->PlaySound (CHAN_WEAPON, SoundIndex("weapons/hgrena1b.wav"));
		return false;
	case 11:
	case 12:
	case 15:
		return true;
	}
	return false;
}

void CHandGrenade::WeaponGeneric (CPlayerEntity *Player)
{
	// Idea from Brazen source
	sint32 newFrame = -1, newState = -1;

	switch (Player->Client.WeaponState)
	{
	case WS_ACTIVATING:
		newFrame = IdleStart;
		newState = WS_IDLE;
		break;
	case WS_IDLE:
		if (Player->Client.NewWeapon && Player->Client.NewWeapon != this)
		{
			// We want to go away!
			newState = WS_DEACTIVATING;
			newFrame = DeactStart;
		}
		else if ((Player->Client.Buttons|Player->Client.LatchedButtons) & BUTTON_ATTACK)
		{
			Player->Client.LatchedButtons &= ~BUTTON_ATTACK;

			// We want to attack!
			// First call, check AttemptToFire
			if (AttemptToFire(Player))
			{
				// Got here, we can fire!
				Player->Client.PlayerState.GetGunFrame() = FireStart;
				Player->Client.WeaponState = WS_FIRING;
				Player->Client.Grenade.Time = 0;

				// We need to check against us right away for first-frame firing
				WeaponGeneric(Player);
				return;
			}
			else
			{
				OutOfAmmo(Player);
				NoAmmoWeaponChange (Player);
			}
		}

		// Either we are still idle or a failed fire.
		if (newState == -1)
		{
			if (Player->Client.PlayerState.GetGunFrame() == IdleEnd)
				newFrame = IdleStart;
			else
			{
				if (CanStopFidgetting(Player) && (randomMT()&15))
					newFrame = Player->Client.PlayerState.GetGunFrame();
			}
		}
		break;
	case WS_FIRING:
		// Check if this is a firing frame.
		if (CanFire(Player))
		{
			Fire(Player);

			// Now, this call above CAN change the underlying frame and state.
			// We need this block to make sure we are still doing what we are supposed to.
			newState = Player->Client.WeaponState;
			newFrame = Player->Client.PlayerState.GetGunFrame();
		}

		// Only do this if we haven't been explicitely set a newFrame
		// because we might want to keep firing beyond this point
		if (newFrame == -1 && Player->Client.PlayerState.GetGunFrame() == FireEnd)
		{
			if (!Player->Client.Persistent.Inventory.Has(Item))
			{
				NoAmmoWeaponChange (Player);
				Player->Client.Grenade.Time = 0;
				Player->Client.Grenade.Thrown = false;
			}
			else
			{
				Player->Client.Grenade.Time = 0;
				Player->Client.Grenade.Thrown = false;
				newFrame = IdleStart;
				newState = WS_IDLE;
			}
		}
		break;
	case WS_DEACTIVATING:
		// Change weapon
		ChangeWeapon (Player);
		return;
		break;
	}

	if (newFrame != -1)
		Player->Client.PlayerState.GetGunFrame() = newFrame;
	if (newState != -1)
		Player->Client.WeaponState = newState;

	if (newFrame == -1 && newState == -1)
		Player->Client.PlayerState.GetGunFrame()++;
}

WEAPON_DEFS (CHandGrenade);

void CHandGrenade::CreateItem (CItemList *List)
{
};

