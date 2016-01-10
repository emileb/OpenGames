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
// cc_makron.cpp
// 
//

#include "Local.h"
#include "Monsters/JORG.h" // Req.
#include "Monsters/Makron.h"

CMakron::CMakron (uint32 ID) :
IMonster (ID)
{
}

void CMakron::Taunt ()
{
	MediaIndex Sound = Sounds[SOUND_TAUNT1];
	switch (irandom(3))
	{
	case 1:
		Sound = Sounds[SOUND_TAUNT2];
	default:
		break;
	case 2:
		Sound = Sounds[SOUND_TAUNT3];
		break;
	};

	Entity->PlaySound (CHAN_AUTO, Sound, 255, ATTN_NONE);
}

//
// stand
//

CFrame MakronFramesStand []=
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
	CFrame (&IMonster::AI_Stand, 0),		// 10
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),		// 20
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),		// 30
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),		// 40
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),		// 50
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),		// 60
};
CAnim	MakronMoveStand (CMakron::FRAME_stand201, CMakron::FRAME_stand260, MakronFramesStand);
	
void CMakron::Stand ()
{
	CurrentMove = &MakronMoveStand;
}

void CMakron::Hit ()
{
	Entity->PlaySound (CHAN_AUTO, Sounds[SOUND_HIT], 255, ATTN_NONE);
}

void CMakron::PopUp ()
{
	Entity->PlaySound (CHAN_BODY, Sounds[SOUND_POPUP], 255, ATTN_NONE);
}

void CMakron::StepLeft ()
{
	Entity->PlaySound (CHAN_BODY, Sounds[SOUND_STEPLEFT]);
}

void CMakron::StepRight ()
{
	Entity->PlaySound (CHAN_BODY, Sounds[SOUND_STEPRIGHT]);
}

void CMakron::BrainSplorch ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_BRAIN_SPLORCH]);
}

void CMakron::PreRailgun ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_PRE_RAILGUN]);
}

CFrame MakronFramesRun [] =
{
	CFrame (&IMonster::AI_Run, 3,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8),
	CFrame (&IMonster::AI_Run, 8,	ConvertDerivedFunction(&CMakron::StepRight)),
	CFrame (&IMonster::AI_Run, 6),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 9),
	CFrame (&IMonster::AI_Run, 6),
	CFrame (&IMonster::AI_Run, 12)
};
CAnim	MakronMoveRun (CMakron::FRAME_walk204, CMakron::FRAME_walk213, MakronFramesRun);

CFrame MakronFramesWalk [] =
{
	CFrame (&IMonster::AI_Walk, 3,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&IMonster::AI_Walk, 12),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8),
	CFrame (&IMonster::AI_Walk, 8,	ConvertDerivedFunction(&CMakron::StepRight)),
	CFrame (&IMonster::AI_Walk, 6),
	CFrame (&IMonster::AI_Walk, 12),
	CFrame (&IMonster::AI_Walk, 9),
	CFrame (&IMonster::AI_Walk, 6),
	CFrame (&IMonster::AI_Walk, 12)
};
CAnim	MakronMoveWalk (CMakron::FRAME_walk204, CMakron::FRAME_walk213, MakronFramesRun);

void CMakron::Walk ()
{
	CurrentMove = &MakronMoveWalk;
}

void CMakron::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &MakronMoveStand : &MakronMoveRun;
}

CFrame MakronFramesPain6 [] =
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
	CFrame (&IMonster::AI_Move,	0),		// 10
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::PopUp)),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),		// 20
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::Taunt)),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim MakronMovePain6 (CMakron::FRAME_pain601, CMakron::FRAME_pain627, MakronFramesPain6, &IMonster::Run);

CFrame MakronFramesPain5 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim MakronMovePain5 (CMakron::FRAME_pain501, CMakron::FRAME_pain504, MakronFramesPain5, &IMonster::Run);

CFrame MakronFramesPain4 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim MakronMovePain4 (CMakron::FRAME_pain401, CMakron::FRAME_pain404, MakronFramesPain4, &IMonster::Run);

void CMakron::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
			Entity->State.GetSkinNum() = 1;

	if (Level.Frame < PainDebounceTime)
			return;

	// Lessen the chance of him going into his pain frames
	if ((Damage <= 25) && (frand() < 0.2f))
		return;

	PainDebounceTime = Level.Frame + 30;
	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	if (Damage <= 40)
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN4], 255, ATTN_NONE);
		CurrentMove = &MakronMovePain4;
	}
	else if (Damage <= 110)
	{
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN5], 255, ATTN_NONE);
		CurrentMove = &MakronMovePain5;
	}
	else
	{
		if ((Damage <= 150) && (frand() <= 0.45f))
		{
			Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN6], 255, ATTN_NONE);
			CurrentMove = &MakronMovePain6;
		}
		else if (frand() <= 0.35f)
		{
			Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN6], 255, ATTN_NONE);
			CurrentMove = &MakronMovePain6;
		}
	}
};

CFrame MakronFramesDeath2 [] =
{
	CFrame (&IMonster::AI_Move,	-15),
	CFrame (&IMonster::AI_Move,	3),
	CFrame (&IMonster::AI_Move,	-12),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),			// 10
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	11),
	CFrame (&IMonster::AI_Move,	12),
	CFrame (&IMonster::AI_Move,	11,	ConvertDerivedFunction(&CMakron::StepRight)),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),			// 20
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),			
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),			// 30
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	5),
	CFrame (&IMonster::AI_Move,	7),
	CFrame (&IMonster::AI_Move,	6,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	-1),
	CFrame (&IMonster::AI_Move,	2),			// 40
	CFrame (&IMonster::AI_Move,	0),			
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),			
	CFrame (&IMonster::AI_Move,	0),			// 50
	CFrame (&IMonster::AI_Move,	0),			
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	-6),
	CFrame (&IMonster::AI_Move,	-4),
	CFrame (&IMonster::AI_Move,	-6,	ConvertDerivedFunction(&CMakron::StepRight)),
	CFrame (&IMonster::AI_Move,	-4),
	CFrame (&IMonster::AI_Move,	-4,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),			// 60
	CFrame (&IMonster::AI_Move,	0),			
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	-2),
	CFrame (&IMonster::AI_Move,	-5),
	CFrame (&IMonster::AI_Move,	-3,	ConvertDerivedFunction(&CMakron::StepRight)),
	CFrame (&IMonster::AI_Move,	-8),
	CFrame (&IMonster::AI_Move,	-3,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&IMonster::AI_Move,	-7),
	CFrame (&IMonster::AI_Move,	-4),
	CFrame (&IMonster::AI_Move,	-4,	ConvertDerivedFunction(&CMakron::StepRight)),			// 70
	CFrame (&IMonster::AI_Move,	-6),			
	CFrame (&IMonster::AI_Move,	-7),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),			// 80
	CFrame (&IMonster::AI_Move,	0),			
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	-2),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	2),
	CFrame (&IMonster::AI_Move,	0),			// 90
	CFrame (&IMonster::AI_Move,	27,	ConvertDerivedFunction(&CMakron::Hit)),			
	CFrame (&IMonster::AI_Move,	26),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::BrainSplorch)),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)			// 95
};
CAnim MakronMoveDeath2 (CMakron::FRAME_death201, CMakron::FRAME_death295, MakronFramesDeath2, ConvertDerivedFunction(&CMakron::Dead));

CFrame MakronFramesDeath3 [] =
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
	CFrame (&IMonster::AI_Move,	0)
};
CAnim MakronMoveDeath3 (CMakron::FRAME_death301, CMakron::FRAME_death320, MakronFramesDeath3);

CFrame MakronFramesSight [] =
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
	CFrame (&IMonster::AI_Move,	0)
};
CAnim MakronMoveSight (CMakron::FRAME_active01, CMakron::FRAME_active13, MakronFramesSight, &IMonster::Run);

void CMakron::FireBFG ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start;
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_MAKRON_BFG], Entity->State.GetAngles().ToVectors(), start);

	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_ATTACK_BFG]);
	MonsterFireBfg (start,
		((Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight)) - start).GetNormalized(),
		50, 300, 100, 300, MZ2_MAKRON_BFG);
}	

CFrame MakronFramesAttack3 []=
{
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CMakron::FireBFG)),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim MakronMoveAttack3 (CMakron::FRAME_attak301, CMakron::FRAME_attak308, MakronFramesAttack3, &IMonster::Run);

CFrame MakronFramesAttack4[]=
{
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim MakronMoveAttack4 (CMakron::FRAME_attak401, CMakron::FRAME_attak426, MakronFramesAttack4, &IMonster::Run);

void CMakron::FireHyperblaster ()
{
	if (!HasValidEnemy())
		return;

	vec3f	dir;
	vec3f	start;
	sint32		flash_number = MZ2_MAKRON_BLASTER_1 + (Entity->State.GetFrame() - CMakron::FRAME_attak405);

	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flash_number], Entity->State.GetAngles().ToVectors(), start);

	if (Entity->Enemy)
		dir.X = (Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight) - start).ToAngles().X;
	else
		dir.X = 0;

	if (Entity->State.GetFrame() <= CMakron::FRAME_attak413)
		dir.Y = Entity->State.GetAngles().Y - 10 * (Entity->State.GetFrame() - CMakron::FRAME_attak413);
	else
		dir.Y = Entity->State.GetAngles().Y + 10 * (Entity->State.GetFrame() - CMakron::FRAME_attak421);
	dir.Z = 0;

	MonsterFireBlaster (start, dir.ToVectors().Forward, 15, 1000, MZ2_MAKRON_BLASTER_1, FX_BLASTER);
}	

CFrame MakronFramesAttack5[]=
{
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CMakron::PreRailgun)),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CMakron::SavePosition)),
	CFrame (&IMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireRailgun)),		// Fire railgun
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim MakronMoveAttack5 (CMakron::FRAME_attak501, CMakron::FRAME_attak516, MakronFramesAttack5, &IMonster::Run);

void CMakron::SavePosition ()
{
	SavedLoc = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight);
};

void CMakron::FireRailgun ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start;
	vec3f	dir;

	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_MAKRON_RAILGUN_1], Entity->State.GetAngles().ToVectors(), start);
	
	// calc direction to where we targted
	dir = SavedLoc - start;
	dir.Normalize();

	MonsterFireRailgun (start, dir, 50, 100, MZ2_MAKRON_RAILGUN_1);
}

void CMakron::Sight()
{
	CurrentMove = &MakronMoveSight;
};

void CMakron::Attack()
{
	switch (irandom(3))
	{
	case 0:
		CurrentMove = &MakronMoveAttack3;
		break;
	case 1:
		CurrentMove = &MakronMoveAttack4;
		break;
	case 2:
	default:
		CurrentMove = &MakronMoveAttack5;
		break;
	};
}

/*
---
Makron Torso. This needs to be spawned in
---
*/

class CMakronTorso : public virtual IBaseEntity, public IThinkableEntity
{
public:
	CMakronTorso () :
	  IBaseEntity(),
	  IThinkableEntity()
	  {
	  };

	CMakronTorso (sint32 Index) :
	  IBaseEntity(Index),
	  IThinkableEntity (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER(CMakronTorso)

	void SaveFields (CFile &File)
	{
		IThinkableEntity::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		IThinkableEntity::LoadFields (File);
	};

	void Think ()
	{
		if (++State.GetFrame() < 365)
			NextThink = Level.Frame + ServerFramesPerSecond;
		else
		{		
			State.GetFrame() = 346;
			NextThink = Level.Frame + ServerFramesPerSecond;
		}
	};

	static void Spawn (CMonsterEntity *Owner)
	{
		CMakronTorso *NewClass = QNewEntityOf CMakronTorso;

		NewClass->State.GetOrigin() = Owner->State.GetOrigin();
		NewClass->State.GetAngles() = Owner->State.GetAngles();
		vec3f origin = Owner->State.GetOrigin();
		origin.Y -= 84;

		NewClass->GetMins().Set (-8, -8, 0);
		NewClass->GetMaxs().Set (8);
		NewClass->State.GetModelIndex() = Owner->State.GetModelIndex();
		NewClass->NextThink = Level.Frame + 2;
		NewClass->State.GetSound() = SoundIndex("makron/spine.wav");
		NewClass->State.GetFrame() = 346;
		NewClass->GetSolid() = SOLID_NOT;
		NewClass->Link ();
	};
};

//
// death
//

void CMakron::Dead ()
{
	Entity->GetMins().Set (-60, -60, 0);
	Entity->GetMaxs().Set (60, 60, 72);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

void CMakron::Die(IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	Entity->State.GetSound() = 0;
	// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (sint32 n= 0; n < 1 /*4*/; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
		for (sint32 n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMetal(), Damage, GIB_METALLIC);
		Entity->ThrowHead (GameMedia.Gib_Gear(), Damage, GIB_METALLIC);
		Entity->IsDead = true;
		return;
	}

	if (Entity->IsDead == true)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DEATH], 255, ATTN_NONE);
	Entity->IsDead = true;
	Entity->CanTakeDamage = true;

	// Spawn torso
	CMakronTorso::Spawn (Entity);

	CurrentMove = &MakronMoveDeath2;
}

bool CMakron::CheckAttack ()
{
#if !ROGUE_FEATURES
	float	chance;

	if (entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0)
	{
	// see if any entities are in the way of the shot
		vec3f spot1 = Entity->State.GetOrigin();
		spot1.Z += Entity->ViewHeight;
		vec3f spot2 = Entity->Enemy->State.GetOrigin();
		spot2.Z += Entity->Enemy->ViewHeight;

		CTrace tr (spot1, spot2, Entity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.Entity != Entity->Enemy)
			return false;
	}
	
	// melee attack
	if (EnemyRange == RANGE_MELEE)
	{
		// don't always melee in easy mode
		if (CvarList[CV_SKILL].Integer() == 0 && (randomMT()&3) )
			return false;
		if (MonsterFlags & MF_HAS_MELEE)
			AttackState = AS_MELEE;
		else
			AttackState = AS_MISSILE;
		return true;
	}
	
// missile attack
	if (!(MonsterFlags & MF_HAS_ATTACK))
		return false;
		
	if (Level.Frame < AttackFinished)
		return false;
		
	if (EnemyRange == RANGE_FAR)
		return false;

	if (AIFlags & AI_STAND_GROUND)
	{
		chance = 0.4f;
	}
	else if (EnemyRange == RANGE_MELEE)
	{
		chance = 0.8f;
	}
	else if (EnemyRange == RANGE_NEAR)
	{
		chance = 0.4f;
	}
	else if (EnemyRange == RANGE_MID)
	{
		chance = 0.2f;
	}
	else
	{
		return false;
	}

	if (CvarList[CV_SKILL].Integer() == 0)
		chance *= 0.5;
	else if (CvarList[CV_SKILL].Integer() >= 2)
		chance *= 2;

	if (frand () < chance)
	{
		AttackState = AS_MISSILE;
		AttackFinished = Level.Frame + ((2*frand())*10);
		return true;
	}

	if (AIFlags & AI_FLY)
	{
		if (frand() < 0.3)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

	return false;
#else
	float	chance;

	if (entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0)
	{
	// see if any entities are in the way of the shot
		vec3f	spot1 = Entity->State.GetOrigin() + vec3f(0, 0, Entity->ViewHeight);
		vec3f	spot2 = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight);

		CTrace tr (spot1, spot2, Entity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.Entity != Entity->Enemy)
		{	
			// PGM - we want them to go ahead and shoot at info_notnulls if they can.
			if(Entity->Enemy->GetSolid() != SOLID_NOT || tr.Fraction < 1.0)		//PGM
			{
				// PMM - if we can't see our target, and we're not blocked by a monster, go into blind fire if available
				if ((!(tr.Entity->GetSvFlags() & SVF_MONSTER)) && (!IsVisible(Entity, *Entity->Enemy)))
				{
					if ((BlindFire) && (BlindFireDelay <= 20.0))
					{
						if (Level.Frame < AttackFinished)
							return false;
						if (Level.Frame < (TrailTime + BlindFireDelay))
							// wait for our time
							return false;
						else
						{
							// make sure we're not going to shoot a monster
							tr (spot1, BlindFireTarget, Entity, CONTENTS_MONSTER);
							if (tr.AllSolid || tr.StartSolid || ((tr.Fraction < 1.0) && (tr.Entity != Entity->Enemy)))
								return false;

							AttackState = AS_BLIND;
							return true;
						}
					}
				}
				// pmm
				return false;
			}
		}
	}
	
	// melee attack
	if (EnemyRange == RANGE_MELEE)
	{
		// don't always melee in easy mode
		if (CvarList[CV_SKILL].Integer() == 0 && (randomMT()&3) )
		{
			// PMM - fix for melee only monsters & strafing
			AttackState = AS_STRAIGHT;
			return false;
		}
		if (MonsterFlags & MF_HAS_MELEE)
			AttackState = AS_MELEE;
		else
			AttackState = AS_MISSILE;
		return true;
	}
	
// missile attack
	if (!(MonsterFlags & MF_HAS_ATTACK))
	{
		// PMM - fix for melee only monsters & strafing
		AttackState = AS_STRAIGHT;
		return false;
	}
	
	if (Level.Frame < AttackFinished)
		return false;
		
	if (EnemyRange == RANGE_FAR)
		return false;

	if (AIFlags & AI_STAND_GROUND)
		chance = 0.4f;
	else if (EnemyRange == RANGE_MELEE)
		chance = 0.8f;
	else if (EnemyRange == RANGE_NEAR)
		chance = 0.4f;
	else if (EnemyRange == RANGE_MID)
		chance = 0.2f;
	else
		return false;

	if (CvarList[CV_SKILL].Integer() == 0)
		chance *= 0.5;
	else if (CvarList[CV_SKILL].Integer() >= 2)
		chance *= 2;

	// PGM - go ahead and shoot every time if it's a info_notnull
	if ((frand () < chance) || (Entity->Enemy->GetSolid() == SOLID_NOT))
	{
		AttackState = AS_MISSILE;
		AttackFinished = Level.Frame + ((2*frand())*10);
		return true;
	}

	// PMM -daedalus should strafe more .. this can be done here or in a customized
	// check_attack code for the hover.
	if (AIFlags & AI_FLY)
	{
		// originally, just 0.3
		float strafe_chance = 0.8f;

		// if enemy is tesla, never strafe
		if (Entity->Enemy && (Entity->Enemy->ClassName != "tesla"))
			strafe_chance = 0;

		if (frand() < strafe_chance)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}
// do we want the monsters strafing?
	else
	{
		if (frand() < 0.4)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

//-PMM

	return false;
#endif
}

//
// monster_makron
//

void CMakron::Precache ()
{
	SoundIndex ("makron/pain3.wav");
	SoundIndex ("makron/pain2.wav");
	SoundIndex ("makron/pain1.wav");
	SoundIndex ("makron/death.wav");
	SoundIndex ("makron/step1.wav");
	SoundIndex ("makron/step2.wav");
	SoundIndex ("makron/bfg_fire.wav");
	SoundIndex ("makron/brain1.wav");
	SoundIndex ("makron/rail_up.wav");
	SoundIndex ("makron/popup.wav");
	SoundIndex ("makron/voice4.wav");
	SoundIndex ("makron/voice3.wav");
	SoundIndex ("makron/voice.wav");
	SoundIndex ("makron/bhit.wav");

	ModelIndex ("models/monsters/boss3/rider/tris.md2");
}

/*QUAKED monster_makron (1 .5 0) (-30 -30 0) (30 30 90) Ambush Trigger_Spawn Sight
*/
void CMakron::Spawn ()
{
	Sounds[SOUND_PAIN4] = SoundIndex ("makron/pain3.wav");
	Sounds[SOUND_PAIN5] = SoundIndex ("makron/pain2.wav");
	Sounds[SOUND_PAIN6] = SoundIndex ("makron/pain1.wav");
	Sounds[SOUND_DEATH] = SoundIndex ("makron/death.wav");
	Sounds[SOUND_STEPLEFT] = SoundIndex ("makron/step1.wav");
	Sounds[SOUND_STEPRIGHT] = SoundIndex ("makron/step2.wav");
	Sounds[SOUND_ATTACK_BFG] = SoundIndex ("makron/bfg_fire.wav");
	Sounds[SOUND_BRAIN_SPLORCH] = SoundIndex ("makron/brain1.wav");
	Sounds[SOUND_PRE_RAILGUN] = SoundIndex ("makron/rail_up.wav");
	Sounds[SOUND_POPUP] = SoundIndex ("makron/popup.wav");
	Sounds[SOUND_TAUNT1] = SoundIndex ("makron/voice4.wav");
	Sounds[SOUND_TAUNT2] = SoundIndex ("makron/voice3.wav");
	Sounds[SOUND_TAUNT3] = SoundIndex ("makron/voice.wav");
	Sounds[SOUND_HIT] = SoundIndex ("makron/bhit.wav");
	
	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/boss3/rider/tris.md2");
	Entity->GetMins().Set (-30, -30, 0);
	Entity->GetMaxs().Set (30, 30, 90);

	Entity->Health = 3000;
	Entity->GibHealth = -2000;
	Entity->Mass = 500;

	MonsterFlags = (MF_HAS_ATTACK | MF_HAS_SIGHT);
	Entity->Link ();
	
	CurrentMove = &MakronMoveSight;
	WalkMonsterStart();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_makron", CMakron);

/*
=================
CMakronJumpTimer class

Jorg is just about dead, so set up to launch Makron out
=================
*/
CMakronJumpTimer::CMakronJumpTimer () :
IBaseEntity(),
IThinkableEntity ()
{
};
CMakronJumpTimer::CMakronJumpTimer (sint32 Index) :
IBaseEntity(Index),
IThinkableEntity (Index)
{
};

void CMakronJumpTimer::Think ()
{
	CMonsterEntity *newClass = QNewEntityOf CMonsterEntity;
	CMakron *Monster = QNewEntityOf CMakron (CMakron_ID);
	newClass->Monster = Monster;
	Monster->Entity = newClass;
	newClass->State.GetOrigin() = State.GetOrigin();
	Monster->Spawn ();
	newClass->NextThink = Level.Frame + 1;
	newClass->Target = LinkedJorg->Target;

	// jump at player
	CPlayerEntity *Player = Level.SightClient;
	if (!Player)
		return;

	vec3f vec = (Player->State.GetOrigin() - newClass->State.GetOrigin());
	newClass->State.GetAngles().Set (0, vec.ToYaw(), 0);
	vec.Normalize();
	vec3f vel = vec3fOrigin.MultiplyAngles (400, vec);
	newClass->Velocity = vel;
	newClass->Velocity.Z = 200;
	newClass->GroundEntity = nullentity;
	newClass->Link ();

	Free ();
};

void CMakronJumpTimer::Spawn (CJorg *Jorg)
{
	CMakronJumpTimer *Timer = QNewEntityOf CMakronJumpTimer;
	
	Timer->NextThink = Level.Frame + 8;
	Timer->LinkedJorg = Jorg->Entity;
	Timer->State.GetOrigin() = Jorg->Entity->State.GetOrigin();
	Timer->Link();
}

