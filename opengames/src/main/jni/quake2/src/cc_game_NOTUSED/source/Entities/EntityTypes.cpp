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
// cc_entitytypes.cpp
// Different base entity classes
//

#include "Local.h"
#include "Items/Items.h"

/**
\fn	IHurtableEntity::IHurtableEntity ()

\brief	New entity constructor. 

\author	Paril
\date	13/06/2010
**/
IHurtableEntity::IHurtableEntity () :
  IBaseEntity(),
  CanTakeDamage (false),
  AffectedByKnockback (true)
{
	EntityFlags |= EF_HURTABLE;
};

/**
\fn	IHurtableEntity::IHurtableEntity (sint32 Index)

\brief	Existing entity constructor. 

\author	Paril
\date	13/06/2010

\param	Index	Zero-based index of entity to use. 
**/
IHurtableEntity::IHurtableEntity (sint32 Index) :
  IBaseEntity(Index),
  CanTakeDamage (false),
  AffectedByKnockback (true)
{
	EntityFlags |= EF_HURTABLE;
};

ENTITYFIELDS_BEGIN(IHurtableEntity)
{
	CEntityField ("health", EntityMemberOffset(IHurtableEntity,Health), FT_INT | FT_SAVABLE),

	CEntityField ("CanTakeDamage", EntityMemberOffset(IHurtableEntity,CanTakeDamage), FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("IsDead", EntityMemberOffset(IHurtableEntity,IsDead), FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("AffectedByKnockback", EntityMemberOffset(IHurtableEntity,AffectedByKnockback), FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("MaxHealth", EntityMemberOffset(IHurtableEntity,MaxHealth), FT_INT | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("GibHealth", EntityMemberOffset(IHurtableEntity,GibHealth), FT_INT | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(IHurtableEntity)

bool			IHurtableEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<IHurtableEntity> (this, Key, Value))
		return true;

	// Couldn't find it here
	return false;
};

void			IHurtableEntity::SaveFields (CFile &File)
{
	SaveEntityFields<IHurtableEntity> (this, File);
};

void			IHurtableEntity::LoadFields (CFile &File)
{
	LoadEntityFields<IHurtableEntity> (this, File);
};

/**
\fn	bool IHurtableEntity::DamageCanReach (IBaseEntity *Inflictor)

\brief	Tests if Inflictor can damage this entity in a bounding box path. Used for radius
		damages. 

\author	Paril
\date	13/06/2010

\param [in,out]	Inflictor	If non-null, the inflictor. 

\return	true if it succeeds, false if it fails. 
**/
bool IHurtableEntity::DamageCanReach (IBaseEntity *Inflictor)
{
// bmodels need special checking because their origin is 0,0,0
	if ((EntityFlags & EF_PHYSICS) && ((entity_cast<IPhysicsEntity>(this))->PhysicsType == PHYSICS_PUSH))
	{
		vec3f dest = (GetAbsMin() + GetAbsMax()) * 0.5f;
		CTrace trace (Inflictor->State.GetOrigin(), dest, Inflictor, CONTENTS_MASK_SOLID);
		if (trace.Fraction == 1.0 || trace.Entity == this)
			return true;
		return false;
	}
	
	static const vec3f additions[] = 
	{
		vec3f(0, 0, 0),
		vec3f(15, 15, 0),
		vec3f(15, -15, 0),
		vec3f(-15, 15, 0),
		vec3f(-15, -15, 0)
	};

	for (sint32 i = 0; i < 5; i++)
	{
		vec3f end = State.GetOrigin() + additions[i];
		CTrace trace (Inflictor->State.GetOrigin(), end, Inflictor, CONTENTS_MASK_SOLID);
		if (trace.Fraction == 1.0)
			return true;
	};

	return false;
}

/**
\fn	bool IHurtableEntity::CheckTeamDamage (IBaseEntity *Attacker)

\brief	Tests if this entity and Attacker are on a team. 

\author	Paril
\date	13/06/2010

\param [in,out]	Attacker	If non-null, the attacker. 

\return	true if they are on a team, false if not. 
**/
bool IHurtableEntity::CheckTeamDamage (IBaseEntity *Attacker)
{
#if CLEANCTF_ENABLED
//ZOID
	if ((Game.GameMode & GAME_CTF) && (EntityFlags & EF_PLAYER) && (Attacker->EntityFlags & EF_PLAYER))
	{
		CPlayerEntity *Targ = entity_cast<CPlayerEntity>(this);
		CPlayerEntity *PlayerAttacker = entity_cast<CPlayerEntity>(Attacker);
		if (Targ->Client.Respawn.CTF.Team == PlayerAttacker->Client.Respawn.CTF.Team &&
			(this != PlayerAttacker))
			return true;
	}
//ZOID
#endif

	return false;
}

#include "Utility/TemporaryEntities.h"

/**
\fn	sint32 IHurtableEntity::CheckPowerArmor (vec3f &Point, vec3f &Normal, sint32 Damage,
	EDamageFlags DamageFlags)

\brief	Checks power armor and makes adjustments based on that. 

\author	Paril
\date	13/06/2010

\param [in,out]	Point	The point. 
\param [in,out]	Normal	The normal. 
\param	Damage			The damage. 
\param	DamageFlags		The damage flags. 

\return	The amount of damage saved from the power armor. 
**/
sint32 IHurtableEntity::CheckPowerArmor (vec3f &Point, vec3f &Normal, sint32 Damage, EDamageFlags DamageFlags)
{
	return 0;
}

/**
\fn	void IHurtableEntity::Killed (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage,
	vec3f &Point)

\brief	The function called when entity is killed. This calls Die, sets Enemy and clamps health
		to -999. 

\author	Paril
\date	13/06/2010

\param [in,out]	Inflictor	If non-null, the inflictor. 
\param [in,out]	Attacker	If non-null, the attacker. 
\param	Damage				The damage. 
\param [in,out]	Point		The point. 
**/
void IHurtableEntity::Killed (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	if (Health < -999)
		Health = -999;

	
	if (!Inflictor)
	{
		DebugPrintf("IHurtableEntity::Killed(): Null inflictor\n");
		Inflictor = World;
	}
	if (!Attacker)
	{
		DebugPrintf("IHurtableEntity::Killed(): Null Attacker\n");
		Attacker = World;
	}
	if (Attacker->EntityFlags & EF_HURTABLE)
		Enemy = Attacker;

	if (CanTakeDamage)
		Die (Inflictor, Attacker, Damage, Point);
}

/**
\fn	void IHurtableEntity::DamageEffect (vec3f &Dir, vec3f &Point, vec3f &Normal, sint32 &Damage,
	EDamageFlags &DamageFlags, EMeansOfDeath &MeansOfDeath)

\brief	An "extension" of sorts to TakeDamage which handles the effects when we are hurt. 

\author	Paril
\date	13/06/2010

\param [in,out]	Dir				The dir. 
\param [in,out]	Point			The point. 
\param [in,out]	Normal			The normal. 
\param [in,out]	Damage			The damage. 
\param [in,out]	DamageFlags		The damage flags. 
\param [in,out]	MeansOfDeath	The means of death. 
**/
void IHurtableEntity::DamageEffect (vec3f &Dir, vec3f &Point, vec3f &Normal, sint32 &Damage, EDamageFlags &DamageFlags, EMeansOfDeath &MeansOfDeath)
{
	if ((EntityFlags & EF_MONSTER) || (EntityFlags & EF_PLAYER))
	{
#if ROGUE_FEATURES
		if (MeansOfDeath == MOD_CHAINFIST)
			CBlood(Point, Normal, BT_MORE_BLOOD).Send();
		else
#endif
		CBlood(Point, Normal).Send();
	}
	else
		CSparks(Point, Normal, (DamageFlags & DAMAGE_BULLET) ? ST_BULLET_SPARKS : ST_SPARKS, SPT_SPARKS).Send();
}

bool LastPelletShot = true;

/**
\fn	void IHurtableEntity::TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker, vec3f Dir,
	vec3f Point, vec3f Normal, sint32 Damage, sint32 KnockBack, EDamageFlags DamageFlags,
	EMeansOfDeath MeansOfDeath)

\brief	Makes this entity take damage. 

\author	Paril
\date	13/06/2010

\param [in,out]	Inflictor	If non-null, the inflictor. 
\param [in,out]	Attacker	If non-null, the attacker. 
\param	Dir					The dir. 
\param	Point				The point. 
\param	Normal				The normal. 
\param	Damage				The damage. 
\param	KnockBack			The knock back. 
\param	DamageFlags			The damage flags. 
\param	MeansOfDeath		The means of death. 
**/
void IHurtableEntity::TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker,
							vec3f Dir, vec3f Point, vec3f Normal, sint32 Damage,
							sint32 KnockBack, EDamageFlags DamageFlags, EMeansOfDeath MeansOfDeath)
{
	if (CvarList[CV_MAP_DEBUG].Boolean())
		return;

	sint32			take;
	sint32			save;
	sint32			asave = 0;
	sint32			psave = 0;

	// Needed?
	if (!CanTakeDamage)
		return;

	meansOfDeath = MeansOfDeath;
	Dir.Normalize ();

	if (DeathmatchFlags.dfDmTechs.IsEnabled()
#if CLEANCTF_ENABLED
	|| (Game.GameMode & GAME_CTF)
#endif
	)
	{
		if (Attacker->EntityFlags & EF_PLAYER)
		{
			CPlayerEntity *Atk = entity_cast<CPlayerEntity>(Attacker);
			if (Atk->Client.Persistent.Tech && (Atk->Client.Persistent.Tech->TechType == CTech::TECH_AGGRESSIVE))
				entity_cast<CPlayerEntity>(Attacker)->Client.Persistent.Tech->DoAggressiveTech (Atk, this, false, Damage, KnockBack, DamageFlags, MeansOfDeath, false);
		}
	}

	if (!AffectedByKnockback)
		KnockBack = 0;

// figure momentum add
	if (KnockBack && !(DamageFlags & DAMAGE_NO_KNOCKBACK) && (EntityFlags & EF_PHYSICS))
	{
		IPhysicsEntity *Phys = entity_cast<IPhysicsEntity>(this);
		if (!(Phys->PhysicsType == PHYSICS_NONE ||
			Phys->PhysicsType == PHYSICS_BOUNCE ||
			Phys->PhysicsType == PHYSICS_PUSH ||
			Phys->PhysicsType == PHYSICS_STOP))
			Phys->Velocity += Dir * (500.0f * (float)KnockBack / Clamp<float> (Phys->Mass, 50, Phys->Mass));
	}

	take = Damage;
	save = 0;

	// check for godmode
	if ((Flags & FL_GODMODE) && !(DamageFlags & DAMAGE_NO_PROTECTION))
	{
		take = 0;
		save = Damage;
		CSparks (Point, Normal, (DamageFlags & DAMAGE_BULLET) ? ST_BULLET_SPARKS : ST_SPARKS, SPT_SPARKS).Send();
	}

	psave = CheckPowerArmor (Point, Normal, take, DamageFlags);
	take -= psave;

	//treat cheat/powerup savings the same as armor
	asave += save;

	// team damage avoidance
	if (!(DamageFlags & DAMAGE_NO_PROTECTION) && CheckTeamDamage (Attacker))
		return;

// ROGUE - this option will do damage both to the armor and person. originally for DPU rounds
	if (DamageFlags & DAMAGE_DESTROY_ARMOR)
	{
		if (!(Flags & FL_GODMODE) && !(DamageFlags & DAMAGE_NO_PROTECTION))
			take = Damage;
	}
// ROGUE

// do the damage
	if (take)
	{
		DamageEffect (Dir, Point, Normal, take, DamageFlags, MeansOfDeath);
		Health -= take;
	}

	if (Health <= 0)
	{
		Killed (Inflictor, Attacker, take, Point);
		return;
	}

	if (take)
		Pain (Attacker, take);
}

/**
\fn	IThinkableEntity::IThinkableEntity ()

\brief	New entity constructor. 

\author	Paril
\date	13/06/2010
**/
IThinkableEntity::IThinkableEntity () :
  IBaseEntity()
{
	EntityFlags |= EF_THINKABLE;

	ThinkEntity = this;
};

/**
\fn	IThinkableEntity::IThinkableEntity (sint32 Index)

\brief	Existing entity constructor. 

\author	Paril
\date	13/06/2010

\param	Index	Zero-based index of the entity to use. 
**/
IThinkableEntity::IThinkableEntity (sint32 Index) :
  IBaseEntity(Index)
{
	EntityFlags |= EF_THINKABLE;

	ThinkEntity = this;
};

void IThinkableEntity::SaveFields (CFile &File)
{
	// Save NextThink
	File.Write<FrameNumber> (NextThink);
};

void IThinkableEntity::LoadFields (CFile &File)
{
	// Load NextThink
	NextThink = File.Read<FrameNumber> ();
};

/**
\fn	void IThinkableEntity::RunThink ()

\brief	Function that runs the think process. 
\author	Paril. 
\date	13/06/2010. 
**/
void IThinkableEntity::RunThink ()
{
	if (NextThink <= 0 || NextThink > Level.Frame)
		return;
	
	NextThink = 0;
	Think ();
}

ITouchableEntity::ITouchableEntity () :
IBaseEntity()
{
	EntityFlags |= EF_TOUCHABLE;
};

ITouchableEntity::ITouchableEntity (sint32 Index) :
IBaseEntity(Index)
{
	EntityFlags |= EF_TOUCHABLE;
};

void ITouchableEntity::SaveFields (CFile &File)
{
	File.Write<bool> (Touchable);
};

void ITouchableEntity::LoadFields (CFile &File)
{
	Touchable = File.Read<bool> ();
};

/**
\fn	IPhysicsEntity::IPhysicsEntity ()

\brief	New entity constructor. 

\author	Paril
\date	13/06/2010
**/
IPhysicsEntity::IPhysicsEntity () :
  IBaseEntity(),
  GravityMultiplier(1.0f),
  PhysicsFlags(0),
  DampeningEffect(1, 1, 1),
  ADampeningEffect(1, 1, 1),
  GravityVector(0, 0, -1)
{
	EntityFlags |= EF_PHYSICS;
	PhysicsType = PHYSICS_NONE;
};

/**
\fn	IPhysicsEntity::IPhysicsEntity (sint32 Index)

\brief	Existing entity constructor. 

\author	Paril
\date	13/06/2010

\param	Index	Zero-based index of the entity to use. 
**/
IPhysicsEntity::IPhysicsEntity (sint32 Index) :
  IBaseEntity(Index),
  GravityMultiplier(1.0f),
  PhysicsFlags(0),
  DampeningEffect(1, 1, 1),
  ADampeningEffect(1, 1, 1),
  GravityVector(0, 0, -1)
{
	EntityFlags |= EF_PHYSICS;
	PhysicsType = PHYSICS_NONE;
};

/**
\fn	void IPhysicsEntity::AddGravity()

\brief	Adds gravity to this entity's velocity. 
**/
void IPhysicsEntity::AddGravity()
{
	if (GravityVector[2] > 0)
		Velocity = Velocity.MultiplyAngles (GravityMultiplier * CvarList[CV_GRAVITY].Float() * 0.1f, GravityVector);
	else
		Velocity.Z -= GravityMultiplier * CvarList[CV_GRAVITY].Float() * 0.1f;
}

/**
\fn	CTrace IPhysicsEntity::PushEntity (vec3f &Push)

\brief	Causes this entity to move and push/impact any obstacles. 

\author	Paril
\date	13/06/2010

\param [in,out]	Push	The push vector. 

\return	The last trace to be processed by the pushing function. 
**/
CTrace IPhysicsEntity::PushEntity (vec3f &Push)
{
	vec3f		Start = State.GetOrigin();
	vec3f		End = Start + Push;

	CTrace Trace;
	while (true)
	{
		Trace (Start, GetMins(), GetMaxs(), End, this, (GetClipmask()) ? GetClipmask() : CONTENTS_MASK_SOLID);
		
		State.GetOrigin() = Trace.EndPosition;
		Link();

		if (Trace.Fraction != 1.0)
		{
			Impact (&Trace);

			// if the pushed entity went away and the pusher is still there
			if (!Trace.Entity->GetInUse() && GetInUse())
			{
				// move the pusher back and try again
				State.GetOrigin() = Start;
				Link ();
				continue;
			}
		}
		break;
	}

	if (GetInUse())
		TouchTriggers ();

	return Trace;
}

/**
\fn	void IPhysicsEntity::Impact (CTrace *Trace)

\brief	Causes this entity to impact with the entity hit by Trace. Calls the Touch functions, if
		applicable. 

\author	Paril
\date	13/06/2010

\param [in,out]	Trace	If non-null, the trace. 
**/
void IPhysicsEntity::Impact (CTrace *Trace)
{
	if (!Trace->Entity)
		return;

	if (GetSolid() != SOLID_NOT && (EntityFlags & EF_TOUCHABLE))
	{
		ITouchableEntity *Touched = entity_cast<ITouchableEntity>(this);

		if (Touched->Touchable)
			Touched->Touch (Trace->Entity, &Trace->Plane, Trace->Surface);
	}

	if ((Trace->Entity->EntityFlags & EF_TOUCHABLE) && Trace->Entity->GetSolid() != SOLID_NOT)
	{
		ITouchableEntity *Touched = entity_cast<ITouchableEntity>(Trace->Entity);

		if (Touched->Touchable)
			Touched->Touch (this, NULL, NULL);
	}
}

/**
\fn	void IPhysicsEntity::PushInDirection (vec3f Vel, ESpawnflags Flags)

\brief	Pushes this entity in speed vector 'Vel'. The spawnflags of the pusher entity can be
		passed via Flags to modify velocity or otherwise. 

\author	Paril
\date	13/06/2010

\param	Vel		The vel. 
\param	Flags	The flags. 
**/
void IPhysicsEntity::PushInDirection (vec3f Vel, ESpawnflags Flags)
{
	if ((EntityFlags & EF_HURTABLE) && (entity_cast<IHurtableEntity>(this)->Health > 0))
		Velocity = Vel;
}

/**
\fn	IBounceProjectile::IBounceProjectile ()

\brief	New entity constructor. 

\author	Paril
\date	14/06/2010
**/
IBounceProjectile::IBounceProjectile () :
  IPhysicsEntity (),
  BackOff(1.5f),
  AffectedByGravity(true),
  StopOnEqualPlane(true),
  AimInVelocityDirection(false)
{
	PhysicsType = PHYSICS_BOUNCE;
}

/**
\fn	IBounceProjectile::IBounceProjectile (sint32 Index)

\brief	Existing entity constructor. 

\author	Paril
\date	14/06/2010

\param	Index	Zero-based index of the entity to use. 
**/
IBounceProjectile::IBounceProjectile (sint32 Index) :
  IBaseEntity(Index),
  IPhysicsEntity (Index),
  BackOff(1.5f),
  AffectedByGravity(true),
  StopOnEqualPlane(true),
  AimInVelocityDirection(false)
{
	PhysicsType = PHYSICS_BOUNCE;
}

const float STOP_EPSILON    = 0.1f;

/**
\fn	vec3f ClipVelocity (vec3f &InVelocity, vec3f &Normal, float OverBounce)

\brief	Slide off impacting objec. 

\author	Paril
\date	14/06/2010

\param [in,out]	InVelocity		The in velocity. 
\param [in,out]	Normal			The colliding normal. 
\param	OverBounce				The overbounce value. 

\return	New velocity. 
**/
vec3f ClipVelocity (vec3f &InVelocity, vec3f &Normal, float OverBounce)
{
	float backoff = (InVelocity | Normal) * OverBounce;

	vec3f outVelocity;
	for (sint32 i = 0; i < 3; i++)
	{
		float change = Normal[i]*backoff;
		outVelocity[i] = InVelocity[i] - change;
		if (outVelocity[i] > -STOP_EPSILON && outVelocity[i] < STOP_EPSILON)
			outVelocity[i] = 0;
	}
	return outVelocity;
}

/**
\fn	bool IBounceProjectile::Run ()

\brief	Runs this object's physics. 

\return	true if it succeeds, false if it fails. 
**/
bool IBounceProjectile::Run ()
{
	CTrace	trace;
	vec3f		move, old_origin;
	bool		wasinwater;
	bool		isinwater;

	// if not a team captain, so movement will be handled elsewhere
	if (IsSlave())
		return false;

	if (Velocity.Z > 0)
		GroundEntity = nullentity;

// check for the groundentity going away
	if (!GroundEntity)
		GroundEntity = nullentity;

// if onground, return without moving
	if (GroundEntity && GravityMultiplier > 0.0)
		return false;

	old_origin = State.GetOrigin();

// add gravity
	if (AffectedByGravity)
		AddGravity ();

// move angles
	State.GetAngles() = State.GetAngles().MultiplyAngles (0.1f, AngularVelocity);

// move origin
	move = Velocity * 0.1f;

	trace = PushEntity (move);
	if (!GetInUse() || (PhysicsType != PHYSICS_BOUNCE && PhysicsType != PHYSICS_TOSS && PhysicsType != PHYSICS_FLYMISSILE))
		return false;

	if (trace.Fraction < 1)
	{
		Velocity = ClipVelocity (Velocity, trace.Plane.Normal, BackOff);

		if (AimInVelocityDirection)
			State.GetAngles() = Velocity.ToAngles();

		// stop if on ground
		if (trace.Plane.Normal.Z > 0.7 && StopOnEqualPlane)
		{		
			if (Velocity.Z < 60)
			{
				GroundEntity = trace.Entity;
				GroundEntityLinkCount = GroundEntity->GetLinkCount();
				Velocity.Clear ();
				AngularVelocity.Clear ();
			}
		}
	}
	
// check for water transition
	wasinwater = (WaterInfo.Type & CONTENTS_MASK_WATER) ? true : false;
	WaterInfo.Type = PointContents (State.GetOrigin ());
	isinwater = (WaterInfo.Type & CONTENTS_MASK_WATER) ? true : false;

	EWaterLevel newLevel = (isinwater) ? WATER_FEET : WATER_NONE;
	if (WaterInfo.Level != newLevel)
	{
		WaterInfo.OldLevel = WaterInfo.Level;
		WaterInfo.Level = newLevel;
	}

	if (!wasinwater && isinwater)
		World->PlayPositionedSound (old_origin, CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));
	else if (wasinwater && !isinwater)
		World->PlayPositionedSound (State.GetOrigin (), CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));

// move teamslaves
	for (IBaseEntity *slave = Team.Chain; slave; slave = slave->Team.Chain)
	{
		slave->State.GetOrigin() = State.GetOrigin();
		slave->Link ();
	}

	// Run physics modifiers
	if (PhysicsFlags & PHYSICFLAG_BUOYANCY)
		entity_cast<IBuoyancyPhysics>(this)->RunBouyancy ();

	if (PhysicsFlags & PHYSICFLAG_RESISTANCE)
		entity_cast<IResistancePhysics>(this)->RunResistance ();

	if (PhysicsFlags & PHYSICFLAG_AERODYNAMICS)
		entity_cast<IAerodynamicPhysics>(this)->RunAerodynamics();

	// Use data
	Velocity *= DampeningEffect;
	Velocity += VelocityEffect;	
	AngularVelocity *= ADampeningEffect;
	AngularVelocity += AVelocityEffect;	
	
	// Reset data
	VelocityEffect = AVelocityEffect = vec3fOrigin;
	DampeningEffect = ADampeningEffect = vec3f (1, 1, 1);

	return true;
}

ITossProjectile::ITossProjectile () :
  IBounceProjectile ()
{
	BackOff = 1.0f;

	PhysicsType = PHYSICS_TOSS;
}

ITossProjectile::ITossProjectile (sint32 Index) :
  IBaseEntity (Index),
  IBounceProjectile (Index)
{
	BackOff = 1.0f;

	PhysicsType = PHYSICS_TOSS;
}

IFlyMissileProjectile::IFlyMissileProjectile () :
  IBounceProjectile ()
{
	AffectedByGravity = false;
	BackOff = 1.0f;
	PhysicsType = PHYSICS_FLYMISSILE;
}

IFlyMissileProjectile::IFlyMissileProjectile (sint32 Index) :
  IBaseEntity (Index),
  IBounceProjectile (Index)
{
	AffectedByGravity = false;
	BackOff = 1.0f;
	PhysicsType = PHYSICS_FLYMISSILE;
}

/**
\fn	IStepPhysics::IStepPhysics ()

\brief	New entity constructor. 

\author	Paril
\date	14/06/2010
**/
IStepPhysics::IStepPhysics () :
  IPhysicsEntity ()
{
	PhysicsType = PHYSICS_STEP;
}

/**
\fn	IStepPhysics::IStepPhysics (sint32 Index)

\brief	Existing entity constructor. 

\author	Paril
\date	14/06/2010

\param	Index	Zero-based index of the entity to use. 
**/
IStepPhysics::IStepPhysics (sint32 Index) :
  IBaseEntity (Index),
  IPhysicsEntity (Index)
{
	PhysicsType = PHYSICS_STEP;
}

/**
\fn	void IStepPhysics::CheckGround ()

\brief	Check ground virtual function. Provides the code that checks if an entity is on-ground. 
**/
void IStepPhysics::CheckGround ()
{
	if (Velocity.Z > 100)
	{
		GroundEntity = nullentity;
		return;
	}

// if the hull point one-quarter unit down is solid the entity is on ground
	vec3f point = State.GetOrigin();
	point.Z -= 0.25f;

	CTrace trace (State.GetOrigin(), GetMins(), GetMaxs(), point, this, CONTENTS_MASK_MONSTERSOLID);

	// check steepness
	if (trace.Plane.Normal.Z < 0.7 && !trace.StartSolid)
	{
		GroundEntity = nullentity;
		return;
	}

	if (!trace.StartSolid && !trace.AllSolid)
	{
		State.GetOrigin() = trace.EndPosition;
		GroundEntity = trace.Entity;
		GroundEntityLinkCount = trace.Entity->GetLinkCount();
		Velocity.Z = 0;
	}
}

const float SV_STOPSPEED		= 100.0f;
const float SV_FRICTION			= 6.0f;
const float SV_WATERFRICTION	= 1.0f;

/**
\fn	void IStepPhysics::AddRotationalFriction ()

\brief	Adds rotational friction. 
**/
void IStepPhysics::AddRotationalFriction ()
{
	State.GetAngles() = State.GetAngles().MultiplyAngles (0.1f, AngularVelocity);

	float adjustment = 0.1f * SV_STOPSPEED * SV_FRICTION;
	for (sint32 n = 0; n < 3; n++)
	{
		if (AngularVelocity[n] > 0)
		{
			AngularVelocity[n] -= adjustment;
			if (AngularVelocity[n] < 0)
				AngularVelocity[n] = 0;
		}
		else
		{
			AngularVelocity[n] += adjustment;
			if (AngularVelocity[n] > 0)
				AngularVelocity[n] = 0;
		}
	}
}

const int MAX_CLIP_PLANES	= 5;

/**
\fn	sint32 IStepPhysics::FlyMove (float Time, EBrushContents Mask)

\brief	Performs the move. 

\author	Paril
\date	14/06/2010

\param	Time	The time to complete the move. 
\param	Mask	The content mask. 

\return	The blocked flags. 
**/
sint32 IStepPhysics::FlyMove (float Time, EBrushContents Mask)
{
	sint32		i, j, blocked = 0, numplanes = 0, numbumps = 4;
	vec3f		planes[MAX_CLIP_PLANES];
	float		time_left = Time;
	vec3f		original_velocity = Velocity, primal_velocity = Velocity;

	GroundEntity = nullentity;

	for (sint32 bumpcount = 0; bumpcount < numbumps; bumpcount++)
	{
		vec3f end = State.GetOrigin () + time_left * Velocity;

		CTrace trace (State.GetOrigin (), GetMins(), GetMaxs(), end, this, Mask);

		if (trace.AllSolid)
		{
			// entity is trapped in another solid
			Velocity.Clear ();
			return 3;
		}

		if (trace.Fraction > 0)
		{
			// actually covered some distance
			State.GetOrigin() = trace.EndPosition;
			original_velocity = Velocity;
			numplanes = 0;
		}

		if (trace.Fraction == 1)
			 break;		// moved the entire distance

		IBaseEntity *hit = trace.Entity;

		if (trace.Plane.Normal.Z > 0.7f)
		{
			blocked |= 1;		// floor
			if (hit->GetSolid() == SOLID_BSP)
			{
				GroundEntity = hit;
				GroundEntityLinkCount = GroundEntity->GetLinkCount();
			}
		}

		if (!trace.Plane.Normal.Z)
			blocked |= 2;		// step

//
// run the impact function
//
		Impact (&trace);
		if (!GetInUse() || PhysicsType != PHYSICS_STEP)
			break;		// removed by the impact function
	
		time_left -= time_left * trace.Fraction;
		
	// cliped to another plane
		if (numplanes >= MAX_CLIP_PLANES)
		{
			// this shouldn't really happen
			Velocity.Clear ();
			return 3;
		}

		planes[numplanes++] = trace.Plane.Normal;

//
// modify original_velocity so it parallels all of the clip planes
//
		vec3f new_velocity;
		for (i = 0; i < numplanes; i++)
		{
			new_velocity = ClipVelocity (original_velocity, planes[i], 1);

			for (j = 0; j < numplanes; j++)
			{
				if ((j != i) && planes[i] != planes[j])
				{
					if ((new_velocity | planes[j]) < 0)
						break;	// not ok
				}
			}

			if (j == numplanes)
				break;
		}
		
		if (i != numplanes)
			// go along this plane
			Velocity = new_velocity;
		else
		{
			// go along the crease
			if (numplanes != 2)
			{
				Velocity.Clear ();
				return 7;
			}

			vec3f dir = planes[0] ^ planes[1];
			Velocity = dir * (dir | Velocity);
		}

//
// if original velocity is against the original velocity, stop dead
// to avoid tiny occilations in sloping corners
//
		if ((Velocity | primal_velocity) <= 0)
		{
			Velocity.Clear ();
			return blocked;
		}
	}

	return blocked;
}

/**
\fn	bool IStepPhysics::Run ()

\brief	Runs this object's physics. 

\return	true if it succeeds, false if it fails. 
**/
bool IStepPhysics::Run ()
{
	bool		hitsound = false;
	float		speed, newspeed, control;
	float		friction;
	vec3f		saveOrigin = State.GetOrigin();

	if (PhysicsDisabled)
		return false;

	// airborn monsters should always check for ground
	if (!GroundEntity && (EntityFlags & EF_MONSTER))
		(entity_cast<CMonsterEntity>(this))->Monster->CheckGround ();
	else if (!(EntityFlags & EF_MONSTER))
		CheckGround (); // Specific non-monster checkground

	bool wasonground = GroundEntity;
		
	if (AngularVelocity != vec3fOrigin)
		AddRotationalFriction ();

	CMonsterEntity	*Monster = (EntityFlags & EF_MONSTER) ? entity_cast<CMonsterEntity>(this) : NULL;

	// add gravity except:
	//   flying monsters
	//   swimming monsters who are in the water
	if (Monster)
	{
		if (!wasonground)
		{
			if (!(Monster->Monster->AIFlags & AI_FLY))
			{
				if (!((Monster->Monster->AIFlags & AI_SWIM) && (WaterInfo.Level > WATER_WAIST)))
				{
					if (Velocity.Z < CvarList[CV_GRAVITY].Float() * -0.1)
						hitsound = true;
					if (WaterInfo.Level == WATER_NONE)
						AddGravity ();
				}
			}
		}

		// friction for flying monsters that have been given vertical velocity
		if ((Monster->Monster->AIFlags & AI_FLY) && (Velocity.Z != 0))
		{
			speed = Abs(Velocity.Z);
			control = (speed < SV_STOPSPEED) ? SV_STOPSPEED : speed;
			friction = SV_FRICTION/3;
			newspeed = speed - (0.1f * control * friction);
			if (newspeed < 0)
				newspeed = 0;
			newspeed /= speed;
			Velocity.Z *= newspeed;
		}

		// friction for flying monsters that have been given vertical velocity
		if ((Monster->Monster->AIFlags & AI_SWIM) && (Velocity.Z != 0))
		{
			speed = Abs(Velocity.Z);
			control = (speed < SV_STOPSPEED) ? SV_STOPSPEED : speed;
			newspeed = speed - (0.1f * control * SV_WATERFRICTION * WaterInfo.Level);
			if (newspeed < 0)
				newspeed = 0;
			newspeed /= speed;
			Velocity.Z *= newspeed;
		}
	}
	else
	{
		if (!wasonground)
		{
			if (WaterInfo.Level > WATER_WAIST)
			{
				if (Velocity.Z < CvarList[CV_GRAVITY].Float() * -0.1)
					hitsound = true;
				if (WaterInfo.Level == WATER_NONE)
					AddGravity ();
			}
		}
	}

	if (Velocity != vec3fOrigin)
	{
		// apply friction
		// let dead monsters who aren't completely onground slide
		if (Monster && ((wasonground) || (Monster->Monster->AIFlags & (AI_SWIM | AI_FLY))) &&
			!((Monster->Health <= 0 && !Monster->Monster->CheckBottom())))
		{
			speed = sqrtf(Velocity.X*Velocity.X + Velocity.Y*Velocity.Y);
			if (speed)
			{
				friction = SV_FRICTION;

				control = (speed < SV_STOPSPEED) ? SV_STOPSPEED : speed;
				newspeed = speed - 0.1f*control*friction;

				if (newspeed < 0)
					newspeed = 0;
				newspeed /= speed;

				Velocity.X *= newspeed;
				Velocity.Y *= newspeed;
			}
		}

		FlyMove (0.1f,  (EntityFlags & EF_MONSTER) ? CONTENTS_MASK_MONSTERSOLID : CONTENTS_MASK_SOLID);

		Link();

		GravityMultiplier = 1.0f;

		TouchTriggers ();
		if (!GetInUse())
			return false;

		if (GroundEntity && !wasonground && hitsound)
			PlaySound (CHAN_AUTO, SoundIndex("world/land.wav"));
	}

	// if we moved, check and fix origin if needed
	if (State.GetOrigin() != saveOrigin)
	{
		CTrace tr (State.GetOrigin(), GetMins(), GetMaxs(), saveOrigin, this, CONTENTS_MASK_MONSTERSOLID);
		if(tr.AllSolid || tr.StartSolid)
		{
			State.GetOrigin() = saveOrigin;
			Link ();
		}
	}

	return true;
}

// Move physics
IPushPhysics::IPushPhysics() :
  IPhysicsEntity()
{
	PhysicsType = PHYSICS_PUSH;
};

IPushPhysics::IPushPhysics(sint32 Index) :
  IBaseEntity (Index),
  IPhysicsEntity(Index)
{
	PhysicsType = PHYSICS_PUSH;
};

class CPushed
{
public:
	IBaseEntity		*Entity;
	float			DeltaYaw;
	vec3f			Origin;
	vec3f			Angles;

	CPushed () :
	Entity(NULL),
	DeltaYaw(0),
	Origin(),
	Angles()
	{
	};

	CPushed (IBaseEntity *Entity, float DeltaYaw, vec3f Origin, vec3f Angles) :
	Entity(Entity),
	DeltaYaw(DeltaYaw),
	Origin(Origin),
	Angles(Angles)
	{
	};
};

IBaseEntity *obstacle;

/*
============
SV_TestEntityPosition

============
*/
inline IBaseEntity *SV_TestEntityPosition (IBaseEntity *Entity)
{
	return (CTrace(Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), Entity->State.GetOrigin(), Entity, (Entity->GetClipmask()) ? Entity->GetClipmask() : CONTENTS_MASK_SOLID).StartSolid) ? World : NULL;
}

typedef std::vector<CPushed> TPushedList;

bool Push (TPushedList &Pushed, IBaseEntity *Entity, vec3f &move, vec3f &amove)
{
	// clamp the move to 1/8 units, so the position will
	// be accurate for client side prediction
	for (sint32 i = 0; i < 3; i++)
	{
		float temp = (move[i]*8.0);
		move[i] = 0.125 * (sint32)((temp > 0.0f) ? (temp + 0.5f) : (temp -0.5f));
	}

	// find the bounding box
	vec3f mins = Entity->GetAbsMin() + move;
	vec3f maxs = Entity->GetAbsMax() + move;

	// we need this for pushing things later
	anglef angles = (-amove).ToVectors ();

	// save the pusher's original position
	Pushed.push_back (CPushed	(Entity,
								(Entity->EntityFlags & EF_PLAYER) ? (entity_cast<CPlayerEntity>(Entity))->Client.PlayerState.GetPMove()->DeltaAngles[YAW] : 0,
								Entity->State.GetOrigin(), Entity->State.GetAngles()
								));

	// move the pusher to it's final position
	Entity->State.GetOrigin() += move;
	Entity->State.GetAngles() += amove;
	Entity->Link ();

	// see if any solid entities are inside the final position
	for (TEntitiesContainer::iterator it = Level.Entities.Closed.begin()++; it != Level.Entities.Closed.end(); ++it)
	{
		IBaseEntity *Check = (*it)->Entity;

		if (!Check || !Check->GetInUse())
			continue;

		if (Check->EntityFlags & EF_PHYSICS)
		{
			IPhysicsEntity *CheckPhys = entity_cast<IPhysicsEntity>(Check);
			if (CheckPhys->PhysicsType == PHYSICS_PUSH
				|| CheckPhys->PhysicsType == PHYSICS_STOP
				|| CheckPhys->PhysicsType == PHYSICS_NONE
				|| CheckPhys->PhysicsType == PHYSICS_NOCLIP)
				continue;
		}
		else if (Check->GetSolid() != SOLID_BBOX)
			continue;

		if ((Check->EntityFlags & EF_PLAYER) && (entity_cast<CPlayerEntity>(Check)->NoClip))
			continue;

		if (!Check->GetArea()->Prev)
			continue;               // not linked in anywhere

		// if the entity is standing on the pusher, it will definitely be moved
		if (Check->GroundEntity != Entity)
		{
			// see if the ent needs to be tested
			if (Check->GetAbsMin().X >= maxs.X
			|| Check->GetAbsMin().Y >= maxs.Y
			|| Check->GetAbsMin().Z >= maxs.Z
			|| Check->GetAbsMax().X <= mins.X
			|| Check->GetAbsMax().Y <= mins.Y
			|| Check->GetAbsMax().Z <= mins.Z)
				continue;

			// see if the ent's bbox is inside the pusher's final position
			if (!SV_TestEntityPosition (Check))
				continue;
		}

		if ((entity_cast<IPhysicsEntity>(Entity)->PhysicsType == PHYSICS_PUSH) || (Check->GroundEntity == Entity))
		{
			// move this entity
			CPushed PushedEntity (Check, 0, Check->State.GetOrigin(), Check->State.GetAngles());

			// try moving the contacted entity
			Check->State.GetOrigin() += move;

			if (Check->EntityFlags & EF_PLAYER)
			{
				CPlayerEntity *Player = entity_cast<CPlayerEntity>(Check);
				Player->Client.PlayerState.GetPMove()->DeltaAngles[YAW] += amove[YAW];

				//r1: dead-body-on-lift / other random view distortion fix
				PushedEntity.DeltaYaw = Player->Client.PlayerState.GetPMove()->DeltaAngles[YAW];
			}
			else
				Check->State.GetAngles().Y += amove.Y;

			Pushed.push_back (PushedEntity);

			// figure movement due to the pusher's amove
			vec3f org = Check->State.GetOrigin() - Entity->State.GetOrigin ();
			Check->State.GetOrigin() += (vec3f (
				org | angles.Forward,
				-(org | angles.Right),
				org | angles.Up
				) - org);

			// may have pushed them off an edge
			if (Check->GroundEntity != Entity)
				Check->GroundEntity = nullentity;

			if (!SV_TestEntityPosition (Check))
			{       // pushed ok
				Check->Link ();
				// impact?
				continue;
			}

			// if it is ok to leave in the old position, do it
			// this is only relevent for riding entities, not pushed
			// FIXME: this doesn't acount for rotation
			Check->State.GetOrigin() -= move;
			if (!SV_TestEntityPosition (Check))
			{
				Pushed.pop_back();
				continue;
			}
		}

		// save off the obstacle so we can call the block function
		obstacle = Check;

		// move back any entities we already moved
		// go backwards, so if the same entity was pushed
		// twice, it goes back to the original position
		for (TPushedList::reverse_iterator pit = Pushed.rbegin(); pit < Pushed.rend(); ++pit)
		{
			CPushed &PushedEntity = (*pit);

			PushedEntity.Entity->State.GetOrigin() = PushedEntity.Origin;
			PushedEntity.Entity->State.GetAngles() = PushedEntity.Angles;
			if (PushedEntity.Entity->EntityFlags & EF_PLAYER)
				(entity_cast<CPlayerEntity>(PushedEntity.Entity))->Client.PlayerState.GetPMove()->DeltaAngles[YAW] = PushedEntity.DeltaYaw;
			PushedEntity.Entity->Link ();
		}

		return false;
	}

	// see if anything we moved has touched a trigger
	for (TPushedList::reverse_iterator it = Pushed.rbegin(); it < Pushed.rend(); ++it)
	{
		CPushed &PushedEntity = *it;
		PushedEntity.Entity->TouchTriggers ();
	}

	return true;
}

#include "Entities/BrushModels.h"

bool IPushPhysics::Run ()
{
	vec3f					move, amove;
	IBaseEntity				*part;
	TPushedList				Pushed;

	// if not a team captain, so movement will be handled elsewhere
	if (IsSlave())
		return false;

	// make sure all team slaves can move before commiting
	// any moves or calling any think functions
	// if the move is blocked, all moved objects will be backed out
	for (part = this; part; part = part->Team.Chain)
	{
		if (!(part->EntityFlags & EF_PHYSICS))
			continue;

		IPhysicsEntity *Phys = entity_cast<IPhysicsEntity>(part);

		if ((Phys->Velocity.X || Phys->Velocity.Y || Phys->Velocity.Z) ||
			(Phys->AngularVelocity.X || Phys->AngularVelocity.Y || Phys->AngularVelocity.Z))
		{
			// object is moving
			if (part->EntityFlags & EF_PHYSICS)
			{
				move = Phys->Velocity * 0.1f;
				amove = Phys->AngularVelocity * 0.1f;
			}
			else
				move = amove = vec3fOrigin;

			if (!Push (Pushed, part, move, amove))
				break;
		}
	}
	if (Pushed.size() > MAX_CS_EDICTS)
		GameError ("Pushed.size() > MAX_CS_EDICTS, memory corrupted");

	if (part)
	{
		// the move failed, bump all nextthink times and back out moves
		for (IBaseEntity *mv = this; mv; mv = mv->Team.Chain)
		{
			if (mv->EntityFlags & EF_THINKABLE)
			{
				IThinkableEntity *Thinkable = entity_cast<IThinkableEntity>(mv);

				if (Thinkable->NextThink > 0)
					Thinkable->NextThink += ServerFramesPerSecond;
			}
		}

		// if the pusher has a "blocked" function, call it
		// otherwise, just stay in place until the obstacle is gone
		if ((part->EntityFlags & EF_BLOCKABLE) && obstacle)
			(entity_cast<IBlockableEntity>(part))->Blocked (obstacle);
	}
	else
	{
		// the move succeeded, so call all think functions
		for (part = this; part; part = part->Team.Chain)
		{
			if (part->EntityFlags & EF_THINKABLE)
				part->ThinkEntity->RunThink ();
		}
		
		// Paril: Train movewith
		if ((EntityFlags & EF_BRUSHMODEL) && (entity_cast<IBrushModel>(this)->BrushType & BRUSH_TRAIN))
		{
			if (Team.HasTeam)
			{
				for (IBaseEntity *e = Team.Chain; e ; e = e->Team.Chain)
					// Move the object by our velocity
				{
					if (!(e->EntityFlags & EF_BRUSHMODEL))
					{
						e->State.GetOrigin() += Velocity * 0.1f;
						e->Link ();
					}
				}
			}
		}
	}

	return true;
}


IStopPhysics::IStopPhysics () :
IPushPhysics()
{
	PhysicsType = PHYSICS_STOP;
};

IStopPhysics::IStopPhysics (sint32 Index) :
IPushPhysics(Index)
{
	PhysicsType = PHYSICS_STOP;
};

bool IStopPhysics::Run ()
{
	return IPushPhysics::Run ();
}

/**
\fn	IBlockableEntity::IBlockableEntity ()

\brief	New entity constructor. 

\author	Paril
\date	13/06/2010
**/
IBlockableEntity::IBlockableEntity () :
  IBaseEntity ()
{
	EntityFlags |= EF_BLOCKABLE;
}

/**
\fn	IBlockableEntity::IBlockableEntity (sint32 Index)

\brief	Existing entity constructor. 

\author	Paril
\date	13/06/2010

\param	Index	Zero-based index of entity to use. 
**/
IBlockableEntity::IBlockableEntity (sint32 Index) :
  IBaseEntity (Index)
{
	EntityFlags |= EF_BLOCKABLE;
}

/**
\fn	IUsableEntity::IUsableEntity ()

\brief	New entity constructor. 

\author	Paril
\date	13/06/2010
**/
IUsableEntity::IUsableEntity () :
  IBaseEntity (),
  NoiseIndex (0),
  Delay (0),
  Usable (true)
{
	EntityFlags |= EF_USABLE;
}

/**
\fn	IUsableEntity::IUsableEntity (sint32 Index)

\brief	Existing entity constructor. 

\author	Paril
\date	13/06/2010

\param	Index	Zero-based index of entity to use. 
**/
IUsableEntity::IUsableEntity (sint32 Index) :
  IBaseEntity (Index),
  NoiseIndex (0),
  Delay (0),
  Usable (true)
{
	EntityFlags |= EF_USABLE;
}

ENTITYFIELDS_BEGIN(IUsableEntity)
{
	CEntityField ("message",	EntityMemberOffset(IUsableEntity,Message),			FT_STRING | FT_SAVABLE),
	CEntityField ("noise",		EntityMemberOffset(IUsableEntity,NoiseIndex),		FT_SOUND_INDEX | FT_SAVABLE),
	CEntityField ("delay",		EntityMemberOffset(IUsableEntity,Delay),			FT_FRAMENUMBER | FT_SAVABLE),
	CEntityField ("target",		EntityMemberOffset(IUsableEntity,Target),			FT_STRING | FT_SAVABLE),
	CEntityField ("killtarget",	EntityMemberOffset(IUsableEntity,KillTarget),		FT_STRING | FT_SAVABLE),
	CEntityField ("pathtarget", EntityMemberOffset(IUsableEntity,PathTarget),		FT_STRING | FT_SAVABLE),

	CEntityField ("Usable", 	EntityMemberOffset(IUsableEntity,Usable),			FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(IUsableEntity)

bool			IUsableEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<IUsableEntity> (this, Key, Value))
		return true;

	// Couldn't find it here
	return false;
};

void			IUsableEntity::SaveFields (CFile &File)
{
	User.Write(File);
	SaveEntityFields <IUsableEntity> (this, File);
};

void			IUsableEntity::LoadFields (CFile &File)
{
	User = entity_ptr<IBaseEntity>::Read(File);
	LoadEntityFields <IUsableEntity> (this, File);
};

class CDelayedUse : public IThinkableEntity, public IUsableEntity
{
public:
	CDelayedUse () :
	  IBaseEntity (),
	  IThinkableEntity ()
	  {
	  };

	CDelayedUse (sint32 Index) :
	  IBaseEntity (Index),
	  IThinkableEntity (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER(CDelayedUse)

	void SaveFields (CFile &File)
	{
		IThinkableEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IThinkableEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
	}

	void Use (IBaseEntity *, IBaseEntity *)
	{
	};

	void Think ()
	{
		UseTargets (*User, Message);
		Free ();
	}
};

IMPLEMENT_SAVE_SOURCE (CDelayedUse)

/**
\fn	void IUsableEntity::UseTargets (IBaseEntity *Activator, String &Message)

\brief	Use targets. 

\author	Paril
\date	13/06/2010

\param [in,out]	Activator	If non-null, the activator. 
\param [in,out]	Message		The message. 
**/
void IUsableEntity::UseTargets (IBaseEntity *Activator, String &Message)
{
//
// check for a delay
//
	if (Delay)
	{
	// create a temp object to fire at a later time
		CDelayedUse *t = QNewEntityOf CDelayedUse;
		t->ClassName = "DelayedUse";

		// Paril: for compatibility
		t->NextThink = Level.Frame + Delay;
		t->User = Activator;

		// Does this EVER happen? It needs to be called with an Activator...
		t->Message = Message;
		t->Target = Target;
		t->KillTarget = KillTarget;
		return;
	}

//
// print the message
//
	if ((!Message.IsNullOrEmpty()) && (Activator->EntityFlags & EF_PLAYER))
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Activator);
		Player->PrintToClient (PRINT_CENTER, "%s", Message.CString());
		Player->PlaySound (CHAN_AUTO, (NoiseIndex) ? NoiseIndex : SoundIndex ("misc/talk1.wav"));
	}

//
// kill killtargets
//
	if (!KillTarget.IsNullOrEmpty())
	{
		IMapEntity *t = NULL;
		while ((t = CC_Find<IMapEntity, EF_MAP, EntityMemberOffset(IMapEntity,TargetName)> (t, KillTarget.CString())) != NULL)
		{
#if ROGUE_FEATURES
			// if this entity is part of a train, cleanly remove it
			if (t->IsSlave())
			{
				if (t->Team.Master)
				{
					IBaseEntity *master = t->Team.Master;
					bool done = false;
					while (!done)
					{
						if (master->Team.Chain == t)
						{
							master->Team.Chain = t->Team.Chain;
							done = true;
						}
						master = master->Team.Chain;
					}
				}
			}
#endif

			t->Free ();

			if (!GetInUse())
			{
				MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Entity was removed while using killtargets\n");
				return;
			}
		}
	}

//
// fire targets
//
	if (!Target.IsNullOrEmpty())
	{
		IMapEntity *Ent = NULL;
		while ((Ent = CC_Find<IMapEntity, EF_MAP, EntityMemberOffset(IMapEntity,TargetName)> (Ent, Target.CString())) != NULL)
		{
			if (!Ent)
				continue;

			// doors fire area portals in a specific way
			if (Ent->ClassName == "func_areaportal" &&
				(ClassName == "func_door" || ClassName == "func_door_rotating"))
				continue;

			if (Ent->EntityFlags & EF_USABLE)
			{
				IUsableEntity *Used = entity_cast<IUsableEntity>(Ent);
				bool is_self = false;
				if (Used == this)
				{
					MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Entity used itself.\n");
					is_self = true; //Avoid a infinite loop
				}

				if (Used->Usable && !is_self)
				{
					Used->Use (this, Activator);
				}
			}

			if (!GetInUse())
			{
				MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Entity was removed while using targets\n");
				return;
			}
		}
	}
}

// Buoyancy class
IBuoyancyPhysics::IBuoyancyPhysics () :
  IBaseEntity (),
  IPhysicsEntity ()
{
	PhysicsFlags |= PHYSICFLAG_BUOYANCY;
};

IBuoyancyPhysics::IBuoyancyPhysics (sint32 Index) :
  IBaseEntity (Index),
  IPhysicsEntity (Index)
{
	PhysicsFlags |= PHYSICFLAG_BUOYANCY;
};

void IBuoyancyPhysics::RunBouyancy ()
{
	EBrushContents Contents;
	float EndPower = BuoyancyPowerAir;
	bool InWater = false;

	Contents = PointContents(State.GetOrigin());

	if (Contents & CONTENTS_MASK_WATER)
	{
		EndPower = BuoyancyPowerWater;
		InWater = true;
	}

	PhysicsFlags &= ~PHYSICFLAG_FLOATING;

	if (VelocityEffect.Z < 60.0f && VelocityEffect.Z > -60.0f)
	{
		EBrushContents TopContents = PointContents (State.GetOrigin() + vec3f(0, 0, 2.4f));

		if (TopContents != Contents)
		{
			float TopPower = BuoyancyPowerAir;

			if (TopContents & CONTENTS_MASK_WATER)
				TopPower = BuoyancyPowerWater;

			EBrushContents MidContents = PointContents(State.GetOrigin() + vec3f(0, 0, 1.2f));
			float MidPower;

			if (MidContents == TopContents)
				MidPower = TopPower;
			else
				MidPower = EndPower;

			EndPower = (EndPower + TopPower + MidPower) / 3;
			DampeningEffect.Z *= powf (0.2f, ServerFramesPerSecond / 10);
			PhysicsFlags |= PHYSICFLAG_FLOATING;
		}
	}

	VelocityEffect.Z += (EndPower * CvarList[CV_GRAVITY].Float() * 0.1f);

	// Add
	Velocity *= DampeningEffect;
	Velocity += VelocityEffect;

	DampeningEffect.Set (1, 1, 1);
	VelocityEffect.Clear();
};

void IBuoyancyPhysics::SetBuoyancy (float Power, float ModAir, float ModWater)
{
	float BuoyPower = Power / Mass;

	BuoyancyPowerAir = BuoyPower * ModAir;
	BuoyancyPowerWater = BuoyPower * ModWater;
};

// Resistance class
IResistancePhysics::IResistancePhysics () :
  IBaseEntity (),
  IPhysicsEntity ()
{
	PhysicsFlags |= PHYSICFLAG_RESISTANCE;
};

IResistancePhysics::IResistancePhysics (sint32 Index) :
  IBaseEntity (Index),
  IPhysicsEntity (Index)
{
	PhysicsFlags |= PHYSICFLAG_RESISTANCE;
};

void IResistancePhysics::RunResistance ()
{
	float ResistPower = ResistPowerAir;
	EBrushContents Contents = PointContents(State.GetOrigin());

	if (Contents & CONTENTS_MASK_WATER)
		ResistPower = ResistPowerWater;

	ResistPower = powf (ResistPower, (float)ServerFramesPerSecond / 10);
	DampeningEffect *= ResistPower;
	ADampeningEffect *= ResistPower;
};

void IResistancePhysics::SetResistance (float Power, float ModAir, float ModWater)
{
	ResistPowerAir = 1.0f / (((Power * ModAir * 10.0f) / Mass) + 1.0f);
	ResistPowerWater = 1.0f / (((Power * ModWater * 10.0f) / Mass) + 1.0f);
};

// Aerodynamics class
IAerodynamicPhysics::IAerodynamicPhysics () :
  IBaseEntity (),
  IPhysicsEntity ()
{
	PhysicsFlags |= PHYSICFLAG_AERODYNAMICS;
};

IAerodynamicPhysics::IAerodynamicPhysics (sint32 Index) :
  IBaseEntity (Index),
  IPhysicsEntity (Index)
{
	PhysicsFlags |= PHYSICFLAG_AERODYNAMICS;
};

void IAerodynamicPhysics::RunAerodynamics ()
{
	if (Velocity.IsNearlyZero())
		return;

	float MoveSpeed = Velocity.LengthFast(), Power, AVelMult, ADampMult;
	vec3f MoveAngle;

	if (AeroPower > 0)
	{
		Power = AeroPower * 0.01f;
		MoveAngle = Velocity.ToAngles();
	}
	else
	{
		Power = AeroPower * -0.01f;
		MoveAngle = (vec3fOrigin - Velocity).ToAngles();
	}

	if (PhysicsFlags & PHYSICFLAG_FLOATING)
	{
		MoveAngle.X = 0;
		MoveSpeed += 200;
	}

	AVelMult = MoveSpeed * Power;
	ADampMult = 1.0 / ((MoveSpeed * Power * 0.2) + 1.0f);

	vec3f TurnAngle = MoveAngle - State.GetAngles();
	while (TurnAngle.X > 180)
		TurnAngle.X -= 360;
	while (TurnAngle.X < -180)
		TurnAngle.X += 360;
	while (TurnAngle.Y > 180)
		TurnAngle.Y -= 360;
	while (TurnAngle.Y < -180)
		TurnAngle.Y += 360;
	while (TurnAngle.Z > 180)
		TurnAngle.Z -= 360;
	while (TurnAngle.Z < -180)
		TurnAngle.Z += 360;

	AVelocityEffect += (TurnAngle * AVelMult * ((float)ServerFramesPerSecond / 10));
	ADampeningEffect *= powf (ADampMult, (float)ServerFramesPerSecond / 10);
};

void IAerodynamicPhysics::SetAerodynamics (float Power)
{
	AeroPower = Power / Mass;
};
