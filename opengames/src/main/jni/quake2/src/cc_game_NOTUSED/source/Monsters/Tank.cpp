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
// cc_tank.cpp
// Tank
//

#include "Local.h"
#include "Monsters/Tank.h"

CTank::CTank (uint32 ID) :
IMonster(ID)
{
}

CTankCommander::CTankCommander (uint32 ID) :
CTank (ID)
{
}

//
// misc
//

void CTank::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

void CTank::Footstep ()
{
	Entity->PlaySound (CHAN_BODY, Sounds[SOUND_STEP]);
}

void CTank::Thud ()
{
	Entity->PlaySound (CHAN_BODY, Sounds[SOUND_THUD]);
}

void CTank::Windup ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_WINDUP]);
}

void CTank::Idle ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_IDLE], 255, ATTN_IDLE);
}

//
// stand
//

CFrame TankFramesStand []=
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
CAnim	TankMoveStand (CTank::FRAME_stand01, CTank::FRAME_stand30, TankFramesStand);
	
void CTank::Stand ()
{
	CurrentMove = &TankMoveStand;
}

//
// walk
//

CFrame TankFramesWalk [] =
{
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 3),
	CFrame (&IMonster::AI_Walk, 2),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 4, ConvertDerivedFunction(&CTank::Footstep)),
	CFrame (&IMonster::AI_Walk, 3),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 4),
	CFrame (&IMonster::AI_Walk, 5),
	CFrame (&IMonster::AI_Walk, 7),
	CFrame (&IMonster::AI_Walk, 7),
	CFrame (&IMonster::AI_Walk, 6),
	CFrame (&IMonster::AI_Walk, 6, ConvertDerivedFunction(&CTank::Footstep))
};
CAnim TankMoveWalk (CTank::FRAME_walk05, CTank::FRAME_walk20, TankFramesWalk);

void CTank::Walk ()
{
	CurrentMove = &TankMoveWalk;
}

//
// run
//

CFrame TankFramesStartRun [] =
{
	CFrame (&IMonster::AI_Run,  0),
	CFrame (&IMonster::AI_Run,  6),
	CFrame (&IMonster::AI_Run,  6),
	CFrame (&IMonster::AI_Run, 11, ConvertDerivedFunction(&CTank::Footstep))
};
CAnim TankMoveStartRun (CTank::FRAME_walk01, CTank::FRAME_walk04, TankFramesStartRun, ConvertDerivedFunction(&CTank::Run));

CFrame TankFramesRun [] =
{
	CFrame (&IMonster::AI_Run, 4),
	CFrame (&IMonster::AI_Run, 5),
	CFrame (&IMonster::AI_Run, 3),
	CFrame (&IMonster::AI_Run, 2),
	CFrame (&IMonster::AI_Run, 5),
	CFrame (&IMonster::AI_Run, 5),
	CFrame (&IMonster::AI_Run, 4),
	CFrame (&IMonster::AI_Run, 4, ConvertDerivedFunction(&CTank::Footstep)),
	CFrame (&IMonster::AI_Run, 3),
	CFrame (&IMonster::AI_Run, 5),
	CFrame (&IMonster::AI_Run, 4),
	CFrame (&IMonster::AI_Run, 5),
	CFrame (&IMonster::AI_Run, 7),
	CFrame (&IMonster::AI_Run, 7),
	CFrame (&IMonster::AI_Run, 6),
	CFrame (&IMonster::AI_Run, 6, ConvertDerivedFunction(&CTank::Footstep))
};
CAnim TankMoveRun (CTank::FRAME_walk05, CTank::FRAME_walk20, TankFramesRun);

CFrame TankFramesStopRun [] =
{
	CFrame (&IMonster::AI_Run,  3),
	CFrame (&IMonster::AI_Run,  3),
	CFrame (&IMonster::AI_Run,  2),
	CFrame (&IMonster::AI_Run,  2),
	CFrame (&IMonster::AI_Run,  4, ConvertDerivedFunction(&CTank::Footstep))
};
CAnim TankMoveStopRun (CTank::FRAME_walk21, CTank::FRAME_walk25, TankFramesStopRun, ConvertDerivedFunction(&CTank::Walk));

void CTank::Run ()
{
	if (Entity->Enemy && (Entity->Enemy->EntityFlags & EF_PLAYER))
		AIFlags |= AI_BRUTAL;
	else
		AIFlags &= ~AI_BRUTAL;

	if (AIFlags & AI_STAND_GROUND)
	{
		CurrentMove = &TankMoveStand;
		return;
	}

	if (CurrentMove == &TankMoveWalk ||
		CurrentMove == &TankMoveStartRun)
	{
		CurrentMove = &TankMoveRun;
	}
	else
	{
		CurrentMove = &TankMoveStartRun;
	}
}

//
// pain
//

CFrame TankFramesPain1 [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim TankMovePain1 (CTank::FRAME_pain101, CTank::FRAME_pain104, TankFramesPain1, ConvertDerivedFunction(&CTank::Run));

CFrame TankFramesPain2 [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim TankMovePain2 (CTank::FRAME_pain201, CTank::FRAME_pain205, TankFramesPain2, ConvertDerivedFunction(&CTank::Run));

CFrame TankFramesPain3 [] =
{
	CFrame (&IMonster::AI_Move, -7),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 3),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CTank::Footstep))
};
CAnim TankMovePain3 (CTank::FRAME_pain301, CTank::FRAME_pain316, TankFramesPain3, ConvertDerivedFunction(&CTank::Run));

void CTank::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() |= 1;

	if (Damage <= 10)
		return;

	if (Level.Frame < PainDebounceTime)
		return;

	if (Damage <= 30 && frand() > 0.2)
		return;
	
	// If hard or nightmare, don't go into pain while attacking
	if ( CvarList[CV_SKILL].Integer() >= 2)
	{
		if ( (Entity->State.GetFrame() >= CTank::FRAME_attak301) && (Entity->State.GetFrame() <= CTank::FRAME_attak330) )
			return;
		if ( (Entity->State.GetFrame() >= CTank::FRAME_attak101) && (Entity->State.GetFrame() <= CTank::FRAME_attak116) )
			return;
	}

	PainDebounceTime = Level.Frame + 30;
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN]);

	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

#if ROGUE_FEATURES
	// PMM - blindfire cleanup
	AIFlags &= ~AI_MANUAL_STEERING;
	// pmm
#endif

	CurrentMove = ((Damage <= 60) ? ((Damage <= 30) ? &TankMovePain1 : &TankMovePain2) : &TankMovePain3);
};


//
// attacks
//

void CTank::Blaster ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start, end, dir;
	sint32		flash_number;

	switch (Entity->State.GetFrame())
	{
	case CTank::FRAME_attak110:
		flash_number = MZ2_TANK_BLASTER_1;
		break;
	case CTank::FRAME_attak113:
		flash_number = MZ2_TANK_BLASTER_2;
		break;
	default:
		flash_number = MZ2_TANK_BLASTER_3;
		break;
	}

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flash_number], angles, start);

	end = Entity->Enemy->State.GetOrigin();
	end.Z += Entity->Enemy->ViewHeight;
	dir = end - start;

	MonsterFireBlaster (start, dir, 30, 800, flash_number, FX_BLASTER);
}	

void CTank::Strike ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_STRIKE]);
}	

void CTank::Rocket ()
{
	if (!HasValidEnemy())
		return;

#if ROGUE_FEATURES
	vec3f	start, dir, vec, target;
	sint32		flash_number, rocketSpeed;
	CTrace	trace;
	bool blindfire = false;

	if(!Entity->Enemy || !Entity->Enemy->GetInUse())
		return;

	if (AIFlags & AI_MANUAL_STEERING)
		blindfire = true;

	switch (Entity->State.GetFrame())
	{
	case CTank::FRAME_attak324:
		flash_number = MZ2_TANK_ROCKET_1;
		break;
	case CTank::FRAME_attak327:
		flash_number = MZ2_TANK_ROCKET_2;
		break;
	default:
		flash_number = MZ2_TANK_ROCKET_3;
		break;
	}

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flash_number], angles, start);

	rocketSpeed = 500 + (100 * CvarList[CV_SKILL].Integer());	// PGM rock & roll.... :)

	target = (blindfire) ? BlindFireTarget : Entity->Enemy->State.GetOrigin();

	if (blindfire)
	{
		vec = target;
		dir = vec - start;
	}
	else if(frand() < 0.66 || (start[2] < Entity->Enemy->GetAbsMin().Z))
	{
		vec = Entity->Enemy->State.GetOrigin();
		vec.Z += Entity->Enemy->ViewHeight;
		dir = vec - start;
	}
	else
	{
		vec = Entity->Enemy->State.GetOrigin();
		vec.Z = Entity->Enemy->GetAbsMin().Z;
		dir = vec - start;
	}

	if (!blindfire && ((frand() < (0.2 + ((3 - CvarList[CV_SKILL].Integer()) * 0.15)))))
	{
		if (Entity->Enemy->EntityFlags & EF_PHYSICS)
			vec = vec.MultiplyAngles (dir.Length() / rocketSpeed, entity_cast<IPhysicsEntity>(*Entity->Enemy)->Velocity);
		dir = vec - start;
	}

	dir.NormalizeFast ();

	trace (start, vec, Entity, CONTENTS_MASK_SHOT);
	if (blindfire)
	{
		if (!(trace.StartSolid || trace.AllSolid || (trace.Fraction < 0.5)))
			MonsterFireRocket (start, dir, 50, rocketSpeed, flash_number);
		else 
		{
			vec = target.MultiplyAngles (-20, angles.Right);
			dir = vec - start;
			dir.NormalizeFast ();

			trace (start, vec, Entity, CONTENTS_MASK_SHOT);
			if (!(trace.StartSolid || trace.AllSolid || (trace.Fraction < 0.5)))
				MonsterFireRocket (start, dir, 50, rocketSpeed, flash_number);
			else 
			{
				vec = target.MultiplyAngles (20, angles.Right);
				dir = vec - start;
				dir.NormalizeFast ();

				trace (start, vec, Entity, CONTENTS_MASK_SHOT);
				if (!(trace.StartSolid || trace.AllSolid || (trace.Fraction < 0.5)))
					MonsterFireRocket (start, dir, 50, rocketSpeed, flash_number);
			}
		}
	}
	else
	{
		trace (start, vec, Entity, CONTENTS_MASK_SHOT);
		if (trace.Entity == Entity->Enemy || trace.Entity == World)
		{
			if (trace.Fraction > 0.5 || (trace.Entity->EntityFlags & EF_PLAYER))
				MonsterFireRocket (start, dir, 50, rocketSpeed, flash_number);
		}
	}
#else
	vec3f	start, dir, vec;
	sint32		flash_number;

	switch (Entity->State.GetFrame())
	{
	case CTank::FRAME_attak324:
		flash_number = MZ2_TANK_ROCKET_1;
		break;
	case CTank::FRAME_attak327:
		flash_number = MZ2_TANK_ROCKET_2;
		break;
	default:
		flash_number = MZ2_TANK_ROCKET_3;
		break;
	}

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flash_number], angles, start);

	vec = Entity->Enemy->State.GetOrigin();
	vec.Z += Entity->Enemy->ViewHeight;
	dir = vec - start;
	dir.NormalizeFast ();

	MonsterFireRocket (start, dir, 50, 550, flash_number);
#endif
}

void CTank::MachineGun ()
{
	if (!HasValidEnemy())
		return;

	vec3f		dir, start;
	sint32		flash_number = MZ2_TANK_MACHINEGUN_1 + (Entity->State.GetFrame() - CTank::FRAME_attak406);

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flash_number], angles, start);

	if (Entity->Enemy)
	{
		vec3f vec = Entity->Enemy->State.GetOrigin();
		vec.Z += Entity->Enemy->ViewHeight;
		vec -= start;
		vec = vec.ToAngles ();
		dir.X = vec.X;
	}
	else
		dir.X = 0;

	if (Entity->State.GetFrame() <= CTank::FRAME_attak415)
		dir.Y = Entity->State.GetAngles().Y - 8 * (Entity->State.GetFrame() - CTank::FRAME_attak411);
	else
		dir.Y = Entity->State.GetAngles().Y + 8 * (Entity->State.GetFrame() - CTank::FRAME_attak419);
	dir.Z = 0;

	angles = dir.ToVectors ();
	MonsterFireBullet (start, angles.Forward, 20, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, flash_number);
}	


CFrame TankFramesAttackBlast [] =
{
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, -2),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CTank::Blaster)),		// 10
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CTank::Blaster)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CTank::Blaster)) // 16
};
CAnim TankMoveAttackBlast (CTank::FRAME_attak101, CTank::FRAME_attak116, TankFramesAttackBlast, ConvertDerivedFunction(&CTank::ReAttackBlaster));

CFrame TankFramesReAttackBlast [] =
{
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CTank::Blaster)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CTank::Blaster))			// 16
};
CAnim TankMoveReAttackBlast (CTank::FRAME_attak111, CTank::FRAME_attak116, TankFramesReAttackBlast, ConvertDerivedFunction(&CTank::ReAttackBlaster));

CFrame TankFramesAttackPostBlast [] =	
{
	CFrame (&IMonster::AI_Move, 0),				// 17
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 3),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, -2,	ConvertDerivedFunction(&CTank::Footstep))		// 22
};
CAnim TankMoveAttackPostBlast (CTank::FRAME_attak117, CTank::FRAME_attak122, TankFramesAttackPostBlast, ConvertDerivedFunction(&CTank::Run));

void CTank::ReAttackBlaster ()
{
	if (CvarList[CV_SKILL].Integer() >= 2 && IsVisible (Entity, *Entity->Enemy) && entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0 && frand() <= 0.6)
	{
		CurrentMove = &TankMoveReAttackBlast;
		return;
	}
	CurrentMove = &TankMoveAttackPostBlast;
}

void CTank::PostStrike ()
{
	Entity->Enemy = nullentity;
	Run ();
}

CFrame TankFramesAttackStrike [] =
{
	CFrame (&IMonster::AI_Move, 3),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 6),
	CFrame (&IMonster::AI_Move, 7),
	CFrame (&IMonster::AI_Move, 9, ConvertDerivedFunction(&CTank::Footstep)),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 2, ConvertDerivedFunction(&CTank::Footstep)),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CTank::Windup)),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0, ConvertDerivedFunction(&CTank::Strike)),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -1),
	CFrame (&IMonster::AI_Move, -3),
	CFrame (&IMonster::AI_Move, -10),
	CFrame (&IMonster::AI_Move, -10),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, -3),
	CFrame (&IMonster::AI_Move, -2, ConvertDerivedFunction(&CTank::Footstep))
};
CAnim TankMoveAttackStrike (CTank::FRAME_attak201, CTank::FRAME_attak238, TankFramesAttackStrike, ConvertDerivedFunction(&CTank::PostStrike));

CFrame TankFramesAttackPreRocket [] =
{
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),			// 10

	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 1),
	CFrame (&IMonster::AI_Charge, 2),
	CFrame (&IMonster::AI_Charge, 7),
	CFrame (&IMonster::AI_Charge, 7),
	CFrame (&IMonster::AI_Charge, 7,  ConvertDerivedFunction(&CTank::Footstep)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),			// 20

	CFrame (&IMonster::AI_Charge, -3)
};
CAnim TankMoveAttackPreRocket (CTank::FRAME_attak301, CTank::FRAME_attak321, TankFramesAttackPreRocket, ConvertDerivedFunction(&CTank::DoAttackRocket));

CFrame TankFramesAttackFireRocket [] =
{
	CFrame (&IMonster::AI_Charge, -3),			// Loop Start	22 
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0,  ConvertDerivedFunction(&CTank::Rocket)),		// 24
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0,  ConvertDerivedFunction(&CTank::Rocket)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, -1, ConvertDerivedFunction(&CTank::Rocket))		// 30	Loop End
};
CAnim TankMoveAttackFireRocket (CTank::FRAME_attak322, CTank::FRAME_attak330, TankFramesAttackFireRocket, ConvertDerivedFunction(&CTank::ReFireRocket));

CFrame TankFramesAttackPostRocket [] =
{	
	CFrame (&IMonster::AI_Charge, 0),			// 31
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 2),
	CFrame (&IMonster::AI_Charge, 3),
	CFrame (&IMonster::AI_Charge, 4),
	CFrame (&IMonster::AI_Charge, 2),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),			// 40

	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, -9),
	CFrame (&IMonster::AI_Charge, -8),
	CFrame (&IMonster::AI_Charge, -7),
	CFrame (&IMonster::AI_Charge, -1),
	CFrame (&IMonster::AI_Charge, -1, ConvertDerivedFunction(&CTank::Footstep)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),			// 50

	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim TankMoveAttackPostRocket (CTank::FRAME_attak331, CTank::FRAME_attak353, TankFramesAttackPostRocket, ConvertDerivedFunction(&CTank::Run));

CFrame TankFramesAttackChain [] =
{
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim TankMoveAttackChain (CTank::FRAME_attak401, CTank::FRAME_attak429, TankFramesAttackChain, ConvertDerivedFunction(&CTank::Run));

void CTank::ReFireRocket ()
{
#if ROGUE_FEATURES
	// PMM - blindfire cleanup
	if (AIFlags & AI_MANUAL_STEERING)
	{
		AIFlags &= ~AI_MANUAL_STEERING;
		CurrentMove = &TankMoveAttackPostRocket;
		return;
	}
	// pmm
#endif

	// Only on hard or nightmare
	if (CvarList[CV_SKILL].Integer() >= 2 && entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0 && IsVisible(Entity, *Entity->Enemy) && frand() <= 0.4)
	{
		CurrentMove = &TankMoveAttackFireRocket;
		return;
	}
	CurrentMove = &TankMoveAttackPostRocket;
}

void CTank::DoAttackRocket ()
{
	CurrentMove = &TankMoveAttackFireRocket;
}

void CTank::Attack ()
{
	if (!Entity->Enemy)
		return;

	if ((Entity->Enemy->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(*Entity->Enemy)->Health < 0)
	{
		CurrentMove = &TankMoveAttackStrike;
		AIFlags &= ~AI_BRUTAL;
		return;
	}

#if ROGUE_FEATURES
	// PMM 
	if (AttackState == AS_BLIND)
	{
		// setup shot probabilities
		float r = frand();

		BlindFireDelay += 3.2 + 2.0 + frand()*3.0;

		// don't shoot at the origin
		if (BlindFireTarget == vec3fOrigin)
			return;

		// don't shoot if the dice say not to
		if (BlindFireDelay < 6.5 && (r > 0.4f))
			return;
		else if (r > 0.1f)
			return;

		// turn on manual steering to signal both manual steering and blindfire
		AIFlags |= AI_MANUAL_STEERING;
		CurrentMove = &TankMoveAttackFireRocket;
		AttackFinished = Level.Frame + 30 + ((2*frand())*10);
		PainDebounceTime = Level.Frame + 50;	// no pain for a while
		return;
	}
	// pmm
#endif

	float range = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).Length();
	float r = frand();

	if (range <= 125)
	{
		if (r < 0.4)
			CurrentMove = &TankMoveAttackChain;
		else 
			CurrentMove = &TankMoveAttackBlast;
	}
	else if (range <= 250)
	{
		if (r < 0.5)
			CurrentMove = &TankMoveAttackChain;
		else
			CurrentMove = &TankMoveAttackBlast;
	}
	else
	{
		if (r < 0.33)
			CurrentMove = &TankMoveAttackChain;
		else if (r < 0.66)
		{
			CurrentMove = &TankMoveAttackPreRocket;
			PainDebounceTime = Level.Frame + 50;	// no pain for a while
		}
		else
			CurrentMove = &TankMoveAttackBlast;
	}
}


//
// death
//

void CTank::Dead ()
{
	Entity->GetMins().Set (-16);
	Entity->GetMaxs().Set (16, 16, -0);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame TankFramesDeath1 [] =
{
	CFrame (&IMonster::AI_Move, -7),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 3),
	CFrame (&IMonster::AI_Move, 6),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 1),
	CFrame (&IMonster::AI_Move, 2),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -2),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -3),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, -4),
	CFrame (&IMonster::AI_Move, -6),
	CFrame (&IMonster::AI_Move, -4),
	CFrame (&IMonster::AI_Move, -5),
	CFrame (&IMonster::AI_Move, -7),
	CFrame (&IMonster::AI_Move, -15, ConvertDerivedFunction(&CTank::Thud)),
	CFrame (&IMonster::AI_Move, -5),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim TankMoveDeath (CTank::FRAME_death101, CTank::FRAME_death132, TankFramesDeath1, ConvertDerivedFunction(&CTank::Dead));

void CTank::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (sint32 n= 0; n < 1 /*4*/; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
		for (sint32 n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMetal(), Damage, GIB_METALLIC);
		CGibEntity::Spawn (Entity, GameMedia.Gib_Chest, Damage, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_Gear(), Damage, GIB_METALLIC);
		Entity->IsDead = true;
		return;
	}

	if (Entity->IsDead == true)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DIE]);
	Entity->IsDead = true;
	Entity->CanTakeDamage = true;

	CurrentMove = &TankMoveDeath;
}


//
// monster_tank
//

void CTank::Spawn ()
{
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/tank/tris.md2");
	Entity->GetMins().Set (-32, -32, -16);
	Entity->GetMaxs().Set (32, 32, 72);
	Entity->GetSolid() = SOLID_BBOX;

	Sounds[SOUND_PAIN] = SoundIndex ("tank/tnkpain2.wav");
	Sounds[SOUND_THUD] = SoundIndex ("tank/tnkdeth2.wav");
	Sounds[SOUND_IDLE] = SoundIndex ("tank/tnkidle1.wav");
	Sounds[SOUND_DIE] = SoundIndex ("tank/death.wav");
	Sounds[SOUND_STEP] = SoundIndex ("tank/step.wav");
	Sounds[SOUND_WINDUP] = SoundIndex ("tank/tnkatck4.wav");
	Sounds[SOUND_STRIKE] = SoundIndex ("tank/tnkatck5.wav");
	Sounds[SOUND_SIGHT] = SoundIndex ("tank/sight1.wav");

	SoundIndex ("tank/tnkatck1.wav");
	SoundIndex ("tank/tnkatk2a.wav");
	SoundIndex ("tank/tnkatk2b.wav");
	SoundIndex ("tank/tnkatk2c.wav");
	SoundIndex ("tank/tnkatk2d.wav");
	SoundIndex ("tank/tnkatk2e.wav");
	SoundIndex ("tank/tnkatck3.wav");

	Entity->Health = 750;
	Entity->GibHealth = -200;
	Entity->Mass = 500;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_SIGHT | MF_HAS_IDLE);

#if ROGUE_FEATURES
	// PMM
	AIFlags |= AI_IGNORE_SHOTS;
	BlindFire = true;
	//pmm
#endif

	Entity->Link ();
	WalkMonsterStart();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_tank", CTank);

void CTankCommander::Spawn ()
{
	CTank::Spawn ();
	Entity->Health = 1000;
	Entity->GibHealth = -225;
	Entity->State.GetSkinNum() = 2;
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_tank_commander", CTankCommander);

