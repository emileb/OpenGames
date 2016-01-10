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
// cc_grapple.cpp
// CTF Grappling hook
//

#include "Local.h"
#if CLEANCTF_ENABLED
#include "Weapons/WeaponMain.h"

CGrapple::CGrapple() :
IWeaponBase(0, -1, "models/weapons/grapple/tris.md2", 0, 5, 6, 9,
		10, 31, 32, 36)
{
}

bool CGrapple::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 6:
		return true;
	}
	return false;
}

bool CGrapple::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 10:
	case 18:
	case 27:
		return true;
	}
	return false;
}

bool CGrapple::AttemptToFire (CPlayerEntity *Player)
{
	return true;
}

// Player is player
void CGrapple::PlayerResetGrapple(CPlayerEntity *Player)
{
	if (Player->Client.Grapple.Entity)
		Player->Client.Grapple.Entity->ResetGrapple ();
}

void CGrapple::Fire (CPlayerEntity *Player)
{
	vec3f	start, offset (24, 8, Player->ViewHeight-6);

	if (Player->Client.Grapple.State > CTF_GRAPPLE_STATE_FLY)
		return; // it's already out

	anglef angles = Player->Client.ViewAngle.ToVectors();
	Player->P_ProjectSource (offset, angles, start);

	Player->Client.KickOrigin = angles.Forward * -2;
	Player->Client.KickAngles.X = -1;

	Player->PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grfire.wav"), (Player->Client.Timers.SilencerShots) ? 51 : 255);
	CGrappleEntity::Spawn (Player, start, angles.Forward, 10, CTF_GRAPPLE_SPEED);
		
	Player->PlayerNoiseAt (start, PNOISE_WEAPON);
	FireAnimation(Player);

	Player->Client.PlayerState.GetGunFrame()++;
}

void CGrapple::WeaponGeneric (CPlayerEntity *Player)
{
	// Idea from Brazen source
	sint32 newFrame = -1, newState = -1;

	switch (Player->Client.WeaponState)
	{
	case WS_ACTIVATING:
		if (Player->Client.PlayerState.GetGunFrame() == ActivationEnd)
		{
			newFrame = IdleStart;
			newState = WS_IDLE;
		
			// if we just switched back to grapple, immediately go to fire frame
			if (Player->Client.Grapple.State > CTF_GRAPPLE_STATE_FLY)
			{
				newFrame = 9;
				newState = WS_FIRING;
			}
		}
		break;
	case WS_IDLE:
		if (Player->Client.Grapple.State == CTF_GRAPPLE_STATE_HANG+1)
		{
			Player->Client.Grapple.State = CTF_GRAPPLE_STATE_FLY;
			Player->PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grreset.wav"), (Player->Client.Timers.SilencerShots) ? 51 : 255);
		}
		if (Player->Client.NewWeapon && Player->Client.NewWeapon != this)
		{
			// We want to go away!
			newState = WS_DEACTIVATING;
			newFrame = DeactStart;
		}
		else if ((Player->Client.Buttons|Player->Client.LatchedButtons) & BUTTON_ATTACK)
		{
			Player->Client.LatchedButtons &= ~BUTTON_ATTACK;

			// This here is ugly, but necessary so that machinegun/chaingun/hyperblaster
			// get the right acceptance on first-frame-firing
			Player->Client.Buttons |= BUTTON_ATTACK;

			// We want to attack!
			// First call, check AttemptToFire
			if (AttemptToFire(Player))
			{
				// Got here, we can fire!
				Player->Client.PlayerState.GetGunFrame() = FireStart;
				Player->Client.WeaponState = WS_FIRING;

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
				if (CanStopFidgetting(Player) && (randomMT() & 15))
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
		// Go right away if we aren't holding attack
		else if (!(Player->Client.Buttons & BUTTON_ATTACK))
		{
			if (Player->Client.Grapple.Entity)
				Player->Client.Grapple.Entity->ResetGrapple ();
			newFrame = IdleStart+1;
			newState = WS_IDLE;
		}
		else if (Player->Client.NewWeapon && 
			Player->Client.Grapple.State > CTF_GRAPPLE_STATE_FLY)
		{
			// he wants to change weapons while grappled
			newState = WS_DEACTIVATING;
			newFrame = 32;
		}
		else if (newFrame == -1 && Player->Client.PlayerState.GetGunFrame() > FireEnd)
		{
			// Grapple shouldn't change unless we want it to
			if ((Player->Client.Buttons & BUTTON_ATTACK) && 
				Player->Client.Grapple.Entity)
				newFrame = Player->Client.PlayerState.GetGunFrame();
			else
			{
				newFrame = IdleStart+1;
				newState = WS_IDLE;
			}
		}
		break;
	case WS_DEACTIVATING:
		if (Player->Client.PlayerState.GetGunFrame() == DeactEnd)
		{
			// Change weapon
			ChangeWeapon (Player);
			return;
		}
		break;
	}

	if (newFrame != -1)
		Player->Client.PlayerState.GetGunFrame() = newFrame;
	if (newState != -1)
		Player->Client.WeaponState = newState;

	if (newFrame == -1 && newState == -1)
		Player->Client.PlayerState.GetGunFrame()++;
}

WEAPON_DEFS (CGrapple);

void CGrapple::CreateItem (CItemList *List)
{
	NItems::Grapple = QNew (TAG_GENERIC) CWeaponItem (NULL, NULL, 0, NULL, "w_grapple", "Grapple", ITEMFLAG_WEAPON|ITEMFLAG_USABLE, &Weapon, NULL, 0, "#w_grapple.md2");
};

#endif
