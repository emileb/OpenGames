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
// cc_monster_ai.cpp
// 
//

#include "Local.h"

#if !ROGUE_FEATURES

#include "Entities/BrushModels.h"
#include "Utility/TemporaryEntities.h"

void IMonster::HuntTarget()
{
	Entity->GoalEntity = Entity->Enemy;

	if (AIFlags & AI_STAND_GROUND)
		Stand ();
	else
		Run ();

	IdealYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();

	// wait a while before first attack
	if (!(AIFlags & AI_STAND_GROUND))
		AttackFinished = Level.Frame + 10;
}

bool IMonster::FindTarget()
{
	if (AIFlags & AI_GOOD_GUY)
	{
		if (Entity->GoalEntity && !Entity->GoalEntity->ClassName.IsNullOrEmpty())
		{
			if (strcmp(Entity->GoalEntity->ClassName.CString(), "target_actor") == 0)
				return false;
		}

		return false;
	}

	// if we're going to a combat point, just proceed
	if (AIFlags & AI_COMBAT_POINT)
		return false;

// if the first spawnflag bit is set, the monster will only wake up on
// really seeing the player, not another monster getting angry or hearing
// something

// revised behavior so they will wake up if they "see" a player make a noise
// but not weapon impact/explosion noises

	bool heardit = false;
	IBaseEntity *client = NULL;

	if ((Level.SightEntityFrame >= (Level.Frame - 1)) && !(Entity->SpawnFlags & MONSTER_AMBUSH) )
	{
		client = Level.SightEntity;

		if (client->Enemy == Entity->Enemy)
			return false;
	}
	else if (Level.SoundEntityFrame >= (Level.Frame - 1))
	{
		client = Level.SoundEntity;
		heardit = true;
	}
	else if (!Entity->Enemy && (Level.SoundEntity2Frame >= (Level.Frame - 1)) && !(Entity->SpawnFlags & MONSTER_AMBUSH) )
	{
		client = Level.SoundEntity2;
		heardit = true;
	}
	else
	{
		client = Level.SightClient;

		if (!client)
			return false;	// no clients to get mad at
	}

	// if the entity went away, forget it
	if (!client->GetInUse())
		return false;

	if (client == Entity->Enemy)
		return true;	// JDC false;

	if (client->EntityFlags & EF_PLAYER)
	{
		if (client->Flags & FL_NOTARGET)
			return false;
	}
	else if (client->EntityFlags & EF_MONSTER)
	{
		if (!client->Enemy)
			return false;
		if (client->Enemy->Flags & FL_NOTARGET)
			return false;
	}
	else if (heardit)
	{
		if (client->GetOwner()->Flags & FL_NOTARGET)
			return false;
	}
	else
		return false;

	if (!heardit)
	{
		ERangeType r = Range (Entity, client);

		if (r == RANGE_FAR)
			return false;

// this is where we would check invisibility

		if (!IsVisible (Entity, client))
			return false;

		if (r == RANGE_NEAR)
		{
			if ((client->EntityFlags & EF_MONSTER) && (entity_cast<CMonsterEntity>(client)->ShowHostile) < Level.Frame && !IsInFront (Entity, client)
				|| !IsInFront (Entity, client))
				return false;
		}
		else if (r == RANGE_MID)
		{
			if (!IsInFront (Entity, client))
				return false;
		}

		Entity->Enemy = client;

		if (!(Entity->Enemy->EntityFlags & EF_NOISE))
		{
			AIFlags &= ~AI_SOUND_TARGET;

			if (!(Entity->Enemy->EntityFlags & EF_PLAYER))
			{
				Entity->Enemy = Entity->Enemy->Enemy;

				if (!(Entity->Enemy->EntityFlags & EF_PLAYER))
				{
					Entity->Enemy = nullentity;
					return false;
				}
			}
		}
	}
	else	// heardit
	{
		if (Entity->SpawnFlags & MONSTER_AMBUSH)
		{
			if (!IsVisible (Entity, client))
				return false;
		}
		else
		{
			if (!InHearableArea (Entity->State.GetOrigin(), client->State.GetOrigin()))
				return false;
		}

		vec3f temp = client->State.GetOrigin() - Entity->State.GetOrigin();

		if (temp.Length() > 1000)	// too far to hear
			return false;

		// check area portals - if they are different and not connected then we can't hear it
		if (client->GetAreaNum() != Entity->GetAreaNum() && (!client->AreasConnectedTo(Entity)))
			return false;

		IdealYaw = temp.ToYaw();
		ChangeYaw ();

		// hunt the sound for a bit; hopefully find the real player
		AIFlags |= AI_SOUND_TARGET;
		if (client->EntityFlags & EF_HURTABLE)
			Entity->Enemy = client;
		else
			return false; // can't be damaged (sounds etc..)
	}

//
// got one
//
	FoundTarget ();

	if (!(AIFlags & AI_SOUND_TARGET) && (MonsterFlags & MF_HAS_SIGHT))
		Sight ();

	return true;
}

void IMonster::MoveToGoal (float Dist)
{	
	if (!Entity->GoalEntity && !(AIFlags & (AI_FLY | AI_SWIM)))
		return;

// if the next step hits the enemy, return immediately
	if (Entity->Enemy && CloseEnough (*Entity->Enemy, Dist) )
		return;

// bump around...
	if ((randomMT() & 3) == 1 || !StepDirection (IdealYaw, Dist))
	{
		if (Entity->GetInUse())
			NewChaseDir (*Entity->GoalEntity, Dist);
	}
}

bool IMonster::MoveStep (vec3f move, bool ReLink)
{
// try the move	
	vec3f oldOrg = Entity->State.GetOrigin(), newOrg = oldOrg + move;

// flying monsters don't step up
	if (AIFlags & (AI_SWIM | AI_FLY))
	{
	// try one move with vertical motion, then one without
		for (int i = 0; i < 2; i++)
		{
			newOrg = Entity->State.GetOrigin() + move;

			if (i == 0 && Entity->Enemy)
			{
				if (!Entity->GoalEntity)
					Entity->GoalEntity = Entity->Enemy;

				float dz = Entity->State.GetOrigin().Z - Entity->GoalEntity->State.GetOldOrigin().Z;

				if (Entity->GoalEntity->EntityFlags & EF_PLAYER)
				{
					if (dz > 40)
						newOrg.Z -= 8;
					if (!((AIFlags & AI_SWIM) && (Entity->WaterInfo.Level < WATER_WAIST)) && (dz < 30))
						newOrg.Z += 8;
				}
				else
				{
					if (dz > 8)
						newOrg.Z -= 8;
					else if (dz > 0)
						newOrg.Z -= dz;
					else if (dz < -8)
						newOrg.Z += 8;
					else
						newOrg.Z += dz;
				}
			}
			CTrace trace (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), newOrg, Entity, CONTENTS_MASK_MONSTERSOLID);
	
			// fly monsters don't enter water voluntarily
			if (AIFlags & AI_FLY)
			{
				if (!Entity->WaterInfo.Level)
				{
					if (PointContents(trace.EndPosition + vec3f(0, 0, Entity->GetMins().Z + 1)) & CONTENTS_MASK_WATER)
						return false;
				}
			}

			// swim monsters don't exit water voluntarily
			if (AIFlags & AI_SWIM)
			{
				if (Entity->WaterInfo.Level < 2)
				{
					if (!(PointContents(trace.EndPosition + vec3f(0, 0, Entity->GetMins().Z + 1)) & CONTENTS_MASK_WATER))
						return false;
				}
			}

			if (trace.Fraction == 1)
			{
				Entity->State.GetOrigin() = trace.EndPosition;
				
				if (ReLink)
				{
					Entity->Link ();
					Entity->TouchTriggers ();
				}
				
				return true;
			}
			
			if (!Entity->Enemy)
				break;
		}
		
		return false;
	}

// push down from a step height above the wished position
	float stepsize = (!(AIFlags & AI_NOSTEP)) ? STEPSIZE : 1;

	newOrg.Z += stepsize;
	vec3f end = newOrg - vec3f(0, 0, stepsize * 2);

	CTrace trace (newOrg, Entity->GetMins(), Entity->GetMaxs(), end, Entity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.AllSolid)
		return false;

	if (trace.StartSolid)

	{
		newOrg.Z -= stepsize;
		trace (newOrg, Entity->GetMins(), Entity->GetMaxs(), end, Entity, CONTENTS_MASK_MONSTERSOLID);

		if (trace.AllSolid || trace.StartSolid)
			return false;
	}


	// don't go in to water
	if (Entity->WaterInfo.Level == 0)
	{
		if (PointContents(trace.EndPosition + vec3f(0, 0, Entity->GetMins().Z + 1)) & CONTENTS_MASK_WATER)
			return false;
	}

	if (trace.Fraction == 1)
	{
	// if monster had the ground pulled out, go ahead and fall
		if (AIFlags & AI_PARTIALGROUND)
		{
			Entity->State.GetOrigin() += move;

			if (ReLink)
			{
				Entity->Link ();
				Entity->TouchTriggers ();
			}

			Entity->GroundEntity = nullentity;
			return true;
		}
	
		return false;		// walked off an edge
	}

// check point traces down for dangling corners
	Entity->State.GetOrigin() = trace.EndPosition;
	
	if (!CheckBottom ())
	{
		if (AIFlags & AI_PARTIALGROUND)
		{
			// entity had floor mostly pulled out from underneath it
			// and is trying to correct
			
			if (ReLink)
			{
				Entity->Link ();
				Entity->TouchTriggers ();
			}
			return true;
		}

		Entity->State.GetOrigin() = oldOrg;
		return false;
	}

	if (AIFlags & AI_PARTIALGROUND)
		AIFlags &= ~AI_PARTIALGROUND;

	Entity->GroundEntity = trace.Entity;
	Entity->GroundEntityLinkCount = trace.Entity->GetLinkCount();

// the move is ok
	if (ReLink)
	{
		Entity->Link ();
		Entity->TouchTriggers ();
	}

	return true;
}

void IMonster::NewChaseDir (IBaseEntity *Enemy, float Dist)
{
	if (!Enemy)
		return;

	float olddir = AngleModf ((int)(IdealYaw / 45) * 45);
	float turnaround = AngleModf (olddir - 180);

	float deltax = Enemy->State.GetOrigin().X - Entity->State.GetOrigin().X;
	float deltay = Enemy->State.GetOrigin().Y - Entity->State.GetOrigin().Y;
	vec3f d;

	if (deltax > 10)
		d.Y = 0;
	else if (deltax < -10)
		d.Y = 180;
	else
		d.Y = DI_NODIR;

	if (deltay < -10)
		d.Z = 270;
	else if (deltay > 10)
		d.Z = 90;
	else
		d.Z = DI_NODIR;

// try direct route
	if (d.Y != DI_NODIR && d.Z != DI_NODIR)
	{
		float tdir;

		if (d.Y == 0)
			tdir = d[2] == 90 ? 45 : 315;
		else
			tdir = d[2] == 90 ? 135 : 215;
			
		if (tdir != turnaround && StepDirection (tdir, Dist))
			return;
	}

// try other directions
	if (((randomMT() & 3) & 1) || Abs (deltay) > Abs (deltax))
	{
		float tdir = d.Y;
		d.Y = d.Z;
		d.Z = tdir;
	}

	if (d[1]!=DI_NODIR && d[1]!=turnaround 
	&& StepDirection(d[1], Dist))
			return;

	if (d[2]!=DI_NODIR && d[2]!=turnaround
	&& StepDirection(d[2], Dist))
			return;

/* there is no direct path to the player, so pick another direction */

	if (olddir!=DI_NODIR && StepDirection(olddir, Dist))
			return;

	if (randomMT() & 1) 	/*randomly determine direction of search*/
	{
		for (int tdir=0 ; tdir<=315 ; tdir += 45)
			if (tdir != turnaround && StepDirection(tdir, Dist) )
					return;
	}
	else
	{
		for (int tdir=315 ; tdir >=0 ; tdir -= 45)
			if (tdir != turnaround && StepDirection(tdir, Dist) )
					return;
	}

	if (turnaround != DI_NODIR && StepDirection(turnaround, Dist) )
			return;

	IdealYaw = olddir;		// can't move

// if a bridge was pulled out from underneath a monster, it may not have
// a valid standing position at all

	if (!CheckBottom ())
		AIFlags |= AI_PARTIALGROUND;
}

bool IMonster::StepDirection (float Yaw, float Dist)
{	
	IdealYaw = Yaw;
	ChangeYaw ();
	
	Yaw = Yaw * M_PI * 2 / 360;
	vec3f move (cosf(Yaw)*Dist, sinf(Yaw)*Dist, 0);

	vec3f oldorigin = Entity->State.GetOrigin();
	if (MoveStep (move, false))
	{
		float delta = Entity->State.GetAngles().Y - IdealYaw;

		if (delta > 45 && delta < 315)	// not turned far enough, so don't take the step
			Entity->State.GetOrigin() = oldorigin;

		Entity->Link ();
		Entity->TouchTriggers ();
		return true;
	}

	Entity->Link ();
	Entity->TouchTriggers ();
	return false;
}

bool IMonster::CheckAttack ()
{
	if ((Entity->Enemy->EntityFlags & EF_HURTABLE) && (entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0))
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
		if (CvarList[CV_SKILL].Integer() == 0 && (rand()&3) )
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

	float chance;

	if (AIFlags & AI_STAND_GROUND)
		chance = 0.4f;
	else 
	{
		switch (EnemyRange)
		{
		case RANGE_MELEE:
			chance = 0.2f;
			break;
		case RANGE_NEAR:
			chance = 0.1f;
			break;
		case RANGE_MID:
			chance = 0.02f;
			break;
		default:
			return false;
		};
	}

	if (CvarList[CV_SKILL].Integer() == 0)
		chance *= 0.5;
	else if (CvarList[CV_SKILL].Integer() >= 2)
		chance *= 2;

	if (frand() < chance)
	{
		AttackState = AS_MISSILE;
		AttackFinished = Level.Frame + (20 * frand());
		return true;
	}

	if (AIFlags & AI_FLY)
	{
		if (frand() < 0.3f)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

	return false;
}

#include "Monsters/Tank.h"
#include "Monsters/Supertank.h"
#include "Monsters/Makron.h"
#include "Monsters/JORG.h"

void IMonster::ReactToDamage (IBaseEntity *Attacker, IBaseEntity *Inflictor)
{
	if (!(Attacker->EntityFlags & EF_PLAYER) && !(Attacker->EntityFlags & EF_MONSTER))
		return;

	if (Attacker == Entity || Attacker == Entity->Enemy)
		return;

	// if we are a good guy monster and our attacker is a player
	// or another good guy, do not get mad at them
	if (AIFlags & AI_GOOD_GUY)
	{
		if ((Attacker->EntityFlags & EF_PLAYER) || ((Attacker->EntityFlags & EF_MONSTER) && (entity_cast<CMonsterEntity>(Attacker)->Monster->AIFlags & AI_GOOD_GUY)))
			return;
	}

	// we now know that we are not both good guys

	// if attacker is a client, get mad at them because he's good and we're not
	if (Attacker->EntityFlags & EF_PLAYER)
	{
		AIFlags &= ~AI_SOUND_TARGET;

		// this can only happen in coop (both new and old enemies are clients)
		// only switch if can't see the current enemy
		if (Entity->Enemy && (Entity->Enemy->EntityFlags & EF_PLAYER))
		{
			if (IsVisible (Entity, *Entity->Enemy))
			{
				Entity->OldEnemy = Attacker;
				return;
			}

			Entity->OldEnemy = Entity->Enemy;
		}

		Entity->Enemy = Attacker;

		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
		return;
	}

	CMonsterEntity *AttackerMonster = (Attacker->EntityFlags & EF_MONSTER) ? entity_cast<CMonsterEntity>(Attacker) : NULL;

	// it's the same base (walk/swim/fly) type and a different classname and it's not a tank
	// (they spray too much), get mad at them
	if (((AIFlags & (AI_FLY | AI_SWIM)) == (AttackerMonster->Monster->AIFlags & (AI_FLY | AI_SWIM))) &&
			(MonsterID != AttackerMonster->Monster->MonsterID) &&
			(AttackerMonster->Monster->MonsterID != CTank::ID) &&
			(AttackerMonster->Monster->MonsterID != CSuperTank::ID) &&
			(AttackerMonster->Monster->MonsterID != CMakron::ID) &&
			(AttackerMonster->Monster->MonsterID != CJorg::ID))
	{
		if (Entity->Enemy && (Entity->Enemy->EntityFlags & EF_PLAYER))
			Entity->OldEnemy = Entity->Enemy;

		Entity->Enemy = Attacker;

		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}
	// if they *meant* to shoot us, then shoot back
	else if (Attacker->Enemy == Entity)
	{
		if (Entity->Enemy && (Entity->Enemy->EntityFlags & EF_PLAYER))
			Entity->OldEnemy = Entity->Enemy;
		
		Entity->Enemy = Attacker;
	
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}
	// otherwise get mad at whoever they are mad at (help our buddy) unless it is us!
	else if (Attacker->Enemy && Attacker->Enemy != Entity)
	{
		if (Entity->Enemy && (Entity->Enemy->EntityFlags & EF_PLAYER))
			Entity->OldEnemy = Entity->Enemy;

		Entity->Enemy = Attacker->Enemy;

		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}
}

void IMonster::AI_Walk(float Dist)
{
	MoveToGoal (Dist);

	// check for noticing a player
	if (FindTarget ())
		return;

	if ((MonsterFlags & MF_HAS_SEARCH) && (Level.Frame > IdleTime))
	{
		if (IdleTime)
		{
			Search ();
			IdleTime = Level.Frame + 150 + (frand() * 150);
		}
		else
			IdleTime = Level.Frame + (frand() * 150);
	}
}

void IMonster::AI_Charge(float Dist)
{
	IdealYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();
	ChangeYaw ();

	if (Dist)
		WalkMove (Entity->State.GetAngles().Y, Dist);
}

bool IMonster::AI_CheckAttack()
{
// this causes monsters to run blindly to the combat point w/o firing
	if (Entity->GoalEntity)
	{
		if (AIFlags & AI_COMBAT_POINT)
			return false;

		if (AIFlags & AI_SOUND_TARGET)
		{
			if ((Entity->Enemy->EntityFlags & EF_NOISE) && (Level.Frame - (entity_cast<CPlayerNoise>(*Entity->Enemy)->Time)) > 50)
			{
				if (Entity->GoalEntity == Entity->Enemy)
				{
					if (Entity->MoveTarget)
						Entity->GoalEntity = Entity->MoveTarget;
					else
						Entity->GoalEntity = nullentity;
				}

				AIFlags &= ~AI_SOUND_TARGET;
				if (AIFlags & AI_TEMP_STAND_GROUND)
					AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
			}
			else
			{
				Entity->ShowHostile = Level.Frame + 10;
				return false;
			}
		}
	}

	EnemyVis = false;

// see if the enemy is dead
	bool hesDeadJim = false;
	IHurtableEntity *HurtableEnemy = (Entity->Enemy && (Entity->Enemy->EntityFlags & EF_HURTABLE)) ? entity_cast<IHurtableEntity>(*Entity->Enemy) : NULL;

	if (!Entity->Enemy)
		hesDeadJim = true;
	else if (AIFlags & AI_MEDIC)
	{
		if ((HurtableEnemy) && (HurtableEnemy->Health > 0))
		{
			hesDeadJim = true;
			AIFlags &= ~AI_MEDIC;
		}
	}
	else
	{
		if (AIFlags & AI_BRUTAL)
		{
			if ((HurtableEnemy) && (HurtableEnemy->Health <= HurtableEnemy->GibHealth))
				hesDeadJim = true;
		}
		else
		{
			if ((HurtableEnemy) && (HurtableEnemy->Health <= 0))
				hesDeadJim = true;
		}
	}

	if (hesDeadJim)
	{
		Entity->Enemy = nullentity;

		if (Entity->OldEnemy && (Entity->OldEnemy->EntityFlags & EF_HURTABLE) && (entity_cast<IHurtableEntity>(*Entity->OldEnemy)->Health > 0))
		{
			Entity->Enemy = Entity->OldEnemy;
			Entity->OldEnemy = nullentity;
			HuntTarget ();
		}
		else
		{
			if (Entity->MoveTarget)
			{
				Entity->GoalEntity = Entity->MoveTarget;
				Walk ();
			}
			else
			{
				// we need the pausetime otherwise the stand code
				// will just revert to walking with no target and
				// the monsters will wonder around aimlessly trying
				// to hunt the world entity
				PauseTime = Level.Frame + 100000000;
				Stand ();
			}
			return true;
		}
	}

	Entity->ShowHostile = Level.Frame + 10;		// wake up other monsters

// check knowledge of enemy
	EnemyVis = IsVisible(Entity, *Entity->Enemy);
	if (EnemyVis)
	{
		SearchTime = Level.Frame + 50;
		LastSighting = Entity->Enemy->State.GetOrigin();
	}

	EnemyInfront = IsInFront (Entity, *Entity->Enemy);
	EnemyRange = Range (Entity, *Entity->Enemy);
	EnemyYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();


	// JDC self->ideal_yaw = enemy_yaw;

	if (AttackState == AS_MISSILE)
	{
		AI_Run_Missile ();
		return true;
	}
	if (AttackState == AS_MELEE)
	{
		AI_Run_Melee ();
		return true;
	}

	// if enemy is not currently visible, we will never attack
	if (!EnemyVis)
		return false;

	return CheckAttack ();
}

void IMonster::AI_Move (float Dist)
{
	WalkMove (Entity->State.GetAngles().Y, Dist);
}

void IMonster::AI_Run(float Dist)
{
	// if we're going to a combat point, just proceed
	if (AIFlags & AI_COMBAT_POINT)
	{
		MoveToGoal (Dist);
		return;
	}

	if (AIFlags & AI_SOUND_TARGET)
	{
		if ((Entity->State.GetOrigin() - Entity->Enemy->State.GetOrigin()).Length() < 64)
		{
			AIFlags |= (AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
			Stand ();
			return;
		}

		MoveToGoal (Dist);

		if (!FindTarget ())
			return;
	}

	if (AI_CheckAttack ())
		return;

	if (AttackState == AS_SLIDING)
	{
		AI_Run_Slide (Dist);
		return;
	}

	if (EnemyVis)
	{
		MoveToGoal (Dist);
		AIFlags &= ~AI_LOST_SIGHT;
		LastSighting = Entity->Enemy->State.GetOrigin();
		TrailTime = Level.Frame;
		return;
	}

	// coop will change to another enemy if visible
	if (Game.GameMode & GAME_COOPERATIVE)
	{
		if (FindTarget ())
			return;
	}

	if ((SearchTime) && (Level.Frame > (SearchTime + 200)))
	{
		MoveToGoal (Dist);
		SearchTime = 0;
		return;
	}

	//save = self->goalentity;
	//tempgoal = G_Spawn();
	//self->goalentity = tempgoal;

	bool isNew = false;

	if (!(AIFlags & AI_LOST_SIGHT))
	{
		// just lost sight of the player, decide where to go first
		AIFlags |= (AI_LOST_SIGHT | AI_PURSUIT_LAST_SEEN);
		AIFlags &= ~(AI_PURSUE_NEXT | AI_PURSUE_TEMP);
		isNew = true;
	}

	if (AIFlags & AI_PURSUE_NEXT)
	{
		CPlayerMarker *Marker;
		AIFlags &= ~AI_PURSUE_NEXT;

		// give ourself more time since we got this far
		SearchTime = Level.Frame + 50;

		if (AIFlags & AI_PURSUE_TEMP)
		{
//			dprint("was temp goal; retrying original\n");
			AIFlags &= ~AI_PURSUE_TEMP;
			Marker = NULL;
			LastSighting = SavedGoal;
			isNew = true;
		}
		else if (AIFlags & AI_PURSUIT_LAST_SEEN)
		{
			AIFlags &= ~AI_PURSUIT_LAST_SEEN;
			Marker = PlayerTrail_PickFirst (Entity);
		}
		else
		{
			Marker = PlayerTrail_PickNext (Entity);
		}

		if (Marker)
		{
			LastSighting = Marker->Origin;
			TrailTime = Marker->Timestamp;
			Entity->State.GetAngles().Y = IdealYaw = Marker->Angles.Y;

			isNew = true;
		}
	}

	vec3f v = Entity->State.GetOrigin() - LastSighting;
	float d1 = v.Length();

	if (d1 <= Dist)
	{
		AIFlags |= AI_PURSUE_NEXT;
		Dist = d1;
	}

	//Vec3Copy (self->monsterinfo.last_sighting, self->goalentity->s.origin);

	if (isNew)
	{
		CTrace tr (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), LastSighting, Entity, CONTENTS_MASK_PLAYERSOLID);

		if (tr.Fraction < 1)
		{
			v = LastSighting - Entity->State.GetOrigin();
			d1 = v.Length();
			float center = tr.Fraction;
			float d2 = d1 * ((center+1)/2);
			Entity->State.GetAngles().Y = IdealYaw = v.ToYaw();
		
			anglef angles = Entity->State.GetAngles().ToVectors ();

			v.Set (d2, -16, 0);
			vec3f left_target;
			G_ProjectSource (Entity->State.GetOrigin(), v, angles, left_target);
			tr (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), left_target, Entity, CONTENTS_MASK_PLAYERSOLID);
			float left = tr.Fraction;

			v.Set (d2, 16, 0);
			vec3f right_target;
			G_ProjectSource (Entity->State.GetOrigin(), v, angles, right_target);
			tr (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), right_target, Entity, CONTENTS_MASK_PLAYERSOLID);
			float right = tr.Fraction;

			center = (d1*center)/d2;
			if (left >= center && left > right)
			{
				if (left < 1)
				{
					v.Set (d2 * left * 0.5, -16, 0);
					G_ProjectSource (Entity->State.GetOrigin(), v, angles, left_target);
				}

				SavedGoal = LastSighting;
				AIFlags |= AI_PURSUE_TEMP;

				vec3f oldLeftTarget = LastSighting;
				LastSighting = left_target;
				
				Entity->State.GetAngles().Y = IdealYaw = (oldLeftTarget - Entity->State.GetOrigin()).ToYaw();
			}
			else if (right >= center && right > left)
			{
				if (right < 1)
				{
					v.Set (d2 * right * 0.5, 16, 0);
					G_ProjectSource (Entity->State.GetOrigin(), v, angles, right_target);
				}

				SavedGoal = LastSighting;
				AIFlags |= AI_PURSUE_TEMP;

				vec3f oldRightTarget = LastSighting;
				LastSighting = right_target;

				Entity->State.GetAngles().Y = IdealYaw = (oldRightTarget - Entity->State.GetOrigin()).ToYaw();
			}
		}
	}

	MoveToGoal (Dist);
}

void IMonster::AI_Run_Melee ()
{
	IdealYaw = EnemyYaw;
	ChangeYaw ();

	if (FacingIdeal())
	{
		Melee ();
		AttackState = AS_STRAIGHT;
	}
}

void IMonster::AI_Run_Missile()
{
	IdealYaw = EnemyYaw;
	ChangeYaw ();

	if (FacingIdeal())
	{
		Attack ();
		AttackState = AS_STRAIGHT;
	}
}

void IMonster::AI_Run_Slide(float Dist)
{	
	IdealYaw = EnemyYaw;
	ChangeYaw ();

	float ofs = (Lefty) ? 90 : -90;

	if (WalkMove (IdealYaw + ofs, Dist))
		return;
		
	Lefty = !Lefty;
	WalkMove (IdealYaw - ofs, Dist);
}

void IMonster::AI_Stand (float Dist)
{
	if (Dist)
		WalkMove (Entity->State.GetAngles().Y, Dist);

	if (AIFlags & AI_STAND_GROUND)
	{
		if (Entity->Enemy)
		{
			IdealYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();
			if (Entity->State.GetAngles().Y != IdealYaw && (AIFlags & AI_TEMP_STAND_GROUND))
			{
				AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
				Run ();
			}
			ChangeYaw ();
			AI_CheckAttack ();
		}
		else
			FindTarget ();
		return;
	}

	if (FindTarget ())
		return;
	
	if (Level.Frame > PauseTime)
	{
		Walk ();
		return;
	}

	if (!(Entity->SpawnFlags & MONSTER_AMBUSH) && (MonsterFlags & MF_HAS_IDLE) && (Level.Frame > IdleTime))
	{
		if (IdleTime)
		{
			Idle ();
			IdleTime = Level.Frame + 150 + (frand() * 150);
		}
		else
			IdleTime = Level.Frame + (frand() * 150);
	}
}

void IMonster::FoundTarget ()
{
	// let other monsters see this monster for a while
	if (Entity->Enemy->EntityFlags & EF_PLAYER)
	{
		Level.SightEntity = Entity;
		Level.SightEntityFrame = Level.Frame;
	}

	Entity->ShowHostile = Level.Frame + 10;		// wake up other monsters

	LastSighting = Entity->Enemy->State.GetOrigin();
	TrailTime = Level.Frame;

	if (Entity->CombatTarget.IsNullOrEmpty())
	{
		HuntTarget ();
		return;
	}

	Entity->GoalEntity = Entity->MoveTarget = CC_PickTarget (Entity->CombatTarget);

	if (!Entity->MoveTarget)
	{
		Entity->GoalEntity = Entity->MoveTarget = Entity->Enemy;
		HuntTarget ();
		MapPrint (MAPPRINT_ERROR, Entity, Entity->State.GetOrigin(), "CombatTarget %s not found\n", Entity->CombatTarget.CString());
		return;
	}

	// clear out our combattarget, these are a one shot deal
	Entity->CombatTarget.Clear();
	AIFlags |= AI_COMBAT_POINT;

	// clear the targetname, that point is ours!
	entity_cast<IMapEntity>(*Entity->MoveTarget)->TargetName.Clear();
	PauseTime = 0;

	// run for it
	Run ();
}

#endif
