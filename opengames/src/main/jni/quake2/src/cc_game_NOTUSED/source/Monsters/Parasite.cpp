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
// cc_parasite.cpp
// Parasite
//

#include "Local.h"
#include "Monsters/Parasite.h"

CParasite::CParasite (uint32 ID) :
IMonster(ID)
{
}

void CParasite::Launch ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_LAUNCH]);
}

void CParasite::ReelIn ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_REELIN]);
}

void CParasite::Sight ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_SIGHT]);
}

void CParasite::Tap ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_TAP], 255, ATTN_IDLE);
}

void CParasite::Scratch ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_SCRATCH], 255, ATTN_IDLE);
}

CFrame ParasiteFramesStartFidget [] =
{
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim ParasiteMoveStartFidget (CParasite::FRAME_stand18, CParasite::FRAME_stand21, ParasiteFramesStartFidget, ConvertDerivedFunction(&CParasite::DoFidget));

CFrame ParasiteFramesFidget [] =
{
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Scratch)),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Scratch)),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim ParasiteMoveFidget (CParasite::FRAME_stand22, CParasite::FRAME_stand27, ParasiteFramesFidget, ConvertDerivedFunction(&CParasite::ReFidget));

CFrame ParasiteFramesEndFidget [] =
{
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Scratch)),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim ParasiteMoveEndFidget (CParasite::FRAME_stand28, CParasite::FRAME_stand35, ParasiteFramesEndFidget, &IMonster::Stand);

void CParasite::DoFidget ()
{
	CurrentMove = &ParasiteMoveFidget;
}

void CParasite::ReFidget ()
{
	CurrentMove = (frand() <= 0.8) ? &ParasiteMoveFidget : &ParasiteMoveEndFidget;
}

void CParasite::Idle ()
{
	CurrentMove = &ParasiteMoveStartFidget;
}


CFrame ParasiteFramesStand [] =
{
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Tap)),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Tap)),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Tap)),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Tap)),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Tap)),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Tap))
};
CAnim ParasiteMoveStand (CParasite::FRAME_stand01, CParasite::FRAME_stand17, ParasiteFramesStand, &IMonster::Stand);

void CParasite::Stand ()
{
	CurrentMove = &ParasiteMoveStand;
}

CFrame ParasiteFramesRun [] =
{
	CFrame (&IMonster::AI_Run, 30),
	CFrame (&IMonster::AI_Run, 30),
	CFrame (&IMonster::AI_Run, 22),
	CFrame (&IMonster::AI_Run, 19),
	CFrame (&IMonster::AI_Run, 24),
	CFrame (&IMonster::AI_Run, 28),
	CFrame (&IMonster::AI_Run, 25)
};
CAnim ParasiteMoveRun (CParasite::FRAME_run03, CParasite::FRAME_run09, ParasiteFramesRun);

CFrame ParasiteFramesStartRun [] =
{
	CFrame (&IMonster::AI_Run, 0),
	CFrame (&IMonster::AI_Run, 30),
};
CAnim ParasiteMoveStartRun (CParasite::FRAME_run01, CParasite::FRAME_run02, ParasiteFramesStartRun, ConvertDerivedFunction(&CParasite::StartRun));

CFrame ParasiteFramesStopRun [] =
{	
	CFrame (&IMonster::AI_Run, 20),
	CFrame (&IMonster::AI_Run, 20),
	CFrame (&IMonster::AI_Run, 12),
	CFrame (&IMonster::AI_Run, 10),
	CFrame (&IMonster::AI_Run, 0),
	CFrame (&IMonster::AI_Run, 0)
};
CAnim ParasiteMoveStopRun (CParasite::FRAME_run10, CParasite::FRAME_run15, ParasiteFramesStopRun);

void CParasite::Run ()
{	
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &ParasiteMoveStand : &ParasiteMoveStartRun;
}

void CParasite::StartRun ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &ParasiteMoveStand : &ParasiteMoveRun;
}

CFrame ParasiteFramesWalk [] =
{
	CFrame (&IMonster::AI_Walk, 30),
	CFrame (&IMonster::AI_Walk, 30),
	CFrame (&IMonster::AI_Walk, 22),
	CFrame (&IMonster::AI_Walk, 19),
	CFrame (&IMonster::AI_Walk, 24),
	CFrame (&IMonster::AI_Walk, 28),
	CFrame (&IMonster::AI_Walk, 25)
};
CAnim ParasiteMoveWalk (CParasite::FRAME_run03, CParasite::FRAME_run09, ParasiteFramesWalk, &IMonster::Walk);

CFrame ParasiteFramesStartWalk [] =
{
	CFrame (&IMonster::AI_Walk, 0),
	CFrame (&IMonster::AI_Walk, 30, ConvertDerivedFunction(&CParasite::DoWalk))
};
CAnim ParasiteMoveStartWalk (CParasite::FRAME_run01, CParasite::FRAME_run02, ParasiteFramesStartWalk);

CFrame ParasiteFramesStopWalk [] =
{	
	CFrame (&IMonster::AI_Walk, 20),
	CFrame (&IMonster::AI_Walk, 20),
	CFrame (&IMonster::AI_Walk, 12),
	CFrame (&IMonster::AI_Walk, 10),
	CFrame (&IMonster::AI_Walk, 0),
	CFrame (&IMonster::AI_Walk, 0)
};
CAnim ParasiteMoveStopWalk (CParasite::FRAME_run10, CParasite::FRAME_run15, ParasiteFramesStopWalk);

void CParasite::Walk ()
{	
	CurrentMove = &ParasiteMoveStartWalk;
}

void CParasite::DoWalk ()
{
	CurrentMove = &ParasiteMoveWalk;
}

CFrame ParasiteFramesPain1 [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 6),
	CFrame (&IMonster::AI_Move, 16),
	CFrame (&IMonster::AI_Move, -6),
	CFrame (&IMonster::AI_Move, -7),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim ParasiteMovePain1 (CParasite::FRAME_pain101, CParasite::FRAME_pain111, ParasiteFramesPain1, &IMonster::Run);

void CParasite::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;

	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	Entity->PlaySound (CHAN_VOICE, (frand() < 0.5) ? Sounds[SOUND_PAIN1] : Sounds[SOUND_PAIN2]);

	CurrentMove = &ParasiteMovePain1;
}

bool CParasite::DrainAttackOK (vec3f &start, vec3f &end)
{
	vec3f dir = start - end;

	// check for max distance
	if (dir.Length() > 256)
		return false;

	// check for min/max pitch
	dir.Z = 0;
	vec3f angles = dir.ToAngles();
	if (angles.X < -180)
		angles.X += 360;
	if (Abs(angles.X) > 30)
		return false;

	return true;
}

#include "Utility/TemporaryEntities.h"

void CParasite::DrainAttack ()
{
	anglef angles = Entity->State.GetAngles().ToVectors ();
	
	static const vec3f frameOffsets[] =
	{
		vec3f(7 + 16.4f, 0, 5),
		vec3f(7 + 16.4f, 0, 4.4f),
		vec3f(7 + 16.4f, 0, 3),
		vec3f(6 + 16.4f, 0, 5),
		vec3f(4 + 16.4f, 0, 4),
		vec3f(5 + 16.4f, 0, 7),
		vec3f(5 + 16.4f, 0, 6),
		vec3f(4.7f + 16.4f, 0, 8),
		vec3f(4.3f + 16.4f, 0, 8),
		vec3f(1 + 16.4f, 0, 10),
		vec3f(2 + 16.4f, 0, 9)
	};

	vec3f start;
 	G_ProjectSource (Entity->State.GetOrigin(), frameOffsets[Entity->State.GetFrame() - 41], angles, start);

	vec3f end = Entity->Enemy->State.GetOrigin();
	if (!DrainAttackOK(start, end))
	{
		end.Z = Entity->Enemy->State.GetOrigin().Z + Entity->Enemy->GetMaxs().Z - 8;
		if (!DrainAttackOK(start, end))
		{
			end.Z = Entity->Enemy->State.GetOrigin().Z + Entity->Enemy->GetMaxs().Z + 8;
			if (!DrainAttackOK(start, end))
				return;
		}
	}
	end = Entity->Enemy->State.GetOrigin();

	CTrace tr (start, end, Entity, CONTENTS_MASK_SHOT);
	if (tr.Entity != Entity->Enemy)
		return;

	sint32 Damage = (Entity->State.GetFrame() == CParasite::FRAME_drain03) ? 5 : 2;
	if (Entity->State.GetFrame() == CParasite::FRAME_drain03)
		Entity->Enemy->PlaySound (CHAN_AUTO, Sounds[SOUND_IMPACT]);
	else if (Entity->State.GetFrame() == CParasite::FRAME_drain04)
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_SUCK]);

	CFleshCable(start, end, Entity->State.GetNumber()).Send();

	if (Entity->Enemy)
		entity_cast<IHurtableEntity>(*Entity->Enemy)->TakeDamage (Entity, Entity, start - end, Entity->Enemy->State.GetOrigin(), vec3fOrigin, Damage, 0, DAMAGE_NO_KNOCKBACK, MOD_UNKNOWN);
}

CFrame ParasiteFramesDrain [] =
{
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::Launch)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 15,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// Target hits
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&IMonster::AI_Charge, -2,  ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&IMonster::AI_Charge, -2,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&IMonster::AI_Charge, -3,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&IMonster::AI_Charge, -2,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&IMonster::AI_Charge, -1,  ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&IMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::ReelIn)),				// let go
	CFrame (&IMonster::AI_Charge, -2),
	CFrame (&IMonster::AI_Charge, -2),
	CFrame (&IMonster::AI_Charge, -3),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim ParasiteMoveDrain (CParasite::FRAME_drain01, CParasite::FRAME_drain18, ParasiteFramesDrain, &IMonster::Run);

/*
=== 
Break Stuff Ends
===
*/

void CParasite::Attack ()
{
//	if (frand() <= 0.2)
//		self->monsterinfo.currentmove = &parasite_move_break;
//	else
	CurrentMove = &ParasiteMoveDrain;
}

bool CParasite::CheckAttack ()
{
	if (!IMonster::CheckAttack ())
		return false;

	vec3f start = Entity->State.GetOrigin();
	vec3f end = Entity->Enemy->State.GetOrigin();
	return DrainAttackOK(start, end);
}

/*
===
Death Stuff Starts
===
*/

void CParasite::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame ParasiteFramesDeath [] =
{
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0),
	CFrame (&IMonster::AI_Move, 0)
};
CAnim ParasiteMoveDeath (CParasite::FRAME_death101, CParasite::FRAME_death107, ParasiteFramesDeath, ConvertDerivedFunction(&CParasite::Dead));

void CParasite::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
// check for gib
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

// regular death
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DIE]);
	Entity->IsDead = true;
	Entity->CanTakeDamage = true;
	CurrentMove = &ParasiteMoveDeath;
}

/*
===
End Death Stuff
===
*/

/*QUAKED monster_parasite (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void CParasite::Spawn ()
{
	Sounds[SOUND_PAIN1] = SoundIndex ("parasite/parpain1.wav");	
	Sounds[SOUND_PAIN2] = SoundIndex ("parasite/parpain2.wav");	
	Sounds[SOUND_DIE] = SoundIndex ("parasite/pardeth1.wav");	
	Sounds[SOUND_LAUNCH] = SoundIndex("parasite/paratck1.wav");
	Sounds[SOUND_IMPACT] = SoundIndex("parasite/paratck2.wav");
	Sounds[SOUND_SUCK] = SoundIndex("parasite/paratck3.wav");
	Sounds[SOUND_REELIN] = SoundIndex("parasite/paratck4.wav");
	Sounds[SOUND_SIGHT] = SoundIndex("parasite/parsght1.wav");
	Sounds[SOUND_TAP] = SoundIndex("parasite/paridle1.wav");
	Sounds[SOUND_SCRATCH] = SoundIndex("parasite/paridle2.wav");
	Sounds[SOUND_SEARCH] = SoundIndex("parasite/parsrch1.wav");

	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/parasite/tris.md2");
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, 24);

	Entity->GetSolid() = SOLID_BBOX;

	Entity->Health = 175;
	Entity->GibHealth = -50;
	Entity->Mass = 250;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_IDLE | MF_HAS_SIGHT);

	Entity->Link ();

	CurrentMove = &ParasiteMoveStand;	
	WalkMonsterStart ();

	Entity->ViewHeight = 12;
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_parasite", CParasite);
