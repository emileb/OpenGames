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
// cc_supertank.h
// Supertank (Boss1)
//

#include "Local.h"
#include "Monsters/Supertank.h"

CSuperTank::CSuperTank (uint32 ID) :
IMonster (ID)
{
};

void CSuperTank::PlayTreadSound ()
{
	Entity->PlaySound (CHAN_BODY, Sounds[SOUND_TREAD]);
}

void CSuperTank::Search ()
{
	Entity->PlaySound (CHAN_VOICE, (frand() < 0.5) ? Sounds[SOUND_SEARCH1] : Sounds[SOUND_SEARCH2]);
}

//
// stand
//

CFrame SuperTankFramesStand []=
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
CAnim SuperTankMoveStand (CSuperTank::FRAME_stand1, CSuperTank::FRAME_stand60, SuperTankFramesStand);
	
void CSuperTank::Stand ()
{
	CurrentMove = &SuperTankMoveStand;
}

CFrame SuperTankFramesRun [] =
{
	CFrame (&IMonster::AI_Run, 12, ConvertDerivedFunction(&CSuperTank::PlayTreadSound)),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 12)
};
CAnim SuperTankMoveRun (CSuperTank::FRAME_forwrd1, CSuperTank::FRAME_forwrd18, SuperTankFramesRun);

//
// walk
//

CFrame SuperTankFramesForward [] =
{
	CFrame (&IMonster::AI_Walk, 4, ConvertDerivedFunction(&CSuperTank::PlayTreadSound)),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4)
};
CAnim SuperTankMoveForward (CSuperTank::FRAME_forwrd1, CSuperTank::FRAME_forwrd18, SuperTankFramesForward);

void CSuperTank::Walk ()
{
	CurrentMove = &SuperTankMoveForward;
}

void CSuperTank::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &SuperTankMoveStand : &SuperTankMoveRun;
}

CFrame SuperTankFramesPain3 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim SuperTankMovePain3 (CSuperTank::FRAME_painc9, CSuperTank::FRAME_painc12, SuperTankFramesPain3, &IMonster::Run);

CFrame SuperTankFramesPain2 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim SuperTankMovePain2 (CSuperTank::FRAME_painb5, CSuperTank::FRAME_painb8, SuperTankFramesPain2, &IMonster::Run);

CFrame SuperTankFramesPain1 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim SuperTankMovePain1 (CSuperTank::FRAME_paina1, CSuperTank::FRAME_paina4, SuperTankFramesPain1, &IMonster::Run);

CFrame SuperTankFramesDeath1 [] =
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
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0, &IMonster::BossExplode)
};
CAnim SuperTankMoveDeath (CSuperTank::FRAME_death1, CSuperTank::FRAME_death24, SuperTankFramesDeath1, ConvertDerivedFunction(&CSuperTank::Dead));

#if (MONSTER_SPECIFIC_FLAGS & SUPERTANK_USES_GRENADES)
CFrame SuperTankFramesAttack4[] =
{
	CFrame (&IMonster::AI_Move,	0, ConvertDerivedFunction(&CSuperTank::Grenade)),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0, ConvertDerivedFunction(&CSuperTank::Grenade)),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim SuperTankMoveAttack4 (CSuperTank::FRAME_attakd1, CSuperTank::FRAME_attakd6, SuperTankFramesAttack4, &IMonster::Run);

void CSuperTank::Grenade ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start;
	vec3f offset (32.0f, 37.0f, 50.0f);

	if (Entity->State.GetFrame() == CSuperTank::FRAME_attakd4)
		offset.Y = -offset.Y;

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), offset, angles, start);

	if (Entity->Enemy)
	{
		vec3f vec = Entity->Enemy->State.GetOrigin();
		vec.Z += Entity->Enemy->ViewHeight;
		angles.Forward = vec - start;
		angles.Forward.Normalize ();
	}

	Entity->PlaySound (CHAN_WEAPON, SoundIndex("gunner/Gunatck3.wav"));

	MonsterFireGrenade (start, angles.Forward, 25, 600, -1);
}
#endif

CFrame SuperTankFramesAttack2[]=
{
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0,	ConvertDerivedFunction(&CSuperTank::Rocket)),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0,	ConvertDerivedFunction(&CSuperTank::Rocket)),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0,	ConvertDerivedFunction(&CSuperTank::Rocket)),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0),
	CFrame (&IMonster::AI_Charge,		0)
};
CAnim SuperTankMoveAttack2 (CSuperTank::FRAME_attakb1, CSuperTank::FRAME_attakb27, SuperTankFramesAttack2, &IMonster::Run);

CFrame SuperTankFramesAttack1[]=
{
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CSuperTank::MachineGun)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CSuperTank::MachineGun)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CSuperTank::MachineGun)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CSuperTank::MachineGun)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CSuperTank::MachineGun)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CSuperTank::MachineGun)),
};
CAnim SuperTankMoveAttack1 (CSuperTank::FRAME_attaka1, CSuperTank::FRAME_attaka6, SuperTankFramesAttack1, ConvertDerivedFunction(&CSuperTank::ReAttack1));

CFrame SuperTankFramesEndAttack1[]=
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
	CFrame (&IMonster::AI_Move,	0)
};
CAnim SuperTankMoveEndAttack1 (CSuperTank::FRAME_attaka7, CSuperTank::FRAME_attaka20, SuperTankFramesEndAttack1, &IMonster::Run);

void CSuperTank::ReAttack1 ()
{
	if (IsVisible(Entity, *Entity->Enemy))
		CurrentMove = (frand() < 0.9) ? &SuperTankMoveAttack1 : &SuperTankMoveEndAttack1;
	else
		CurrentMove = &SuperTankMoveEndAttack1;
}

void CSuperTank::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (!(Entity->State.GetSkinNum() & 1) && Entity->Health < (Entity->MaxHealth / 2))
			Entity->State.GetSkinNum() |= 1;

	if (Level.Frame < PainDebounceTime)
			return;

	// Lessen the chance of him going into his pain frames
	if (Damage <= 25 && frand() < 0.2)
		return;

	// Don't go into pain if he's firing his rockets
	if (CvarList[CV_SKILL].Integer() >= 2 && (Entity->State.GetFrame() >= CSuperTank::FRAME_attakb1) && (Entity->State.GetFrame() <= CSuperTank::FRAME_attakb14) )
		return;

	PainDebounceTime = Level.Frame + 30;

	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	if (Damage <= 10)
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
		CurrentMove = &SuperTankMovePain1;
	}
	else if (Damage <= 25)
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN3]);
		CurrentMove = &SuperTankMovePain2;
	}
	else
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN2]);
		CurrentMove = &SuperTankMovePain3;
	}

#if ROGUE_FEATURES
	// PMM - blindfire cleanup
	AIFlags &= ~AI_MANUAL_STEERING;
	// pmm
#endif
};

void CSuperTank::Rocket ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start;
	int		FlashNumber;

	switch (Entity->State.GetFrame())
	{
	case CSuperTank::FRAME_attakb8:
		FlashNumber = MZ2_SUPERTANK_ROCKET_1;
		break;
	case CSuperTank::FRAME_attakb11:
		FlashNumber = MZ2_SUPERTANK_ROCKET_2;
		break;
	default:
		FlashNumber = MZ2_SUPERTANK_ROCKET_3;
		break;
	}

	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[FlashNumber], Entity->State.GetAngles().ToVectors (), start);

	MonsterFireRocket (start, ((Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight)) - start).GetNormalized(), 50, 500, FlashNumber);
}	

void CSuperTank::MachineGun ()
{
	if (!HasValidEnemy())
		return;

	vec3f start, dir (0, Entity->State.GetAngles().Y, 0);
	sint32		FlashNumber = MZ2_SUPERTANK_MACHINEGUN_1 + (Entity->State.GetFrame() - CSuperTank::FRAME_attaka1);

	anglef angles = dir.ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[FlashNumber], angles, start);

	if (Entity->Enemy)
	{
		vec3f vec = Entity->Enemy->State.GetOrigin();
		vec.Z += Entity->Enemy->ViewHeight;
		angles.Forward = vec - start;
		angles.Forward.Normalize ();
	}

	MonsterFireBullet (start, angles.Forward, 6, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, FlashNumber);
}	


void CSuperTank::Attack ()
{
#if ROGUE_FEATURES
	// PMM 
	if (AttackState == AS_BLIND)
	{
		float chance, r;
		// setup shot probabilities
		if (BlindFireDelay < 1.0)
			chance = 1.0;
		else if (BlindFireDelay < 7.5)
			chance = 0.4f;
		else
			chance = 0.1f;

		r = frand();

		BlindFireDelay += 3.2 + 2.0 + frand()*3.0;

		// don't shoot at the origin
		if (BlindFireTarget == vec3fOrigin)
			return;

		// don't shoot if the dice say not to
		if (r > chance)
			return;

		// turn on manual steering to signal both manual steering and blindfire
		AIFlags |= AI_MANUAL_STEERING;
		CurrentMove = &SuperTankMoveAttack2;
		AttackFinished = Level.Frame + 30 + 20*frand();
		return;
	}
	// pmm
#endif

	float range = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).Length();

#ifndef SUPERTANK_USES_GRENADE_LAUNCHER
	if (range <= 160)
		CurrentMove = &SuperTankMoveAttack1;
	else // fire rockets more often at distance
		CurrentMove = (frand() < 0.3) ? &SuperTankMoveAttack1 : &SuperTankMoveAttack2;
#else
	if (range <= 160)
		CurrentMove = (frand() < 0.3) ? &SuperTankMoveAttack4 : &SuperTankMoveAttack1;
	else if (range <= 650)
		CurrentMove = (frand() < 0.6) ? &SuperTankMoveAttack4 : &SuperTankMoveAttack1;
	else
		CurrentMove = (frand() < 0.3) ? &SuperTankMoveAttack1 : &SuperTankMoveAttack2;
#endif
}


//
// death
//

void CSuperTank::Dead ()
{
	Entity->GetMins().Set (-60, -60, 0);
	Entity->GetMaxs().Set (60, 60, 72);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

void CSuperTank::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DEATH]);
	Entity->IsDead = true;
	Entity->CanTakeDamage = false;
	ExplodeCount = 0;
	CurrentMove = &SuperTankMoveDeath;
}

//
// monster_supertank
//

/*QUAKED monster_supertank (1 .5 0) (-64 -64 0) (64 64 72) Ambush Trigger_Spawn Sight
*/

#if XATRIX_FEATURES
/**
\enum	

\brief	Values that represent spawnflags pertaining to CSuperTank. 
**/
enum
{
	SPAWNFLAG_SUPERTANK_POWER_SHIELD		= BIT(4)
};
#endif

void CSuperTank::Spawn ()
{
	Sounds[SOUND_PAIN1] = SoundIndex ("bosstank/btkpain1.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("bosstank/btkpain2.wav");
	Sounds[SOUND_PAIN3] = SoundIndex ("bosstank/btkpain3.wav");
	Sounds[SOUND_DEATH] = SoundIndex ("bosstank/btkdeth1.wav");
	Sounds[SOUND_SEARCH1] = SoundIndex ("bosstank/btkunqv1.wav");
	Sounds[SOUND_SEARCH2] = SoundIndex ("bosstank/btkunqv2.wav");
	Sounds[SOUND_TREAD] = SoundIndex ("bosstank/btkengn1.wav");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/boss1/tris.md2");
	Entity->GetMins().Set (-64, -64, 0);
	Entity->GetMaxs().Set (64, 64, 112);

	Entity->Health = 1500;
	Entity->GibHealth = -500;
	Entity->Mass = 800;

#if ROGUE_FEATURES
	// PMM
	AIFlags |= AI_IGNORE_SHOTS;
	BlindFire = true;
	//pmm
#endif

	MonsterFlags = (MF_HAS_ATTACK | MF_HAS_SEARCH);
	Entity->Link ();
	Stand();

#if XATRIX_FEATURES
	// Paril FIXME: used? or is boss5 replacement??
	if (Entity->SpawnFlags & SPAWNFLAG_SUPERTANK_POWER_SHIELD)
	{
		PowerArmorType = POWER_ARMOR_SHIELD;
		PowerArmorPower = 400;
	}
#endif

	WalkMonsterStart();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_supertank", CSuperTank);
