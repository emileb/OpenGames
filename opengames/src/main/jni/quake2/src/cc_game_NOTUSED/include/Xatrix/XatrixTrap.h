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
// cc_xatrix_trap.h
// 
//

#if !defined(CC_GUARD_CC_XATRIX_TRAP_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_XATRIX_TRAP_H

class CTrap : public IWeaponBase
{
public:
	CTrap();
	CTrap(char *model, sint32 FireStart, sint32 FireNumFrames,
						 sint32 IdleStart, sint32 IdleNumFrames);

	bool		CanFire	(CPlayerEntity *Player);
	bool		CanStopFidgetting (CPlayerEntity *Player);

	void	WeaponGeneric (CPlayerEntity *Player);

	// The function called to "fire"
	void	Fire (CPlayerEntity *Player);

	void	Hold (CPlayerEntity *Player);
	void	FireGrenade (CPlayerEntity *Player, bool inHand);
	void	Wait (CPlayerEntity *Player);

	WEAPON_CLASS_DEFS (CTrap);
};

class CTrapProjectile : public IBounceProjectile, public IThinkableEntity, public ITouchableEntity
{
public:
	FrameNumber		TimeStamp;
	int					Damage;
	int					Wait;
	FrameNumber		Delay;
	bool				DoFree;
	IBaseEntity			*TrapEntities[3];

	CTrapProjectile ();
	CTrapProjectile (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CTrapProjectile)

	void SaveFields (CFile &File)
	{
		File.Write<FrameNumber> (TimeStamp);
		File.Write<int> (Damage);
		File.Write<int> (Wait);
		File.Write<FrameNumber> (Delay);
		File.Write<bool> (DoFree);

		IThinkableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
		IBounceProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		TimeStamp = File.Read<FrameNumber> ();
		Damage = File.Read<int> ();
		Wait = File.Read<int> ();
		Delay = File.Read<FrameNumber> ();
		DoFree = File.Read<bool> ();

		IThinkableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
		IBounceProjectile::LoadFields (File);
	}

	void Think ();
	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	void Explode ();

	static void Spawn	(IBaseEntity *Spawner, vec3f start, vec3f aimdir, int damage, float timer, sint32 speed);

	bool Run ();
};

// Food cube
class CFoodCube : public IBasePowerUp
{
public:
	CFoodCube ();
	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
};

#else
FILE_WARNING
#endif
