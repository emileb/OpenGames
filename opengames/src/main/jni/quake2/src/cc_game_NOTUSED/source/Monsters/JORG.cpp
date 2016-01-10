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
// cc_jorg.cpp
// 
//

#include "Local.h"
#include "Monsters/Makron.h"
#include "Monsters/JORG.h"

CJorg::CJorg (uint32 ID) :
IMonster (ID)
{
}

void CJorg::Search ()
{
	switch (irandom(3))
	{
	case 0:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SEARCH1]);
		break;
	case 1:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SEARCH2]);
		break;
	case 2:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SEARCH3]);
		break;
	};
}

//
// stand
//

CFrame JorgFramesStand []=
{
	CFrame (&IMonster::AI_Stand, 0, &IMonster::Idle),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),		// 10
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),		// 20
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),		// 30
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 19),
	CFrame (&IMonster::AI_Stand, 11, ConvertDerivedFunction(&CJorg::StepLeft)),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 6),
	CFrame (&IMonster::AI_Stand, 9, ConvertDerivedFunction(&CJorg::StepRight)),
	CFrame (&IMonster::AI_Stand, 0),		// 40
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, -2),
	CFrame (&IMonster::AI_Stand, -17, ConvertDerivedFunction(&CJorg::StepLeft)),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, -12),		// 50
	CFrame (&IMonster::AI_Stand, -14, ConvertDerivedFunction(&CJorg::StepRight))	// 51
};
CAnim	JorgMoveStand (CJorg::FRAME_stand01, CJorg::FRAME_stand51, JorgFramesStand);

void CJorg::Idle ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_IDLE]);
}

void CJorg::DeathHit ()
{
	Entity->PlaySound (CHAN_BODY, Sounds[SOUND_DEATH_HIT]);
}

void CJorg::StepLeft ()
{
	Entity->PlaySound (CHAN_BODY, Sounds[SOUND_STEPLEFT]);
}

void CJorg::StepRight ()
{
	Entity->PlaySound (CHAN_BODY, Sounds[SOUND_STEPRIGHT]);
}

void CJorg::Stand ()
{
	CurrentMove = &JorgMoveStand;
}

CFrame JorgFramesRun [] =
{
	CFrame (&IMonster::AI_Run, 17,	ConvertDerivedFunction(&CJorg::StepLeft)),
	CFrame (&IMonster::AI_Run, 0),
	CFrame (&IMonster::AI_Run, 0),
	CFrame (&IMonster::AI_Run, 0),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 33,	ConvertDerivedFunction(&CJorg::StepRight)),
	CFrame (&IMonster::AI_Run, 0),
	CFrame (&IMonster::AI_Run, 0),
	CFrame (&IMonster::AI_Run, 0),
	CFrame (&IMonster::AI_Run, 9),
	CFrame (&IMonster::AI_Run, 9),
	CFrame (&IMonster::AI_Run, 9)
};
CAnim	JorgMoveRun (CJorg::FRAME_walk06, CJorg::FRAME_walk19, JorgFramesRun);

//
// walk
//

CFrame JorgFramesWalk [] =
{
	CFrame (&IMonster::AI_Walk, 17),
	CFrame (&IMonster::AI_Walk, 0),
	CFrame (&IMonster::AI_Walk, 0),
	CFrame (&IMonster::AI_Walk, 0),
	CFrame (&IMonster::AI_Walk, 12),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 10),
	CFrame (&IMonster::AI_Walk, 33),
	CFrame (&IMonster::AI_Walk, 0),
	CFrame (&IMonster::AI_Walk, 0),
	CFrame (&IMonster::AI_Walk, 0),
	CFrame (&IMonster::AI_Walk, 9),
	CFrame (&IMonster::AI_Walk, 9),
	CFrame (&IMonster::AI_Walk, 9)
};
CAnim	JorgMoveWalk (CJorg::FRAME_walk06, CJorg::FRAME_walk19, JorgFramesWalk);

void CJorg::Walk ()
{
	CurrentMove = &JorgMoveWalk;
}

void CJorg::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &JorgMoveStand : &JorgMoveRun;
}

CFrame JorgFramesPain3 [] =
{
	CFrame (&IMonster::AI_Move,	-28),
	CFrame (&IMonster::AI_Move,	-6),
	CFrame (&IMonster::AI_Move,	-3,	ConvertDerivedFunction(&CJorg::StepLeft)),
	CFrame (&IMonster::AI_Move,	-9),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CJorg::StepRight)),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	-7),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	-11),
	CFrame (&IMonster::AI_Move,	-4),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	10),
	CFrame (&IMonster::AI_Move,	11),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	10),
	CFrame (&IMonster::AI_Move,	3),
	CFrame (&IMonster::AI_Move,	10),
	CFrame (&IMonster::AI_Move,	7,	ConvertDerivedFunction(&CJorg::StepLeft)),
	CFrame (&IMonster::AI_Move,	17),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CJorg::StepRight))
};
CAnim JorgMovePain3 (CJorg::FRAME_pain301, CJorg::FRAME_pain325, JorgFramesPain3, &IMonster::Run);

CFrame JorgFramesPain2 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim JorgMovePain2 (CJorg::FRAME_pain201, CJorg::FRAME_pain203, JorgFramesPain2, &IMonster::Run);

CFrame JorgFramesPain1 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim JorgMovePain1 (CJorg::FRAME_pain101, CJorg::FRAME_pain103, JorgFramesPain1, &IMonster::Run);

void CJorg::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
			Entity->State.GetSkinNum() = 1;
	
	Entity->State.GetSound() = 0;

	if (Level.Frame < PainDebounceTime)
			return;

	// Lessen the chance of him going into his pain frames if he takes little damage
	if ((Damage <= 40) && (frand() <= 0.6))
		return;

	/* 
	If he's entering his attack1 or using attack1, lessen the chance of him
	going into pain
	*/
	
	sint32 frame = Entity->State.GetFrame();
	if (((frame >= CJorg::FRAME_attak101) && (frame <= CJorg::FRAME_attak108)) && (frand() <= 0.005))
		return;

	if (((frame >= CJorg::FRAME_attak109) && (frame <= CJorg::FRAME_attak114)) && (frand() <= 0.00005))
		return;

	if (((frame >= CJorg::FRAME_attak201) && (frame <= CJorg::FRAME_attak208)) && (frand() <= 0.005))
		return;

	PainDebounceTime = Level.Frame + 30;
	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	if (Damage <= 50)
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
		CurrentMove = &JorgMovePain1;
	}
	else if (Damage <= 100)
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN2]);
		CurrentMove = &JorgMovePain2;
	}
	else if (frand() <= 0.3)
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN3]);
		CurrentMove = &JorgMovePain3;
	}
};

CFrame JorgFramesDeath1 [] =
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
	CFrame (&IMonster::AI_Move,	0),		// 10
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),		// 20
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),			
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),			
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),		// 30
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),		// 40
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),			
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,  ConvertDerivedFunction(&CJorg::DeathHit)),			
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CJorg::TossMakron)),
	CFrame (&IMonster::AI_Move,	0,	&IMonster::BossExplode)		// 50
};
CAnim JorgMoveDeath (CJorg::FRAME_death01, CJorg::FRAME_death50, JorgFramesDeath1, NULL);

void CJorg::TossMakron ()
{
	CMakronJumpTimer::Spawn (this);
};

void CJorg::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DEATH]);
	Entity->IsDead = true;
	Entity->CanTakeDamage = false;
	Entity->State.GetSound() = 0;
	ExplodeCount = 0;
	CurrentMove = &JorgMoveDeath;
}

CFrame JorgFramesAttack2 []=
{
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBFG)),		
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim JorgMoveAttack2 (CJorg::FRAME_attak201, CJorg::FRAME_attak213, JorgFramesAttack2, &IMonster::Run);

CFrame JorgFramesStartAttack1 [] =
{
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0)
};
CAnim JorgMoveStartAttack1 (CJorg::FRAME_attak101, CJorg::FRAME_attak108, JorgFramesStartAttack1, ConvertDerivedFunction(&CJorg::DoChainguns));

CFrame JorgFramesAttack1[]=
{
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBullet)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBullet)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBullet)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBullet)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBullet)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBullet))
};
CAnim JorgMoveAttack1 (CJorg::FRAME_attak109, CJorg::FRAME_attak114, JorgFramesAttack1, ConvertDerivedFunction(&CJorg::ReAttack1));

CFrame JorgFramesEndAttack1[]=
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim JorgMoveEndAttack1 (CJorg::FRAME_attak115, CJorg::FRAME_attak118, JorgFramesEndAttack1, &IMonster::Run);

void CJorg::ReAttack1()
{
	if (IsVisible(Entity, *Entity->Enemy))
	{
		if (frand() < 0.9)
			CurrentMove = &JorgMoveAttack1;
		else
		{
			Entity->State.GetSound() = 0;
			CurrentMove = &JorgMoveEndAttack1;	
		}
	}
	else
	{
		Entity->State.GetSound() = 0;
		CurrentMove = &JorgMoveEndAttack1;	
	}
}

void CJorg::DoChainguns()
{
	CurrentMove = &JorgMoveAttack1;
}

void CJorg::FireBFG ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start;
	vec3f	dir;
	vec3f	vec;

	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_JORG_BFG_1], Entity->State.GetAngles().ToVectors(), start);

	dir = ((Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight)) - start).GetNormalized();
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_ATTACK2]);
	MonsterFireBfg (start, dir, 50, 300, 100, 200, MZ2_JORG_BFG_1);
}

void CJorg::FireBullet ()
{
	if (!HasValidEnemy())
		return;

	vec3f	forward, target;
	vec3f	start;
	anglef angles = Entity->State.GetAngles().ToVectors();

	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_JORG_MACHINEGUN_R1], angles, start);

	target = Entity->Enemy->State.GetOrigin().MultiplyAngles(-0.2f, entity_cast<IPhysicsEntity>(*Entity->Enemy)->Velocity);
	target[2] += Entity->Enemy->ViewHeight;
	forward = (target - start);
	forward.Normalize();

	MonsterFireBullet (start, forward, 6, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MZ2_JORG_MACHINEGUN_R1);

	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_JORG_MACHINEGUN_L1], angles, start);

	target = Entity->Enemy->State.GetOrigin().MultiplyAngles(-0.2f, entity_cast<IPhysicsEntity>(*Entity->Enemy)->Velocity);
	target[2] += Entity->Enemy->ViewHeight;
	forward = (target - start);
	forward.Normalize();

	MonsterFireBullet (start, forward, 6, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MZ2_JORG_MACHINEGUN_L1);
}

void CJorg::Attack()
{
	if (frand() <= 0.75)
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_ATTACK1]);
		Entity->State.GetSound() = SoundIndex ("boss3/w_loop.wav");
		CurrentMove = &JorgMoveStartAttack1;
	}
	else
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_ATTACK2]);
		CurrentMove = &JorgMoveAttack2;
	}
}

bool CJorg::CheckAttack ()
{
#if !ROGUE_FEATURES
	float	chance;

	if (entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0)
	{
	// see if any entities are in the way of the shot
		vec3f spot1 = Entity->State.GetOrigin();
		spot1.Z += Entity->ViewHeight;
		vec3f spot2 = Entity->Enemy->State.GetOrigin();
		spot2.Z += Entity->Enemy->ViewHeight;

		CTrace tr (spot1, spot2, Entity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.Entity != Entity->Enemy)
			return false;
	}
	
	// melee attack
	if (EnemyRange == RANGE_MELEE)
	{
		// don't always melee in easy mode
		if (CvarList[CV_SKILL].Integer() == 0 && (randomMT()&3) )
			return false;
		if (MonsterFlags & MF_HAS_MELEE)
			AttackState = AS_MELEE;
		else
			AttackState = AS_MISSILE;
		return true;
	}
	
// missile attack
	if (!(MonsterFlags & MF_HAS_ATTACK))
		return false;
		
	if (Level.Frame < AttackFinished)
		return false;
		
	if (EnemyRange == RANGE_FAR)
		return false;

	if (AIFlags & AI_STAND_GROUND)
	{
		chance = 0.4f;
	}
	else if (EnemyRange == RANGE_MELEE)
	{
		chance = 0.8f;
	}
	else if (EnemyRange == RANGE_NEAR)
	{
		chance = 0.4f;
	}
	else if (EnemyRange == RANGE_MID)
	{
		chance = 0.2f;
	}
	else
	{
		return false;
	}

	if (CvarList[CV_SKILL].Integer() == 0)
		chance *= 0.5;
	else if (CvarList[CV_SKILL].Integer() >= 2)
		chance *= 2;

	if (frand () < chance)
	{
		AttackState = AS_MISSILE;
		AttackFinished = Level.Frame + ((2*frand())*10);
		return true;
	}

	if (AIFlags & AI_FLY)
	{
		if (frand() < 0.3)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

	return false;
#else
	float	chance;

	if (entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0)
	{
		// see if any entities are in the way of the shot
		vec3f spot1 = Entity->State.GetOrigin ();
		spot1.Z += Entity->ViewHeight;
		vec3f spot2 = Entity->Enemy->State.GetOrigin();
		spot2.Z += Entity->Enemy->ViewHeight;

		CTrace tr (spot1, spot2, Entity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.Entity != Entity->Enemy)
		{	
			// PGM - we want them to go ahead and shoot at info_notnulls if they can.
			if(Entity->Enemy->GetSolid() != SOLID_NOT || tr.Fraction < 1.0)		//PGM
			{
				// PMM - if we can't see our target, and we're not blocked by a monster, go into blind fire if available
				if ((!(tr.Entity->GetSvFlags() & SVF_MONSTER)) && (!IsVisible(Entity, *Entity->Enemy)))
				{
					if ((BlindFire) && (BlindFireDelay <= 20.0))
					{
						if (Level.Frame < AttackFinished)
							return false;
						if (Level.Frame < (TrailTime + BlindFireDelay))
							// wait for our time
							return false;
						else
						{
							// make sure we're not going to shoot a monster
							tr (spot1, BlindFireTarget, Entity, CONTENTS_MONSTER);
							if (tr.AllSolid || tr.StartSolid || ((tr.Fraction < 1.0) && (tr.Entity != Entity->Enemy)))
								return false;

							AttackState = AS_BLIND;
							return true;
						}
					}
				}
				// pmm
				return false;
			}
		}
	}
	
	// melee attack
	if (EnemyRange == RANGE_MELEE)
	{
		// don't always melee in easy mode
		if (CvarList[CV_SKILL].Integer() == 0 && (randomMT()&3) )
		{
			// PMM - fix for melee only monsters & strafing
			AttackState = AS_STRAIGHT;
			return false;
		}
		if (MonsterFlags & MF_HAS_MELEE)
			AttackState = AS_MELEE;
		else
			AttackState = AS_MISSILE;
		return true;
	}
	
// missile attack
	if (!(MonsterFlags & MF_HAS_ATTACK))
	{
		// PMM - fix for melee only monsters & strafing
		AttackState = AS_STRAIGHT;
		return false;
	}
	
	if (Level.Frame < AttackFinished)
		return false;
		
	if (EnemyRange == RANGE_FAR)
		return false;

	if (AIFlags & AI_STAND_GROUND)
		chance = 0.4f;
	else if (EnemyRange == RANGE_MELEE)
		chance = 0.8f;
	else if (EnemyRange == RANGE_NEAR)
		chance = 0.4f;
	else if (EnemyRange == RANGE_MID)
		chance = 0.2f;
	else
		return false;

	if (CvarList[CV_SKILL].Integer() == 0)
		chance *= 0.5;
	else if (CvarList[CV_SKILL].Integer() >= 2)
		chance *= 2;

	// PGM - go ahead and shoot every time if it's a info_notnull
	if ((frand () < chance) || (Entity->Enemy->GetSolid() == SOLID_NOT))
	{
		AttackState = AS_MISSILE;
		AttackFinished = Level.Frame + ((2*frand())*10);
		return true;
	}

	// PMM -daedalus should strafe more .. this can be done here or in a customized
	// check_attack code for the hover.
	if (AIFlags & AI_FLY)
	{
		// originally, just 0.3
		float strafe_chance = 0.6f;

		// if enemy is tesla, never strafe
		if ((Entity->Enemy) && (!Entity->Enemy->ClassName.IsNullOrEmpty()) && (!strcmp(Entity->Enemy->ClassName.CString(), "tesla")))
			strafe_chance = 0;

		if (frand() < strafe_chance)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}
// do we want the monsters strafing?
	else
	{
		if (frand() < 0.4)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

//-PMM

	return false;
#endif
}

/*QUAKED monster_jorg (1 .5 0) (-80 -80 0) (90 90 140) Ambush Trigger_Spawn Sight
*/
void CJorg::Spawn ()
{
	Sounds[SOUND_PAIN1] = SoundIndex ("boss3/bs3pain1.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("boss3/bs3pain2.wav");
	Sounds[SOUND_PAIN3] = SoundIndex ("boss3/bs3pain3.wav");
	Sounds[SOUND_DEATH] = SoundIndex ("boss3/bs3deth1.wav");
	Sounds[SOUND_ATTACK1] = SoundIndex ("boss3/bs3atck1.wav");
	Sounds[SOUND_ATTACK2] = SoundIndex ("boss3/bs3atck2.wav");
	Sounds[SOUND_SEARCH1] = SoundIndex ("boss3/bs3srch1.wav");
	Sounds[SOUND_SEARCH2] = SoundIndex ("boss3/bs3srch2.wav");
	Sounds[SOUND_SEARCH3] = SoundIndex ("boss3/bs3srch3.wav");
	Sounds[SOUND_IDLE] = SoundIndex ("boss3/bs3idle1.wav");
	Sounds[SOUND_STEPLEFT] = SoundIndex ("boss3/step1.wav");
	Sounds[SOUND_STEPRIGHT] = SoundIndex ("boss3/step2.wav");
	Sounds[SOUND_FIREGUN] = SoundIndex ("boss3/xfire.wav");
	Sounds[SOUND_DEATH_HIT] = SoundIndex ("boss3/d_hit.wav");

	CMakron::Precache ();

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex(2) = ModelIndex ("models/monsters/boss3/rider/tris.md2");
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/boss3/jorg/tris.md2");
	Entity->GetMins().Set (-80, -80, 0);
	Entity->GetMaxs().Set (80, 80, 140);

	Entity->Health = 3000;
	Entity->GibHealth = -2000;
	Entity->Mass = 1000;

	Entity->Link();
	MonsterFlags = (MF_HAS_ATTACK | MF_HAS_SEARCH);
	
	CurrentMove = &JorgMoveStand;

	WalkMonsterStart();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_jorg", CJorg);
