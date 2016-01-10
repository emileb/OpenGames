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
// cc_trigger_entities.cpp
// 
//

#include "Local.h"
#include "Items/Items.h"
#include "Entities/BrushModels.h"
#include "Entities/TriggerEntities.h"

/*QUAKED trigger_always (.5 .5 .5) (-8 -8 -8) (8 8 8)
This trigger will always fire.  It is activated by the world.
*/
class CTriggerAlways : public IMapEntity, public IUsableEntity
{
public:
	CTriggerAlways () :
	  IBaseEntity (),
	  IMapEntity ()
	{
	};

	CTriggerAlways (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CTriggerAlways)

	bool ParseField (const char *Key, const char *Value)
	{
		return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

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

	void Use (IBaseEntity *, IBaseEntity *)
	{
	};

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Spawn ()
	{
		// we must have some delay to make sure our use targets are present
		if (Delay < 2)
			Delay = 2;

		UseTargets (this, Message);
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_always", CTriggerAlways);

/**
\enum	

\brief	Values that represent spawnflags pertaining to CTrigger*. 
**/
enum
{
	TRIGGER_MONSTER		= BIT(0),
	TRIGGER_NOT_PLAYER	= BIT(1),
	TRIGGER_TRIGGERED	= BIT(2),
#if ROGUE_FEATURES
	TRIGGER_TOGGLE		= BIT(3),
#endif
};

CTriggerBase::CTriggerBase () :
  IBaseEntity (),
  IMapEntity (),
  ITouchableEntity (),
  IUsableEntity (),
  IBrushModel (),
  ThinkType (TRIGGER_THINK_NONE)
{
};

CTriggerBase::CTriggerBase (sint32 Index) :
  IBaseEntity (Index),
  IMapEntity (Index),
  ITouchableEntity (Index),
  IUsableEntity (Index),
  IBrushModel (Index),
  ThinkType (TRIGGER_THINK_NONE)
{
};

bool CTriggerBase::Run ()
{
	return IBaseEntity::Run();
};

void CTriggerBase::Think ()
{
	switch (ThinkType)
	{
	case TRIGGER_THINK_WAIT:
		NextThink = 0;
		break;
	case TRIGGER_THINK_FREE:
		Free ();
		break;
	default:
		break;
	};
};

void CTriggerBase::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	if (!Touchable)
		return;

	if(Other->EntityFlags & EF_PLAYER)
	{
		if (SpawnFlags & TRIGGER_NOT_PLAYER)
			return;
	}
	else if (Other->EntityFlags & EF_MONSTER)
	{
		if (!(SpawnFlags & TRIGGER_MONSTER))
			return;
	}
	else
		return;

	if (MoveDir != vec3fOrigin)
	{
		if ((Other->State.GetAngles().ToVectors().Forward | MoveDir) < 0)
			return;
	}

	User = Other;
	Trigger ();
};

void CTriggerBase::Use (IBaseEntity *Other, IBaseEntity *Activator)
{
#if ROGUE_FEATURES
	if (SpawnFlags & TRIGGER_TOGGLE)
	{
		GetSolid() = (GetSolid() == SOLID_TRIGGER) ? SOLID_NOT : SOLID_TRIGGER;	

		Link ();
		return;
	}

#endif
	User = Activator;
	Trigger ();
};

void CTriggerBase::Init ()
{
	if (State.GetAngles() != vec3fOrigin)
		SetMoveDir ();

	GetSolid() = SOLID_TRIGGER;
	SetBrushModel ();

	GetSvFlags() = SVF_NOCLIENT;
};

// the trigger was just activated
// ent->Activator should be set to the Activator so it can be held through a delay
// so wait for the delay time before firing
void CTriggerBase::Trigger ()
{
	if (NextThink)
		return;		// already been triggered

	UseTargets (*User, Message);

	if (Wait > 0)	
	{
		ThinkType = TRIGGER_THINK_WAIT;

		NextThink = Level.Frame + Wait;
	}
	else
	{	// we can't just remove (self) here, because this is a touch function
		// called while looping through area links...
		Touchable = false;
		NextThink = Level.Frame + ServerFramesPerSecond;
		ThinkType = TRIGGER_THINK_FREE;
	}
};

ENTITYFIELDS_BEGIN(CTriggerBase)
{
	CEntityField ("wait", EntityMemberOffset(CTriggerBase,Wait), FT_FRAMENUMBER | FT_SAVABLE),
	CEntityField ("sounds", EntityMemberOffset(CTriggerBase,Sounds), FT_BYTE | FT_SAVABLE),

	CEntityField ("ThinkType", EntityMemberOffset(CTriggerBase,ThinkType), FT_UINT | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("MoveDir", EntityMemberOffset(CTriggerBase,MoveDir), FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
}
ENTITYFIELDS_END(CTriggerBase)

bool			CTriggerBase::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerBase> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IBrushModel::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void			CTriggerBase::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerBase> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
	IBrushModel::SaveFields (File);
	ITouchableEntity::SaveFields (File);
}

void			CTriggerBase::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerBase> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
	IBrushModel::LoadFields (File);
	ITouchableEntity::LoadFields (File);
}

/*QUAKED trigger_multiple (.5 .5 .5) ? MONSTER NOT_PLAYER TRIGGERED
Variable sized repeatable trigger.  Must be targeted at one or more entities.
If "delay" is set, the trigger waits some time after activating before firing.
"wait" : Seconds between triggerings. (.2 default)
sounds
1)	secret
2)	beep beep
3)	large switch
4)
set "message" to text string
*/

CTriggerMultiple::CTriggerMultiple () :
  IBaseEntity (),
  CTriggerBase (),
  ActivateUse(false)
  {
  };

CTriggerMultiple::CTriggerMultiple (sint32 Index) :
  IBaseEntity(Index),
  CTriggerBase (Index),
  ActivateUse(false)
  {
  };

void CTriggerMultiple::Use (IBaseEntity *Other, IBaseEntity *Activator)
{
	if (ActivateUse)
	{
		ActivateUse = false;
		GetSolid() = SOLID_TRIGGER;
		Link ();
		return;
	}
	CTriggerBase::Use (Other, Activator);
};

void CTriggerMultiple::Spawn ()
{
	Touchable = true;
	switch (Sounds)
	{
	case 1:
		NoiseIndex = SoundIndex ("misc/secret.wav");
		break;
	case 2:
		NoiseIndex = SoundIndex ("misc/talk.wav");
		break;
	case 3:
		NoiseIndex = SoundIndex ("misc/trigger1.wav");
		break;
	}

	if (!Wait)
		Wait = 2;

	if (SpawnFlags & (TRIGGER_TRIGGERED
#if ROGUE_FEATURES
		| TRIGGER_TOGGLE
#endif
		))
	{
		GetSolid() = SOLID_NOT;
		ActivateUse = true;
	}
	else
	{
		GetSolid() = SOLID_TRIGGER;
		ActivateUse = false;
	}

	if (State.GetAngles() != vec3fOrigin)
		SetMoveDir ();

	SetBrushModel ();

	if (!CvarList[CV_MAP_DEBUG].Boolean())
		GetSvFlags() |= SVF_NOCLIENT;
	else
	{
		GetSolid() = SOLID_BBOX;
		GetSvFlags() = (SVF_MONSTER|SVF_DEADMONSTER);
	}

	Link ();
};

void CTriggerMultiple::SaveFields (CFile &File)
{
	File.Write<bool> (ActivateUse);
	CTriggerBase::SaveFields (File);
};

void CTriggerMultiple::LoadFields (CFile &File)
{
	ActivateUse = File.Read<bool> ();
	CTriggerBase::LoadFields (File);
};

LINK_CLASSNAME_TO_CLASS ("trigger_multiple", CTriggerMultiple);

/*QUAKED trigger_once (.5 .5 .5) ? x x TRIGGERED
Triggers once, then removes itself.
You must set the key "target" to the name of another object in the level that has a matching "targetname".

If TRIGGERED, this trigger must be triggered before it is live.

sounds
 1)	secret
 2)	beep beep
 3)	large switch
 4)

"message"	string to be displayed when triggered
*/

class CTriggerOnce : public CTriggerMultiple
{
public:
	CTriggerOnce () :
	  IBaseEntity (),
	  CTriggerMultiple ()
	  {
	  };

	CTriggerOnce (sint32 Index) :
	  IBaseEntity (Index),
	  CTriggerMultiple (Index)
	  {
	  };

	ENTITYFIELDS_SAVABLE(CTriggerOnce)

	void Spawn ()
	{
		// make old maps work because I messed up on flag assignments here
		// triggered was on bit 1 when it should have been on bit 4
		if (SpawnFlags & 1)
		{
			SpawnFlags &= ~1;
			SpawnFlags |= TRIGGER_TRIGGERED;

			vec3f origin = GetMins().MultiplyAngles (0.5f, GetSize());
			MapPrint (MAPPRINT_WARNING, this, origin, "Fixed TRIGGERED flag\n");
		}

		Wait = -1;
		CTriggerMultiple::Spawn ();
	};

	bool CheckValidity ()
	{
		// Yet another map hack
		if (!Q_stricmp(Level.ServerLevelName.CString(), "command") && !Q_stricmp(Model, "*27"))
			SpawnFlags &= ~SPAWNFLAG_NOT_HARD;
		return IMapEntity::CheckValidity ();
	};
};

void CTriggerOnce::SaveFields (CFile &File)
{
	File.Write<bool> (ActivateUse);
	CTriggerBase::SaveFields (File);
};

void CTriggerOnce::LoadFields (CFile &File)
{
	ActivateUse = File.Read<bool> ();
	CTriggerBase::LoadFields (File);
};

LINK_CLASSNAME_TO_CLASS ("trigger_once", CTriggerOnce);

/*QUAKED trigger_counter (.5 .5 .5) ? nomessage
Acts as an intermediary for an action that takes multiple inputs.

If nomessage is not set, t will print "1 more.. " etc when triggered and "sequence complete" when finished.

After the counter has been triggered "count" times (default 2), it will fire all of it's targets and remove itself.
*/

/**
\enum	

\brief	Values that represent spawnflags pertaining to CTriggerCounter. 
**/
enum
{
	COUNTER_NO_MESSAGE	= BIT(0)
};

class CTriggerCounter : public CTriggerMultiple
{
public:
	uint8			Count;

	CTriggerCounter () :
	  IBaseEntity (),
	  CTriggerMultiple ()
	  {
	  };

	CTriggerCounter (sint32 Index) :
	  IBaseEntity (Index),
	  CTriggerMultiple (Index)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTriggerCounter)

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		bool IsClient = true;
		if (Count == 0)
			return;
		if (Activator && !(Activator->EntityFlags & EF_PLAYER))
			IsClient = false;
		
		CPlayerEntity *Player = (IsClient) ? entity_cast<CPlayerEntity>(Activator) : NULL;

		if (--Count)
		{
			if (!(SpawnFlags & COUNTER_NO_MESSAGE))
			{
				if (IsClient)
					Player->PrintToClient (PRINT_CENTER, "%i more to go...", Count);
				Activator->PlaySound (CHAN_AUTO, SoundIndex ("misc/talk1.wav"));
			}
			return;
		}
		
		if (!(SpawnFlags & COUNTER_NO_MESSAGE))
		{
			if (IsClient)
				Player->PrintToClient (PRINT_CENTER, "Sequence completed!");
			Activator->PlaySound (CHAN_AUTO, SoundIndex ("misc/talk1.wav"));
		}

		if (Activator && (Activator->EntityFlags & EF_USABLE))
		{
			IUsableEntity *Usable = entity_cast<IUsableEntity>(Activator);
			
			if (Usable->User)
			{
				DebugPrintf ("Swapped activator %s for %s\n", Activator->ClassName.CString(), Usable->User->ClassName.CString());
				Activator = *Usable->User;
			}
		}

		User = Activator;
		Trigger ();
	};

	void Spawn ()
	{
		Touchable = true;
		Wait = -1;
		if (!Count)
			Count = 2;
	};
};

ENTITYFIELDS_BEGIN(CTriggerCounter)
{
	CEntityField ("count", EntityMemberOffset(CTriggerCounter,Count), FT_BYTE | FT_SAVABLE),
};
ENTITYFIELDS_END(CTriggerCounter);

bool CTriggerCounter::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerCounter> (this, Key, Value))
		return true;

	return CTriggerBase::ParseField (Key, Value);
}

void			CTriggerCounter::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerCounter> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
}

void			CTriggerCounter::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerCounter> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("trigger_counter", CTriggerCounter);

/*QUAKED trigger_push (.5 .5 .5) ? PUSH_ONCE
Pushes the player
"speed"		defaults to 1000
*/
class CTriggerPush : public CTriggerMultiple
{
public:
	bool	Q3Touch;
	float	Speed;

	CTriggerPush () :
	  IBaseEntity (),
	  CTriggerMultiple (),
	  Q3Touch(false),
	  Speed (0)
	  {
	  };

	CTriggerPush (sint32 Index) :
	  IBaseEntity (Index),
	  CTriggerMultiple (Index),
	  Q3Touch(false),
	  Speed (0)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTriggerPush)

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
		vec3f vel = MoveDir * Speed;

		if (Q3Touch)
		{
			if (Other->EntityFlags & EF_PHYSICS)
				entity_cast<IPhysicsEntity>(Other)->Velocity = vel;

			if (Other->EntityFlags & EF_PLAYER)
			{
				// don't take falling damage immediately from this
				CPlayerEntity *Player = entity_cast<CPlayerEntity>(Other);
				Player->Client.OldVelocity = Player->Velocity;
			}
		}
		else
		{
			if (Other->EntityFlags & EF_PHYSICS)
				entity_cast<IPhysicsEntity>(Other)->PushInDirection (vel, SpawnFlags);

			if (SpawnFlags & PUSH_ONCE)
				Free ();
		}
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
#if ROGUE_FEATURES
		GetSolid() = (GetSolid() == SOLID_NOT) ? SOLID_TRIGGER : SOLID_NOT;
		Link ();
#endif
	};

	void Spawn ()
	{
		Touchable = true;
		Init ();

		if (!Speed)
			Speed = 1000;
		Speed *= 10;

		IBaseEntity *target;
		if (Target.IsNullOrEmpty())
			Q3Touch = false;
		else if ((target = CC_Find<IMapEntity, EF_MAP, EntityMemberOffset(IMapEntity,TargetName)> (NULL, Target.CString())) != NULL)
		{
			Free ();
			return;
		}

		Usable = false;

#if ROGUE_FEATURES
		if (!TargetName.IsNullOrEmpty())		// toggleable
		{
			Usable = true;
			if (SpawnFlags & PUSH_START_OFF)
				GetSolid() = SOLID_NOT;
		}
		else if (SpawnFlags & PUSH_START_OFF)
		{
			MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "START_OFF but not targeted.\n");
			GetSvFlags() = 0;
			Touchable = false;
			GetSolid() = SOLID_BSP;
		}
#endif

		Link ();
	};
};

ENTITYFIELDS_BEGIN(CTriggerPush)
{
	CEntityField ("speed", EntityMemberOffset(CTriggerPush,Speed), FT_FLOAT | FT_SAVABLE),
};
ENTITYFIELDS_END(CTriggerPush)

bool			CTriggerPush::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerPush> (this, Key, Value))
		return true;

	// Couldn't find it here
	return CTriggerBase::ParseField (Key, Value);
};

void			CTriggerPush::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerPush> (this, File);
	CTriggerBase::SaveFields (File);
}

void			CTriggerPush::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerPush> (this, File);
	CTriggerBase::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("trigger_push", CTriggerPush);

/*QUAKED trigger_hurt (.5 .5 .5) ? START_OFF TOGGLE SILENT NO_PROTECTION SLOW
Any entity that touches this will be hurt.

It does dmg points of damage each server frame

SILENT			supresses playing the sound
SLOW			changes the damage rate to once per second
NO_PROTECTION	*nothing* stops the damage

"dmg"			default 5 (whole numbers only)

*/

/**
\enum	

\brief	Values that represent spawnflags pertaining to CTriggerHurt. 
**/
enum
{
	HURT_START_OFF		= BIT(0),
	HURT_TOGGLE			= BIT(1),
	HURT_SILENT			= BIT(2),
	HURT_NO_PROTECTION	= BIT(3),
	HURT_SLOW			= BIT(4)
};

class CTriggerHurt : public CTriggerMultiple
{
public:
	FrameNumber		NextHurt;
	sint32					Damage;

	CTriggerHurt () :
	  IBaseEntity (),
	  CTriggerMultiple (),
	  NextHurt(0),
	  Damage (0)
	  {
	  };

	CTriggerHurt (sint32 Index) :
	  IBaseEntity (Index),
	  CTriggerMultiple (Index),
	  NextHurt(0),
	  Damage (0)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTriggerHurt)

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
		if (!((Other->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(Other)->CanTakeDamage))
			return;

		if (NextHurt > Level.Frame)
			return;

		NextHurt = Level.Frame + ((SpawnFlags & HURT_SLOW) ? 10 : ServerFramesPerSecond);
		if (!(SpawnFlags & HURT_SILENT))
		{
			if ((Level.Frame % 10) == 0)
				Other->PlaySound (CHAN_AUTO, NoiseIndex);
		}

		entity_cast<IHurtableEntity>(Other)->TakeDamage (this, this, vec3fOrigin, Other->State.GetOrigin(),
															vec3fOrigin, Damage, Damage,
															(SpawnFlags & 8) ? DAMAGE_NO_PROTECTION : 0, MOD_TRIGGER_HURT);
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		if (ActivateUse)
			return;

		GetSolid() = ((GetSolid() == SOLID_NOT) ? SOLID_TRIGGER : SOLID_NOT);
		Link ();

		if (!(SpawnFlags & HURT_TOGGLE))
			ActivateUse = true;
	};

	void Spawn ()
	{
		Touchable = true;
		Init ();
		NoiseIndex = SoundIndex ("world/electro.wav");

		if (!Damage)
			Damage = 5;

		GetSolid() = ((SpawnFlags & HURT_START_OFF) ? SOLID_NOT : SOLID_TRIGGER);
		ActivateUse = (SpawnFlags & HURT_TOGGLE) ? false : true;
		Link ();
	};
};

ENTITYFIELDS_BEGIN(CTriggerHurt)
{
	CEntityField ("dmg", EntityMemberOffset(CTriggerHurt,Damage), FT_INT | FT_SAVABLE),
};
ENTITYFIELDS_END(CTriggerHurt)

bool			CTriggerHurt::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerHurt> (this, Key, Value))
		return true;

	// Couldn't find it here
	return CTriggerBase::ParseField (Key, Value);
};

void			CTriggerHurt::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerHurt> (this, File);
	CTriggerBase::SaveFields (File);
}

void			CTriggerHurt::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerHurt> (this, File);
	CTriggerBase::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("trigger_hurt", CTriggerHurt);

/*QUAKED trigger_monsterjump (.5 .5 .5) ?
Walking monsters that touch this will jump in the direction of the trigger's angle
"speed" default to 200, the speed thrown forward
"height" default to 200, the speed thrown upwards
*/
class CTriggerMonsterJump : public CTriggerMultiple
{
public:
	float	Speed;

	CTriggerMonsterJump () :
	  IBaseEntity (),
	  CTriggerMultiple (),
	  Speed (0)
	  {
	  };

	CTriggerMonsterJump (sint32 Index) :
	  IBaseEntity (Index),
	  CTriggerMultiple (Index),
	  Speed (0)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTriggerMonsterJump)

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		if (!(Other->EntityFlags & EF_MONSTER))
			return;
		if (Other->GetSvFlags() & SVF_DEADMONSTER)
			return;

	// set XY even if not on ground, so the jump will clear lips
		CMonsterEntity *Monster = entity_cast<CMonsterEntity>(Other);

		if (Monster->Monster->AIFlags & (AI_FLY | AI_SWIM))
			return;

		Monster->Velocity = MoveDir * Speed;
		
		if (!Monster->GroundEntity)
			return;
		
		Monster->GroundEntity = nullentity;
		Monster->Velocity.Z = MoveDir.Z;
	};

	void Spawn ()
	{
		Touchable = true;
		if (!Speed)
			Speed = 200;
		Speed *= 10;

		if (!MoveDir.Z)
			MoveDir.Z = 200;

		if (State.GetAngles().Y == 0)
			State.GetAngles().Y = 360;

		Init ();
	};
};

ENTITYFIELDS_BEGIN(CTriggerMonsterJump)
{
	CEntityField ("speed", EntityMemberOffset(CTriggerMonsterJump,Speed), FT_FLOAT | FT_SAVABLE),
	CEntityField ("height", EntityMemberOffset(CTriggerMonsterJump,MoveDir.Z), FT_FLOAT),
};
ENTITYFIELDS_END(CTriggerMonsterJump)

bool			CTriggerMonsterJump::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerMonsterJump> (this, Key, Value))
		return true;

	// Couldn't find it here
	return CTriggerBase::ParseField (Key, Value);
};

void			CTriggerMonsterJump::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerMonsterJump> (this, File);
	CTriggerBase::SaveFields (File);
}

void			CTriggerMonsterJump::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerMonsterJump> (this, File);
	CTriggerBase::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("trigger_monsterjump", CTriggerMonsterJump);

/*QUAKED trigger_gravity (.5 .5 .5) ?
Changes the touching entites gravity to
the value of "gravity".  1.0 is standard
gravity for the Level.
*/
#if ROGUE_FEATURES
/**
\enum	

\brief	Values that represent spawnflags pertaining to CTriggerGravity. 
**/
enum
{
	GRAVITY_TOGGLE		= BIT(0),
	GRAVITY_START_OFF	= BIT(1)
};
#endif

class CTriggerGravity : public CTriggerMultiple
{
public:
	float	Gravity;

	CTriggerGravity () :
	  IBaseEntity (),
	  CTriggerMultiple (),
	  Gravity (0)
	  {
	  };

	CTriggerGravity (sint32 Index) :
	  IBaseEntity (Index),
	  CTriggerMultiple (Index),
	  Gravity (0)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTriggerGravity)

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
		if (Other->EntityFlags & EF_PHYSICS)
			entity_cast<IPhysicsEntity>(Other)->GravityMultiplier = Gravity;
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
#if ROGUE_FEATURES
		GetSolid() = (GetSolid() == SOLID_NOT) ? SOLID_TRIGGER : SOLID_NOT;
		Link ();
#endif
	};

	void Spawn ()
	{
		Touchable = true;
		Usable = false;
		if (!Gravity)
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No gravity set\n");
			Free ();
			return;
		}

#if ROGUE_FEATURES
		if (SpawnFlags & GRAVITY_TOGGLE)
			Usable = true;

		if (SpawnFlags & GRAVITY_START_OFF)
		{
			Usable = true;
			GetSolid() = SOLID_NOT;
		}
#endif

		Init ();
	};
};

ENTITYFIELDS_BEGIN(CTriggerGravity)
{
	CEntityField ("gravity",		EntityMemberOffset(CTriggerGravity,Gravity),		FT_FLOAT | FT_SAVABLE),
};
ENTITYFIELDS_END(CTriggerGravity)

bool			CTriggerGravity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerGravity> (this, Key, Value))
		return true;

	// Couldn't find it here
	return false;
};

void			CTriggerGravity::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerGravity> (this, File);
}

void			CTriggerGravity::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerGravity> (this, File);
}

LINK_CLASSNAME_TO_CLASS ("trigger_gravity", CTriggerGravity);

/*
==============================================================================

trigger_key

==============================================================================
*/

/*QUAKED trigger_key (.5 .5 .5) (-8 -8 -8) (8 8 8)
A relay trigger that only fires it's targets if player has the proper key.
Use "item" to specify the required key, for example "key_data_cd"
*/
class CTriggerKey : public IMapEntity, public IUsableEntity
{
public:
	IBaseItem			*Item;
	FrameNumber		TouchDebounce;

	CTriggerKey () :
	  IBaseEntity (),
	  IMapEntity (),
	  IUsableEntity (),
	  TouchDebounce(0),
	  Item(NULL)
	{
	};

	CTriggerKey (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index),
	  TouchDebounce(0),
	  Item(NULL)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTriggerKey)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		if (!Usable)
			return;
		if (!Item)
			return;
		if (!(Activator->EntityFlags & EF_PLAYER))
			return;

		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Activator);

		sint32 index = Item->GetIndex();
		if (!Player->Client.Persistent.Inventory.Has(index))
		{
			if (Level.Frame < TouchDebounce)
				return;
			TouchDebounce = Level.Frame + 50;

			if (Message.IsNullOrEmpty())
				Player->PrintToClient (PRINT_CENTER, "You need the %s", Item->Name);
			else
				Player->PrintToClient (PRINT_CENTER, "%s", Message.CString());
			Player->PlaySound (CHAN_AUTO, SoundIndex ("misc/keytry.wav"));
			return;
		}

		Player->PlaySound (CHAN_AUTO, SoundIndex ("misc/keyuse.wav"));
		if (Game.GameMode & GAME_COOPERATIVE)
		{
			if (Item == NItems::PowerCube)
			{
				sint32	cube;
				for (cube = 0; cube < 8; cube++)
				{
					if (Player->Client.Persistent.PowerCubeCount & (1 << cube))
						break;
				}

				for (sint32 player = 1; player <= Game.MaxClients; player++)
				{
					CPlayerEntity *LoopPlayer = entity_cast<CPlayerEntity>(Game.Entities[player].Entity);
					if (!LoopPlayer->GetInUse())
						continue;
					if (LoopPlayer->Client.Persistent.PowerCubeCount & (1 << cube))
					{
						LoopPlayer->Client.Persistent.Inventory -= index;
						LoopPlayer->Client.Persistent.PowerCubeCount &= ~(1 << cube);
					}
				}
			}
			else
			{
				for (sint32 player = 1; player <= Game.MaxClients; player++)
				{
					CPlayerEntity *LoopPlayer = entity_cast<CPlayerEntity>(Game.Entities[player].Entity);
					if (!LoopPlayer->GetInUse())
						continue;
					LoopPlayer->Client.Persistent.Inventory.Set(index, 0);
				}
			}
		}
		else
			Player->Client.Persistent.Inventory -= index;

		UseTargets (Activator, Message);

		Usable = false;
	};

	void Spawn ()
	{
		if (Target.IsNullOrEmpty())
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No target\n");
			return;
		}

		SoundIndex ("misc/keytry.wav");
		SoundIndex ("misc/keyuse.wav");
	};
};

ENTITYFIELDS_BEGIN(CTriggerKey)
{
	CEntityField ("item", EntityMemberOffset(CTriggerKey,Item), FT_ITEM | FT_SAVABLE),
};
ENTITYFIELDS_END(CTriggerKey)

bool CTriggerKey::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerKey> (this, Key, Value))
		return true;

	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
}

void			CTriggerKey::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerKey> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
}

void			CTriggerKey::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerKey> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("trigger_key", CTriggerKey);
