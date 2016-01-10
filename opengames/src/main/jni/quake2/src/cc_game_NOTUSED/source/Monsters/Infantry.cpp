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
#include "Monsters/Infantry.h"

CInfantry::CInfantry (uint32 ID) :
IMonster (ID)
{
}

CFrame InfantryFramesStand [] =
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
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim InfantryMoveStand (CInfantry::FRAME_stand50, CInfantry::FRAME_stand71, InfantryFramesStand);

void CInfantry::Stand ()
{
	CurrentMove = &InfantryMoveStand;
}

CFrame InfantryFramesFidget [] =
{
	CFrame (&IMonster::AI_Stand, 1),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 1),
	CFrame (&IMonster::AI_Stand, 3),
	CFrame (&IMonster::AI_Stand, 6),
	CFrame (&IMonster::AI_Stand, 3),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 1),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 1),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, -1),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 1),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, -2),
	CFrame (&IMonster::AI_Stand, 1),
	CFrame (&IMonster::AI_Stand, 1),
	CFrame (&IMonster::AI_Stand, 1),
	CFrame (&IMonster::AI_Stand, -1),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, -1),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, -1),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 1),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, -1),
	CFrame (&IMonster::AI_Stand, -1),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, -3),
	CFrame (&IMonster::AI_Stand, -2),
	CFrame (&IMonster::AI_Stand, -3),
	CFrame (&IMonster::AI_Stand, -3),
	CFrame (&IMonster::AI_Stand, -2)
};
CAnim InfantryMoveFidget (CInfantry::FRAME_stand01, CInfantry::FRAME_stand49, InfantryFramesFidget, &IMonster::Stand);

void CInfantry::Idle ()
{
	CurrentMove = &InfantryMoveFidget;
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_IDLE], 255, ATTN_IDLE);
}

CFrame InfantryFramesWalk [] =
{
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 6),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 5)
};
CAnim InfantryMoveWalk (CInfantry::FRAME_walk03, CInfantry::FRAME_walk14, InfantryFramesWalk);

void CInfantry::Walk ()
{
	CurrentMove = &InfantryMoveWalk;
}

CFrame InfantryFramesRun [] =
{
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 20),
	CFrame (&IMonster::AI_Run, 5),
	CFrame (&IMonster::AI_Run, 7),
	CFrame (&IMonster::AI_Run, 30),
	CFrame (&IMonster::AI_Run, 35),
	CFrame (&IMonster::AI_Run, 2),
	CFrame (&IMonster::AI_Run, 6)
};
CAnim InfantryMoveRun (CInfantry::FRAME_run01, CInfantry::FRAME_run08, InfantryFramesRun);

void CInfantry::Run ()
{
	if (AIFlags & AI_STAND_GROUND)
		CurrentMove = &InfantryMoveStand;
	else
		CurrentMove = &InfantryMoveRun;
}

CFrame InfantryFramesPain1 [] =
{
	CFrame (&IMonster::AI_Move, -3),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 6),
	CFrame (&IMonster::AI_Move, 2)
};
CAnim InfantryMovePain1 (CInfantry::FRAME_pain101, CInfantry::FRAME_pain110, InfantryFramesPain1, ConvertDerivedFunction(&CInfantry::Run));

CFrame InfantryFramesPain2 [] =
{
	CFrame (&IMonster::AI_Move, -3),
	CFrame (&IMonster::AI_Move, -3),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 5),
	CFrame (&IMonster::AI_Move, 2)
};
CAnim InfantryMovePain2 (CInfantry::FRAME_pain201, CInfantry::FRAME_pain210, InfantryFramesPain2, ConvertDerivedFunction(&CInfantry::Run));

void CInfantry::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

#if ROGUE_FEATURES
	DoneDodge();
#endif

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;
	
	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	CurrentMove = (!irandom(2)) ? &InfantryMovePain1 : &InfantryMovePain2;
	Entity->PlaySound (CHAN_VOICE, (!irandom(2)) ? Sounds[SOUND_PAIN1] : Sounds[SOUND_PAIN2]);

#if ROGUE_FEATURES
	// PMM - clear duck flag
	if (AIFlags & AI_DUCKED)
		UnDuck();
#endif
}

static const vec3f	DeathAimAngles[] =
{
	vec3f(0.0, 5.0, 0.0),
	vec3f(10.0, 15.0, 0.0),
	vec3f(20.0, 25.0, 0.0),
	vec3f(25.0, 35.0, 0.0),
	vec3f(30.0, 40.0, 0.0),
	vec3f(30.0, 45.0, 0.0),
	vec3f(25.0, 50.0, 0.0),
	vec3f(20.0, 40.0, 0.0),
	vec3f(15.0, 35.0, 0.0),
	vec3f(40.0, 35.0, 0.0),
	vec3f(70.0, 35.0, 0.0),
	vec3f(90.0, 35.0, 0.0)
};

void CInfantry::MachineGun ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start;
	sint32		flash_number;

	anglef	angles;

	if (Entity->State.GetFrame() == 
#if XATRIX_FEATURES || ROGUE_FEATURES
		CInfantry::FRAME_attak103
#else
		CInfantry::FRAME_attak111
#endif
		)
	{
		flash_number = MZ2_INFANTRY_MACHINEGUN_1;
		angles = Entity->State.GetAngles().ToVectors();
		G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flash_number], angles, start);

		if (Entity->Enemy)
		{
			vec3f target = Entity->Enemy->State.GetOrigin();
			if (Entity->Enemy->EntityFlags & EF_PHYSICS)
				target = target.MultiplyAngles (-0.2f, entity_cast<IPhysicsEntity>(*Entity->Enemy)->Velocity);
			target.Z += Entity->Enemy->ViewHeight;

			angles.Forward = target - start;
			angles.Forward.NormalizeFast ();
		}
	}
	else
	{
		flash_number = MZ2_INFANTRY_MACHINEGUN_2 + (Entity->State.GetFrame() - CInfantry::FRAME_death211);

		G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flash_number], angles, start);

		angles = (Entity->State.GetAngles() - DeathAimAngles[flash_number-MZ2_INFANTRY_MACHINEGUN_2]).ToVectors ();
	}

	MonsterFireBullet (start, angles.Forward, 3, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, flash_number);
}

void CInfantry::Sight ()
{
	Entity->PlaySound (CHAN_BODY, Sounds[SOUND_SIGHT]);
}

void CInfantry::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->Link ();

	CheckFlies ();
}

CFrame InfantryFramesDeath1 [] =
{
	CFrame (&IMonster::AI_Move, -4),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -4),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, 3),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 9),
	CFrame (&IMonster::AI_Move, 9),
	CFrame (&IMonster::AI_Move, 5),
	CFrame (&IMonster::AI_Move, -3),
	CFrame (&IMonster::AI_Move, -3)
};
CAnim InfantryMoveDeath1 (CInfantry::FRAME_death101, CInfantry::FRAME_death120, InfantryFramesDeath1, ConvertDerivedFunction(&CInfantry::Dead));

// Off with his head
CFrame InfantryFramesDeath2 [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 5),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 4),
	CFrame (&IMonster::AI_Move, 3),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -2,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&IMonster::AI_Move, -2,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&IMonster::AI_Move, -3,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&IMonster::AI_Move, -1,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&IMonster::AI_Move, -2,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&IMonster::AI_Move, 0,   ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&IMonster::AI_Move, 2,   ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&IMonster::AI_Move, 2,   ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&IMonster::AI_Move, 3,   ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&IMonster::AI_Move, -10, ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&IMonster::AI_Move, -7,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&IMonster::AI_Move, -8,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&IMonster::AI_Move, -6),
	CFrame (&IMonster::AI_Move, 4),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim InfantryMoveDeath2 (CInfantry::FRAME_death201, CInfantry::FRAME_death225, InfantryFramesDeath2, ConvertDerivedFunction(&CInfantry::Dead));

CFrame InfantryFramesDeath3 [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -6),
	CFrame (&IMonster::AI_Move, -11),
	CFrame (&IMonster::AI_Move, -3),
	CFrame (&IMonster::AI_Move, -11),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim InfantryMoveDeath3 (CInfantry::FRAME_death301, CInfantry::FRAME_death309, InfantryFramesDeath3, ConvertDerivedFunction(&CInfantry::Dead));


void CInfantry::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
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
	Entity->IsDead = true;
	Entity->CanTakeDamage = true;

	CAnim *Animation;
	sint32 pSound;
	switch (irandom(3))
	{
	case 0:
	default:
		Animation = &InfantryMoveDeath1;
		pSound = Sounds[SOUND_DIE2];
		break;
	case 1:
		Animation = &InfantryMoveDeath2;
		pSound = Sounds[SOUND_DIE1];
		break;
	case 2:
		Animation = &InfantryMoveDeath3;
		pSound = Sounds[SOUND_DIE2];
		break;
	}
	CurrentMove = Animation;
	Entity->PlaySound (CHAN_VOICE, pSound);
}

#if !ROGUE_FEATURES
void CInfantry::Duck_Down ()
{
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	Entity->GetMaxs().Z -= 32;
	Entity->CanTakeDamage = true;
	PauseTime = Level.Frame + 10;
	Entity->Link ();
}

void CInfantry::Duck_Hold ()
{
	if (Level.Frame >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CInfantry::Duck_Up ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->GetMaxs().Z += 32;
	Entity->CanTakeDamage = true;
	Entity->Link ();
}
#endif

CFrame InfantryFramesDuck [] =
{
#if !ROGUE_FEATURES
	CFrame (&IMonster::AI_Move, -2, ConvertDerivedFunction(&CInfantry::Duck_Down)),
	CFrame (&IMonster::AI_Move, -5, ConvertDerivedFunction(&CInfantry::Duck_Hold)),
	CFrame (&IMonster::AI_Move, 3),
	CFrame (&IMonster::AI_Move, 4, ConvertDerivedFunction(&CInfantry::Duck_Up)),
	CFrame (&IMonster::AI_Move, 0)
#else
	CFrame (&IMonster::AI_Move, -2, &IMonster::DuckDown),
	CFrame (&IMonster::AI_Move, -5, &IMonster::DuckHold),
	CFrame (&IMonster::AI_Move, 3),
	CFrame (&IMonster::AI_Move, 4, &IMonster::UnDuck),
	CFrame (&IMonster::AI_Move, 0)
#endif
};
CAnim InfantryMoveDuck (CInfantry::FRAME_duck01, CInfantry::FRAME_duck05, InfantryFramesDuck, ConvertDerivedFunction(&CInfantry::Run));

#if !ROGUE_FEATURES
void CInfantry::Dodge (IBaseEntity *Attacker, float eta)
{
	if (frand() > 0.25)
		return;

	if (!Entity->Enemy && Entity->Enemy->EntityFlags & EF_HURTABLE)
		Entity->Enemy = Attacker;

	CurrentMove = &InfantryMoveDuck;
}
#endif

void CInfantry::CockGun ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_WEAPON_COCK]);
}

void CInfantry::Fire ()
{
	MachineGun ();

	if (Level.Frame >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

#if XATRIX_FEATURES
CFrame InfantryFramesAttack1 [] =
{
	CFrame (&IMonster::AI_Charge, 10),
	CFrame (&IMonster::AI_Charge, 6),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CInfantry::Fire)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, -7),
	CFrame (&IMonster::AI_Charge, -6),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CInfantry::CockGun)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, -1)
};
CAnim InfantryMoveAttack1 (CInfantry::FRAME_attak101, CInfantry::FRAME_attak115, InfantryFramesAttack1, ConvertDerivedFunction(&CInfantry::Run));
#elif ROGUE_FEATURES
CFrame InfantryFramesAttack1 [] =
{
	CFrame (&IMonster::AI_Charge, -3),
	CFrame (&IMonster::AI_Charge, -2),
	CFrame (&IMonster::AI_Charge, -1, ConvertDerivedFunction(&CInfantry::Fire)),
	CFrame (&IMonster::AI_Charge, 5),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, -3),
	CFrame (&IMonster::AI_Charge, -2),
	CFrame (&IMonster::AI_Charge, 2, ConvertDerivedFunction(&CInfantry::CockGun)),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, 4)
};
CAnim InfantryMoveAttack1 (CInfantry::FRAME_attak101, CInfantry::FRAME_attak115, InfantryFramesAttack1, ConvertDerivedFunction(&CInfantry::Run));
#elif !(MONSTER_SPECIFIC_FLAGS & INFANTRY_DOES_REVERSE_GUN_ATTACK)
CFrame InfantryFramesAttack1 [] =
{
	CFrame (&IMonster::AI_Charge, 4),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CInfantry::CockGun)),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 2),
	CFrame (&IMonster::AI_Charge, -2),
	CFrame (&IMonster::AI_Charge, -3),
	CFrame (&IMonster::AI_Charge, 1, ConvertDerivedFunction(&CInfantry::Fire)),
	CFrame (&IMonster::AI_Charge, 5),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, -2),
	CFrame (&IMonster::AI_Charge, -3)
};
CAnim InfantryMoveAttack1 (CInfantry::FRAME_attak101, CInfantry::FRAME_attak115, InfantryFramesAttack1, ConvertDerivedFunction(&CInfantry::Run));
#else
CFrame InfantryFramesAttack1 [] =
{
	//CFrame (&IMonster::AI_Charge, -3),
	//CFrame (&IMonster::AI_Charge, -2),
	//CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, 5),
	CFrame (&IMonster::AI_Charge, 1, ConvertDerivedFunction(&CInfantry::Fire)),
	CFrame (&IMonster::AI_Charge, -3),
	CFrame (&IMonster::AI_Charge, -2),
	CFrame (&IMonster::AI_Charge, 2),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, -1, ConvertDerivedFunction(&CInfantry::CockGun)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, 4)
};
CAnim InfantryMoveAttack1 (CInfantry::FRAME_attak112, CInfantry::FRAME_attak101, InfantryFramesAttack1, ConvertDerivedFunction(&CInfantry::Run));
#endif


void CInfantry::Swing ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_PUNCH_SWING]);
}

void CInfantry::Smack ()
{
	static vec3f	aim (MELEE_DISTANCE, 0, 0);
	if (CMeleeWeapon::Fire (Entity, aim, (5 + (irandom(5))), 50))
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_PUNCH_HIT]);
}

CFrame InfantryFramesAttack2 [] =
{
	CFrame (&IMonster::AI_Charge, 3),
	CFrame (&IMonster::AI_Charge, 6),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CInfantry::Swing)),
	CFrame (&IMonster::AI_Charge, 8),
	CFrame (&IMonster::AI_Charge, 5),
	CFrame (&IMonster::AI_Charge, 8, ConvertDerivedFunction(&CInfantry::Smack)),
	CFrame (&IMonster::AI_Charge, 6),
	CFrame (&IMonster::AI_Charge, 3),
};
CAnim InfantryMoveAttack2 (CInfantry::FRAME_attak201, CInfantry::FRAME_attak208, InfantryFramesAttack2, ConvertDerivedFunction(&CInfantry::Run));

void CInfantry::Attack ()
{
	PauseTime = Level.Frame + ((randomMT() & 15) + 
#if ROGUE_FEATURES
		5
#else
		11
#endif
		);
	CurrentMove = &InfantryMoveAttack1;
}

void CInfantry::Melee ()
{
	CurrentMove = &InfantryMoveAttack2;
}

#if ROGUE_FEATURES
void CInfantry::Duck (float eta)
{
	if ((CurrentMove == &InfantryMoveAttack1) ||
		(CurrentMove == &InfantryMoveAttack2))
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

	NextFrame = CInfantry::FRAME_duck01;
	CurrentMove = &InfantryMoveDuck;
}

void CInfantry::SideStep ()
{
	if ((CurrentMove == &InfantryMoveAttack1) ||
		(CurrentMove == &InfantryMoveAttack2))
	{
		// if we're shooting, and not on easy, don't dodge
		if (CvarList[CV_SKILL].Integer())
		{
			AIFlags &= ~AI_DODGING;
			return;
		}
	}

	if (CurrentMove != &InfantryMoveRun)
		CurrentMove = &InfantryMoveRun;
}
#endif

void CInfantry::Spawn ()
{
	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex("models/monsters/infantry/tris.md2");
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, 32);

	Sounds[SOUND_PAIN1] = SoundIndex ("infantry/infpain1.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("infantry/infpain2.wav");
	Sounds[SOUND_DIE1] = SoundIndex ("infantry/infdeth1.wav");
	Sounds[SOUND_DIE2] = SoundIndex ("infantry/infdeth2.wav");

	Sounds[SOUND_GUNSHOT] = SoundIndex ("infantry/infatck1.wav");
	Sounds[SOUND_WEAPON_COCK] = SoundIndex ("infantry/infatck3.wav");
	Sounds[SOUND_PUNCH_SWING] = SoundIndex ("infantry/infatck2.wav");
	Sounds[SOUND_PUNCH_HIT] = SoundIndex ("infantry/melee2.wav");
	
	Sounds[SOUND_SIGHT] = SoundIndex ("infantry/infsght1.wav");
	Sounds[SOUND_SEARCH] = SoundIndex ("infantry/infsrch1.wav");
	Sounds[SOUND_IDLE] = SoundIndex ("infantry/infidle1.wav");

	Entity->Health = 100;
	Entity->GibHealth = -40;
	Entity->Mass = 200;

	MonsterFlags = (MF_HAS_MELEE | MF_HAS_ATTACK | MF_HAS_IDLE | MF_HAS_SIGHT
#if ROGUE_FEATURES
		| MF_HAS_SIDESTEP | MF_HAS_DUCK | MF_HAS_UNDUCK | MF_HAS_DODGE
#endif
		);

	Entity->Link ();

	CurrentMove = &InfantryMoveStand;
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_infantry", CInfantry);
