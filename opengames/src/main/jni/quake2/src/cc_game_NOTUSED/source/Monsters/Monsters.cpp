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
// cc_monsters.cpp
// Monsters
//

#include "Local.h"
#include "Entities/BrushModels.h"
#include "Utility/TemporaryEntities.h"
#include "Weapons/WeaponMain.h"

/*
===============================
MONSTER LIST
===============================
*/

typedef std::vector<CMonsterTableIndex*> TMonsterListType;
typedef std::multimap<size_t, size_t> THashedMonsterListType;

TMonsterListType &MonsterTable ()
{
	static TMonsterListType MonsterTableV;
	return MonsterTableV;
};

THashedMonsterListType &MonsterHashTable ()
{
	static THashedMonsterListType Table;
	return Table;
}

CMonsterTableIndex::CMonsterTableIndex (const char *Name, IMonster *(*FuncPtr) (uint32 MonsterID)) :
  Name(Name),
  FuncPtr(FuncPtr)
{
	MonsterTable().push_back (this);

	// Link it in the hash tree
	MonsterHashTable().insert (std::make_pair<size_t, size_t> (Com_HashGeneric (Name, MAX_CLASSNAME_CLASSES_HASH), MonsterTable().size()-1));
};

IMonster *CreateMonsterFromTable (uint32 MonsterID, const char *Name)
{
	uint32 hash = Com_HashGeneric(Name, MAX_CLASSNAME_CLASSES_HASH);

	for (THashedMonsterListType::iterator it = MonsterHashTable().equal_range(hash).first; it != MonsterHashTable().equal_range(hash).second; ++it)
	{
		CMonsterTableIndex *Table = MonsterTable().at((*it).second);
		if (Q_stricmp (Table->Name, Name) == 0)
			return Table->FuncPtr(MonsterID);
	}

	return NULL;
};

void LoadMonsterData (CMonsterEntity *Entity, const char *LoadedName, uint32 MonsterID, CFile &File)
{
	// Create base monster
	Entity->Monster = CreateMonsterFromTable (MonsterID, Entity->ClassName.CString());

	// Fill it
	Entity->Monster->LoadFields (File);

	// Check.
	// LoadFields will actually re-load the monster's ID, so here we need to make sure
	// they still are the same.
	CC_ASSERT_EXPR (!((Entity->Monster->MonsterID != MonsterID) || Q_stricmp(Entity->Monster->GetMonsterName(), LoadedName)), "Loaded monster differs in ID or Name\n");

	Entity->Monster->Entity = Entity;
}

void IMonster::SaveFields (CFile &File)
{
	File.Write<uint32> (MonsterID);
	File.Write<float> (IdealYaw);
	File.Write<float> (YawSpeed);
	File.Write<uint32> (AIFlags);
#if ROGUE_FEATURES
	File.Write<bool> (BlindFire);
	File.Write<float> (BaseHeight);
	File.Write<FrameNumber> (NextDuckTime);
	File.Write<FrameNumber> (DuckWaitTime);
	File.Write<FrameNumber> (BlindFireDelay);
	File.Write<sint32> ((LastPlayerEnemy) ? LastPlayerEnemy->State.GetNumber() : -1);
	File.Write<FrameNumber> (NextDuckTime);
	File.Write<vec3f> (BlindFireTarget);
	File.Write<sint32> ((BadMedic1) ? BadMedic1->State.GetNumber() : -1);
	File.Write<sint32> ((BadMedic2) ? BadMedic2->State.GetNumber() : -1);
	File.Write<sint32> ((Healer) ? Healer->State.GetNumber() : -1);
#endif
	File.Write<sint32> (NextFrame);
	File.Write<FrameNumber> (PauseTime);
	File.Write<FrameNumber> (AttackFinished);
	File.Write<FrameNumber> (SearchTime);
	File.Write<vec3f> (LastSighting);
	File.Write<vec3f> (SavedGoal);
	File.Write<sint32> (AttackState);
	File.Write<bool> (Lefty);
	File.Write<float> (IdleTime);
	File.Write<sint32> (LinkCount);
	File.Write<EPowerArmorType> (PowerArmorType);
	File.Write<sint32> (PowerArmorPower);
	File.Write<uint8> (PowerArmorTime);
	File.Write<uint8> (ExplodeCount);
	File.Write<bool> (EnemyInfront);
	File.Write<bool> (EnemyVis);
	File.Write<ERangeType> (EnemyRange);
	File.Write<float> (EnemyYaw);
	File.Write<CAnim*> (CurrentMove);
	File.Write<uint32> (MonsterFlags);
	File.Write<FrameNumber> (PainDebounceTime);

	File.Write <void (IMonster::*) ()> (Think);

#if ROGUE_FEATURES
	File.Write<uint8> (MonsterSlots);
	File.Write<uint8> (MonsterUsed);
	File.Write<sint32> ((Commander) ? Commander->State.GetNumber() : -1);
	File.Write<FrameNumber> (QuadFramenum);
	File.Write<FrameNumber> (InvincibleFramenum);
	File.Write<FrameNumber> (DoubleFramenum);
	SaveBadArea (File, BadArea);
#endif

	SaveMonsterFields (File);
};

void IMonster::LoadFields (CFile &File)
{
	File.Read ((void*)(&MonsterID), sizeof(uint32));
	IdealYaw = File.Read<float> ();
	YawSpeed = File.Read<float> ();
	AIFlags = File.Read<uint32> ();
#if ROGUE_FEATURES
	BlindFire = File.Read<bool> ();
	BaseHeight = File.Read<float> ();
	NextDuckTime = File.Read<FrameNumber> ();
	DuckWaitTime = File.Read<FrameNumber> ();
	BlindFireDelay = File.Read<FrameNumber> ();
	sint32 Index = File.Read<sint32> ();
	if (Index != -1)
		LastPlayerEnemy = entity_cast<CPlayerEntity>(Game.Entities[Index].Entity);

	NextDuckTime = File.Read<FrameNumber> ();
	BlindFireTarget = File.Read<vec3f> ();
	Index = File.Read<sint32> ();
	if (Index != -1)
		BadMedic1 = entity_cast<CMonsterEntity>(Game.Entities[Index].Entity);

	Index = File.Read<sint32> ();
	if (Index != -1)
		BadMedic2 = entity_cast<CMonsterEntity>(Game.Entities[Index].Entity);

	Index = File.Read<sint32> ();
	if (Index != -1)
		Healer = entity_cast<CMonsterEntity>(Game.Entities[Index].Entity);
#endif
	NextFrame = File.Read<sint32> ();
	PauseTime = File.Read<FrameNumber> ();
	AttackFinished = File.Read<FrameNumber> ();
	SearchTime = File.Read<FrameNumber> ();
	LastSighting = File.Read<vec3f> ();
	SavedGoal = File.Read<vec3f> ();
	AttackState = File.Read<sint32> ();
	Lefty = File.Read<bool> ();
	IdleTime = File.Read<float> ();
	LinkCount = File.Read<sint32> ();
	PowerArmorType = File.Read<EPowerArmorType> ();
	PowerArmorPower = File.Read<sint32> ();
	PowerArmorTime = File.Read<uint8> ();
	ExplodeCount = File.Read<uint8> ();
	EnemyInfront = File.Read<bool> ();
	EnemyVis = File.Read<bool> ();
	EnemyRange = File.Read<ERangeType> ();
	EnemyYaw = File.Read<float> ();
	CurrentMove = File.Read<CAnim*> ();
	MonsterFlags = File.Read<uint32> ();
	PainDebounceTime = File.Read<FrameNumber> ();

	Think = File.Read <void (IMonster::*) ()> ();

#if ROGUE_FEATURES
	MonsterSlots = File.Read<uint8> ();
	MonsterUsed = File.Read<uint8> ();

	Index = File.Read<sint32> ();
	if (Index != -1)
		Commander = entity_cast<CMonsterEntity>(Game.Entities[Index].Entity);

	QuadFramenum = File.Read<FrameNumber> ();
	InvincibleFramenum = File.Read<FrameNumber> ();
	DoubleFramenum = File.Read<FrameNumber> ();
	BadArea = LoadBadArea (File);
#endif

	LoadMonsterFields (File);
};

/*
=================
AI_SetSightClient

Called once each frame to set Level.SightClient to the
player to be checked for in findtarget.

If all clients are either dead or in notarget, SightClient
will be null.

In coop games, SightClient will cycle between the clients.
=================
*/
void AI_SetSightClient ()
{
	sint32		start, check;

	if (Level.SightClient == NULL)
		start = 1;
	else
		start = Level.SightClient->State.GetNumber();

	check = start;
	while (1)
	{
		check++;
		if (check > Game.MaxClients)
			check = 1;
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[check].Entity);
		if (Player->GetInUse()
			&& (Player->Health > 0)
			&& !(Player->Flags & FL_NOTARGET) 
			&& (Player->Client.Persistent.State >= SVCS_SPAWNED))
		{
			Level.SightClient = Player;
			return;		// got one
		}
		if (check == start)
		{
			Level.SightClient = NULL;
			return;		// nobody to see
		}
	}
}

CMonsterEntity::CMonsterEntity () :
IBaseEntity(),
IMapEntity(),
ITossProjectile(),
IPushPhysics(),
IHurtableEntity(),
IThinkableEntity(),
IStepPhysics(),
ITouchableEntity(),
IUsableEntity(),
UseState(MONSTERENTITY_THINK_NONE)
{
	EntityFlags |= EF_MONSTER;
	PhysicsType = PHYSICS_STEP;
};

CMonsterEntity::CMonsterEntity (sint32 Index) :
IBaseEntity(Index),
IMapEntity(Index),
ITossProjectile(Index),
IPushPhysics(Index),
IHurtableEntity(Index),
IThinkableEntity(Index),
IStepPhysics(Index),
ITouchableEntity(Index),
IUsableEntity(Index),
UseState(MONSTERENTITY_THINK_NONE)
{
	EntityFlags |= EF_MONSTER;
	PhysicsType = PHYSICS_STEP;
};

bool CMonsterEntity::CheckValidity ()
{
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		Free ();
		return false;
	}
	return IMapEntity::CheckValidity ();
}

void CMonsterEntity::Spawn ()
{
	Touchable = true;
	PhysicsType = PHYSICS_STEP;
};

/**
\fn	void CMonsterEntity::TouchTriggers ()

\brief	Cause all triggers that this entity is touching to activate.
**/
void CMonsterEntity::TouchTriggers ()
{
	// dead things don't activate triggers!
	if (CanTakeDamage && (Health <= 0))
		return;

	IBaseEntity::TouchTriggers();
}

ENTITYFIELDS_BEGIN(CMonsterEntity)
{
	CEntityField ("deathtarget",		EntityMemberOffset(CMonsterEntity,DeathTarget),			FT_STRING | FT_SAVABLE),
	CEntityField ("combattarget",		EntityMemberOffset(CMonsterEntity,CombatTarget),		FT_STRING | FT_SAVABLE),
	CEntityField ("item",				EntityMemberOffset(CMonsterEntity,Item),				FT_ITEM | FT_SAVABLE),

	CEntityField ("IsHead",				EntityMemberOffset(CMonsterEntity,IsHead),				FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("UseState",			EntityMemberOffset(CMonsterEntity,UseState),			FT_BYTE | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("AirFinished",		EntityMemberOffset(CMonsterEntity,AirFinished),			FT_FRAMENUMBER | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("DamageDebounceTime",	EntityMemberOffset(CMonsterEntity,DamageDebounceTime),	FT_FRAMENUMBER | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("BonusDamageTime",	EntityMemberOffset(CMonsterEntity,BonusDamageTime),		FT_FRAMENUMBER | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("ShowHostile",		EntityMemberOffset(CMonsterEntity,ShowHostile),			FT_FRAMENUMBER | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CMonsterEntity)

bool			CMonsterEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CMonsterEntity> (this, Key, Value))
		return true;

	return (IUsableEntity::ParseField (Key, Value) || IHurtableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void			CMonsterEntity::SaveFields (CFile &File)
{
	// Write the monster's name first - this is used for checking later
	if (CC_ASSERT_EXPR (!(!Monster || strlen(Monster->GetMonsterName()) == 0), "Monster with no monster or name!\n"))
		return;

	File.WriteString (Monster->GetMonsterName());

	// Write ID
	File.Write<uint32> (Monster->MonsterID);

	OldEnemy.Write(File);
	GoalEntity.Write(File);
	MoveTarget.Write(File);

	SaveEntityFields <CMonsterEntity> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
	IHurtableEntity::SaveFields (File);
	ITouchableEntity::SaveFields (File);
	IThinkableEntity::SaveFields (File);
	ITossProjectile::SaveFields (File);

	// Write the monster's info last
	Monster->SaveFields (File);
}

void			CMonsterEntity::LoadFields (CFile &File)
{
	// Load in the monster name
	char *tempBuffer = File.ReadString (TAG_GENERIC);
	uint32 tempId = File.Read<uint32> ();

	OldEnemy = entity_ptr<IBaseEntity>::Read(File);
	GoalEntity = entity_ptr<IBaseEntity>::Read(File);
	MoveTarget = entity_ptr<IBaseEntity>::Read(File);

	// Let the rest of the entity load first
	LoadEntityFields <CMonsterEntity> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
	IHurtableEntity::LoadFields (File);
	ITouchableEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);
	ITossProjectile::LoadFields (File);

	// Now load the monster info
	LoadMonsterData (this, tempBuffer, tempId, File);
	QDelete[] tempBuffer;
}

IMPLEMENT_SAVE_STRUCTURE (CMonsterEntity, CMonsterEntity)

void CMonsterEntity::Think ()
{
	if (IsHead)
		Free ();
	else if (Monster->Think)
	{
		void	(IMonster::*TheThink) () = Monster->Think;
		(Monster->*TheThink) ();
	}
}

bool CMonsterEntity::Blocked (float Dist)
{
	return Monster->Blocked (Dist);
}

void CMonsterEntity::Die(IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	Monster->Die (Inflictor, Attacker, Damage, Point);
}

void CMonsterEntity::Pain (IBaseEntity *Other, sint32 Damage)
{
	Monster->Pain (Other, Damage);
}

/**
\fn	sint32 CMonsterEntity::CheckPowerArmor (vec3f &Point, vec3f &Normal, sint32 Damage,
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
sint32 CMonsterEntity::CheckPowerArmor (vec3f &Point, vec3f &Normal, sint32 Damage, EDamageFlags DamageFlags)
{
	if (!Damage)
		return 0;

	if (DamageFlags & DAMAGE_NO_ARMOR)
		return 0;

	static const sint32	index = NItems::Cells->GetIndex();

	sint32			DamagePerCell;
	bool			ScreenSparks = false;

	EPowerArmorType Type = Monster->PowerArmorType;
	sint32 Power = Monster->PowerArmorPower;

	if (!Power)
		return 0;

	switch (Type)
	{
	case POWER_ARMOR_NONE:
	default:
		return 0;
	case POWER_ARMOR_SCREEN:
		{
			vec3f		vec;

			// only works if damage point is in front
			anglef angles = State.GetAngles().ToVectors();
			vec = Point - State.GetOrigin();
			vec.Normalize ();
			if ((vec | angles.Forward) <= 0.3)
				return 0;

			DamagePerCell = 1;
			ScreenSparks = true;
			Damage /= 3;
		}
		break;
	case POWER_ARMOR_SHIELD:
		DamagePerCell = 2;
		Damage = (2 * Damage) / 3;
		break;
	};

	sint32 Saved = Power * DamagePerCell;
	if (!Saved)
		return 0;
	if (Saved > Damage)
		Saved = Damage;

	CShieldSparks(Point, Normal, ScreenSparks).Send();

	sint32 PowerUsed = Saved / DamagePerCell;
	if (!PowerUsed)
		PowerUsed = 1;

	Monster->PowerArmorPower -= PowerUsed;
	Monster->PowerArmorTime = 2;

	return Saved;
}

void CMonsterEntity::TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker,
								vec3f Dir, vec3f Point, vec3f Normal, sint32 Damage,
								sint32 Knockback, EDamageFlags DamageFlags, EMeansOfDeath MeansOfDeath)
{
	Monster->TakeDamage (Inflictor, Attacker, Dir, Point, Normal, Damage, Knockback, DamageFlags, MeansOfDeath);
}

#include "Monsters/Medic.h"

#if ROGUE_FEATURES
#include "Rogue/RogueCarrier.h"
#include "Rogue/RogueMedicCommander.h"
#include "Rogue/RogueWidowStand.h"
#include "Rogue/RogueBlackWidow.h"
#endif

void CMonsterEntity::Killed (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	if (Health < -999)
		Health = -999;

	Enemy = Attacker;

#if ROGUE_FEATURES
	if (Monster->AIFlags & AI_MEDIC)
	{
		if (Enemy && (Enemy->EntityFlags & EF_MONSTER))  // god, I hope so
			entity_cast<CMonsterEntity>(*Enemy)->Monster->CleanupHealTarget ();

		// clean up self
		Monster->AIFlags &= ~AI_MEDIC;
		Enemy = Attacker;
	}
#endif

#if ROGUE_FEATURES
	if (!IsDead)
	{
		if (Monster->AIFlags & AI_SPAWNED_CARRIER)
		{
			if (Monster->Commander && Monster->Commander->GetInUse() && 
				Monster->MonsterID == CCarrier::ID)
				Monster->Commander->Monster->MonsterSlots++;
		}

		if (Monster->AIFlags & AI_SPAWNED_MEDIC_C)
		{
			if (Monster->Commander)
			{
				if (Monster->Commander->GetInUse() && Monster->MonsterID == CMedicCommander::ID)
					Monster->Commander->Monster->MonsterSlots++;
			}
		}

		if (Monster->AIFlags & AI_SPAWNED_WIDOW)
		{
			// need to check this because we can have variable numbers of coop players
			if (Monster->Commander && Monster->Commander->GetInUse() && 
				(Monster->MonsterID == CWidowStand::ID || Monster->MonsterID == CBlackWidow::ID))
			{
				if (Monster->Commander->Monster->MonsterUsed > 0)
					Monster->Commander->Monster->MonsterUsed--;
			}
		}

		if (!(Monster->AIFlags & AI_GOOD_GUY) && !(Monster->AIFlags & AI_DO_NOT_COUNT))
		{
			Level.Monsters.Killed++;
			if ((Game.GameMode & GAME_COOPERATIVE) && (Attacker->EntityFlags & EF_PLAYER))
				(entity_cast<CPlayerEntity>(Attacker))->Client.Respawn.Score++;
		}
	}
#else
	if (!IsDead)
	{
		if (!(Monster->AIFlags & AI_GOOD_GUY))
		{
			Level.Monsters.Killed++;
			if ((Game.GameMode & GAME_COOPERATIVE) && Attacker && (Attacker->EntityFlags & EF_PLAYER))
				(entity_cast<CPlayerEntity>(Attacker))->Client.Respawn.Score++;
			// medics won't heal monsters that they kill themselves

#if !ROGUE_FEATURES
			if (Attacker && (Attacker->EntityFlags & EF_MONSTER) && entity_cast<CMonsterEntity>(Attacker)->Monster->MonsterID == CMedic::ID)
				SetOwner(Attacker);
#endif
		}
	}
#endif

	if (!IsDead)
	{
		Touchable = false;
		Monster->MonsterDeathUse();
	}

	if (CanTakeDamage)
		Die (Inflictor, Attacker, Damage, Point);
}

void CMonsterEntity::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	Monster->Touch (Other, plane, surf);
}

void CMonsterEntity::ThrowHead (MediaIndex gibIndex, sint32 Damage, sint32 type, uint32 effects)
{
	float	vscale;

	IsHead = true;
	State.GetSkinNum() = 0;
	State.GetFrame() = 0;

	GetMins().Clear ();
	GetMaxs().Clear ();

	State.GetModelIndex(2) = 0;
	State.GetModelIndex() = gibIndex;
	GetSolid() = SOLID_NOT;
	State.GetEffects() |= effects;
	State.GetEffects() &= ~FX_FLIES;
	State.GetSound() = 0;
	AffectedByKnockback = false;
	GetSvFlags() &= ~SVF_MONSTER;
	CanTakeDamage = true;

	if (type == GIB_ORGANIC)
	{
		PhysicsType = PHYSICS_TOSS;
		vscale = 0.5;
	}
	else
	{
		PhysicsType = PHYSICS_BOUNCE;
		vscale = 1.0;
	}
	
	Velocity = Velocity.MultiplyAngles (vscale, VelocityForDamage (Damage));
	Velocity.X = Clamp<float> (Velocity.X, -300, 300);
	Velocity.Y = Clamp<float> (Velocity.Y, -300, 300);
	Velocity.Z = Clamp<float> (Velocity.Z, 200, 500); // always some upwards

	AngularVelocity.Y = crand()*600;

	NextThink = Level.Frame + 100 + frand()*100;

	Link();
}

bool CMonsterEntity::Run ()
{
	switch (PhysicsType)
	{
	case PHYSICS_TOSS:
		return ITossProjectile::Run();
	case PHYSICS_BOUNCE:
		BackOff = 1.5f;
		return IBounceProjectile::Run ();
	case PHYSICS_PUSH:
		return IPushPhysics::Run ();
	case PHYSICS_NONE:
		return false;
	default:
		return IStepPhysics::Run ();
	};
}

void CMonsterEntity::DamageEffect (vec3f &Dir, vec3f &Point, vec3f &Normal, sint32 &Damage, EDamageFlags &DamageFlags, EMeansOfDeath &MeansOfDeath)
{
	Monster->DamageEffect (Dir, Point, Normal, Damage, DamageFlags, MeansOfDeath);
}

void IMonster::DamageEffect (vec3f &Dir, vec3f &Point, vec3f &Normal, sint32 &Damage, EDamageFlags &DamageFlags, EMeansOfDeath &MeansOfDeath)
{
#if ROGUE_FEATURES
	if (MeansOfDeath == MOD_CHAINFIST)
		CBlood(Point, Normal, BT_MORE_BLOOD).Send();
	else
#endif
	CBlood(Point, Normal).Send();
}

void IMonster::ChangeYaw ()
{
	float current = AngleModf (Entity->State.GetAngles().Y);

	if (current == IdealYaw)
		return;

	float move = IdealYaw - current;
	if (IdealYaw > current)
	{
		if (move >= 180)
			move = move - 360;
	}
	else
	{
		if (move <= -180)
			move = move + 360;
	}
	if (move > 0)
	{
		if (move > YawSpeed)
			move = YawSpeed;
	}
	else
	{
		if (move < -YawSpeed)
			move = -YawSpeed;
	}
	
	Entity->State.GetAngles().Y = AngleModf(current+move);
}

bool IMonster::CheckBottom ()
{
	vec3f	mins, maxs, start, stop;
	CTrace	trace;
	sint32		x, y;
	float	mid, bottom;
	
	mins = Entity->State.GetOrigin() + Entity->GetMins();
	maxs = Entity->State.GetOrigin() + Entity->GetMaxs();

// if all of the points under the corners are solid world, don't bother
// with the tougher checks
// the corners must be within 16 of the midpoint
	bool gotOutEasy = false;

	start.Z = mins.Z - ((Entity->GravityVector.Z > 0) ? -1 : 1);

	for	(x=0 ; x<=1 && !gotOutEasy; x++)
	{
		for	(y=0 ; y<=1 && !gotOutEasy; y++)
		{
			start.X = x ? maxs.X : mins.X;
			start.Y = y ? maxs.Y : mins.Y;

			if (PointContents (start) != CONTENTS_SOLID)
				gotOutEasy = true;
		}

	}

	if (gotOutEasy)
		return true;		// we got out easy

//
// check it for real...
//
	start.Z = mins.Z;
	
// the midpoint must be within 16 of the bottom
	start.X = stop.X = (mins.X + maxs.X)*0.5;
	start.Y = stop.Y = (mins.Y + maxs.Y)*0.5;

	if (Entity->GravityVector.Z < 0)
	{
		start.Z = mins.Z;
		stop.Z = start.Z - (STEPSIZE * 2);
	}
	else
	{
		start.Z = maxs.Z;
		stop.Z = start.Z + (STEPSIZE * 2);
	}

	trace (start, stop, Entity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.Fraction == 1.0)
		return false;
	mid = bottom = trace.EndPosition[2];
	
// the corners must be within 16 of the midpoint	
	for	(x=0 ; x<=1 ; x++)
		for	(y=0 ; y<=1 ; y++)
		{
			start.X = stop.X = x ? maxs.X : mins.X;
			start.Y = stop.Y = y ? maxs.Y : mins.Y;
			
			trace (start, stop, Entity, CONTENTS_MASK_MONSTERSOLID);
			
			if (Entity->GravityVector.Z > 0)
			{
				if (trace.Fraction != 1.0 && trace.EndPosition.Z < bottom)
					bottom = trace.EndPosition.Z;
				if (trace.Fraction == 1.0 || trace.EndPosition.Z - mid > STEPSIZE)
					return false;
			}
			else
			{
				if (trace.Fraction != 1.0 && trace.EndPosition.Z > bottom)
					bottom = trace.EndPosition.Z;
				if (trace.Fraction == 1.0 || mid - trace.EndPosition.Z > STEPSIZE)
					return false;
			}
		}

	return true;
}

bool IMonster::WalkMove (float Yaw, float Dist)
{	
	if (!Entity->GroundEntity && !(AIFlags & (AI_FLY | AI_SWIM)))
		return false;

	Yaw = Yaw*M_PI*2 / 360;
	return MoveStep(vec3f (cosf(Yaw)*Dist, sinf(Yaw)*Dist, 0), true);
}

bool IMonster::CloseEnough (IBaseEntity *Goal, float Dist)
{
	if (Goal->GetAbsMin()[0] > (Entity->GetAbsMax()[0] + Dist) ||
		Goal->GetAbsMin()[1] > (Entity->GetAbsMax()[1] + Dist) ||
		Goal->GetAbsMin()[2] > (Entity->GetAbsMax()[2] + Dist))
		return false;
	if (Goal->GetAbsMax()[0] < (Entity->GetAbsMin()[0] - Dist) ||
		Goal->GetAbsMax()[1] < (Entity->GetAbsMin()[1] - Dist) ||
		Goal->GetAbsMax()[2] < (Entity->GetAbsMin()[2] - Dist))
		return false;
	return true;
}

#if ROGUE_FEATURES
#include "Rogue/RogueStalker.h"
#endif

void IMonster::WalkMonsterStartGo ()
{
	if (!(Entity->SpawnFlags & MONSTER_TRIGGER_SPAWN) && Level.Frame < 10)
	{
		DropToFloor ();

		if (Entity->GroundEntity)
		{
			if (!WalkMove (0, 0))
				MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "In solid\n");
		}
	}
	
	if (!YawSpeed)
		YawSpeed = 20;

#if ROGUE_FEATURES
	if (MonsterID == CStalker::ID)
		Entity->ViewHeight = 15;
	else
#endif
		Entity->ViewHeight = 25;

	MonsterStartGo ();

	if (Entity->SpawnFlags & MONSTER_TRIGGER_SPAWN)
		MonsterTriggeredStart ();
}

void IMonster::WalkMonsterStart ()
{
	Think = &IMonster::WalkMonsterStartGo;
	Entity->NextThink = Level.Frame + ServerFramesPerSecond;
	MonsterStart ();
}

void IMonster::SwimMonsterStartGo ()
{
	if (!YawSpeed)
		YawSpeed = 10;
	Entity->ViewHeight = 10;

	MonsterStartGo ();

	if (Entity->SpawnFlags & MONSTER_TRIGGER_SPAWN)
		MonsterTriggeredStart ();
}

void IMonster::SwimMonsterStart ()
{
	AIFlags |= AI_SWIM;
	Think = &IMonster::SwimMonsterStartGo;
	MonsterStart ();
}

void IMonster::FlyMonsterStartGo ()
{
	if (!WalkMove (0, 0))
		MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Entity in solid\n");

	if (!YawSpeed)
		YawSpeed = 10;
	Entity->ViewHeight = 25;

	MonsterStartGo ();

	if (Entity->SpawnFlags & MONSTER_TRIGGER_SPAWN)
		MonsterTriggeredStart ();
}

void IMonster::FlyMonsterStart ()
{
	AIFlags |= AI_FLY;
	Think = &IMonster::FlyMonsterStartGo;
	MonsterStart ();
}

void IMonster::MonsterStartGo ()
{
	if (Entity->Health <= 0)
		return;

	// check for target to combat_point and change to combattarget
	if (!Entity->Target.IsNullOrEmpty())
	{
		bool		notcombat = false, fixup = false;
		IMapEntity		*target = NULL;

		while ((target = CC_Find<IMapEntity, EF_MAP, EntityMemberOffset(IMapEntity,TargetName)> (target, Entity->Target.CString())) != NULL)
		{
			if (target->ClassName == "point_combat")
			{
				Entity->CombatTarget = Entity->Target;
				fixup = true;
			}
			else
				notcombat = true;
		}
		if (notcombat && !Entity->CombatTarget.IsNullOrEmpty())
			MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Target with mixed types\n");
		if (fixup)
			Entity->Target.Clear();
	}

	// validate combattarget
	if (!Entity->CombatTarget.IsNullOrEmpty())
	{
		IMapEntity		*target = NULL;
		while ((target = CC_Find<IMapEntity, EF_MAP, EntityMemberOffset(IMapEntity,TargetName)> (target, Entity->CombatTarget.CString())) != NULL)
		{
			if (target->ClassName != "point_combat")
				MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Has a bad combattarget (\"%s\")\n", Entity->CombatTarget.CString());
		}
	}

	if (!Entity->Target.IsNullOrEmpty())
	{
		IBaseEntity *Target = CC_PickTarget(Entity->Target);

		if (Target)
			Entity->GoalEntity = Entity->MoveTarget = Target;
		if (!Entity->MoveTarget)
		{
			MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Can't find target\n");
			Entity->Target.Clear();
			PauseTime = 100000000;
			Stand ();
		}
		else if (Entity->MoveTarget->ClassName == "path_corner")
		{
			IdealYaw = Entity->State.GetAngles().Y = (Entity->GoalEntity->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();
			Walk ();
			Entity->Target.Clear();
		}
		else
		{
			Entity->GoalEntity = Entity->MoveTarget = nullentity;
			PauseTime = 100000000;
			Stand ();
		}
	}
	else
	{
		PauseTime = 100000000;
		Stand ();
	}

	// are we in debug mode?
	if (CvarList[CV_MAP_DEBUG].Integer())
	{
		Think = NULL; // Don't think
		
		// Make us a deadmonster solid
		Entity->GetSolid() = SOLID_BBOX;
		Entity->GetSvFlags() = (SVF_MONSTER|SVF_DEADMONSTER);

		Entity->Link ();
	}
	else
	{
		Think = &IMonster::MonsterThink;
		Entity->NextThink = Level.Frame + ServerFramesPerSecond;
	}
}

void IMonster::MonsterStart ()
{
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		Entity->Free ();
		return;
	}

	if ((Entity->SpawnFlags & MONSTER_SIGHT) && !(AIFlags & AI_GOOD_GUY))
	{
		Entity->SpawnFlags &= ~MONSTER_SIGHT;
		Entity->SpawnFlags |= MONSTER_AMBUSH;
	}

	if (!(AIFlags & AI_GOOD_GUY) && !(Entity->SpawnFlags & MONSTER_DONT_COUNT))
		Level.Monsters.Total++;

	Entity->NextThink = Level.Frame + ServerFramesPerSecond;
	Entity->GetSvFlags() |= SVF_MONSTER;
	Entity->State.GetRenderEffects() |= RF_FRAMELERP;
	Entity->CanTakeDamage = true;
	Entity->AirFinished = Level.Frame + 120;
	Entity->UseState = MONSTERENTITY_THINK_USE;
	Entity->MaxHealth = Entity->Health;
	Entity->GetClipmask() = CONTENTS_MASK_MONSTERSOLID;

	Entity->IsDead = false;
	Entity->GetSvFlags() &= ~SVF_DEADMONSTER;

	Entity->State.GetOldOrigin() = Entity->State.GetOrigin();

	// randomize what frame they start on
	if (CurrentMove)
		Entity->State.GetFrame() = (CurrentMove->FirstFrame + (irandom(CurrentMove->LastFrame - CurrentMove->FirstFrame + 1)));

#if ROGUE_FEATURES
	BaseHeight = Entity->GetMaxs().Z;
#endif

	Entity->NextThink = Level.Frame + ServerFramesPerSecond;
}

void IMonster::MonsterTriggeredStart ()
{
	Entity->GetSolid() = SOLID_NOT;
	Entity->PhysicsDisabled = true;

	if (!CvarList[CV_MAP_DEBUG].Integer())
		Entity->GetSvFlags() |= SVF_NOCLIENT;
	else
		Entity->State.GetEffects() = FX_SPHERETRANS;
	Entity->NextThink = 0;
	Think = NULL;
	Entity->UseState = MONSTERENTITY_THINK_TRIGGEREDSPAWNUSE;
}

void IMonster::StationaryMonsterStartGo ()
{	
	if (!YawSpeed)
		YawSpeed = 20;

	MonsterStartGo ();

	if (Entity->SpawnFlags & 2)
		MonsterTriggeredStart ();
}

void IMonster::StationaryMonsterStart ()
{
	MonsterStart ();
	Think = &IMonster::StationaryMonsterStartGo;
}

void CMonsterEntity::Use (IBaseEntity *Other, IBaseEntity *Activator)
{
	switch (UseState)
	{
	case MONSTERENTITY_THINK_NONE:
		break;
	case MONSTERENTITY_THINK_CUSTOM:
		Monster->Use (Other, Activator);
		break;
	case MONSTERENTITY_THINK_USE:
		if (Enemy)
			return;
		if (Health <= 0)
			return;
		if (!Activator)
			return;
		if (Activator->Flags & FL_NOTARGET)
			return;
		if (!(Activator->EntityFlags & EF_PLAYER) && !(Monster->AIFlags & AI_GOOD_GUY))
			return;
#if ROGUE_FEATURES
		if (Activator->Flags & FL_DISGUISED)		// PGM
			return;									// PGM
#endif

	// delay reaction so if the monster is teleported, its sound is still heard
		Enemy = Activator;
		Monster->FoundTarget ();
		break;
	case MONSTERENTITY_THINK_TRIGGEREDSPAWNUSE:
		// we have a one frame delay here so we don't telefrag the guy who activated us
		Monster->Think = &IMonster::MonsterTriggeredSpawn;
		NextThink = Level.Frame + ServerFramesPerSecond;
		if (Activator && (Activator->EntityFlags & EF_PLAYER))
			Enemy = Activator;
		UseState = MONSTERENTITY_THINK_USE;
		break;
	};
}

void IMonster::MonsterTriggeredSpawn ()
{
	Entity->State.GetOrigin().Z += 1;
	Entity->KillBox ();

	Entity->GetSolid() = SOLID_BBOX;
	Entity->PhysicsDisabled = false;
	Entity->GetSvFlags() &= ~SVF_NOCLIENT;
	Entity->AirFinished = Level.Frame + 120;
	Entity->Link ();

	MonsterStartGo ();

	if (Entity->Enemy && !(Entity->SpawnFlags & MONSTER_AMBUSH) && !(Entity->Enemy->Flags & FL_NOTARGET))
#if ROGUE_FEATURES
	{
		if (!(Entity->Enemy->Flags & FL_DISGUISED))		// PGM
			FoundTarget ();
		else // PMM - just in case, make sure to clear the enemy so FindTarget doesn't get confused
			Entity->Enemy = nullentity;
	}
#else
		FoundTarget ();
#endif
	else
		Entity->Enemy = nullentity;
}

void IMonster::TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker,
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
	if (!Entity->CanTakeDamage)
		return;

	meansOfDeath = MeansOfDeath;
	Dir.Normalize ();

// bonus damage for surprising a monster
// Paril revision: Allow multiple pellet weapons to take advantage of this too!
	if (!(DamageFlags & DAMAGE_RADIUS) && (Entity->EntityFlags & EF_MONSTER) && Attacker && (Attacker->EntityFlags & EF_PLAYER))
	{
		if ((Entity->Health > 0) &&
			(!Entity->Enemy && (Entity->BonusDamageTime <= Level.Frame)) ||
			(Entity->Enemy && (Entity->BonusDamageTime == Level.Frame)))
		{
			Entity->BonusDamageTime = Level.Frame;
			Damage *= 2;
		}
	}

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
				entity_cast<CPlayerEntity>(Attacker)->Client.Persistent.Tech->DoAggressiveTech (Atk, Entity, false, Damage, KnockBack, DamageFlags, MeansOfDeath, false);
		}
	}

	if (!Entity->AffectedByKnockback)
		KnockBack = 0;

// figure momentum add
	if (KnockBack && !(DamageFlags & DAMAGE_NO_KNOCKBACK))
	{
		if (!(Entity->PhysicsType == PHYSICS_NONE ||
			Entity->PhysicsType == PHYSICS_BOUNCE ||
			Entity->PhysicsType == PHYSICS_PUSH ||
			Entity->PhysicsType == PHYSICS_STOP))
			Entity->Velocity += Dir * (500.0f * (float)KnockBack / Clamp<float> (Entity->Mass, 50, Entity->Mass));
	}

	take = Damage;
	save = 0;

	// check for godmode
	if ((Entity->Flags & FL_GODMODE) && !(DamageFlags & DAMAGE_NO_PROTECTION))
	{
		take = 0;
		save = Damage;
		CSparks (Point, Normal, (DamageFlags & DAMAGE_BULLET) ? ST_BULLET_SPARKS : ST_SPARKS, SPT_SPARKS).Send();
	}

	// check for invincibility
#if ROGUE_FEATURES
	if ((InvincibleFramenum > Level.Frame) && !(DamageFlags & DAMAGE_NO_PROTECTION))
	{
		if (PainDebounceTime < Level.Frame)
		{
			Entity->PlaySound (CHAN_ITEM, SoundIndex("items/protect4.wav"));
			PainDebounceTime = Level.Frame + 20;
		}
		take = 0;
		save = Damage;
	}
#endif

	psave = Entity->CheckPowerArmor (Point, Normal, take, DamageFlags);
	take -= psave;

	//treat cheat/powerup savings the same as armor
	asave += save;

	// team damage avoidance
	if (!(DamageFlags & DAMAGE_NO_PROTECTION) && Entity->CheckTeamDamage (Attacker))
		return;

// ROGUE - this option will do damage both to the armor and person. originally for DPU rounds
	if (DamageFlags & DAMAGE_DESTROY_ARMOR)
	{
		if (!(Entity->Flags & FL_GODMODE) && !(DamageFlags & DAMAGE_NO_PROTECTION))
			take = Damage;
	}
// ROGUE

// do the damage
	if (take)
	{
		DamageEffect (Dir, Point, Normal, take, DamageFlags, MeansOfDeath);
		Entity->Health -= take;
	}

	if (Entity->Health <= 0)
	{
		Entity->AffectedByKnockback = false;
		Entity->Killed (Inflictor, Attacker, take, Point);
		return;
	}

	ReactToDamage (Attacker, Inflictor);

	if (!(AIFlags & AI_DUCKED) && take)
	{
		Entity->Pain (Attacker, take);
		if (CvarList[CV_SKILL].Integer() == 3)
			PainDebounceTime = Level.Frame + 50;
	}
}

void IMonster::MonsterFireBullet (vec3f start, vec3f dir, sint32 Damage, sint32 kick, sint32 hspread, sint32 vspread, sint32 flashtype)
{
	CBullet::Fire (Entity, start, dir, Damage, kick, hspread, vspread, MOD_MACHINEGUN);

	if (flashtype != -1)
		CMuzzleFlash(start, Entity->State.GetNumber(), flashtype, true).Send();
}

void IMonster::MonsterFireShotgun (vec3f start, vec3f aimdir, sint32 Damage, sint32 kick, sint32 hspread, sint32 vspread, sint32 count, sint32 flashtype)
{

	CShotgunPellets::Fire (Entity, start, aimdir, Damage, kick, hspread, vspread, count, MOD_SHOTGUN);

	if (flashtype != -1)
		CMuzzleFlash(start, Entity->State.GetNumber(), flashtype, true).Send();
}

void IMonster::MonsterFireBlaster (vec3f start, vec3f dir, sint32 Damage, sint32 speed, sint32 flashtype, sint32 effect)
{

	CBlasterProjectile::Spawn (Entity, start, dir, Damage, speed, effect, false);

	if (flashtype != -1)
		CMuzzleFlash(start, Entity->State.GetNumber(), flashtype, true).Send();
}	

void IMonster::MonsterFireGrenade (vec3f start, vec3f aimdir, sint32 Damage, sint32 speed, sint32 flashtype)
{
	CGrenade::Spawn (Entity, start, aimdir, Damage, speed, 25, Damage+40);

	if (flashtype != -1)
		CMuzzleFlash(start, Entity->State.GetNumber(), flashtype, true).Send();
}

void IMonster::MonsterFireRocket (vec3f start, vec3f dir, sint32 Damage, sint32 speed, sint32 flashtype)
{
	CRocket::Spawn (Entity, start, dir, Damage, speed, Damage+20, Damage);

	if (flashtype != -1)
		CMuzzleFlash(start, Entity->State.GetNumber(), flashtype, true).Send();
}	

void IMonster::MonsterFireRailgun (vec3f start, vec3f aimdir, sint32 Damage, sint32 kick, sint32 flashtype)
{
	if (!(PointContents (start) & CONTENTS_MASK_SOLID))
		CRailGunShot::Fire (Entity, start, aimdir, Damage, kick);

	if (flashtype != -1)
		CMuzzleFlash(start, Entity->State.GetNumber(), flashtype, true).Send();
}

void IMonster::MonsterFireBfg (vec3f start, vec3f aimdir, sint32 Damage, sint32 speed, sint32 kick, float damage_radius, sint32 flashtype)
{
	CBFGBolt::Spawn (Entity, start, aimdir, Damage, speed, damage_radius);

	if (flashtype != -1)
		CMuzzleFlash(start, Entity->State.GetNumber(), flashtype, true).Send();
}

#if ROGUE_FEATURES
#include "Rogue/RogueWeaponry.h"

void IMonster::MonsterFireBlaster2 (vec3f start, vec3f dir, sint32 Damage, sint32 speed, sint32 flashtype, sint32 effect)
{
	CGreenBlasterProjectile::Spawn (Entity, start, dir, Damage, speed, effect);

	if (flashtype != -1)
		CMuzzleFlash(start, Entity->State.GetNumber(), flashtype, true).Send();
}

void IMonster::MonsterFireTracker (vec3f start, vec3f dir, int damage, int speed, IBaseEntity *enemy, int flashtype)
{
	CDisruptorTracker::Spawn (Entity, start, dir, damage, speed, enemy);

	if (flashtype != -1)
		CMuzzleFlash(start, Entity->State.GetNumber(), flashtype, true).Send();
}

void IMonster::MonsterFireHeat (vec3f start, vec3f dir, int damage, int kick, int flashtype)
{
	CHeatBeam::Fire (Entity, start, dir, damage, kick, MOD_HEATBEAM, true);

	if (flashtype != -1)
		CMuzzleFlash(start, Entity->State.GetNumber(), flashtype, true).Send();
}
#endif

#if XATRIX_FEATURES
#include "Xatrix/XatrixIonRipper.h"
#include "Monsters/SoldierBase.h"
#include "Xatrix/XatrixSoldierHyper.h"
#include "Monsters/Bitch.h"
#include "Xatrix/XatrixChickHeat.h"

void IMonster::MonsterFireRipper (vec3f start, vec3f dir, sint32 Damage, sint32 speed, sint32 flashtype)
{
	CIonRipperBoomerang::Spawn (Entity, start, dir, Damage, speed);

	if (flashtype != -1)
		CMuzzleFlash(start, Entity->State.GetNumber(), flashtype, true).Send();
}

void IMonster::MonsterFireBlueBlaster (vec3f start, vec3f dir, sint32 Damage, sint32 speed, sint32 flashtype)
{
	CBlueBlasterProjectile::Spawn (Entity, start, dir, Damage, speed, FX_BLUEHYPERBLASTER);

	if (flashtype != -1)
		CMuzzleFlash(start, Entity->State.GetNumber(), flashtype, true).Send();
}

void IMonster::MonsterFireHeatRocket (vec3f start, vec3f dir, sint32 Damage, sint32 speed, sint32 flashtype)
{
	CHeatRocket::Spawn (Entity, start, dir, Damage, speed, Damage+20, Damage);

	if (flashtype != -1)
		CMuzzleFlash(start, Entity->State.GetNumber(), flashtype, true).Send();
}	

// RAFAEL
CMonsterBeamLaser::CMonsterBeamLaser () :
IThinkableEntity ()
{
};

void CMonsterBeamLaser::Think ()
{
	if (DoFree)
	{
		Free ();
		return;
	}

	IBaseEntity	*ignore;
	vec3f	start;
	vec3f	end;
	const uint8 Count = (MakeEffect) ? 8 : 4;

	ignore = this;
	start = State.GetOrigin();
	end = start.MultiplyAngles (2048, MoveDir);
	CTrace tr;
	while(1)
	{
		tr (start, end, ignore, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);

		if (!tr.Entity)
			break;

		IBaseEntity *Entity = tr.Entity;

		// hurt it if we can
		if ((Entity->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(Entity)->CanTakeDamage && (Entity != GetOwner()))
			entity_cast<IHurtableEntity>(Entity)->TakeDamage (this, GetOwner(), MoveDir, tr.EndPosition, vec3fOrigin, Damage, CvarList[CV_SKILL].Integer(), DAMAGE_ENERGY, MOD_TARGET_LASER);

		if (Damage < 0) // healer ray
		{
			// when player is at 100 health
			// just undo health fix
			// keeping fx
			if (Entity->EntityFlags & EF_PLAYER)
			{
				CPlayerEntity *Player = entity_cast<CPlayerEntity>(Entity);
				if (Player->Health > 100)
					Player->Health += Damage; 
			}
		}

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
	DoFree = true;
}

// RAFAEL
void IMonster::MonsterFireBeam (CMonsterBeamLaser *Ent)
{	
	Ent->GetSolid() = SOLID_NOT;
	Ent->State.GetRenderEffects() |= RF_BEAM|RF_TRANSLUCENT;
	Ent->State.GetModelIndex() = 1;
		
	Ent->State.GetFrame() = 2;
	
	if (AIFlags & AI_MEDIC)
		Ent->State.GetSkinNum() = 0xf3f3f1f1;
	else
		Ent->State.GetSkinNum() = 0xf2f2f0f0;

	if (Entity->Enemy)
	{
		vec3f lastMoveDir = Ent->MoveDir;
		vec3f point = Entity->Enemy->GetAbsMin().MultiplyAngles (0.5f, Entity->Enemy->GetSize());
		if (AIFlags & AI_MEDIC)
			point.X += (sinf (Level.Frame) * 8) * 10;
		Ent->MoveDir = (point - Ent->State.GetOrigin()).GetNormalized();
		if (Ent->MoveDir != lastMoveDir)
			Ent->MakeEffect = true;
	}
	else
		Ent->MoveDir = Ent->State.GetAngles();

	Ent->NextThink = Level.Frame + 1;
	Ent->GetMins().Set (-8, -8, -8);
	Ent->GetMaxs().Set (8, 8, 8);
	Ent->Link ();
 	
	Ent->MakeEffect = true;
	Ent->GetSvFlags() &= ~SVF_NOCLIENT;
}
#endif

void IMonster::DropToFloor ()
{
	vec3f end = Entity->State.GetOrigin();

	if (Entity->GravityVector.Z < 0)
	{
		Entity->State.GetOrigin().Z += 1;
		end.Z -= 256;
	}
	else
	{
		Entity->State.GetOrigin().Z -= 1;
		end.Z += 256;
	}

	CTrace trace (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), end, Entity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.Fraction == 1 || trace.AllSolid)
		return;

	Entity->State.GetOrigin() = trace.EndPosition;

	Entity->Link ();
	CheckGround ();
	CatagorizePosition ();
}

// These are intended to be virtually replaced.
void IMonster::Stand ()
{
}

void IMonster::Idle ()
{
	if (MonsterFlags & MF_HAS_IDLE)
		DebugPrint ("Warning: Monster with no idle has MF_HAS_IDLE!\n");
}

void IMonster::Search ()
{
	if (MonsterFlags & MF_HAS_SEARCH)
		DebugPrint ("Warning: Monster with no search has MF_HAS_SEARCH!\n");
}

void IMonster::Walk ()
{
}

void IMonster::Run ()
{
}

void IMonster::Dodge (IBaseEntity *Other, float eta
#if ROGUE_FEATURES
		, CTrace *tr
#endif
		)
{
}

void IMonster::Attack()
{
	if (MonsterFlags & MF_HAS_ATTACK)
		DebugPrint ("Warning: Monster with no attack has MF_HAS_ATTACK!\n");
}

void IMonster::Melee ()
{
	if (MonsterFlags & MF_HAS_MELEE)
		DebugPrint ("Warning: Monster with no melee has MF_HAS_MELEE!\n");
}

void IMonster::Sight ()
{
	if (MonsterFlags & MF_HAS_SIGHT)
		DebugPrint ("Warning: Monster with no sight has MF_HAS_SIGHT!\n");
}

void IMonster::MonsterDeathUse ()
{
	AIFlags &= ~(AI_FLY | AI_SWIM |AI_GOOD_GUY);

	if (Entity->Item)
	{
		Entity->Item->DropItem (Entity);
		Entity->Item = NULL;
	}

	if (!Entity->DeathTarget.IsNullOrEmpty())
		Entity->Target = Entity->DeathTarget;

	if (Entity->Target.IsNullOrEmpty())
		return;

	Entity->UseTargets (*Entity->Enemy, Entity->Message);
}

void IMonster::FixInvalidEntities ()
{
}

void IMonster::MonsterThink ()
{
	FixInvalidEntities ();

	Entity->NextThink = Level.Frame + ServerFramesPerSecond;
	MoveFrame ();
	if (Entity->GetLinkCount() != LinkCount)
	{
		LinkCount = Entity->GetLinkCount();
		CheckGround ();
	}
	CatagorizePosition ();
	WorldEffects ();
	SetEffects ();
}

void IMonster::MoveFrame ()
{
	sint32		index;
	CAnim	*Move = CurrentMove;

	// Backwards animation support
	if (Move->FirstFrame > Move->LastFrame)
	{
		if ((NextFrame) && (NextFrame >= Move->LastFrame) && (NextFrame <= Move->FirstFrame))
		{
			Entity->State.GetFrame() = NextFrame;
			NextFrame = 0;
		}
		else
		{
			if (Entity->State.GetFrame() == Move->LastFrame)
			{
				if (Move->EndFunc)
				{
					void (IMonster::*EndFunc) () = Move->EndFunc;
					(this->*EndFunc) ();

					// regrab move, endfunc is very likely to change it
					Move = CurrentMove;

					// check for death
					if (Entity->GetSvFlags() & SVF_DEADMONSTER)
						return;
				}
			}

			if (Entity->State.GetFrame() < Move->LastFrame || Entity->State.GetFrame() > Move->FirstFrame)
			{
				AIFlags &= ~AI_HOLD_FRAME;
				Entity->State.GetFrame() = Move->FirstFrame;
			}
			else
			{
				if (!(AIFlags & AI_HOLD_FRAME))
				{
					if (--Entity->State.GetFrame() < Move->LastFrame)
						Entity->State.GetFrame() = Move->FirstFrame;
				}
			}
		}

		index = Move->FirstFrame - Entity->State.GetFrame();

		void (IMonster::*AIFunc) (float Dist) = Move->Frames[index].AIFunc;
		if (AIFunc)
			(this->*AIFunc) ((AIFlags & AI_HOLD_FRAME) ? 0 : (Move->Frames[index].Dist * GetScale()));

		void (IMonster::*Function) () = Move->Frames[index].Function;
		if (Function)
			(this->*Function) ();
	}
	else
	{
		if ((NextFrame) && (NextFrame >= Move->FirstFrame) && (NextFrame <= Move->LastFrame))
		{
			Entity->State.GetFrame() = NextFrame;
			NextFrame = 0;
		}
		else
		{
			if (Entity->State.GetFrame() == Move->LastFrame)
			{
				if (Move->EndFunc)
				{
					void (IMonster::*EndFunc) () = Move->EndFunc;
					(this->*EndFunc) ();

					// regrab move, endfunc is very likely to change it
					Move = CurrentMove;

					// check for death
					if (Entity->GetSvFlags() & SVF_DEADMONSTER)
						return;
				}
			}

			if (Entity->State.GetFrame() < Move->FirstFrame || Entity->State.GetFrame() > Move->LastFrame)
			{
				AIFlags &= ~AI_HOLD_FRAME;
				Entity->State.GetFrame() = Move->FirstFrame;
			}
			else
			{
				if (!(AIFlags & AI_HOLD_FRAME))
				{
					if (++Entity->State.GetFrame() > Move->LastFrame)
						Entity->State.GetFrame() = Move->FirstFrame;
				}
			}
		}

		index = Entity->State.GetFrame() - Move->FirstFrame;

		void (IMonster::*AIFunc) (float Dist) = Move->Frames[index].AIFunc;
		if (AIFunc)
			(this->*AIFunc) ((AIFlags & AI_HOLD_FRAME) ? 0 : (Move->Frames[index].Dist * GetScale()));

		void (IMonster::*Function) () = Move->Frames[index].Function;
		if (Function)
			(this->*Function) ();
	}
}

void IMonster::SetEffects()
{
	Entity->State.GetEffects() = 0;
	Entity->State.GetRenderEffects() = RF_FRAMELERP;

	if (AIFlags & AI_RESURRECTING)
	{
		Entity->State.GetEffects() |= FX_COLOR_SHELL;
		Entity->State.GetRenderEffects() |= RF_SHELL_RED;
	}

	if (Entity->Health <= 0)
		return;

	if (PowerArmorTime)
	{
		if (PowerArmorType == POWER_ARMOR_SCREEN)
			Entity->State.GetEffects() |= FX_POWERSCREEN;
		else if (PowerArmorType == POWER_ARMOR_SHIELD)
		{
			Entity->State.GetEffects() |= FX_COLOR_SHELL;
			Entity->State.GetRenderEffects() |= RF_SHELL_GREEN;
		}
		PowerArmorTime--;
	}

#if ROGUE_FEATURES
	if (QuadFramenum > Level.Frame)
	{
		FrameNumber remaining = QuadFramenum - Level.Frame;
		if (remaining > 30 || (remaining & 4) )
			Entity->State.GetEffects() |= FX_QUAD;
	}

	if (DoubleFramenum > Level.Frame)
	{
		FrameNumber remaining = DoubleFramenum - Level.Frame;
		if (remaining > 30 || (remaining & 4) )
			Entity->State.GetEffects() |= FX_DOUBLE;
	}

	if (InvincibleFramenum > Level.Frame)
	{
		FrameNumber remaining = InvincibleFramenum - Level.Frame;
		if (remaining > 30 || (remaining & 4) )
			Entity->State.GetEffects() |= FX_PENT;
	}
#endif
}

void IMonster::WorldEffects()
{
	vec3f origin = Entity->State.GetOrigin();

	if (Entity->Health > 0)
	{
		if (!(AIFlags & AI_SWIM))
		{
			if (Entity->WaterInfo.Level < WATER_UNDER)
				Entity->AirFinished = Level.Frame + 120;
			else if (Entity->AirFinished < Level.Frame)
			{
				if (PainDebounceTime < Level.Frame)
				{
					sint32 dmg = 2 + 2 * (Level.Frame - Entity->AirFinished);
					if (dmg > 15)
						dmg = 15;
					Entity->TakeDamage (World, World, vec3fOrigin, origin, vec3fOrigin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					PainDebounceTime = Level.Frame + 10;
				}
			}
		}
		else
		{
			if (Entity->WaterInfo.Level > WATER_NONE)
				Entity->AirFinished = Level.Frame + 90;
			else if (Entity->AirFinished < Level.Frame)
			{	// suffocate!
				if (PainDebounceTime < Level.Frame)
				{
					sint32 dmg = 2 + 2 * (Level.Frame - Entity->AirFinished);
					if (dmg > 15)
						dmg = 15;
					Entity->TakeDamage (World, World, vec3fOrigin, origin, vec3fOrigin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					PainDebounceTime = Level.Frame + 10;
				}
			}
		}
	}
	
	if (Entity->WaterInfo.Level == WATER_NONE)
	{
		if (Entity->WaterInfo.OldLevel != WATER_NONE)
			Entity->PlaySound (CHAN_BODY, SoundIndex("player/watr_out.wav"));

		return;
	}

	if (Entity->WaterInfo.Type & CONTENTS_LAVA)
	{
		if (Entity->DamageDebounceTime < Level.Frame)
		{
			Entity->DamageDebounceTime = Level.Frame + 2;
			Entity->TakeDamage (World, World, vec3fOrigin, origin, vec3fOrigin, 10*Entity->WaterInfo.Level, 0, 0, MOD_LAVA);
		}
	}

	if (Entity->WaterInfo.Type & CONTENTS_SLIME)
	{
		if (Entity->DamageDebounceTime < Level.Frame)
		{
			Entity->DamageDebounceTime = Level.Frame + 10;
			Entity->TakeDamage (World, World, vec3fOrigin, origin, vec3fOrigin, 4*Entity->WaterInfo.Level, 0, 0, MOD_SLIME);
		}
	}
	
	if (Entity->WaterInfo.OldLevel == WATER_NONE)
	{	
		if (!(Entity->GetSvFlags() & SVF_DEADMONSTER))
		{
			if (Entity->WaterInfo.Type & CONTENTS_LAVA)
			{
				if (frand() <= 0.5)
					Entity->PlaySound (CHAN_BODY, SoundIndex("player/lava1.wav"));
				else
					Entity->PlaySound (CHAN_BODY, SoundIndex("player/lava2.wav"));
			}
			else
				Entity->PlaySound (CHAN_BODY, SoundIndex("player/watr_in.wav"));
		}

		Entity->DamageDebounceTime = 0;
	}
}

void IMonster::CatagorizePosition()
{
//
// get waterlevel
//
	vec3f point = Entity->State.GetOrigin() + vec3f (0, 0, Entity->GetMins().Z + 1);	
	EBrushContents cont = PointContents (point);
	Entity->WaterInfo.OldLevel = Entity->WaterInfo.Level;

	if (!(cont & CONTENTS_MASK_WATER))
	{
		Entity->WaterInfo.Level = WATER_NONE;
		Entity->WaterInfo.Type = 0;
		return;
	}

	Entity->WaterInfo.Type = cont;
	Entity->WaterInfo.Level = WATER_FEET;
	point.Z += 26;
	cont = PointContents (point);
	if (!(cont & CONTENTS_MASK_WATER))
		return;

	Entity->WaterInfo.Level = WATER_WAIST;
	point.Z += 22;
	cont = PointContents (point);
	if (cont & CONTENTS_MASK_WATER)
		Entity->WaterInfo.Level = WATER_UNDER;
}

void IMonster::CheckGround()
{
	if (AIFlags & (AI_SWIM | AI_FLY))
		return;

	if ((Entity->Velocity.Z * Entity->GravityVector.Z) < -100)
	{
		Entity->GroundEntity = nullentity;
		return;
	}

// if the hull point one-quarter unit down is solid the entity is on ground
	vec3f point = Entity->State.GetOrigin() + vec3f(0, 0, 0.25 * Entity->GravityVector.Z);
	CTrace trace (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), point, Entity, CONTENTS_MASK_MONSTERSOLID);

	// check steepness
	if (Entity->GravityVector.Z < 0)		// normal gravity
	{
		if (trace.Plane.Normal.Z < 0.7f && !trace.StartSolid)
		{
			Entity->GroundEntity = nullentity;
			return;
		}
	}
	else								// inverted gravity
	{
		if (trace.Plane.Normal.Z > -0.7f && !trace.StartSolid)
		{
			Entity->GroundEntity = nullentity;
			return;
		}
	}

	if (!trace.StartSolid && !trace.AllSolid)
	{
		Entity->State.GetOrigin() = trace.EndPosition;
		Entity->GroundEntity = trace.Entity;
		Entity->GroundEntityLinkCount = trace.Entity->GetLinkCount();
		Entity->Velocity.Z = 0;
	}
}

bool IMonster::FacingIdeal()
{
	float delta = AngleModf (Entity->State.GetAngles().Y - IdealYaw);
	if (delta > 45 && delta < 315)
		return false;
	return true;
}

void IMonster::FliesOff()
{
	Entity->State.GetEffects() &= ~FX_FLIES;
	Entity->State.GetSound() = 0;
}

void IMonster::FliesOn ()
{
	if (Entity->WaterInfo.Level)
		return;
	Entity->State.GetEffects() |= FX_FLIES;
	Entity->State.GetSound() = SoundIndex ("infantry/inflies1.wav");
	Think = &IMonster::FliesOff;
	Entity->NextThink = Level.Frame + 600;
}

void IMonster::CheckFlies ()
{
	if (Entity->WaterInfo.Level)
		return;

	if (frand() > 0.5)
		return;

	Think = &IMonster::FliesOn;
	Entity->NextThink = Level.Frame + ((5 + 10 * frand()) * 10);
}

uint32 LastID = 0;
IMonster::IMonster (uint32 ID) :
MonsterID(ID)
{
}

void IMonster::BossExplode ()
{
	vec3f	org = Entity->State.GetOrigin() + vec3f(0, 0, 24 + (randomMT()&15));
	Think = &IMonster::BossExplode;

	switch (ExplodeCount++)
	{
	case 0:
		org.X -= 24;
		org.Y -= 24;
		break;
	case 1:
		org.X += 24;
		org.Y += 24;
		break;
	case 2:
		org.X += 24;
		org.Y -= 24;
		break;
	case 3:
		org.X -= 24;
		org.Y += 24;
		break;
	case 4:
		org.X -= 48;
		org.Y -= 48;
		break;
	case 5:
		org.X += 48;
		org.Y += 48;
		break;
	case 6:
		org.X -= 48;
		org.Y += 48;
		break;
	case 7:
		org.X += 48;
		org.Y -= 48;
		break;
	case 8:
		Entity->State.GetSound() = 0;
		for (sint32 n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, 500, GIB_ORGANIC);
		for (sint32 n= 0; n < 8; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMetal(), 500, GIB_METALLIC);
		CGibEntity::Spawn (Entity, GameMedia.Gib_Chest, 500, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_Gear(), 500, GIB_METALLIC);
		Entity->IsDead = true;
		return;
	}

	CRocketExplosion (org).Send();
	Entity->NextThink = Level.Frame + ServerFramesPerSecond;
}

