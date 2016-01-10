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
// cc_rogue_target_entities.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES

#include "Entities/BrushModels.h"
#include "Entities/FuncEntities.h"
#include "Utility/TemporaryEntities.h"

/*QUAKED target_steam (1 0 0) (-8 -8 -8) (8 8 8)
Creates a steam effect (particles w/ velocity in a line).

  speed = velocity of particles (default 50)
  count = number of particles (default 32)
  sounds = color of particles (default 8 for steam)
     the color range is from this color to this color + 6
  wait = seconds to run before stopping (overrides default
     value derived from func_timer)

  best way to use this is to tie it to a func_timer that "pokes"
  it every second (or however long you set the wait time, above)

  note that the width of the base is proportional to the speed
  good colors to use:
  6-9 - varying whites (darker to brighter)
  224 - sparks
  176 - blue water
  80  - brown water
  208 - slime
  232 - blood
*/
class CTargetSteam : public IMapEntity, public IThinkableEntity, public IUsableEntity
{
public:
	sint32				Speed, Count, Color, Wait;
	vec3f				MoveDir;

	CTargetSteam () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity (),
	  IUsableEntity ()
	{
	};

	CTargetSteam (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index),
	  IUsableEntity (Index)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetSteam)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Think ()
	{
		Usable = true;

		if (!Target.IsNullOrEmpty())
		{
			IBaseEntity *ent = CC_Find<IMapEntity, EF_MAP, EntityMemberOffset(IMapEntity,TargetName)> (NULL, Target.CString());
			if (!ent)
				MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "\"%s\" is a bad target\n", Target.CString());
			Enemy = ent;
		}
		else
			SetMoveDir (MoveDir);

		if (!Count)
			Count = 32;
		if (!Speed)
			Speed = 75;
		if (!Color)
			Color = 8;
		if (Wait)
			Wait *= 1000;  // we want it in milliseconds, not seconds

		// paranoia is good
		Color &= 0xff;
		Count &= 0xff;

		GetSvFlags() = SVF_NOCLIENT;

		Link ();
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		static int nextid = 0;

		if (nextid > 20000)
			nextid = 0;

		nextid++;
	
		// automagically set wait from func_timer unless they set it already, or
		// default to 1000 if not called by a func_timer (eek!)
		if (!Wait)
		{
			if (Other && Other->ClassName == "func_timer")
				Wait = entity_cast<CFuncTimer>(Other)->Wait * 1000;
			else
				Wait = 1000;
		}

		if (Enemy)
			MoveDir = ((Enemy->GetAbsMin().MultiplyAngles (0.5f, Enemy->GetSize())) - State.GetOrigin()).GetNormalized();

		vec3f point = State.GetOrigin().MultiplyAngles (Speed * 0.5f, MoveDir);

CC_DISABLE_DEPRECATION
		if (Wait > 100)
			CSteam(State.GetOrigin(), MoveDir, Count, Color & 0xFF, Speed, nextid, Wait).Send();
		else
			CSteam(State.GetOrigin(), MoveDir, Count, Color & 0xFF, Speed, -1).Send();
CC_ENABLE_DEPRECATION
	};

	void Spawn ()
	{
		Usable = false;

		if (!Target.IsNullOrEmpty())
			NextThink = Level.Frame + 10;
		else
			Think ();
	};
};

ENTITYFIELDS_BEGIN(CTargetSteam)
{
	// I found this field in biggun.bsp.
	// Supporting it.
	CEntityField ("speed", EntityMemberOffset(CTargetSteam,Speed), FT_INT | FT_SAVABLE),
	CEntityField ("count", EntityMemberOffset(CTargetSteam,Count), FT_INT | FT_SAVABLE),
	CEntityField ("sounds", EntityMemberOffset(CTargetSteam,Color), FT_INT | FT_SAVABLE),
	CEntityField ("wait", EntityMemberOffset(CTargetSteam,Wait), FT_INT | FT_SAVABLE),

	CEntityField ("MoveDir", EntityMemberOffset(CTargetSteam,MoveDir), FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetSteam)

bool			CTargetSteam::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetSteam> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void			CTargetSteam::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetSteam> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
	IThinkableEntity::SaveFields (File);
}

void			CTargetSteam::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetSteam> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_steam", CTargetSteam);


/*QUAKED target_anger (1 0 0) (-8 -8 -8) (8 8 8)
This trigger will cause an entity to be angry at another entity when a player touches it. Target the
entity you want to anger, and killtarget the entity you want it to be angry at.

target - entity to piss off
killtarget - entity to be pissed off at
*/
class CTargetAnger : public IMapEntity, public IUsableEntity
{
public:
	CTargetAnger () :
	  IBaseEntity (),
	  IMapEntity (),
	  IUsableEntity ()
	{
	};

	CTargetAnger (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index)
	{
	};

	ENTITYFIELDS_SAVABLE(CTargetAnger)
	bool		ParseField (const char *Key, const char *Value)
	{
		return (IMapEntity::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value));
	}

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		IMapEntity *AngryMonster = entity_cast<IMapEntity>(CC_Find<IMapEntity, EF_MAP, EntityMemberOffset(IMapEntity, TargetName)>  (NULL, KillTarget.CString()));

		if (AngryMonster && !Target.IsNullOrEmpty())
		{
			// Make whatever a "good guy" so the monster will try to kill it!
			if (AngryMonster->EntityFlags & EF_MONSTER)
				entity_cast<CMonsterEntity>(AngryMonster)->Monster->AIFlags |= AI_GOOD_GUY;
			AngryMonster->GetSvFlags() |= SVF_MONSTER;
			if (AngryMonster->EntityFlags & EF_HURTABLE)
				entity_cast<IHurtableEntity>(AngryMonster)->Health = 300;

			IBaseEntity *t = NULL;
			while ((t = CC_Find<IMapEntity, EF_MAP, EntityMemberOffset(IMapEntity,TargetName)> (t, Target.CString())) != NULL)
			{
				if (t == this)
					MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Entity used itself.\n");
				else
				{
					CMonsterEntity *Monster = entity_cast<CMonsterEntity>(t);
					if (Monster->Usable)
					{
						if (Monster->Health < 0)
							return;

						Monster->Enemy = AngryMonster;
						Monster->Monster->AIFlags |= AI_TARGET_ANGER;
						Monster->Monster->FoundTarget ();
					}
				}
			}
		}
	};

	void Spawn ()
	{
		if (Target.IsNullOrEmpty())
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No target\n");
			Free ();
			return;
		}
		if (KillTarget.IsNullOrEmpty())
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No killtarget\n");
			Free ();
			return;
		}

		Usable = true;
		GetSvFlags() = SVF_NOCLIENT;
	};
};

void		CTargetAnger::SaveFields (CFile &File)
{
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
}

void		CTargetAnger::LoadFields (CFile &File)
{
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_anger", CTargetAnger);

/*QUAKED target_killplayers (1 0 0) (-8 -8 -8) (8 8 8)
When triggered, this will kill all the players on the map.
*/

void NailHurtableEntity (IBaseEntity *Killer, IHurtableEntity *Hurtable)
{
	Hurtable->TakeDamage (Killer, Killer, vec3fOrigin, Killer->State.GetOrigin(), vec3fOrigin, (Hurtable->EntityFlags & EF_PLAYER) ? 100000 : Hurtable->Health, 0, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
}

class CForEachPlayerKillCallback : public CForEachPlayerCallback
{
public:
	IBaseEntity *Killer;

	CForEachPlayerKillCallback (IBaseEntity *Killer) :
	  Killer (Killer)
	  {
	  };

	void Callback (CPlayerEntity *Player)
	{
		NailHurtableEntity (Killer, Player);
	}
};

class CForEachNailMonsterCallback : public CForEachEntityCallback
{
public:
	IBaseEntity *Killer;

	CForEachNailMonsterCallback (IBaseEntity *Killer) :
	  Killer (Killer)
	  {
	  };

	void Callback (IBaseEntity *Entity)
	{
		IHurtableEntity *Hurtable = entity_cast<IHurtableEntity>(Entity);		

		if (Hurtable->Health < 1)
			return;
		if (!Hurtable->CanTakeDamage)
			return;

		for (uint8 i = 1; i <= 8; ++i)
		{
			if (Game.Entities[i].Entity->GetInUse() && IsVisible(Game.Entities[i].Entity, Hurtable))
			{
				NailHurtableEntity (Killer, entity_cast<IHurtableEntity>(Entity));
				break;
			}
		}
	}
};

class CTargetKillPlayers : public IMapEntity, public IUsableEntity
{
public:
	CTargetKillPlayers () :
	  IBaseEntity (),
	  IMapEntity (),
	  IUsableEntity ()
	{
	};

	CTargetKillPlayers (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index)
	{
	};

	ENTITYFIELDS_SAVABLE(CTargetKillPlayers)
	bool		ParseField (const char *Key, const char *Value)
	{
		return (IMapEntity::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value));
	}

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		CForEachPlayerKillCallback(this).Query();
		CForEachNailMonsterCallback(this).Query(EF_HURTABLE | EF_MONSTER);
	};

	void Spawn ()
	{
		Usable = true;
		GetSvFlags() = SVF_NOCLIENT;
	};
};

void		CTargetKillPlayers::SaveFields (CFile &File)
{
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
}

void		CTargetKillPlayers::LoadFields (CFile &File)
{
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_killplayers", CTargetKillPlayers);

/*QUAKED target_blacklight (1 0 1) (-16 -16 -24) (16 16 24) 
Pulsing black light with sphere in the center
*/

class CTargetBlacklight : public IMapEntity, public IThinkableEntity
{
public:
	CTargetBlacklight () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity ()
	{
	};

	CTargetBlacklight (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index)
	{
	};

	ENTITYFIELDS_SAVABLE(CTargetBlacklight)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Think ()
	{
		State.GetAngles().Set (frand()*360, frand()*360, frand()*360);
		NextThink = Level.Frame + 1;
	};

	void Spawn ()
	{
		if (Game.GameMode & GAME_DEATHMATCH)
		{
			// auto-remove for deathmatch
			Free ();
			return;
		}

		GetMins() = GetMaxs() = vec3fOrigin;

		NextThink = Level.Frame + 1;
		State.GetModelIndex() = ModelIndex ("models/items/spawngro2/tris.md2");
		State.GetFrame() = 1;
		State.GetEffects() |= FX_BLACKVOID;
		Link ();
	};
};

void		CTargetBlacklight::SaveFields (CFile &File)
{
	IMapEntity::SaveFields (File);
	IThinkableEntity::SaveFields (File);
}

void		CTargetBlacklight::LoadFields (CFile &File)
{
	IMapEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_blacklight", CTargetBlacklight);

/*QUAKED target_orb (1 0 1) (-16 -16 -24) (16 16 24) 
Translucent pulsing orb with speckles
*/

class CTargetOrb : public IMapEntity, public IThinkableEntity
{
public:
	CTargetOrb () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity ()
	{
	};

	CTargetOrb (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index)
	{
	};

	ENTITYFIELDS_SAVABLE(CTargetOrb)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Think ()
	{
		State.GetAngles().Set (frand()*360, frand()*360, frand()*360);
		NextThink = Level.Frame + 1;
	};

	void Spawn ()
	{
		if (Game.GameMode & GAME_DEATHMATCH)
		{
			// auto-remove for deathmatch
			Free ();
			return;
		}

		GetMins() = GetMaxs() = vec3fOrigin;

		NextThink = Level.Frame + 1;
		State.GetModelIndex() = ModelIndex ("models/items/spawngro2/tris.md2");
		State.GetFrame() = 2;
		State.GetEffects() |= FX_SPHERETRANS;
		Link ();
	};
};

void		CTargetOrb::SaveFields (CFile &File)
{
	IMapEntity::SaveFields (File);
	IThinkableEntity::SaveFields (File);
}

void		CTargetOrb::LoadFields (CFile &File)
{
	IMapEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_orb", CTargetOrb);

#endif
