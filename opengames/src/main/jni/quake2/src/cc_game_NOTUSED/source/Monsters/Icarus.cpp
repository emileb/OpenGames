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
// cc_icarus.cpp
// Icarus
//

#include "Local.h"
#include "Monsters/Icarus.h"

CIcarus::CIcarus (uint32 ID) :
IMonster (ID)
{
}

void CIcarus::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

void CIcarus::Search ()
{
	Entity->PlaySound (CHAN_VOICE, (frand() < 0.5) ? Sounds[SOUND_SEARCH1] : Sounds[SOUND_SEARCH2]);
}

CFrame HoverFramesStand [] =
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
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim HoverMoveStand (CIcarus::FRAME_stand01, CIcarus::FRAME_stand30, HoverFramesStand);

/*mframe_t hover_frames_stop1 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	NULL
};
mmove_t hover_move_stop1 = {CIcarus::FRAME_stop101, CIcarus::FRAME_stop109, hover_frames_stop1, NULL};

mframe_t hover_frames_stop2 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	NULL
};
mmove_t hover_move_stop2 = {CIcarus::FRAME_stop201, CIcarus::FRAME_stop208, hover_frames_stop2, NULL};

mframe_t hover_frames_takeoff [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	-2),
	CFrame (&IMonster::AI_Move,	5),
	CFrame (&IMonster::AI_Move,	-1),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	-1),
	CFrame (&IMonster::AI_Move,	-1),
	CFrame (&IMonster::AI_Move,	-1),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	-6),
	CFrame (&IMonster::AI_Move,	-9),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	3),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	0,	NULL
};
mmove_t hover_move_takeoff = {CIcarus::FRAME_takeof01, CIcarus::FRAME_takeof30, hover_frames_takeoff, NULL};*/

CFrame HoverFramesPain3 [] =
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
CAnim HoverMovePain3 (CIcarus::FRAME_pain301, CIcarus::FRAME_pain309, HoverFramesPain3, &IMonster::Run);

CFrame HoverFramesPain2 [] =
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
	CFrame (&IMonster::AI_Move,	0)
};
CAnim HoverMovePain2 (CIcarus::FRAME_pain201, CIcarus::FRAME_pain212, HoverFramesPain2, &IMonster::Run);

CFrame HoverFramesPain1 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	-8),
	CFrame (&IMonster::AI_Move,	-4),
	CFrame (&IMonster::AI_Move,	-6),
	CFrame (&IMonster::AI_Move,	-4),
	CFrame (&IMonster::AI_Move,	-3),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	3),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	3),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	7),
	CFrame (&IMonster::AI_Move,	1),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	5),
	CFrame (&IMonster::AI_Move,	3),
	CFrame (&IMonster::AI_Move,	4)
};
CAnim HoverMovePain1 (CIcarus::FRAME_pain101, CIcarus::FRAME_pain128, HoverFramesPain1, &IMonster::Run);

/*mframe_t hover_frames_land [] =
{
	CFrame (&IMonster::AI_Move,	0,	NULL
};
mmove_t hover_move_land = {CIcarus::FRAME_land01, CIcarus::FRAME_land01, hover_frames_land, NULL};

mframe_t hover_frames_forward [] =
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
	CFrame (&IMonster::AI_Move,	0,	NULL
};
mmove_t hover_move_forward = {CIcarus::FRAME_forwrd01, CIcarus::FRAME_forwrd35, hover_frames_forward, NULL};*/

CFrame HoverFramesWalk [] =
{
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4),
	CFrame (&IMonster::AI_Walk,	4)
};
CAnim HoverMoveWalk (CIcarus::FRAME_forwrd01, CIcarus::FRAME_forwrd35, HoverFramesWalk);

CFrame HoverFramesRun [] =
{
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10),
	CFrame (&IMonster::AI_Run,	10)
};
CAnim HoverMoveRun (CIcarus::FRAME_forwrd01, CIcarus::FRAME_forwrd35, HoverFramesRun);

CFrame HoverFramesDeath1 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	-10),
	CFrame (&IMonster::AI_Move,	3),
	CFrame (&IMonster::AI_Move,	5),
	CFrame (&IMonster::AI_Move,	4),
	CFrame (&IMonster::AI_Move,	7)
};
CAnim HoverMoveDeath1 (CIcarus::FRAME_death101, CIcarus::FRAME_death111, HoverFramesDeath1, ConvertDerivedFunction(&CIcarus::Dead));

/*mframe_t hover_frames_backward [] =
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
	CFrame (&IMonster::AI_Move,	0,	NULL
};
mmove_t hover_move_backward = {CIcarus::FRAME_backwd01, CIcarus::FRAME_backwd24, hover_frames_backward, NULL};*/

CFrame HoverFramesStartAttack [] =
{
	CFrame (&IMonster::AI_Charge,	1),
	CFrame (&IMonster::AI_Charge,	1),
	CFrame (&IMonster::AI_Charge,	1)
};
CAnim HoverMoveStartAttack (CIcarus::FRAME_attak101, CIcarus::FRAME_attak103, HoverFramesStartAttack, ConvertDerivedFunction(&CIcarus::StartAttack));

CFrame HoverFramesAttack1 [] =
{
	CFrame (&IMonster::AI_Charge,	-10,	ConvertDerivedFunction(&CIcarus::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	-10,	ConvertDerivedFunction(&CIcarus::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	0,		ConvertDerivedFunction(&CIcarus::ReAttack))
};
CAnim HoverMoveAttack1 (CIcarus::FRAME_attak104, CIcarus::FRAME_attak106, HoverFramesAttack1);

CFrame HoverFramesEndAttack [] =
{
	CFrame (&IMonster::AI_Charge,	1),
	CFrame (&IMonster::AI_Charge,	1)
};
CAnim HoverMoveEndAttack (CIcarus::FRAME_attak107, CIcarus::FRAME_attak108, HoverFramesEndAttack, &IMonster::Run);

#if ROGUE_FEATURES

/* PMM - circle strafing code */
CFrame HoverFramesStartAttack2 [] =
{
	CFrame (&IMonster::AI_Charge,	15),
	CFrame (&IMonster::AI_Charge,	15),
	CFrame (&IMonster::AI_Charge,	15)
};
CAnim HoverMoveStartAttack2 (CIcarus::FRAME_attak101, CIcarus::FRAME_attak103, HoverFramesStartAttack2, ConvertDerivedFunction(&CIcarus::StartAttack));

CFrame HoverFramesAttack2 [] =
{
	CFrame (&IMonster::AI_Charge,	10,	ConvertDerivedFunction(&CIcarus::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	10,	ConvertDerivedFunction(&CIcarus::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	10,	ConvertDerivedFunction(&CIcarus::ReAttack))
};
CAnim HoverMoveAttack2 (CIcarus::FRAME_attak104, CIcarus::FRAME_attak106, HoverFramesAttack2);

CFrame HoverFramesEndAttack2 [] =
{
	CFrame (&IMonster::AI_Charge,	15),
	CFrame (&IMonster::AI_Charge,	15)
};
CAnim HoverMoveEndAttack2 (CIcarus::FRAME_attak107, CIcarus::FRAME_attak108, HoverFramesEndAttack2, &IMonster::Run);
// end of circle strafe
#endif

void CIcarus::ReAttack ()
{
	if (entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0 && IsVisible (Entity, *Entity->Enemy) && frand() <= 0.6)
	{
#if ROGUE_FEATURES
		CurrentMove = (AttackState == AS_SLIDING) ? &HoverMoveAttack2 : &HoverMoveAttack1;
#else
		CurrentMove = &HoverMoveAttack1;
#endif
		return;
	}

	CurrentMove = &HoverMoveEndAttack;
}


void CIcarus::FireBlaster ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start, end, dir;

	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_HOVER_BLASTER_1], Entity->State.GetAngles().ToVectors(), start);

	end = Entity->Enemy->State.GetOrigin();
	end.Z += Entity->Enemy->ViewHeight;
	dir = end - start;

	MonsterFireBlaster (start, dir, 1, 1000, MZ2_HOVER_BLASTER_1, (Entity->State.GetFrame() == CIcarus::FRAME_attak104) ? FX_HYPERBLASTER : 0);
}


void CIcarus::Stand ()
{
	CurrentMove = &HoverMoveStand;
}

void CIcarus::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &HoverMoveStand : &HoverMoveRun;
}

void CIcarus::Walk ()
{
	CurrentMove = &HoverMoveWalk;
}

void CIcarus::Attack ()
{
#if ROGUE_FEATURES
	float chance;

	// 0% chance of circle in easy
	// 50% chance in normal
	// 75% chance in hard
	// 86.67% chance in nightmare
	if (!CvarList[CV_SKILL].Integer())
		chance = 0;
	else
		chance = 1.0f - (0.5f/CvarList[CV_SKILL].Float());

	// daedalus strafes more
	if (Entity->State.GetSkinNum() >= 2)
		chance += 0.1f;

	if (frand() > chance)
	{
		CurrentMove = &HoverMoveStartAttack;
		AttackState = AS_STRAIGHT;
	}
	else // circle strafe
	{
		if (frand () <= 0.5) // switch directions
			Lefty = !Lefty;
		CurrentMove = &HoverMoveStartAttack2;
		AttackState = AS_SLIDING;
	}
#else
	CurrentMove = &HoverMoveStartAttack;
#endif
}

void CIcarus::StartAttack()
{
	CurrentMove = &HoverMoveAttack1;
}

void CIcarus::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() |= 1;

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;

	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	if (Damage <= 25)
	{
		if (frand() < 0.5)
		{
			Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
			CurrentMove = &HoverMovePain3;
		}
		else
		{
			Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN2]);
			CurrentMove = &HoverMovePain2;
		}
	}
	else
	{
#if !ROGUE_FEATURES
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
		CurrentMove = &HoverMovePain1;
#else
		//PGM pain sequence is WAY too long
		if (frand() < (0.45 - (0.1 * CvarList[CV_SKILL].Float())))
		{
			Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
			CurrentMove = &HoverMovePain1;
		}
		else
		{
			Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN2]);
			CurrentMove = &HoverMovePain2;
		}
#endif
	}
}

void CIcarus::DeadThink ()
{
	if (!Entity->GroundEntity && Level.Frame < TimeStamp)
	{
		Entity->NextThink = Level.Frame + ServerFramesPerSecond;
		return;
	}
	Entity->BecomeExplosion(false);
}

void CIcarus::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);
	Think = ConvertDerivedFunction(&CIcarus::DeadThink);
	Entity->NextThink = Level.Frame + ServerFramesPerSecond;
	TimeStamp = Level.Frame + 150;
	Entity->Link ();
}

void CIcarus::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
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

	Entity->PhysicsType = PHYSICS_TOSS;
// regular death
	Entity->PlaySound (CHAN_VOICE, (frand() < 0.5) ? Sounds[SOUND_DEATH1] : Sounds[SOUND_DEATH2]);
	Entity->IsDead = true;
	Entity->CanTakeDamage = true;
	CurrentMove = &HoverMoveDeath1;
}

/*QUAKED monster_hover (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void CIcarus::Spawn ()
{
	Sounds[SOUND_PAIN1] = SoundIndex ("hover/hovpain1.wav");	
	Sounds[SOUND_PAIN2] = SoundIndex ("hover/hovpain2.wav");	
	Sounds[SOUND_DEATH1] = SoundIndex ("hover/hovdeth1.wav");	
	Sounds[SOUND_DEATH2] = SoundIndex ("hover/hovdeth2.wav");	
	Sounds[SOUND_SIGHT] = SoundIndex ("hover/hovsght1.wav");	
	Sounds[SOUND_SEARCH1] = SoundIndex ("hover/hovsrch1.wav");	
	Sounds[SOUND_SEARCH2] = SoundIndex ("hover/hovsrch2.wav");	

	SoundIndex ("hover/hovatck1.wav");	

	Entity->State.GetSound() = SoundIndex ("hover/hovidle1.wav");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex("models/monsters/hover/tris.md2");
	Entity->GetMins().Set (-24, -24, -24);
	Entity->GetMaxs().Set (24, 24, 32);

	Entity->Health = 240;
	Entity->GibHealth = -100;
	Entity->Mass = 150;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_SIGHT | MF_HAS_SEARCH);

	Entity->Link ();

	CurrentMove = &HoverMoveStand;	

	FlyMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_hover", CIcarus);
