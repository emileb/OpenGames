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
// cc_berserk.cpp
// Berserker
//

#include "Local.h"
#include "Monsters/Berserk.h"

CBerserker::CBerserker (uint32 ID) :
IMonster(ID)
{
}

void CBerserker::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

void CBerserker::Search ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SEARCH]);
}


CFrame BerserkFramesStand [] =
{
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CBerserker::Fidget)),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim BerserkMoveStand (CBerserker::FRAME_stand1, CBerserker::FRAME_stand5, BerserkFramesStand);

void CBerserker::Stand ()
{
	CurrentMove = &BerserkMoveStand;
}

CFrame BerserkFramesStandFidget [] =
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
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim BerserkMoveStandFidget (CBerserker::FRAME_standb1, CBerserker::FRAME_standb20, BerserkFramesStandFidget, &IMonster::Stand);

void CBerserker::Fidget ()
{
	if (AIFlags & AI_STAND_GROUND)
		return;
	if (frand() > 0.15)
		return;

	CurrentMove = &BerserkMoveStandFidget;
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_IDLE], 255, ATTN_IDLE);
}


CFrame BerserkFramesWalk [] =
{
	CFrame (&IMonster::AI_Walk, 9.1f),
	CFrame (&IMonster::AI_Walk, 6.3f),
	CFrame (&IMonster::AI_Walk, 4.9f),
	CFrame (&IMonster::AI_Walk, 6.7f),
	CFrame (&IMonster::AI_Walk, 6.0f),
	CFrame (&IMonster::AI_Walk, 8.2f),
	CFrame (&IMonster::AI_Walk, 7.2f),
	CFrame (&IMonster::AI_Walk, 6.1f),
	CFrame (&IMonster::AI_Walk, 4.9f),
	CFrame (&IMonster::AI_Walk, 4.7f),
	CFrame (&IMonster::AI_Walk, 4.7f),
	CFrame (&IMonster::AI_Walk, 4.8f)
};
CAnim BerserkMoveWalk (CBerserker::FRAME_walkc1, CBerserker::FRAME_walkc11, BerserkFramesWalk);

void CBerserker::Walk ()
{
	CurrentMove = &BerserkMoveWalk;
}

/*

  *****************************
  SKIPPED THIS FOR NOW!
  *****************************

   Running -> Arm raised in air

void()	berserk_runb1	=[	$r_att1 ,	berserk_runb2	] {ai_run(21);};
void()	berserk_runb2	=[	$r_att2 ,	berserk_runb3	] {ai_run(11);};
void()	berserk_runb3	=[	$r_att3 ,	berserk_runb4	] {ai_run(21);};
void()	berserk_runb4	=[	$r_att4 ,	berserk_runb5	] {ai_run(25);};
void()	berserk_runb5	=[	$r_att5 ,	berserk_runb6	] {ai_run(18);};
void()	berserk_runb6	=[	$r_att6 ,	berserk_runb7	] {ai_run(19);};
// running with arm in air : start loop
void()	berserk_runb7	=[	$r_att7 ,	berserk_runb8	] {ai_run(21);};
void()	berserk_runb8	=[	$r_att8 ,	berserk_runb9	] {ai_run(11);};
void()	berserk_runb9	=[	$r_att9 ,	berserk_runb10	] {ai_run(21);};
void()	berserk_runb10	=[	$r_att10 ,	berserk_runb11	] {ai_run(25);};
void()	berserk_runb11	=[	$r_att11 ,	berserk_runb12	] {ai_run(18);};
void()	berserk_runb12	=[	$r_att12 ,	berserk_runb7	] {ai_run(19);};
// running with arm in air : end loop
*/

CFrame BerserkFramesRun1 [] =
{
	CFrame (&IMonster::AI_Run, 21),
	CFrame (&IMonster::AI_Run, 11),
	CFrame (&IMonster::AI_Run, 21),
	CFrame (&IMonster::AI_Run, 25),
	CFrame (&IMonster::AI_Run, 18),
	CFrame (&IMonster::AI_Run, 19)
};
CAnim BerserkMoveRun1 (CBerserker::FRAME_run1, CBerserker::FRAME_run6, BerserkFramesRun1);

void CBerserker::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &BerserkMoveStand : &BerserkMoveRun1;
}

void CBerserker::AttackSpike ()
{
	static	vec3f	aim (MELEE_DISTANCE, 0, -24);
	CMeleeWeapon::Fire (Entity, aim, (15 + (irandom(6))), 400);		//	Faster attack -- upwards and backwards
}

void CBerserker::Swing ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_PUNCH]);
}

CFrame BerserkFramesAttackSpike [] =
{
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CBerserker::Swing)),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CBerserker::AttackSpike)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim BerserkMoveAttackSpike (CBerserker::FRAME_att_c1, CBerserker::FRAME_att_c8, BerserkFramesAttackSpike, &IMonster::Run);

void CBerserker::AttackClub ()
{
	vec3f	aim (MELEE_DISTANCE, Entity->GetMins().X, -4);
	CMeleeWeapon::Fire (Entity, aim, (5 + (irandom(6))), 400);		// Slower attack
}

CFrame BerserkFramesAttackClub [] =
{	
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CBerserker::Swing)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CBerserker::AttackClub)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim BerserkMoveAttackClub (CBerserker::FRAME_att_c9, CBerserker::FRAME_att_c20, BerserkFramesAttackClub, &IMonster::Run);

void CBerserker::Melee ()
{
	CurrentMove = (!irandom(2)) ? &BerserkMoveAttackSpike : &BerserkMoveAttackClub;
}

CFrame BerserkFramesPain1 [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim BerserkMovePain1 (CBerserker::FRAME_painc1, CBerserker::FRAME_painc4, BerserkFramesPain1, &IMonster::Run);

CFrame BerserkFramesPain2 [] =
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
	CFrame (&IMonster::AI_Move, 0)
};
CAnim BerserkMovePain2 (CBerserker::FRAME_painb1, CBerserker::FRAME_painb20, BerserkFramesPain2, &IMonster::Run);

void CBerserker::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN]);

	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	CurrentMove = ((Damage < 20) || (frand() < 0.5)) ? &BerserkMovePain1 : &BerserkMovePain2;
}

void CBerserker::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);

	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->Link ();

	Entity->NextThink = 0;
}

CFrame BerserkFramesDeath1 [] =
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
	CFrame (&IMonster::AI_Move, 0)
};
CAnim BerserkMoveDeath1 (CBerserker::FRAME_death1, CBerserker::FRAME_death13, BerserkFramesDeath1, ConvertDerivedFunction(&CBerserker::Dead));

CFrame BerserkFramesDeath2 [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim BerserkMoveDeath2 (CBerserker::FRAME_deathc1, CBerserker::FRAME_deathc8, BerserkFramesDeath2, ConvertDerivedFunction(&CBerserker::Dead));

void CBerserker::Die(IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
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

	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DIE]);
	Entity->IsDead = true;
	Entity->CanTakeDamage = true;

	CurrentMove = (Damage >= 50) ? &BerserkMoveDeath1 : &BerserkMoveDeath2;
}

#if ROGUE_FEATURES
void CBerserker::SideStep ()
{
	// don't check for attack; the eta should suffice for melee monsters
	if (CurrentMove != &BerserkMoveRun1)
		CurrentMove = &BerserkMoveRun1;
}
#endif

/*QUAKED monster_berserk (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void CBerserker::Spawn ()
{
	// pre-caches
	Sounds[SOUND_PAIN]  = SoundIndex ("berserk/berpain2.wav");
	Sounds[SOUND_DIE]   = SoundIndex ("berserk/berdeth2.wav");
	Sounds[SOUND_IDLE]  = SoundIndex ("berserk/beridle1.wav");
	Sounds[SOUND_PUNCH] = SoundIndex ("berserk/attack.wav");
	Sounds[SOUND_SEARCH] = SoundIndex ("berserk/bersrch1.wav");
	Sounds[SOUND_SIGHT] = SoundIndex ("berserk/sight.wav");

	Entity->State.GetModelIndex() = ModelIndex("models/monsters/berserk/tris.md2");
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, 32);
	Entity->GetSolid() = SOLID_BBOX;

	Entity->Health = 240;
	Entity->GibHealth = -60;
	Entity->Mass = 250;

	MonsterFlags = (MF_HAS_MELEE | MF_HAS_SEARCH | MF_HAS_SIGHT
#if ROGUE_FEATURES
		| MF_HAS_SIDESTEP
#endif
		);

	CurrentMove = &BerserkMoveStand;

	Entity->Link ();
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_berserk", CBerserker);
