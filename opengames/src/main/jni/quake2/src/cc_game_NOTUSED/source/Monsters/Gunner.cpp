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
// cc_infantry.cpp
// Infantry Monster
//

#include "Local.h"
#include "Monsters/Gunner.h"

CGunner::CGunner (uint32 ID) :
IMonster (ID)
{
}

void CGunner::Idle ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_IDLE], 255, ATTN_IDLE);
}

void CGunner::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

void CGunner::Search ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SEARCH]);
}

CFrame GunnerFramesFidget [] =
{
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CGunner::Idle)),
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
CAnim GunnerMoveFidget (CGunner::FRAME_stand31, CGunner::FRAME_stand70, GunnerFramesFidget, ConvertDerivedFunction(&CGunner::Stand));

void CGunner::Fidget ()
{
	if (AIFlags & AI_STAND_GROUND)
		return;
	if (frand() <= 0.05)
		CurrentMove = &GunnerMoveFidget;
}

CFrame GunnerFramesStand [] =
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
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CGunner::Fidget)),

	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CGunner::Fidget)),

	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CGunner::Fidget))
};
CAnim GunnerMoveStand (CGunner::FRAME_stand01, CGunner::FRAME_stand30, GunnerFramesStand);

void CGunner::Stand ()
{
	CurrentMove = &GunnerMoveStand;
}

CFrame GunnerFramesWalk [] =
{
	CFrame (&IMonster::AI_Walk, 0),
	CFrame (&IMonster::AI_Walk, 3),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 7),
	CFrame (&IMonster::AI_Walk, 2),
	CFrame (&IMonster::AI_Walk, 6),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 2),
	CFrame (&IMonster::AI_Walk, 7),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 7),
	CFrame (&IMonster::AI_Walk, 4)
};
CAnim GunnerMoveWalk (CGunner::FRAME_walk07, CGunner::FRAME_walk19, GunnerFramesWalk);

void CGunner::Walk ()
{
	CurrentMove = &GunnerMoveWalk;
}

CFrame GunnerFramesRun [] =
{
	CFrame (&IMonster::AI_Run, 26),
	CFrame (&IMonster::AI_Run, 9),
	CFrame (&IMonster::AI_Run, 9),
	CFrame (&IMonster::AI_Run, 9),
	CFrame (&IMonster::AI_Run, 15),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 6)
};
CAnim GunnerMoveRun(CGunner::FRAME_run01, CGunner::FRAME_run08, GunnerFramesRun);

void CGunner::Run ()
{
#if ROGUE_FEATURES
	DoneDodge();
#endif
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &GunnerMoveStand : &GunnerMoveRun;
}

CFrame GunnerFramesRunAndShoot [] =
{
	CFrame (&IMonster::AI_Run, 32),
	CFrame (&IMonster::AI_Run, 15),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 18),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 20)
};
CAnim GunnerMoveRunAndShoot (CGunner::FRAME_runs01, CGunner::FRAME_runs06, GunnerFramesRunAndShoot);

void CGunner::RunAndShoot ()
{
	CurrentMove = &GunnerMoveRunAndShoot;
}

CFrame GunnerFramesPain3 [] =
{
	CFrame (&IMonster::AI_Move, -3),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 1)
};
CAnim GunnerMovePain3 (CGunner::FRAME_pain301, CGunner::FRAME_pain305, GunnerFramesPain3, ConvertDerivedFunction(&CGunner::Run));

CFrame GunnerFramesPain2 [] =
{
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, 11),
	CFrame (&IMonster::AI_Move, 6),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -7),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, -7)
};
CAnim GunnerMovePain2 (CGunner::FRAME_pain201, CGunner::FRAME_pain208, GunnerFramesPain2, ConvertDerivedFunction(&CGunner::Run));

CFrame GunnerFramesPain1 [] =
{
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -5),
	CFrame (&IMonster::AI_Move, 3),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim GunnerMovePain1 (CGunner::FRAME_pain101, CGunner::FRAME_pain118, GunnerFramesPain1, ConvertDerivedFunction(&CGunner::Run));

void CGunner::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

#if ROGUE_FEATURES
	DoneDodge();
#endif

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;
	Entity->PlaySound (CHAN_VOICE, (irandom(2)) ? Sounds[SOUND_PAIN1] : Sounds[SOUND_PAIN2]);

	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	CurrentMove = ((Damage <= 10) ? &GunnerMovePain3 : ((Damage <= 25) ? &GunnerMovePain2 : &GunnerMovePain1));

#if ROGUE_FEATURES
	AIFlags &= ~AI_MANUAL_STEERING;

	// PMM - clear duck flag
	if (AIFlags & AI_DUCKED)
		UnDuck();
#endif
}

void CGunner::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame GunnerFramesDeath [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -7),
	CFrame (&IMonster::AI_Move, -3),
	CFrame (&IMonster::AI_Move, -5),
	CFrame (&IMonster::AI_Move, 8),
	CFrame (&IMonster::AI_Move, 6),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim GunnerMoveDeath (CGunner::FRAME_death01, CGunner::FRAME_death11, GunnerFramesDeath, ConvertDerivedFunction(&CGunner::Dead));

void CGunner::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (sint32 n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_Bone[0], Damage, GIB_ORGANIC);
		for (sint32 n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_Head[1], Damage, GIB_ORGANIC);
		Entity->IsDead = true;
		return;
	}

	if (Entity->IsDead == true)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DEATH]);
	Entity->IsDead = true;
	Entity->CanTakeDamage = true;
	CurrentMove = &GunnerMoveDeath;
}

void CGunner::DuckDown ()
{
#if !ROGUE_FEATURES
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	if (CvarList[CV_SKILL].Integer() >= 2)
	{
		if (frand() > 0.5)
			Grenade ();
	}

	Entity->GetMaxs().Z -= 32;
	Entity->CanTakeDamage = true;
	PauseTime = Level.Frame + 10;
	Entity->Link ();
#else
	AIFlags |= AI_DUCKED;
	if (CvarList[CV_SKILL].Integer() >= 2)
	{
		if (frand() > 0.5)
			Grenade ();
	}

	Entity->GetMaxs().Z = BaseHeight - 32;
	Entity->CanTakeDamage = true;
	if (DuckWaitTime < Level.Frame)
		DuckWaitTime = Level.Frame + 10;
	Entity->Link ();
#endif
}

#if !ROGUE_FEATURES
void CGunner::DuckHold ()
{
	if (Level.Frame >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CGunner::DuckUp ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->GetMaxs().Z += 32;
	Entity->CanTakeDamage = true;
	Entity->Link ();
}
#endif

CFrame GunnerFramesDuck [] =
{
	CFrame (&IMonster::AI_Move, 1, ConvertDerivedFunction(&CGunner::DuckDown)),
	CFrame (&IMonster::AI_Move, 1),
#if !ROGUE_FEATURES
	CFrame (&IMonster::AI_Move, 1, ConvertDerivedFunction(&CGunner::DuckHold)),
#else
	CFrame (&IMonster::AI_Move, 1, &IMonster::DuckHold),
#endif
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1),
#if !ROGUE_FEATURES
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CGunner::DuckUp)),
#else
	CFrame (&IMonster::AI_Move, 0, &CGunner::UnDuck),
#endif
	CFrame (&IMonster::AI_Move, -1)
};
CAnim GunnerMoveDuck (CGunner::FRAME_duck01, CGunner::FRAME_duck08, GunnerFramesDuck, ConvertDerivedFunction(&CGunner::Run));

#if !ROGUE_FEATURES
void CGunner::Dodge (IBaseEntity *Attacker, float eta)
{
	if (frand() > 0.25)
		return;

	if (!Entity->Enemy && Entity->Enemy->EntityFlags & EF_HURTABLE)
		Entity->Enemy = Attacker;

	CurrentMove = &GunnerMoveDuck;
}
#endif

void CGunner::OpenGun ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_OPEN], 255, ATTN_IDLE);
}

#if ROGUE_FEATURES
bool CGunner::GrenadeCheck()
{
	if(!Entity->Enemy)
		return false;

	vec3f		start, target, dir;

	// if the player is above my head, use machinegun.

	// check for flag telling us that we're blindfiring
	if (AIFlags & AI_MANUAL_STEERING)
	{
		if ((Entity->State.GetOrigin().Z + Entity->ViewHeight) < BlindFireTarget.Z)
			return false;
	}
	else if(Entity->GetAbsMax().Z <= Entity->Enemy->GetAbsMin().Z)
		return false;

	// check to see that we can trace to the player before we start
	// tossing grenades around.
	anglef angles = Entity->State.GetAngles().ToVectors();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_GUNNER_GRENADE_1], angles, start);

	// pmm - check for blindfire flag
	target = (AIFlags & AI_MANUAL_STEERING) ? BlindFireTarget : Entity->Enemy->State.GetOrigin();

	dir = Entity->State.GetOrigin() - target;

	if (dir.Length() < 100)
		return false;

	CTrace tr (start, target, Entity, CONTENTS_MASK_SHOT);
	if(tr.Entity == Entity->Enemy || tr.Fraction == 1)
		return true;

	return false;
}
#endif

void CGunner::Fire ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start, target, aim;
	sint32		flash_number = MZ2_GUNNER_MACHINEGUN_1 + (Entity->State.GetFrame() - CGunner::FRAME_attak216);

	anglef angles = Entity->State.GetAngles().ToVectors();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flash_number], angles, start);

	// project enemy back a bit and target there
	target = Entity->Enemy->State.GetOrigin();
	if (Entity->Enemy->EntityFlags & EF_PHYSICS)
		target = target.MultiplyAngles (-0.2f, entity_cast<IPhysicsEntity>(*Entity->Enemy)->Velocity);
	target.Z += Entity->Enemy->ViewHeight;

	aim = (target - start).GetNormalizedFast();
	MonsterFireBullet (start, aim, 3, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, flash_number);
}

void CGunner::Grenade ()
{
	if (!HasValidEnemy())
		return;

#if !ROGUE_FEATURES
	vec3f	start;
	sint32		flash_number;

	switch (Entity->State.GetFrame())
	{
	case CGunner::FRAME_attak105:
		flash_number = MZ2_GUNNER_GRENADE_1;
		break;
	case CGunner::FRAME_attak108:
		flash_number = MZ2_GUNNER_GRENADE_2;
		break;
	case CGunner::FRAME_attak111:
		flash_number = MZ2_GUNNER_GRENADE_3;
		break;
	default:
		flash_number = MZ2_GUNNER_GRENADE_4;
		break;
	}

	anglef angles = Entity->State.GetAngles().ToVectors();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flash_number], angles, start);

	MonsterFireGrenade (start, angle.Forward, 50, 600, flash_number);
#else
	vec3f	start, aim;
	sint32		flash_number;
	float	spread;
	float	pitch = 0;
	// PMM
	vec3f	target;	

	if(!Entity->Enemy || !Entity->Enemy->GetInUse())		//PGM
		return;									//PGM

	switch (Entity->State.GetFrame())
	{
	case CGunner::FRAME_attak105:
		flash_number = MZ2_GUNNER_GRENADE_1;
		spread = .02f;
		break;
	case CGunner::FRAME_attak108:
		flash_number = MZ2_GUNNER_GRENADE_2;
		spread = .05f;
		break;
	case CGunner::FRAME_attak111:
		flash_number = MZ2_GUNNER_GRENADE_3;
		spread = .08f;
		break;
	default:
		flash_number = MZ2_GUNNER_GRENADE_4;
		AIFlags &= ~AI_MANUAL_STEERING;
		spread = .11f;
		break;
	}

	//	pmm
	// if we're shooting blind and we still can't see our enemy
	if ((AIFlags & AI_MANUAL_STEERING) && (!IsVisible(Entity, *Entity->Enemy)))
	{
		// and we have a valid blind_fire_target
		if (BlindFireTarget == vec3fOrigin)
			return;

		target = BlindFireTarget;
	}
	else
		target = Entity->State.GetOrigin();
	// pmm

	anglef angles = Entity->State.GetAngles().ToVectors();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flash_number], angles, start);

//PGM
	if(Entity->Enemy)
	{
		aim = target - Entity->State.GetOrigin();

		// aim up if they're on the same level as me and far away.
		if((aim.Length() > 512) && (aim.Z < 64) && (aim.Z > -64))
			aim.Z += (aim.Length() - 512);

		aim.NormalizeFast ();
		pitch = aim.Z;
		if(pitch > 0.4f)
			pitch = 0.4f;
		else if(pitch < -0.5f)
			pitch = -0.5f;
	}
//PGM

	aim = (angles.Forward.MultiplyAngles (spread, angles.Right)).MultiplyAngles (pitch, angles.Up);
	MonsterFireGrenade (start, aim, 50, 600, flash_number);
#endif
}

CFrame GunnerFramesAttackChain [] =
{
	/*
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	*/
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::OpenGun)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim GunnerMoveAttackChain (CGunner::FRAME_attak209, CGunner::FRAME_attak215, GunnerFramesAttackChain, ConvertDerivedFunction(&CGunner::FireChain));

CFrame GunnerFramesFireChain [] =
{
	CFrame (&IMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&IMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&IMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&IMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&IMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&IMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&IMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&IMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire))
};
CAnim GunnerMoveFireChain (CGunner::FRAME_attak216, CGunner::FRAME_attak223, GunnerFramesFireChain, ConvertDerivedFunction(&CGunner::ReFireChain));

CFrame GunnerFramesEndFireChain [] =
{
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim GunnerMoveEndFireChain (CGunner::FRAME_attak224, CGunner::FRAME_attak230, GunnerFramesEndFireChain, ConvertDerivedFunction(&CGunner::Run));

#if ROGUE_FEATURES
void CGunner::BlindCheck ()
{
	if (AIFlags & AI_MANUAL_STEERING)
		IdealYaw = (BlindFireTarget - Entity->State.GetOrigin()).ToYaw();
}
#endif

CFrame GunnerFramesAttackGrenade [] =
{
#if ROGUE_FEATURES
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::BlindCheck)),
#else
	CFrame (&IMonster::AI_Charge, 0),
#endif
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::Grenade)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::Grenade)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::Grenade)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::Grenade)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim GunnerMoveAttackGrenade (CGunner::FRAME_attak101, CGunner::FRAME_attak121, GunnerFramesAttackGrenade, ConvertDerivedFunction(&CGunner::Run));

void CGunner::Attack()
{
#if !ROGUE_FEATURES
	if (Range (Entity, *Entity->Enemy) == RANGE_MELEE)
		CurrentMove = &GunnerMoveAttackChain;
	else
		CurrentMove = (frand() <= 0.5) ? &GunnerMoveAttackGrenade : &GunnerMoveAttackChain;
#else
	DoneDodge();

	// PMM 
	if (AttackState == AS_BLIND)
	{
		// minimum of 2 seconds, plus 0-3, after the shots are done
		BlindFireDelay += 2.1 + 2.0 + frand()*3.0;

		// don't shoot at the origin
		if (BlindFireTarget == vec3fOrigin)
			return;

		// don't shoot if the dice say not to
		float r = frand();
		if (BlindFireDelay < 7.5 && (r > 0.4f))
			return;
		else if (r > 0.1f)
			return;


		// turn on manual steering to signal both manual steering and blindfire
		AIFlags |= AI_MANUAL_STEERING;
		if (GrenadeCheck())
		{
			// if the check passes, go for the attack
			CurrentMove = &GunnerMoveAttackGrenade;
			AttackFinished = Level.Frame + ((2*frand())*10);
		}
		// turn off blindfire flag
		AIFlags &= ~AI_MANUAL_STEERING;
		return;
	}
	// pmm

	// PGM - gunner needs to use his chaingun if he's being attacked by a tesla.
	if (Range (Entity, *Entity->Enemy) == RANGE_MELEE)
		CurrentMove = &GunnerMoveAttackChain;
	else
		CurrentMove = (frand() <= 0.5 && GrenadeCheck()) ? &GunnerMoveAttackGrenade : &GunnerMoveAttackChain;
#endif
}

void CGunner::FireChain ()
{
	CurrentMove = &GunnerMoveFireChain;
}

void CGunner::ReFireChain ()
{
	if (Entity->Enemy && Entity->Enemy->EntityFlags & EF_HURTABLE && entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0 && IsVisible (Entity, *Entity->Enemy) && frand() <= 0.5)
	{
		CurrentMove = &GunnerMoveFireChain;
		return;
	}
	CurrentMove = &GunnerMoveEndFireChain;
}

#if ROGUE_FEATURES
void CGunner::Duck (float eta)
{
	if ((CurrentMove == &GunnerMoveAttackChain) ||
		(CurrentMove == &GunnerMoveFireChain) ||
		(CurrentMove == &GunnerMoveAttackGrenade))
	{
		// if we're shooting, and not on easy, don't dodge
		if (CvarList[CV_SKILL].Integer())
		{
			AIFlags &= ~AI_DUCKED;
			return;
		}
	}

	if (CvarList[CV_SKILL].Integer() == 0)
		// PMM - stupid dodge
		DuckWaitTime = Level.Frame + ((eta + 1) * 10);
	else
		DuckWaitTime = Level.Frame + ((eta + (0.1 * (3 - CvarList[CV_SKILL].Integer()))) * 10);

	// has to be done immediately otherwise he can get stuck
	DuckDown();

	NextFrame = CGunner::FRAME_duck01;
	CurrentMove = &GunnerMoveDuck;
	return;
}

void CGunner::SideStep ()
{
	if ((CurrentMove == &GunnerMoveAttackChain) ||
		(CurrentMove == &GunnerMoveFireChain) ||
		(CurrentMove == &GunnerMoveAttackGrenade))
	{
		// if we're shooting, and not on easy, don't dodge
		if (CvarList[CV_SKILL].Integer())
		{
			AIFlags &= ~AI_DODGING;
			return;
		}
	}

	if (CurrentMove != &GunnerMoveRun)
		CurrentMove = &GunnerMoveRun;
}
#endif

void CGunner::Spawn ()
{
	Sounds[SOUND_DEATH] = SoundIndex ("gunner/death1.wav");	
	Sounds[SOUND_PAIN1] = SoundIndex ("gunner/gunpain2.wav");	
	Sounds[SOUND_PAIN2] = SoundIndex ("gunner/gunpain1.wav");	
	Sounds[SOUND_IDLE] = SoundIndex ("gunner/gunidle1.wav");	
	Sounds[SOUND_OPEN] = SoundIndex ("gunner/gunatck1.wav");	
	Sounds[SOUND_SEARCH] = SoundIndex ("gunner/gunsrch1.wav");	
	Sounds[SOUND_SIGHT] = SoundIndex ("gunner/sight1.wav");	

	SoundIndex ("gunner/gunatck2.wav");
	SoundIndex ("gunner/gunatck3.wav");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/gunner/tris.md2");
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, 32);

	Entity->Health = 175;
	Entity->GibHealth = -70;
	Entity->Mass = 200;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_SIGHT
#if ROGUE_FEATURES
		| MF_HAS_DODGE | MF_HAS_DUCK | MF_HAS_UNDUCK | MF_HAS_SIDESTEP
#endif
		);
	Entity->Link ();

#if ROGUE_FEATURES
	BlindFire = true;
#endif

	CurrentMove = &GunnerMoveStand;	
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_gunner", CGunner);
