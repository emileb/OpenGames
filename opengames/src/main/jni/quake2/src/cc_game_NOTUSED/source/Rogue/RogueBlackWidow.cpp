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
// cc_rogue_black_widow.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES

#include "Rogue/RogueWidowStand.h"
#include "Rogue/RogueBlackWidow.h"
#include "Rogue/RogueMonsterSpawning.h"
#include "Utility/TemporaryEntities.h"

CBlackWidow::CBlackWidow (uint32 ID) :
CWidowStand (ID)
{
};

// timestamp used to prevent rapid fire of melee attack


const int NUM_STALKERS_SPAWNED		= 6;		// max # of stalkers she can spawn
const int DISRUPT_TIME				= 30;

// sqrt(64*64*2) + sqrt(28*28*2) => 130.1
static vec3f spawnpoints[] = {
	vec3f(30,  135, 0),
	vec3f(30, -135, 0)
};

static float sweep_angles[] = {
	-40.0, -32.0, -24.0, -16.0, -8.0, 0.0, 8.0, 16.0, 24.0, 32.0, 40.0
};

extern vec3f	stalker_mins, stalker_maxs;

// these offsets used by the tongue
static vec3f offsets[] = {
	vec3f(17.48f, 0.10f, 68.92f),
	vec3f(17.47f, 0.29f, 68.91f),
	vec3f(17.45f, 0.53f, 68.87f),
	vec3f(17.42f, 0.78f, 68.81f),
	vec3f(17.39f, 1.02f, 68.75f),
	vec3f(17.37f, 1.20f, 68.70f),
	vec3f(17.36f, 1.24f, 68.71f),
	vec3f(17.37f, 1.21f, 68.72f),
};

void CBlackWidow::Search ()
{
	if (frand() < 0.5)
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SEARCH1]);
}

void CBlackWidow::FireBeam ()
{
	if (!HasValidEnemy())
		return;

	anglef angles = Entity->State.GetAngles().ToVectors ();
	
	if ((Entity->State.GetFrame() >= CBlackWidow::FRAME_fireb05) && (Entity->State.GetFrame() <= CBlackWidow::FRAME_fireb09))
	{
		vec3f start;

		// regular beam attack
		SaveBeamTarget();
		EMuzzleFlash flashnum = MZ2_WIDOW2_BEAMER_1 + Entity->State.GetFrame() - CBlackWidow::FRAME_fireb05;

		G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flashnum], angles, start);

		vec3f target = BeamPos[1] + vec3f(0, 0, Entity->Enemy->ViewHeight - 10);

		MonsterFireHeat (start, (target - start).GetNormalized(), 10, 50, flashnum);
	}
	else if ((Entity->State.GetFrame() >= CBlackWidow::FRAME_spawn04) && (Entity->State.GetFrame() <= CBlackWidow::FRAME_spawn14))
	{
		vec3f start;

		// sweep
		EMuzzleFlash flashnum = MZ2_WIDOW2_BEAM_SWEEP_1 + Entity->State.GetFrame() - CBlackWidow::FRAME_spawn04;

		G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flashnum], angles, start);
		
		vec3f targ_angles = (Entity->Enemy->State.GetOrigin() - start).ToAngles();
		vec3f vec = Entity->State.GetAngles();		

		vec.X += targ_angles.X;
		vec.Y -= sweep_angles[flashnum-MZ2_WIDOW2_BEAM_SWEEP_1];

		MonsterFireHeat (start, vec.ToVectors().Forward, 10, 50, flashnum);
	}
	else
	{
		vec3f start;

		SaveBeamTarget();
		G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_WIDOW2_BEAMER_1], angles, start);

		vec3f target = BeamPos[1] + vec3f(0, 0, Entity->Enemy->ViewHeight - 10);

		MonsterFireHeat (start, (target - start).GetNormalized(), 10, 50, 0);
	}	
}

void CBlackWidow::SpawnCheck ()
{
	FireBeam ();

	anglef angles = Entity->State.GetAngles().ToVectors ();

	for (uint8 i = 0; i < 2; i++)
	{
		vec3f offset = spawnpoints[i], startpoint, spawnpoint;
		G_ProjectSource (Entity->State.GetOrigin(), offset, angles, startpoint);

		if (FindSpawnPoint (startpoint, stalker_mins, stalker_maxs, spawnpoint, 64))
		{
			CMonsterEntity *ent = CreateGroundMonster (spawnpoint, Entity->State.GetAngles(), stalker_mins, stalker_maxs, "monster_stalker", 256);

			if (!ent)
				continue;
			
			MonsterUsed++;
			ent->Monster->Commander = Entity;

			ent->NextThink = Level.Frame;
			void	(IMonster::*TheThink) () = ent->Monster->Think;
			(ent->Monster->*TheThink) ();
			
			ent->Monster->AIFlags |= AI_SPAWNED_WIDOW|AI_DO_NOT_COUNT|AI_IGNORE_SHOTS;

			IBaseEntity *designated_enemy = *Entity->Enemy;
			if (Game.GameMode & GAME_COOPERATIVE)
			{
				designated_enemy = PickCoopTarget(ent);
				if (designated_enemy)
				{
					// try to avoid using my enemy
					if (designated_enemy == Entity->Enemy)
					{
						designated_enemy = PickCoopTarget(ent);
						if (!designated_enemy)
							designated_enemy = *Entity->Enemy;
					}
				}
				else
					designated_enemy = *Entity->Enemy;
			}

			if ((designated_enemy->GetInUse()) && ((designated_enemy->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(designated_enemy)->Health > 0))
			{
				ent->Enemy = designated_enemy;
				ent->Monster->FoundTarget ();
				ent->Monster->Attack();
			}
		}
	}
}

void CBlackWidow::ReadySpawn ()
{
	vec3f	f, r, u, offset, startpoint, spawnpoint;

	FireBeam ();
	anglef angles = Entity->State.GetAngles().ToVectors ();

	for (uint8 i = 0; i < 2; i++)
	{
		offset = spawnpoints[i];
		G_ProjectSource (Entity->State.GetOrigin(), offset, angles, startpoint);
		if (FindSpawnPoint (startpoint, stalker_mins, stalker_maxs, spawnpoint, 64))
			CSpawnGrow::Spawn (spawnpoint, 1);
	}
}

CFrame widow2_frames_stand [] =
{
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim	Widow2MoveStand (CBlackWidow::FRAME_blackwidow3, CBlackWidow::FRAME_blackwidow3, widow2_frames_stand);

CFrame widow2_frames_walk [] =
{
	CFrame (&IMonster::AI_Walk,	9.01f),
	CFrame (&IMonster::AI_Walk,	7.55f),
	CFrame (&IMonster::AI_Walk,	7.01f),
	CFrame (&IMonster::AI_Walk,	6.66f),
	CFrame (&IMonster::AI_Walk,	6.20f),
	CFrame (&IMonster::AI_Walk,	5.78f),
	CFrame (&IMonster::AI_Walk,	7.25f),
	CFrame (&IMonster::AI_Walk,	8.37f),
	CFrame (&IMonster::AI_Walk,	10.41f)
};
CAnim Widow2MoveWalk (CBlackWidow::FRAME_walk01, CBlackWidow::FRAME_walk09, widow2_frames_walk);

CFrame widow2_frames_run [] =
{
	CFrame (&IMonster::AI_Run,	9.01f),
	CFrame (&IMonster::AI_Run,	7.55f),
	CFrame (&IMonster::AI_Run,	7.01f),
	CFrame (&IMonster::AI_Run,	6.66f),
	CFrame (&IMonster::AI_Run,	6.20f),
	CFrame (&IMonster::AI_Run,	5.78f),
	CFrame (&IMonster::AI_Run,	7.25f),
	CFrame (&IMonster::AI_Run,	8.37f),
	CFrame (&IMonster::AI_Run,	10.41f)
};
CAnim Widow2MoveRun (CBlackWidow::FRAME_walk01, CBlackWidow::FRAME_walk09, widow2_frames_run);

CFrame widow2_frames_attack_pre_beam [] =
{
	CFrame (&IMonster::AI_Charge,	4),
	CFrame (&IMonster::AI_Charge,	4),
	CFrame (&IMonster::AI_Charge,	4),
	CFrame (&IMonster::AI_Charge,	4,	ConvertDerivedFunction(&CBlackWidow::AttackBeam))
};
CAnim Widow2MoveAttackPreBeam (CBlackWidow::FRAME_fireb01, CBlackWidow::FRAME_fireb04, widow2_frames_attack_pre_beam);

// Loop this
CFrame widow2_frames_attack_beam [] =
{
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::FireBeam)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::FireBeam)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::FireBeam)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::FireBeam)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::ReAttackBeam))
};
CAnim Widow2MoveAttackBeam (CBlackWidow::FRAME_fireb05, CBlackWidow::FRAME_fireb09, widow2_frames_attack_beam);

CFrame widow2_frames_attack_post_beam [] =
{
	CFrame (&IMonster::AI_Charge,	4),
	CFrame (&IMonster::AI_Charge,	4),
	CFrame (&IMonster::AI_Charge,	4)
};
CAnim Widow2MoveAttackPostBeam (CBlackWidow::FRAME_fireb06, CBlackWidow::FRAME_fireb07, widow2_frames_attack_post_beam, &IMonster::Run);

void CBlackWidow::FireDisrupt ()
{
	if (!HasValidEnemy())
		return;

	vec3f start;

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_WIDOW_DISRUPTOR], angles, start);

	vec3f dir = BeamPos[0] - Entity->Enemy->State.GetOrigin();
	float len = dir.Length();

	if (len < 30)
	{
		// calc direction to where we targeted
		MonsterFireTracker(start, (BeamPos[0] - start).GetNormalized(), 20, 500, *Entity->Enemy, MZ2_WIDOW_DISRUPTOR);
	}
	else
	{
		PredictAim (*Entity->Enemy, start, 1200, true, 0, &dir, NULL);
		MonsterFireTracker(start, dir, 20, 1200, NULL, MZ2_WIDOW_DISRUPTOR);
	}
}

void CBlackWidow::SaveDisruptLocation ()
{
	if (Entity->Enemy && Entity->Enemy->GetInUse())
		BeamPos[0] = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight);
	else
		BeamPos[0] = vec3fOrigin;
};

void CBlackWidow::DisruptReAttack ()
{
	if (frand() < (0.25f + (CvarList[CV_SKILL].Float() * 0.15f)))
		NextFrame = CBlackWidow::FRAME_firea01;
}

CFrame widow2_frames_attack_disrupt [] =
{
	CFrame (&IMonster::AI_Charge, 2),
	CFrame (&IMonster::AI_Charge, 2),
	CFrame (&IMonster::AI_Charge, 2, ConvertDerivedFunction(&CBlackWidow::SaveDisruptLocation)),
	CFrame (&IMonster::AI_Charge, -20, ConvertDerivedFunction(&CBlackWidow::FireDisrupt)),
	CFrame (&IMonster::AI_Charge, 2),
	CFrame (&IMonster::AI_Charge, 2),
	CFrame (&IMonster::AI_Charge, 2, ConvertDerivedFunction(&CBlackWidow::DisruptReAttack))
};
CAnim Widow2MoveAttackDisrupt (CBlackWidow::FRAME_firea01, CBlackWidow::FRAME_firea07, widow2_frames_attack_disrupt, &IMonster::Run);

void CBlackWidow::SaveBeamTarget ()
{
	if (Entity->Enemy && Entity->Enemy->GetInUse())
	{
		BeamPos[1] = BeamPos[0];
		BeamPos[0] = Entity->Enemy->State.GetOrigin(); //save for aiming the shot
	}
	else
		BeamPos[0] = BeamPos[1] = vec3fOrigin;
}

CFrame widow2_frames_spawn [] =
{
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::StartSpawn)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::FireBeam)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::FireBeam)),				//5
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::FireBeam)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::FireBeam)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::FireBeam)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::FireBeam)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::ReadySpawn)),				//10
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::FireBeam)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::FireBeam)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::FireBeam)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::SpawnCheck)),
	CFrame (&IMonster::AI_Charge,	0),				//15
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::ReAttackBeam))
};
CAnim Widow2MoveSpawn (CBlackWidow::FRAME_spawn01, CBlackWidow::FRAME_spawn18, widow2_frames_spawn);

bool CBlackWidow::TongueAttackOK (vec3f &start, vec3f &end, float range)
{
	// check for max distance
	vec3f dir = start - end;
	if (dir.Length() > range)
		return false;

	// check for min/max pitch
	vec3f angles = dir.ToAngles();
	if (angles.X < -180)
		angles.X += 360;
	if (Abs(angles.X) > 30)
		return false;

	return true;
}

void CBlackWidow::Tongue ()
{
	vec3f start, end, dir;

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), offsets[Entity->State.GetFrame() - CBlackWidow::FRAME_tongs01], angles, start);
	end = Entity->Enemy->State.GetOrigin();
	if (!TongueAttackOK(start, end, 256))
	{
		end.Z = Entity->Enemy->State.GetOrigin().Z + Entity->Enemy->GetMaxs().Z - 8;
		if (!TongueAttackOK(start, end, 256))
		{
			end[2] = Entity->Enemy->State.GetOrigin().Z + Entity->Enemy->GetMins().Z + 8;
			if (!TongueAttackOK(start, end, 256))
				return;
		}
	}
	end = Entity->Enemy->State.GetOrigin();

	CTrace tr (start, end, Entity, CONTENTS_MASK_SHOT);
	if (tr.Entity != Entity->Enemy)
		return;

	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_TENTACLES_RETRACT]);

	CFleshCable (CTempEntFlags(CAST_MULTI, CASTFLAG_PVS, Entity->State.GetOrigin()), start, end, Entity->State.GetNumber()).Send();

	dir = start - end;
	entity_cast<IHurtableEntity>(*Entity->Enemy)->TakeDamage (Entity, Entity, dir, Entity->Enemy->State.GetOrigin(), vec3fOrigin, 2, 0, DAMAGE_NO_KNOCKBACK, MOD_UNKNOWN);
}

void CBlackWidow::TonguePull ()
{
	if (!Entity->Enemy)
	{
		Run ();
		return;
	}

	vec3f start;
	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), offsets[Entity->State.GetFrame() - CBlackWidow::FRAME_tongs01], angles, start);
	vec3f end = Entity->Enemy->State.GetOrigin();

	if (!TongueAttackOK(start, end, 256))
		return;

	if (Entity->Enemy->GroundEntity)
	{
		Entity->Enemy->State.GetOrigin().Z += 1;
		Entity->Enemy->GroundEntity = nullentity;
		
		Entity->Enemy->Link ();
	}
	
	vec3f vec = Entity->State.GetOrigin() - Entity->Enemy->State.GetOrigin();
	if (Entity->Enemy->EntityFlags & EF_PLAYER)
	{
		vec.Normalize();
		entity_cast<IPhysicsEntity>(*Entity->Enemy)->Velocity.MultiplyAngles (1000, vec);
	}
	else
	{
		CMonsterEntity *Mon = entity_cast<CMonsterEntity>(*Entity->Enemy);

		Mon->Monster->IdealYaw = vec.ToYaw();	
		Mon->Monster->ChangeYaw ();
		Mon->Velocity = angles.Forward * 1000;
	}
}

void CBlackWidow::Crunch ()
{
	if (!Entity->Enemy)
	{
		Run ();
		return;
	}

	TonguePull ();

	// 70 + 32
	static const vec3f aim (150, 0, 4);
	if (Entity->State.GetFrame() != CBlackWidow::FRAME_tongs07)
		CMeleeWeapon::Fire (Entity, aim, 20 + (rand() % 6), 0);
	else
	{
		if (Entity->Enemy->GroundEntity)
			CMeleeWeapon::Fire (Entity, aim, (20 + (rand() % 6)), 500);
		else	// not as much kick if they're in the air .. makes it harder to land on her head
			CMeleeWeapon::Fire (Entity, aim, (20 + (rand() % 6)), 250);
	}
}

void CBlackWidow::Toss ()
{
	MeleeDebounceTime = Level.Frame + 30;
	return;
}

CFrame widow2_frames_tongs [] =
{
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::Tongue)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::Tongue)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::Tongue)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::TonguePull)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::TonguePull)),				//5
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::TonguePull)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::Crunch)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBlackWidow::Toss))
};
CAnim Widow2MoveTongs (CBlackWidow::FRAME_tongs01, CBlackWidow::FRAME_tongs08, widow2_frames_tongs, &IMonster::Run);

CFrame widow2_frames_pain [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim Widow2MovePain (CBlackWidow::FRAME_pain01, CBlackWidow::FRAME_pain05, widow2_frames_pain, &IMonster::Run);

CFrame widow2_frames_death [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CBlackWidow::WidowExplosion1)),	// 3 boom
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),				// 5

	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CBlackWidow::WidowExplosion2)),	// 6 boom
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),				// 10

	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),				// 12
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),				// 15

	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CBlackWidow::WidowExplosion2)),	// 18
	CFrame (&IMonster::AI_Move,	0),				// 19
	CFrame (&IMonster::AI_Move,	0),				// 20

	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CBlackWidow::WidowExplosion4)),	// 25

	CFrame (&IMonster::AI_Move,	0),				// 26
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CBlackWidow::WidowExplosion5)),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CBlackWidow::WidowExplosionLeg)),	// 30

	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CBlackWidow::WidowExplosion6)),
	CFrame (&IMonster::AI_Move,	0),				// 35

	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CBlackWidow::WidowExplosion7)),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),				// 40

	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CBlackWidow::WidowExplode))		// 44
};
CAnim Widow2MoveDeath (CBlackWidow::FRAME_death01, CBlackWidow::FRAME_death44, widow2_frames_death);

CFrame widow2_frames_dead [] =
{
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CBlackWidow::StartSearching)),
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
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CBlackWidow::KeepSearching))
};
CAnim Widow2MoveDead (CBlackWidow::FRAME_dthsrh01, CBlackWidow::FRAME_dthsrh15, widow2_frames_dead);

CFrame widow2_frames_really_dead [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),

	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CBlackWidow::Dead))
};
CAnim Widow2MoveReallyDead (CBlackWidow::FRAME_dthsrh16, CBlackWidow::FRAME_dthsrh22, widow2_frames_really_dead);

void CBlackWidow::StartSearching ()
{
	ExplodeCount = 0;
}

void CBlackWidow::KeepSearching ()
{
	if (ExplodeCount <= 2)
	{
		CurrentMove = &Widow2MoveDead;
		Entity->State.GetFrame() = CBlackWidow::FRAME_dthsrh01;
		ExplodeCount++;
		return;
	}

	CurrentMove = &Widow2MoveReallyDead;
}

void CBlackWidow::Stand ()
{
	CurrentMove = &Widow2MoveStand;
}

void CBlackWidow::Run ()
{
	AIFlags &= ~AI_HOLD_FRAME;

	if (AIFlags & AI_STAND_GROUND)
		CurrentMove = &Widow2MoveStand;
	else
		CurrentMove = &Widow2MoveRun;
}

void CBlackWidow::Walk ()
{
	CurrentMove = &Widow2MoveWalk;
}

void CBlackWidow::Melee ()
{
	CurrentMove = &Widow2MoveTongs;
}

void CBlackWidow::Attack ()
{
	bool blocked = false;

	if (AIFlags & AI_BLOCKED)
	{
		blocked = true;
		AIFlags &= ~AI_BLOCKED;
	}
	
	if (!Entity->Enemy)
		return;

	if (BadArea)
	{
		if ((frand() < 0.75f) || (Level.Frame < AttackFinished))
			CurrentMove = &Widow2MoveAttackPreBeam;
		else
			CurrentMove = &Widow2MoveAttackDisrupt;
		return;
	}

	CalcSlots();

	// if we can't see the target, spawn stuff
	if ((AttackState == AS_BLIND) && (SlotsLeft() >= 2))
	{
		CurrentMove = &Widow2MoveSpawn;
		return;
	}

	// accept bias towards spawning
	if (blocked  && (SlotsLeft() >= 2))
	{
		CurrentMove = &Widow2MoveSpawn;
		return;
	}

	float range = RangeFrom(Entity->State.GetOrigin(), Entity->Enemy->State.GetOrigin());

	if (range < 600)
	{
		float luck = frand();
		if (SlotsLeft() >= 2)
		{
			if (luck <= 0.40f)
				CurrentMove = &Widow2MoveAttackPreBeam;
			else if ((luck <= 0.7f) && !(Level.Frame < AttackFinished))
				CurrentMove = &Widow2MoveAttackDisrupt;
			else
				CurrentMove = &Widow2MoveSpawn;
		}
		else
		{
			if ((luck <= 0.50f) || (Level.Frame < AttackFinished))
				CurrentMove = &Widow2MoveAttackPreBeam;
			else
				CurrentMove = &Widow2MoveAttackDisrupt;
		}
	}
	else
	{
		float luck = frand();
		if (SlotsLeft() >= 2)
		{
			if (luck < 0.3f)
				CurrentMove = &Widow2MoveAttackPreBeam;
			else if ((luck < 0.65f) || (Level.Frame < AttackFinished))
				CurrentMove = &Widow2MoveSpawn;
			else
				CurrentMove = &Widow2MoveAttackDisrupt;
		}
		else
		{
			if ((luck < 0.45f) || (Level.Frame < AttackFinished))
				CurrentMove = &Widow2MoveAttackPreBeam;
			else
				CurrentMove = &Widow2MoveAttackDisrupt;
		}
	}
}

void CBlackWidow::AttackBeam ()
{
	CurrentMove = &Widow2MoveAttackBeam;
}

void CBlackWidow::ReAttackBeam ()
{
	AIFlags &= ~AI_MANUAL_STEERING;

	if (IsInFront(Entity, *Entity->Enemy))
	{
		if (frand() <= 0.5)
		{
			if ((frand() < 0.7) || (SlotsLeft() < 2))
				CurrentMove = &Widow2MoveAttackBeam;
			else
				CurrentMove = &Widow2MoveSpawn;
		}
		else
			CurrentMove = &Widow2MoveAttackPostBeam;
	}
	else
		CurrentMove = &Widow2MoveAttackPostBeam;
}

void CBlackWidow::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 50;

	if (Damage < 15)
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
	else if (Damage < 75)
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN2]);
		if ((CvarList[CV_SKILL].Integer() < 3) && (frand() < (0.6f - (0.2f*(CvarList[CV_SKILL].Float())))))
		{
			AIFlags &= ~AI_MANUAL_STEERING;
			CurrentMove = &Widow2MovePain;
		}
	}
	else 
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN3]);
		if ((CvarList[CV_SKILL].Integer() < 3) && (frand() < (0.75f - (0.1f*(CvarList[CV_SKILL].Float())))))
		{
			AIFlags &= ~AI_MANUAL_STEERING;
			CurrentMove = &Widow2MovePain;
		}
	}
}

void CBlackWidow::Dead ()
{
	Entity->GetMins().Set (-70, -70, 0);
	Entity->GetMaxs().Set (70, 70, 80);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->CanTakeDamage = true;
	Entity->NextThink = 0;
	Entity->Link();
}

void CBlackWidow::KillChildren ()
{
	CMonsterEntity *ent = NULL;
	while (1)
	{
		ent = entity_cast<CMonsterEntity>(CC_FindByClassName<IBaseEntity, EF_MONSTER> (ent, "monster_stalker"));
		if(!ent)
			return;
		
		if ((ent->GetInUse()) && (ent->Health > 0))
			ent->TakeDamage (Entity, Entity, vec3fOrigin, Entity->Enemy->State.GetOrigin(), vec3fOrigin, (ent->Health + 1), 0, DAMAGE_NO_KNOCKBACK, MOD_UNKNOWN);
	}
}

void CBlackWidow::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		int clipped = Min<> (Damage, 100);

		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (int n = 0; n < 2; n++)
			ThrowWidowGibLoc (Entity, GameMedia.Gib_Bone[0], clipped, GIB_ORGANIC, vec3fOrigin, false);
		for (int n = 0; n < 3; n++)
			ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMeat, clipped, GIB_ORGANIC, vec3fOrigin, false);
		for (int n = 0; n < 3; n++)
		{
			ThrowWidowGibSized (Entity, ModelIndex("models/monsters/blackwidow2/gib1/tris.md2"), clipped, GIB_METALLIC, vec3fOrigin,
				0, false);
			ThrowWidowGibSized (Entity, ModelIndex("models/monsters/blackwidow2/gib2/tris.md2"), clipped, GIB_METALLIC, vec3fOrigin, 
				SoundIndex ("misc/fhit3.wav"), false);
		}
		for (int n = 0; n < 2; n++)
		{
			ThrowWidowGibSized (Entity, ModelIndex("models/monsters/blackwidow2/gib3/tris.md2"), clipped, GIB_METALLIC, vec3fOrigin, 
				0, false);
			ThrowWidowGibSized (Entity, ModelIndex("models/monsters/blackwidow/gib3/tris.md2"), clipped, GIB_METALLIC, vec3fOrigin, 
				0, false);
		}
		CGibEntity::Spawn (Entity, GameMedia.Gib_Chest, clipped, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_Head[1], clipped, GIB_ORGANIC);
		Entity->IsDead = true;
		return;
	}

	if (Entity->IsDead)
		return;

	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DEATH]);
	Entity->CanTakeDamage = false;
	ExplodeCount = 0;
	KillChildren ();
	QuadFramenum = DoubleFramenum = InvincibleFramenum = 0;
	CurrentMove = &Widow2MoveDeath;
}

bool CBlackWidow::CheckAttack ()
{
	if (!Entity->Enemy)
		return false;

	Powerups ();

	if ((frand() < 0.8f) && (SlotsLeft() >= 2) && (RangeFrom(Entity->State.GetOrigin(), Entity->Enemy->State.GetOrigin()) > 150))
	{
		AIFlags |= AI_BLOCKED;
		AttackState = AS_MISSILE;
		return true;
	}

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
			if ((Entity->Enemy->EntityFlags & EF_PLAYER) && SlotsLeft() >= 2)
			{
				AttackState = AS_BLIND;
				return true;
			}
				
			// PGM - we want them to go ahead and shoot at info_notnulls if they can.
			if (Entity->GetSolid() != SOLID_NOT || tr.Fraction < 1.0)		//PGM
				return false;
		}
	}
	
	EnemyInfront = IsInFront(Entity, *Entity->Enemy);

	ERangeType enemy_range = Range(Entity, *Entity->Enemy);
	IdealYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();

	// melee attack
	if (MeleeDebounceTime < Level.Frame)
	{
		float real_enemy_range = RangeFrom (Entity->State.GetOrigin(), Entity->Enemy->State.GetOrigin());
		if (real_enemy_range < 300)
		{
			vec3f spot1;
			anglef angles = Entity->State.GetAngles().ToVectors ();
			G_ProjectSource (Entity->State.GetOrigin(), offsets[0], angles, spot1);

			vec3f spot2 = Entity->Enemy->State.GetOrigin();
			if (TongueAttackOK(spot1, spot2, 256))
			{
				// melee attack ok

				// be nice in easy mode
				if (CvarList[CV_SKILL].Integer() == 0 && (randomMT()&3) )
					return false;

				AttackState = AS_MELEE;
				return true;
			}
		}
	}
	
	if (Level.Frame < AttackFinished)
		return false;
	
	float chance = 0.0f;
	if (!(AIFlags & AI_STAND_GROUND))
	{
		switch (enemy_range)
		{
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
	if ((frand () < chance) || (Entity->Enemy->GetSolid() == SOLID_NOT))
	{
		AttackState = AS_MISSILE;
		return true;
	}
	return false;
}

bool CBlackWidow::Blocked (float Dist)
{
	return false;
}

/*QUAKED monster_widow2 (1 .5 0) (-70 -70 0) (70 70 144) Ambush Trigger_Spawn Sight
*/
void CBlackWidow::Spawn ()
{
	Sounds[SOUND_PAIN1] = SoundIndex ("widow/bw2pain1.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("widow/bw2pain2.wav");
	Sounds[SOUND_PAIN3] = SoundIndex ("widow/bw2pain3.wav");
	Sounds[SOUND_DEATH] = SoundIndex ("widow/death.wav");
	Sounds[SOUND_SEARCH1] = SoundIndex ("bosshovr/bhvunqv1.wav");
	Sounds[SOUND_TENTACLES_RETRACT] = SoundIndex ("brain/brnatck3.wav");

	Entity->PhysicsType = PHYSICS_STEP;
	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/blackwidow2/tris.md2");
	Entity->GetMins().Set (-70, -70, 0);
	Entity->GetMaxs().Set (70, 70, 144);

	Entity->Health = 2000 + 800 + 1000*(CvarList[CV_SKILL].Integer());
	if (Game.GameMode & GAME_COOPERATIVE)
		Entity->Health += 500*(CvarList[CV_SKILL].Integer());

	Entity->GibHealth = -900;
	Entity->Mass = 2500;

	if (CvarList[CV_SKILL].Integer() == 3)
	{
		PowerArmorType = POWER_ARMOR_SHIELD;
		PowerArmorPower = 750;
	}

	YawSpeed = 30;
	
	AIFlags |= AI_IGNORE_SHOTS;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_MELEE | MF_HAS_SEARCH);
	Entity->Link ();

	CurrentMove = &Widow2MoveStand;	

	// cache in all of the stalker stuff, widow stuff, spawngro stuff, gibs
	SoundIndex ("parasite/parpain1.wav");	
	SoundIndex ("parasite/parpain2.wav");	
	SoundIndex ("parasite/pardeth1.wav");	
	SoundIndex ("parasite/paratck1.wav");
	SoundIndex ("parasite/parsght1.wav");
	SoundIndex ("infantry/melee2.wav");
	SoundIndex ("misc/fhit3.wav");

	SoundIndex ("tank/tnkatck3.wav");
	SoundIndex ("weapons/disrupt.wav");
	SoundIndex ("weapons/disint2.wav");

	ModelIndex ("models/monsters/stalker/tris.md2");
	ModelIndex ("models/items/spawngro2/tris.md2");
	ModelIndex ("models/proj/laser2/tris.md2");
	ModelIndex ("models/proj/disintegrator/tris.md2");

	ModelIndex ("models/monsters/blackwidow/gib1/tris.md2");
	ModelIndex ("models/monsters/blackwidow/gib2/tris.md2");
	ModelIndex ("models/monsters/blackwidow/gib3/tris.md2");
	ModelIndex ("models/monsters/blackwidow/gib4/tris.md2");
	ModelIndex ("models/monsters/blackwidow2/gib1/tris.md2");
	ModelIndex ("models/monsters/blackwidow2/gib2/tris.md2");
	ModelIndex ("models/monsters/blackwidow2/gib3/tris.md2");
	ModelIndex ("models/monsters/blackwidow2/gib4/tris.md2");

	CalcSlots();
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_widow2", CBlackWidow);

//
// Death sequence stuff
//

vec3f WidowVelocityForDamage (int damage)
{
	return vec3f (damage * crand(), damage * crand(), damage * crand() + 200.0f);
}

void CWidowGib::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	GetSolid() = SOLID_NOT;
	Touchable = false;
	State.GetAngles().X = State.GetAngles().Z = 0;
	AngularVelocity.Clear ();

	if (HitSound)
		PlaySound (CHAN_VOICE, HitSound);
}

void ThrowWidowGib (IBaseEntity *self, MediaIndex GibIndex, int damage, int type)
{
	CWidowGib::Spawn (self, GibIndex, damage, type, vec3fOrigin, false, 0, true);
}

void ThrowWidowGibLoc (IBaseEntity *self, MediaIndex GibIndex, int damage, int type, vec3f startpos, bool fade)
{
	CWidowGib::Spawn (self, GibIndex, damage, type, startpos, false, 0, fade);
}

void ThrowWidowGibSized (IBaseEntity *self, MediaIndex GibIndex, int damage, int type, vec3f startpos, MediaIndex hitsound, bool fade)
{
	CWidowGib::Spawn (self, GibIndex, damage, type, startpos, true, hitsound, fade);
}

void CWidowGib::Spawn (IBaseEntity *self, MediaIndex GibIndex, int damage, EGibType type, vec3f startpos, bool sized, MediaIndex hitsound, bool fade)
{
	if (!GibIndex)
		return;

	CWidowGib *gib = JunkList->GetFreeJunk<CWidowGib> ();

	if (!startpos.IsZero())
		gib->State.GetOrigin() = startpos;
	else
	{
		vec3f size = (self->GetSize() * 0.5f);
		vec3f origin = self->GetAbsMin();

		gib->State.GetOrigin().Set (	origin.X + frand() * size.X,
										origin.Y + frand() * size.Y,
										origin.Z + frand() * size.Z);
	}

	gib->GetSolid() = SOLID_NOT;
	gib->State.GetEffects() |= FX_GIB;
	gib->State.GetRenderEffects() |= RF_IR_VISIBLE;

	if (fade)
	{
		// sized gibs last longer
		if (sized)
			gib->NextThink = Level.Frame + 200 + frand()*150;
		else
			gib->NextThink = Level.Frame + 50 + frand()*100;
	}
	else
	{
		// sized gibs last longer
		if (sized)
			gib->NextThink = Level.Frame + 600 + frand()*150;
		else
			gib->NextThink = Level.Frame + 250 + frand()*100;
	}

	float velocityScale = 1.0f;
	gib->PhysicsType = PHYSICS_BOUNCE;

	if (type == GIB_ORGANIC)
	{
		gib->PhysicsType = PHYSICS_TOSS;
		gib->Touchable = true;
		velocityScale = 0.5;
	}

	vec3f vd = WidowVelocityForDamage (damage);
	gib->Velocity = ((self->EntityFlags & EF_PHYSICS) ? entity_cast<IPhysicsEntity>(self)->Velocity : vec3fOrigin) .MultiplyAngles (velocityScale, vd);
	gib->ClipGibVelocity ();

	gib->State.GetModelIndex() = GibIndex;

	if (sized)
	{
		gib->HitSound = hitsound;
		gib->GetSolid() = SOLID_BBOX;
		gib->AngularVelocity.Set (frand()*400, frand()*400, frand()*200);
		if (gib->Velocity.Z < 0)
			gib->Velocity.Z *= -1;
		gib->Velocity.X *= 2;
		gib->Velocity.Y *= 2;
		gib->ClipGibVelocity ();
		gib->Velocity.Z = Max<float> ((350 + (frand()*100.0)), gib->Velocity.Z);
		gib->GravityMultiplier = 0.25f;
		gib->Touchable = true;
		gib->SetOwner(self);

		if (gib->State.GetModelIndex() == ModelIndex ("models/monsters/blackwidow2/gib2/tris.md2"))
		{
			gib->GetMins().Set (-10, -10, 0);
			gib->GetMaxs().Set (10, 10, 10);
		}
		else
		{
			gib->GetMins().Set (-5, -5, 0);
			gib->GetMaxs().Set (5, 5, 5);
		}
	}
	else
	{
		gib->Velocity.X *= 2;
		gib->Velocity.Y *= 2;
		gib->AngularVelocity.Set (frand()*600, frand()*600, frand()*600);
	}

	gib->Link ();
}

void ThrowSmallStuff (IBaseEntity *Entity, vec3f point)
{
	for (int n = 0; n < 2; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMeat, 300, GIB_ORGANIC, point, false);
	ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 300, GIB_METALLIC, point, false);
	ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 100, GIB_METALLIC, point, false);
}

void ThrowMoreStuff (IBaseEntity *Entity, vec3f point)
{
	if (Game.GameMode & GAME_COOPERATIVE)
	{
		ThrowSmallStuff (Entity, point);
		return;
	}

	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMeat, 300, GIB_ORGANIC, point, false);
	for (int n = 0; n < 2; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 300, GIB_METALLIC, point, false);
	for (int n = 0; n < 3; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 100, GIB_METALLIC, point, false);
}

void ThrowArm1 (IBaseEntity *Entity)
{
	vec3f	startpoint;
	static const vec3f offset1 (65.76f, 17.52f, 7.56f);

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), offset1, angles, startpoint);

	CRocketExplosion (CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, Entity->State.GetOrigin()), startpoint).Send();

	for (int n = 0; n < 2; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 100, GIB_METALLIC, startpoint, false);
}

void ThrowArm2 (IBaseEntity *Entity)
{
	vec3f	startpoint;
	static const vec3f offset1 (65.76f, 17.52f, 7.56f);

	anglef angles = Entity->State.GetAngles().ToVectors();
	G_ProjectSource (Entity->State.GetOrigin(), offset1, angles, startpoint);

	ThrowWidowGibSized (Entity, ModelIndex("models/monsters/blackwidow2/gib4/tris.md2"), 200, GIB_METALLIC, startpoint, SoundIndex ("misc/fhit3.wav"), false);
	ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMeat, 300, GIB_ORGANIC, startpoint, false);
}

void CBlackWidow::WidowExplode ()
{
	Think = ConvertDerivedFunction(&CBlackWidow::WidowExplode);

	vec3f org = Entity->State.GetOrigin() + vec3f(0, 0, 24 + irandom(16));

	if (ExplodeCount < 8)
		org.Z += 24 + (irandom(32));

	switch (ExplodeCount)
	{
	case 0:
		org.X -= 24;
		org.Y -= 24;
		break;
	case 1:
		org.X += 24;
		org.Y += 24;
		ThrowSmallStuff(Entity, org);
		break;
	case 2:
		org.X += 24;
		org.Y -= 24;
		break;
	case 3:
		org.X -= 24;
		org.Y += 24;
		ThrowMoreStuff(Entity, org);
		break;
	case 4:
		org.X -= 48;
		org.Y -= 48;
		break;
	case 5:
		org.X += 48;
		org.Y += 48;
		ThrowArm1 (Entity);
		break;
	case 6:
		org.X -= 48;
		org.Y += 48;
		ThrowArm2 (Entity);
		break;
	case 7:
		org.X += 48;
		org.Y -= 48;
		ThrowSmallStuff(Entity, org);
		break;
	case 8:
		org.X += 18;
		org.Y += 18;
		org.Z = Entity->State.GetOrigin().Z + 48;
		ThrowMoreStuff(Entity, org);
		break;
	case 9:
		org.X -= 18;
		org.Y += 18;
		org.Z = Entity->State.GetOrigin().Z + 48;
		break;
	case 10:
		org.X += 18;
		org.Y -= 18;
		org.Z = Entity->State.GetOrigin().Z + 48;
		break;
	case 11:
		org.X -= 18;
		org.Y -= 18;
		org.Z = Entity->State.GetOrigin().Z + 48;
		break;
	case 12:
		Entity->State.GetSound() = 0;
		for (uint8 n = 0; n < 1; n++)
			ThrowWidowGib (Entity, GameMedia.Gib_SmallMeat, 400, GIB_ORGANIC);
		for (uint8 n = 0; n < 2; n++)
			ThrowWidowGib (Entity, GameMedia.Gib_SmallMetal(), 100, GIB_METALLIC);
		for (uint8 n = 0; n < 2; n++)
			ThrowWidowGib (Entity, GameMedia.Gib_SmallMetal(), 400, GIB_METALLIC);

		Entity->IsDead = true;
		Think = &IMonster::MonsterThink;
		Entity->NextThink = Level.Frame + ServerFramesPerSecond;
		CurrentMove = &Widow2MoveDead;
		return;
	}

	ExplodeCount++;
	CRocketExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, Entity->State.GetOrigin()), org).Send();

	Entity->NextThink = Level.Frame + ServerFramesPerSecond;
}

void CBlackWidow::WidowExplosion1 ()
{
	vec3f	startpoint;
	static const vec3f offset1 (23.74f, -37.67f, 76.96f);

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), offset1, angles, startpoint);

	CRocketExplosion (CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, Entity->State.GetOrigin()), startpoint).Send();
	
	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMeat, 300, GIB_ORGANIC, startpoint, false);
	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 100, GIB_METALLIC, startpoint, false);
	for (int n = 0; n < 2; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 300, GIB_METALLIC, startpoint, false);
}

void CBlackWidow::WidowExplosion2 ()
{
	vec3f startpoint;
	static const vec3f	offset1 (-20.49f, 36.92f, 73.52f);

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), offset1, angles, startpoint);

	CRocketExplosion (CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, Entity->State.GetOrigin()), startpoint).Send();
	
	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMeat, 300, GIB_ORGANIC, startpoint, false);
	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 100, GIB_METALLIC, startpoint, false);
	for (int n = 0; n < 2; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 300, GIB_METALLIC, startpoint, false);
}

void CBlackWidow::WidowExplosion3 ()
{
	vec3f	startpoint;
	static const vec3f offset1 (2.11f, 0.05f, 92.20f);

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), offset1, angles, startpoint);

	CRocketExplosion (CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, Entity->State.GetOrigin()), startpoint).Send();
	
	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMeat, 300, GIB_ORGANIC, startpoint, false);
	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 100, GIB_METALLIC, startpoint, false);
	for (int n = 0; n < 2; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 300, GIB_METALLIC, startpoint, false);
}

void CBlackWidow::WidowExplosion4 ()
{
	vec3f startpoint;
	static const vec3f	offset1 (-28.04f, -35.57f, -77.56f);

	anglef angles = Entity->State.GetAngles().ToVectors();
	G_ProjectSource (Entity->State.GetOrigin(), offset1, angles, startpoint);

	CRocketExplosion (CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, Entity->State.GetOrigin()), startpoint).Send();
	
	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMeat, 300, GIB_ORGANIC, startpoint, false);
	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 100, GIB_METALLIC, startpoint, false);
	for (int n = 0; n < 2; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 300, GIB_METALLIC, startpoint, false);
}

void CBlackWidow::WidowExplosion5 ()
{
	vec3f	startpoint;
	static const vec3f offset1 (-20.11f, -1.11f, 40.76f);

	anglef angles = Entity->State.GetAngles().ToVectors();
	G_ProjectSource (Entity->State.GetOrigin(), offset1, angles, startpoint);

	CRocketExplosion (CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, Entity->State.GetOrigin()), startpoint).Send();

	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMeat, 300, GIB_ORGANIC, startpoint, false);
	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 100, GIB_METALLIC, startpoint, false);
	for (int n = 0; n < 2; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 300, GIB_METALLIC, startpoint, false);
}

void CBlackWidow::WidowExplosion6 ()
{
	vec3f	startpoint;
	static const vec3f	offset1 (-20.11f, -1.11f, 40.76f);

	anglef angles = Entity->State.GetAngles().ToVectors();
	G_ProjectSource (Entity->State.GetOrigin(), offset1, angles, startpoint);

	CRocketExplosion (CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, Entity->State.GetOrigin()), startpoint).Send();
	
	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMeat, 300, GIB_ORGANIC, startpoint, false);
	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 100, GIB_METALLIC, startpoint, false);
	for (int n = 0; n < 2; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 300, GIB_METALLIC, startpoint, false);
}

void CBlackWidow::WidowExplosion7 ()
{
	vec3f	startpoint;
	static const vec3f	offset1 (-20.11f, -1.11f, 40.76f);

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), offset1, angles, startpoint);

	CRocketExplosion (CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, Entity->State.GetOrigin()), startpoint).Send();
	
	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMeat, 300, GIB_ORGANIC, startpoint, false);
	for (int n = 0; n < 1; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 100, GIB_METALLIC, startpoint, false);
	for (int n = 0; n < 2; n++)
		ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 300, GIB_METALLIC, startpoint, false);
}

void CBlackWidow::WidowExplosionLeg ()
{
	vec3f	startpoint;
	static const vec3f offset1 (-31.89f, -47.86f, 67.02f);
	static const vec3f offset2 (-44.9f, -82.14f, 54.72f);

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), offset1, angles, startpoint);

	CRocketExplosion (CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, Entity->State.GetOrigin()), startpoint).Send();

	ThrowWidowGibSized (Entity, ModelIndex("models/monsters/blackwidow2/gib2/tris.md2"), 200, GIB_METALLIC, startpoint, SoundIndex ("misc/fhit3.wav"), false);
	ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMeat, 300, GIB_ORGANIC, startpoint, false);
	ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 100, GIB_METALLIC, startpoint, false);

	G_ProjectSource (Entity->State.GetOrigin(), offset2, angles, startpoint);

	CRocketExplosion (CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, Entity->State.GetOrigin()), startpoint).Send();

	ThrowWidowGibSized (Entity, ModelIndex("models/monsters/blackwidow2/gib1/tris.md2"), 300, GIB_METALLIC, startpoint, SoundIndex ("misc/fhit3.wav"), false);
	ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMeat, 300, GIB_ORGANIC, startpoint, false);
	ThrowWidowGibLoc (Entity, GameMedia.Gib_SmallMetal(), 100, GIB_METALLIC, startpoint, false);
}

#endif
