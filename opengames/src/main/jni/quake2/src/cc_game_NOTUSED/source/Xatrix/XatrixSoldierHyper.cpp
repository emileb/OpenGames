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
// cc_xatrix_soldier_hyper.cpp
// 
//

#include "Local.h"

#if XATRIX_FEATURES
#include "Monsters/SoldierBase.h"
#include "Xatrix/XatrixSoldierHyper.h"
#include "Utility/TemporaryEntities.h"

CBlueBlasterProjectile::CBlueBlasterProjectile () :
  IFlyMissileProjectile(),
  ITouchableEntity(),
  IThinkableEntity()
{
};

CBlueBlasterProjectile::CBlueBlasterProjectile (sint32 Index) :
  IBaseEntity (Index),
  IFlyMissileProjectile(Index),
  ITouchableEntity(Index),
  IThinkableEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CBlueBlasterProjectile)

void CBlueBlasterProjectile::Think ()
{
	Free();
}

void CBlueBlasterProjectile::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	if (Other == GetOwner())
		return;

	if (surf && (surf->Flags & SURF_TEXINFO_SKY))
	{
		Free (); // "delete" the entity
		return;
	}

	if (GetOwner() && (GetOwner()->EntityFlags & EF_PLAYER))
		entity_cast<CPlayerEntity>(GetOwner())->PlayerNoiseAt (State.GetOrigin (), PNOISE_IMPACT);

	if ((Other->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(Other)->CanTakeDamage)
		entity_cast<IHurtableEntity>(Other)->TakeDamage (this, GetOwner(), Velocity, State.GetOrigin (), plane ? plane->Normal : vec3fOrigin, Damage, 1, DAMAGE_ENERGY, MOD_BLASTER);
	else
		CBlasterSplash(State.GetOrigin(), plane ? plane->Normal : vec3fOrigin, BL_BLUE_HYPERBLASTER).Send();

	Free (); // "delete" the entity
}

void CBlueBlasterProjectile::Spawn (IBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed, sint32 effect)
{
	CBlueBlasterProjectile		*Bolt = QNewEntityOf CBlueBlasterProjectile;

	Bolt->GetSvFlags() |= (SVF_DEADMONSTER | SVF_PROJECTILE);
	Bolt->State.GetOrigin() = start;
	Bolt->State.GetOldOrigin() = start;
	Bolt->State.GetAngles() = dir.ToAngles();
	Bolt->Velocity = dir.GetNormalizedFast() * speed;

	Bolt->State.GetEffects() = effect;
	Bolt->State.GetModelIndex() = ModelIndex ("models/objects/blaser/tris.md2");

	Bolt->State.GetSound() = SoundIndex ("misc/lasfly.wav");
	Bolt->SetOwner(Spawner);
	Bolt->NextThink = Level.Frame + 20;
	Bolt->Damage = Damage;
	Bolt->ClassName = "bolt";
	Bolt->GetClipmask() = CONTENTS_MASK_SHOT;
	Bolt->GetSolid() = SOLID_BBOX;
	Bolt->GetMins().Clear ();
	Bolt->GetMaxs().Clear ();
	Bolt->Touchable = true;
	Bolt->Link ();

	CTrace tr ((Spawner) ? Spawner->State.GetOrigin() : start, start, Bolt, CONTENTS_MASK_SHOT);
	if (tr.Fraction < 1.0)
	{
		start = start.MultiplyAngles (-10, dir.GetNormalizedFast());
		Bolt->State.GetOrigin() = start;
		Bolt->State.GetOldOrigin() = start;

		if (tr.Entity)
			Bolt->Touch (tr.Entity, &tr.Plane, tr.Surface);
	}
	else if (Spawner && (Spawner->EntityFlags & EF_PLAYER))
		CheckDodge (Spawner, start, dir, speed);
}

bool CBlueBlasterProjectile::Run ()
{
	return IFlyMissileProjectile::Run();
}

CSoldierHyper::CSoldierHyper (uint32 ID) :
CSoldierBase (ID)
{
}

CFrame SoldierHyperFramesAttack1 [] =
{
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierHyper::HyperSound)),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Fire1)),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Fire1)),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Fire1)),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Attack1_Refire1)),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierHyper::HyperRefire1)),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::CockGun)),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Attack1_Refire2)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim SoldierHyperMoveAttack1 (CSoldierBase::FRAME_attak101, CSoldierBase::FRAME_attak112, SoldierHyperFramesAttack1, ConvertDerivedFunction(&CSoldierBase::Run));

CFrame SoldierHyperFramesAttack2 [] =
{
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierHyper::HyperSound)),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Fire2)),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Fire2)),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Fire2)),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Attack2_Refire1)),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierHyper::HyperRefire2)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::CockGun)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Attack2_Refire2)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0)
};
CAnim SoldierHyperMoveAttack2 (CSoldierBase::FRAME_attak201, CSoldierBase::FRAME_attak218, SoldierHyperFramesAttack2, ConvertDerivedFunction(&CSoldierBase::Run));

extern CAnim SoldierMoveAttack6;

void CSoldierHyper::HyperSound ()
{
	Entity->PlaySound (CHAN_AUTO, SoundIndex("weapons/hyprbl1a.wav"));
}

void CSoldierHyper::HyperRefire1 ()
{
	if (frand() < 0.7f)
		Entity->State.GetFrame() = FRAME_attak103;
	else
		Entity->PlaySound (CHAN_AUTO, SoundIndex("weapons/hyprbd1a.wav"));
}

void CSoldierHyper::HyperRefire2 ()
{
	if (frand() < 0.7f)
		Entity->State.GetFrame() = FRAME_attak205;
	else
		Entity->PlaySound (CHAN_AUTO, SoundIndex("weapons/hyprbd1a.wav"));
}

void CSoldierHyper::Attack ()
{
#if ROGUE_FEATURES
	DoneDodge ();

	// PMM - blindfire!
	if (AttackState == AS_BLIND)
	{
		float r, chance;
		// setup shot probabilities
		if (BlindFireDelay < 1.0)
			chance = 1.0f;
		else if (BlindFireDelay < 7.5)
			chance = 0.4f;
		else
			chance = 0.1f;

		r = frand();

		// minimum of 2 seconds, plus 0-3, after the shots are done
		BlindFireDelay += 2.1 + 2.0 + frand()*3.0;

		// don't shoot at the origin
		if (BlindFireTarget == vec3fOrigin)
			return;

		// don't shoot if the dice say not to
		if (r > chance)
			return;

		// turn on manual steering to signal both manual steering and blindfire
		AIFlags |= AI_MANUAL_STEERING;
		CurrentMove = &SoldierHyperMoveAttack1;
		AttackFinished = Level.Frame + ((1.5 + frand()) * 10);
		return;
	}
	// pmm

	float r = frand();
	if ((!(AIFlags & (AI_BLOCKED|AI_STAND_GROUND))) &&
		(Range(Entity, *Entity->Enemy) >= RANGE_NEAR) && 
		(r < (CvarList[CV_SKILL].Integer()*0.25)))
		CurrentMove = &SoldierMoveAttack6;
	else
#endif
	{
		if (frand() < 0.5)
			CurrentMove = &SoldierHyperMoveAttack1;
		else
			CurrentMove = &SoldierHyperMoveAttack2;
	}
}

static sint32 BlasterFlash [] = {MZ2_SOLDIER_BLASTER_1, MZ2_SOLDIER_BLASTER_2, MZ2_SOLDIER_BLASTER_3, MZ2_SOLDIER_BLASTER_4, MZ2_SOLDIER_BLASTER_5, MZ2_SOLDIER_BLASTER_6, MZ2_SOLDIER_BLASTER_7, MZ2_SOLDIER_BLASTER_8};
void CSoldierHyper::FireGun (sint32 FlashNumber)
{
	if (!HasValidEnemy())
		return;

	vec3f	start, aim;

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[BlasterFlash[FlashNumber]], angles, start);

	switch (FlashNumber)
	{
	case 5:
	case 6:
		aim = angles.Forward;
		break;
	default:
		{
			IBaseEntity *Enemy = *Entity->Enemy;
			vec3f end;

			end = Enemy->State.GetOrigin() + vec3f(0, 0, Enemy->ViewHeight);
			aim = (end - start);

			vec3f dir;
			dir = aim.ToAngles ();

			angles = dir.ToVectors ();

			end = start.MultiplyAngles (8192, angles.Forward);
			end = end.MultiplyAngles (crand() * 100, angles.Right);
			end = end.MultiplyAngles (crand() * 50, angles.Up);

			aim = end - start;
			aim.Normalize ();
		}
		break;
	};

	MonsterFireBlueBlaster (start, aim, 1, 600, MZ_BLUEHYPERBLASTER);
}

void CSoldierHyper::SpawnSoldier ()
{
	Sounds[SOUND_PAIN] = SoundIndex ("soldier/solpain1.wav");
	Sounds[SOUND_DEATH] = SoundIndex ("soldier/soldeth1.wav");
	ModelIndex ("models/objects/blaser/tris.md2");
	SoundIndex ("misc/lasfly.wav");
	SoundIndex ("soldier/solatck1.wav");

	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/soldierh/tris.md2");
	Entity->State.GetSkinNum() = 2;
	SoldierAI = AI_SHOTGUN;
	Entity->Health = 60;
	Entity->GibHealth = -30;

#if ROGUE_FEATURES
	BlindFire = true;
#endif
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_soldier_hypergun", CSoldierHyper);
#endif
