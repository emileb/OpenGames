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
// cc_DeathmatchFlags.h
// This is so that I don't have to keep doing DeathmatchFlags.Integer(). Should be faster than manually re-getting the value every frame!
//

#include "Local.h"

CDeathmatchFlags DeathmatchFlags;	// dmflags

/**
\fn	CDeathmatchFlags::CDeathmatchFlags()

\brief	Default constructor.
		Initializes all dmflags to their default values.

\author	Paril
\date	29/05/2010
**/
CDeathmatchFlags::CDeathmatchFlags() :
  dfNoHealth (DF_NO_HEALTH),
  dfNoItems (DF_NO_ITEMS),
  dfWeaponsStay (DF_WEAPONS_STAY),
  dfNoFallingDamage (DF_NO_FALLING),
  dfInstantItems (DF_INSTANT_ITEMS),
  dfSameLevel (DF_SAME_LEVEL),
  dfSkinTeams (DF_SKINTEAMS),
  dfModelTeams (DF_MODELTEAMS),
  dfNoFriendlyFire (DF_NO_FRIENDLY_FIRE),
  dfSpawnFarthest (DF_SPAWN_FARTHEST),
  dfForceRespawn (DF_FORCE_RESPAWN),
  dfNoArmor (DF_NO_ARMOR),
  dfAllowExit (DF_ALLOW_EXIT),
  dfInfiniteAmmo (DF_INFINITE_AMMO),
  dfQuadDrop (DF_QUAD_DROP),
  dfFixedFov (DF_FIXED_FOV),
  dfQuadFireDrop (DF_QUADFIRE_DROP),
  dfNoMines (DF_NO_MINES),
  dfNoStackDouble (DF_NO_STACK_DOUBLE),
  dfNoNukes (DF_NO_NUKES),
  dfNoSpheres (DF_NO_SPHERES),
#if CLEANCTF_ENABLED
  dfCtfForceJoin (DF_CTF_FORCEJOIN),
  dfCtfArmorProtect (DF_ARMOR_PROTECT),
  dfCtfNoTech (DF_CTF_NO_TECH),
#endif
  dfDmTechs (DF_DM_TECHS)
{
};

/**
\fn	void CDeathmatchFlags::UpdateFlags (EDeathmatchFlags wantedFlags)
	
\brief	Updates the flags described by wantedFlags.
	
\author	Paril
\date	29/05/2010
	
\param	wantedFlags	The wanted flags. 
**/
void CDeathmatchFlags::UpdateFlags (EDeathmatchFlags wantedFlags)
{
	dfNoHealth.Check (wantedFlags);
	dfNoItems.Check (wantedFlags);
	dfWeaponsStay.Check (wantedFlags);
	dfNoFallingDamage.Check (wantedFlags);
	dfInstantItems.Check (wantedFlags);
	dfSameLevel.Check (wantedFlags);

	dfSkinTeams.Check (wantedFlags);
	dfModelTeams.Check (wantedFlags);
	dfNoFriendlyFire.Check (wantedFlags);
	dfSpawnFarthest.Check (wantedFlags);
	dfForceRespawn.Check (wantedFlags);
	dfNoArmor.Check (wantedFlags);
	dfAllowExit.Check (wantedFlags);
	dfInfiniteAmmo.Check (wantedFlags);
	dfQuadDrop.Check (wantedFlags);
	dfFixedFov.Check (wantedFlags);
	dfQuadFireDrop.Check (wantedFlags);
	dfNoMines.Check (wantedFlags);
	dfNoStackDouble.Check (wantedFlags);
	dfNoNukes.Check (wantedFlags);
	dfNoSpheres.Check (wantedFlags);
#if CLEANCTF_ENABLED
	dfCtfForceJoin.Check (wantedFlags);
	dfCtfArmorProtect.Check (wantedFlags);
	dfCtfNoTech.Check (wantedFlags);
#endif
	dfDmTechs.Check (wantedFlags);
};
