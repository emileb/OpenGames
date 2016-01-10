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
// cc_xatrix_entities.cpp
// 
//

#include "Local.h"

#if XATRIX_FEATURES
#include "Utility/TemporaryEntities.h"

/*QUAKED rotating_light (0 .5 .8) (-8 -8 -8) (8 8 8) START_OFF ALARM
"health"	if set, the light may be killed.
*/

// RAFAEL 
// note to self
// the lights will take damage from explosions
// this could leave a player in total darkness very bad

/**
\enum	

\brief	Values that represent spawnflags pertaining to CRotatingLight. 
**/
enum
{
	ROTATING_LIGHT_START_OFF	= BIT(0),
	ROTATING_LIGHT_ALARM		= BIT(1)
};

class CRotatingLight : public IMapEntity, public IHurtableEntity, public IThinkableEntity, public IUsableEntity
{
public:
	bool		DoFree;
	MediaIndex	AlarmSound;

	CRotatingLight () :
	  IMapEntity (),
	  IHurtableEntity (),
	  IThinkableEntity (),
	  IUsableEntity (),
	  DoFree (false),
	  AlarmSound (0)
	{
	};

	CRotatingLight (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IHurtableEntity (Index),
	  IThinkableEntity (Index),
	  IUsableEntity (Index),
	  DoFree (false),
	  AlarmSound (0)
	{
	};

	IMPLEMENT_SAVE_HEADER(CRotatingLight)

	bool			ParseField (const char *Key, const char *Value)
	{
		return (IMapEntity::ParseField (Key, Value) || IHurtableEntity::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value));
	};

	void			SaveFields (CFile &File)
	{
		File.Write<bool> (DoFree);
		WriteIndex (File, AlarmSound, INDEX_SOUND);

		IMapEntity::SaveFields (File);
		IHurtableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
	};
	void			LoadFields (CFile &File)
	{
		DoFree = File.Read<bool> ();
		ReadIndex (File, AlarmSound, INDEX_SOUND);

		IMapEntity::LoadFields (File);
		IHurtableEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
	};

	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
	{
		CSparks(State.GetOrigin(), vec3fOrigin, ST_WELDING_SPARKS, 0xe0 + irandom(7), 30).Send();

		State.GetEffects() &= ~FX_SPINNINGLIGHTS;
		Usable = false;

		DoFree = true;
		NextThink = Level.Frame + 1;
	};

	void Think ()
	{
		if (DoFree)
		{
			Free ();
			return;
		}

		if (SpawnFlags & ROTATING_LIGHT_START_OFF)
			NextThink = 0;	
		else
		{
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, AlarmSound, 255, ATTN_STATIC, 0);
			NextThink = Level.Frame + 10;
		}
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		if (SpawnFlags & ROTATING_LIGHT_START_OFF)
		{
			SpawnFlags &= ~ROTATING_LIGHT_START_OFF;
			State.GetEffects() |= FX_SPINNINGLIGHTS;

			if (SpawnFlags & ROTATING_LIGHT_ALARM)
				NextThink = Level.Frame + 1;
		}
		else
		{
			SpawnFlags |= ROTATING_LIGHT_START_OFF;
			State.GetEffects() &= ~FX_SPINNINGLIGHTS;
		}
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		State.GetModelIndex() = ModelIndex ("models/objects/light/tris.md2");	
		State.GetFrame() = 0;
			
		Usable = true;
		
		if (SpawnFlags & ROTATING_LIGHT_START_OFF)
			State.GetEffects() &= ~FX_SPINNINGLIGHTS;
		else
			State.GetEffects() |= FX_SPINNINGLIGHTS;

		if (!Health)
			Health = 10;

		MaxHealth = Health;
		CanTakeDamage = true;
	
		if (SpawnFlags & ROTATING_LIGHT_ALARM)
			AlarmSound = SoundIndex ("misc/alarm.wav");	
		
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("rotating_light", CRotatingLight);

#include "Entities/BrushModels.h"

/*QUAKED misc_crashviper (1 .5 0) (-176 -120 -24) (176 120 72) 
This is a large viper about to crash
*/
class CMiscCrashViper : public CTrainBase, public ITouchableEntity
{
	bool MyUse;
public:
	CMiscCrashViper() :
		IBaseEntity (),
		CTrainBase(),
		ITouchableEntity(),
		MyUse(true)
	{
	};

	CMiscCrashViper(sint32 Index) :
		IBaseEntity (Index),
		CTrainBase(Index),
		ITouchableEntity(Index),
		MyUse(true)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscCrashViper)

	void SaveFields (CFile &File)
	{
		File.Write<bool> (MyUse);
		CTrainBase::SaveFields (File);
		ITouchableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		MyUse = File.Read<bool> ();
		CTrainBase::LoadFields (File);
		ITouchableEntity::LoadFields (File);
	}

	bool Run ()
	{
		return CTrainBase::Run ();
	};

	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		if (MyUse)
		{
			GetSvFlags() &= ~SVF_NOCLIENT;
			MyUse = false;
		}
		CTrainBase::Use (Other, Activator);
	};

	virtual void Spawn ()
	{
		if (Target.IsNullOrEmpty())
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No targetname\n");
			Free ();
			return;
		}

		if (!Speed)
			Speed = 300;

		PhysicsType = PHYSICS_PUSH;
		Touchable = true;
		GetSolid() = SOLID_NOT;
		State.GetModelIndex() = ModelIndex ("models/ships/bigviper/tris.md2");
		GetMins().Set (-16, -16, 0);
		GetMaxs().Set (16, 16, 32);

		NextThink = Level.Frame + ServerFramesPerSecond;
		ThinkType = TRAINTHINK_FIND;
		GetSvFlags() |= SVF_NOCLIENT;
		Accel = Decel = Speed;

		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_viper", CMiscCrashViper);

/*QUAKED misc_amb4 (1 0 0) (-16 -16 -16) (16 16 16)
Mal's amb4 loop entity
*/
class CMiscAmb4 : public IMapEntity, public IThinkableEntity
{
public:
	MediaIndex	Amb4Sound;

	CMiscAmb4 () :
		IBaseEntity (),
		IMapEntity (),
		IThinkableEntity ()
	{
	};

	CMiscAmb4 (sint32 Index) :
		IBaseEntity (Index),
		IMapEntity (Index),
		IThinkableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscAmb4)

	void SaveFields (CFile &File)
	{
		WriteIndex (File, Amb4Sound, INDEX_SOUND);
		IMapEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		ReadIndex (File, Amb4Sound, INDEX_SOUND);
		IMapEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);
	}

	void Think ()
	{
		NextThink = Level.Frame + 27;
		PlaySound (CHAN_VOICE, Amb4Sound, 255, ATTN_NONE);
	}

	void Spawn ()
	{
		NextThink = Level.Frame + 10;
		Amb4Sound = SoundIndex ("world/amb4.wav");
		Link ();
	}
};

LINK_CLASSNAME_TO_CLASS ("misc_amb4", CMiscAmb4);

/*QUAKED misc_nuke (1 0 0) (-16 -16 -16) (16 16 16)
*/
class CMiscNuke : public IMapEntity, public IUsableEntity
{
public:
	CMiscNuke () :
		IBaseEntity (),
		IMapEntity (),
		IUsableEntity ()
	{
	};

	CMiscNuke (sint32 Index) :
		IBaseEntity (Index),
		IMapEntity (Index),
		IUsableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscNuke)

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
	}

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		for (TEntitiesContainer::iterator it = Level.Entities.Closed.begin(); it != Level.Entities.Closed.end(); ++it)
		{
			IBaseEntity *Entity = (*it)->Entity;
			if (!Entity || !Entity->GetInUse())
				continue;

			if (!(Entity->EntityFlags & EF_HURTABLE))
				continue;

			if (Entity == this)
				continue;

			IHurtableEntity *Hurtable = entity_cast<IHurtableEntity>(Entity);

			if (Hurtable->EntityFlags & EF_PLAYER)
				Hurtable->TakeDamage (this, this, vec3fOrigin, Hurtable->State.GetOrigin(), vec3fOrigin, 100000, 1, 0, MOD_TRAP);
			else if (Hurtable->EntityFlags & EF_MONSTER)
				Hurtable->Free ();
		}

		Usable = false;
	}

	void Spawn ()
	{
		Usable = true;
	}
};

LINK_CLASSNAME_TO_CLASS ("misc_nuke", CMiscNuke);

// RAFAEL
/*QUAKED misc_viper_missile (1 0 0) (-8 -8 -8) (8 8 8)
"dmg"	how much boom should the bomb make? the default value is 250
*/

/*QUAKED misc_viper_bomb (1 0 0) (-8 -8 -8) (8 8 8)
"dmg"	how much boom should the bomb make?
*/
class CMiscViperMissile : public IMapEntity,public IUsableEntity
{
public:
	sint32			Damage;

	CMiscViperMissile () :
	  IBaseEntity (),
	  IMapEntity (),
	  IUsableEntity ()
	{
	};

	CMiscViperMissile (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CMiscViperMissile)

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		vec3f	start, dir;
		vec3f	vec;
				
		IBaseEntity *target = CC_FindByClassName<IBaseEntity, EF_BASE> (NULL, Target.CString());
		
		vec = target->State.GetOrigin();
		vec.Z += 16;
		
		start = State.GetOrigin();
		dir = (vec - start).GetNormalized();
		
		CRocket::Spawn (World, start, dir, Damage, 500, Damage, Damage+40);
		CMuzzleFlash(State.GetOrigin(), State.GetNumber(), MZ2_CHICK_ROCKET_1, true).Send();
		
		Free ();
	};

	void Spawn ()
	{
		GetSolid() = SOLID_NOT;
		GetMins().Set (-8);
		GetMaxs().Set (8);

		State.GetModelIndex() = ModelIndex ("models/objects/bomb/tris.md2");

		if (!Damage)
			Damage = 1000;

		GetSvFlags() |= SVF_NOCLIENT;
		Link ();
	};
};

ENTITYFIELDS_BEGIN(CMiscViperMissile)
{
	CEntityField ("dmg", EntityMemberOffset(CMiscViperMissile,Damage), FT_INT | FT_SAVABLE)
};
ENTITYFIELDS_END(CMiscViperMissile)

bool			CMiscViperMissile::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CMiscViperMissile> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void		CMiscViperMissile::SaveFields (CFile &File)
{
	SaveEntityFields <CMiscViperMissile> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
}

void		CMiscViperMissile::LoadFields (CFile &File)
{
	LoadEntityFields <CMiscViperMissile> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("misc_viper_missile", CMiscViperMissile);

#include "Entities/MiscEntities.h"

// RAFAEL 17-APR-98
/*QUAKED misc_transport (1 0 0) (-8 -8 -8) (8 8 8) TRIGGER_SPAWN
Maxx's transport at end of game
*/

class CMiscTransport : public CMiscViper
{
public:
	CMiscTransport () :
	  IBaseEntity (),
	  CMiscViper ()
	  {
	  };

	CMiscTransport (sint32 Index) :
	  IBaseEntity (Index),
	  CMiscViper (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER (CMiscTransport);

	bool Run ()
	{
		return CTrainBase::Run ();
	};

	void Spawn ()
	{
		if (!(SpawnFlags & 1))
			SpawnFlags |= 1;

		CMiscViper::Spawn ();
		State.GetModelIndex() = ModelIndex ("models/objects/ship/tris.md2");
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_transport", CMiscTransport);

// RAFAEL 15-APR-98
/*QUAKED target_mal_laser (1 0 0) (-4 -4 -4) (4 4 4) START_ON RED GREEN BLUE YELLOW ORANGE FAT
Mal's laser
*/
#include "Entities/TargetEntities.h"

class CTargetMalLaser : public CTargetLaser
{
public:
	FrameNumber		Wait;
	FrameNumber		Delay;

	CTargetMalLaser () :
	  CTargetLaser ()
	  {
	  };

	CTargetMalLaser (sint32 Index) :
	  IBaseEntity (Index),
	  CTargetLaser (Index)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE (CTargetMalLaser)

	void On ()
	{
		if (!User)
			User = this;
		SpawnFlags |= 0x80000001;
		GetSvFlags() &= ~SVF_NOCLIENT;

		NextThink = Level.Frame + Wait + Delay;
	};

	void Think ()
	{
		CTargetLaser::Think ();
		NextThink = Level.Frame + Wait + 1;
		SpawnFlags |= 0x80000000;
	};
};

ENTITYFIELDS_BEGIN(CTargetMalLaser)
{
	CEntityField ("wait", EntityMemberOffset(CTargetMalLaser,Wait), FT_FRAMENUMBER | FT_SAVABLE),
	CEntityField ("delay", EntityMemberOffset(CTargetMalLaser,Delay), FT_FRAMENUMBER | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetMalLaser)

bool			CTargetMalLaser::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetMalLaser> (this, Key, Value))
		return true;

	// Couldn't find it here
	return CTargetLaser::ParseField (Key, Value);
};

void			CTargetMalLaser::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetLaser> (this, File);
	CTargetLaser::SaveFields (File);
}

void			CTargetMalLaser::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetLaser> (this, File);
	CTargetLaser::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_mal_laser", CTargetMalLaser);

/*QUAKED func_object_repair (1 .5 0) (-8 -8 -8) (8 8 8) 
object to be repaired.
The default delay is 1 second
"delay" the delay in seconds for spark to occur
*/

class CFuncObjectRepair : public IMapEntity, public IThinkableEntity, public IHurtableEntity, public IUsableEntity
{
public:
	/**
	\typedef	uint8 ERepairThinkType
	
	\brief	Defines an alias representing think type of a CFuncObjectRepair.
	**/
	typedef uint8 ERepairThinkType;

	/**
	\enum	
	
	\brief	Values that represent think types for CFuncObjectRepair. 
	**/
	enum
	{
		THINK_NONE,
		THINK_SPARKS,
		THINK_DEAD,
		THINK_FX
	};

	FrameNumber		Delay;
	ERepairThinkType	ThinkType;

	CFuncObjectRepair () :
	  IMapEntity (),
	  IThinkableEntity (),
	  IHurtableEntity (),
	  IUsableEntity ()
	  {
	  };

	CFuncObjectRepair (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index),
	  IHurtableEntity (Index),
	  IUsableEntity (Index)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE (CFuncObjectRepair)

	bool Run ()
	{
		return IBaseEntity::Run ();
	};

	void RepairFX ()
	{
		NextThink = Level.Frame + Delay;

		if (Health <= 100)
			Health++;
		else
			CSparks(State.GetOrigin(), vec3fOrigin, ST_WELDING_SPARKS, 0xe0 + irandom(7), 10).Send();
	};

	void Dead ()
	{
		UseTargets (this, Message);
		NextThink = Level.Frame + 1;
		ThinkType = THINK_FX;
	};

	void Sparks ()
	{
		if (Health < 0)
		{
			NextThink = Level.Frame + 1;
			ThinkType = THINK_DEAD;
			return;
		}

		NextThink = Level.Frame + Delay;
	
		CSparks(State.GetOrigin(), vec3fOrigin, ST_WELDING_SPARKS, 0xe0 + irandom(7), 10).Send();
	};
	
	void Think ()
	{
		switch (ThinkType)
		{
		case THINK_NONE:
			return;
		case THINK_SPARKS:
			Sparks ();
			break;
		case THINK_DEAD:
			Dead ();
			break;
		case THINK_FX:
			RepairFX ();
			break;
		};
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		GetMins().Set (-8, -8, 8);
		GetMaxs().Set (8, 8, 8);
		NextThink = Level.Frame + ServerFramesPerSecond;
		ThinkType = THINK_SPARKS;
		Health = 100;

		if (!Delay)
			Delay = 10;
	};
};

ENTITYFIELDS_BEGIN(CFuncObjectRepair)
{
	CEntityField ("delay", EntityMemberOffset(CFuncObjectRepair,Delay), FT_FRAMENUMBER | FT_SAVABLE)
};
ENTITYFIELDS_END(CFuncObjectRepair)

bool			CFuncObjectRepair::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CFuncObjectRepair> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField(Key, Value) || IHurtableEntity::ParseField (Key, Value));
};

void			CFuncObjectRepair::SaveFields (CFile &File)
{
	SaveEntityFields <CFuncObjectRepair> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
	IHurtableEntity::SaveFields (File);
	IThinkableEntity::SaveFields (File);
}

void			CFuncObjectRepair::LoadFields (CFile &File)
{
	LoadEntityFields <CFuncObjectRepair> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
	IHurtableEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("func_object_repair", CFuncObjectRepair);
#endif
