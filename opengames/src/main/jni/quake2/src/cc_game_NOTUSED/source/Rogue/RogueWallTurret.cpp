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
// cc_rogue_wall_turret.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES

#include "Rogue/RogueWallTurret.h"
#include "Entities/BrushModels.h"
#include "Utility/TemporaryEntities.h"

class CWallTurretBase : public IBrushModel
{
public:
	CWallTurretBase () :
	  IBrushModel ()
	  {
	  };

	CWallTurretBase (sint32 Index) :
	  IBrushModel (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER (CWallTurretBase);

	static void Spawn (CWallTurret *Owner)
	{
		CWallTurretBase *Base = QNewEntityOf CWallTurretBase;
		Base->State.GetOrigin() = Owner->Entity->State.GetOrigin();
		Base->State.GetAngles() = Owner->Entity->State.GetAngles();

		int angle = (Base->State.GetAngles().X == 90) ? -1 : (Base->State.GetAngles().X == 270) ? -2 : Base->State.GetAngles().Y;

		switch (angle)
		{
			case -1:
				Base->GetMins().Set (-16, -16, -8);
				Base->GetMaxs().Set (16, 16, 0);
				break;
			case -2:
				Base->GetMins().Set (-16, -16, 0);
				Base->GetMaxs().Set (16, 16, 8);
				break;
			case 0:
				Base->GetMins().Set (-8, -16, -16);
				Base->GetMaxs().Set (0, 16, 16);
				break;
			case 90:
				Base->GetMins().Set (-16, -8, -16);
				Base->GetMaxs().Set (16, 0, 16);
				break;
			case 180:
				Base->GetMins().Set (0, -16, -16);
				Base->GetMaxs().Set (8, 16, 16);
				break;
			case 270:
				Base->GetMins().Set (-16, 0, -16);
				Base->GetMaxs().Set (16, 8, 16);
				break;
		}

		Base->PhysicsType = PHYSICS_PUSH;
		Base->GetSolid() = SOLID_NOT;
		Base->GetClipmask() = 0;

		Base->Team.Master = Owner->Entity;
		Owner->Entity->Team.Master = Owner->Entity;
		Owner->Entity->Team.Chain = Base;
		Base->Team.Chain = NULL;
		Base->SetOwner(Owner->Entity);

		Base->State.GetModelIndex() = ModelIndex("models/monsters/turretbase/tris.md2");
		Base->Link ();
	};
};

IMPLEMENT_SAVE_SOURCE (CWallTurretBase);

/**
\enum	

\brief	Values that represent spawnflags pertaining to CWallTurret. 
**/
enum
{
	SPAWN_BLASTER			= BIT(3),
	SPAWN_MACHINEGUN		= BIT(4),
	SPAWN_ROCKET			= BIT(5),
	SPAWN_HEATBEAM			= BIT(6),
	SPAWN_WEAPONCHOICE		= (SPAWN_BLASTER | SPAWN_MACHINEGUN | SPAWN_ROCKET | SPAWN_HEATBEAM),
	SPAWN_INSTANT_WEAPON	= (SPAWN_MACHINEGUN | SPAWN_HEATBEAM),
	SPAWN_WALL_UNIT			= BIT(7),
};

void CWallTurret::DamageEffect (vec3f &Dir, vec3f &Point, vec3f &Normal, sint32 &Damage, EDamageFlags &DamageFlags, EMeansOfDeath &MeansOfDeath)
{
	CSparks(Point, Normal, ST_ELECTRIC_SPARKS).Send();
}

CWallTurret::CWallTurret (uint32 ID) :
IMonster(ID)
{
}

CFrame TurretFrameStand [] =
{
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
};
CAnim TurretMoveStand (CWallTurret::FRAME_stand01, CWallTurret::FRAME_stand02, TurretFrameStand);

void CWallTurret::Stand ()
{
	if (!TurretEnabled)
		return;

	CurrentMove = &TurretMoveStand;
}

CFrame TurretFramesReadyGun [] =
{
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),

	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	CFrame (&IMonster::AI_Stand, 0),
	
	CFrame (&IMonster::AI_Stand, 0)
};
CAnim TurretMoveReadyGun (CWallTurret::FRAME_active01, CWallTurret::FRAME_run01, TurretFramesReadyGun, &IMonster::Run);

void CWallTurret::ReadyGun ()
{
	CurrentMove = &TurretMoveReadyGun;
}

CFrame TurretFramesSeek [] =
{
	CFrame (&IMonster::AI_Walk, 0, ConvertDerivedFunction(&CWallTurret::Aim)),
	CFrame (&IMonster::AI_Walk, 0, ConvertDerivedFunction(&CWallTurret::Aim))
};
CAnim TurretMoveSeek (CWallTurret::FRAME_run01, CWallTurret::FRAME_run02, TurretFramesSeek);

void CWallTurret::Walk ()
{
	if (!TurretEnabled)
		return;

	if (Entity->State.GetFrame() < FRAME_run01)
		ReadyGun ();
	else
		CurrentMove = &TurretMoveSeek;
}

CFrame TurretFramesRun [] =
{
	CFrame (&IMonster::AI_Run, 0, ConvertDerivedFunction(&CWallTurret::Aim)),
	CFrame (&IMonster::AI_Run, 0, ConvertDerivedFunction(&CWallTurret::Aim))
};
CAnim TurretMoveRun (CWallTurret::FRAME_run01, CWallTurret::FRAME_run02, TurretFramesRun, &IMonster::Run);

void CWallTurret::Run ()
{
	if (!TurretEnabled)
		return;

	if (Entity->State.GetFrame() < FRAME_run01)
		ReadyGun ();
	else
		CurrentMove = &TurretMoveRun;
}

// **********************
//  ATTACK
// **********************

const int TURRET_BULLET_DAMAGE		= 4;
const int TURRET_HEAT_DAMAGE		= 4;

void CWallTurret::Fire ()
{
	Aim ();

	if (!HasValidEnemy())
		return;

	vec3f dir = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).GetNormalized();
	anglef angles = Entity->State.GetAngles().ToVectors ();

	if ((dir | angles.Forward) < 0.98)
		return;

	float chance = frand();
	float rocketSpeed = 550;

	// rockets fire less often than the others do.
	if (Entity->SpawnFlags & SPAWN_ROCKET)
	{
		chance = chance * 3;

		if (Entity->SpawnFlags & SPAWN_ROCKET)
		{
			switch (CvarList[CV_SKILL].Integer())
			{
			case 2:
				rocketSpeed += 200 * frand();
				break;
			case 3:
				rocketSpeed += 100 + (200 * frand());
				break;
			};
		}
	}
	else if (Entity->SpawnFlags & SPAWN_BLASTER)
	{
		switch (CvarList[CV_SKILL].Integer())
		{
		case 0:
			rocketSpeed = 600;
			break;
		case 1:
			rocketSpeed = 800;
			break;
		default:
			rocketSpeed = 1000;
			break;
		};

		chance = chance * 2;
	}
	
	// up the fire chance 20% per skill level.
	chance -= (0.2f * CvarList[CV_SKILL].Float());

	if (IsVisible(Entity, *Entity->Enemy))
	{
		vec3f start = Entity->State.GetOrigin();
		vec3f end = Entity->Enemy->State.GetOrigin();
		
		// aim for the head.
		if ((Entity->Enemy) && (Entity->Enemy->EntityFlags & EF_PLAYER))
			end.Z += Entity->Enemy->ViewHeight;
		else
			end.Z += 22;

		dir = end - start;
		
		// check for predictive fire if distance less than 512
		if (!(Entity->SpawnFlags & SPAWN_INSTANT_WEAPON) && (dir.Length() < 512))
		{
			// ramp chance. easy - 50%, avg - 60%, hard - 70%, nightmare - 80%
			chance = frand() + (3 - CvarList[CV_SKILL].Float()) * 0.1f;

			// lead the target....
			if (chance < 0.8f)
				dir = (end.MultiplyAngles(dir.Length() / 1000, entity_cast<IPhysicsEntity>(*Entity->Enemy)->Velocity) - start);
		}

		//dir.Normalize ();
		CTrace trace (start, end, Entity, CONTENTS_MASK_SHOT);

		if (trace.Entity == Entity->Enemy || trace.Entity == World)
		{
			if (Entity->SpawnFlags & SPAWN_BLASTER)
				MonsterFireBlaster (start, dir, 20, rocketSpeed, MZ2_TURRET_BLASTER, FX_BLASTER);
			else if (Entity->SpawnFlags & SPAWN_MACHINEGUN)
				MonsterFireBullet (start, dir, TURRET_BULLET_DAMAGE, 0, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MZ2_TURRET_MACHINEGUN);
			else if (Entity->SpawnFlags & SPAWN_ROCKET)
			{
				if (dir.Length() * trace.Fraction > 72)
					MonsterFireRocket (start, dir.GetNormalized(), 50, rocketSpeed, MZ2_TURRET_ROCKET);
			}
		}	
	}
}

// PMM
void CWallTurret::FireBlind ()
{
	Aim ();

	if (!HasValidEnemy())
		return;

	vec3f dir = (BlindFireTarget - Entity->State.GetOrigin()).GetNormalized();

	anglef angles = Entity->State.GetAngles().ToVectors ();
	
	if ((dir | angles.Forward) < 0.98)
		return;

	float rocketSpeed = 550;
	if (Entity->SpawnFlags & SPAWN_ROCKET)
	{
		switch (CvarList[CV_SKILL].Integer())
		{
		case 2:
			rocketSpeed += 200 * frand();
			break;
		case 3:
			rocketSpeed += 100 + (200 * frand());
			break;
		};
	}
	
	vec3f start = Entity->State.GetOrigin(), end = BlindFireTarget;
		
	if (Entity->Enemy->State.GetOrigin().Z < BlindFireTarget.Z)
		end.Z += Entity->Enemy->ViewHeight + 10;
	else
		end.Z += Entity->Enemy->GetMins().Z - 10;

	dir = (end - start).GetNormalized();

	if (Entity->SpawnFlags & SPAWN_BLASTER)
		MonsterFireBlaster (start, dir, 20, 1000, MZ2_TURRET_BLASTER, FX_BLASTER);
	else if (Entity->SpawnFlags & SPAWN_ROCKET)
		MonsterFireRocket (start, dir, 50, rocketSpeed, MZ2_TURRET_ROCKET);
}
//pmm

CFrame TurretFramesFire [] =
{
	CFrame (&IMonster::AI_Run,   0, ConvertDerivedFunction(&CWallTurret::Fire)),
	CFrame (&IMonster::AI_Run,   0, ConvertDerivedFunction(&CWallTurret::Aim)),
	CFrame (&IMonster::AI_Run,   0, ConvertDerivedFunction(&CWallTurret::Aim)),
	CFrame (&IMonster::AI_Run,   0, ConvertDerivedFunction(&CWallTurret::Aim))
};
CAnim TurretMoveFire (CWallTurret::FRAME_pow01, CWallTurret::FRAME_pow04, TurretFramesFire, &IMonster::Run);

//PMM

// the blind frames need to aim first
CFrame TurretFramesFireBlind [] =
{
	CFrame (&IMonster::AI_Run,   0, ConvertDerivedFunction(&CWallTurret::Aim)),
	CFrame (&IMonster::AI_Run,   0, ConvertDerivedFunction(&CWallTurret::Aim)),
	CFrame (&IMonster::AI_Run,   0, ConvertDerivedFunction(&CWallTurret::Aim)),
	CFrame (&IMonster::AI_Run,   0, ConvertDerivedFunction(&CWallTurret::FireBlind))
};
CAnim TurretMoveFireBlind (CWallTurret::FRAME_pow01, CWallTurret::FRAME_pow04, TurretFramesFireBlind, &IMonster::Run);
//pmm

void CWallTurret::Attack ()
{
	if (!TurretEnabled)
		return;

	if (Entity->State.GetFrame() < FRAME_run01)
		ReadyGun ();
	// PMM
	else if (AttackState != AS_BLIND)
	{
		NextFrame = FRAME_pow01;
		CurrentMove = &TurretMoveFire;
	}
	else
	{
		float chance = 0.1f;
		// setup shot probabilities
		if (BlindFireDelay < 1.0)
			chance = 1.0f;
		else if (BlindFireDelay < 7.5)
			chance = 0.4f;

		// minimum of 3 seconds, plus 0-4, after the shots are done - total time should be max less than 7.5
		BlindFireDelay += 4 + 30 + frand()*40;
		// don't shoot at the origin
		if (BlindFireTarget == vec3fOrigin)
			return;

		// don't shoot if the dice say not to
		if (frand() > chance)
			return;

		NextFrame = FRAME_pow01;
		CurrentMove = &TurretMoveFireBlind;
	}
	// pmm
}

void CWallTurret::Aim ()
{
	if (!Entity->Enemy || Entity->Enemy == World)
	{
		if (!FindTarget ())
			return;
	}

	// if turret is still in inactive mode, ready the gun, but don't aim
	if (Entity->State.GetFrame() < FRAME_active01)
	{
		ReadyGun ();
		return;
	}
	// if turret is still readying, don't aim.
	if (Entity->State.GetFrame() < FRAME_run01)
		return;

	vec3f end;

	// PMM - blindfire aiming here
	if (CurrentMove == &TurretMoveFireBlind)
	{
		end = BlindFireTarget;

		if (Entity->Enemy->State.GetOrigin().Z < BlindFireTarget.Z)
			end.Z += Entity->Enemy->ViewHeight + 10;
		else
			end.Z += Entity->Enemy->GetMins().Z - 10;
	}
	else
	{
		end = Entity->Enemy->State.GetOrigin();

		if (Entity->Enemy->EntityFlags & EF_PLAYER)
			end.Z += Entity->Enemy->ViewHeight;
	}

	vec3f dir = end - Entity->State.GetOrigin();
	vec3f ang = dir.ToAngles();

	//
	// Clamp first
	//

	float idealPitch = ang.X;
	float idealYaw = ang.Y;

	int orientation = Offset.Y;
	switch (orientation)
	{
		case -1:			// up		pitch: 0 to 90
			if (idealPitch < -90)
				idealPitch += 360;
			if (idealPitch > -5)
				idealPitch = -5;
			break;
		case -2:			// down		pitch: -180 to -360
			if (idealPitch > -90)
				idealPitch -= 360;
			if (idealPitch < -355)
				idealPitch = -355;
			else if (idealPitch > -185)
				idealPitch = -185;
			break;
		case 0:				// +X		pitch: 0 to -90, -270 to -360 (or 0 to 90)
			if (idealPitch < -180)
				idealPitch += 360;

			if (idealPitch > 85)
				idealPitch = 85;
			else if (idealPitch < -85)
				idealPitch = -85;

							//			yaw: 270 to 360, 0 to 90
							//			yaw: -90 to 90 (270-360 == -90-0)
			if (idealYaw > 180)
				idealYaw -= 360;
			if (idealYaw > 85)
				idealYaw = 85;
			else if (idealYaw < -85)
				idealYaw = -85;
			break;
		case 90:			// +Y	pitch: 0 to 90, -270 to -360 (or 0 to 90)
			if (idealPitch < -180)
				idealPitch += 360;

			if (idealPitch > 85)
				idealPitch = 85;
			else if (idealPitch < -85)
				idealPitch = -85;

							//			yaw: 0 to 180
			if (idealYaw > 270)
				idealYaw -= 360;
			if (idealYaw > 175)	idealYaw = 175;
			else if (idealYaw < 5)	idealYaw = 5;

			break;
		case 180:			// -X	pitch: 0 to 90, -270 to -360 (or 0 to 90)
			if (idealPitch < -180)
				idealPitch += 360;

			if (idealPitch > 85)
				idealPitch = 85;
			else if (idealPitch < -85)
				idealPitch = -85;

							//			yaw: 90 to 270
			if (idealYaw > 265)	idealYaw = 265;
			else if (idealYaw < 95)	idealYaw = 95;

			break;
		case 270:			// -Y	pitch: 0 to 90, -270 to -360 (or 0 to 90)
			if (idealPitch < -180)
				idealPitch += 360;

			if (idealPitch > 85)
				idealPitch = 85;
			else if (idealPitch < -85)
				idealPitch = -85;

							//			yaw: 180 to 360
			if (idealYaw < 90)
				idealYaw += 360;
			if (idealYaw > 355)	idealYaw = 355;
			else if (idealYaw < 185)	idealYaw = 185;
			break;
	}

	//
	// adjust pitch
	//
	float current = Entity->State.GetAngles().X;
	float speed = 45;

	if (idealPitch != current)
	{
		float move = idealPitch - current;

		while (move >= 360)
			move -= 360;
	
		if (move >= 90)
			move = move - 360;

		while(move <= -360)
			move += 360;
		if (move <= -90)
			move = move + 360;

		if (move > 0)
		{
			if (move > speed)
				move = speed;
		}
		else
		{
			if (move < -speed)
				move = -speed;
		}

		Entity->State.GetAngles().X = AngleModf (current + move);
	}

	//
	// adjust yaw
	//
	current = Entity->State.GetAngles().Y;
	speed = 45;

	if (idealYaw != current)
	{
		float move = idealYaw - current;

		if (move >= 180)
			move = move - 360;

		if (move <= -180)
			move = move + 360;

		if (move > 0)
		{
			if (move > speed)
				move = speed;
		}
		else
		{
			if (move < -speed)
				move = -speed;
		}

		Entity->State.GetAngles().Y = AngleModf (current + move);
	}
}

// **********************
//  PAIN
// **********************

void CWallTurret::Pain (IBaseEntity *Other, sint32 Damage)
{
}

// **********************
//  DEATH
// **********************

void CWallTurret::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	CRocketExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_PHS, Entity->State.GetOrigin()), Entity->State.GetOrigin(), false, true).Send();

	anglef angles = Entity->State.GetAngles().ToVectors ();
	vec3f start = Entity->State.GetOrigin().MultiplyAngles (1, angles.Forward);

	if (Entity->Team.Chain)
	{
		CWallTurretBase *TurretBase = entity_cast<CWallTurretBase>(Entity->Team.Chain);
		TurretBase->GetSolid() = SOLID_BBOX;
		TurretBase->PhysicsType = PHYSICS_NONE;
		TurretBase->Link ();
	}

	if (!Entity->Target.IsNullOrEmpty())
	{
		if (Entity->Enemy && Entity->Enemy->GetInUse())
			Entity->UseTargets (*Entity->Enemy, Entity->Message);
		else
			Entity->UseTargets (Entity, Entity->Message);
	}

	Entity->Free ();
}

void CWallTurret::Wake ()
{
	CurrentMove = &TurretMoveStand;
	Entity->CanTakeDamage = true;
	Entity->PhysicsType = PHYSICS_NONE;
	// prevent counting twice
	AIFlags |= AI_DO_NOT_COUNT;

	Entity->Link ();

	StationaryMonsterStart ();
	
	if (Entity->SpawnFlags & SPAWN_MACHINEGUN)
		Entity->State.GetSkinNum() = 1;
	else if (Entity->SpawnFlags & SPAWN_ROCKET)
		Entity->State.GetSkinNum() = 2;

	// but we do want the death to count
	AIFlags &= ~AI_DO_NOT_COUNT;
}

void CWallTurret::MoveDone ()
{
	Entity->Velocity.Clear ();
	Wake ();
	TurretEnabled = true;
}

void CWallTurret::MoveFinal ()
{
	if (RemainingDistance == 0)
	{
		MoveDone ();
		return;
	}

	Entity->Velocity = Dir * RemainingDistance;

	ThinkType = TURRETTHINK_MOVEDONE;
	Entity->NextThink = Level.Frame + ServerFramesPerSecond;
}

void CWallTurret::MoveBegin ()
{
	if ((45 * 0.1f) >= RemainingDistance)
	{
		MoveFinal ();
		return;
	}
	Entity->Velocity = (Dir * 45) / 10;

	float frames = floor((RemainingDistance / 45) / 0.1f);
	RemainingDistance -= (frames * 45 / 10);
	Entity->NextThink = Level.Frame + frames;
	ThinkType = TURRETTHINK_MOVEFINAL;
}

void CWallTurret::MoveCalc (vec3f dest)
{
	Entity->Velocity.Clear ();
	Dir = dest - Entity->State.GetOrigin();
	RemainingDistance = Dir.Normalize();

	if (Level.CurrentEntity == (Entity->IsSlave() ? Entity->Team.Master : Entity))
		MoveBegin ();
	else
	{
		Entity->NextThink = Level.Frame + ServerFramesPerSecond;
		ThinkType = TURRETTHINK_MOVEBEGIN;
	}
}

void CWallTurret::MoveThink ()
{
	switch (ThinkType)
	{
	case TURRETTHINK_MOVEBEGIN:
		MoveBegin ();
		break;
	case TURRETTHINK_MOVEFINAL:
		MoveFinal ();
		break;
	case TURRETTHINK_MOVEDONE:
		MoveDone ();
		break;
	};
}

void CWallTurret::Use (IBaseEntity *Other, IBaseEntity *Activator)
{
	Entity->PhysicsType = PHYSICS_PUSH;

	vec3f forward;

	if (Entity->State.GetAngles().X == 270)
		forward.Set (0,0,1);
	else if (Entity->State.GetAngles().X == 90)
		forward.Set (0,0,-1);
	else if (Entity->State.GetAngles().Y == 0)
		forward.Set (1,0,0);
	else if (Entity->State.GetAngles().Y == 90)
		forward.Set (0,1,0);
	else if (Entity->State.GetAngles().Y == 180)
		forward.Set (-1,0,0);
	else if (Entity->State.GetAngles().Y == 270)
		forward.Set (0,-1,0);
	
	// start up the turret
	vec3f endpos = Entity->State.GetOrigin().MultiplyAngles (32, forward);
	MoveCalc(endpos);
	Think = ConvertDerivedFunction(&CWallTurret::MoveThink);

	if (Entity->Team.Chain)
	{
		CWallTurretBase *TurretBase = entity_cast<CWallTurretBase>(Entity->Team.Chain);
		TurretBase->PhysicsType = PHYSICS_PUSH;
		TurretBase->Speed = TurretBase->Accel = TurretBase->Decel = TurretBase->MoveSpeed = 45;

		// start up the wall section
		endpos = TurretBase->State.GetOrigin().MultiplyAngles (32, forward);
		TurretBase->MoveCalc (endpos, 0);
	}

	Entity->PlaySound (CHAN_VOICE, SoundIndex ("world/dr_short.wav"));
	Entity->Usable = false;
}

// PMM
// checkattack .. ignore range, just attack if available
bool CWallTurret::CheckAttack ()
{
	if (!Entity->Enemy || !Entity->Enemy->GetInUse())
		return false;

	if (!(Entity->Enemy->EntityFlags & EF_HURTABLE) || entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0)
	{
	// see if any entities are in the way of the shot
		vec3f spot1 = Entity->State.GetOrigin() + vec3f(0, 0, Entity->ViewHeight);
		vec3f spot2 = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight);

		CTrace tr (spot1, spot2, Entity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.Entity != Entity->Enemy)
		{	
			// PGM - we want them to go ahead and shoot at info_notnulls if they can.
			if (Entity->Enemy->GetSolid() != SOLID_NOT || tr.Fraction < 1.0)		//PGM
			{
				// PMM - if we can't see our target, and we're not blocked by a monster, go into blind fire if available
				if ((!(tr.Entity->GetSvFlags() & SVF_MONSTER)) && (!IsVisible(Entity, *Entity->Enemy)))
				{
					if ((BlindFire) && (BlindFireDelay <= 100))
					{
						if (Level.Frame < AttackFinished)
							return false;

						// wait for our time
						if (Level.Frame < (TrailTime + BlindFireDelay))
							return false;
						else
						{
							// make sure we're not going to shoot something we don't want to shoot
							tr (spot1, BlindFireTarget, Entity, CONTENTS_MONSTER);

							if (tr.AllSolid || tr.StartSolid || ((tr.Fraction < 1.0) && (tr.Entity != Entity->Enemy)))
								return false;

							AttackState = AS_BLIND;
							AttackFinished = Level.Frame + 5 + 20*frand();
							return true;
						}
					}
				}
				// pmm
				return false;
			}
		}
	}
	
	if (Level.Frame < AttackFinished)
		return false;

	EnemyRange = Range (Entity, *Entity->Enemy);

	if (EnemyRange == RANGE_MELEE)
	{
		// don't always melee in easy mode
		if (!CvarList[CV_SKILL].Boolean() && (randomMT()&3) )
			return false;
		AttackState = AS_MISSILE;
		return true;
	}

	float chance = 0.50f;
	FrameNumber nexttime = (8 - ((0.1f * CvarList[CV_SKILL].Float()) * 10));
	
	if (Entity->SpawnFlags & SPAWN_ROCKET)
	{
		chance = 0.10f;
		nexttime = (18 - ((0.2f * CvarList[CV_SKILL].Float()) * 10));
	}
	else if (Entity->SpawnFlags & SPAWN_BLASTER)
	{
		chance = 0.35f;
		nexttime = (12 - ((0.2f * CvarList[CV_SKILL].Float()) * 10));
	}

	if (!CvarList[CV_SKILL].Boolean())
		chance *= 0.5;
	else if (CvarList[CV_SKILL].Integer() > 1)
		chance *= 2;

	// PGM - go ahead and shoot every time if it's a info_notnull
	// PMM - added visibility check
	if ( ((frand() < chance) && (IsVisible(Entity, *Entity->Enemy))) || (Entity->Enemy->GetSolid() == SOLID_NOT))
	{
		AttackState = AS_MISSILE;
		AttackFinished = Level.Frame + nexttime;
		return true;
	}

	AttackState = AS_STRAIGHT;
	return false;
}


// **********************
//  SPAWN
// **********************

/*QUAKED monster_turret (1 .5 0) (-16 -16 -16) (16 16 16) Ambush Trigger_Spawn Sight Blaster MachineGun Rocket Heatbeam WallUnit

The automated defense turret that mounts on walls. 
Check the weapon you want it to use: blaster, machinegun, rocket, heatbeam.
Default weapon is blaster.
When activated, wall units move 32 units in the direction they're facing.
*/
void CWallTurret::Spawn ()
{
	SoundIndex ("world/dr_short.wav");
	ModelIndex ("models/objects/debris1/tris.md2");

	Entity->State.GetModelIndex() = ModelIndex("models/monsters/turret/tris.md2");

	Entity->GetMins().Set (-12, -12, -12);
	Entity->GetMaxs().Set (12, 12, 12);
	Entity->PhysicsType = PHYSICS_NONE;
	Entity->GetSolid() = SOLID_BBOX;

	Entity->Health = 240;
	Entity->GibHealth = -100;
	Entity->Mass = 250;

	// map designer didn't specify weapon type. set it now.
	if (!(Entity->SpawnFlags & SPAWN_WEAPONCHOICE))
		Entity->SpawnFlags |= SPAWN_BLASTER;

	if (Entity->SpawnFlags & SPAWN_HEATBEAM)
	{
		Entity->SpawnFlags &= ~SPAWN_HEATBEAM;
		Entity->SpawnFlags |= SPAWN_BLASTER;
	}

	TurretEnabled = false;

	if (!(Entity->SpawnFlags & SPAWN_WALL_UNIT))
	{
		TurretEnabled = true;
		CurrentMove = &TurretMoveStand;
	}

	AIFlags |= AI_MANUAL_STEERING;
	Entity->GravityMultiplier = 0;

	Offset = Entity->State.GetAngles();
	switch ((int)Entity->State.GetAngles().Y)
	{
		case -1:					// up
			Entity->State.GetAngles().X = 270;
			Entity->State.GetAngles().Y = 0;
			Entity->State.GetOrigin().Z += 2;
			break;
		case -2:					// down
			Entity->State.GetAngles().X = 90;
			Entity->State.GetAngles().Y = 0;
			Entity->State.GetOrigin().Z -= 2;
			break;
		case 0:
			Entity->State.GetOrigin().X += 2;
			break;
		case 90:
			Entity->State.GetOrigin().Y += 2;
			break;
		case 180:
			Entity->State.GetOrigin().X -= 2;
			break;
		case 270:
			Entity->State.GetOrigin().Y -= 2;
			break;
		default:
			break;
	}

	Entity->Link ();

	if (Entity->SpawnFlags & SPAWN_WALL_UNIT)
	{
		if (Entity->TargetName.IsNullOrEmpty())
		{
			Entity->Free ();
			return;
		}

		Entity->CanTakeDamage = false;
		Entity->Usable = true;
		Entity->UseState = MONSTERENTITY_THINK_CUSTOM;
		CWallTurretBase::Spawn (this);

		if ((!(AIFlags & AI_GOOD_GUY)) && (!(AIFlags & AI_DO_NOT_COUNT)))
			Level.Monsters.Total++;
	}
	else
		StationaryMonsterStart ();

	if (Entity->SpawnFlags & SPAWN_MACHINEGUN)
	{
		SoundIndex ("infantry/infatck1.wav");
		Entity->State.GetSkinNum() = 1;
	}
	else if (Entity->SpawnFlags & SPAWN_ROCKET)
	{
		SoundIndex ("weapons/rockfly.wav");
		ModelIndex ("models/objects/rocket/tris.md2");
		SoundIndex ("chick/chkatck2.wav");
		Entity->State.GetSkinNum() = 2;
	}
	else
	{
		if (!(Entity->SpawnFlags & SPAWN_BLASTER))
			Entity->SpawnFlags |= SPAWN_BLASTER;

		ModelIndex ("models/objects/laser/tris.md2");
		SoundIndex ("misc/lasfly.wav");
		SoundIndex ("soldier/solatck2.wav");
	}
	
	// PMM  - turrets don't get mad at monsters, and visa versa
	AIFlags |= AI_IGNORE_SHOTS;
	// PMM - blindfire
	if (Entity->SpawnFlags & (SPAWN_ROCKET|SPAWN_BLASTER))
		BlindFire = true;

	MonsterFlags |= (MF_HAS_ATTACK);
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_turret", CWallTurret);

#endif