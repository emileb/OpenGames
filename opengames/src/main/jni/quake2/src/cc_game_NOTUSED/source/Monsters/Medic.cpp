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
// cc_medic.cpp
// Medic
//

#include "Local.h"
#include "Monsters/Medic.h"

const int	MEDIC_MIN_DISTANCE			= 32;
const int	MEDIC_MAX_HEAL_DISTANCE		= 400;
const int	MEDIC_TRY_TIME				= 100;

CMedic::CMedic (uint32 ID) :
IMonster(ID)
{
}

#if ROGUE_FEATURES
void CMedic::CleanupHeal (bool ChangeFrame)
{
	// clean up target, if we have one and it's legit
	if (Entity->Enemy && Entity->Enemy->GetInUse() && (Entity->Enemy->EntityFlags & EF_MONSTER))
	{
		CMonsterEntity *Enemy = entity_cast<CMonsterEntity>(*Entity->Enemy);
		Enemy->Monster->Healer = nullentity;
		Enemy->Monster->AIFlags &= ~AI_RESURRECTING;
		Enemy->CanTakeDamage = true;
		Enemy->Monster->SetEffects ();
	}

	if (ChangeFrame)
		NextFrame = CMedic::FRAME_attack52;
}

#include "Rogue/RogueMedicCommander.h"

void CMedic::AbortHeal (bool Gib, bool Mark)
{
	static vec3f	PainNormal (0, 0, 1);

	// clean up target
	CleanupHeal (true);

	// gib em!
	if ((Mark) && (Entity->Enemy) && (Entity->Enemy->GetInUse()) && (Entity->Enemy->EntityFlags & EF_MONSTER))
	{
		CMonsterEntity *Enemy = entity_cast<CMonsterEntity>(*Entity->Enemy);
		// if the first badMedic slot is filled by a medic, skip it and use the second one
		if ((Enemy->Monster->BadMedic1) && (Enemy->Monster->BadMedic1->GetInUse())
			&& (Enemy->Monster->MonsterID == CMedic::ID || Enemy->Monster->MonsterID == CMedicCommander::ID))
			Enemy->Monster->BadMedic2 = Entity;
		else
			Enemy->Monster->BadMedic1 = Entity;
	}
	if ((Gib) && (Entity->Enemy) && (Entity->Enemy->GetInUse()) && (Entity->Enemy->EntityFlags & EF_MONSTER))
	{
		CMonsterEntity *Enemy = entity_cast<CMonsterEntity>(*Entity->Enemy);

		sint32 hurt = (Enemy->GibHealth) ? -Enemy->GibHealth : 500;

		Enemy->TakeDamage (Entity, Entity, vec3fOrigin, Enemy->State.GetOrigin(),
				PainNormal, hurt, 0, 0, MOD_UNKNOWN);
	}
	// clean up self

	AIFlags &= ~AI_MEDIC;
	if ((Entity->OldEnemy) && (Entity->OldEnemy->GetInUse()))
		Entity->Enemy = Entity->OldEnemy;
	else
		Entity->Enemy = nullentity;

	MedicTries = 0;
}

#endif

CMonsterEntity *CMedic::FindDeadMonster ()
{
	CMonsterEntity *Found = NULL, *Best = NULL;

	while ((Found = FindRadius<CMonsterEntity, EF_MONSTER>(Found, Entity->State.GetOrigin(), 1024)) != NULL)
	{
		if (Found == Entity)
			continue;
		if (Found->Monster->AIFlags & AI_GOOD_GUY)
			continue;
		if (Found->Health > 0)
			continue;
		if (Found->NextThink)
			continue;
		if (!IsVisible(Entity, Found))
			continue;
#if ROGUE_FEATURES
		if (Found->Monster->Healer)
			continue;
		// check to make sure we haven't bailed on this guy already
		if ((Found->Monster->BadMedic1 == Entity) || (Found->Monster->BadMedic2 == Entity))
			continue;
		if (Found->Monster->Healer && (Found->Monster->Healer->GetInUse()) && (Found->Monster->Healer->Health > 0) &&
			(Found->Monster->Healer->GetSvFlags() & SVF_MONSTER) && (Found->Monster->Healer->Monster->AIFlags & AI_MEDIC))
			continue;
#endif
		if (!Best)
		{
			Best = Found;
			continue;
		}
		if (Found->MaxHealth <= Best->MaxHealth)
			continue;
		Best = Found;
	}

#if ROGUE_FEATURES
	if (Best)
		MedicTryTime = Level.Frame + MEDIC_TRY_TIME;
#endif

	return Best;
}

void CMedic::Idle ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_IDLE], 255, ATTN_IDLE);

	CMonsterEntity *Found = FindDeadMonster();
	if (Found)
	{
		Entity->Enemy = Found;
#if ROGUE_FEATURES
		Found->Monster->Healer = Entity;
#endif
		AIFlags |= AI_MEDIC;
		FoundTarget ();
	}
}

void CMedic::Search ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SEARCH], 255, ATTN_IDLE);

	if (!Entity->OldEnemy)
	{
		CMonsterEntity *Found = FindDeadMonster();
		if (Found)
		{
			Entity->OldEnemy = Entity->Enemy;
			Entity->Enemy = Found;
#if ROGUE_FEATURES
			Found->Monster->Healer = Entity;
#endif
			AIFlags |= AI_MEDIC;
			FoundTarget ();
		}
	}
}

void CMedic::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

CFrame MedicFramesStand [] =
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
};
CAnim MedicMoveStand (CMedic::FRAME_wait1, CMedic::FRAME_wait90, MedicFramesStand);

void CMedic::Stand ()
{
	CurrentMove = &MedicMoveStand;
}

CFrame MedicFramesWalk [] =
{
	CFrame (&IMonster::AI_Walk, 6.2f),
	CFrame (&IMonster::AI_Walk, 18.1f),
	CFrame (&IMonster::AI_Walk, 1),
	CFrame (&IMonster::AI_Walk, 9),
	CFrame (&IMonster::AI_Walk, 10),
	CFrame (&IMonster::AI_Walk, 9),
	CFrame (&IMonster::AI_Walk, 11),
	CFrame (&IMonster::AI_Walk, 11.6f),
	CFrame (&IMonster::AI_Walk, 2),
	CFrame (&IMonster::AI_Walk, 9.9f),
	CFrame (&IMonster::AI_Walk, 14),
	CFrame (&IMonster::AI_Walk, 9.3f)
};
CAnim MedicMoveWalk (CMedic::FRAME_walk1, CMedic::FRAME_walk12, MedicFramesWalk);

void CMedic::Walk ()
{
	CurrentMove = &MedicMoveWalk;
}

CFrame MedicFramesRun [] =
{
	CFrame (&IMonster::AI_Run, 18),
	CFrame (&IMonster::AI_Run, 22.5f),
	CFrame (&IMonster::AI_Run, 25.4f
#if ROGUE_FEATURES
	, &IMonster::DoneDodge
#endif
	),
	CFrame (&IMonster::AI_Run, 23.4f),
	CFrame (&IMonster::AI_Run, 24),
	CFrame (&IMonster::AI_Run, 35.6f)
};
CAnim MedicMoveRun (CMedic::FRAME_run1, CMedic::FRAME_run6, MedicFramesRun);

void CMedic::Run ()
{
	if (!(AIFlags & AI_MEDIC))
	{
		CMonsterEntity *Found = FindDeadMonster();
		if (Found)
		{
			Entity->OldEnemy = Entity->Enemy;
			Entity->Enemy = Found;
#if ROGUE_FEATURES
			Found->Monster->Healer = Entity;
#endif
			AIFlags |= AI_MEDIC;
			FoundTarget ();
			return;
		}
	}

	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &MedicMoveStand : &MedicMoveRun;
}

CFrame MedicFramesPain1 [] =
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
CAnim MedicMovePain1 (CMedic::FRAME_paina1, CMedic::FRAME_paina8, MedicFramesPain1, &IMonster::Run);

CFrame MedicFramesPain2 [] =
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
	CFrame (&IMonster::AI_Move, 0)
};
CAnim MedicMovePain2 (CMedic::FRAME_painb1, CMedic::FRAME_painb15, MedicFramesPain2, &IMonster::Run);

void CMedic::Pain(IBaseEntity *Other, sint32 Damage)
{
#if ROGUE_FEATURES
	DoneDodge ();
#endif

	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() |= 1;

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;

	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

#if ROGUE_FEATURES
	// if we're healing someone, we ignore pain
	if (AIFlags & AI_MEDIC)
		return;

	// PMM - clear duck flag
	if (AIFlags & AI_DUCKED)
		UnDuck ();
#endif

	float r = frand();
	CurrentMove = (r < 0.5) ? &MedicMovePain1 : &MedicMovePain2;
	Entity->PlaySound (CHAN_VOICE, (r < 0.5) ? Sounds[SOUND_PAIN1] : Sounds[SOUND_PAIN2]);
}

void CMedic::FireBlaster ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start;
	vec3f	end;
	vec3f	dir;
	sint32		effect = 0;

	switch (Entity->State.GetFrame())
	{
	case CMedic::FRAME_attack9:
	case CMedic::FRAME_attack12:
		effect = FX_BLASTER;
		break;
	case CMedic::FRAME_attack19:
	case CMedic::FRAME_attack22:
	case CMedic::FRAME_attack25:
	case CMedic::FRAME_attack28:
		effect = FX_HYPERBLASTER;
		break;
	default:
		break;
	};

	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_MEDIC_BLASTER_1], Entity->State.GetAngles().ToVectors(), start);

	end = Entity->Enemy->State.GetOrigin();
	end.Z += Entity->Enemy->ViewHeight;
	dir = end - start;

	MonsterFireBlaster (start, dir, 2, 1000, MZ2_MEDIC_BLASTER_1, effect);
}

void CMedic::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame MedicFramesDeath [] =
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
	CFrame (&IMonster::AI_Move, 0)
};
CAnim MedicMoveDeath (CMedic::FRAME_death1, CMedic::FRAME_death30, MedicFramesDeath, ConvertDerivedFunction(&CMedic::Dead));

void CMedic::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	// if we had a pending patient, free him up for another medic
#if ROGUE_FEATURES
	if ((Entity->Enemy) &&
		(Entity->Enemy->EntityFlags & EF_MONSTER ) && 
		((entity_cast<CMonsterEntity>(*Entity->Enemy))->Monster->Healer == Entity))
		(entity_cast<CMonsterEntity>(*Entity->Enemy))->Monster->Healer = nullentity;
#endif

// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (sint32 n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_Bone[0], Damage, GIB_ORGANIC);
		for (sint32 n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
		Entity->ThrowHead(GameMedia.Gib_Head[1], Damage, GIB_ORGANIC);
		Entity->IsDead = true;
		return;
	}

	if (Entity->IsDead == true)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DIE]);
	Entity->IsDead = true;
	Entity->CanTakeDamage = true;

	CurrentMove = &MedicMoveDeath;
}

CFrame MedicFramesAttackHyperBlaster [] =
{
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster))
};
CAnim MedicMoveAttackHyperBlaster (CMedic::FRAME_attack15, CMedic::FRAME_attack30, MedicFramesAttackHyperBlaster, &IMonster::Run);

void CMedic::ContinueFiring ()
{
	if (IsVisible (Entity, *Entity->Enemy) && (frand() <= 0.95))
		CurrentMove = &MedicMoveAttackHyperBlaster;
}

CFrame MedicFramesAttackBlaster [] =
{
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 5),
	CFrame (&IMonster::AI_Charge, 5),
	CFrame (&IMonster::AI_Charge, 3),
	CFrame (&IMonster::AI_Charge, 2),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),	
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::ContinueFiring))	// Change to medic_continue... Else, go to frame 32
};
CAnim MedicMoveAttackBlaster (CMedic::FRAME_attack1, CMedic::FRAME_attack14, MedicFramesAttackBlaster, &IMonster::Run);

void CMedic::HookLaunch ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_HOOK_LAUNCH]);
}

static vec3f	MedicCableOffsets[] =
{
	vec3f(45.0f,  -9.2f, 15.5f),
	vec3f(48.4f,  -9.7f, 15.2f),
	vec3f(47.8f,  -9.8f, 15.8f),
	vec3f(47.3f,  -9.3f, 14.3f),
	vec3f(45.4f, -10.1f, 13.1f),
	vec3f(41.9f, -12.7f, 12.0f),
	vec3f(37.8f, -15.8f, 11.2f),
	vec3f(34.3f, -18.4f, 10.7f),
	vec3f(32.7f, -19.7f, 10.4f),
	vec3f(32.7f, -19.7f, 10.4f)
};

#include "Utility/TemporaryEntities.h"

void CMedic::CableAttack ()
{
	vec3f	offset, start, end;
	CTrace	tr;
	vec3f	dir, angles;
	float	distance;

#if !ROGUE_FEATURES
	if (!Entity->Enemy)
		return;
#else
	if ((!Entity->Enemy) || (Entity->Enemy->State.GetEffects() & FX_GIB))
	{
		AbortHeal (false, false);
		return;
	}

	// see if our enemy has changed to a client, or our target has more than 0 health,
	// abort it .. we got switched to someone else due to damage
	if ((Entity->Enemy->EntityFlags & EF_PLAYER) || (entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0))
	{
		AbortHeal (false, false);
		return;
	}
#endif

	if (!(Entity->Enemy->EntityFlags & EF_MONSTER))
		return;

	offset = MedicCableOffsets[Entity->State.GetFrame() - CMedic::FRAME_attack42];
	G_ProjectSource (Entity->State.GetOrigin(), offset, Entity->State.GetAngles().ToVectors (), start);

	// check for max distance
	dir = start - Entity->Enemy->State.GetOrigin();
	distance = dir.Length();

	// According to Rogue, not needed; done in CheckAttack
	//if (distance > 256)
	//	return;
	if (distance < MEDIC_MIN_DISTANCE)
	{
#if ROGUE_FEATURES
		AbortHeal (true, false);
#endif
		return;
	}

	// check for min/max pitch
	angles = dir.ToAngles();
	if (angles.X < -180)
		angles.X += 360;
	if (fabs(angles.X) > 45)
		return;

#if !ROGUE_FEATURES
	tr (start, Entity->Enemy->State.GetOrigin(), Entity, CONTENTS_MASK_SHOT);
	if (tr.Fraction != 1.0 && tr.Entity != Entity->Enemy)
		return;
#else
	tr (start, Entity->Enemy->State.GetOrigin(), Entity, CONTENTS_MASK_SHOT);

	if (tr.Fraction != 1.0 && tr.Entity != Entity->Enemy)
	{
		if (tr.Entity == World)
		{
			// give up on second try
			if (MedicTries > 1)
			{
				AbortHeal (false, true);
				return;
			}
			MedicTries++;
			CleanupHeal (true);
			return;
		}
		AbortHeal (false, false);
		return;
	}
#endif

	CMonsterEntity *Monster;
	switch (Entity->State.GetFrame())
	{
	case CMedic::FRAME_attack43:
		Entity->PlaySound (CHAN_AUTO, Sounds[SOUND_HOOK_HIT]);
		(entity_cast<CMonsterEntity>(*Entity->Enemy))->Monster->AIFlags |= AI_RESURRECTING;

		Entity->Enemy->State.GetEffects() = FX_PENT;
		break;
	case CMedic::FRAME_attack50:
		Entity->Enemy->SpawnFlags = 0;
		Monster = (entity_cast<CMonsterEntity>(*Entity->Enemy));
		Monster->DeathTarget.Clear();
		Monster->CombatTarget.Clear();
#if ROGUE_FEATURES
		Monster->Monster->Healer = Entity;
#endif
		Monster->Monster->AIFlags = 0;
		Monster->Target.Clear();
		Monster->TargetName.Clear();

#if !ROGUE_FEATURES
		Monster->Monster->Spawn ();
		Monster->NextThink = Level.Frame;
		Monster->Think ();
		Monster->Monster->AIFlags &= ~AI_RESURRECTING;
		Monster->Enemy = nullentity;
		// Paril, fix skinnum
		if (Monster->State.GetSkinNum() & 1)
			Monster->State.GetSkinNum() -= 1;
		Monster->PhysicsType = PHYSICS_STEP;
		Monster->AffectedByKnockback = true;
		Monster->GetSvFlags() |= SVF_MONSTER;
		Monster->GetSolid() = SOLID_BBOX;
		Monster->Link ();

		if (Entity->OldEnemy && (Entity->OldEnemy->EntityFlags & EF_PLAYER))
		{
			Monster->Enemy = Entity->OldEnemy;
			Monster->Monster->FoundTarget ();
		}
#else
		{
			vec3f maxs = Entity->Enemy->GetMaxs() + vec3f(0, 0, 48);
			tr (Entity->Enemy->State.GetOrigin(), Entity->Enemy->GetMins(), maxs, Entity->Enemy->State.GetOrigin(), *Entity->Enemy, CONTENTS_MASK_MONSTERSOLID);
		}

		if (tr.StartSolid || tr.AllSolid)
		{
			AbortHeal (true, false);
			return;
		} 
		else if (tr.Entity != World)
		{
			AbortHeal (true, false);
			return;
		}
		else
		{
			Monster->Monster->Spawn ();
			Monster->Monster->Healer = nullentity;
			Monster->NextThink = Level.Frame;
			Monster->Think ();
			Monster->Monster->AIFlags &= ~AI_RESURRECTING;
			Monster->Enemy = nullentity;

			// Paril, fix skinnum
			Monster->State.GetSkinNum() &= ~1;
			Monster->PhysicsType = PHYSICS_STEP;
			Monster->AffectedByKnockback = true;
			Monster->GetSvFlags() |= SVF_MONSTER;
			Monster->GetSolid() = SOLID_BBOX;
			Monster->Link ();

			if (Entity->OldEnemy && (Entity->OldEnemy->EntityFlags & EF_PLAYER))
			{
				Monster->Enemy = Entity->OldEnemy;
				Monster->Monster->FoundTarget ();
			}
			else
			{
				Entity->Enemy->Enemy = nullentity;
				if (!entity_cast<CMonsterEntity>(*Entity->Enemy)->Monster->FindTarget ())
				{
					// no valid enemy, so stop acting
					Monster->Monster->PauseTime = Level.Frame + 100000000;
					Monster->Monster->Stand ();
				}
				Entity->Enemy = nullentity;
				Entity->OldEnemy = nullentity;
				if (!FindTarget ())
				{
					// no valid enemy, so stop acting
					PauseTime = Level.Frame + 100000000;
					Stand ();
					return;
				}
			}
		}
#endif
		break;
	case CMedic::FRAME_attack44:
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_HOOK_HEAL]);
	default:
		break;
	}
	// adjust start for beam origin being in middle of a segment
	start = start.MultiplyAngles (8, Entity->State.GetAngles().ToVectors().Forward);

	// adjust end z for end spot since the monster is currently dead
	end = Entity->Enemy->State.GetOrigin();
	end.Z = Entity->Enemy->GetAbsMin().Z + Entity->Enemy->GetSize().Z / 2;

	CFleshCable(start, end, Entity->State.GetNumber()).Send();
}

void CMedic::HookRetract ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_HOOK_RETRACT]);
#if !ROGUE_FEATURES
	if (Entity->Enemy && (Entity->Enemy->EntityFlags & EF_MONSTER))
		(entity_cast<CMonsterEntity>(*Entity->Enemy))->Monster->AIFlags &= ~AI_RESURRECTING;
#endif
}

CFrame MedicFramesAttackCable [] =
{
// Only in rogue:
// ROGUE - negated 36-40 so he scoots back from his target a little
// ROGUE - switched 33-36 to ai_charge
// ROGUE - changed frame 52 to 0 to compensate for changes in 36-40

#if ROGUE_FEATURES
	CFrame (&IMonster::AI_Charge, 2),
	CFrame (&IMonster::AI_Charge, 3),
	CFrame (&IMonster::AI_Charge, 5),
#else
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 3),
	CFrame (&IMonster::AI_Move, 5),
#endif

#if ROGUE_FEATURES
	CFrame (&IMonster::AI_Charge, -4.4f), // 36
	CFrame (&IMonster::AI_Charge, -4.7f),
	CFrame (&IMonster::AI_Charge, -5),
	CFrame (&IMonster::AI_Charge, -6),
	CFrame (&IMonster::AI_Charge, -4), // 40
#else
	CFrame (&IMonster::AI_Move, 4.4f), // 36
	CFrame (&IMonster::AI_Charge, 4.7f),
	CFrame (&IMonster::AI_Charge, 5),
	CFrame (&IMonster::AI_Charge, 6),
	CFrame (&IMonster::AI_Charge, 4), // 40
#endif
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::HookLaunch)),
	CFrame (&IMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&IMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&IMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&IMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&IMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&IMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&IMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&IMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&IMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&IMonster::AI_Move, 
#if ROGUE_FEATURES
	0
#else
	-15
#endif
	,	ConvertDerivedFunction(&CMedic::HookRetract)),
	CFrame (&IMonster::AI_Move, -1.5f),
	CFrame (&IMonster::AI_Move, -1.2f),
	CFrame (&IMonster::AI_Move, -3),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, 0.3f),
	CFrame (&IMonster::AI_Move, 0.7f),
	CFrame (&IMonster::AI_Move, 1.2f),
	CFrame (&IMonster::AI_Move, 1.3f)
};
CAnim MedicMoveAttackCable (CMedic::FRAME_attack33, CMedic::FRAME_attack60, MedicFramesAttackCable, &IMonster::Run);

void CMedic::Attack()
{
#if ROGUE_FEATURES
	DoneDodge ();
#endif

	CurrentMove = (AIFlags & AI_MEDIC) ? &MedicMoveAttackCable : &MedicMoveAttackBlaster;
}

bool CMedic::CheckAttack ()
{
	if (AIFlags & AI_MEDIC)
	{
#if !ROGUE_FEATURES
		if ((Entity->Enemy->EntityFlags & EF_MONSTER) && (entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > entity_cast<IHurtableEntity>(*Entity->Enemy)->GibHealth))
		{
			Attack();
			return true;
		}
		else
			AIFlags &= ~AI_MEDIC;
#else
		// if our target went away
		if (!Entity->Enemy)
		{
			AbortHeal (false, false);
			return false;
		}

		// if we ran out of time, give up
		if (MedicTryTime < Level.Frame)
		{
			AbortHeal (false, true);
			MedicTryTime = 0;
			return false;
		}
	
		if (RangeFrom(Entity->State.GetOrigin(), Entity->Enemy->State.GetOrigin()) < MEDIC_MAX_HEAL_DISTANCE+10)
		{
			Attack ();
			return true;
		}
		else
		{
			AttackState = AS_STRAIGHT;
			return false;
		}
#endif
	}

#if ROGUE_FEATURES
	// ROGUE
	// since his idle animation looks kinda bad in combat, if we're not in easy mode, always attack
	// when he's on a combat point
	if ((CvarList[CV_SKILL].Integer() > 0) && (AIFlags & AI_STAND_GROUND))
	{
		AttackState = AS_MISSILE;
		return true;
	}
#endif

	return IMonster::CheckAttack ();
}

#if !ROGUE_FEATURES
void CMedic::Duck_Down ()
{
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	PauseTime = Level.Frame + 10;
	Entity->GetMins().Z -= 32;
	Entity->Link ();
}

void CMedic::Duck_Hold ()
{
	if (Level.Frame >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CMedic::Duck_Up ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->GetMins().Z += 32;
	Entity->Link ();
}
#endif


CFrame MedicFramesDuck [] =
{
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1),
#if ROGUE_FEATURES
	CFrame (&IMonster::AI_Move, -1, &IMonster::DuckDown),
	CFrame (&IMonster::AI_Move, -1,	&IMonster::DuckHold),
#else
	CFrame (&IMonster::AI_Move, -1, ConvertDerivedFunction(&CMedic::Duck_Down)),
	CFrame (&IMonster::AI_Move, -1,	ConvertDerivedFunction(&CMedic::Duck_Hold)),
#endif
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1
#if ROGUE_FEATURES
	,	&IMonster::UnDuck // in Rogue AI, the UP is down
#else
	,	ConvertDerivedFunction(&CMedic::Duck_Up)
#endif
	),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1
#if ROGUE_FEATURES
	, &IMonster::UnDuck
#endif
	),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1)
};
CAnim MedicMoveDuck (CMedic::FRAME_duck1, CMedic::FRAME_duck16, MedicFramesDuck, &IMonster::Run);

#if !ROGUE_FEATURES
void CMedic::Dodge (IBaseEntity *Attacker, float eta)
{
	if (frand() > 0.25)
		return;

	if (!Entity->Enemy && Entity->Enemy->EntityFlags & EF_HURTABLE)
		Entity->Enemy = Attacker;

	CurrentMove = &MedicMoveDuck;
}
#else
void CMedic::Duck (float eta)
{
//	don't dodge if you're healing
	if (AIFlags & AI_MEDIC)
		return;

	if ((CurrentMove == &MedicMoveAttackHyperBlaster) ||
		(CurrentMove == &MedicMoveAttackCable) ||
		(CurrentMove == &MedicMoveAttackBlaster))
	{
		// he ignores skill
		AIFlags &= ~AI_DUCKED;
		return;
	}

	DuckWaitTime = Level.Frame + ((CvarList[CV_SKILL].Integer() == 0) ? ((eta + 1) * 10) : ((eta + (0.1 * (3 - CvarList[CV_SKILL].Integer()))) * 10));

	// has to be done immediately otherwise he can get stuck
	DuckDown();

	NextFrame = CMedic::FRAME_duck1;
	CurrentMove = &MedicMoveDuck;
}

void CMedic::SideStep ()
{
	if ((CurrentMove == &MedicMoveAttackHyperBlaster) ||
		(CurrentMove == &MedicMoveAttackCable) ||
		(CurrentMove == &MedicMoveAttackBlaster))
	{
		// if we're shooting, and not on easy, don't dodge
		if (CvarList[CV_SKILL].Integer())
		{
			AIFlags &= ~AI_DODGING;
			return;
		}
	}

	if (CurrentMove != &MedicMoveRun)
		CurrentMove = &MedicMoveRun;
}
#endif

/*QUAKED monster_medic (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void CMedic::Spawn ()
{
	Sounds[SOUND_IDLE] = SoundIndex ("medic/idle.wav");
	Sounds[SOUND_PAIN1] = SoundIndex ("medic/medpain1.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("medic/medpain2.wav");
	Sounds[SOUND_DIE] = SoundIndex ("medic/meddeth1.wav");
	Sounds[SOUND_SIGHT] = SoundIndex ("medic/medsght1.wav");
	Sounds[SOUND_SEARCH] = SoundIndex ("medic/medsrch1.wav");
	Sounds[SOUND_HOOK_LAUNCH] = SoundIndex ("medic/medatck2.wav");
	Sounds[SOUND_HOOK_HIT] = SoundIndex ("medic/medatck3.wav");
	Sounds[SOUND_HOOK_HEAL] = SoundIndex ("medic/medatck4.wav");
	Sounds[SOUND_HOOK_RETRACT] = SoundIndex ("medic/medatck5.wav");

	SoundIndex ("medic/medatck1.wav");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/medic/tris.md2");
	Entity->GetMins().Set (-24, -24, -24);
	Entity->GetMaxs().Set (24, 24, 32);

	Entity->Health = 300;
	Entity->GibHealth = -130;
	Entity->Mass = 400;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_SIGHT | MF_HAS_IDLE | MF_HAS_SEARCH
#if ROGUE_FEATURES
		| MF_HAS_DODGE | MF_HAS_DUCK | MF_HAS_UNDUCK | MF_HAS_SIDESTEP
#endif
		);

	Entity->Link ();
	CurrentMove = &MedicMoveStand;

	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_medic", CMedic);
