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
// cc_debugweapons.h
// 
//

#if !defined(CC_GUARD_DEBUGWEAPONS_H) || !INCLUDE_GUARDS
#define CC_GUARD_DEBUGWEAPONS_H

class CDebugWeapon : public IWeaponBase
{
public:
	CDebugWeapon();

	bool		CanFire	(CPlayerEntity *Player)
	{
		return true;
	};
	bool		CanStopFidgetting (CPlayerEntity *Player)
	{
		return true;
	};
	
	// This function is called when the player hits the attack button.
	// Returns "true" if the animation can go ahead (check for ammo, etc here)
	inline bool	AttemptToFire (CPlayerEntity *Player)
	{
		return true;
	}; 

	// The function called to "fire"
	virtual void	Fire (CPlayerEntity *Player) = 0;
	void	Think (CPlayerEntity *Player);
};

class CSurfacePicker : public CDebugWeapon
{
public:
	CSurfacePicker ();

	void Fire (CPlayerEntity *Player);

	WEAPON_CLASS_DEFS (CSurfacePicker);
};

#else
FILE_WARNING
#endif
