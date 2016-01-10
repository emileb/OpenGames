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
// cc_weaponentities.h
// Entities related to weaponry
//

#if !defined(CC_GUARD_WEAPONENTITIES_H) || !INCLUDE_GUARDS
#define CC_GUARD_WEAPONENTITIES_H

class CGrenade : public IBounceProjectile, public IThinkableEntity, public ITouchableEntity
{
public:
	sint32		Damage;
	float		RadiusDamage;

	CGrenade ();
	CGrenade (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CGrenade)

	void SaveFields (CFile &File)
	{
		File.Write<sint32> (Damage);
		File.Write<float> (RadiusDamage);

		IThinkableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
		IBounceProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Damage = File.Read<sint32> ();
		RadiusDamage = File.Read<float> ();

		IThinkableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
		IBounceProjectile::LoadFields (File);
	}

	void Think ();
	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);
	void PushInDirection (vec3f vel, uint32 flags);

	void Explode ();

	static void Spawn	(IBaseEntity *Spawner, vec3f start, vec3f aimdir,
						sint32 Damage, sint32 speed, FrameNumber timer, float damage_radius, bool handNade = false, bool held = false);

	bool Run ();
};

class CBlasterProjectile : public IFlyMissileProjectile, public IThinkableEntity, public ITouchableEntity
{
public:
	sint32		Damage;

	CBlasterProjectile ();
	CBlasterProjectile (sint32 Index);

	IMPLEMENT_SAVE_HEADER (CBlasterProjectile)

	void SaveFields (CFile &File)
	{
		File.Write<sint32> (Damage);

		IThinkableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
		IFlyMissileProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Damage = File.Read <sint32>();

		IThinkableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
		IFlyMissileProjectile::LoadFields (File);
	}

	void Think ();
	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	static void Spawn	(IBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed, sint32 effect, bool isHyper);

	bool Run ();
};

class CRocket : public IFlyMissileProjectile, public IThinkableEntity, public ITouchableEntity
{
public:
	sint32		Damage, RadiusDamage;
	float	DamageRadius;

	CRocket ();
	CRocket (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CRocket)

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

	static CRocket *Spawn	(IBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed, float damage_radius, sint32 radius_damage);

	bool Run ();
};

class CBFGBolt : public IFlyMissileProjectile, public IThinkableEntity, public ITouchableEntity
{
public:
	bool			Exploded;
	sint32			Damage;
	float			DamageRadius;
	FrameNumber	FreeTime;

	CBFGBolt ();
	CBFGBolt (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CBFGBolt)

	void SaveFields (CFile &File)
	{
		File.Write<bool> (Exploded);
		File.Write<sint32> (Damage);
		File.Write<float> (DamageRadius);
		File.Write<FrameNumber> (FreeTime);

		IThinkableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
		IFlyMissileProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Exploded = File.Read<bool> ();
		Damage = File.Read<sint32> ();
		DamageRadius = File.Read<float> ();
		FreeTime = File.Read<FrameNumber> ();

		IThinkableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
		IFlyMissileProjectile::LoadFields (File);
	}

	void Think ();
	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	static void Spawn	(IBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed, float damage_radius);

	bool Run ();
};

const int HITSCANSTEP = 100;

class CHitScan
{
public:
	sint32			Damage;
	sint32			Kick;
	bool			ThroughAndThrough;

	CHitScan (sint32 Damage, sint32 Kick, bool ThroughAndThrough) :
	Damage(Damage),
	Kick(Kick),
	ThroughAndThrough(ThroughAndThrough) {};

	virtual CTrace		DoTrace		(vec3f &start, vec3f &end, IBaseEntity *ignore, sint32 mask);
	virtual bool		DoDamage	(IBaseEntity *Attacker, IHurtableEntity *Target, vec3f &Dir, vec3f &Point, vec3f &Normal);
	virtual void		DoEffect	(vec3f &start, vec3f &end, bool water);
	virtual void		DoSolidHit	(CTrace *Trace);
	virtual void		DoWaterHit	(CTrace *Trace);
	virtual bool		ModifyEnd	(vec3f &aimDir, vec3f &start, vec3f &end);

	virtual void		DoFire		(IBaseEntity *Entity, vec3f start, vec3f aimdir);
};

class CRailGunShot : public CHitScan
{
public:
	CRailGunShot (sint32 Damage, sint32 Kick) :
	CHitScan (Damage, Kick, true) {};

	bool		DoDamage (IBaseEntity *Attacker, IHurtableEntity *Target, vec3f &Dir, vec3f &Point, vec3f &Normal);
	void		DoEffect (vec3f &start, vec3f &end, bool water);

	static void		Fire		(IBaseEntity *Entity, vec3f start, vec3f aimdir, sint32 Damage, sint32 kick);
};

class CBullet : public CHitScan
{
public:
	EMeansOfDeath MeansOfDeath;
	sint32 vSpread, hSpread;

	CBullet (sint32 Damage, sint32 Kick, sint32 hSpread, sint32 vSpread, sint32 mod) :
	CHitScan (Damage, Kick, false),
	vSpread(vSpread),
	hSpread(hSpread),
	MeansOfDeath(mod) {};

	bool					DoDamage (IBaseEntity *Attacker, IHurtableEntity *Target, vec3f &Dir, vec3f &Point, vec3f &Normal);
	virtual void			DoSolidHit	(CTrace *Trace);
	void					DoWaterHit	(CTrace *Trace);
	bool					ModifyEnd (vec3f &aimDir, vec3f &start, vec3f &end);
	void					DoEffect (vec3f &start, vec3f &end, bool water);
	virtual void			DoFire		(IBaseEntity *Entity, vec3f start, vec3f aimdir);

	static void				Fire		(IBaseEntity *Entity, vec3f start, vec3f aimdir, sint32 Damage, sint32 kick, sint32 hSpread, sint32 vSpread, sint32 mod);
};

class CShotgunPellets : public CBullet
{
public:
	CShotgunPellets (sint32 Damage, sint32 Kick, sint32 hSpread, sint32 vSpread, sint32 mod) :
	CBullet (Damage, Kick, hSpread, vSpread, mod)
	{
	};

	void					DoSolidHit (CTrace *Trace);

	static void				Fire		(IBaseEntity *Entity, vec3f start, vec3f aimdir, sint32 Damage, sint32 kick, sint32 hSpread, sint32 vSpread, sint32 Count, sint32 mod);
};

// FIXME: see todo.txt
class CMeleeWeapon
{
	CMeleeWeapon() {};

public:
	static bool		Fire (IBaseEntity *Entity, vec3f aim, sint32 Damage, sint32 kick);
};

class CPlayerMeleeWeapon
{
	CPlayerMeleeWeapon () {};

public:
	static void		Fire (CPlayerEntity *Entity, vec3f Start, vec3f Aim, int Reach, int Damage, int Kick, EMeansOfDeath Mod);
};

#if CLEANCTF_ENABLED
// Grappling class
class CGrappleEntity : public IFlyMissileProjectile, public ITouchableEntity
{
	CPlayerEntity		*Player;
	float				Damage;

public:
	CGrappleEntity ();
	CGrappleEntity (sint32 Index);

	void SaveFields (CFile &File);
	void LoadFields (CFile &File);

	IMPLEMENT_SAVE_HEADER(CGrappleEntity)

	void ResetGrapple ();
	void GrapplePull ();
	void GrappleDrawCable ();

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);
	bool Run ();

	static void Spawn (CPlayerEntity *Spawner, vec3f start, vec3f dir, sint32 Damage, sint32 speed);
};
#endif

void CheckDodge (IBaseEntity *self, vec3f &start, vec3f &Dir, sint32 speed);

#else
FILE_WARNING
#endif
