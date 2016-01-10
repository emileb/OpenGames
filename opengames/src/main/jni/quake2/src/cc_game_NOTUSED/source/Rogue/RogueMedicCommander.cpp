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
// cc_rogue_medic_commander.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES
#include "Monsters/Medic.h"
#include "Rogue/RogueMonsterSpawning.h"
#include "Rogue/RogueMedicCommander.h"

CReinforcement Reinforcements[] =
{
	CReinforcement ("monster_soldier_light", vec3f(-16, -16, -24), vec3f(16, 16, 32), vec3f(80, 0, 0)),
	CReinforcement ("monster_soldier", vec3f(-16, -16, -24), vec3f(16, 16, 32), vec3f(40, 60, 0)),
	CReinforcement ("monster_soldier_ss", vec3f(-16, -16, -24), vec3f(16, 16, 32), vec3f(40, -60, 0)),
	CReinforcement ("monster_infantry", vec3f(-16, -16, -24), vec3f(16, 16, 32), vec3f(0, 80, 0)),
	CReinforcement ("monster_gunner", vec3f(-16, -16, -24), vec3f(16, 16, 32), vec3f(0, -80, 0)),
	CReinforcement ("monster_medic", vec3f(-16, -16, -24), vec3f(16, 16, 32), vec3fOrigin),
	CReinforcement ("monster_gladiator", vec3f(-32, -32, -24), vec3f(32, 32, 64), vec3fOrigin),
};

CMedicCommander::CMedicCommander (uint32 ID) :
CMedic(ID)
{
}

void CMedicCommander::FireBlaster ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start, end, dir;
	sint32		effect = 0;

	switch (Entity->State.GetFrame())
	{
	case FRAME_attack9:
	case FRAME_attack12:
		effect = FX_BLASTER;
		break;
	case FRAME_attack19:
	case FRAME_attack22:
	case FRAME_attack25:
	case FRAME_attack28:
		effect = FX_HYPERBLASTER;
		break;
	default:
		break;
	};

	anglef angles = Entity->State.GetAngles().ToVectors();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_MEDIC_BLASTER_2], angles, start);

	end = Entity->Enemy->State.GetOrigin();
	end.Z += Entity->Enemy->ViewHeight;
	dir = end - start;

	MonsterFireBlaster2 (start, dir, 2, 1000, MZ2_MEDIC_BLASTER_2, effect);
}

extern CAnim MedicMoveAttackBlaster;
extern CAnim MedicMoveAttackCable;
extern CAnim MedicMoveStand;

void CMedicCommander::StartSpawn ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundIndex ("medic_commander/monsterspawn1.wav"));
	NextFrame = FRAME_attack48;
}

void CMedicCommander::DetermineSpawn ()
{
	int		num_success = 0;

	float lucky = frand();
	sint16 summonStr = CvarList[CV_SKILL].Integer();

	// bell curve - 0 = 67%, 1 = 93%, 2 = 99% -- too steep
	// this ends up with
	// -3 = 5%
	// -2 = 10%
	// -1 = 15%
	// 0  = 40%
	// +1 = 15%
	// +2 = 10%
	// +3 = 5%
	if (lucky < 0.05)
		summonStr -= 3;
	else if (lucky < 0.15)
		summonStr -= 2;
	else if (lucky < 0.3)
		summonStr -= 1;
	else if (lucky > 0.95)
		summonStr += 3;
	else if (lucky > 0.85)
		summonStr += 2;
	else if (lucky > 0.7)
		summonStr += 1;

	if (summonStr < 0)
		summonStr = 0;

	SummonCount = summonStr;

	anglef angles = Entity->State.GetAngles().ToVectors();

// this yields either 1, 3, or 5
	int num_summoned = (summonStr) ? ((summonStr - 1) + (summonStr % 2)) : 1;

	for (int count = 0; count < num_summoned; count++)
	{
		int inc = count + (count%2); // 0, 2, 2, 4, 4
		vec3f offset = Reinforcements[count].Position;

		vec3f startpoint;
		G_ProjectSource (Entity->State.GetOrigin(), offset, angles, startpoint);
		// a little off the ground
		startpoint.Z += 10;

		vec3f spawnpoint;
		if (FindSpawnPoint (startpoint, Reinforcements[summonStr-inc].Mins, Reinforcements[summonStr-inc].Maxs, spawnpoint, 32))
		{
			if (CheckGroundSpawnPoint(spawnpoint, 
				Reinforcements[summonStr-inc].Mins, Reinforcements[summonStr-inc].Maxs, 
				256, -1))
			{
				num_success++;
				// we found a spot, we're done here
				count = num_summoned;
			}
		}
	}

	if (num_success == 0)
	{
		for (int count = 0; count < num_summoned; count++)
		{
			int inc = count + (count%2); // 0, 2, 2, 4, 4
			vec3f offset = Reinforcements[count].Position;
			
			// check behind
			offset[0] *= -1.0;
			offset[1] *= -1.0;
			vec3f startpoint;
			G_ProjectSource (Entity->State.GetOrigin(), offset, angles, startpoint);
			// a little off the ground
			startpoint[2] += 10;

			vec3f spawnpoint;
			if (FindSpawnPoint (startpoint, Reinforcements[summonStr-inc].Mins, Reinforcements[summonStr-inc].Maxs, spawnpoint, 32))
			{
				if (CheckGroundSpawnPoint(spawnpoint, 
					Reinforcements[summonStr-inc].Mins, Reinforcements[summonStr-inc].Maxs, 
					256, -1))
				{
					num_success++;
					// we found a spot, we're done here
					count = num_summoned;
				}
			}
		}

		if (num_success)
		{
			AIFlags |= AI_MANUAL_STEERING;
			IdealYaw = AngleModf(Entity->State.GetAngles().Y) + 180;
			if (IdealYaw > 360.0)
				IdealYaw -= 360.0;
		}
	}

	if (num_success == 0)
		NextFrame = FRAME_attack53;
}

void CMedicCommander::SpawnGrows ()
{
	int		num_success = 0;

	// if we've been directed to turn around
	if (AIFlags & AI_MANUAL_STEERING)
	{
		float current_yaw = AngleModf(Entity->State.GetAngles().Y);
		if (Abs(current_yaw - IdealYaw) > 0.1)
		{
			AIFlags |= AI_HOLD_FRAME;
			return;
		}

		// done turning around
		AIFlags &= ~AI_HOLD_FRAME;
		AIFlags &= ~AI_MANUAL_STEERING;
	}

	sint16 summonStr = SummonCount;
	anglef angles = Entity->State.GetAngles().ToVectors ();

	int num_summoned = (summonStr) ? ((summonStr - 1) + (summonStr % 2)) : 1;

	for (int count = 0; count < num_summoned; count++)
	{
		int inc = count + (count%2); // 0, 2, 2, 4, 4
		vec3f offset = Reinforcements[count].Position;

		vec3f startpoint;
		G_ProjectSource (Entity->State.GetOrigin(), offset, angles, startpoint);
		// a little off the ground
		startpoint.Z += 10;

		vec3f spawnpoint;
		if (FindSpawnPoint (startpoint, Reinforcements[summonStr-inc].Mins, Reinforcements[summonStr-inc].Maxs, spawnpoint, 32))
		{
			if (CheckGroundSpawnPoint(spawnpoint, 
				Reinforcements[summonStr-inc].Mins, Reinforcements[summonStr-inc].Maxs, 
				256, -1))
			{
				num_success++;
				if ((summonStr-inc) > 3)
					CSpawnGrow::Spawn (spawnpoint, 1);		// big monster
				else
					CSpawnGrow::Spawn (spawnpoint, 0);		// normal size
			}
		}
	}

	if (num_success == 0)
		NextFrame = FRAME_attack53;
}

void CMedicCommander::FinishSpawn ()
{
	bool behind = false;

	if (SummonCount < 0)
	{
		behind = true;
		SummonCount *= -1;
	}
	sint16 summonStr = SummonCount;
	anglef angles = Entity->State.GetAngles().ToVectors ();

	int num_summoned = (summonStr) ? (summonStr - 1) + (summonStr % 2) : 1;

	for (int count = 0; count < num_summoned; count++)
	{
		int inc = count + (count%2); // 0, 2, 2, 4, 4
		vec3f offset = Reinforcements[count].Position;

		vec3f startpoint;
		G_ProjectSource (Entity->State.GetOrigin(), offset, angles, startpoint);

		// a little off the ground
		startpoint.Z += 10;

		CMonsterEntity *ent = NULL;
		vec3f spawnpoint;
		if (FindSpawnPoint (startpoint, Reinforcements[summonStr-inc].Mins, Reinforcements[summonStr-inc].Maxs, spawnpoint, 32))
		{
			if (CheckSpawnPoint (spawnpoint, Reinforcements[summonStr-inc].Mins, Reinforcements[summonStr-inc].Maxs))
				ent = CreateGroundMonster (spawnpoint, Entity->State.GetAngles(),
					Reinforcements[summonStr-inc].Mins, Reinforcements[summonStr-inc].Maxs,
					Reinforcements[summonStr-inc].Classname, 256);
		}

		if (!ent)
			continue;

		if (ent->Monster->Think)
		{
			ent->NextThink = Level.Frame;

			void	(IMonster::*TheThink) () = ent->Monster->Think;
			(ent->Monster->*TheThink) ();
		}

		ent->Monster->AIFlags |= AI_IGNORE_SHOTS|AI_DO_NOT_COUNT|AI_SPAWNED_MEDIC_C;
		ent->Monster->Commander = Entity;
		MonsterSlots--;

		IBaseEntity *designated_enemy;
		if (AIFlags & AI_MEDIC)
			designated_enemy = *Entity->OldEnemy;
		else
			designated_enemy = *Entity->Enemy;

		if (Game.GameMode & GAME_COOPERATIVE)
		{
			designated_enemy = PickCoopTarget(Entity);
			if (designated_enemy)
			{
				// try to avoid using my enemy
				if (designated_enemy == Entity->Enemy)
				{
					designated_enemy = PickCoopTarget(Entity);
					if (!designated_enemy)
						designated_enemy = *Entity->Enemy;
				}
			}
			else
				designated_enemy = *Entity->Enemy;
		}

		if ((designated_enemy) && (designated_enemy->GetInUse()) && ((designated_enemy->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(designated_enemy)->Health > 0))
		{
			ent->Enemy = designated_enemy;
			ent->Monster->FoundTarget ();
		}
		else
		{
			ent->Enemy = nullentity;
			ent->Monster->Stand ();
		}
	}
}

CFrame MedicFramesCallReinforcements [] =
{
	// ROGUE - 33-36 now CFrame (&IMonster::AI_Charge
	CFrame (&IMonster::AI_Charge, 2),					//33
	CFrame (&IMonster::AI_Charge, 3),
	CFrame (&IMonster::AI_Charge, 5),
	CFrame (&IMonster::AI_Charge, 4.4f),					//36
	CFrame (&IMonster::AI_Charge, 4.7f),
	CFrame (&IMonster::AI_Charge, 5),
	CFrame (&IMonster::AI_Charge, 6),
	CFrame (&IMonster::AI_Charge, 4),					//40
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedicCommander::StartSpawn)),		//42
	CFrame (&IMonster::AI_Move, 0),		//43 -- 43 through 47 are skipped
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedicCommander::DetermineSpawn)),		//48
	CFrame (&IMonster::AI_Charge, 0,		ConvertDerivedFunction(&CMedicCommander::SpawnGrows)),			//49
	CFrame (&IMonster::AI_Move, 0),		//50
	CFrame (&IMonster::AI_Move, 0),		//51
	CFrame (&IMonster::AI_Move, -15,	ConvertDerivedFunction(&CMedicCommander::FinishSpawn)),		//52
	CFrame (&IMonster::AI_Move, -1.5f),
	CFrame (&IMonster::AI_Move, -1.2f),
	CFrame (&IMonster::AI_Move, -3),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, 0.3f),
	CFrame (&IMonster::AI_Move, 0.7f),
	CFrame (&IMonster::AI_Move, 1.2f),
	CFrame (&IMonster::AI_Move, 1.3f)					//60
};
CAnim MedicMoveCallReinforcements (CMedic::FRAME_attack33, CMedic::FRAME_attack60, MedicFramesCallReinforcements, &IMonster::Run);

void CMedicCommander::Attack ()
{
	DoneDodge ();

	EnemyRange = Range(Entity, *Entity->Enemy);

	// signal from checkattack to spawn
	if (AIFlags & AI_BLOCKED)
	{
		CurrentMove = &MedicMoveCallReinforcements;
		AIFlags &= ~AI_BLOCKED;
	}

	float r = frand();
	if (AIFlags & AI_MEDIC)
	{
		if ((r > 0.8) && (MonsterSlots > 2))
			CurrentMove = &MedicMoveCallReinforcements;
		else	
			CurrentMove = &MedicMoveAttackCable;
	}
	else
	{
		if (AttackState == AS_BLIND)
		{
			CurrentMove = &MedicMoveCallReinforcements;
			return;
		}
		if ((r > 0.2) && (EnemyRange != RANGE_MELEE) && (MonsterSlots > 2))
			CurrentMove = &MedicMoveCallReinforcements;
		else
			CurrentMove = &MedicMoveAttackBlaster;
	}
}

const int MEDIC_MIN_DISTANCE			= 32;
const int MEDIC_MAX_HEAL_DISTANCE		= 400;
const int MEDIC_TRY_TIME				= 100;

bool CMedicCommander::CheckAttack ()
{
	if (AIFlags & AI_MEDIC)
	{
		// if our target went away
		if (!Entity->Enemy)
		{
			AbortHeal (false, false);
			return false;
		}

		// if we ran out of time, give up
		if (MedicTryTime < Level.Frame)
		{
			AbortHeal (false, true);
			MedicTryTime = 0;
			return false;
		}
	
		if (RangeFrom(Entity->State.GetOrigin(), Entity->Enemy->State.GetOrigin()) < MEDIC_MAX_HEAL_DISTANCE+10)
		{
			Attack ();
			return true;
		}
		else
		{
			AttackState = AS_STRAIGHT;
			return false;
		}
	}

	if ((Entity->Enemy->EntityFlags & EF_PLAYER) && !IsVisible (Entity, *Entity->Enemy) && (MonsterSlots > 2))
	{
		AttackState = AS_BLIND;
		return true;
	}

	// give a LARGE bias to spawning things when we have room
	// use AI_BLOCKED as a signal to attack to spawn
	if ((frand() < 0.8) && (MonsterSlots > 5) && (RangeFrom(Entity->State.GetOrigin(), Entity->Enemy->State.GetOrigin()) > 150))
	{
		AIFlags |= AI_BLOCKED;
		AttackState = AS_MISSILE;
		return true;
	}
	
	// ROGUE
	// since his idle animation looks kinda bad in combat, if we're not in easy mode, always attack
	// when he's on a combat point
	if ((CvarList[CV_SKILL].Integer() > 0) && (AIFlags & AI_STAND_GROUND))
	{
		AttackState = AS_MISSILE;
		return true;
	}

	return IMonster::CheckAttack ();
}

void CMedicCommander::Spawn ()
{
	Sounds[SOUND_IDLE] = SoundIndex ("medic_commander/medidle.wav");
	Sounds[SOUND_PAIN1] = SoundIndex ("medic_commander/medpain1.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("medic_commander/medpain2.wav");
	Sounds[SOUND_DIE] = SoundIndex ("medic_commander/meddeth.wav");
	Sounds[SOUND_SIGHT] = SoundIndex ("medic_commander/medsght.wav");
	Sounds[SOUND_SEARCH] = SoundIndex ("medic_commander/medsrch.wav");
	Sounds[SOUND_HOOK_LAUNCH] = SoundIndex ("medic_commander/medatck2c.wav");
	Sounds[SOUND_HOOK_HIT] = SoundIndex ("medic_commander/medatck3a.wav");
	Sounds[SOUND_HOOK_HEAL] = SoundIndex ("medic_commander/medatck4a.wav");
	Sounds[SOUND_HOOK_RETRACT] = SoundIndex ("medic_commander/medatck5a.wav");

	SoundIndex ("medic/medatck1.wav");
	SoundIndex ("medic_commander/monsterspawn1.wav");

	ModelIndex ("models/items/spawngro/tris.md2");
	ModelIndex ("models/items/spawngro2/tris.md2");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/medic/tris.md2");
	Entity->GetMins().Set (-24, -24, -24);
	Entity->GetMaxs().Set (24, 24, 32);

	Entity->State.GetSkinNum() = 2;
	switch (CvarList[CV_SKILL].Integer())
	{
	case 0:
	default:
		MonsterSlots = 3;
		break;
	case 1:
		MonsterSlots = 4;
		break;
	case 2:
	case 3:
		MonsterSlots = 6;
		break;
	};

	Entity->Health = 600;
	Entity->GibHealth = -130;
	YawSpeed = 40;
	Entity->Mass = 400;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_SIGHT | MF_HAS_IDLE | MF_HAS_SEARCH
#if ROGUE_FEATURES
		| MF_HAS_DODGE | MF_HAS_DUCK | MF_HAS_UNDUCK | MF_HAS_SIDESTEP
#endif
		);

	Entity->Link ();
	CurrentMove = &MedicMoveStand;

	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_medic_commander", CMedicCommander);

#endif