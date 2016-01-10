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
// cc_barracuda.cpp
// Sharky!
//

#include "Local.h"
#include "Monsters/Barracuda.h"

CBarracudaShark::CBarracudaShark (uint32 ID) :
IMonster (ID)
{
}

CFrame FlipperFramesStand [] =
{
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim FlipperMoveStand (CBarracudaShark::FRAME_flphor01, CBarracudaShark::FRAME_flphor01, FlipperFramesStand);

void CBarracudaShark::Stand ()
{
	CurrentMove = &FlipperMoveStand;
}

const int FLIPPER_RUN_SPEED	= 24;

CFrame FlipperFramesRun [] =
{
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),	// 6
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),	// 10

	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),	// 20

	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&IMonster::AI_Run, FLIPPER_RUN_SPEED)		// 29
};
CAnim FlipperMoveRunLoop (CBarracudaShark::FRAME_flpver06, CBarracudaShark::FRAME_flpver29, FlipperFramesRun);

void CBarracudaShark::RunLoop ()
{
	CurrentMove = &FlipperMoveRunLoop;
}

CFrame FlipperFramesRunStart [] =
{
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8)
};
CAnim FlipperMoveRunStart (CBarracudaShark::FRAME_flpver01, CBarracudaShark::FRAME_flpver06, FlipperFramesRunStart, ConvertDerivedFunction(&CBarracudaShark::RunLoop));

void CBarracudaShark::DoRun ()
{
	CurrentMove = &FlipperMoveRunStart;
}

/* Standard Swimming */ 
CFrame FlipperFramesWalk [] =
{
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
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4)
};
CAnim FlipperMoveWalk (CBarracudaShark::FRAME_flphor01, CBarracudaShark::FRAME_flphor24, FlipperFramesWalk);

void CBarracudaShark::Walk ()
{
	CurrentMove = &FlipperMoveWalk;
}

CFrame FlipperFramesStartRun [] =
{
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8)
};
CAnim FlipperMoveStartRun (CBarracudaShark::FRAME_flphor01, CBarracudaShark::FRAME_flphor05, FlipperFramesStartRun, ConvertDerivedFunction(&CBarracudaShark::DoRun));

void CBarracudaShark::Run ()
{
	CurrentMove = &FlipperMoveStartRun;
}

CFrame FlipperFramesPain2 [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim FlipperMovePain2 (CBarracudaShark::FRAME_flppn101, CBarracudaShark::FRAME_flppn105, FlipperFramesPain2, &IMonster::Run);

CFrame FlipperFramesPain1 [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim FlipperMovePain1 (CBarracudaShark::FRAME_flppn201, CBarracudaShark::FRAME_flppn205, FlipperFramesPain1, &IMonster::Run);

void CBarracudaShark::Bite ()
{
	static vec3f	aim (MELEE_DISTANCE, 0, 0);
	CMeleeWeapon::Fire (Entity, aim, 5, 0);
}

void CBarracudaShark::PreAttack ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_CHOMP]);
}

CFrame FlipperFramesAttack [] =
{
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CBarracudaShark::PreAttack)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CBarracudaShark::Bite)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CBarracudaShark::Bite)),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim FlipperMoveAttack (CBarracudaShark::FRAME_flpbit01, CBarracudaShark::FRAME_flpbit20, FlipperFramesAttack, &IMonster::Run);

void CBarracudaShark::Melee()
{
	CurrentMove = &FlipperMoveAttack;
}

void CBarracudaShark::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;
	
	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	switch (irandom(2))
	{
	case 0:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
		CurrentMove = &FlipperMovePain1;
		break;
	case 1:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN2]);
		CurrentMove = &FlipperMovePain2;
		break;
	}
}

void CBarracudaShark::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame FlipperFramesDeath [] =
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
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),

	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim FlipperMoveDeath (CBarracudaShark::FRAME_flpdth01, CBarracudaShark::FRAME_flpdth56, FlipperFramesDeath, ConvertDerivedFunction(&CBarracudaShark::Dead));

void CBarracudaShark::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

void CBarracudaShark::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (sint32 n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_Bone[0], Damage, GIB_ORGANIC);
		for (sint32 n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
		Entity->IsDead = true;
		return;
	}

	if (Entity->IsDead == true)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DEATH]);
	Entity->IsDead = true;
	Entity->CanTakeDamage = true;
	CurrentMove = &FlipperMoveDeath;
}

/*QUAKED monster_flipper (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void CBarracudaShark::Spawn ()
{
	Sounds[SOUND_PAIN1]		= SoundIndex ("flipper/flppain1.wav");	
	Sounds[SOUND_PAIN2]		= SoundIndex ("flipper/flppain2.wav");	
	Sounds[SOUND_DEATH]		= SoundIndex ("flipper/flpdeth1.wav");	
	Sounds[SOUND_CHOMP]		= SoundIndex ("flipper/flpatck1.wav");
	Sounds[SOUND_ATTACK]		= SoundIndex ("flipper/flpatck2.wav");
	Sounds[SOUND_IDLE]		= SoundIndex ("flipper/flpidle1.wav");
	Sounds[SOUND_SEARCH]		= SoundIndex ("flipper/flpsrch1.wav");
	Sounds[SOUND_SIGHT]		= SoundIndex ("flipper/flpsght1.wav");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/flipper/tris.md2");
	Entity->GetMins().Set (-16, -16, 0);
	Entity->GetMaxs().Set (16, 16, 32);

	Entity->Health = 50;
	Entity->GibHealth = -30;
	Entity->Mass = 100;

	MonsterFlags = (MF_HAS_MELEE | MF_HAS_SIGHT);

	Entity->Link ();
	CurrentMove = &FlipperMoveStand;	

	SwimMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_flipper", CBarracudaShark);
