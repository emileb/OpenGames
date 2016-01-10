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
// cc_target_entities.cpp
// 
//

#include "Local.h"
#include "Entities/TargetEntities.h"
#include "Utility/TemporaryEntities.h"
#include "Items/Items.h"

/*QUAKED target_speaker (1 0 0) (-8 -8 -8) (8 8 8) looped-on looped-off reliable
"noise"		wav file to play
"attenuation"
-1 = none, send to whole level
1 = normal fighting sounds
2 = idle sound level
3 = ambient sound level
"volume"	0.0 to 1.0

Normal sounds play each time the target is used.  The reliable flag can be set for crucial voiceovers.

Looped sounds are always atten 3 / vol 1, and the use function toggles it on/off.
Multiple identical looping sounds will just increase volume without any speed cost
*/

/**
\enum	

\brief	Values that represent spawnflags pertaining to CTargetSpeaker. 
**/
enum
{
	SPEAKER_LOOPED_ON		= BIT(0),
	SPEAKER_LOOPED_OFF		= BIT(1),
	SPEAKER_RELIABLE		= BIT(2)
};

class CTargetSpeaker : public IMapEntity, public IUsableEntity
{
public:
	uint8		Volume;
	sint32		Attenuation;

	CTargetSpeaker () :
	  IBaseEntity (),
	  IMapEntity (),
	  IUsableEntity (),
	  Volume(0),
	  Attenuation(0)
	  {
	  };

	CTargetSpeaker (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index),
	  Volume(0),
	  Attenuation(0)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetSpeaker)

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		if (SpawnFlags & (SPEAKER_LOOPED_ON|SPEAKER_LOOPED_OFF)) // looping sound toggles
			State.GetSound() = (State.GetSound() ? 0 : NoiseIndex); // start or stop it
		else
			// use a positioned_sound, because this entity won't normally be
			// sent to any clients because it is invisible
			PlayPositionedSound (State.GetOrigin(), (SpawnFlags & SPEAKER_RELIABLE) ? CHAN_VOICE|CHAN_RELIABLE : CHAN_VOICE, NoiseIndex, Volume, Attenuation);
	};

	void Spawn ()
	{
		if(!NoiseIndex)
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No or missing noise set\n");
			return;
		}

		if (!Volume)
			Volume = 255;

		switch (Attenuation)
		{
		case 0:
			Attenuation = 1;
			break;
		case -1: // use -1 so 0 defaults to 1
			Attenuation = 0;
			break;
		};

		// check for prestarted looping sound
		if (SpawnFlags & SPEAKER_LOOPED_ON)
			State.GetSound() = NoiseIndex;

		// must link the entity so we get areas and clusters so
		// the server can determine who to send updates to
		Link ();
	};
};

ENTITYFIELDS_BEGIN(CTargetSpeaker)
{
	CEntityField ("volume", EntityMemberOffset(CTargetSpeaker,Volume), FT_FLOAT | FT_SAVABLE),
	CEntityField ("attenuation", EntityMemberOffset(CTargetSpeaker,Attenuation), FT_INT | FT_SAVABLE)
};
ENTITYFIELDS_END(CTargetSpeaker)

bool			CTargetSpeaker::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetSpeaker> (this, Key, Value))
		return true;

	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void		CTargetSpeaker::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetSpeaker> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
}

void		CTargetSpeaker::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetSpeaker> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_speaker", CTargetSpeaker);

class CTargetExplosion : public IMapEntity, public IThinkableEntity, public IUsableEntity
{
public:
	sint32			Damage;

	CTargetExplosion () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity (),
	  IUsableEntity (),
	  Damage(0)
	{
	};

	CTargetExplosion (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index),
	  IUsableEntity (Index),
	  Damage(0)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetExplosion)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Think ()
	{
		CRocketExplosion (State.GetOrigin()).Send();

		if (Damage)
			SplashDamage (*User, Damage, NULL, Damage+40, MOD_EXPLOSIVE);

		FrameNumber save = Delay;
		Delay = 0;
		UseTargets (*User, Message);
		Delay = save;

		User = nullentity;
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		User = Activator;

		if (!Delay)
		{
			Think ();
			return;
		}

		NextThink = Level.Frame + Delay;
	};

	void Spawn ()
	{
		GetSvFlags() = SVF_NOCLIENT;
	};
};

ENTITYFIELDS_BEGIN(CTargetExplosion)
{
	CEntityField ("dmg", EntityMemberOffset(CTargetExplosion,Damage), FT_INT | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetExplosion)

bool			CTargetExplosion::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetExplosion> (this, Key, Value))
		return true;

	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void		CTargetExplosion::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetExplosion> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
}

void		CTargetExplosion::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetExplosion> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_explosion", CTargetExplosion);

/*QUAKED target_spawner (1 0 0) (-8 -8 -8) (8 8 8)
Set target to the type of entity you want spawned.
Useful for spawning monsters and gibs in the factory levels.

For monsters:
	Set direction to the facing you want it to have.

For gibs:
	Set direction if you want it moving and
	speed how fast it should be moving otherwise it
	will just be dropped
*/
class CTargetSpawner : public IMapEntity, public IUsableEntity
{
public:
	vec3f	MoveDir;
	float	Speed;

	CTargetSpawner () :
	  IBaseEntity (),
	  IMapEntity (),
	  IUsableEntity (),
	  Speed (0)
	{
	};

	CTargetSpawner (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index),
	  Speed (0)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetSpawner)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		IBaseEntity *Entity = CreateEntityFromClassname(Target);

		if (!Entity)
			return;

#if ROGUE_FEATURES
		Entity->State.GetRenderEffects() = RF_IR_VISIBLE;
#endif
		Entity->State.GetOrigin() = State.GetOrigin();
		Entity->State.GetAngles() = State.GetAngles();

		Entity->Unlink ();
		Entity->KillBox ();
		Entity->Link ();

		if (Speed && (Entity->EntityFlags & EF_PHYSICS))
			entity_cast<IPhysicsEntity>(Entity)->Velocity = MoveDir;
	};

	void Spawn ()
	{
		GetSvFlags() = SVF_NOCLIENT;
		if (Speed)
		{
			SetMoveDir (MoveDir);
			MoveDir *= Speed;
		}
	};
};

ENTITYFIELDS_BEGIN(CTargetSpawner)
{
	CEntityField ("speed", EntityMemberOffset(CTargetSpawner,Speed), FT_FLOAT | FT_SAVABLE),

	CEntityField ("MoveDir", EntityMemberOffset(CTargetSpawner,MoveDir), FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetSpawner)

bool			CTargetSpawner::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetSpawner> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void		CTargetSpawner::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetSpawner> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
}

void		CTargetSpawner::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetSpawner> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_spawner", CTargetSpawner);

/*QUAKED target_splash (1 0 0) (-8 -8 -8) (8 8 8)
Creates a particle splash effect when used.

Set "sounds" to one of the following:
  1) sparks
  2) blue water
  3) brown water
  4) slime
  5) lava
  6) blood

"count"	how many pixels in the splash
"dmg"	if set, does a radius damage at this location when it splashes
		useful for lava/sparks
*/
class CTargetSplash : public IMapEntity, public IUsableEntity
{
public:
	vec3f	MoveDir;
	sint32	Damage;
	uint8	Color;
	uint8	Count;

	CTargetSplash () :
	  IBaseEntity (),
	  IMapEntity (),
	  IUsableEntity (),
	  Damage (0),
	  Count (0)
	{
	};

	CTargetSplash (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index),
	  Damage (0),
	  Count (0)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetSplash)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		CSplash(State.GetOrigin(), MoveDir, Color, Count).Send();

		if (Damage)
			SplashDamage (Activator, Damage, NULL, Damage+40, MOD_SPLASH);
	};

	void Spawn ()
	{
		SetMoveDir (MoveDir);

		if (!Count)
			Count = 32;

		GetSvFlags() = SVF_NOCLIENT;
	};
};

ENTITYFIELDS_BEGIN(CTargetSplash)
{
	CEntityField ("dmg", EntityMemberOffset(CTargetSplash,Damage), FT_INT | FT_SAVABLE),
	CEntityField ("sounds", EntityMemberOffset(CTargetSplash,Color), FT_BYTE | FT_SAVABLE),
	CEntityField ("count", EntityMemberOffset(CTargetSplash,Count), FT_BYTE | FT_SAVABLE),

	CEntityField ("MoveDir", EntityMemberOffset(CTargetSplash,MoveDir), FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetSplash)

bool			CTargetSplash::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetSplash> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void		CTargetSplash::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetSplash> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
}

void		CTargetSplash::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetSplash> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_splash", CTargetSplash);

/*QUAKED target_temp_entity (1 0 0) (-8 -8 -8) (8 8 8)
Fire an origin based temp entity event to the clients.
"style"		type uint8
*/
class CTargetTempEntity : public IMapEntity, public IUsableEntity
{
public:
	uint8		Style;

	CTargetTempEntity () :
	  IBaseEntity (),
	  IMapEntity (),
	  IUsableEntity ()
	{
	};

	CTargetTempEntity (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetTempEntity)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		WriteByte (SVC_TEMP_ENTITY);
		WriteByte (Style);
		WritePosition (State.GetOrigin());
		Cast (CASTFLAG_PVS, State.GetOrigin());
	};

	void Spawn ()
	{
	};
};

ENTITYFIELDS_BEGIN(CTargetTempEntity)
{
	CEntityField ("style", EntityMemberOffset(CTargetTempEntity,Style), FT_BYTE | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetTempEntity)

bool CTargetTempEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetTempEntity> (this, Key, Value))
		return true;

	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
}

void		CTargetTempEntity::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetTempEntity> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
}

void		CTargetTempEntity::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetTempEntity> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_temp_entity", CTargetTempEntity);

#include "Entities/InfoEntities.h"

/*QUAKED target_changelevel (1 0 0) (-8 -8 -8) (8 8 8)
Changes level to "map" when fired
*/
void BeginIntermission (CTargetChangeLevel *targ)
{
	if (Level.Intermission.Time)
		return;		// already activated

#if CLEANCTF_ENABLED
//ZOID
	if (Game.GameMode & GAME_CTF)
		CTFCalcScores();
//ZOID
#endif

	Game.AutoSaved = false;

	// respawn any dead clients
	for (sint32 i = 0; i < Game.MaxClients; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>((Game.Entities + 1 + i)->Entity);
		if (!Player->GetInUse())
			continue;
		if (Player->Health <= 0)
			Player->Respawn();
	}

	Level.Intermission.Time = Level.Frame;
#if !STDCPP_LINUX_HACK
	new (&Level.Intermission.ChangeMap) String(targ->Map); // Solving a linux quirk
	if (Level.Intermission.ChangeMap.IndexOf('*') != -1)
#else
	Level.Intermission.ChangeMap = Mem_TagStrDup(targ->Map.CString(), TAG_LEVEL);
        if (strstr(Level.Intermission.ChangeMap, "*"))
#endif
	{
		if (Game.GameMode & GAME_COOPERATIVE)
		{
			for (sint32 i = 0; i < Game.MaxClients; i++)
			{
				CPlayerEntity *Player = entity_cast<CPlayerEntity>((Game.Entities + 1 + i)->Entity);
				if (!Player->GetInUse())
					continue;
				// strip players of all keys between units
				for (uint16 n = 0; n < MAX_CS_ITEMS; n++)
				{
					if (n >= GetNumItems())
						break;
					if (GetItemByIndex(n)->Flags & ITEMFLAG_KEY)
						Player->Client.Persistent.Inventory.Set(n, 0);
				}
			}
		}
	}
	else
	{
		if (!(Game.GameMode & GAME_DEATHMATCH))
		{
			Level.Intermission.ShouldExit = true;		// go immediately to the next level
			if (targ->ExitOnNextFrame)
				Level.Intermission.ShouldExitOnNextFrame = true;
			return;
		}
	}

	Level.Intermission.ShouldExitOnNextFrame = false;

	// find an intermission spot
	IBaseEntity *Entity;
	if (!CPlayerIntermission::SpawnPoints().size())
	{
		// the map creator forgot to put in an intermission point...
		if (!CPlayerStart::SpawnPoints().size())
			Entity = CPlayerDeathmatch::SpawnPoints()[0];
		else
			Entity = CPlayerStart::SpawnPoints()[0];
	}
	else // choose spot at random
		Entity = CPlayerIntermission::SpawnPoints()[irandom(CPlayerIntermission::SpawnPoints().size())];

	Level.Intermission.Origin = Entity->State.GetOrigin ();
	Level.Intermission.Angles = Entity->State.GetAngles ();

	// move all clients to the intermission point
	for (sint32 i = 0; i < Game.MaxClients; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>((Game.Entities + 1 + i)->Entity);
		if (!Player->GetInUse())
			continue;
		Player->MoveToIntermission();
	}
}

CTargetChangeLevel::CTargetChangeLevel () :
	IBaseEntity (),
	IMapEntity (),
	IUsableEntity (),
	Map(),
	ExitOnNextFrame(false)
{
};

CTargetChangeLevel::CTargetChangeLevel (sint32 Index) :
	IBaseEntity (Index),
	IMapEntity (Index),
	IUsableEntity (Index),
	Map(),
	ExitOnNextFrame(false)
{
};

bool CTargetChangeLevel::Run ()
{
	return IBaseEntity::Run ();
};

void CTargetChangeLevel::Use (IBaseEntity *Other, IBaseEntity *Activator)
{
	if (Level.Intermission.Time)
		return;		// already activated

	if (Game.GameMode & GAME_SINGLEPLAYER)
	{
		if (entity_cast<CPlayerEntity>(Game.Entities[1].Entity)->Health <= 0)
			return;
	}

	// if noexit, do a ton of damage to other
	if ((Game.GameMode & GAME_DEATHMATCH) && !DeathmatchFlags.dfAllowExit.IsEnabled() && (Other != World))
	{
		if ((Other->EntityFlags & EF_HURTABLE))
		{
			IHurtableEntity *Hurtable = entity_cast<IHurtableEntity>(Other);

			if (Hurtable->CanTakeDamage)
				Hurtable->TakeDamage (this, this, vec3fOrigin, Other->State.GetOrigin(), vec3fOrigin, 10 * Hurtable->MaxHealth, 1000, 0, MOD_EXIT);
		}
		return;
	}

	// if multiplayer, let everyone know who hit the exit
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		if (Activator && (Activator->EntityFlags & EF_PLAYER))
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(Activator);
			BroadcastPrintf (PRINT_HIGH, "%s exited the Level.\n", Player->Client.Persistent.Name.CString());
		}
	}

	// if going to a new unit, clear cross triggers
	if (Map.IndexOf('*') != -1)	
		Game.ServerFlags &= ~(SFL_CROSS_TRIGGER_MASK);

	BeginIntermission (this);
};

void CTargetChangeLevel::Spawn ()
{
	if (Map.IsNullOrEmpty())
	{
		MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No map\n");
		Free ();
		return;
	}

	// ugly hack because *SOMEBODY* screwed up their map
	if (Level.ServerLevelName == "fact1" && Map == "fact3")
	{
		Map = "fact3$secret1";
		// Paril
		// ...
		ExitOnNextFrame = true;
	}

	GetSvFlags() = SVF_NOCLIENT;
};

ENTITYFIELDS_BEGIN(CTargetChangeLevel)
{
	CEntityField ("map", EntityMemberOffset(CTargetChangeLevel,Map), FT_STRING | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetChangeLevel)

bool			CTargetChangeLevel::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetChangeLevel> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void			CTargetChangeLevel::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetChangeLevel> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
}

void			CTargetChangeLevel::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetChangeLevel> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_changelevel", CTargetChangeLevel);

CTargetChangeLevel *CreateTargetChangeLevel(const char *map)
{
	CTargetChangeLevel *Temp = QNewEntityOf CTargetChangeLevel;
	Temp->ClassName = "target_changelevel";

	Level.NextMap = map;
	Temp->Map = (char*)Level.NextMap.CString();

	return Temp;
}

/*QUAKED target_crosslevel_trigger (.5 .5 .5) (-8 -8 -8) (8 8 8) trigger1 trigger2 trigger3 trigger4 trigger5 trigger6 trigger7 trigger8
Once this trigger is touched/used, any trigger_crosslevel_target with the same trigger number is automatically used when a level is started within the same unit.  It is OK to check multiple triggers.  Message, delay, target, and killtarget also work.
*/
class CTargetCrossLevelTrigger : public IMapEntity, public IUsableEntity
{
public:
	CTargetCrossLevelTrigger () :
	  IBaseEntity (),
	  IMapEntity (),
	  IUsableEntity ()
	{
	};

	CTargetCrossLevelTrigger (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CTargetCrossLevelTrigger)

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
	};

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		Game.ServerFlags |= SpawnFlags;
		Free ();
	};

	void Spawn ()
	{
		GetSvFlags() = SVF_NOCLIENT;
	};
};

LINK_CLASSNAME_TO_CLASS ("target_crosslevel_trigger", CTargetCrossLevelTrigger);

/*QUAKED target_crosslevel_target (.5 .5 .5) (-8 -8 -8) (8 8 8) trigger1 trigger2 trigger3 trigger4 trigger5 trigger6 trigger7 trigger8
Triggered by a trigger_crosslevel elsewhere within a unit.  If multiple triggers are checked, all must be true.  Delay, target and
killtarget also work.

"delay"		delay before using targets if the trigger has been activated (default 1)
*/
class CTargetCrossLevelTarget : public IMapEntity, public IThinkableEntity, public IUsableEntity
{
public:
	CTargetCrossLevelTarget () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity ()
	{
	};

	CTargetCrossLevelTarget (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CTargetCrossLevelTarget)

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File);

	void Use (IBaseEntity *, IBaseEntity *)
	{
	};

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Think ()
	{
		if (SpawnFlags == (Game.ServerFlags & SFL_CROSS_TRIGGER_MASK & SpawnFlags))
		{
			UseTargets (this, Message);
			Free ();
		}
	};

	void Spawn ()
	{
		if (!Delay)
			Delay = 1;
		GetSvFlags() = SVF_NOCLIENT;
		
		NextThink = Level.Frame + Delay;
	};

	void FireTarget ();
};

LINK_CLASSNAME_TO_CLASS ("target_crosslevel_target", CTargetCrossLevelTarget);

typedef std::vector<CTargetCrossLevelTarget*> CrossLevelTargetList;

CrossLevelTargetList &GetCrossLevelTargetList ()
{
	static CrossLevelTargetList List;
	return List;
}

void CTargetCrossLevelTarget::LoadFields (CFile &File)
{
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);

	GetCrossLevelTargetList().push_back (this);
}

void CTargetCrossLevelTarget::FireTarget ()
{
	NextThink = Level.Frame + Delay;
}

void FireCrossLevelTargets ()
{
	for (CrossLevelTargetList::iterator it = GetCrossLevelTargetList().begin(); it < GetCrossLevelTargetList().end(); ++it)
		(*it)->FireTarget ();

	GetCrossLevelTargetList().clear();
}

/*QUAKED target_secret (1 0 1) (-8 -8 -8) (8 8 8)
Counts a secret found.
These are single use targets.
*/
class CTargetSecret : public IMapEntity, public IUsableEntity
{
public:
	CTargetSecret () :
	  IBaseEntity (),
	  IMapEntity (),
	  IUsableEntity ()
	{
	};

	CTargetSecret (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CTargetSecret)

	virtual bool ParseField (const char *Key, const char *Value);

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
	};

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		PlaySound (CHAN_VOICE, NoiseIndex);

		Level.Secrets.Found++;

		UseTargets (Activator, Message);
		Free ();
	};

	void Spawn ()
	{
		if (Game.GameMode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		if (!NoiseIndex)
			NoiseIndex = SoundIndex("misc/secret.wav");

		GetSvFlags() = SVF_NOCLIENT;
		Level.Secrets.Total++;
		// map bug hack

		if (!Q_stricmp(Level.ServerLevelName.CString(), "mine3") && (State.GetOrigin() == vec3f(280, -2048, -624)))
			//(State.GetOrigin().X == 280 && State.GetOrigin().Y == -2048 && State.GetOrigin().Z == -624))
			Message = "You have found a secret area.";
	};
};

bool			CTargetSecret::ParseField (const char *Key, const char *Value)
{
	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
}

LINK_CLASSNAME_TO_CLASS ("target_secret", CTargetSecret);

/*QUAKED target_goal (1 0 1) (-8 -8 -8) (8 8 8)
Counts a goal completed.
These are single use targets.
*/
class CTargetGoal : public IMapEntity, public IUsableEntity
{
public:
	CTargetGoal () :
	  IBaseEntity (),
	  IMapEntity (),
	  IUsableEntity ()
	{
	};

	CTargetGoal (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CTargetGoal)

	virtual bool ParseField (const char *Key, const char *Value);

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
	};

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		PlaySound (CHAN_VOICE, NoiseIndex);

		Level.Goals.Found++;

		if (Level.Goals.Found == Level.Goals.Total)
			ConfigString (CS_CDTRACK, "0");

		UseTargets (Activator, Message);
		Free ();
	};

	void Spawn ()
	{
		if (Game.GameMode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		if (!NoiseIndex)
			NoiseIndex = SoundIndex ("misc/secret.wav");

		GetSvFlags() = SVF_NOCLIENT;
		Level.Goals.Total++;
	};
};

bool			CTargetGoal::ParseField (const char *Key, const char *Value)
{
	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
}

LINK_CLASSNAME_TO_CLASS ("target_goal", CTargetGoal);

/*QUAKED target_blaster (1 0 0) (-8 -8 -8) (8 8 8) NOTRAIL NOEFFECTS
Fires a blaster bolt in the set direction when triggered.

dmg		default is 15
speed	default is 1000
*/

/**
\enum	

\brief	Values that represent spawnflags pertaining to CTargetBlaster. 
**/
enum
{
	BLASTER_NO_TRAIL		= BIT(0),
	BLASTER_NO_EFFECTS		= BIT(1)
};

class CTargetBlaster : public IMapEntity, public IUsableEntity
{
public:
	vec3f		MoveDir;
	float		Speed;
	sint32		Damage;

	CTargetBlaster () :
	  IBaseEntity (),
	  IMapEntity (),
	  IUsableEntity (),
	  Speed (0),
	  Damage (0)
	{
	};

	CTargetBlaster (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index),
	  Speed (0),
	  Damage (0)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetBlaster)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		CBlasterProjectile::Spawn (this, State.GetOrigin(), MoveDir, Damage, Speed, (SpawnFlags & BLASTER_NO_EFFECTS) ? 0 : ((SpawnFlags & BLASTER_NO_TRAIL) ? FX_HYPERBLASTER : FX_BLASTER), true);
		PlaySound (CHAN_VOICE, NoiseIndex);
	};

	void Spawn ()
	{
		SetMoveDir (MoveDir);
		NoiseIndex = SoundIndex ("weapons/laser2.wav");

		if (!Damage)
			Damage = 15;
		if (!Speed)
			Speed = 1000;

		GetSvFlags() = SVF_NOCLIENT;
	};
};

ENTITYFIELDS_BEGIN(CTargetBlaster)
{
	CEntityField ("speed", EntityMemberOffset(CTargetBlaster,Speed), FT_FLOAT | FT_SAVABLE),
	CEntityField ("dmg", EntityMemberOffset(CTargetBlaster,Damage), FT_INT | FT_SAVABLE),

	CEntityField ("MoveDir", EntityMemberOffset(CTargetBlaster,MoveDir), FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetBlaster)

bool			CTargetBlaster::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetBlaster> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void		CTargetBlaster::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetBlaster> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
}

void		CTargetBlaster::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetBlaster> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_blaster", CTargetBlaster);

/*QUAKED target_laser (0 .5 .8) (-8 -8 -8) (8 8 8) START_ON RED GREEN BLUE YELLOW ORANGE FAT
When triggered, fires a laser.  You can either set a target
or a direction.
*/

CTargetLaser::CTargetLaser () :
  IBaseEntity (),
  IMapEntity (),
  IThinkableEntity (),
  IUsableEntity (),
  StartLaser(true),
  MakeEffect(false),
  Damage (0)
{
    	User = World;
};

CTargetLaser::CTargetLaser (sint32 Index) :
  IBaseEntity (Index),
  IMapEntity (Index),
  IThinkableEntity (Index),
  IUsableEntity (Index),
  StartLaser(true),
  MakeEffect(false),
  Damage (0)
{
    	User = World;
};

bool CTargetLaser::Run ()
{
	return IBaseEntity::Run();
};

void CTargetLaser::Think ()
{
	if (StartLaser)
	{
		Start ();
		return;
	}

	IBaseEntity	*ignore;
	vec3f	start;
	vec3f	end;
	const uint8 Count = (MakeEffect) ? 8 : 4;

	if (Enemy)
	{
		vec3f last_movedir = MoveDir;
		vec3f point = Enemy->GetAbsMin().MultiplyAngles (0.5f, Enemy->GetSize());

		MoveDir = point - State.GetOrigin();
		MoveDir.Normalize ();
		if (MoveDir != last_movedir)
			MakeEffect = true;
	}

	ignore = this;
	start = State.GetOrigin();
	end = start.MultiplyAngles (2048, MoveDir);
	CTrace tr;
	while(1)
	{
		tr (start, end, ignore, (SpawnFlags & LASER_STOPWINDOW) ? CONTENTS_MASK_SHOT : CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);

		if (!tr.Entity)
			break;

		IBaseEntity *Entity = tr.Entity;
		// hurt it if we can
		if ((Entity->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(Entity)->CanTakeDamage)
			entity_cast<IHurtableEntity>(Entity)->TakeDamage (this, *User, MoveDir, tr.EndPosition, vec3fOrigin, Damage, 1, DAMAGE_ENERGY, MOD_TARGET_LASER);

		// if we hit something that's not a monster or player or is immune to lasers, we're done
		if (!(Entity->EntityFlags & EF_MONSTER) && (!(Entity->EntityFlags & EF_PLAYER)))
		{
			if (MakeEffect)
			{
				MakeEffect = false;
				CSparks(tr.EndPosition, tr.Plane.Normal, ST_LASER_SPARKS, (State.GetSkinNum() & 255), Count).Send();
			}
			break;
		}

		ignore = tr.Entity;
		start = tr.EndPosition;
	}

	State.GetOldOrigin() = tr.EndPosition;
	NextThink = Level.Frame + ServerFramesPerSecond;
};

void CTargetLaser::Use (IBaseEntity *Other, IBaseEntity *Activator)
{
	if (!Usable)
		return;

	User = Activator;
	if (SpawnFlags & LASER_START_ON)
		Off ();
	else
		On ();
};

void CTargetLaser::On ()
{
	SpawnFlags |= LASER_START_ON;
	MakeEffect = true;
	GetSvFlags() &= ~SVF_NOCLIENT;
	Think ();
};
void CTargetLaser::Off ()
{
	SpawnFlags &= ~LASER_START_ON;
	GetSvFlags() |= SVF_NOCLIENT;
	NextThink = 0;
	User = nullentity;
};
void CTargetLaser::Start ()
{
	GetSolid() = SOLID_NOT;
	State.GetRenderEffects() |= (RF_BEAM|RF_TRANSLUCENT);
	State.GetModelIndex() = 1;			// must be non-zero

	// set the beam diameter
	State.GetFrame() = (SpawnFlags & LASER_FAT) ? 16 : 4;

	// set the color
	if (SpawnFlags & LASER_RED)
		State.GetSkinNum() = Color_RGBAToHex (NSColor::PatriotRed, NSColor::PatriotRed, NSColor::Red, NSColor::Red);
	else if (SpawnFlags & LASER_GREEN)
		State.GetSkinNum() = Color_RGBAToHex (NSColor::Green, NSColor::Lime, NSColor::FireSpeechGreen, NSColor::Harlequin);
	else if (SpawnFlags & LASER_BLUE)
		State.GetSkinNum() = Color_RGBAToHex (NSColor::PatriotBlue, NSColor::PatriotBlue, NSColor::NeonBlue, NSColor::NeonBlue);
	else if (SpawnFlags & LASER_YELLOW)
		State.GetSkinNum() = Color_RGBAToHex (NSColor::ParisDaisy, NSColor::Gorse, NSColor::Lemon, NSColor::Gold);
	else if (SpawnFlags & LASER_ORANGE)
		State.GetSkinNum() = Color_RGBAToHex (NSColor::HarvestGold, NSColor::RobRoy, NSColor::TulipTree, NSColor::FireBush);

	if (!Enemy)
	{
		if (!Target.IsNullOrEmpty())
		{
			IBaseEntity *Entity = CC_Find<IMapEntity, EF_MAP, EntityMemberOffset(IMapEntity,TargetName)> (NULL, Target.CString());
			if (!Entity)
				MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "\"%s\" is a bad target\n", Target.CString());
			Enemy = Entity;
		}
		else
			SetMoveDir (MoveDir);
	}

	Usable = true;
	StartLaser = false;

	if (!Damage)
		Damage = 1;

	GetMins().Set (-8);
	GetMaxs().Set (8);
	Link ();

	if (SpawnFlags & LASER_START_ON)
		On ();
	else
		Off ();
};

void CTargetLaser::Spawn ()
{
	Usable = false;

	// let everything else get spawned before we start firing
	NextThink = Level.Frame + 10;
};

ENTITYFIELDS_BEGIN(CTargetLaser)
{
	CEntityField ("dmg", EntityMemberOffset(CTargetLaser,Damage), FT_INT | FT_SAVABLE),

	CEntityField ("MoveDir", EntityMemberOffset(CTargetLaser,MoveDir), FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("StartLaser", EntityMemberOffset(CTargetLaser,StartLaser), FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetLaser)

bool			CTargetLaser::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetLaser> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void			CTargetLaser::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetLaser> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
	IThinkableEntity::SaveFields (File);
}

void			CTargetLaser::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetLaser> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_laser", CTargetLaser);

/*QUAKED target_help (1 0 1) (-16 -16 -24) (16 16 24) help1
When fired, the "message" key becomes the current personal computer string, and the message light will be set on all clients status bars.
*/

/**
\enum	

\brief	Values that represent spawnflags pertaining to CTargetHelp. 
**/
enum
{
	HELP_FIRST_MESSAGE	= BIT(0)
};

class CTargetHelp : public IMapEntity, public IUsableEntity
{
public:
	CTargetHelp () :
	  IBaseEntity (),
	  IMapEntity (),
	  IUsableEntity ()
	{
	};

	CTargetHelp (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CTargetHelp)

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
	};

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		Game.HelpMessages[(SpawnFlags & HELP_FIRST_MESSAGE) ? 0 : 1] = Message;
		Game.HelpChanged++;
	};

	void Spawn ()
	{
		if (Game.GameMode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		if (Message.IsNullOrEmpty())
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No message\n");
			Free ();
			return;
		}
	};
};

LINK_CLASSNAME_TO_CLASS ("target_help", CTargetHelp);

//==========================================================

/*QUAKED target_earthquake (1 0 0) (-8 -8 -8) (8 8 8)
When triggered, this initiates a level-wide earthquake.
All players and monsters are affected.
"speed"		severity of the quake (default:200)
"count"		duration of the quake (default:5)
*/

#if ROGUE_FEATURES
/**
\enum	

\brief	Values that represent spawnflags pertaining to CTargetEarthquake. 
**/
enum
{
	EARTHQUAKE_SILENT = BIT(0)
};
#endif

void CEarthQuakeShakePlayers::Callback (CPlayerEntity *Player)
{
	if (!Player->GroundEntity)
		return;

	Player->GroundEntity = nullentity;
	Player->Velocity.X += crand()* 150;
	Player->Velocity.Y += crand()* 150;
	Player->Velocity.Z = Speed * (100.0 / Player->Mass);
}

class CTargetEarthquake : public IMapEntity, public IThinkableEntity, public IUsableEntity
{
public:
	FrameNumber		LastShakeTime;
	FrameNumber		TimeStamp;
	float				Speed;
	FrameNumber		Duration;

	CTargetEarthquake () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity (),
	  IUsableEntity (),
	  LastShakeTime (0),
	  Speed (0)
	{
	};

	CTargetEarthquake (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index),
	  IUsableEntity (Index),
	  LastShakeTime (0),
	  Speed (0)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetEarthquake)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Think ()
	{
		if (
#if ROGUE_FEATURES
			!(SpawnFlags & EARTHQUAKE_SILENT) && 
#endif
			LastShakeTime < Level.Frame)
		{
			PlayPositionedSound (State.GetOrigin(), CHAN_AUTO, NoiseIndex, 255, ATTN_NONE);
			LastShakeTime = Level.Frame + 5;
		}

		CEarthQuakeShakePlayers(Speed).Query();

		if (Level.Frame < TimeStamp)
			NextThink = Level.Frame + ServerFramesPerSecond;
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		TimeStamp = Level.Frame + Duration;
		NextThink = Level.Frame + ServerFramesPerSecond;
		LastShakeTime = 0;
	};

	void Spawn ()
	{
		if (TargetName.IsNullOrEmpty())
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No targetname\n");

		if (!Duration)
			Duration = 5;

		if (!Speed)
			Speed = 200;

		GetSvFlags() |= SVF_NOCLIENT;

#if ROGUE_FEATURES
		if (!(SpawnFlags & EARTHQUAKE_SILENT))
#endif
			NoiseIndex = SoundIndex ("world/quake.wav");
	};
};

ENTITYFIELDS_BEGIN(CTargetEarthquake)
{
	// I found this field in biggun.bsp.
	// Supporting it.
	CEntityField ("duration", EntityMemberOffset(CTargetEarthquake,Duration), FT_FRAMENUMBER),

	CEntityField ("speed", EntityMemberOffset(CTargetEarthquake,Speed), FT_FLOAT | FT_SAVABLE),
	CEntityField ("count", EntityMemberOffset(CTargetEarthquake,Duration), FT_FRAMENUMBER | FT_SAVABLE),

	CEntityField ("LastShakeTime", EntityMemberOffset(CTargetEarthquake,LastShakeTime), FT_FRAMENUMBER | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("TimeStamp", EntityMemberOffset(CTargetEarthquake,TimeStamp), FT_FRAMENUMBER | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetEarthquake)

bool			CTargetEarthquake::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetEarthquake> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void			CTargetEarthquake::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetEarthquake> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
	IThinkableEntity::SaveFields (File);
}

void			CTargetEarthquake::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetEarthquake> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_earthquake", CTargetEarthquake);

