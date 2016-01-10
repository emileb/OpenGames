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
// cc_turret_entities.cpp
// Turret entities
//

#include "Local.h"
#include "Entities/BrushModels.h"
#include "Monsters/Infantry.h"
#include "Monsters/TurretEntities.h"

void AnglesNormalize(vec3f &vec)
{
	while(vec.X > 360)
		vec.X -= 360;
	while(vec.X < 0)
		vec.X += 360;
	while(vec.Y > 360)
		vec.Y -= 360;
	while(vec.Y < 0)
		vec.Y += 360;
}

float SnapToEights(float x)
{
	x *= 8.0;
	if (x > 0.0)
		x += 0.5;
	else
		x -= 0.5;
	return 0.125 * (sint32)x;
}

CTurretEntityBase::CTurretEntityBase () :
IBaseEntity (),
IMapEntity (),
IBrushModel (),
IBlockableEntity ()
{
};

CTurretEntityBase::CTurretEntityBase (sint32 Index) :
IBaseEntity (Index),
IMapEntity (Index),
IBrushModel (Index),
IBlockableEntity (Index)
{
};

bool CTurretEntityBase::Run ()
{
	return IBrushModel::Run ();
};

void CTurretEntityBase::Blocked (IBaseEntity *Other)
{
	if ((Other->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(Other)->CanTakeDamage)
		entity_cast<IHurtableEntity>(Other)->TakeDamage (this, (Team.Master->GetOwner()) ? Team.Master->GetOwner() : Team.Master,
					vec3fOrigin, Other->State.GetOrigin(), vec3fOrigin, entity_cast<IBrushModel>(Team.Master)->Damage, 10, 0, MOD_CRUSH);
}

/*QUAKED turret_breach (0 0 0) ?
This portion of the turret can change both pitch and yaw.
The model  should be made with a flat pitch.
It (and the associated base) need to be oriented towards 0.
Use "angle" to set the starting angle.

"speed"		default 50
"dmg"		default 10
"angle"		point this forward
"target"	point this at an info_notnull at the muzzle tip
"minpitch"	min acceptable pitch angle : default -30
"maxpitch"	max acceptable pitch angle : default 30
"minyaw"	min acceptable yaw angle   : default 0
"maxyaw"	max acceptable yaw angle   : default 360
*/

CTurretBreach::CTurretBreach () :
IBaseEntity (),
CTurretEntityBase (),
FinishInit(true),
ShouldFire(false),
Target()
{
	PitchOptions[0] = PitchOptions[1] = PitchOptions[2] = PitchOptions[3] = 0;
};

CTurretBreach::CTurretBreach (sint32 Index) :
IBaseEntity (Index),
CTurretEntityBase (Index),
FinishInit(true),
ShouldFire(false),
Target()
{
	PitchOptions[0] = PitchOptions[1] = PitchOptions[2] = PitchOptions[3] = 0;
};

void CTurretBreach::Fire ()
{
	anglef angles = State.GetAngles().ToVectors ();
	vec3f start = State.GetOrigin().MultiplyAngles (MoveOrigin.X, angles.Forward);
	start = start.MultiplyAngles (MoveOrigin.Y, angles.Right);
	start = start.MultiplyAngles (MoveOrigin.Z, angles.Up);

	sint32 Damage = 100 + frand() * 50;
	CRocket::Spawn (Team.Master->GetOwner(), start, angles.Forward, Damage, 550 + 50 * CvarList[CV_SKILL].Integer(), 150, Damage);
	PlayPositionedSound (start, CHAN_WEAPON, SoundIndex("weapons/rocklf1a.wav"));
}

class CAvelocityForEachTeamChainCallback : public CForEachTeamChainCallback
{
public:
	float avelYaw;

	CAvelocityForEachTeamChainCallback (float avelYaw) :
	avelYaw(avelYaw)
	{
	};

	void Callback (IBaseEntity *Entity)
	{
		if (Entity->EntityFlags & EF_PHYSICS)
			entity_cast<IPhysicsEntity>(Entity)->AngularVelocity.Y = avelYaw;
	};
};

void CTurretBreach::Think ()
{
	if (FinishInit)
	{
		FinishInit = false;

		// get and save info for muzzle location
		if (Target.IsNullOrEmpty())
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "Needs a target\n");
		else
		{
			IBaseEntity *targ = CC_PickTarget (Target);
			MoveOrigin = (targ->State.GetOrigin() - State.GetOrigin());
			targ->Free();

			Target.Clear();
		}

		entity_cast<IBrushModel>(Team.Master)->Damage = Damage;
		Think ();
	}
	else
	{
		vec3f current_angles = State.GetAngles ();

		AnglesNormalize(current_angles);
		AnglesNormalize(MoveAngles);

		if (MoveAngles.X > 180)
			MoveAngles.X -= 360;

		// clamp angles to mins & maxs
		if (MoveAngles.X > Positions[0].X)
			MoveAngles.X = Positions[0].X;
		else if (MoveAngles.X < Positions[1].X)
			MoveAngles.X = Positions[1].X;

		if ((MoveAngles.Y < Positions[0].Y) || (MoveAngles.Y > Positions[1].Y))
		{
			float dmin = Abs(Positions[0].Y - MoveAngles.Y);
			if (dmin < -180)
				dmin += 360;
			else if (dmin > 180)
				dmin -= 360;

			float dmax = Abs(Positions[1].Y - MoveAngles.Y);
			if (dmax < -180)
				dmax += 360;
			else if (dmax > 180)
				dmax -= 360;

			MoveAngles.Y = Positions[(Abs(dmin) < Abs(dmax)) ? 0 : 1].Y;
		}

		vec3f delta = (MoveAngles - current_angles);
		if (delta.X < -180)
			delta.X += 360;
		else if (delta.X > 180)
			delta.X -= 360;
		if (delta.Y < -180)
			delta.Y += 360;
		else if (delta.Y > 180)
			delta.Y -= 360;
		delta.Z = 0;

		if (delta.X > Speed * 0.1f)
			delta.X = Speed * 0.1f;
		if (delta.X < -1 * Speed * 0.1f)
			delta.X = -1 * Speed * 0.1f;
		if (delta.Y > Speed * 0.1f)
			delta.Y = Speed * 0.1f;
		if (delta.Y < -1 * Speed * 0.1f)
			delta.Y = -1 * Speed * 0.1f;

		AngularVelocity = delta * 1.0/0.1f;
		NextThink = Level.Frame + ServerFramesPerSecond;

		CAvelocityForEachTeamChainCallback (AngularVelocity.Y).Query (this);

		// if we have adriver, adjust his velocities
		if (GetOwner() && (GetOwner()->EntityFlags & EF_MONSTER))
		{
			float	angle;
			float	target_z;
			float	diff;
			vec3f	target;
			vec3f	dir;
			CMonsterEntity *TheDriver = entity_cast<CMonsterEntity>(GetOwner());
			CTurretDriver *Driver = dynamic_cast<CTurretDriver*>(TheDriver->Monster);

			// angular is easy, just copy ours
			TheDriver->AngularVelocity.X = AngularVelocity.Z;
			TheDriver->AngularVelocity.Y = AngularVelocity.Y;

			// x & y
			angle = State.GetAngles().Y + Driver->MoveOrigin.Y;
			angle *= (((float)M_PI)*2 / 360);
			target.X = SnapToEights(State.GetOrigin().X + cosf(angle) * Driver->MoveOrigin.X);
			target.Y = SnapToEights(State.GetOrigin().Y + sinf(angle) * Driver->MoveOrigin.X);
			target.Z = TheDriver->State.GetOrigin().Z;

			dir = target - TheDriver->State.GetOrigin();

			TheDriver->Velocity.X = dir.X * 1.0 / 0.1f;
			TheDriver->Velocity.Y = dir.Y * 1.0 / 0.1f;

			// z
			angle = State.GetAngles().X * (M_PI*2 / 360);
			target_z = SnapToEights(State.GetOrigin().Z + Driver->MoveOrigin.X * tan(angle) + Driver->MoveOrigin.Z);

			diff = target_z - TheDriver->State.GetOrigin().Z;
			TheDriver->Velocity.Z = diff * 1.0 / 0.1f;
		}

		if (ShouldFire)
		{
			Fire ();
			ShouldFire = false;
		}
	}
}

ENTITYFIELDS_BEGIN(CTurretBreach)
{
	CEntityField ("minpitch", EntityMemberOffset(CTurretBreach,PitchOptions[0]), FT_FLOAT | FT_SAVABLE),
	CEntityField ("maxpitch", EntityMemberOffset(CTurretBreach,PitchOptions[1]), FT_FLOAT | FT_SAVABLE),
	CEntityField ("minyaw", EntityMemberOffset(CTurretBreach,PitchOptions[2]), FT_FLOAT | FT_SAVABLE),
	CEntityField ("maxyaw", EntityMemberOffset(CTurretBreach,PitchOptions[3]), FT_FLOAT | FT_SAVABLE),
	CEntityField ("target", EntityMemberOffset(CTurretBreach,Target), FT_STRING | FT_SAVABLE),
};
ENTITYFIELDS_END(CTurretBreach)

bool			CTurretBreach::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTurretBreach> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CTurretEntityBase::ParseField (Key, Value));
};

void			CTurretBreach::SaveFields (CFile &File)
{
	SaveEntityFields <CTurretBreach> (this, File);
	CTurretEntityBase::SaveFields (File);
}

void			CTurretBreach::LoadFields (CFile &File)
{
	LoadEntityFields <CTurretBreach> (this, File);
	CTurretEntityBase::LoadFields (File);
}

void CTurretBreach::Spawn ()
{
	GetSolid() = SOLID_BSP;
	PhysicsType = PHYSICS_PUSH;
	SetBrushModel ();

	if (!Speed)
		Speed = 50;
	if (!Damage)
		Damage = 10;

	if (!PitchOptions[0])
		PitchOptions[0] = -30;
	if (!PitchOptions[1])
		PitchOptions[1] = 30;
	if (!PitchOptions[3])
		PitchOptions[3] = 360;

	Positions[0].X = -1 * PitchOptions[0];
	Positions[0].Y = PitchOptions[2];
	Positions[1].X = -1 * PitchOptions[1];
	Positions[1].Y = PitchOptions[3];

	MoveAngles.Y = State.GetAngles().Y;
	State.GetAngles().Clear ();

	NextThink = Level.Frame + ServerFramesPerSecond;
	Link ();
}

LINK_CLASSNAME_TO_CLASS ("turret_breach", CTurretBreach);

/*QUAKED turret_base (0 0 0) ?
This portion of the turret changes yaw only.
MUST be teamed with a turret_breach.
*/

CTurretBase::CTurretBase () :
IBaseEntity (),
CTurretEntityBase ()
{
};

CTurretBase::CTurretBase (sint32 Index) :
IBaseEntity (Index),
CTurretEntityBase (Index)
{
};

void CTurretBase::Spawn ()
{
	GetSolid() = SOLID_BSP;
	PhysicsType = PHYSICS_PUSH;
	SetBrushModel ();
	Link ();
}

LINK_CLASSNAME_TO_CLASS ("turret_base", CTurretBase);

// Paril Note
// This may seem like a bit of a jump to call this guy a "monster",
// but he does have a lot of monster-like qualities.
// The only difference you'll see in-game is that in CleanCode
// the turret driver will not gib when you kill him; he'll do the
// infantry's death animation, like I hope they intended!

/*QUAKED turret_driver (1 .5 0) (-16 -16 -24) (16 16 32)
Must NOT be on the team with the rest of the turret parts.
Instead it must target the turret_breach.
*/

CTurretDriver::CTurretDriver (uint32 ID) :
CInfantry (ID),
TargetedBreach(NULL)
{
};

void CTurretDriver::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
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

	IBaseEntity	*TeamEntity;

	// level the gun
	TargetedBreach->MoveAngles.X = 0;

	// remove the driver from the end of them team chain
	for (TeamEntity = TargetedBreach->Team.Master; TeamEntity->Team.Chain != Entity; TeamEntity = TeamEntity->Team.Chain)
		;
	TeamEntity->Team.Chain = NULL;
	Entity->Team.Master = NULL;
	Entity->Velocity.Clear ();

	TargetedBreach->SetOwner(NULL);
	TargetedBreach->Team.Master->SetOwner(NULL);

	CInfantry::Die (Inflictor, Attacker, Damage, Point);
	Think = &IMonster::MonsterThink;
	Entity->NextThink = Level.Frame + ServerFramesPerSecond;
}

void CTurretDriver::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;

	Entity->PlaySound (CHAN_VOICE, (!irandom(2) == 0) ? Sounds[SOUND_PAIN1] : Sounds[SOUND_PAIN2]);
}

void CTurretDriver::TurretThink ()
{
	Entity->NextThink = Level.Frame + ServerFramesPerSecond;

	if (Entity->Enemy && (!(Entity->Enemy->EntityFlags & EF_HURTABLE) || (!Entity->Enemy || entity_cast<IHurtableEntity>(*Entity->Enemy)->Health <= 0)))
		Entity->Enemy = nullentity;

	if (!Entity->Enemy)
	{
		if (!FindTarget ())
			return;
		TrailTime = Level.Frame;
		AIFlags &= ~AI_LOST_SIGHT;
	}
	else
	{
		if (IsVisible (Entity, *Entity->Enemy))
		{
			if (AIFlags & AI_LOST_SIGHT)
			{
				TrailTime = Level.Frame;
				AIFlags &= ~AI_LOST_SIGHT;
			}
		}
		else
		{
			AIFlags |= AI_LOST_SIGHT;
			return;
		}
	}

	if (Entity->Enemy)
	{
		// let the turret know where we want it to aim
		vec3f dir = (Entity->Enemy->State.GetOrigin() +
			vec3f(0, 0, Entity->Enemy->ViewHeight)) -
			TargetedBreach->State.GetOrigin();

		vec3f ang = dir.ToAngles ();
		TargetedBreach->MoveAngles = ang;

		// decide if we should shoot
		if (Level.Frame < AttackFinished)
			return;

		float reaction_time = (3 - CvarList[CV_SKILL].Integer()) * 1.0;
		if ((Level.Frame - TrailTime) < (reaction_time * 10))
			return;

		AttackFinished = Level.Frame + ((reaction_time + 1.0) * 10);

		TargetedBreach->ShouldFire = true;
	}
}

void CTurretDriver::TurretLink ()
{
	Think = ConvertDerivedFunction(&CTurretDriver::TurretThink);
	Entity->NextThink = Level.Frame + ServerFramesPerSecond;

	TargetedBreach = entity_cast<CTurretBreach>(CC_PickTarget (Entity->Target));
	TargetedBreach->SetOwner(Entity);
	TargetedBreach->Team.Master->SetOwner(Entity);
	Entity->State.GetAngles() = TargetedBreach->State.GetAngles();

	vec3f vec = (TargetedBreach->State.GetOrigin() - Entity->State.GetOrigin());
	vec.Z = 0;
	MoveOrigin.X = vec.Length();

	vec = (Entity->State.GetOrigin() - TargetedBreach->State.GetOrigin());
	vec = vec.ToAngles();
	AnglesNormalize(vec);
	MoveOrigin.Y = vec.Y;

	MoveOrigin.Z = Entity->State.GetOrigin().Z - TargetedBreach->State.GetOrigin().Z;

	// add the driver to the end of them team chain
	IBaseEntity	*TeamEntity;
	for (TeamEntity = TargetedBreach->Team.Master; TeamEntity->Team.Chain; TeamEntity = TeamEntity->Team.Chain)
		;
	TeamEntity->Team.Chain = Entity;
	Entity->Team.Master = TargetedBreach->Team.Master;
}

void CTurretDriver::Spawn ()
{
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		Entity->Free ();
		return;
	}

	Entity->PhysicsType = PHYSICS_PUSH;
	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex("models/monsters/infantry/tris.md2");
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, 32);

	Entity->Health = Entity->MaxHealth = 100;
	Entity->GibHealth = -40;
	Entity->Mass = 200;
	Entity->ViewHeight = 24;

	Sounds[SOUND_PAIN1] = SoundIndex ("infantry/infpain1.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("infantry/infpain2.wav");
	Sounds[SOUND_DIE1] = SoundIndex ("infantry/infdeth1.wav");
	Sounds[SOUND_DIE2] = SoundIndex ("infantry/infdeth2.wav");

	Entity->AffectedByKnockback = false;

	Level.Monsters.Total++;

	Entity->GetSvFlags() |= SVF_MONSTER;
	Entity->State.GetRenderEffects() |= RF_FRAMELERP;
	Entity->CanTakeDamage = true;
	Entity->GetClipmask() = CONTENTS_MASK_MONSTERSOLID;
	Entity->State.GetOldOrigin() = Entity->State.GetOrigin ();
	AIFlags |= (AI_STAND_GROUND);

	Think = ConvertDerivedFunction(&CTurretDriver::TurretLink);
	Entity->NextThink = Level.Frame + ServerFramesPerSecond;

	Entity->Link ();
}
LINK_MONSTER_CLASSNAME_TO_CLASS ("turret_driver", CTurretDriver);
