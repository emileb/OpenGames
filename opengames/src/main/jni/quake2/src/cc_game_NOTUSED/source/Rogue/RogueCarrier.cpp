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
// cc_rogue_carrier.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES
#include "Rogue/RogueCarrier.h"
#include "Monsters/Flyer.h"
#include "Rogue/RogueFlyerKamikaze.h"
#include "Rogue/RogueMonsterSpawning.h"

const int CARRIER_ROCKET_TIME		= 20;		// number of seconds between rocket shots
const int CARRIER_ROCKET_SPEED		= 750;
const int NUM_FLYERS_SPAWNED		= 6;		// max # of flyers he can spawn

const int RAIL_FIRE_TIME			= 30;

vec3f flyer_mins (-16, -16, -24);
vec3f flyer_maxs (16, 16, 16);

extern CAnim FlyerMoveAttack2, FlyerMoveKamikaze;

CCarrier::CCarrier (uint32 ID) :
IMonster (ID)
{
};

void CCarrier::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

//
// this is the smarts for the rocket launcher in coop
//
// if there is a player behind/below the carrier, and we can shoot, and we can trace a LOS to them ..
// pick one of the group, and let it rip
void CCarrier::CoopCheck ()
{
	// no more than 4 players in coop, so..
	static std::vector<CPlayerEntity*> targets;
	targets.clear();

	// if we're not in coop, this is a noop
	if (!(Game.GameMode & GAME_COOPERATIVE))
		return;
	// if we are, and we have recently fired, bail
	if (RefireWait > Level.Frame)
		return;

	// cycle through players
	for (int player = 1; player <= Game.MaxClients; player++)
	{
		CPlayerEntity *ent = entity_cast<CPlayerEntity>(Game.Entities[player].Entity);

		if (!ent->GetInUse())
			continue;
		
		if (IsInBack(Entity, ent) || IsBelow(Entity, ent))
		{
			CTrace tr (Entity->State.GetOrigin(), ent->State.GetOrigin(), Entity, CONTENTS_MASK_SOLID);
		
			if (tr.Fraction == 1.0)
				targets.push_back (ent);
		}
	}

	if (targets.empty())
		return;

	int target = irandom(targets.size());

	// make sure to prevent rapid fire rockets
	RefireWait = Level.Frame + CARRIER_ROCKET_TIME;

	// save off the real enemy
	IBaseEntity *OldEnemy = *Entity->Enemy;
	// set the new guy as temporary enemy
	Entity->Enemy = targets[target];
	Rocket ();
	// put the real enemy back
	Entity->Enemy = OldEnemy;

	// we're done
	return;
}

void CCarrier::Grenade ()
{
	CoopCheck ();

	if (!HasValidEnemy())
		return;

	float direction = (frand() < 0.5f) ? -1 : 1;
	FrameNumber mytime = ((Level.Frame - FrameCalc) / 4);

	float spreadR = 0, spreadU = 0;
	switch (mytime)
	{
	case 0:
		spreadR = 0.15f * direction;
		spreadU = 0.1f - 0.1f * direction;
		break;
	case 1:
		spreadR = 0;
		spreadU = 0.1f;
		break;
	case 2:
		spreadR = -0.15f * direction;
		spreadU = 0.1f - -0.1f * direction;
		break;
	case 3:
		spreadR = 0;
		spreadU = 0.1f;
		break;
	};

	vec3f start;

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_CARRIER_GRENADE], angles, start);

	vec3f aim = (Entity->Enemy->State.GetOrigin() - start).GetNormalized()
				.MultiplyAngles (spreadR, angles.Right)
				.MultiplyAngles (spreadU, angles.Up);

	if (aim.Z > 0.15f)
		aim.Z = 0.15f;
	else if (aim.Z < -0.5f)
		aim.Z = -0.5f;

	MonsterFireGrenade (start, aim, 50, 600, MZ2_GUNNER_GRENADE_1);
}

void CCarrier::PredictiveRocket  ()
{
	vec3f start, dir;
	anglef angles = Entity->State.GetAngles().ToVectors ();

//1
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_CARRIER_ROCKET_1], angles, start);
	PredictAim (*Entity->Enemy, start, CARRIER_ROCKET_SPEED, false, -0.3f, &dir, NULL);
	MonsterFireRocket (start, dir, 50, CARRIER_ROCKET_SPEED, MZ2_CARRIER_ROCKET_1);

//2
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_CARRIER_ROCKET_2], angles, start);
	PredictAim (*Entity->Enemy, start, CARRIER_ROCKET_SPEED, false, -0.15f, &dir, NULL);
	MonsterFireRocket (start, dir, 50, CARRIER_ROCKET_SPEED, MZ2_CARRIER_ROCKET_2);

//3
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_CARRIER_ROCKET_3], angles, start);
	PredictAim (*Entity->Enemy, start, CARRIER_ROCKET_SPEED, false, 0, &dir, NULL);
	MonsterFireRocket (start, dir, 50, CARRIER_ROCKET_SPEED, MZ2_CARRIER_ROCKET_3);

//4
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_CARRIER_ROCKET_4], angles, start);
	PredictAim (*Entity->Enemy, start, CARRIER_ROCKET_SPEED, false, 0.15f, &dir, NULL);
	MonsterFireRocket (start, dir, 50, CARRIER_ROCKET_SPEED, MZ2_CARRIER_ROCKET_4);
}	

void CCarrier::Rocket ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start, dir, vec;

	if ((Entity->Enemy->EntityFlags & EF_PLAYER) && frand() < 0.5f)
	{
		PredictiveRocket ();
		return;
	}

	anglef angles = Entity->State.GetAngles().ToVectors ();

//1
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_CARRIER_ROCKET_1], angles, start);
	vec = (Entity->Enemy->State.GetOrigin() - vec3f(0, 0, 15));
	dir = (vec - start).GetNormalized().MultiplyAngles (0.4f, angles.Right).GetNormalized();
	MonsterFireRocket (start, dir, 50, 500, MZ2_CARRIER_ROCKET_1);

//2
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_CARRIER_ROCKET_2], angles, start);
	vec = Entity->Enemy->State.GetOrigin();
	dir = (vec - start).GetNormalized().MultiplyAngles (0.025f, angles.Right).GetNormalized();
	MonsterFireRocket (start, dir, 50, 500, MZ2_CARRIER_ROCKET_2);

//3
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_CARRIER_ROCKET_3], angles, start);
	vec = Entity->Enemy->State.GetOrigin();
	dir = (vec - start).GetNormalized().MultiplyAngles (-0.025f, angles.Right).GetNormalized();
	MonsterFireRocket (start, dir, 50, 500, MZ2_CARRIER_ROCKET_3);

//4
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_CARRIER_ROCKET_4], angles, start);
	vec = (Entity->Enemy->State.GetOrigin() - vec3f(0, 0, 15));
	dir = (vec - start).GetNormalized().MultiplyAngles (-0.4f, angles.Right).GetNormalized();
	MonsterFireRocket (start, dir, 50, 500, MZ2_CARRIER_ROCKET_4);
}	

void CCarrier::FireBulletRight ()
{
	if (!HasValidEnemy())
		return;

	// if we're in manual steering mode, it means we're leaning down .. use the lower shot
	EMuzzleFlash flashnum = (AIFlags & AI_MANUAL_STEERING) ? MZ2_CARRIER_MACHINEGUN_R2 : MZ2_CARRIER_MACHINEGUN_R1;

	vec3f start;
	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flashnum], angles, start);

	vec3f target = Entity->Enemy->State.GetOrigin().MultiplyAngles (0.2f, entity_cast<IPhysicsEntity>(*Entity->Enemy)->Velocity) + vec3f(0, 0, Entity->Enemy->ViewHeight);
	angles.Forward = (target - start).GetNormalized();

	MonsterFireBullet (start, angles.Forward, 6, 4, DEFAULT_BULLET_HSPREAD*3, DEFAULT_BULLET_VSPREAD, flashnum);
}	

void CCarrier::FireBulletLeft ()
{
	if (!HasValidEnemy())
		return;

	// if we're in manual steering mode, it means we're leaning down .. use the lower shot
	EMuzzleFlash flashnum = (AIFlags & AI_MANUAL_STEERING) ? MZ2_CARRIER_MACHINEGUN_L2 : MZ2_CARRIER_MACHINEGUN_L1;

	vec3f start;
	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flashnum], angles, start);

	vec3f target = Entity->Enemy->State.GetOrigin().MultiplyAngles (-0.2f, entity_cast<IPhysicsEntity>(*Entity->Enemy)->Velocity) + vec3f(0, 0, Entity->Enemy->ViewHeight);
	angles.Forward = (target - start).GetNormalized();

	MonsterFireBullet (start, angles.Forward, 6, 4, DEFAULT_BULLET_HSPREAD*3, DEFAULT_BULLET_VSPREAD, flashnum);
}

void CCarrier::MachineGun ()
{
	CoopCheck ();

	if (Entity->Enemy)
	{
		FireBulletLeft ();
		FireBulletRight ();
	}
}	

void CCarrier::SpawnMonsters ()
{
	vec3f offset (105, 0, -58); // real distance needed is (sqrt (56*56*2) + sqrt(16*16*2)) or 101.8
	anglef angles = Entity->State.GetAngles().ToVectors ();

	vec3f startpoint;
	G_ProjectSource (Entity->State.GetOrigin(), offset, angles, startpoint);

	// the +0.1 is because level.time is sometimes a little low
	FrameNumber mytime = ((Level.Frame + 1 - FrameCalc) / 5);

	vec3f spawnpoint;
	if (FindSpawnPoint (startpoint, flyer_mins, flyer_maxs, spawnpoint, 32))
	{
		CMonsterEntity *ent = NULL;

		// the second flier should be a kamikaze flyer
		if (mytime != 2)
			ent = CreateMonster (spawnpoint, Entity->State.GetAngles(), "monster_flyer");
		else
			ent = CreateMonster (spawnpoint, Entity->State.GetAngles(), "monster_kamikaze");

		if (!ent)
			return;

		Entity->PlaySound (CHAN_BODY, Sounds[SOUND_SPAWN]);

		MonsterSlots--;

		ent->NextThink = Level.Frame;
		void	(IMonster::*TheThink) () = ent->Monster->Think;
		(ent->Monster->*TheThink) ();
		
		ent->Monster->AIFlags |= AI_SPAWNED_CARRIER|AI_DO_NOT_COUNT|AI_IGNORE_SHOTS;
		ent->Monster->Commander = Entity;

		if ((Entity->Enemy->GetInUse()) && (entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0))
		{
			ent->Enemy = Entity->Enemy;
			ent->Monster->FoundTarget ();
			if (mytime == 1)
			{
				ent->Monster->Lefty = false;
				ent->Monster->AttackState = AS_SLIDING;
				ent->Monster->CurrentMove = &FlyerMoveAttack2;
			}
			else if (mytime == 2)
			{
				ent->Monster->Lefty = false;
				ent->Monster->AttackState = AS_STRAIGHT;
				ent->Monster->CurrentMove = &FlyerMoveKamikaze;
				ent->Monster->AIFlags |= AI_CHARGING;
			}
			else if (mytime == 3)
			{
				ent->Monster->Lefty = true;
				ent->Monster->AttackState = AS_SLIDING;
				ent->Monster->CurrentMove = &FlyerMoveAttack2;
			}
		}
	}
}

void CCarrier::PrepSpawn ()
{
	CoopCheck ();
	AIFlags |= AI_MANUAL_STEERING;
	FrameCalc = Level.Frame;
	YawSpeed = 10;
	MachineGun ();
}

void CCarrier::SpawnCheck ()
{
	CoopCheck ();
	MachineGun ();
	SpawnMonsters ();

	if (Level.Frame > (FrameCalc + 11))  // 0.5 seconds per flyer.  this gets three
	{
		AIFlags &= ~AI_MANUAL_STEERING;
		YawSpeed = OriginalYawSpeed;
		return;
	}
	else
		NextFrame = CCarrier::FRAME_spawn08;
}

void CCarrier::ReadySpawn ()
{
	CoopCheck();
	MachineGun();

	float current_yaw = AngleModf (Entity->State.GetAngles().Y);

	if (Abs(current_yaw - IdealYaw) > 0.1f)
	{
		AIFlags |= AI_HOLD_FRAME;
		FrameCalc += ServerFramesPerSecond;
		return;
	}

	AIFlags &= ~AI_HOLD_FRAME;

	static const vec3f offset (105, 0, -58);
	vec3f startpoint, spawnpoint;

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), offset, angles, startpoint);

	if (FindSpawnPoint (startpoint, flyer_mins, flyer_maxs, spawnpoint, 32))
		CSpawnGrow::Spawn (spawnpoint, 0);
}

void CCarrier::StartSpawn ()
{
	CoopCheck ();
	if (!OriginalYawSpeed)
		OriginalYawSpeed = YawSpeed;

	if (!Entity->Enemy)
		return;

	FrameNumber mytime = ((Level.Frame - FrameCalc) / 5);	
	float enemy_yaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();

	// note that the offsets are based on a forward of 105 from the end angle
	switch (mytime)
	{
	case 0:
		IdealYaw = AngleModf(enemy_yaw - 30);
		break;
	case 1:
		IdealYaw = AngleModf(enemy_yaw);
		break;
	case 2:
		IdealYaw = AngleModf(enemy_yaw + 30);
		break;
	};

	MachineGun ();
}

CFrame CarrierFramesStand [] =
{
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim CarrierMoveStand (CCarrier::FRAME_search01, CCarrier::FRAME_search13, CarrierFramesStand);

CFrame CarrierFramesWalk [] =
{
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4)
};
CAnim CarrierMoveWalk (CCarrier::FRAME_search01, CCarrier::FRAME_search13, CarrierFramesWalk);

CFrame CarrierFramesRun [] =
{
	CFrame (&IMonster::AI_Run,	6,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Run,	6,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Run,	6,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Run,	6,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Run,	6,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Run,	6,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Run,	6,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Run,	6,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Run,	6,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Run,	6,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Run,	6,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Run,	6,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Run,	6,	ConvertDerivedFunction(&CCarrier::CoopCheck))
};
CAnim CarrierMoveRun (CCarrier::FRAME_search01, CCarrier::FRAME_search13, CarrierFramesRun);

CFrame CarrierFramesPreAttackMG [] =
{
	CFrame (&IMonster::AI_Charge,	4,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge,	4,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge,	4,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge,	4,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge,	4,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge,	4,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge,	4,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge,	4,	ConvertDerivedFunction(&CCarrier::AttackMachinegun))
};
CAnim CarrierMovePreAttackMG (CCarrier::FRAME_firea01, CCarrier::FRAME_firea08, CarrierFramesPreAttackMG);

// Loop this
CFrame CarrierFramesAttackMG [] =
{
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::ReAttackMachinegun))
};
CAnim CarrierMoveAttackMG (CCarrier::FRAME_firea09, CCarrier::FRAME_firea11, CarrierFramesAttackMG);

CFrame CarrierFramesAttackPostMG [] =
{
	CFrame (&IMonster::AI_Charge,	4,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge,	4,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge,	4,	ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge,	4,	ConvertDerivedFunction(&CCarrier::CoopCheck))
};
CAnim CarrierMoveAttackPostMG (CCarrier::FRAME_firea12, CCarrier::FRAME_firea15, CarrierFramesAttackPostMG, &IMonster::Run);

CFrame CarrierFramesAttackPreGrenade [] =
{
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::AttackGrenade))
};
CAnim CarrierMoveAttackPreGrenade (CCarrier::FRAME_fireb01, CCarrier::FRAME_fireb06, CarrierFramesAttackPreGrenade);

CFrame CarrierFramesAttackGrenade [] =
{
	CFrame (&IMonster::AI_Charge, -15, ConvertDerivedFunction(&CCarrier::Grenade)),
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::ReAttackGrenade))
};
CAnim CarrierMoveAttackGrenade (CCarrier::FRAME_fireb07, CCarrier::FRAME_fireb10, CarrierFramesAttackGrenade);

CFrame CarrierFramesAttackPostGrenade [] =
{
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 4, ConvertDerivedFunction(&CCarrier::CoopCheck))
};
CAnim CarrierMoveAttackPostGrenade (CCarrier::FRAME_fireb11, CCarrier::FRAME_fireb16, CarrierFramesAttackPostGrenade, &IMonster::Run);

CFrame CarrierFramesAttackRocket [] =
{
	CFrame (&IMonster::AI_Charge,	15,	ConvertDerivedFunction(&CCarrier::Rocket))
};
CAnim CarrierMoveAttackRocket (CCarrier::FRAME_fireb01, CCarrier::FRAME_fireb01, CarrierFramesAttackRocket, &IMonster::Run);

void CCarrier::Rail ()
{
	CoopCheck ();

	if (!HasValidEnemy())
		return;

	vec3f start;
	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_CARRIER_RAILGUN], angles, start);

	// calc direction to where we targeted
	vec3f dir = (RailFireSpot - start).GetNormalized();

	MonsterFireRailgun (start, dir, 50, 100, MZ2_CARRIER_RAILGUN);
	AttackFinished = Level.Frame + RAIL_FIRE_TIME;
}

void CCarrier::SaveLoc ()
{
	CoopCheck ();
	RailFireSpot = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight);
};

CFrame CarrierFramesAttackRail [] =
{
	CFrame (&IMonster::AI_Charge, 2, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 2, ConvertDerivedFunction(&CCarrier::SaveLoc)),
	CFrame (&IMonster::AI_Charge, 2, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, -20, ConvertDerivedFunction(&CCarrier::Rail)),
	CFrame (&IMonster::AI_Charge, 2, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 2, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 2, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 2, ConvertDerivedFunction(&CCarrier::CoopCheck)),
	CFrame (&IMonster::AI_Charge, 2, ConvertDerivedFunction(&CCarrier::CoopCheck))
};
CAnim CarrierMoveAttackRail (CCarrier::FRAME_search01, CCarrier::FRAME_search09, CarrierFramesAttackRail, &IMonster::Run);

CFrame CarrierFramesSpawn [] =
{
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::PrepSpawn)),		// 7 - end of wind down
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::StartSpawn)),		// 8 - start of spawn
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::ReadySpawn)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-10, ConvertDerivedFunction(&CCarrier::SpawnCheck)),		//12 - actual spawn
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),		//13 - begin of wind down
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::MachineGun)),
	CFrame (&IMonster::AI_Charge,	-2,	ConvertDerivedFunction(&CCarrier::ReAttackMachinegun))		//18 - end of wind down
};
CAnim CarrierMoveSpawn (CCarrier::FRAME_spawn01, CCarrier::FRAME_spawn18, CarrierFramesSpawn);

CFrame CarrierFramesPainHeavy [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim CarrierMovePainHeavy (CCarrier::FRAME_death01, CCarrier::FRAME_death10, CarrierFramesPainHeavy, &IMonster::Run);

CFrame CarrierFramesPainLight [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim CarrierMovePainLight (CCarrier::FRAME_spawn01, CCarrier::FRAME_spawn04, CarrierFramesPainLight, &IMonster::Run);

CFrame CarrierFramesDeath [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	&IMonster::BossExplode)
};
CAnim CarrierMoveDeath (CCarrier::FRAME_death01, CCarrier::FRAME_death16, CarrierFramesDeath, ConvertDerivedFunction(&CCarrier::Dead));

void CCarrier::Stand ()
{
	CurrentMove = &CarrierMoveStand;
}

void CCarrier::Run ()
{
	AIFlags &= ~AI_HOLD_FRAME;

	if (AIFlags & AI_STAND_GROUND)
		CurrentMove = &CarrierMoveStand;
	else
		CurrentMove = &CarrierMoveRun;
}

void CCarrier::Walk ()
{
	CurrentMove = &CarrierMoveWalk;
}

void CCarrier::Attack ()
{	
	AIFlags &= ~AI_HOLD_FRAME;

	if (!Entity->Enemy)
		return;

	bool EnemyInback = IsInBack(Entity, *Entity->Enemy);
	EnemyInfront = IsInFront (Entity, *Entity->Enemy);
	bool EnemyBelow = IsBelow (Entity, *Entity->Enemy);

	if (BadArea)
	{
		if ((EnemyInback) || (EnemyBelow))
			CurrentMove = &CarrierMoveAttackRocket;
		else if ((frand() < 0.1) || (Level.Frame < AttackFinished))
			CurrentMove = &CarrierMovePreAttackMG;
		else
		{
			Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_RAIL]);
			CurrentMove = &CarrierMoveAttackRail;
		}
		return;
	}

	if (AttackState == AS_BLIND)
	{
		CurrentMove = &CarrierMoveSpawn;
		return;
	}

	if (!EnemyInback && !EnemyInfront && !EnemyBelow) // to side and not under
	{
		if ((frand() < 0.1) || (Level.Frame < AttackFinished)) 
			CurrentMove = &CarrierMovePreAttackMG;
		else
		{
			Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_RAIL]);
			CurrentMove = &CarrierMoveAttackRail;
		}
		return;
	}

	if (EnemyInfront)
	{
		float range = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).Length();
		if (range <= 125)
		{
			if ((frand() < 0.8) || (Level.Frame < AttackFinished))
				CurrentMove = &CarrierMovePreAttackMG;
			else
			{
				Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_RAIL]);
				CurrentMove = &CarrierMoveAttackRail;
			}
		}
		else if (range < 600)
		{
			float luck = frand();
			if (MonsterSlots > 2)
			{
				if (luck <= 0.20)
					CurrentMove = &CarrierMovePreAttackMG;
				else if (luck <= 0.40)
					CurrentMove = &CarrierMoveAttackPreGrenade;
				else if ((luck <= 0.7) && !(Level.Frame < AttackFinished))
				{
					Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_RAIL]);
					CurrentMove = &CarrierMoveAttackRail;
				}
				else
					CurrentMove = &CarrierMoveSpawn;
			}
			else
			{
				if (luck <= 0.30)
					CurrentMove = &CarrierMovePreAttackMG;
				else if (luck <= 0.65)
					CurrentMove = &CarrierMoveAttackPreGrenade;
				else if (Level.Frame >= AttackFinished)
				{
					Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_RAIL]);
					CurrentMove = &CarrierMoveAttackRail;
				}
				else
					CurrentMove = &CarrierMovePreAttackMG;
			}
		}
		else // won't use grenades at this range
		{
			float luck = frand();
			if (MonsterSlots > 2)
			{
				if (luck < 0.3)
					CurrentMove = &CarrierMovePreAttackMG;
				else if ((luck < 0.65) && !(Level.Frame < AttackFinished))
				{
					Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_RAIL]);
					RailFireSpot = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight);
					CurrentMove = &CarrierMoveAttackRail;
				}
				else
					CurrentMove = &CarrierMoveSpawn;
			}
			else
			{
				if ((luck < 0.45f) || (Level.Frame < AttackFinished))
					CurrentMove = &CarrierMovePreAttackMG;
				else
				{
					Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_RAIL]);
					CurrentMove = &CarrierMoveAttackRail;
				}
			}
		}
	}
	else if ((EnemyBelow) || (EnemyInback))
		CurrentMove = &CarrierMoveAttackRocket;
}

void CCarrier::AttackMachinegun ()
{
	CoopCheck();
	CurrentMove = &CarrierMoveAttackMG;
}

void CCarrier::ReAttackMachinegun ()
{
	CoopCheck();

	if (IsInFront(Entity, *Entity->Enemy))
	{
		if (frand() <= 0.5)
		{
			if ((frand() < 0.7) || (MonsterSlots <= 2))
				CurrentMove = &CarrierMoveAttackMG;
			else
				CurrentMove = &CarrierMoveSpawn;
		}
		else
			CurrentMove = &CarrierMoveAttackPostMG;
	}
	else
		CurrentMove = &CarrierMoveAttackPostMG;
}


void CCarrier::AttackGrenade ()
{
	CoopCheck();

	FrameCalc = Level.Frame;
	CurrentMove = &CarrierMoveAttackGrenade;
}

void CCarrier::ReAttackGrenade ()
{
	CoopCheck ();

	if (IsInFront(Entity, *Entity->Enemy) && (FrameCalc + 13) > Level.Frame) // four grenades
	{
		CurrentMove = &CarrierMoveAttackGrenade;
		return;
	}

	CurrentMove = &CarrierMoveAttackPostGrenade;
}


void CCarrier::Pain (IBaseEntity *Other, sint32 Damage)
{
	bool changed = false;

	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 50;

	if (Damage < 10)
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN3]);
	else if (Damage < 30)
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
		if (frand() < 0.5f)
		{
			changed = true;
			CurrentMove = &CarrierMovePainLight;
		}
	}
	else 
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN2]);
		CurrentMove = &CarrierMovePainHeavy;
		changed = true;
	}

	// if we changed frames, clean up our little messes
	if (changed)
	{
		AIFlags &= ~AI_HOLD_FRAME;
		AIFlags &= ~AI_MANUAL_STEERING;
		YawSpeed = OriginalYawSpeed;
	}
}

void CCarrier::Dead ()
{
	Entity->GetMins().Set (-56, -56, 0);
	Entity->GetMaxs().Set (56, 56, 80);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

void CCarrier::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DEATH]);
	Entity->IsDead = true;
	Entity->CanTakeDamage = false;
	ExplodeCount = 0;
	CurrentMove = &CarrierMoveDeath;
}

// Immune to lasers
void CCarrier::TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker,
						vec3f Dir, vec3f Point, vec3f Normal, sint32 Damage,
						sint32 Knockback, EDamageFlags DamageFlags, EMeansOfDeath MeansOfDeath)
{
	if (MeansOfDeath == MOD_TARGET_LASER)
		return;

	IMonster::TakeDamage (Inflictor, Attacker, Dir, Point, Normal, Damage, Knockback, DamageFlags, MeansOfDeath);
}

bool CCarrier::CheckAttack ()
{
	if ((Entity->Enemy->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0)
	{
	// see if any entities are in the way of the shot
		vec3f	spot1 = Entity->State.GetOrigin() + vec3f(0, 0, Entity->ViewHeight),
				spot2 = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight);

		CTrace tr (spot1, spot2, Entity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA);

		// do we have a clear shot?
		if (tr.Entity != Entity->Enemy)
		{	
			// go ahead and spawn stuff if we're mad a a client
			if ((Entity->Enemy->EntityFlags & EF_PLAYER) && MonsterSlots > 2)
			{
				AttackState = AS_BLIND;
				return true;
			}
				
			// PGM - we want them to go ahead and shoot at info_notnulls if they can.
			if (Entity->Enemy->GetSolid() != SOLID_NOT || tr.Fraction < 1.0)		//PGM
				return false;
		}
	}
	
	EnemyInfront = IsInFront(Entity, *Entity->Enemy);
	bool EnemyInback = IsInBack(Entity, *Entity->Enemy);
	bool EnemyBelow = IsBelow (Entity, *Entity->Enemy);

	EnemyRange = Range (Entity, *Entity->Enemy);
	IdealYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();

	// PMM - shoot out the back if appropriate
	if ((EnemyInback) || (!EnemyInfront && EnemyBelow))
	{
		// this is using wait because the attack is supposed to be independent
		if (Level.Frame >= RefireWait)
		{
			RefireWait = Level.Frame + CARRIER_ROCKET_TIME;
			Attack ();
			if (frand() < 0.6)
				AttackState = AS_SLIDING;
			else
				AttackState = AS_STRAIGHT;
			return true;
		}
	}

	// melee attack
	if (EnemyRange == RANGE_MELEE)
	{
		AttackState = AS_MISSILE;
		return true;
	}
	
	float chance = 0.0f;
	if (AIFlags & AI_STAND_GROUND)
		chance = 0.4f;
	else 
	{
		switch (EnemyRange)
		{
		case RANGE_MELEE:
		case RANGE_NEAR:
		case RANGE_MID:
			chance = 0.8f;
			break;
		case RANGE_FAR:
			chance = 0.5f;
			break;
		};
	}

	// PGM - go ahead and shoot every time if it's a info_notnull
	if ((Entity->Enemy->GetSolid() == SOLID_NOT) || (frand () < chance))
	{
		AttackState = AS_MISSILE;
		return true;
	}

	if (AIFlags & AI_FLY)
	{
		if (frand() < 0.6)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

	return false;
}

/*QUAKED monster_carrier (1 .5 0) (-56 -56 -44) (56 56 44) Ambush Trigger_Spawn Sight
*/
void CCarrier::Spawn ()
{
	Sounds[SOUND_PAIN1] = SoundIndex ("carrier/pain_md.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("carrier/pain_lg.wav");
	Sounds[SOUND_PAIN3] = SoundIndex ("carrier/pain_sm.wav");
	Sounds[SOUND_DEATH] = SoundIndex ("carrier/death.wav");
	Sounds[SOUND_RAIL] = SoundIndex ("gladiator/railgun.wav");
	Sounds[SOUND_SIGHT] = SoundIndex ("carrier/sight.wav");
	Sounds[SOUND_SPAWN] = SoundIndex ("medic_commander/monsterspawn1.wav");

	Entity->State.GetSound() = SoundIndex ("bosshovr/bhvengn1.wav");

	Entity->PhysicsType = PHYSICS_STEP;
	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/carrier/tris.md2");
	Entity->GetMins().Set (-56, -56, -44);
	Entity->GetMaxs().Set (56, 56, 44);

	// 2000 - 4000 health
	Entity->Health = Max<> (2000, 2000 + 1000*((CvarList[CV_SKILL].Integer())-1));

	// add health in coop (500 * skill)
	if (Game.GameMode & GAME_COOPERATIVE)
		Entity->Health += 500*(CvarList[CV_SKILL].Integer());	

	Entity->GibHealth = -200;
	Entity->Mass = 1000;

	OriginalYawSpeed = YawSpeed = 15;
	
	AIFlags |= AI_IGNORE_SHOTS;
	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_SIGHT);
	Entity->Link ();

	CurrentMove = &CarrierMoveStand;	

	SoundIndex ("flyer/flysght1.wav");
	SoundIndex ("flyer/flysrch1.wav");
	SoundIndex ("flyer/flypain1.wav");
	SoundIndex ("flyer/flypain2.wav");
	SoundIndex ("flyer/flyatck2.wav");
	SoundIndex ("flyer/flyatck1.wav");
	SoundIndex ("flyer/flydeth1.wav");
	SoundIndex ("flyer/flyatck3.wav");
	SoundIndex ("flyer/flyidle1.wav");
	SoundIndex ("weapons/rockfly.wav");
	SoundIndex ("infantry/infatck1.wav");
	SoundIndex ("gunner/gunatck3.wav");
	SoundIndex ("weapons/grenlb1b.wav");
	SoundIndex ("tank/rocket.wav");

	ModelIndex ("models/monsters/flyer/tris.md2");
	ModelIndex ("models/objects/rocket/tris.md2");
	ModelIndex ("models/objects/debris2/tris.md2");
	ModelIndex ("models/objects/grenade/tris.md2");
	ModelIndex("models/items/spawngro/tris.md2");
	ModelIndex("models/items/spawngro2/tris.md2");
	ModelIndex ("models/objects/gibs/sm_metal/tris.md2");
	ModelIndex ("models/objects/gibs/gear/tris.md2");

	FlyMonsterStart ();

	AttackFinished = 0;
	switch (CvarList[CV_SKILL].Integer())
	{
	case 0:
		MonsterSlots = 3;
		break;
	case 1:
	case 2:
		MonsterSlots = 6;
		break;
	case 3:
		MonsterSlots = 9;
		break;
	default:
		MonsterSlots = 6;
		break;
	}
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_carrier", CCarrier);

#endif
