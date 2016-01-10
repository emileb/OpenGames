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
// cc_xatrix_phalanx.h
// 
//

#if !defined(CC_GUARD_CC_XATRIX_PHALANX_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_XATRIX_PHALANX_H

class CPhalanx : public IWeaponBase
{
public:
	CPhalanx();

	bool		CanFire	(CPlayerEntity *Player);
	bool		CanStopFidgetting (CPlayerEntity *Player);

	// The function called to "fire"
	void	Fire (CPlayerEntity *Player);

	WEAPON_CLASS_DEFS (CPhalanx);
};

class CPhalanxPlasma : public IFlyMissileProjectile, public IThinkableEntity, public ITouchableEntity
{
public:
	sint32		Damage, RadiusDamage;
	float		DamageRadius;

	CPhalanxPlasma ();
	CPhalanxPlasma (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CPhalanxPlasma)

	void SaveFields (CFile &File)
	{
		File.Write<sint32> (Damage);
		File.Write<sint32> (RadiusDamage);
		File.Write<float> (DamageRadius);

		IThinkableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
		IFlyMissileProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Damage = File.Read<sint32> ();
		RadiusDamage = File.Read<sint32> ();
		DamageRadius = File.Read<float> ();

		IThinkableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
		IFlyMissileProjectile::LoadFields (File);
	}

	void Think ();
	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	static CPhalanxPlasma *Spawn	(IBaseEntity *Spawner, vec3f start, vec3f dir,
									sint32 Damage, sint32 speed, float damage_radius, sint32 radius_damage);

	bool Run ();
};


#else
FILE_WARNING
#endif
