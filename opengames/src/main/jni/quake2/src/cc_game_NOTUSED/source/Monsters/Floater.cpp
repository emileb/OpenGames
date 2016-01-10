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
// cc_floater.cpp
// Floater Monster
//

#include "Local.h"
#include "Monsters/Floater.h"

CFloater::CFloater (uint32 ID) :
IMonster (ID)
{
}

void CFloater::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

void CFloater::Idle ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_IDLE], 255, ATTN_IDLE);
}

void CFloater::FireBlaster ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start, end, dir;
	sint32		effect = 0;

	switch (Entity->State.GetFrame())
	{
	case CFloater::FRAME_attak104:
	case CFloater::FRAME_attak107:
		effect = FX_HYPERBLASTER;
		break;
	}

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_FLOAT_BLASTER_1], angles, start);

	end = Entity->Enemy->State.GetOrigin();
	end.Z += Entity->Enemy->ViewHeight;
	dir = end - start;

	MonsterFireBlaster (start, dir, 1, 1000, MZ2_FLOAT_BLASTER_1, effect);
}

CFrame FloaterFramesStand1 [] =
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
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim FloaterMoveStand1 (CFloater::FRAME_stand101, CFloater::FRAME_stand152, FloaterFramesStand1);

CFrame FloaterFramesStand2 [] =
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
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim FloaterMoveStand2 (CFloater::FRAME_stand201, CFloater::FRAME_stand252, FloaterFramesStand2);

void CFloater::Stand ()
{
	CurrentMove = (frand() <= 0.5) ? &FloaterMoveStand1 : &FloaterMoveStand2;
}

CFrame FloaterFramesAttack1 [] =
{
	CFrame (&IMonster::AI_Charge,	0),			// Blaster attack
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),			// BOOM (0, -25.8, 32.5)	-- LOOP Starts
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0)			//							-- LOOP Ends
};
CAnim FloaterMoveAttack1 (CFloater::FRAME_attak101, CFloater::FRAME_attak114, FloaterFramesAttack1, ConvertDerivedFunction(&CFloater::Run));

#if ROGUE_FEATURES
CFrame FloaterFramesAttack1a [] =
{
	CFrame (&IMonster::AI_Charge,	10),			// Blaster attack
	CFrame (&IMonster::AI_Charge,	10),
	CFrame (&IMonster::AI_Charge,	10),
	CFrame (&IMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),			// BOOM (0, -25.8, 32.5)	-- LOOP Starts
	CFrame (&IMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&IMonster::AI_Charge,	10),
	CFrame (&IMonster::AI_Charge,	10),
	CFrame (&IMonster::AI_Charge,	10),
	CFrame (&IMonster::AI_Charge,	10)			//							-- LOOP Ends
};
CAnim FloaterMoveAttack1a (CFloater::FRAME_attak101, CFloater::FRAME_attak114, FloaterFramesAttack1a, ConvertDerivedFunction(&CFloater::Run));
#endif

CFrame FloaterFramesAttack2 [] =
{
	CFrame (&IMonster::AI_Charge,	0),			// Claws
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),			// WHAM (0, -45, 29.6)		-- LOOP Starts
	CFrame (&IMonster::AI_Charge,	0, ConvertDerivedFunction(&CFloater::Wham)),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),			//							-- LOOP Ends
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0)
};
CAnim FloaterMoveAttack2 (CFloater::FRAME_attak201, CFloater::FRAME_attak225, FloaterFramesAttack2, ConvertDerivedFunction(&CFloater::Run));

CFrame FloaterFramesAttack3 [] =
{
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0, ConvertDerivedFunction(&CFloater::Zap)),		//								-- LOOP Starts
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),		//								-- LOOP Ends
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0),
	CFrame (&IMonster::AI_Charge,	0)
};
CAnim FloaterMoveAttack3 (CFloater::FRAME_attak301, CFloater::FRAME_attak334, FloaterFramesAttack3, ConvertDerivedFunction(&CFloater::Run));

CFrame FloaterFramesPain1 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim FloaterMovePain1 (CFloater::FRAME_pain101, CFloater::FRAME_pain107, FloaterFramesPain1, ConvertDerivedFunction(&CFloater::Run));

CFrame FloaterFramesPain2 [] =
{
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0),
	CFrame (&IMonster::AI_Move,	0)
};
CAnim FloaterMovePain2 (CFloater::FRAME_pain201, CFloater::FRAME_pain208, FloaterFramesPain2, ConvertDerivedFunction(&CFloater::Run));

CFrame FloaterFramesPain3 [] =
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
CAnim FloaterMovePain3 (CFloater::FRAME_pain301, CFloater::FRAME_pain312, FloaterFramesPain3, ConvertDerivedFunction(&CFloater::Run));

CFrame FloaterFramesWalk [] =
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
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5)
};
CAnim FloaterMoveWalk (CFloater::FRAME_stand101, CFloater::FRAME_stand152, FloaterFramesWalk);

CFrame FloaterFramesRun [] =
{
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13),
	CFrame (&IMonster::AI_Run, 13)
};
CAnim FloaterMoveRun (CFloater::FRAME_stand101, CFloater::FRAME_stand152, FloaterFramesRun);

void CFloater::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &FloaterMoveStand1 : &FloaterMoveRun;
}

void CFloater::Walk ()
{
	CurrentMove = &FloaterMoveWalk;
}

void CFloater::Wham ()
{
	static	vec3f	aim (MELEE_DISTANCE, 0, 0);
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_ATTACK3]);
	CMeleeWeapon::Fire (Entity, aim, 5 + irandom(6), -50);
}

#include "Utility/TemporaryEntities.h"

void CFloater::Zap ()
{
	vec3f	origin, dir;
	static const vec3f offset (18.5f, -0.9f, 10);
	dir = Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin();

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), offset, angles, origin);

	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_ATTACK2]);
	CSplash(origin, vec3fOrigin, SPT_SPARKS, 32).Send();

	if (Entity->Enemy && (Entity->Enemy->EntityFlags & EF_HURTABLE))
		entity_cast<IHurtableEntity>(*Entity->Enemy)->TakeDamage (Entity, Entity, vec3fOrigin,
		Entity->Enemy->State.GetOrigin(), vec3fOrigin, 5 + irandom(6), -10, DAMAGE_ENERGY, MOD_UNKNOWN);
}

void CFloater::Attack()
{
#if !ROGUE_FEATURES
	CurrentMove = &FloaterMoveAttack1;
#else
	float chance = (!CvarList[CV_SKILL].Integer()) ? 0 : 1.0 - (0.5/CvarList[CV_SKILL].Float());

	// 0% chance of circle in easy
	// 50% chance in normal
	// 75% chance in hard
	// 86.67% chance in nightmare

	if (frand() > chance)
	{
		AttackState = AS_STRAIGHT;
		CurrentMove = &FloaterMoveAttack1;
	}
	else // circle strafe
	{
		if (frand () <= 0.5) // switch directions
			Lefty = !Lefty;
		AttackState = AS_SLIDING;
		CurrentMove = &FloaterMoveAttack1a;
	}
#endif
}

void CFloater::Melee ()
{
	CurrentMove = (frand() < 0.5) ? &FloaterMoveAttack3 : &FloaterMoveAttack2;
}

void CFloater::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;
	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	bool n = (frand() < 0.5);
	Entity->PlaySound (CHAN_VOICE, n ? Sounds[SOUND_PAIN1] : Sounds[SOUND_PAIN2]);
	CurrentMove = n ? &FloaterMovePain1 : &FloaterMovePain2;
}

void CFloater::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DEATH1]);
	Entity->BecomeExplosion (false);
}

void CFloater::Spawn ()
{
	Sounds[SOUND_ATTACK2] = SoundIndex ("floater/fltatck2.wav");
	Sounds[SOUND_ATTACK3] = SoundIndex ("floater/fltatck3.wav");
	Sounds[SOUND_DEATH1] = SoundIndex ("floater/fltdeth1.wav");
	Sounds[SOUND_IDLE] = SoundIndex ("floater/fltidle1.wav");
	Sounds[SOUND_PAIN1] = SoundIndex ("floater/fltpain1.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("floater/fltpain2.wav");
	Sounds[SOUND_SIGHT] = SoundIndex ("floater/fltsght1.wav");

	SoundIndex ("floater/fltatck1.wav");

	Entity->State.GetSound() = SoundIndex ("floater/fltsrch1.wav");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/float/tris.md2");
	Entity->GetMins().Set (-24, -24, -24);
	Entity->GetMaxs().Set (24, 24, 32);

	Entity->Health = 200;
	Entity->GibHealth = -80;
	Entity->Mass = 300;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_MELEE | MF_HAS_SIGHT | MF_HAS_IDLE);

	Entity->Link ();

	CurrentMove = (frand() <= 0.5) ? &FloaterMoveStand1 : &FloaterMoveStand2;	

	FlyMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_floater", CFloater);
