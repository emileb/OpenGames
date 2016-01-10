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
// cc_xatrix_fixbot.cpp
// 
//

#include "Local.h"
#include "Xatrix/XatrixFixbot.h"
#include "Utility/TemporaryEntities.h"

#if XATRIX_FEATURES

CFixbot::CFixbot (uint32 ID) :
IMonster (ID)
{
};

#define MZ2_FIXBOT_BLASTER_1				MZ2_HOVER_BLASTER_1

CMonsterEntity *CFixbot::FindDeadMonster ()
{
	CMonsterEntity *FoundMonster = NULL, *BestMonster = NULL;

	while ((FoundMonster = FindRadius<CMonsterEntity, EF_MONSTER>(FoundMonster, Entity->State.GetOrigin(), 1024)) != NULL)
	{
		if (FoundMonster == Entity)
			continue;
		if (FoundMonster->Monster->AIFlags & AI_GOOD_GUY)
			continue;
		if (FoundMonster->Health > 0)
			continue;
		if (FoundMonster->NextThink)
			continue;
		if (!IsVisible(Entity, FoundMonster))
			continue;
		if (!BestMonster)
		{
			BestMonster = FoundMonster;
			continue;
		}
		if (FoundMonster->MaxHealth <= BestMonster->MaxHealth)
			continue;
		BestMonster = FoundMonster;
	}

	return BestMonster;
}

bool CFixbot::SearchForMonsters ()
{
	if (!Entity->GoalEntity)
	{
		CMonsterEntity *Ent = FindDeadMonster ();
		if (Ent)
		{
			Entity->OldEnemy = Entity->Enemy;
			Entity->Enemy = Ent;
			Entity->Enemy->SetOwner(Entity);
			AIFlags |= AI_MEDIC;
			FoundTarget ();
			return true;
		}
	}
	return false;
}

class CBotGoal : public IBaseEntity
{
public:
	CBotGoal () :
	  IBaseEntity ()
	  {
	  };

	CBotGoal (sint32 Index) : 
	  IBaseEntity (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER (CBotGoal);
};

CFrame FixbotFramesLanding [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),

	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),

	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),

	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),

	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),

	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2)),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::FlyVertical2))
};
CAnim FixbotMoveLanding (CFixbot::FRAME_landing_01, CFixbot::FRAME_landing_58, FixbotFramesLanding);

/*
	generic ambient stand
*/
CFrame FixbotFramesStand [] =
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
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::ChangeToRoam))
};
CAnim FixbotMoveStand (CFixbot::FRAME_ambient_01, CFixbot::FRAME_ambient_19, FixbotFramesStand);

CFrame FixbotFramesStand2 [] =
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
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim FixbotMoveStand2 (CFixbot::FRAME_ambient_01, CFixbot::FRAME_ambient_19, FixbotFramesStand2);

/*
	generic frame to move bot
*/
CFrame FixbotFramesRoamGoal [] =
{
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CFixbot::RoamGoal))
};
CAnim FixbotMoveRoamGoal (CFixbot::FRAME_freeze_01, CFixbot::FRAME_freeze_01, FixbotFramesRoamGoal);

CFrame FixbotFramesTurn [] =
{
	CFrame (ConvertDerivedAIMove (&CFixbot::AI_Facing),	0)
};
CAnim FixbotMoveTurn (CFixbot::FRAME_freeze_01, CFixbot::FRAME_freeze_01, FixbotFramesTurn);

void CFixbot::GoRoam ()
{
	CurrentMove = &FixbotMoveStand;	
}

/*
	takeoff
*/
CFrame FixbotFramesTakeOff [] =
{
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),

	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical)),
	CFrame (&IMonster::AI_Move, 0.01f,	ConvertDerivedFunction (&CFixbot::FlyVertical))
};
CAnim FixbotMoveTakeOff (CFixbot::FRAME_takeoff_01, CFixbot::FRAME_takeoff_16, FixbotFramesTakeOff);

/* findout what this is */
CFrame FixbotFramesPain1 [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim FixbotMovePain1 (CFixbot::FRAME_paina_01, CFixbot::FRAME_paina_06, FixbotFramesPain1, &IMonster::Run);

/* findout what this is */
CFrame FixbotFramesPain2 [] =
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
CAnim FixbotMovePain2 (CFixbot::FRAME_painb_01, CFixbot::FRAME_painb_08, FixbotFramesPain2, &IMonster::Run);

/*
	backup from pain
	call a generic painsound
	some spark effects
*/
CFrame FixbotFramesPain3 [] =
{
	CFrame (&IMonster::AI_Move, -1)
};
CAnim FixbotMovePain3 (CFixbot::FRAME_freeze_01, CFixbot::FRAME_freeze_01, FixbotFramesPain3, &IMonster::Run);

/*
	bot has completed landing
	and is now on the grownd
	( may need second land if the bot is releasing jib into jib vat )
*/
CFrame FixbotFramesLand [] =
{
	CFrame (&IMonster::AI_Move, 0)
};
CAnim FixbotMoveLand (CFixbot::FRAME_freeze_01, CFixbot::FRAME_freeze_01, FixbotFramesLand);

void CFixbot::AI_MoveToGoal (float dist)
{
	MoveToGoal (dist);
}
/*
	
*/
CFrame FixbotFramesForward [] =
{
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_MoveToGoal),	5,	ConvertDerivedFunction(&CFixbot::UseScanner))
};
CAnim FixbotMoveForward (CFixbot::FRAME_freeze_01, CFixbot::FRAME_freeze_01, FixbotFramesForward);

void CFixbot::AI_Facing (float dist)
{
	if (IsInFront (Entity, *Entity->GoalEntity))
		CurrentMove = &FixbotMoveForward;
	else
	{
		IdealYaw = (Entity->GoalEntity->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();	
		ChangeYaw ();
	}
};

/*
	
*/
CFrame FixbotFramesWalk [] =
{
	CFrame (&IMonster::AI_Walk,	5)
};
CAnim FixbotMoveWalk (CFixbot::FRAME_freeze_01, CFixbot::FRAME_freeze_01, FixbotFramesWalk);

/*
	
*/
CFrame FixbotFramesRun [] =
{
	CFrame (&IMonster::AI_Run,	10)
};
CAnim FixbotMoveRun (CFixbot::FRAME_freeze_01, CFixbot::FRAME_freeze_01, FixbotFramesRun);

//
CFrame FixbotFramesBackward [] =
{
	CFrame (&IMonster::AI_Move, 0)
};
CAnim FixbotMoveBackward (CFixbot::FRAME_freeze_01, CFixbot::FRAME_freeze_01, FixbotFramesBackward);

/*
	TBD: 
	need to get laser attack anim
	attack with the laser blast
*/
CFrame FixbotFramesAttack1 [] =
{
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	-10,  ConvertDerivedFunction(&CFixbot::FireBlaster)),
};
CAnim FixbotMoveAttack1 (CFixbot::FRAME_shoot_01, CFixbot::FRAME_shoot_06, FixbotFramesAttack1);

bool CFixbot::CheckTelefrag ()
{
	vec3f	start;

	anglef angles = Entity->Enemy->State.GetAngles().ToVectors ();
	start = start.MultiplyAngles (48, angles.Up);
	CTrace tr (Entity->Enemy->State.GetOrigin(), Entity->Enemy->GetMins(), Entity->Enemy->GetMaxs(), start, Entity, CONTENTS_MASK_MONSTERSOLID);
	if (tr.Entity->EntityFlags & EF_HURTABLE)
	{
		entity_cast<IHurtableEntity>(tr.Entity)->TakeDamage (Entity, Entity, vec3fOrigin, Entity->Enemy->State.GetOrigin(), vec3fOrigin, 999999, 0, DAMAGE_NO_PROTECTION, MOD_UNKNOWN);
		return false;
	}
			
	return true;
}

void CFixbot::FireLaser ()
{
	if (!HasValidEnemy())
	{
		CurrentMove = &FixbotMoveStand;
		AIFlags &= ~AI_MEDIC;
		return;
	}

	vec3f tempang, start, dir, angles, end;

	CMonsterEntity *Enemy = entity_cast<CMonsterEntity>(*Entity->Enemy);

	// critter dun got blown up while bein' fixed
	if (Enemy->Health <= Enemy->GibHealth)
	{
		CurrentMove = &FixbotMoveStand;
		AIFlags &= ~AI_MEDIC;
		return;
	}

	Entity->PlaySound (CHAN_AUTO, SoundIndex("misc/lasfly.wav"), 255, ATTN_STATIC);

	start = Entity->State.GetOrigin();
	end = Enemy->State.GetOrigin();
	dir = end - start;
	angles = dir.ToAngles();

	CMonsterBeamLaser *Laser = QNewEntityOf CMonsterBeamLaser;
	Laser->State.GetOrigin() = Entity->State.GetOrigin();
	Laser->State.GetAngles() = angles;
	Laser->State.GetOrigin() = Laser->State.GetOrigin().MultiplyAngles (16, angles.ToVectors().Forward);

	Laser->Enemy = Entity->Enemy;
	Laser->SetOwner(Entity);
	Laser->Damage = -1;
	MonsterFireBeam (Laser);

	if (Enemy->Health > (Enemy->Mass / 10))
	{
		// sorry guys but had to fix the problem this way
		// if it doesn't do this then two creatures can share the same space
		// and its real bad.
		if (CheckTelefrag ()) 
		{
			Enemy->SpawnFlags = 0;
			Enemy->Monster->AIFlags = 0;
			Enemy->Target.Clear();
			Enemy->TargetName.Clear();
			Enemy->CombatTarget.Clear();
			Enemy->DeathTarget.Clear();
			Enemy->SetOwner(Entity);
			Enemy->Monster->Spawn ();
			Enemy->SetOwner(NULL);
			Entity->State.GetOrigin().Z += 1;
			Enemy->PhysicsType = PHYSICS_STEP;
			Enemy->AffectedByKnockback = true;
			Enemy->GetSvFlags() |= SVF_MONSTER;
			Enemy->GetSolid() = SOLID_BBOX;
			Enemy->Link ();
		
			Enemy->Monster->AIFlags &= ~AI_RESURRECTING;

			CurrentMove = &FixbotMoveStand;
			AIFlags &= ~AI_MEDIC;
		}
	}
	else
		Enemy->Monster->AIFlags |= AI_RESURRECTING;
}

CFrame FixbotFramesLaserAttack [] =
{
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFixbot::FireLaser)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFixbot::FireLaser)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFixbot::FireLaser)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFixbot::FireLaser)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFixbot::FireLaser)),
	CFrame (&IMonster::AI_Charge,	0,  ConvertDerivedFunction(&CFixbot::FireLaser))
};
CAnim FixbotMoveLaserAttack (CFixbot::FRAME_shoot_01, CFixbot::FRAME_shoot_06, FixbotFramesLaserAttack);

/*
	need to get forward translation data
	for the charge attack
*/
CFrame FixbotFramesAttack2 [] =
{
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	 0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	 0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	 0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	 0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	 0),

	CFrame (&IMonster::AI_Charge,	-10),
	CFrame (&IMonster::AI_Charge,	 -10),
	CFrame (&IMonster::AI_Charge,	-10),
	CFrame (&IMonster::AI_Charge,	 -10),
	CFrame (&IMonster::AI_Charge,	-10),
	CFrame (&IMonster::AI_Charge,	 -10),
	CFrame (&IMonster::AI_Charge,	-10),
	CFrame (&IMonster::AI_Charge,	 -10),
	CFrame (&IMonster::AI_Charge,	-10),
	CFrame (&IMonster::AI_Charge,	 -10),

	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFixbot::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	 0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	 0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	 0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	 0),
	CFrame (&IMonster::AI_Charge,	 0),
	CFrame (&IMonster::AI_Charge,	 0),

	CFrame (&IMonster::AI_Charge,	 0)
};
CAnim FixbotMoveAttack2 (CFixbot::FRAME_charging_01, CFixbot::FRAME_charging_31, FixbotFramesAttack2, &IMonster::Run);

void CFixbot::AI_Move2 (float dist)
{
	vec3f v;
	
	if (dist)
		WalkMove (Entity->State.GetAngles().Y, dist);

	IdealYaw = (Entity->GoalEntity->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();	
	ChangeYaw ();
};

CFrame FixbotFramesWeldStart [] = 
{
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0, ConvertDerivedFunction(&CFixbot::WeldState))
};
CAnim FixbotMoveWeldStart (CFixbot::FRAME_weldstart_01, CFixbot::FRAME_weldstart_10, FixbotFramesWeldStart);

CFrame FixbotFramesWeld [] =
{
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0, ConvertDerivedFunction(&CFixbot::FireWelder)),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0, ConvertDerivedFunction(&CFixbot::FireWelder)),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0, ConvertDerivedFunction(&CFixbot::FireWelder)),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0, ConvertDerivedFunction(&CFixbot::FireWelder)),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0, ConvertDerivedFunction(&CFixbot::FireWelder)),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0, ConvertDerivedFunction(&CFixbot::FireWelder)),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	0, ConvertDerivedFunction(&CFixbot::WeldState))
};
CAnim FixbotMoveWeld (CFixbot::FRAME_weldmiddle_01, CFixbot::FRAME_weldmiddle_07, FixbotFramesWeld);

CFrame FixbotFramesWeldEnd [] =
{
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	-2),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	-2),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	-2),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	-2),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	-2),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	-2),
	CFrame (ConvertDerivedAIMove(&CFixbot::AI_Move2),	-2, ConvertDerivedFunction(&CFixbot::WeldState))
};
CAnim FixbotMoveWeldEnd (CFixbot::FRAME_weldend_01, CFixbot::FRAME_weldend_07, FixbotFramesWeldEnd);

void CFixbot::WeldState ()
{
	switch (Entity->State.GetFrame())
	{
	case CFixbot::FRAME_weldstart_10:
		CurrentMove = &FixbotMoveWeld;
		break;
	case CFixbot::FRAME_weldmiddle_07:
		if (entity_cast<IHurtableEntity>(*Entity->GoalEntity)->Health < 0) 
		{
			Entity->Enemy->SetOwner(NULL);
			CurrentMove = &FixbotMoveWeldEnd;
		}
		else
			entity_cast<IHurtableEntity>(*Entity->GoalEntity)->Health -= 10;
		break;
	default:
		Entity->GoalEntity = Entity->Enemy = nullentity;
		CurrentMove = &FixbotMoveStand;
		break;
	};
}

void CFixbot::FireWelder ()
{
	vec3f	start;
	
	if (!Entity->Enemy)
		return;

	static const vec3f vec (24, -0.8f, -10);

	G_ProjectSource (Entity->State.GetOrigin(), vec, Entity->State.GetAngles().ToVectors(), start);

	vec3f dir = Entity->Enemy->State.GetOrigin() - start;

	CSparks(start, vec3fOrigin, ST_WELDING_SPARKS, 0xe0 + irandom(7), 10).Send();

	if (frand() > 0.8)
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_WELD1 + irandom(3)], 255, ATTN_IDLE); 
}

void CFixbot::FireBlaster ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start, end, dir;

	if (!IsVisible(Entity, *Entity->Enemy))
		CurrentMove = &FixbotMoveRun;
	
	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_FIXBOT_BLASTER_1], angles, start);

	dir = (Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight)) - start;

	MonsterFireBlaster (start, dir, 15, 1000, MZ2_FIXBOT_BLASTER_1, FX_BLASTER);
}

void CFixbot::LandingGoal ()
{
	vec3f end;

	CBotGoal *Goal = QNewEntityOf CBotGoal;	
	Goal->ClassName = "bot_goal";
	Goal->GetSolid() = SOLID_BBOX;
	Goal->SetOwner(Entity);
	Goal->Link ();
	
	Goal->GetMins().Set (-32, -32, -24);
	Goal->GetMaxs().Set (32, 32, 24);
	
	anglef angles = Entity->State.GetAngles().ToVectors ();
	end = Entity->State.GetOrigin().MultiplyAngles (32, angles.Forward).
									MultiplyAngles (-8096, angles.Up);
		
	CTrace tr (Entity->State.GetOrigin(), Goal->GetMins(), Goal->GetMaxs(), end, Entity, CONTENTS_MASK_MONSTERSOLID);

	Goal->State.GetAngles() = tr.EndPosition;
	
	Entity->GoalEntity = Entity->Enemy = Goal;
	CurrentMove = &FixbotMoveLanding;
}

void CFixbot::TakeOffGoal ()
{
	vec3f end;
	CBotGoal *Goal = QNewEntityOf CBotGoal;	
	
	anglef angles = Entity->State.GetAngles().ToVectors ();
	end = Entity->State.GetOrigin().MultiplyAngles (32, angles.Forward).
									MultiplyAngles (128, angles.Up);
		
	CTrace tr (Entity->State.GetOrigin(), Goal->GetMins(), Goal->GetMaxs(), end, Entity, CONTENTS_MASK_MONSTERSOLID);

	Goal->State.GetAngles() = tr.EndPosition;
	
	Entity->GoalEntity = Entity->Enemy = Goal;
	CurrentMove = &FixbotMoveTakeOff;
}

void CFixbot::ChangeToRoam ()
{
	if (SearchForMonsters ())
		return;

	CurrentMove = &FixbotMoveRoamGoal;

	if (Entity->SpawnFlags & 16)
	{
		LandingGoal ();
		CurrentMove = &FixbotMoveLanding;
		Entity->SpawnFlags = 32;
	}

	if (Entity->SpawnFlags & 8) 
	{
		TakeOffGoal ();
		CurrentMove = &FixbotMoveTakeOff;
		Entity->SpawnFlags = 32;
	}

	if (Entity->SpawnFlags & 4)
	{
		CurrentMove = &FixbotMoveRoamGoal;
		Entity->SpawnFlags = 32;
	}

	if (!Entity->SpawnFlags)
		CurrentMove = &FixbotMoveStand2;
}

void CFixbot::RoamGoal ()
{
	vec3f		end, vec, whichvec, dang;
	int			len;
		
	CBotGoal *Goal = QNewEntityOf CBotGoal;	
	Goal->ClassName = "bot_goal";
	Goal->GetSolid() = SOLID_BBOX;
	Goal->SetOwner(Entity);
	Goal->Link ();

	int oldlen = 0;
	for (int i = 0; i < 12; i++) 
	{
		dang = Entity->State.GetAngles();

		if (i < 6)
			dang[YAW] += 30 * i;
		else 
			dang[YAW] -= 30 * (i-6);

		anglef angles = dang.ToVectors ();
		end = Entity->State.GetOrigin().MultiplyAngles (8192, angles.Forward);
		
		CTrace tr (Entity->State.GetOldOrigin(), end, Entity, CONTENTS_MASK_SHOT);

		vec = Entity->State.GetOrigin() - tr.EndPosition;
		len = vec.Normalize ();

		if (len > oldlen)
		{
			oldlen = len;
			whichvec = tr.EndPosition;
		}
	}
	
	Goal->State.GetOrigin() = whichvec;
	Entity->GoalEntity = Entity->Enemy = Goal;
	
	CurrentMove = &FixbotMoveTurn;
}

void CFixbot::UseScanner ()
{
	IHurtableEntity *Hurtable = NULL;

	while ((Hurtable = FindRadius<IHurtableEntity, EF_HURTABLE> (Hurtable, Entity->State.GetOrigin(), 1024)) != NULL)
	{
		if (Hurtable->Health >= 100)
		{
			if (Hurtable->ClassName == "func_object_repair")
			{
				if (IsVisible (Hurtable, Entity))
				{	
					// remove the old one
					if (Entity->GoalEntity->ClassName == "bot_goal")
						Entity->GoalEntity->Free();
					
					Entity->GoalEntity = Entity->Enemy = Hurtable;

					if ((Entity->State.GetOrigin() - Entity->GoalEntity->State.GetOrigin()).Normalize() < 32)
						CurrentMove = &FixbotMoveWeldStart;

					return;
				}
			}
		}
	}

	vec3f vec = Entity->State.GetOrigin() - Entity->GoalEntity->State.GetOrigin();
	float len = vec.Length();

	if (len < 32)
	{
		if (Entity->GoalEntity->ClassName == "func_object_repair")
			CurrentMove = &FixbotMoveWeldStart;
		else 
		{
			Entity->GoalEntity->Free();
			Entity->GoalEntity = Entity->Enemy = nullentity;
			CurrentMove = &FixbotMoveStand;
		}
		return;
	}

	vec = Entity->State.GetOrigin() - Entity->State.GetOldOrigin();
	len = vec.Length();

	/* 
	  bot is stuck get new goalentity
	*/
	if (!len)
	{
		if (Entity->GoalEntity->ClassName == "func_object_repair")
			CurrentMove = &FixbotMoveStand;
		else 
		{
			Entity->GoalEntity->Free();
			Entity->GoalEntity = Entity->Enemy = nullentity;
			CurrentMove = &FixbotMoveStand;
		}
	}
}


/*
	when the bot has found a landing pad
	it will proceed to its goalentity
	just above the landing pad and
	decend translated along the z the current
	frames are at 10fps
*/ 

// Paril
// I commented this out because this seems to be unused code..
// I studied the fixbot's under different cirumstances and never found this being called or used
// in the maps.

void CFixbot::BlastOff (vec3f &start, vec3f &aimdir, sint32 Damage, int kick, int te_impact, int hspread, int vspread)
{
/*	trace_t		tr;
	vec3_t		dir;
	vec3_t		forward, right, up;
	vec3_t		end;
	float		r;
	float		u;
	vec3_t		water_start;
	qboolean	water = false;
	int			content_mask = MASK_SHOT | MASK_WATER;

	hspread+= (self->s.frame - CFixbot::FRAME_takeoff_01);
	vspread+= (self->s.frame - CFixbot::FRAME_takeoff_01);

	tr = gi.trace (self->s.origin, NULL, NULL, start, self, MASK_SHOT);
	if (!(tr.Fraction < 1.0))
	{
		vectoangles (aimdir, dir);
		AngleVectors (dir, forward, right, up);

		r = crandom()*hspread;
		u = crandom()*vspread;
		VectorMA (start, 8192, forward, end);
		VectorMA (end, r, right, end);
		VectorMA (end, u, up, end);

		if (gi.pointcontents (start) & MASK_WATER)
		{
			water = true;
			VectorCopy (start, water_start);
			content_mask &= ~MASK_WATER;
		}

		tr = gi.trace (start, NULL, NULL, end, self, content_mask);

		// see if we hit water
		if (tr.Contents & MASK_WATER)
		{
			int		color;

			water = true;
			VectorCopy (tr.endpos, water_start);

			if (!VectorCompare (start, tr.endpos))
			{
				if (tr.Contents & CONTENTS_WATER)
				{
					if (strcmp(tr.Surface->name, "*brwater") == 0)
						color = SPLASH_BROWN_WATER;
					else
						color = SPLASH_BLUE_WATER;
				}
				else if (tr.Contents & CONTENTS_SLIME)
					color = SPLASH_SLIME;
				else if (tr.Contents & CONTENTS_LAVA)
					color = SPLASH_LAVA;
				else
					color = SPLASH_UNKNOWN;

				if (color != SPLASH_UNKNOWN)
				{
					gi.WriteByte (svc_temp_entity);
					gi.WriteByte (TE_SPLASH);
					gi.WriteByte (8);
					gi.WritePosition (tr.endpos);
					gi.WriteDir (tr.Plane.normal);
					gi.WriteByte (color);
					gi.multicast (tr.endpos, MULTICAST_PVS);
				}

				// change bullet's course when it enters water
				VectorSubtract (end, start, dir);
				vectoangles (dir, dir);
				AngleVectors (dir, forward, right, up);
				r = crandom()*hspread*2;
				u = crandom()*vspread*2;
				VectorMA (water_start, 8192, forward, end);
				VectorMA (end, r, right, end);
				VectorMA (end, u, up, end);
			}

			// re-trace ignoring water this time
			tr = gi.trace (water_start, NULL, NULL, end, self, MASK_SHOT);
		}
	}

	// send gun puff / flash
	if (!((tr.Surface) && (tr.Surface->flags & SURF_SKY)))
	{
		if (tr.Fraction < 1.0)
		{
			if (tr.ent->takedamage)
			{
				T_Damage (tr.ent, self, self, aimdir, tr.endpos, tr.Plane.normal, damage, kick, DAMAGE_BULLET, MOD_BLASTOFF);
			}
			else
			{
				if (strncmp (tr.Surface->name, "sky", 3) != 0)
				{
					gi.WriteByte (svc_temp_entity);
					gi.WriteByte (te_impact);
					gi.WritePosition (tr.endpos);
					gi.WriteDir (tr.Plane.normal);
					gi.multicast (tr.endpos, MULTICAST_PVS);

					if (self->client)
						PlayerNoise(self, tr.endpos, PNOISE_IMPACT);
				}
			}
		}
	}

	// if went through water, determine where the end and make a bubble trail
	if (water)
	{
		vec3_t	pos;

		VectorSubtract (tr.endpos, water_start, dir);
		VectorNormalize (dir);
		VectorMA (tr.endpos, -2, dir, pos);
		if (gi.pointcontents (pos) & MASK_WATER)
			VectorCopy (pos, tr.endpos);
		else
			tr = gi.trace (pos, NULL, NULL, water_start, tr.ent, MASK_WATER);

		VectorAdd (water_start, tr.endpos, pos);
		VectorScale (pos, 0.5, pos);

		gi.WriteByte (svc_temp_entity);
		gi.WriteByte (TE_BUBBLETRAIL);
		gi.WritePosition (water_start);
		gi.WritePosition (tr.endpos);
		gi.multicast (pos, MULTICAST_PVS);
	}*/
}


void CFixbot::FlyVertical ()
{
	vec3f v, up, start, tempvec;
	
	IdealYaw = (Entity->GoalEntity->State.GetOldOrigin() - Entity->State.GetOrigin()).ToYaw ();	
	ChangeYaw ();
	
	if (Entity->State.GetFrame() == CFixbot::FRAME_landing_58 || Entity->State.GetFrame() == CFixbot::FRAME_takeoff_16)
	{
		Entity->GoalEntity->Free();
		// NOTE: DO FREE

		CurrentMove = &FixbotMoveStand;
		Entity->GoalEntity = Entity->Enemy = nullentity;
	}

	// kick up some particles
	tempvec = Entity->State.GetAngles();
	tempvec.X += 90;

	anglef angles = tempvec.ToVectors ();
	
	for (uint8 i = 0; i < 10; i++)
		BlastOff (Entity->State.GetOrigin(), angles.Forward, 2, 1, TE_SHOTGUN, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD);

	// needs sound
}


void CFixbot::FlyVertical2 ()
{
	vec3f subtract = (Entity->GoalEntity->State.GetOrigin() - Entity->State.GetOrigin());
	IdealYaw = subtract.ToYaw ();	
	ChangeYaw ();
	
	if (subtract.Length() < 32)
	{
		Entity->GoalEntity->Free();
		// NOTE: DO FREE

		CurrentMove = &FixbotMoveStand;
		Entity->GoalEntity = Entity->Enemy = nullentity;
	}
		
	// needs sound
}

void CFixbot::Stand ()
{
	CurrentMove = &FixbotMoveStand;
}

void CFixbot::Run ()
{
	if (AIFlags & AI_STAND_GROUND)
		CurrentMove = &FixbotMoveStand;
	else
		CurrentMove = &FixbotMoveRun;
}

void CFixbot::Walk ()
{
	if (Entity->GoalEntity->ClassName == "func_object_repair")
	{
		if ((Entity->State.GetOrigin() - Entity->GoalEntity->State.GetOrigin()).Length() < 32)
		{
			CurrentMove = &FixbotMoveWeldStart;
			return;
		}
	}
	CurrentMove = &FixbotMoveWalk;
}

void CFixbot::Attack ()
{	
	if (AIFlags & AI_MEDIC)
	{
		if ((!IsVisible (Entity, *Entity->GoalEntity)) || (Entity->State.GetOrigin() - Entity->Enemy->State.GetOrigin()).Length() > 128)
			return;
		if (!((!IsVisible (Entity, *Entity->GoalEntity) || (Entity->State.GetOrigin() - Entity->Enemy->State.GetOrigin()).Length() > 128)))
			CurrentMove = &FixbotMoveLaserAttack;
	}
	else
		CurrentMove = &FixbotMoveAttack2;
}

void CFixbot::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);

	CurrentMove = (Damage <= 10) ? &FixbotMovePain3 : ((Damage <= 25) ? &FixbotMovePain2 : &FixbotMovePain1);
}

void CFixbot::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DIE]);
	Entity->BecomeExplosion (false);
}

/*QUAKED monster_fixbot (1 .5 0) (-32 -32 -24) (32 32 24) Ambush Trigger_Spawn Fixit Takeoff Landing
*/
void CFixbot::Spawn ()
{
	Sounds[SOUND_PAIN1] = SoundIndex ("flyer/flypain1.wav");
	Sounds[SOUND_DIE] = SoundIndex ("flyer/flydeth1.wav");

	Sounds[SOUND_WELD1] = SoundIndex ("misc/welder1.wav");
	Sounds[SOUND_WELD2] = SoundIndex ("misc/welder2.wav");
	Sounds[SOUND_WELD3] = SoundIndex ("misc/welder3.wav");

	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/fixbot/tris.md2");
	
	Entity->GetMins().Set (-32, -32, -24);
	Entity->GetMaxs().Set (32, 32, 24);
	
	Entity->GetSolid() = SOLID_BBOX;

	Entity->Health = 150;
	Entity->Mass = 150;

	MonsterFlags = MF_HAS_ATTACK;
	
	Entity->Link ();

	CurrentMove = &FixbotMoveStand;				
	FlyMonsterStart ();	
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_fixbot", CFixbot);

#endif
