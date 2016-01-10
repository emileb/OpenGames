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

You may use any part of this code to help create your own bases and own mods off
this code if you wish. It is under the same license as Quake 2 source (as above),
therefore you are free to have to fun with it. All I ask is you email me so I can
list the mod on my page for CleanCode Quake2 to help get the word around. Thanks.
*/

//
// cc_boss2.cpp
// Biggun Protector
//

#include "Local.h"
#include "Monsters/Boss2.h"

CBoss2::CBoss2 (uint32 ID) :
IMonster (ID)
{
};

void CBoss2::Search ()
{
	if (frand() < 0.5)
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SEARCH], 255, ATTN_NONE);
}

void CBoss2::FireRocket ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start;

	anglef angles = Entity->State.GetAngles().ToVectors ();

//1
	vec3f origin = Entity->State.GetOrigin();

	G_ProjectSource (origin, MonsterFlashOffsets[MZ2_BOSS2_ROCKET_1], angles, start);
	vec3f vec = Entity->Enemy->State.GetOrigin();
	vec.Z += Entity->ViewHeight;
	vec3f dir = vec - start;
	dir.Normalize();
	MonsterFireRocket (start, dir, 50, 500, MZ2_BOSS2_ROCKET_1);

//2
	G_ProjectSource (origin, MonsterFlashOffsets[MZ2_BOSS2_ROCKET_2], angles, start);
	vec = Entity->Enemy->State.GetOrigin();
	vec.Z += Entity->ViewHeight;
	dir = vec - start;
	dir.Normalize();
	MonsterFireRocket (start, dir, 50, 500, MZ2_BOSS2_ROCKET_2);

//3
	G_ProjectSource (origin, MonsterFlashOffsets[MZ2_BOSS2_ROCKET_3], angles, start);
	vec = Entity->Enemy->State.GetOrigin();
	vec.Z += Entity->ViewHeight;
	dir = vec - start;
	dir.Normalize();
	MonsterFireRocket (start, dir, 50, 500, MZ2_BOSS2_ROCKET_3);

//4
	G_ProjectSource (origin, MonsterFlashOffsets[MZ2_BOSS2_ROCKET_4], angles, start);
	vec = Entity->Enemy->State.GetOrigin();
	vec.Z += Entity->ViewHeight;
	dir = vec - start;
	dir.Normalize();
	MonsterFireRocket (start, dir, 50, 500, MZ2_BOSS2_ROCKET_4);
}	

void CBoss2::FireBulletRight ()
{
	if (!HasValidEnemy())
		return;

	vec3f	target;
	vec3f	start;

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_BOSS2_MACHINEGUN_R1], angles, start);

	vec3f tempTarget = Entity->Enemy->State.GetOrigin();
	if (Entity->Enemy->EntityFlags & EF_PHYSICS)
		tempTarget = tempTarget.MultiplyAngles (-0.2f, entity_cast<IPhysicsEntity>(*Entity->Enemy)->Velocity);
	target = tempTarget;
	target.Z += Entity->Enemy->ViewHeight;
	angles.Forward = target - start;
	angles.Forward.Normalize();

	MonsterFireBullet (start, angles.Forward, 6, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MZ2_BOSS2_MACHINEGUN_R1);
}	

void CBoss2::FireBulletLeft ()
{
	if (!HasValidEnemy())
		return;

	vec3f	target, start;

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_BOSS2_MACHINEGUN_R1], angles, start);

	vec3f tempTarget = Entity->Enemy->State.GetOrigin();
	if (Entity->Enemy->EntityFlags & EF_PHYSICS)
		tempTarget = tempTarget.MultiplyAngles (-0.2f, entity_cast<IPhysicsEntity>(*Entity->Enemy)->Velocity);
	target = tempTarget;
	target.Z += Entity->Enemy->ViewHeight;
	angles.Forward = target - start;
	angles.Forward.Normalize();

	MonsterFireBullet (start, angles.Forward, 6, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MZ2_BOSS2_MACHINEGUN_L1);
}	

void CBoss2::MachineGun ()
{
	FireBulletLeft();
	FireBulletRight();
}	


CFrame Boss2FramesStand [] =
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
CAnim Boss2MoveStand (CBoss2::FRAME_stand30, CBoss2::FRAME_stand50, Boss2FramesStand);

CFrame Boss2FramesFidget [] =
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
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim Boss2MoveFidget (CBoss2::FRAME_stand1, CBoss2::FRAME_stand30, Boss2FramesFidget);

CFrame Boss2FramesWalk [] =
{
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8)
};
CAnim Boss2MoveWalk (CBoss2::FRAME_walk1, CBoss2::FRAME_walk20, Boss2FramesWalk);

CFrame Boss2FramesRun [] =
{
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8)
};
CAnim Boss2MoveRun (CBoss2::FRAME_walk1, CBoss2::FRAME_walk20, Boss2FramesRun);

CFrame Boss2FramesAttackPreMg [] =
{
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1, ConvertDerivedFunction(&CBoss2::AttackMg))
};
CAnim Boss2MoveAttackPreMg (CBoss2::FRAME_attack1, CBoss2::FRAME_attack9, Boss2FramesAttackPreMg);

// Loop this
CFrame Boss2FramesAttackMg [] =
{
	CFrame (&IMonster::AI_Charge, 1,	ConvertDerivedFunction(&CBoss2::MachineGun)),
	CFrame (&IMonster::AI_Charge, 1,	ConvertDerivedFunction(&CBoss2::MachineGun)),
	CFrame (&IMonster::AI_Charge, 1,	ConvertDerivedFunction(&CBoss2::MachineGun)),
	CFrame (&IMonster::AI_Charge, 1,	ConvertDerivedFunction(&CBoss2::MachineGun)),
	CFrame (&IMonster::AI_Charge, 1,	ConvertDerivedFunction(&CBoss2::MachineGun)),
	CFrame (&IMonster::AI_Charge, 1,	ConvertDerivedFunction(&CBoss2::ReAttackMg))
};
CAnim Boss2MoveAttackMg (CBoss2::FRAME_attack10, CBoss2::FRAME_attack15, Boss2FramesAttackMg);

CFrame Boss2FramesAttackPostMg [] =
{
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1)
};
CAnim Boss2MoveAttackPostMg (CBoss2::FRAME_attack16, CBoss2::FRAME_attack19, Boss2FramesAttackPostMg, &IMonster::Run);

CFrame Boss2FramesAttackRocket [] =
{
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Move, -20,	ConvertDerivedFunction(&CBoss2::FireRocket)),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1)
};
CAnim Boss2MoveAttackRocket (CBoss2::FRAME_attack20, CBoss2::FRAME_attack40, Boss2FramesAttackRocket, &IMonster::Run);

CFrame Boss2FramesPainHeavy [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim Boss2MovePainHeavy (CBoss2::FRAME_pain2, CBoss2::FRAME_pain19, Boss2FramesPainHeavy, &IMonster::Run);

CFrame Boss2FramesPainLight [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim Boss2MovePainLight (CBoss2::FRAME_pain20, CBoss2::FRAME_pain23, Boss2FramesPainLight, &IMonster::Run);

CFrame Boss2FramesDeath [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0,	&IMonster::BossExplode)
};
CAnim Boss2MoveDeath (CBoss2::FRAME_death2, CBoss2::FRAME_death50, Boss2FramesDeath, ConvertDerivedFunction(&CBoss2::Dead));

void CBoss2::Stand ()
{
	CurrentMove = &Boss2MoveStand;
}

void CBoss2::Run ()
{
	if (AIFlags & AI_STAND_GROUND)
		CurrentMove = &Boss2MoveStand;
	else
		CurrentMove = &Boss2MoveRun;
}

void CBoss2::Walk ()
{
	CurrentMove = &Boss2MoveWalk;
}

void CBoss2::Attack ()
{
	CurrentMove = ((Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).Length() <= 125) ?
		&Boss2MoveAttackPreMg : ((frand() <= 0.6) ? &Boss2MoveAttackPreMg : &Boss2MoveAttackRocket);
}

void CBoss2::AttackMg ()
{
	CurrentMove = &Boss2MoveAttackMg;
}

void CBoss2::ReAttackMg ()
{
	CurrentMove = (IsInFront(Entity, *Entity->Enemy)) ?
		((frand() <= 0.7) ? &Boss2MoveAttackMg : &Boss2MoveAttackPostMg) : &Boss2MoveAttackPostMg;
}

void CBoss2::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;

// American wanted these at no attenuation
	Entity->PlaySound (CHAN_VOICE, (Damage < 10) ? Sounds[SOUND_PAIN3] : ((Damage < 30) ? Sounds[SOUND_PAIN1] : Sounds[SOUND_PAIN2]), 255, ATTN_NONE);
	CurrentMove = (Damage < 30) ? &Boss2MovePainLight : &Boss2MovePainHeavy;
}

// Immune to lasers
void CBoss2::TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker,
						vec3f Dir, vec3f Point, vec3f Normal, sint32 Damage,
						sint32 Knockback, EDamageFlags DamageFlags, EMeansOfDeath MeansOfDeath)
{
	if (MeansOfDeath == MOD_TARGET_LASER)
		return;

	IMonster::TakeDamage (Inflictor, Attacker, Dir, Point, Normal, Damage, Knockback, DamageFlags, MeansOfDeath);
}

void CBoss2::Dead ()
{
	Entity->GetMins().Set (-56, -56, 0);
	Entity->GetMaxs().Set (56, 56, 80);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

void CBoss2::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DEATH], 255, ATTN_NONE);
	Entity->IsDead = true;
	Entity->CanTakeDamage = false;
	ExplodeCount = 0;
	CurrentMove = &Boss2MoveDeath;
}

bool CBoss2::CheckAttack ()
{
	vec3f	temp;
	float	chance;
	bool	enemy_infront;
	ERangeType		enemy_range;
	float	enemy_yaw;

	if (entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0)
	{
	// see if any entities are in the way of the shot
		vec3f spot1 = Entity->State.GetOrigin();
		spot1.Z += Entity->ViewHeight;
		vec3f spot2 = Entity->Enemy->State.GetOrigin();
		spot2.Z += Entity->Enemy->ViewHeight;

		CTrace tr (spot1, spot2, Entity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA);

		// do we have a clear shot?
		if (tr.Entity != Entity->Enemy)
			return false;
	}
	
	enemy_infront = IsInFront(Entity, *Entity->Enemy);
	enemy_range = Range(Entity, *Entity->Enemy);
	temp = Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin();
	enemy_yaw = temp.ToYaw();

	IdealYaw = enemy_yaw;

	// melee attack
	if (enemy_range == RANGE_MELEE)
	{
		AttackState = AS_MISSILE;
		return true;
	}
	
// missile attack
	if (Level.Frame < AttackFinished)
		return false;
		
	if (enemy_range == RANGE_FAR)
		return false;

	if (AIFlags & AI_STAND_GROUND)
		chance = 0.4f;
	else if (enemy_range == RANGE_MELEE)
		chance = 0.8f;
	else if (enemy_range == RANGE_NEAR)
		chance = 0.8f;
	else if (enemy_range == RANGE_MID)
		chance = 0.8f;
	else
		return false;

	if (frand () < chance)
	{
		AttackState = AS_MISSILE;
		AttackFinished = Level.Frame + (2*frand())*10;
		return true;
	}

	AttackState = (frand() < 0.3) ? AS_SLIDING : AS_STRAIGHT;
	return false;
}

/*QUAKED monster_boss2 (1 .5 0) (-56 -56 0) (56 56 80) Ambush Trigger_Spawn Sight
*/
void CBoss2::Spawn ()
{
	Sounds[SOUND_PAIN1] = SoundIndex ("bosshovr/bhvpain1.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("bosshovr/bhvpain2.wav");
	Sounds[SOUND_PAIN3] = SoundIndex ("bosshovr/bhvpain3.wav");
	Sounds[SOUND_DEATH] = SoundIndex ("bosshovr/bhvdeth1.wav");
	Sounds[SOUND_SEARCH] = SoundIndex ("bosshovr/bhvunqv1.wav");

	Entity->State.GetSound() = SoundIndex ("bosshovr/bhvengn1.wav");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/boss2/tris.md2");
	Entity->GetMins().Set (-56, -56, 0);
	Entity->GetMaxs().Set (56, 56, 80);

	Entity->Health = 2000;
	Entity->GibHealth = -200;
	Entity->Mass = 1000;
	
	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_SEARCH);
	Entity->Link ();

	CurrentMove = &Boss2MoveStand;

	FlyMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_boss2", CBoss2);
