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
// cc_flyer.cpp
// Flyer Monster
//

#include "Local.h"
#include "Monsters/Flyer.h"

CFlyer::CFlyer (uint32 ID) :
IMonster (ID)
{
}

void CFlyer::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

void CFlyer::Idle ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_IDLE], 255, ATTN_IDLE);
}

void CFlyer::PopBlades ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SPROING]);
}

CFrame FlyerFramesStand [] =
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
CAnim FlyerMoveStand (CFlyer::FRAME_stand01, CFlyer::FRAME_stand45, FlyerFramesStand);

CFrame FlyerFramesWalk [] =
{
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5)
};
CAnim FlyerMoveWalk (CFlyer::FRAME_stand01, CFlyer::FRAME_stand45, FlyerFramesWalk);

CFrame FlyerFramesRun [] =
{
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 10)
};
CAnim FlyerMoveRun (CFlyer::FRAME_stand01, CFlyer::FRAME_stand45, FlyerFramesRun);

void CFlyer::Run ()
{
	if (AIFlags & AI_STAND_GROUND)
		CurrentMove = &FlyerMoveStand;
	else
		CurrentMove = &FlyerMoveRun;
}

void CFlyer::Walk ()
{
	CurrentMove = &FlyerMoveWalk;
}

void CFlyer::Stand ()
{
	CurrentMove = &FlyerMoveStand;
}

CFrame FlyerFramesPain3 [] =
{	
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim FlyerMovePain3 (CFlyer::FRAME_pain301, CFlyer::FRAME_pain304, FlyerFramesPain3, ConvertDerivedFunction(&CFlyer::Run));

CFrame FlyerFramesPain2 [] =
{
	CFrame (&IMonster::AI_Move, 0) ,
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim FlyerMovePain2 (CFlyer::FRAME_pain201, CFlyer::FRAME_pain204, FlyerFramesPain2, ConvertDerivedFunction(&CFlyer::Run));

CFrame FlyerFramesPain1 [] =
{
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
CAnim FlyerMovePain1 (CFlyer::FRAME_pain101, CFlyer::FRAME_pain109, FlyerFramesPain1, ConvertDerivedFunction(&CFlyer::Run));

void CFlyer::Fire (sint32 FlashNumber)
{
	vec3f	start, end, dir;
	sint32 effect;

	if (!HasValidEnemy())
		return;

	if ((Entity->State.GetFrame() == CFlyer::FRAME_attak204) || (Entity->State.GetFrame() == CFlyer::FRAME_attak207) || (Entity->State.GetFrame() == CFlyer::FRAME_attak210))
		effect = FX_HYPERBLASTER;
	else
		effect = 0;

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[FlashNumber], angles, start);
	
	end = Entity->Enemy->State.GetOrigin();
	end.Z += Entity->Enemy->ViewHeight;
	dir = end - start;

	MonsterFireBlaster (start, dir, 1, 1000, FlashNumber, effect);
}

void CFlyer::FireLeft ()
{
	Fire (MZ2_FLYER_BLASTER_1);
}

void CFlyer::FireRight ()
{
	Fire (MZ2_FLYER_BLASTER_2);
}

CFrame FlyerFramesAttack2 [] =
{
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, -10, ConvertDerivedFunction(&CFlyer::FireLeft)),			// left gun
	CFrame (&IMonster::AI_Charge, -10, ConvertDerivedFunction(&CFlyer::FireRight)),		// right gun
	CFrame (&IMonster::AI_Charge, -10, ConvertDerivedFunction(&CFlyer::FireLeft)),			// left gun
	CFrame (&IMonster::AI_Charge, -10, ConvertDerivedFunction(&CFlyer::FireRight)),		// right gun
	CFrame (&IMonster::AI_Charge, -10, ConvertDerivedFunction(&CFlyer::FireLeft)),			// left gun
	CFrame (&IMonster::AI_Charge, -10, ConvertDerivedFunction(&CFlyer::FireRight)),		// right gun
	CFrame (&IMonster::AI_Charge, -10, ConvertDerivedFunction(&CFlyer::FireLeft)),			// left gun
	CFrame (&IMonster::AI_Charge, -10, ConvertDerivedFunction(&CFlyer::FireRight)),		// right gun
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim FlyerMoveAttack2 (CFlyer::FRAME_attak201, CFlyer::FRAME_attak217, FlyerFramesAttack2, ConvertDerivedFunction(&CFlyer::Run));

void CFlyer::SlashLeft ()
{
	vec3f	aim (MELEE_DISTANCE, Entity->GetMins().X, 0);
	CMeleeWeapon::Fire (Entity, aim, 5, 0);
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_SLASH]);
}

void CFlyer::SlashRight ()
{
	vec3f	aim (MELEE_DISTANCE, Entity->GetMaxs().X, 0);
	CMeleeWeapon::Fire (Entity, aim, 5, 0);
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_SLASH]);
}

CFrame FlyerFramesStartMelee [] =
{
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CFlyer::PopBlades)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim FlyerMoveStartMelee (CFlyer::FRAME_attak101, CFlyer::FRAME_attak106, FlyerFramesStartMelee, ConvertDerivedFunction(&CFlyer::LoopMelee));

CFrame FlyerFramesEndMelee [] =
{
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim FlyerMoveEndMelee (CFlyer::FRAME_attak119, CFlyer::FRAME_attak121, FlyerFramesEndMelee, ConvertDerivedFunction(&CFlyer::Run));

CFrame FlyerFramesLoopMelee [] =
{
	CFrame (&IMonster::AI_Charge, 0),		// Loop Start
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CFlyer::SlashLeft)),		// Left Wing Strike
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CFlyer::SlashRight)),	// Right Wing Strike
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0)		// Loop Ends
};
CAnim FlyerMoveLoopMelee (CFlyer::FRAME_attak107, CFlyer::FRAME_attak118, FlyerFramesLoopMelee, ConvertDerivedFunction(&CFlyer::CheckMelee));

void CFlyer::LoopMelee ()
{
	CurrentMove = &FlyerMoveLoopMelee;
}

void CFlyer::Attack ()
{
	CurrentMove = &FlyerMoveAttack2;
}

void CFlyer::Melee ()
{
	CurrentMove = &FlyerMoveStartMelee;
}

void CFlyer::CheckMelee ()
{
	if (Range (Entity, *Entity->Enemy) == RANGE_MELEE)
		CurrentMove = (frand() <= 0.8) ? &FlyerMoveLoopMelee : &FlyerMoveEndMelee;
	else
		CurrentMove = &FlyerMoveEndMelee;
}

void CFlyer::Pain (IBaseEntity *Other, sint32 Damage)
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
		CurrentMove = &FlyerMovePain1;
		break;
	case 1:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN2]);
		CurrentMove = &FlyerMovePain2;
		break;
	default:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
		CurrentMove = &FlyerMovePain3;
		break;
	}
}

void CFlyer::Die(IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DIE]);
	Entity->BecomeExplosion(false);
}

#if (MONSTER_SPECIFIC_FLAGS & FLYER_KNOWS_HOW_TO_DODGE)
CFrame FlyerFramesRollLeft [] =
{
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15)
};
CAnim FlyerMoveRollLeft (CFlyer::FRAME_rollf01, CFlyer::FRAME_rollf09, FlyerFramesRollLeft, ConvertDerivedFunction(&CFlyer::ChooseAfterDodge));

CFrame FlyerFramesRollRight [] =
{
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15),
	CFrame (ConvertDerivedAIMove(&CFlyer::AI_Roll), 15)
};
CAnim FlyerMoveRollRight (CFlyer::FRAME_rollr01, CFlyer::FRAME_rollr09, FlyerFramesRollRight, ConvertDerivedFunction(&CFlyer::ChooseAfterDodge));

void CFlyer::AI_Roll(float Dist)
{
	IdealYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();
	ChangeYaw ();

	float Yaw = (Entity->State.GetAngles().Y - 90);
	if (CurrentMove == &FlyerMoveRollRight)
		Yaw = (Entity->State.GetAngles().Y + 90);

	if (Dist)
		WalkMove (Yaw, Dist);
}

void CFlyer::ChooseAfterDodge ()
{
	CurrentMove = (frand() < 0.5) ? &FlyerMoveRun : &FlyerMoveAttack2;
}

#if !ROGUE_FEATURES
void CFlyer::Dodge (IBaseEntity *Attacker, float eta)
#else
void CFlyer::Duck (float eta)
#endif
{
	if (frand() > (0.35f + ((CvarList[CV_SKILL].Float()+1) / 10)) )
		return;
	
	// Don't dodge if we're attacking or dodging already
	if ((CurrentMove == &FlyerMoveRollLeft || CurrentMove == &FlyerMoveRollRight) || (CurrentMove == &FlyerMoveAttack2 && (frand() < 0.25)))
		return;

	CTrace trace;

	anglef angles = Entity->State.GetAngles().ToVectors ();
	bool WantsLeft = (frand() < 0.5);

	// Approximate travel distance.
	// First, check if we can roll right.
	bool CanRollRight = false;
	bool CanRollLeft = false;

	vec3f end = Entity->State.GetOrigin ().MultiplyAngles (-75, angles.Right);
	trace (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), end, Entity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.Fraction == 1.0)
		CanRollRight = true;

	// Now check the left
	end = Entity->State.GetOrigin ().MultiplyAngles (-75, angles.Right);
	trace (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), end, Entity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.Fraction == 1.0)
		CanRollLeft = true;

	// Can we roll the way we wanted?
	if (WantsLeft && !CanRollLeft && CanRollRight)
		WantsLeft = false;
	else if (!WantsLeft && CanRollLeft && !CanRollRight)
		WantsLeft = true;

	CurrentMove = (WantsLeft) ? &FlyerMoveRollLeft : &FlyerMoveRollRight;
}
#endif

#if ROGUE_FEATURES && (MONSTER_SPECIFIC_FLAGS & FLYER_KNOWS_HOW_TO_DODGE)
void CFlyer::SideStep ()
{
	if (AIFlags & AI_STAND_GROUND)
		CurrentMove = &FlyerMoveStand;
	else
		CurrentMove = &FlyerMoveRun;
}
#endif

void CFlyer::Spawn ()
{
	// fix a map bug in jail5.bsp
	if ((Level.ServerLevelName == "jail5") && (Entity->State.GetOrigin().Z == -104))
	{
		Entity->TargetName = Entity->Target;
		Entity->Target.Clear();
	}

	Sounds[SOUND_SIGHT] = SoundIndex ("flyer/flysght1.wav");
	Sounds[SOUND_IDLE] = SoundIndex ("flyer/flysrch1.wav");
	Sounds[SOUND_PAIN1] = SoundIndex ("flyer/flypain1.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("flyer/flypain2.wav");
	Sounds[SOUND_SLASH] = SoundIndex ("flyer/flyatck2.wav");
	Sounds[SOUND_SPROING] = SoundIndex ("flyer/flyatck1.wav");
	Sounds[SOUND_DIE] = SoundIndex ("flyer/flydeth1.wav");

	SoundIndex ("flyer/flyatck3.wav");

	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/flyer/tris.md2");
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16);
	Entity->GetSolid() = SOLID_BBOX;

	Entity->State.GetSound() = SoundIndex ("flyer/flyidle1.wav");

	Entity->Health = 50;
	Entity->Mass = 50;

	MonsterFlags |= (MF_HAS_IDLE | MF_HAS_SIGHT | MF_HAS_MELEE | MF_HAS_ATTACK
#if ROGUE_FEATURES && (MONSTER_SPECIFIC_FLAGS & FLYER_KNOWS_HOW_TO_DODGE)
	| MF_HAS_DUCK | MF_HAS_UNDUCK | MF_HAS_DODGE | MF_HAS_SIDESTEP
#endif
		);
	Entity->Link ();

	CurrentMove = &FlyerMoveStand;
	FlyMonsterStart ();

	Entity->ViewHeight = 8;
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_flyer", CFlyer);
