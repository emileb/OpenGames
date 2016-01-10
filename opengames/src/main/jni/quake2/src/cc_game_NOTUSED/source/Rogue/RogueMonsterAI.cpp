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

#if ROGUE_FEATURES

#include "Rogue/RogueWeaponry.h"

//
// Monster "Bad" Areas
// 

#include "Utility/TemporaryEntities.h"

void CBadArea::Run ()
{
	if (Remove)
		return;

	if (!Owner || !Owner->GetInUse() || Owner->Freed || (Lifespan >= 1 && Lifespan < Level.Frame))
	{
		Remove = true;
		Owner = NULL;
	}
}

std::vector<CBadArea*> BadAreas;

void InitBadAreas ()
{
	for (size_t i = 0; i < BadAreas.size(); ++i)
		QDelete BadAreas[i];

	BadAreas.clear();
}

void SaveBadAreas (CFile &File)
{
	InitBadAreas ();

	File.Write<size_t> (BadAreas.size());
	for (size_t i = 0; i < BadAreas.size(); ++i)
		BadAreas[i]->Save (File);
}

void LoadBadAreas (CFile &File)
{
	InitBadAreas ();

	size_t sz = File.Read<size_t> ();
	for (size_t i = 0; i < sz; ++i)
		BadAreas.push_back (QNew (TAG_GAME) CBadArea(File));
}

bool RemoveBadArea (CBadArea *Area)
{
	if (Area->Remove)
	{
		QDelete Area;
		return true;
	}
	return false;
}

void RunBadAreas ()
{
	for (size_t i = 0; i < BadAreas.size(); ++i)
		BadAreas[i]->Run ();

	BadAreas.erase(std::remove_if(BadAreas.begin(), BadAreas.end(), RemoveBadArea), BadAreas.end());
}

CBadArea::CBadArea (vec3f AbsMin, vec3f AbsMax, FrameNumber Lifespan, IBaseEntity *Owner) :
  AbsMin (AbsMin),
  AbsMax (AbsMax),
  Lifespan (Lifespan),
  Owner (Owner),
  Remove (false)
  {
	  Origin = (AbsMin + AbsMax) * 0.5f;
	  Maxs = Origin - AbsMax;
	  Mins = Origin - AbsMin;

	  BadAreas.push_back(this);
  };

void SaveBadArea (CFile &File, CBadArea *Area)
{
	size_t i;

	for (i = 0; i < BadAreas.size(); ++i)
		if (BadAreas[i] == Area)
			break;

	if (i == BadAreas.size())
		File.Write<sint32> (-1);
	else
		File.Write<sint32> ((sint32)i);
};

CBadArea *LoadBadArea (CFile &File)
{
	sint32 i = File.Read<sint32> ();
	
	if (i == -1)
		return NULL;
	
	return BadAreas[i];
}

CBadArea *IMonster::CheckForBadArea ()
{
	for (size_t i = 0; i < BadAreas.size(); ++i)
	{
		if (Entity->State.GetOrigin().Inside (BadAreas[i]->AbsMin, BadAreas[i]->AbsMax))
			return BadAreas[i];
	}

	return NULL;
}

const int TESLA_DAMAGE_RADIUS		= 128;

bool IMonster::MarkTeslaArea (CTesla *Tesla)
{
	if (!Tesla)
		return false;

	// make sure this tesla doesn't have a bad area around it already...
	if (Tesla->BadArea)
		return false;

	vec3f mins = Tesla->State.GetOrigin() + vec3f(-TESLA_DAMAGE_RADIUS, -TESLA_DAMAGE_RADIUS, Tesla->GetMins().Z);
	vec3f maxs = Tesla->State.GetOrigin() + vec3f(TESLA_DAMAGE_RADIUS, TESLA_DAMAGE_RADIUS, TESLA_DAMAGE_RADIUS);

	CBadArea *area = QNew(TAG_GAME) CBadArea (mins, maxs, (Tesla->RemoveTime) ? Tesla->RemoveTime : (Tesla->NextThink) ? Tesla->NextThink : 30, Tesla);
	Tesla->BadArea = area;

	return true;
}

void IMonster::TargetTesla (CTesla *tesla)
{
	if (!tesla)
		return;

	// PMM - medic bails on healing things
	if (AIFlags & AI_MEDIC)
	{
		if (Entity->Enemy)
			entity_cast<CMonsterEntity>(*Entity->Enemy)->Monster->CleanupHealTarget();
		AIFlags &= ~AI_MEDIC;
	}

	// store the player enemy in case we lose track of him.
	if (Entity->Enemy && (Entity->Enemy->EntityFlags & EF_PLAYER))
		LastPlayerEnemy = entity_cast<CPlayerEntity>(*Entity->Enemy);

	if (Entity->Enemy != tesla)
	{
		Entity->OldEnemy = (Entity->Enemy) ? Entity->Enemy : LastPlayerEnemy;
		Entity->Enemy = tesla;

		if (MonsterFlags & MF_HAS_ATTACK)
		{
			if (Entity->Health <= 0)
				return;

			Attack();
		}
		else
			FoundTarget();
	}
}


void IMonster::DoneDodge ()
{
	AIFlags &= ~AI_DODGING;
}

void IMonster::MonsterDodge (IBaseEntity *Attacker, float eta, CTrace *tr)
{
	// this needs to be here since this can be called after the monster has "died"
	if (Entity->Health < 1)
		return;

	bool	ducker = ((MonsterFlags & MF_HAS_DUCK) && (MonsterFlags & MF_HAS_UNDUCK)),
			dodger = ((MonsterFlags & MF_HAS_SIDESTEP) && !(AIFlags & AI_STAND_GROUND));

	if ((!ducker) && (!dodger))
		return;

	if (!Entity->Enemy)
	{
		Entity->Enemy = Attacker;
		FoundTarget ();
	}

	// PMM - don't bother if it's going to hit anyway; fix for weird in-your-face etas (I was
	// seeing numbers like 13 and 14)
	if ((eta < 0.1) || (eta > 5))
		return;

	// skill level determination..
	if (frand() > (0.25f*((CvarList[CV_SKILL].Integer())+1)))
		return;

	float height = Entity->GetAbsMax().Z;

	if (ducker)
	{
		height = Entity->GetAbsMax().Z - 32 - 1;  // the -1 is because the absmax is s.origin + maxs + 1

		// FIXME, make smarter
		// if we only duck, and ducking won't help or we're already ducking, do nothing
		//
		// need to add monsterinfo.abort_duck() and monsterinfo.next_duck_time

		if ((!dodger) && ((tr->EndPosition.Z <= height) || (AIFlags & AI_DUCKED)))
			return;
	}

	if (dodger)
	{
		// if we're already dodging, just finish the sequence, i.e. don't do anything else
		if (AIFlags & AI_DODGING)
			return;

		// if we're ducking already, or the shot is at our knees
		if ((tr->EndPosition.Z <= height) || (AIFlags & AI_DUCKED))
		{
			vec3f right, diff;
			anglef angles = Entity->State.GetAngles().ToVectors ();
			diff = tr->EndPosition - Entity->State.GetOrigin();

			Lefty = !((angles.Right | diff) < 0);
	
			// if we are currently ducked, unduck

			if ((ducker) && (AIFlags & AI_DUCKED))
				UnDuck ();

			AIFlags |= AI_DODGING;
			AttackState = AS_SLIDING;

			// call the monster specific code here
			SideStep ();
			return;
		}
	}

	if (ducker)
	{
		if (NextDuckTime > Level.Frame)
			return;

		DoneDodge ();

		// set this prematurely; it doesn't hurt, and prevents extra iterations
		AIFlags|= AI_DUCKED;
		Duck (eta);
	}
}

/*
ROGUE
clean up heal targets for medic
*/
void IMonster::CleanupHealTarget ()
{
	Healer = nullentity;
	AIFlags &= ~AI_RESURRECTING;
	SetEffects ();
}

void IMonster::DuckDown ()
{
	AIFlags |= AI_DUCKED;

	Entity->GetMaxs().Z = BaseHeight - 32;
	if (DuckWaitTime < Level.Frame + 10)
		DuckWaitTime = Level.Frame + 10;
	
	Entity->Link ();
}

void IMonster::DuckHold ()
{
	if (Level.Frame >= DuckWaitTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void IMonster::DuckUp ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->GetMaxs().Z = BaseHeight;
	NextDuckTime = Level.Frame + 5;
	Entity->Link ();
}

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
		if (Entity->GoalEntity && Entity->GoalEntity->GetInUse() && !Entity->GoalEntity->ClassName.IsNullOrEmpty())
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

	IBaseEntity *client;
	bool heardit = false;
	if ((Level.SightEntityFrame >= (Level.Frame - 1)) && !(Entity->SpawnFlags & MONSTER_AMBUSH) )
	{
		client = Level.SightEntity;
		if (client->Enemy == Entity->Enemy)
			return false;
	}
	else if (Level.DisguiseViolationFrametime > Level.Frame)
		client = Level.DisguiseViolator;
	else if (Level.SoundEntityFrame >= (Level.Frame - 1))
	{
		client = Level.SoundEntity;
		heardit = true;
	}
	else if (!(Entity->Enemy) && (Level.SoundEntity2Frame >= (Level.Frame - 1)) && !(Entity->SpawnFlags & MONSTER_AMBUSH) )
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
		// pgm - a little more paranoia won't hurt....
		if ((client->GetOwner()) && (client->GetOwner()->Flags & FL_NOTARGET))
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

		vec3f temp = (client->State.GetOrigin() - Entity->State.GetOrigin());
		if (temp.Length() > 1000)	// too far to hear
			return false;

		// check area portals - if they are different and not connected then we can't hear it
		if (client->GetAreaNum() != Entity->GetAreaNum())
			if (!client->AreasConnectedTo(Entity))
				return false;
	
		IdealYaw = temp.ToYaw();
		if (!(AIFlags & AI_MANUAL_STEERING))
			ChangeYaw ();

		// hunt the sound for a bit; hopefully find the real player
		AIFlags |= AI_SOUND_TARGET;
		Entity->Enemy = client;
	}

//
// got one
//

	FoundTarget ();

	// pmm
	if (!(AIFlags & AI_SOUND_TARGET) && (MonsterFlags & MF_HAS_SIGHT))
		Sight ();

	return true;
}

void IMonster::MoveToGoal (float Dist)
{	
	IBaseEntity *goal = *Entity->GoalEntity;

	if (!Entity->GroundEntity && !(AIFlags & (AI_FLY | AI_SWIM)))
		return;

// if the next step hits the enemy, return immediately
	if (Entity->Enemy && CloseEnough (*Entity->Enemy, Dist) )
		return;

// bump around...
// PMM - charging monsters (AI_CHARGING) don't deflect unless they have to
	if ( (((randomMT()&3)==1) && !(AIFlags & AI_CHARGING)) || !StepDirection (IdealYaw, Dist))
	{
		if (AIFlags & AI_BLOCKED)
		{
			AIFlags &= ~AI_BLOCKED;
			return;
		}
		if (Entity->GetInUse())
			NewChaseDir (goal, Dist);
	}
}

bool IMonster::IsBadAhead (CBadArea *bad, vec3f move)
{
	vec3f move_copy = move;
	vec3f dir = (bad->Origin - Entity->State.GetOrigin()).GetNormalized();

	float dp_bad = Entity->State.GetAngles().ToVectors().Forward | dir;

	move_copy.Normalize();
	float dp_move = Entity->State.GetAngles().ToVectors().Forward | move_copy;

	if ((dp_bad < 0) && (dp_move < 0))
		return true;
	if ((dp_bad > 0) && (dp_move > 0))
		return true;

	return false;
}

#include "Rogue/RogueCarrier.h"
#include "Monsters/Icarus.h"
#include "Rogue/RogueDaedalus.h"
#include "Rogue/RogueWidowStand.h"
#include "Rogue/RogueBlackWidow.h"

bool IMonster::MoveStep (vec3f move, bool ReLink)
{
	CBadArea *current_bad = NULL;

	// PMM - who cares about bad areas if you're dead?
	if (Entity->Health > 0)
	{
		current_bad = CheckForBadArea();
		if (current_bad)
		{
			BadArea = current_bad;
			 
			if (Entity->Enemy && !strcmp(Entity->Enemy->ClassName.CString(), "tesla"))
			{
				// if the tesla is in front of us, back up...
				if (IsBadAhead (current_bad, move))
					move *= -1;
			}
		}
		else if (BadArea)
		{
			// if we're no longer in a bad area, get back to business.
			BadArea = NULL;
			if (Entity->OldEnemy)
			{
				Entity->Enemy = Entity->OldEnemy;
				Entity->GoalEntity = Entity->OldEnemy;
				FoundTarget();

				return true;
			}
		}
	}

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

				float dz = Entity->State.GetOrigin().Z - Entity->GoalEntity->State.GetOrigin().Z;
				if (Entity->GoalEntity->EntityFlags & EF_PLAYER)
				{
					// we want the carrier to stay a certain distance off the ground, to help prevent him
					// from shooting his fliers, who spawn in below him
					//
					float minHeight = (MonsterID == CCarrier::ID) ? 104 : 40;

					if (dz > minHeight)
						newOrg.Z -= 8;
					if (!((AIFlags & AI_SWIM) && (Entity->WaterInfo.Level < WATER_WAIST)))
						if (dz < (minHeight - 10))
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
				if (Entity->WaterInfo.Level < WATER_WAIST)
				{
					if (!(PointContents(trace.EndPosition + vec3f(0, 0, Entity->GetMins().Z + 1)) & CONTENTS_MASK_WATER))
						return false;
				}
			}

			// PMM - changed above to this
			if ((trace.Fraction == 1) && (!trace.AllSolid) && (!trace.StartSolid))
			{
				Entity->State.GetOrigin() = trace.EndPosition;

				if(!current_bad && CheckForBadArea())
					Entity->State.GetOrigin() = oldOrg;
				else
				{
					if (ReLink)
					{
						Entity->Link ();
						Entity->TouchTriggers ();
					}

					return true;
				}
			}
			
			if (!Entity->Enemy)
				break;
		}
		
		return false;
	}

// push down from a step height above the wished position
	int stepsize = (!(AIFlags & AI_NOSTEP)) ? STEPSIZE : 1;

//PGM
	// trace from 1 stepsize gravityUp to 2 stepsize gravityDown.
	newOrg = newOrg.MultiplyAngles (-1 * stepsize, Entity->GravityVector);
	vec3f end = newOrg.MultiplyAngles (2 * stepsize, Entity->GravityVector);

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
		vec3f test = trace.EndPosition;
		test.Z += (Entity->GravityVector.Z > 0) ? Entity->GetMaxs().Z - 1 : Entity->GetMins().Z + 1;

		if (PointContents(test) & CONTENTS_MASK_WATER)
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

	// PMM - don't bother with bad areas if we're dead
	if (Entity->Health > 0)
	{
		// use AI_BLOCKED to tell the calling layer that we're now mad at a tesla
		CBadArea *new_bad = CheckForBadArea();
		if(!current_bad && new_bad)
		{
			if (new_bad->Owner)
			{
				if (!strcmp(new_bad->Owner->ClassName.CString(), "tesla"))
				{
					if ((!(Entity->Enemy)) || (!(Entity->Enemy->GetInUse())))
					{
						TargetTesla (entity_cast<CTesla>(new_bad->Owner));
						AIFlags |= AI_BLOCKED;
					}
					else if (!strcmp(Entity->Enemy->ClassName.CString(), "telsa"))
					{
					}
					else if ((Entity->Enemy) && (Entity->Enemy->EntityFlags & EF_PLAYER))
					{
						if (!IsVisible(Entity, *Entity->Enemy))
						{
							TargetTesla (entity_cast<CTesla>(new_bad->Owner));
							AIFlags |= AI_BLOCKED;
						}
					}
					else
					{
						TargetTesla (entity_cast<CTesla>(new_bad->Owner));
						AIFlags |= AI_BLOCKED;
					}
				}
			}

			Entity->State.GetOrigin() = oldOrg;
			return false;
		}
	}

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

//ROGUE
	if ((Entity->GetInUse()) && (Entity->Health > 0))
	{
		if (Entity->Blocked (Dist))
			return;
	}
//ROGUE

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
	if (!Entity->GetInUse())
		return true;		// PGM g_touchtrigger free problem

	IdealYaw = Yaw;
	ChangeYaw ();
	
	Yaw = Yaw*M_PI*2 / 360;
	vec3f move (cosf(Yaw)*Dist, sinf(Yaw)*Dist, 0);
	
	vec3f oldorigin = Entity->State.GetOrigin();
	if (MoveStep (move, false))
	{
		AIFlags &= ~AI_BLOCKED;
		if (!Entity->GetInUse())
			return true;		// PGM g_touchtrigger free problem

		float delta = Entity->State.GetAngles().Y - IdealYaw;

		if (MonsterID == CWidowStand::ID || MonsterID == CBlackWidow::ID)
		{
			if (delta > 45 && delta < 315)
				// not turned far enough, so don't take the step
				Entity->State.GetOrigin() = oldorigin;
		}

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
	if (Entity->Enemy)
	{
		if ((Entity->Enemy->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(*Entity->Enemy)->Health <= 0)
			return false;

	// see if any entities are in the way of the shot
		vec3f spot1 = Entity->State.GetOrigin() + vec3f(0, 0, Entity->ViewHeight), spot2 = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, (Entity->Enemy->ViewHeight > Entity->Enemy->GetMaxs().Z) ? Entity->Enemy->GetMaxs().Z / 2 : Entity->Enemy->ViewHeight);

		CTrace tr (spot1, spot2, Entity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.Entity != Entity->Enemy)
		{	
			// PGM - we want them to go ahead and shoot at info_notnulls if they can.
			if (Entity->Enemy->GetSolid() != SOLID_NOT || tr.Fraction < 1.0)		//PGM
			{
				// PMM - if we can't see our target, and we're not blocked by a monster, go into blind fire if available
				if ((!(tr.Entity->EntityFlags & EF_MONSTER) && (!IsVisible(Entity, *Entity->Enemy))))
				{
					if (BlindFire && (BlindFireDelay <= 20.0))
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

	float chance;

	if (AIFlags & AI_STAND_GROUND)
		chance = 0.4f;
	else if (Entity->ClassName == "tesla")
		chance = 2.0f;
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

	// PGM - go ahead and shoot every time if it's a info_notnull
	if ((Entity->Enemy->GetSolid() == SOLID_NOT) || (frand() < chance))
	{
		AttackState = AS_MISSILE;
		AttackFinished = Level.Frame + 20*frand();
		return true;
	}

	// PMM -daedalus should strafe more .. this can be done here or in a customized
	// check_attack code for the hover.
	if (AIFlags & AI_FLY)
	{
		// originally, just 0.3
		float strafe_chance = (MonsterID == CDaedalus::ID) ? 0.8f : 0.6f;

		// if enemy is tesla, never strafe
		if ((Entity->Enemy) && (!Entity->Enemy->ClassName.IsNullOrEmpty()) && (!strcmp(Entity->Enemy->ClassName.CString(), "tesla")))
			strafe_chance = 0;

		if (frand() < strafe_chance)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}
	else
	{
		if (frand() < 0.4)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

	return false;
}

void IMonster::ReactToDamage (IBaseEntity *Attacker, IBaseEntity *Inflictor)
{
	// pmm
	if (!(Attacker->EntityFlags & EF_PLAYER) && !(Attacker->EntityFlags & EF_MONSTER))
		return;

	// logic for tesla - if you are hit by a tesla, and can't see who you should be mad at (attacker)
	// attack the tesla
	// also, target the tesla if it's a "new" tesla
	if ((Inflictor) && (!strcmp(Inflictor->ClassName.CString(), "tesla")))
	{
		CTesla *tesla = entity_cast<CTesla>(Inflictor);
		bool new_tesla = MarkTeslaArea(tesla);
		if (new_tesla)
			TargetTesla (tesla);
		return;
	}

	if (Attacker == Entity || Attacker == Entity->Enemy)
		return;

	// if we are a good guy monster and our attacker is a player
	// or another good guy, do not get mad at them
	if (AIFlags & AI_GOOD_GUY)
	{
		if ((Attacker->EntityFlags & EF_PLAYER) || (entity_cast<CMonsterEntity>(Attacker)->Monster->AIFlags & AI_GOOD_GUY))
			return;
	}

	// if we're currently mad at something a target_anger made us mad at, ignore
	// damage
	if (Entity->Enemy && (AIFlags & AI_TARGET_ANGER))
	{
		// make sure whatever we were pissed at is still around.
		if (Entity->Enemy->GetInUse())
		{			
			if (((float)(Entity->Health) / (float)(Entity->MaxHealth)) > 0.33)
				return;
		}

		// remove the target anger flag
		AIFlags &= ~AI_TARGET_ANGER;
	}

// if we're healing someone, do like above and try to stay with them
	if ((Entity->Enemy) && (AIFlags & AI_MEDIC))
	{
		// ignore it some of the time
		if (Entity->Enemy->GetInUse() && ((float)(Entity->Health) / (float)(Entity->MaxHealth)) > 0.25)
			return;

		// remove the medic flag
		AIFlags &= ~AI_MEDIC;
		entity_cast<CMonsterEntity>(*Entity->Enemy)->Monster->CleanupHealTarget ();
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
			if (IsVisible(Entity, *Entity->Enemy))
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

	// it's the same base (walk/swim/fly) type and a different classname and it's not a tank
	// (they spray too much), get mad at them
	// PMM
	// added medics to this 
	if ((Attacker->EntityFlags & EF_MONSTER) && 
		((AIFlags & (AI_FLY | AI_SWIM)) == (entity_cast<CMonsterEntity>(Attacker)->Monster->AIFlags & (AI_FLY | AI_SWIM))) &&
		(strcmp (Entity->ClassName.CString(), Attacker->ClassName.CString()) != 0) &&
		!(entity_cast<CMonsterEntity>(Attacker)->Monster->AIFlags & AI_IGNORE_SHOTS) &&
		!(AIFlags & AI_IGNORE_SHOTS) )
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
	float	ofs;

	// PMM - made AI_MANUAL_STEERING affect things differently here .. they turn, but
	// don't set the ideal_yaw
	// This is put in there so monsters won't move towards the origin after killing
	// a tesla. This could be problematic, so keep an eye on it.

	if(!Entity->Enemy || !Entity->Enemy->GetInUse())		//PGM
		return;									//PGM

	if (IsVisible(Entity, *Entity->Enemy))
		BlindFireTarget = Entity->Enemy->State.GetOrigin();

	if (!(AIFlags & AI_MANUAL_STEERING))
		IdealYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();

	ChangeYaw ();

	if (Dist)
	{
		if (AIFlags & AI_CHARGING)
		{
			MoveToGoal (Dist);
			return;
		}

		// circle strafe support
		if (AttackState == AS_SLIDING)
		{
			// if we're fighting a tesla, NEVER circle strafe
			if ((Entity->Enemy) && (!Entity->Enemy->ClassName.IsNullOrEmpty()) && (!strcmp(Entity->Enemy->ClassName.CString(), "tesla")))
				ofs = 0;
			else if (Lefty)
				ofs = 90;
			else
				ofs = -90;
			
			if (WalkMove (IdealYaw + ofs, Dist))
				return;
				
			Lefty = !Lefty;
			WalkMove (IdealYaw - ofs, Dist);
		}
		else
			WalkMove (Entity->State.GetAngles().Y, Dist);
	}
}

bool IMonster::AI_CheckAttack()
{
	bool	retval;

	if (!Entity->Enemy)
		return false;

// this causes monsters to run blindly to the combat point w/o firing
	if (Entity->GoalEntity)
	{
		if (AIFlags & AI_COMBAT_POINT)
			return false;

		if (AIFlags & AI_SOUND_TARGET)
		{
			if ((Entity->Enemy->EntityFlags & EF_NOISE) && (Level.Frame - entity_cast<CPlayerNoise>(*Entity->Enemy)->Time) > 50)
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
	IHurtableEntity *HurtableEnemy = (Entity->Enemy->EntityFlags & EF_HURTABLE) ? entity_cast<IHurtableEntity>(*Entity->Enemy) : NULL;

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
		AIFlags &= ~AI_MEDIC;
		Entity->Enemy = nullentity;

		if (Entity->OldEnemy && (Entity->OldEnemy->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(*Entity->OldEnemy)->Health > 0)
		{
			Entity->Enemy = Entity->OldEnemy;
			Entity->OldEnemy = nullentity;
			HuntTarget ();
		}

//ROGUE - multiple teslas make monsters lose track of the player.
		else if (LastPlayerEnemy && LastPlayerEnemy->Health > 0)
		{
			Entity->Enemy = LastPlayerEnemy;
			Entity->OldEnemy = nullentity;
			LastPlayerEnemy = NULL;
			HuntTarget ();
		}
//ROGUE
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
	EnemyVis = IsVisible (Entity, *Entity->Enemy);
	if (EnemyVis)
	{
		SearchTime = Level.Frame + 50;
		LastSighting = Entity->Enemy->State.GetOrigin();

		AIFlags &= ~AI_LOST_SIGHT;
		TrailTime = Level.Frame;
		BlindFireTarget = Entity->Enemy->State.GetOrigin();
		BlindFireDelay = 0;
	}

	EnemyInfront = IsInFront(Entity, *Entity->Enemy);
	EnemyRange = Range(Entity, *Entity->Enemy);
	EnemyYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();


	// JDC self->ideal_yaw = enemy_yaw;

	// PMM -- reordered so the monster specific checkattack is called before the run_missle/melee/checkvis
	// stuff .. this allows for, among other things, circle strafing and attacking while in ai_run
	retval = CheckAttack ();
	if (retval)
	{
		// PMM
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

		// PMM -- added so monsters can shoot blind
		if (AttackState == AS_BLIND)
		{
			AI_Run_Missile ();
			return true;
		}
		// pmm

		// if enemy is not currently visible, we will never attack
		if (!EnemyVis)
			return false;
		// PMM
	}
	return retval;
}

void IMonster::AI_Move (float Dist)
{
	WalkMove (Entity->State.GetAngles().Y, Dist);
}

void IMonster::AI_Run(float Dist)
{
	bool	alreadyMoved = false;
 
	// if we're going to a combat point, just proceed
	if (AIFlags & AI_COMBAT_POINT)
	{
		MoveToGoal (Dist);
		return;
	}

	// PMM
	if (AIFlags & AI_DUCKED)
		AIFlags &= ~AI_DUCKED;

	if (Entity->GetMaxs().Z != BaseHeight)
		DuckUp ();

	if ((AIFlags & AI_SOUND_TARGET) && (Entity->Enemy->EntityFlags & EF_NOISE))
	{
		if ((!Entity->Enemy) || ((Entity->State.GetOrigin() - Entity->Enemy->State.GetOrigin()).Length() < 64))
		// pmm
		{
			AIFlags |= (AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
			Stand ();
			Entity->Enemy = nullentity;
			return;
		}

		MoveToGoal (Dist);
		alreadyMoved = true;

		if (!Entity->GetInUse())
			return;			// PGM - g_touchtrigger free problem

		if (!FindTarget ())
			return;
	}

	// PMM -- moved ai_checkattack up here so the monsters can attack while strafing or charging
	// PMM -- if we're dodging, make sure to keep the attack_state AS_SLIDING

	bool retval = AI_CheckAttack ();

	// PMM - don't strafe if we can't see our enemy
	if ((!EnemyVis) && (AttackState == AS_SLIDING))
		AttackState = AS_STRAIGHT;
	// unless we're dodging (dodging out of view looks smart)
	if (AIFlags & AI_DODGING)
		AttackState = AS_SLIDING;

	if (AttackState == AS_SLIDING)
	{
		if (!alreadyMoved)
			AI_Run_Slide (Dist);

		// we're using attack_state as the return value out of ai_run_slide to indicate whether or not the
		// move succeeded.  If the move succeeded, and we're still sliding, we're done in here (since we've
		// had our chance to shoot in ai_checkattack, and have moved).
		// if the move failed, our state is as_straight, and it will be taken care of below
		if ((!retval) && (AttackState == AS_SLIDING))
			return;
	}
	else if (AIFlags & AI_CHARGING)
	{
		IdealYaw = EnemyYaw;
		if (!(AIFlags & AI_MANUAL_STEERING))
			ChangeYaw ();
	}
	if (retval)
	{
		// PMM - is this useful?  Monsters attacking usually call the ai_charge routine..
		// the only monster this affects should be the soldier
		if ((Dist != 0) && (!alreadyMoved) && (AttackState == AS_STRAIGHT) && (!(AIFlags & AI_STAND_GROUND)))
			MoveToGoal (Dist);

		if ((Entity->Enemy) && (Entity->Enemy->GetInUse()) && (EnemyVis))
		{
			AIFlags &= ~AI_LOST_SIGHT;
			LastSighting = Entity->Enemy->State.GetOrigin();
			TrailTime = Level.Frame;

			BlindFireTarget = Entity->Enemy->State.GetOrigin();
			BlindFireDelay = 0;
		}
		return;
	}
	//PMM

	// PGM - added a little paranoia checking here... 9/22/98
	if ((Entity->Enemy) && (Entity->Enemy->GetInUse()) && (EnemyVis))
	{
		// PMM - check for alreadyMoved
		if (!alreadyMoved)
			MoveToGoal (Dist);

		if(!Entity->GetInUse())
			return;			// PGM - g_touchtrigger free problem

		AIFlags &= ~AI_LOST_SIGHT;
		LastSighting = Entity->Enemy->State.GetOrigin();
		TrailTime = Level.Frame;
		
		// PMM
		BlindFireTarget = Entity->Enemy->State.GetOrigin();
		BlindFireDelay = 0;
		// pmm
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
		// PMM - double move protection
		if (!alreadyMoved)
			MoveToGoal (Dist);
		
		SearchTime = 0;
		return;
	}

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
		CPlayerMarker *marker;
		AIFlags &= ~AI_PURSUE_NEXT;

		// give ourself more time since we got this far
		SearchTime = Level.Frame + 50;

		if (AIFlags & AI_PURSUE_TEMP)
		{
			AIFlags &= ~AI_PURSUE_TEMP;
			marker = NULL;
			LastSighting = SavedGoal;
			isNew = true;
		}
		else if (AIFlags & AI_PURSUIT_LAST_SEEN)
		{
			AIFlags &= ~AI_PURSUIT_LAST_SEEN;
			marker = PlayerTrail_PickFirst (Entity);
		}
		else
			marker = PlayerTrail_PickNext (Entity);

		if (marker)
		{
			LastSighting = marker->Origin;
			TrailTime = marker->Timestamp;
			Entity->State.GetAngles().Y = IdealYaw = marker->Angles.Y;

			isNew = true;
		}
	}

	vec3f v = Entity->State.GetOrigin() - LastSighting;
	if (v.Length() <= Dist)
	{
		AIFlags |= AI_PURSUE_NEXT;
		Dist = v.Length();
	}

	vec3f lastGoalOrigin = LastSighting;

	if (isNew)
	{
		CTrace tr (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), LastSighting, Entity, CONTENTS_MASK_PLAYERSOLID);
		
		if (tr.Fraction < 1)
		{
			v = lastGoalOrigin - Entity->State.GetOrigin();
			float d1 = v.Length();
			float center = tr.Fraction;
			float d2 = d1 * ((center+1)/2);
			Entity->State.GetAngles().Y = IdealYaw = v.ToYaw();

			vec3f left_target, right_target;
			anglef angles = Entity->State.GetAngles().ToVectors();

			v.Set (d2, -16, 0);
			G_ProjectSource (Entity->State.GetOrigin(), v, angles, left_target);
			tr (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), left_target, Entity, CONTENTS_MASK_PLAYERSOLID);
			float left = tr.Fraction;

			v.Set (d2, 16, 0);
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
				lastGoalOrigin = LastSighting = left_target;
				v = lastGoalOrigin - Entity->State.GetOrigin();
				Entity->State.GetAngles().Y = IdealYaw = v.ToYaw();
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
				lastGoalOrigin = LastSighting = right_target;
				v = lastGoalOrigin - Entity->State.GetOrigin();
				Entity->State.GetAngles().Y = IdealYaw = v.ToYaw();
			}
		}
	}

	MoveToGoal (Dist);
	
	if (!Entity->GetInUse())
		return;			// PGM - g_touchtrigger free problem
}

void IMonster::AI_Run_Melee ()
{
	IdealYaw = EnemyYaw;
	if (!(AIFlags & AI_MANUAL_STEERING))
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
	if (!(AIFlags & AI_MANUAL_STEERING))
		ChangeYaw ();

	if (FacingIdeal())
	{
		Attack ();
		if ((AttackState == AS_MISSILE) || (AttackState == AS_BLIND))
			AttackState = AS_STRAIGHT;
	}
}

const float	MAX_SIDESTEP	= 8.0f;

void IMonster::AI_Run_Slide(float Dist)
{	
	IdealYaw = EnemyYaw;
	
	float ofs = (Lefty) ? 90 : -90;

	if (!(AIFlags & AI_MANUAL_STEERING))
		ChangeYaw ();

	// PMM - clamp maximum sideways move for non flyers to make them look less jerky
	if (!(AIFlags & AI_FLY))
		Dist = Min<> (Dist, MAX_SIDESTEP);

	if (WalkMove (IdealYaw + ofs, Dist))
		return;

	// PMM - if we're dodging, give up on it and go straight
	if (AIFlags & AI_DODGING)
	{
		DoneDodge ();
		// by setting as_straight, caller will know to try straight move
		AttackState = AS_STRAIGHT;
		return;
	}

	Lefty = !Lefty;
	if (WalkMove (IdealYaw - ofs, Dist))
		return;

	// PMM - if we're dodging, give up on it and go straight
	if (AIFlags & AI_DODGING)
		DoneDodge ();

	// PMM - the move failed, so signal the caller (ai_run) to try going straight
	AttackState = AS_STRAIGHT;
}

void IMonster::AI_Stand (float Dist)
{
	bool retval;

	if (Dist)
		WalkMove (Entity->State.GetAngles().Y, Dist);

	if (AIFlags & AI_STAND_GROUND)
	{
		if (Entity->Enemy)
		{
			vec3f v = Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin();
			IdealYaw = v.ToYaw();

			if (Entity->State.GetAngles().Y != IdealYaw && (AIFlags & AI_TEMP_STAND_GROUND))
			{
				AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
				Run ();
			}

			if (!(AIFlags & AI_MANUAL_STEERING))
				ChangeYaw ();
			
			// PMM
			// find out if we're going to be shooting
			retval = AI_CheckAttack ();

			// record sightings of player
			if ((Entity->Enemy) && (Entity->Enemy->GetInUse()) && (IsVisible(Entity, *Entity->Enemy)))
			{
				AIFlags &= ~AI_LOST_SIGHT;
				LastSighting = BlindFireTarget = Entity->Enemy->State.GetOrigin();
				TrailTime = Level.Frame;
				BlindFireDelay = 0;
			}
			// check retval to make sure we're not blindfiring
			else if (!retval)
			{
				FindTarget ();
				return;
			}
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
			IdleTime = Level.Frame + 150 + frand() * 150;
		}
		else
			IdleTime = Level.Frame + frand() * 150;
	}
}

void IMonster::FoundTarget ()
{
	// let other monsters see this monster for a while
	if (Entity->Enemy->EntityFlags & EF_PLAYER)
	{
		if (Entity->Enemy->Flags & FL_DISGUISED)
			Entity->Enemy->Flags &= ~FL_DISGUISED;
		
		Level.SightEntity = Entity;
		Level.SightEntityFrame = Level.Frame;
	}

	Entity->ShowHostile = Level.Frame + 10;		// wake up other monsters

	LastSighting = Entity->Enemy->State.GetOrigin();
	TrailTime = Level.Frame;

	BlindFireTarget = Entity->Enemy->State.GetOrigin();
	BlindFireDelay = 0;

	if (Entity->CombatTarget.IsNullOrEmpty())
	{
		HuntTarget ();
		return;
	}

	Entity->GoalEntity = Entity->MoveTarget = CC_PickTarget(Entity->CombatTarget);
	
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
	entity_cast<IMapEntity>(*Entity->MoveTarget)->ClassName.Clear();
	PauseTime = 0;

	// run for it
	Run ();
}

#endif
