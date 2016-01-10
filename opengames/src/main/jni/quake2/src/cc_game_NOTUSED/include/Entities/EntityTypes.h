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
// cc_entitytypes.h
// Different base entity classes
//

#if !defined(CC_GUARD_ENTITYTYPES_H) || !INCLUDE_GUARDS
#define CC_GUARD_ENTITYTYPES_H

#ifndef EDamageFlags
typedef uint16 EDamageFlags;
#endif

/**
\typedef	uint32 EMeansOfDeath

\brief	Defines an alias representing a means of death.
**/
typedef uint32 EMeansOfDeath;

/**
\enum	

\brief	Values that represent a means of death. 
**/
enum
{
	MOD_UNKNOWN,
	MOD_BLASTER,
	MOD_SHOTGUN,
	MOD_SSHOTGUN,
	MOD_MACHINEGUN,
	MOD_CHAINGUN,
	MOD_GRENADE,
	MOD_G_SPLASH,
	MOD_ROCKET,
	MOD_R_SPLASH,
	MOD_HYPERBLASTER,
	MOD_RAILGUN,
	MOD_BFG_LASER,
	MOD_BFG_BLAST,
	MOD_BFG_EFFECT,
	MOD_HANDGRENADE,
	MOD_HG_SPLASH,
	MOD_WATER,
	MOD_SLIME,
	MOD_LAVA,
	MOD_CRUSH,
	MOD_TELEFRAG,
	MOD_FALLING,
	MOD_SUICIDE,
	MOD_HELD_GRENADE,
	MOD_EXPLOSIVE,
	MOD_BARREL,
	MOD_BOMB,
	MOD_EXIT,
	MOD_SPLASH,
	MOD_TARGET_LASER,
	MOD_TRIGGER_HURT,
	MOD_HIT,
	MOD_TARGET_BLASTER,

#if CLEANCTF_ENABLED
	MOD_GRAPPLE,
#endif

#if XATRIX_FEATURES
	MOD_RIPPER,
	MOD_TRAP,
#endif

#if ROGUE_FEATURES
	MOD_CHAINFIST,
	MOD_DISINTEGRATOR,
	MOD_ETF_RIFLE,
	MOD_BLASTER2,
	MOD_HEATBEAM,
	MOD_TESLA,
	MOD_PROX,
	MOD_NUKE,
	MOD_VENGEANCE_SPHERE,
	MOD_HUNTER_SPHERE,
	MOD_DEFENDER_SPHERE,
	MOD_TRACKER,
	MOD_DOPPLE_EXPLODE,
	MOD_DOPPLE_VENGEANCE,
	MOD_DOPPLE_HUNTER,
#endif

	MOD_FRIENDLY_FIRE		=	512
};

/**
\class	IHurtableEntity

\brief	Hurtable entity subclass. Inherit this to allow an entity to take damage. 

\author	Paril
\date	13/06/2010
**/
class IHurtableEntity : public virtual IBaseEntity
{
public:
	sint32			Health;					// Health level
	sint32			MaxHealth;				// Highest health level. Usually used for storing max health.
	sint32			GibHealth;				// Health that he will gib at.
	bool			IsDead;					// True if dead
	bool			CanTakeDamage;			// true if can take damage
	bool			AffectedByKnockback;	// true if affected by knockback

	ENTITYFIELD_VIRTUAL_DEFS
	ENTITYFIELDS_SAVABLE_VIRTUAL(IHurtableEntity)

	/**
	\fn	IHurtableEntity ()
	
	\brief	New entity constructor.
	
	\author	Paril
	\date	13/06/2010
	**/
	IHurtableEntity ();

	/**
	\fn	IHurtableEntity (sint32 Index)
	
	\brief	Existing entity constructor.
	
	\author	Paril
	\date	13/06/2010
	
	\param	Index	Zero-based index of entity to use.
	**/
	IHurtableEntity (sint32 Index);

	/**
	\fn	virtual void Pain (IBaseEntity *Other, sint32 Damage)
	
	\brief	Virtual pain function. Called by TakeDamage when any damage is taken.
	
	\author	Paril
	\date	13/06/2010
	
	\param [in,out]	Other	If non-null, the entity causing the damage. 
	\param	Damage			The damage. 
	**/
	virtual void Pain (IBaseEntity *Other, sint32 Damage) {};

	/**
	\fn	virtual void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
	
	\brief	Virtual death function. Called when this entity's health is below or equal to 0 when he
			is hit. 
	
	\author	Paril
	\date	13/06/2010
	
	\param [in,out]	Inflictor	If non-null, the inflictor. 
	\param [in,out]	Attacker	If non-null, the attacker. 
	\param	Damage				The damage. 
	\param [in,out]	Point		The point. 
	**/
	virtual void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point) {};

	/**
	\fn	bool DamageCanReach (IBaseEntity *Inflictor)
	
	\brief	Tests if Inflictor can damage this entity in a bounding box path. Used for radius
			damages. 
	
	\author	Paril
	\date	13/06/2010
	
	\param [in,out]	Inflictor	If non-null, the inflictor. 
	
	\return	true if it succeeds, false if it fails. 
	**/
	bool DamageCanReach (IBaseEntity *Inflictor);

	/**
	\fn	bool CheckTeamDamage (IBaseEntity *Attacker)
	
	\brief	Tests if this entity and Attacker are on a team. 
	
	\author	Paril
	\date	13/06/2010
	
	\param [in,out]	Attacker	If non-null, the attacker. 
	
	\return	true if they are on a team, false if not. 
	**/
	bool CheckTeamDamage (IBaseEntity *Attacker);

	/**
	\fn	sint32 CheckPowerArmor (vec3f &Point, vec3f &Normal, sint32 Damage, EDamageFlags DamageFlags)
	
	\brief	Checks power armor and makes adjustments based on that. 
	
	\author	Paril
	\date	13/06/2010
	
	\param [in,out]	Point	The point. 
	\param [in,out]	Normal	The normal. 
	\param	Damage			The damage. 
	\param	DamageFlags		The damage flags. 
	
	\return	The amount of damage saved from the power armor. 
	**/
	virtual sint32 CheckPowerArmor (vec3f &Point, vec3f &Normal, sint32 Damage, EDamageFlags DamageFlags);

	/**
	\fn	virtual void Killed (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage,
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
	virtual void Killed (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);

	/**
	\fn	virtual void DamageEffect (vec3f &Dir, vec3f &Point, vec3f &Normal, sint32 &Damage,
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
	virtual void DamageEffect (vec3f &Dir, vec3f &Point, vec3f &Normal, sint32 &Damage, EDamageFlags &DamageFlags, EMeansOfDeath &MeansOfDeath);

	/**
	\fn	virtual void TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker, vec3f Dir,
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
	virtual void TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker,
							vec3f Dir, vec3f Point, vec3f Normal, sint32 Damage,
							sint32 KnockBack, EDamageFlags DamageFlags, EMeansOfDeath MeansOfDeath);
};

/**
\class	IBlockableEntity

\brief	Blockable entity subclass. Allows this entity to call a Blocked function when stuck
		between a pusher. 

\author	Paril
\date	13/06/2010
**/
class IBlockableEntity : public virtual IBaseEntity
{
public:
	/**
	\fn	IBlockableEntity ()
	
	\brief	New entity constructor.
	
	\author	Paril
	\date	13/06/2010
	**/
	IBlockableEntity ();

	/**
	\fn	IBlockableEntity (sint32 Index)
	
	\brief	Existing entity constructor.
	
	\author	Paril
	\date	13/06/2010
	
	\param	Index	Zero-based index of entity to use. 
	**/
	IBlockableEntity (sint32 Index);

	/**
	\fn	virtual void Blocked (IBaseEntity *Other) = 0
	
	\brief	Blocked callback.
	
	\author	Paril
	\date	13/06/2010
	
	\param [in,out]	Other	If non-null, the pusher. 
	**/
	virtual void Blocked (IBaseEntity *Other) = 0;
};

/**
\class	IUsableEntity

\brief	Usable entity subclass. Inherit this to allow an entity to be used as a target by other
		entities. This entity is strictly used for map entities. 

\author	Paril
\date	13/06/2010
**/
class IUsableEntity : public virtual IBaseEntity
{
public:
	String						Message,	// Message printed on usage
								Target,		// Target to use
								KillTarget,	// Target to use when killed
								PathTarget;	// Target to use when reached a path
	entity_ptr<IBaseEntity>		User;		// The entity who used this
	FrameNumber					Delay;		// The delay after use to actually use
	MediaIndex					NoiseIndex;	// Zero-based index of sound to make
	bool						Usable;		// true if entity is usable

	ENTITYFIELD_VIRTUAL_DEFS
	ENTITYFIELDS_SAVABLE_VIRTUAL(IUsableEntity)

	/**
	\fn	IUsableEntity ()
	
	\brief	New entity constructor.
	
	\author	Paril
	\date	13/06/2010
	**/
	IUsableEntity ();

	/**
	\fn	IUsableEntity (sint32 Index)
	
	\brief	Existing entity constructor.
	
	\author	Paril
	\date	13/06/2010
	
	\param	Index	Zero-based index of entity to use. 
	**/
	IUsableEntity (sint32 Index);

	/**
	\fn	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator)
	
	\brief	Callback function when this entity is used by another.
	
	\author	Paril
	\date	13/06/2010
	
	\param [in,out]	Other		If non-null, the entity that called UseTargets to this entity. 
	\param [in,out]	Activator	If non-null, the main activator; lowest level.
	**/
	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator) {};

	/**
	\fn	virtual void UseTargets (IBaseEntity *Activator, String &Message)
	
	\brief	Use targets. 
	
	\author	Paril
	\date	13/06/2010
	
	\param [in,out]	Activator	If non-null, the activator. 
	\param [in,out]	Message		The message. 
	**/
	virtual void UseTargets (IBaseEntity *Activator, String &Message);
};

/**
\class	IThinkableEntity

\brief	Thinkable entity subclass. This allows an entity to call a function on a specific frame. It also
		allows an entity to "PreThink", which is called each frame before running. 

\author	Paril
\date	13/06/2010
**/
class IThinkableEntity : public virtual IBaseEntity
{
public:
	FrameNumber			NextThink;	// The frame to call Think on.

	/**
	\fn	IThinkableEntity ()
	
	\brief	New entity constructor.
	
	\author	Paril
	\date	13/06/2010
	**/
	IThinkableEntity ();

	/**
	\fn	IThinkableEntity (sint32 Index)
	
	\brief	Existing entity constructor. 
	
	\author	Paril
	\date	13/06/2010
	
	\param	Index	Zero-based index of the entity to use. 
	**/
	IThinkableEntity (sint32 Index);

	ENTITYFIELDS_SAVABLE(IThinkableEntity)

	/**
	\fn	void RunThink ()
	
	\brief	Function that runs the think process. 
	
	\author	Paril
	\date	13/06/2010
	**/
	void			RunThink ();

	/**
	\fn	virtual void Think () = 0
	
	\brief	The think function. 
	**/
	virtual void	Think () = 0;

	/**
	\fn	virtual void PreThink ()
	
	\brief	The pre-think function.
	**/
	virtual void	PreThink () {};
};

/**
\class	ITouchableEntity

\brief	Touchable entity subclass. This allows an entity to generate touch callbacks when another
		entity collides with it. 

\author	Paril
\date	13/06/2010
**/
class ITouchableEntity : public virtual IBaseEntity
{
public:
	bool				Touchable;	// true if Touch will be called

	ENTITYFIELDS_SAVABLE(ITouchableEntity)

	ITouchableEntity ();
	ITouchableEntity (sint32 index);

	/**
	\fn	virtual void Touch (IBaseEntity *Other, SBSPPlane *Plane, SBSPSurface *Surface)
	
	\brief	Touch callback. 
	
	\author	Paril
	\date	13/06/2010
	
	\param [in,out]	Other	If non-null, the other entity that touched this one. 
	\param [in,out]	Plane	If non-null, the plane. 
	\param [in,out]	Surface	If non-null, the surface. 
	**/
	virtual void	Touch (IBaseEntity *Other, SBSPPlane *Plane, SBSPSurface *Surface) {};
};

/**
\typedef	uint8 EPhysicsType

\brief	Defines an alias representing type of physics this entity is currently using.
**/
typedef uint8 EPhysicsType;

/**
\enum	

\brief	Values that represent physics types. 
**/
enum
{
	PHYSICS_NONE,

	PHYSICS_NOCLIP,
	PHYSICS_PUSH,
	PHYSICS_STOP,
	PHYSICS_WALK,
	PHYSICS_STEP,
	PHYSICS_FLY,
	PHYSICS_TOSS,
	PHYSICS_FLYMISSILE,
	PHYSICS_BOUNCE
};

/**
\typedef	uint32 EPhysicsFlags

\brief	Defines an alias representing physics flags.
**/
typedef uint32 EPhysicsFlags;

/**
\enum	

\brief	Values that represent physics flags; extra features that physics objects may have.
		Note that these cannot be set directly. You must inherit the proper classes.
**/
enum
{
	// Class flags
	PHYSICFLAG_BUOYANCY			=	BIT(0), // Has IBuoyancyPhysics
	PHYSICFLAG_RESISTANCE		=	BIT(1), // Has IResistancePhysics
	PHYSICFLAG_AERODYNAMICS		=	BIT(2), // Has IAerodynamicPhysics

	// Other flags
	PHYSICFLAG_FLOATING			=	BIT(16), // Is "floating"
};

/**
\class	IPhysicsEntity

\brief	Physics entity subclass. Contains commonalities for entities to be physics entities. 

\author	Paril
\date	13/06/2010
**/
class IPhysicsEntity : public virtual IBaseEntity
{
public:
	float				GravityMultiplier;	// Per entity gravity multiplier (1.0 is normal)
	EPhysicsType		PhysicsType;		// Physics type
	EPhysicsFlags		PhysicsFlags;		// The physics flags
	vec3f				AngularVelocity,	// The angular velocity
						Velocity,			// The velocity
						GravityVector;		// The gravity vector
	float				Mass;				// The mass of this entity

	/**
	\struct	SWaterInfo
	
	\brief	Information about water on this entity. 
	
	\author	Paril
	\date	13/06/2010
	**/
	struct SWaterInfo
	{
		EBrushContents		Type;		// The water type
		EWaterLevel			Level,		// The water level
							OldLevel;	// Old water level
	}					WaterInfo;

	bool				PhysicsDisabled;	// true if physics are disabled
	vec3f				VelocityEffect,		// Velocity added to regular velocity
						DampeningEffect,	// Dampening effect for velocity
						AVelocityEffect,	// Angular velocity effect
						ADampeningEffect;	// Angular dampening

	/**
	\fn	IPhysicsEntity ()
	
	\brief	New entity constructor.
	
	\author	Paril
	\date	13/06/2010
	**/
	IPhysicsEntity ();

	/**
	\fn	IPhysicsEntity (sint32 Index)
	
	\brief	Existing entity constructor.
	
	\author	Paril
	\date	13/06/2010
	
	\param	Index	Zero-based index of the entity to use. 
	**/
	IPhysicsEntity (sint32 Index);

	virtual void SaveFields (CFile &File)
	{
		File.Write<float> (GravityMultiplier);
		File.Write<EPhysicsType> (PhysicsType);
		File.Write<vec3f> (&AngularVelocity);
		File.Write<vec3f> (&Velocity);
		File.Write<vec3f> (&GravityVector);
		File.Write<float> (&Mass);
		File.Write<SWaterInfo> (WaterInfo);
		File.Write<bool> (PhysicsDisabled);
	};

	virtual void LoadFields (CFile &File)
	{
		GravityMultiplier = File.Read<float> ();
		PhysicsType = File.Read<EPhysicsType> ();
		AngularVelocity = File.Read<vec3f> ();
		Velocity = File.Read<vec3f> ();
		GravityVector = File.Read<vec3f> ();
		Mass = File.Read<float> ();
		WaterInfo = File.Read<SWaterInfo> ();
		PhysicsDisabled = File.Read<bool> ();
	};

	/**
	\fn	CTrace PushEntity (vec3f &Push)
	
	\brief	Causes this entity to move and push/impact any obstacles.
	
	\author	Paril
	\date	13/06/2010
	
	\param [in,out]	Push	The push vector. 
	
	\return	The last trace to be processed by the pushing function. 
	**/
	CTrace			PushEntity (vec3f &Push);

	/**
	\fn	void AddGravity ()
	
	\brief	Adds gravity to this entity's velocity.
	**/
	void			AddGravity ();

	/**
	\fn	void Impact (CTrace *Trace)
	
	\brief	Causes this entity to impact with the entity hit by Trace. Calls the Touch functions, if
			applicable. 
	
	\author	Paril
	\date	13/06/2010
	
	\param [in,out]	Trace	If non-null, the trace. 
	**/
	void			Impact (CTrace *Trace);

	/**
	\fn	virtual void PushInDirection (vec3f Vel, ESpawnflags Flags)
	
	\brief	Pushes this entity in speed vector 'Vel'. The spawnflags of the pusher entity can be
			passed via Flags to modify velocity or otherwise. 
	
	\author	Paril
	\date	13/06/2010
	
	\param	Vel		The vel. 
	\param	Flags	The flags. 
	**/
	virtual void	PushInDirection (vec3f Vel, ESpawnflags Flags);
};

/**
\class	IBounceProjectile

\brief	Bounce projectile subclass. This provides the base for bounce physics. 

\author	Paril
\date	14/06/2010
**/
class IBounceProjectile : public virtual IPhysicsEntity
{
public:
	float			BackOff;				// The overbounce value; this is the velocity reflection amount. 2 is maintain speed.
	bool			AffectedByGravity;		// true if affected by gravity
	bool			StopOnEqualPlane;		// true if the projectile doesn't stop if on ground
	bool			AimInVelocityDirection;	// true to aim in velocity direction

	/**
	\fn	IBounceProjectile ()
	
	\brief	New entity constructor. 
	
	\author	Paril
	\date	14/06/2010
	**/
	IBounceProjectile ();

	/**
	\fn	IBounceProjectile (sint32 Index)
	
	\brief	Existing entity constructor. 
	
	\author	Paril
	\date	14/06/2010
	
	\param	Index	Zero-based index of the entity to use. 
	**/
	IBounceProjectile (sint32 Index);

	virtual void SaveFields (CFile &File)
	{
		File.Write<float> (BackOff);
		IPhysicsEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		BackOff = File.Read<float> ();
		IPhysicsEntity::LoadFields (File);
	}

	/**
	\fn	bool Run ()
	
	\brief	Runs this object's physics.
	
	\return	true if it succeeds, false if it fails. 
	**/
	bool			Run ();
};

/**
\class	ITossProjectile

\brief	Toss projectile subclass. Same as bouncy, except an overbounce of 1.0 (stops).

\todo	Remove this class. It is not required.

\author	Paril
\date	14/06/2010
**/
class ITossProjectile : public virtual IBounceProjectile
{
public:
	ITossProjectile();
	ITossProjectile (sint32 Index);

	virtual void SaveFields (CFile &File)
	{
		IBounceProjectile::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IBounceProjectile::LoadFields (File);
	}
};

/**
\class	IFlyMissileProjectile

\brief	Fly missile projectile subclass. Same as bouncy, except an overbounce of 1.0 (stop) and
		doesn't add gravity. 
\todo	Remove this class. It is not required. 

\author	Paril
\date	14/06/2010
**/
class IFlyMissileProjectile : public virtual IBounceProjectile
{
public:
	IFlyMissileProjectile ();
	IFlyMissileProjectile (sint32 index);

	virtual void SaveFields (CFile &File)
	{
		IPhysicsEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IPhysicsEntity::LoadFields (File);
	}
};

/**
\class	IStepPhysics

\brief	Step physics subclass. Used for stepping entities such as monsters. 

\author	Paril
\date	14/06/2010
**/
class IStepPhysics : public virtual IPhysicsEntity
{
public:
	/**
	\fn	IStepPhysics ()
	
	\brief	New entity constructor. 
	
	\author	Paril
	\date	14/06/2010
	**/
	IStepPhysics ();

	/**
	\fn	IStepPhysics (sint32 Index)
	
	\brief	Existing entity constructor. 
	
	\author	Paril
	\date	14/06/2010
	
	\param	Index	Zero-based index of the entity to use. 
	**/
	IStepPhysics (sint32 Index);

	virtual void SaveFields (CFile &File)
	{
		IPhysicsEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IPhysicsEntity::LoadFields (File);
	}

	/**
	\fn	virtual void CheckGround ()
	
	\brief	Check ground virtual function. Provides the code that checks if an entity is on-ground. 
	**/
	virtual void	CheckGround ();

	/**
	\fn	sint32 FlyMove (float Time, EBrushContents Mask)
	
	\brief	Performs the move. 
	
	\author	Paril
	\date	14/06/2010
	
	\param	Time	The time to complete the move. 
	\param	Mask	The content mask. 
	
	\return	The blocked flags. 
	**/
	sint32			FlyMove (float Time, EBrushContents Mask);

	/**
	\fn	void AddRotationalFriction ()
	
	\brief	Adds rotational friction
	**/
	void			AddRotationalFriction ();

	/**
	\fn	bool Run ()
	
	\brief	Runs this object's physics.
	
	\return	true if it succeeds, false if it fails. 
	**/
	bool			Run ();
};

class IPushPhysics : public virtual IPhysicsEntity
{
public:
	IPushPhysics ();
	IPushPhysics (sint32 Index);

	virtual void SaveFields (CFile &File)
	{
		IPhysicsEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IPhysicsEntity::LoadFields (File);
	}

	bool			Run ();
};

class IStopPhysics : public virtual IPushPhysics
{
public:
	IStopPhysics ();
	IStopPhysics (sint32 Index);

	virtual void SaveFields (CFile &File)
	{
		IPhysicsEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IPhysicsEntity::LoadFields (File);
	}

	bool			Run ();
};

// Buoyancy physics.
// Only works for: Bounce, Toss (via Bounce)
class IBuoyancyPhysics : public virtual IPhysicsEntity
{
public:
	float		BuoyancyPowerWater, BuoyancyPowerAir;

	IBuoyancyPhysics ();
	IBuoyancyPhysics (sint32 Index);

	void RunBouyancy ();
	void SetBuoyancy (float Power, float ModAir = 0.0f, float ModWater = 1.0f);
};

// Resistance physics.
// Only works for: Bounce, Toss (via Bounce)
class IResistancePhysics : public virtual IPhysicsEntity
{
public:
	float		ResistPowerWater, ResistPowerAir;

	IResistancePhysics ();
	IResistancePhysics (sint32 Index);

	void RunResistance ();
	void SetResistance (float Power, float ModAir = 0.0f, float ModWater = 1.0f);
};

// Aerodynamics
class IAerodynamicPhysics : public virtual IPhysicsEntity
{
public:
	float		AeroPower;

	IAerodynamicPhysics ();
	IAerodynamicPhysics (sint32 Index);

	void RunAerodynamics ();
	void SetAerodynamics (float Power);
};

#else
FILE_WARNING
#endif
