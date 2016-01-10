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
// cc_brain.cpp
// 
//

#include "Local.h"
#include "Monsters/Brain.h"

CBrain::CBrain (uint32 ID) :
IMonster (ID)
{
}

void CBrain::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

void CBrain::Search ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SEARCH]);
}

//
// STAND
//

CFrame BrainFramesStand [] =
{
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),

	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),

	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0)
};
CAnim BrainMoveStand (CBrain::FRAME_stand01, CBrain::FRAME_stand30, BrainFramesStand);

void CBrain::Stand ()
{
	CurrentMove = &BrainMoveStand;
}


//
// IDLE
//

CFrame BrainFramesIdle [] =
{
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),

	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),

	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0),
	CFrame (&IMonster::AI_Stand,	0)
};
CAnim BrainMoveIdle (CBrain::FRAME_stand31, CBrain::FRAME_stand60, BrainFramesIdle, &IMonster::Stand);

void CBrain::Idle ()
{
	Entity->PlaySound (CHAN_AUTO, Sounds[SOUND_IDLE3], 255, ATTN_IDLE);
	CurrentMove = &BrainMoveIdle;
}

//
// WALK
//
CFrame BrainFramesWalk1 [] =
{
	CFrame (&IMonster::AI_Walk,	7),
	CFrame (&IMonster::AI_Walk,	2),
	CFrame (&IMonster::AI_Walk,	3),
	CFrame (&IMonster::AI_Walk,	3),
	CFrame (&IMonster::AI_Walk,	1),
	CFrame (&IMonster::AI_Walk,	0),
	CFrame (&IMonster::AI_Walk,	0),
	CFrame (&IMonster::AI_Walk,	9),
	CFrame (&IMonster::AI_Walk,	-4),
	CFrame (&IMonster::AI_Walk,	-1),
	CFrame (&IMonster::AI_Walk,	2)
};
CAnim BrainMoveWalk1 (CBrain::FRAME_walk101, CBrain::FRAME_walk111, BrainFramesWalk1);

void CBrain::Walk ()
{
	//CurrentMove = (frand() <= 0.5) ? &BrainMoveWalk1 : &BrainMoveWalk2;
	CurrentMove = &BrainMoveWalk1;
}

CFrame BrainFramesDefense [] =
{
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
CAnim BrainMoveDefense (CBrain::FRAME_defens01, CBrain::FRAME_defens08, BrainFramesDefense);

CFrame BrainFramesPain3 [] =
{
	CFrame (&IMonster::AI_Move,	-2),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	3),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	-4)
};
CAnim BrainMovePain3 (CBrain::FRAME_pain301, CBrain::FRAME_pain306, BrainFramesPain3, &IMonster::Run);

CFrame BrainFramesPain2 [] =
{
	CFrame (&IMonster::AI_Move,	-2),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	3),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	-2)
};
CAnim BrainMovePain2 (CBrain::FRAME_pain201, CBrain::FRAME_pain208, BrainFramesPain2, &IMonster::Run);

CFrame BrainFramesPain1 [] =
{
	CFrame (&IMonster::AI_Move,	-6),
	CFrame (&IMonster::AI_Move,	-2),
	CFrame (&IMonster::AI_Move,	-6),
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
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	7),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	3),
	CFrame (&IMonster::AI_Move,	-1)
};
CAnim BrainMovePain1 (CBrain::FRAME_pain101, CBrain::FRAME_pain121, BrainFramesPain1, &IMonster::Run);

void CBrain::Pain(IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;
	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	switch (irandom(3))
	{
	case 0:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
		CurrentMove = &BrainMovePain1;
		break;
	case 1:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN2]);
		CurrentMove = &BrainMovePain2;
		break;
	case 2:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
		CurrentMove = &BrainMovePain3;
		break;
	}
}

//
// DUCK
//

CFrame BrainFramesDuck [] =
{
	CFrame (&IMonster::AI_Move,	0),
#if !ROGUE_FEATURES
	CFrame (&IMonster::AI_Move,	-2,	ConvertDerivedFunction(&CBrain::Duck_Down)),
	CFrame (&IMonster::AI_Move,	17,	ConvertDerivedFunction(&CBrain::Duck_Hold)),
	CFrame (&IMonster::AI_Move,	-3),
	CFrame (&IMonster::AI_Move,	-1,	ConvertDerivedFunction(&CBrain::Duck_Up)),
#else
	CFrame (&IMonster::AI_Move,	-2,	&IMonster::DuckDown),
	CFrame (&IMonster::AI_Move,	17,	&IMonster::DuckHold),
	CFrame (&IMonster::AI_Move,	-3),
	CFrame (&IMonster::AI_Move,	-1,	&IMonster::UnDuck),
#endif
	CFrame (&IMonster::AI_Move,	-5),
	CFrame (&IMonster::AI_Move,	-6),
	CFrame (&IMonster::AI_Move,	-6)
};
CAnim BrainMoveDuck (CBrain::FRAME_duck01, CBrain::FRAME_duck08, BrainFramesDuck, &IMonster::Run);

#if !ROGUE_FEATURES
void CBrain::Duck_Down ()
{
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	Entity->GetMins().Z -= 32;
	Entity->Link ();
}

void CBrain::Duck_Hold ()
{
	if (Level.Frame >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CBrain::Duck_Up ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->GetMins().Z += 32;
	Entity->Link ();
}

void CBrain::Dodge (IBaseEntity *Attacker, float eta)
{
	if (frand() > 0.25f)
		return;

	if (!Entity->Enemy && Entity->Enemy->EntityFlags & EF_HURTABLE)
		Entity->Enemy = Attacker;

	PauseTime = Level.Frame + ((eta + 0.5) * 10);
	CurrentMove = &BrainMoveDuck;
}
#else
void CBrain::Duck (float eta)
{
	// has to be done immediately otherwise he can get stuck
	DuckDown ();

	if (!CvarList[CV_SKILL].Boolean())
		// PMM - stupid dodge
		DuckWaitTime = Level.Frame + ((eta + 1) * 10);
	else
		DuckWaitTime = Level.Frame + ((eta + (0.1 * (3 - CvarList[CV_SKILL].Integer()))) * 10);

	CurrentMove = &BrainMoveDuck;
	NextFrame = CBrain::FRAME_duck01;
	return;
}
#endif

CFrame BrainFramesDeath2 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	9),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim BrainMoveDeath2 (CBrain::FRAME_death201, CBrain::FRAME_death205, BrainFramesDeath2, ConvertDerivedFunction(&CBrain::Dead));

CFrame BrainFramesDeath1 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	-2),
	CFrame (&IMonster::AI_Move,	9),
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
	CFrame (&IMonster::AI_Move,	0)
};
CAnim BrainMoveDeath1 (CBrain::FRAME_death101, CBrain::FRAME_death118, BrainFramesDeath1, ConvertDerivedFunction(&CBrain::Dead));

void CBrain::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

void CBrain::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	Entity->State.GetEffects() = 0;
	PowerArmorType = POWER_ARMOR_NONE;

// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (sint32 n = 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_Bone[0], Damage, GIB_ORGANIC);
		for (sint32 n = 0; n < 4; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_Head[1], Damage, GIB_ORGANIC);
		return;
	}

	if (Entity->IsDead)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DEATH]);
	Entity->IsDead = true;
	CurrentMove = (frand() <= 0.5) ? &BrainMoveDeath1 : &BrainMoveDeath2;
}

//
// MELEE
//

void CBrain::SwingRight ()
{
	Entity->PlaySound (CHAN_BODY, Sounds[SOUND_MELEE1]);
}

void CBrain::HitRight ()
{
	const vec3f	aim (MELEE_DISTANCE, Entity->GetMaxs().X, 8);

	if (CMeleeWeapon::Fire (Entity, aim, (15 + (irandom(5))), 40))
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_MELEE3]);
}

void CBrain::SwingLeft ()
{
	Entity->PlaySound (CHAN_BODY, Sounds[SOUND_MELEE2]);
}

void CBrain::HitLeft ()
{
	const vec3f	aim (MELEE_DISTANCE, Entity->GetMins().X, 8);

	if (CMeleeWeapon::Fire (Entity, aim, (15 + (irandom(5))), 40))
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_MELEE3]);
}

CFrame BrainFramesAttack1 [] =
{
	CFrame (&IMonster::AI_Charge,	8),
	CFrame (&IMonster::AI_Charge,	3),
	CFrame (&IMonster::AI_Charge,	5),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	-3,	ConvertDerivedFunction(&CBrain::SwingRight)),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	-5),
	CFrame (&IMonster::AI_Charge,	-7,	ConvertDerivedFunction(&CBrain::HitRight)),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	6,	ConvertDerivedFunction(&CBrain::SwingLeft)),
	CFrame (&IMonster::AI_Charge,	1),
	CFrame (&IMonster::AI_Charge,	2,	ConvertDerivedFunction(&CBrain::HitLeft)),
	CFrame (&IMonster::AI_Charge,	-3),
	CFrame (&IMonster::AI_Charge,	6),
	CFrame (&IMonster::AI_Charge,	-1),
	CFrame (&IMonster::AI_Charge,	-3),
	CFrame (&IMonster::AI_Charge,	2),
	CFrame (&IMonster::AI_Charge,	-11)
};
CAnim BrainMoveAttack1 (CBrain::FRAME_attak101, CBrain::FRAME_attak118, BrainFramesAttack1, &IMonster::Run);

void CBrain::ChestOpen ()
{
	Refire = false;
	PowerArmorType = POWER_ARMOR_NONE;
	Entity->PlaySound (CHAN_BODY, Sounds[SOUND_CHEST_OPEN]);
}

void CBrain::TentacleAttack ()
{
	static const vec3f aim (MELEE_DISTANCE, 0, 8);

	if (CMeleeWeapon::Fire (Entity, aim, (10 + (irandom(5))), -600) && (CvarList[CV_SKILL].Boolean()))
		Refire = true;
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_TENTACLES_RETRACT]);
}

void CBrain::ChestClosed ()
{
	PowerArmorType = POWER_ARMOR_SCREEN;
	if (Refire)
	{
		Refire = false;
		CurrentMove = &BrainMoveAttack1;
	}
}

CFrame BrainFramesAttack2 [] =
{
	CFrame (&IMonster::AI_Charge,	5),
	CFrame (&IMonster::AI_Charge,	-4),
	CFrame (&IMonster::AI_Charge,	-4),
	CFrame (&IMonster::AI_Charge,	-3),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBrain::ChestOpen)),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	13,	ConvertDerivedFunction(&CBrain::TentacleAttack)),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	2),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	-9,	ConvertDerivedFunction(&CBrain::ChestClosed)),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	4),
	CFrame (&IMonster::AI_Charge,	3),
	CFrame (&IMonster::AI_Charge,	2),
	CFrame (&IMonster::AI_Charge,	-3),
	CFrame (&IMonster::AI_Charge,	-6)
};
CAnim BrainMoveAttack2 (CBrain::FRAME_attak201, CBrain::FRAME_attak217, BrainFramesAttack2, &IMonster::Run);

void CBrain::Melee ()
{
	CurrentMove = (frand() <= 0.5) ? &BrainMoveAttack1 : &BrainMoveAttack2;
}

#if XATRIX_FEATURES
static bool BrainTongueAttackCheck (vec3f &start, vec3f &end)
{
	vec3f dir = start - end;

	// check for max distance
	if (dir.Length() > 512)
		return false;

	// check for min/max pitch
	vec3f angles = dir.ToAngles();
	if (angles.X < -180)
		angles.X += 360;
	if (fabsf(angles.X) > 30)
		return false;

	return true;
}

#include "Utility/TemporaryEntities.h"

void CBrain::TongueAttack ()
{
	static const vec3f offset (24, 0, 16);
	vec3f	start = Entity->State.GetOrigin();

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), offset, angles, start);

	vec3f end = Entity->Enemy->State.GetOrigin();
	if (!BrainTongueAttackCheck(start, end))
	{
		end.Z = Entity->Enemy->State.GetOrigin().Z + Entity->Enemy->GetMaxs().Z - 8;
		if (!BrainTongueAttackCheck(start, end))
		{
			end[2] = Entity->Enemy->State.GetOrigin().Z + Entity->Enemy->GetMins().Z + 8;
			if (!BrainTongueAttackCheck(start, end))
				return;
		}
	}
	end = Entity->Enemy->State.GetOrigin();

	CTrace tr (start, end, Entity, CONTENTS_MASK_SHOT);
	if (tr.Entity != Entity->Enemy)
		return;

	static const int damage = 5;
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_TENTACLES_RETRACT]);

	CFleshCable(start, end, Entity->State.GetNumber()).Send();

	vec3f dir = start - end;
	entity_cast<IHurtableEntity>(*Entity->Enemy)->TakeDamage (Entity, Entity, dir, Entity->Enemy->State.GetOrigin(), vec3fOrigin, damage, 0, DAMAGE_NO_KNOCKBACK, MOD_UNKNOWN);

	// pull the enemy in
	Entity->Enemy->State.GetOrigin().Z += 1;
	entity_cast<IPhysicsEntity>(*Entity->Enemy)->Velocity = angles.Forward * -1200;;
}

// Brian right eye center
const vec3f brain_reye [11] =
{
	vec3f ( 0.746700f, 0.238370f, 34.167690f ),
	vec3f ( -1.076390f, 0.238370f, 33.386372f ),
	vec3f ( -1.335500f, 5.334300f, 32.177170f ),
	vec3f ( -0.175360f, 8.846370f, 30.635479f ),
	vec3f ( -2.757590f, 7.804610f, 30.150860f ),
	vec3f ( -5.575090f, 5.152840f, 30.056160f ),
	vec3f ( -7.017550f, 3.262470f, 30.552521f ),
	vec3f ( -7.915740f, 0.638800f, 33.176189f ),
	vec3f ( -3.915390f, 8.285730f, 33.976349f ),
	vec3f ( -0.913540f, 10.933030f, 34.141811f ),
	vec3f ( -0.369900f, 8.923900f, 34.189079f )
};

// Brain left eye center
const vec3f brain_leye [11] =
{
	vec3f ( -3.364710f, 0.327750f, 33.938381f ),
	vec3f ( -5.140450f, 0.493480f, 32.659851f ),
	vec3f ( -5.341980f, 5.646980f, 31.277901f ),
	vec3f ( -4.134480f, 9.277440f, 29.925621f ),
	vec3f ( -6.598340f, 6.815090f, 29.322620f ),
	vec3f ( -8.610840f, 2.529650f, 29.251591f ),
	vec3f ( -9.231360f, 0.093280f, 29.747959f ),
	vec3f ( -11.004110f, 1.936930f, 32.395260f ),
	vec3f ( -7.878310f, 7.648190f, 33.148151f ),
	vec3f ( -4.947370f, 11.430050f, 33.313610f ),
	vec3f ( -4.332820f, 9.444570f, 33.526340f )
};

// note to self
// need to get an x,y,z offset for
// each frame of the run cycle
void CBrain::LaserBeamFire ()
{
	if (!HasValidEnemy())
		return;

	// RAFAEL
	// cant call sound this frequent
	if (frand() > 0.8)
		Entity->PlaySound (CHAN_AUTO, SoundIndex("misc/lasfly.wav"), 255, ATTN_STATIC);

	vec3f angles = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToAngles();
	
	// dis is my right eye
	CMonsterBeamLaser *Laser = QNewEntityOf CMonsterBeamLaser;
	Laser->State.GetOrigin() = Entity->State.GetOrigin();
	anglef angleVecs = angles.ToVectors ();
	Laser->State.GetAngles() = angles;
	Laser->State.GetOrigin() = Laser->State.GetOrigin().
		MultiplyAngles (brain_reye[Entity->State.GetFrame() - CBrain::FRAME_walk101].X, angleVecs.Right).
		MultiplyAngles (brain_reye[Entity->State.GetFrame() - CBrain::FRAME_walk101].Y, angleVecs.Forward).
		MultiplyAngles (brain_reye[Entity->State.GetFrame() - CBrain::FRAME_walk101].Z, angleVecs.Up);
	
	Laser->Enemy = Entity->Enemy;
	Laser->SetOwner(Entity);
	Laser->Damage = 1;
	MonsterFireBeam (Laser);
   
	// dis is me left eye
	Laser = QNewEntityOf CMonsterBeamLaser;
	Laser->State.GetOrigin() = Entity->State.GetOrigin();
	Laser->State.GetAngles() = angles;
	Laser->State.GetOrigin() = Laser->State.GetOrigin().
		MultiplyAngles (brain_leye[Entity->State.GetFrame() - CBrain::FRAME_walk101].X, angleVecs.Right).
		MultiplyAngles (brain_leye[Entity->State.GetFrame() - CBrain::FRAME_walk101].Y, angleVecs.Forward).
		MultiplyAngles (brain_leye[Entity->State.GetFrame() - CBrain::FRAME_walk101].Z, angleVecs.Up);

	Laser->Enemy = Entity->Enemy;
	Laser->SetOwner(Entity);
	Laser->Damage = 1;
	MonsterFireBeam (Laser);
}

void CBrain::LaserBeamRefire ()
{
	if (frand() < 0.5 && IsVisible (Entity, *Entity->Enemy) && entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0)
		Entity->State.GetFrame() = CBrain::FRAME_walk101;
}

CFrame BrainFramesAttack3 [] =
{
	CFrame (&IMonster::AI_Charge, 5),
	CFrame (&IMonster::AI_Charge, -4),
	CFrame (&IMonster::AI_Charge, -4),
	CFrame (&IMonster::AI_Charge, -3),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CBrain::ChestOpen)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CBrain::TongueAttack)),
	CFrame (&IMonster::AI_Charge, 13),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CBrain::TentacleAttack)),
	CFrame (&IMonster::AI_Charge, 2),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CBrain::TongueAttack)),
	CFrame (&IMonster::AI_Charge, -9,	ConvertDerivedFunction(&CBrain::ChestClosed)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 4),
	CFrame (&IMonster::AI_Charge, 3),
	CFrame (&IMonster::AI_Charge, 2),
	CFrame (&IMonster::AI_Charge, -3),
	CFrame (&IMonster::AI_Charge, -6)
};
CAnim BrainMoveAttack3 (CBrain::FRAME_attak201, CBrain::FRAME_attak217, BrainFramesAttack3, &IMonster::Run);

CFrame BrainFramesAttack4 [] =
{
	CFrame (&IMonster::AI_Charge, 9,	ConvertDerivedFunction(&CBrain::LaserBeamFire)),
	CFrame (&IMonster::AI_Charge, 2,	ConvertDerivedFunction(&CBrain::LaserBeamFire)),
	CFrame (&IMonster::AI_Charge, 3,	ConvertDerivedFunction(&CBrain::LaserBeamFire)),
	CFrame (&IMonster::AI_Charge, 3,	ConvertDerivedFunction(&CBrain::LaserBeamFire)),
	CFrame (&IMonster::AI_Charge, 1,	ConvertDerivedFunction(&CBrain::LaserBeamFire)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CBrain::LaserBeamFire)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CBrain::LaserBeamFire)),
	CFrame (&IMonster::AI_Charge, 10,	ConvertDerivedFunction(&CBrain::LaserBeamFire)),
	CFrame (&IMonster::AI_Charge, -4,	ConvertDerivedFunction(&CBrain::LaserBeamFire)),
	CFrame (&IMonster::AI_Charge, -1,	ConvertDerivedFunction(&CBrain::LaserBeamFire)),
	CFrame (&IMonster::AI_Charge, 2,	ConvertDerivedFunction(&CBrain::LaserBeamRefire))
};
CAnim BrainMoveAttack4 (CBrain::FRAME_walk101, CBrain::FRAME_walk111, BrainFramesAttack4, &IMonster::Run);

void CBrain::Attack ()
{	
	if (frand() < 0.8)
	{
		ERangeType r = Range (Entity, *Entity->Enemy);

		switch (r)
		{
		case RANGE_NEAR:
			CurrentMove = (frand() <= 0.5) ? &BrainMoveAttack3 : &BrainMoveAttack4;
			break;
		case RANGE_MID:
		case RANGE_FAR:
			CurrentMove = &BrainMoveAttack4;
			break;
		}
	}
}
#endif

//
// RUN
//

CFrame BrainFramesRun [] =
{
	CFrame (&IMonster::AI_Run,	9),
	CFrame (&IMonster::AI_Run,	2),
	CFrame (&IMonster::AI_Run,	3),
	CFrame (&IMonster::AI_Run,	3),
	CFrame (&IMonster::AI_Run,	1),
	CFrame (&IMonster::AI_Run,	0),
	CFrame (&IMonster::AI_Run,	0),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	-4),
	CFrame (&IMonster::AI_Run,	-1),
	CFrame (&IMonster::AI_Run,	2)
};
CAnim BrainMoveRun (CBrain::FRAME_walk101, CBrain::FRAME_walk111, BrainFramesRun);

void CBrain::Run ()
{
	PowerArmorType = POWER_ARMOR_SCREEN;
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &BrainMoveStand : &BrainMoveRun;
}

/*QUAKED monster_brain (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void CBrain::Spawn ()
{
	Sounds[SOUND_CHEST_OPEN] = SoundIndex ("brain/brnatck1.wav");
	Sounds[SOUND_TENTACLES_EXTEND] = SoundIndex ("brain/brnatck2.wav");
	Sounds[SOUND_TENTACLES_RETRACT] = SoundIndex ("brain/brnatck3.wav");
	Sounds[SOUND_DEATH] = SoundIndex ("brain/brndeth1.wav");
	Sounds[SOUND_IDLE1] = SoundIndex ("brain/brnidle1.wav");
	Sounds[SOUND_IDLE2] = SoundIndex ("brain/brnidle2.wav");
	Sounds[SOUND_IDLE3] = SoundIndex ("brain/brnlens1.wav");
	Sounds[SOUND_PAIN1] = SoundIndex ("brain/brnpain1.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("brain/brnpain2.wav");
	Sounds[SOUND_SIGHT] = SoundIndex ("brain/brnsght1.wav");
	Sounds[SOUND_SEARCH] = SoundIndex ("brain/brnsrch1.wav");
	Sounds[SOUND_MELEE1] = SoundIndex ("brain/melee1.wav");
	Sounds[SOUND_MELEE2] = SoundIndex ("brain/melee2.wav");
	Sounds[SOUND_MELEE3] = SoundIndex ("brain/melee3.wav");

	Entity->PhysicsType = PHYSICS_STEP;
	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/brain/tris.md2");
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, 32);

	Entity->Health = 300;
	Entity->GibHealth = -150;
	Entity->Mass = 400;

	MonsterFlags |= (MF_HAS_MELEE | MF_HAS_SIGHT | MF_HAS_SEARCH | MF_HAS_IDLE
#if ROGUE_FEATURES
		| MF_HAS_DODGE | MF_HAS_DUCK | MF_HAS_UNDUCK
#endif
#if XATRIX_FEATURES
		| MF_HAS_ATTACK
#endif
		);

	PowerArmorType = POWER_ARMOR_SCREEN;
	PowerArmorPower = 100;

	Entity->Link ();

	CurrentMove = &BrainMoveStand;	
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_brain", CBrain);
