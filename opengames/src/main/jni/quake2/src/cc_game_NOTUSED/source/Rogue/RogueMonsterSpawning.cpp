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
// cc_rogue_monster_spawning.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES
#include "Rogue/RogueMonsterSpawning.h"

//
//ROGUE
//
// this returns a randomly selected coop player who is visible to self
// returns NULL if bad
CPlayerEntity *PickCoopTarget (IBaseEntity *Entity)
{
	static std::vector<CPlayerEntity*> targets;

	// if we're not in coop, this is a noop
	if (!(Game.GameMode & GAME_COOPERATIVE))
		return NULL;

	targets.clear ();

	for (int player = 1; player <= Game.MaxClients; player++)
	{
		CPlayerEntity *ent = entity_cast<CPlayerEntity>(Game.Entities[player].Entity);
		if (!ent->GetInUse())
			continue;
		if (IsVisible(Entity, ent))
			targets.push_back (ent);
	}

	if (targets.empty())
		return NULL;

	// get a number from 0 to (num_targets-1)
	int targetID = irandom(targets.size());
	
	return targets[targetID];
}

// only meant to be used in coop
int CountPlayers ()
{
	int		count = 0;

	// if we're not in coop, this is a noop
	if (!(Game.GameMode & GAME_COOPERATIVE))
		return 1;

	for (int player = 1; player <= Game.MaxClients; player++)
	{
		CPlayerEntity *ent = entity_cast<CPlayerEntity>(Game.Entities[player].Entity);
		if (!ent->GetInUse())
			continue;
		count++;
	}

	return count;
}

//
// Monster spawning code
//
// Used by the carrier, the medic_commander, and the black widow
//
// The sequence to create a flying monster is:
//
//  FindSpawnPoint - tries to find suitable spot to spawn the monster in
//  CreateFlyMonster  - this verifies the point as good and creates the monster

// To create a ground walking monster:
//
//  FindSpawnPoint - same thing
//  CreateGroundMonster - this checks the volume and makes sure the floor under the volume is suitable
//

//
// CreateMonster
//
CMonsterEntity *CreateMonster(vec3f origin, vec3f angles, const char *classname)
{
CC_DISABLE_DEPRECATION
	SEntity *ent = G_Spawn ();

	Level.ClassName = classname;
	ent->Server.State.Origin = origin;
	ent->Server.State.Angles = angles;
	ED_CallSpawn (ent);

	CMonsterEntity *Mon = entity_cast<CMonsterEntity>(ent->Entity);
	Mon->Monster->AIFlags |= AI_DO_NOT_COUNT;
	Level.Monsters.Total--; // fix..
	Mon->State.GetRenderEffects() |= RF_IR_VISIBLE;
	Mon->GravityVector.Set (0, 0, -1);

	if (ent->Server.InUse && ent->Entity && !ent->Entity->Freed)
		return Mon;
	return NULL;
CC_ENABLE_DEPRECATION
}

// FindSpawnPoint
// PMM - this is used by the medic commander (possibly by the carrier) to find a good spawn point
// if the startpoint is bad, try above the startpoint for a bit
bool FindSpawnPoint (vec3f startpoint, vec3f mins, vec3f maxs, vec3f &spawnpoint, float maxMoveUp)
{
	CTrace tr (startpoint, mins, maxs, startpoint, NULL, CONTENTS_MASK_MONSTERSOLID|CONTENTS_PLAYERCLIP);

	if ((tr.StartSolid || tr.AllSolid) || (tr.Entity != World))
	{
		vec3f top = startpoint;
		top.Z += maxMoveUp;

		tr (top, mins, maxs, startpoint, NULL, CONTENTS_MASK_MONSTERSOLID);
		
		if (tr.StartSolid || tr.AllSolid)
			return false;
		else
		{
			spawnpoint = tr.EndPosition;
			return true;
		}
	}

	spawnpoint = startpoint;
	return true;
}

//
// CheckSpawnPoint
//
// PMM - checks volume to make sure we can spawn a monster there (is it solid?)
//
// This is all fliers should need

bool CheckSpawnPoint (vec3f origin, vec3f mins, vec3f maxs)
{
	if (mins.IsZero() && maxs.IsZero())
		return false;

	CTrace tr (origin, mins, maxs, origin, NULL, CONTENTS_MASK_MONSTERSOLID);

	return !((tr.StartSolid || tr.AllSolid) || (tr.Entity != World));
}

//
// CheckGroundSpawnPoint
//
// PMM - used for walking monsters
//  checks:
//		1)	is there a ground within the specified height of the origin?
//		2)	is the ground non-water?
//		3)	is the ground flat enough to walk on?
//

bool CheckGroundSpawnPoint (vec3f origin, vec3f entMins, vec3f entMaxs, float height, float gravity)
{
	if (!CheckSpawnPoint (origin, entMins, entMaxs))
		return false;

	vec3f stop = origin;
	stop.Z = origin.Z + entMins.Z - height;

	CTrace tr (origin, entMins, entMaxs, stop, NULL, CONTENTS_MASK_MONSTERSOLID | CONTENTS_MASK_WATER);
	// it's not going to be all solid or start solid, since that's checked above

	if ((tr.Fraction < 1) && (tr.Contents & CONTENTS_MASK_MONSTERSOLID))
	{
		// we found a non-water surface down there somewhere.  now we need to check to make sure it's not too sloped
		//
		// algorithm straight out of m_move.c:M_CheckBottom()
		//

		// first, do the midpoint trace

		vec3f	mins = tr.EndPosition + entMins,
				maxs = tr.EndPosition + entMaxs;

		// first, do the easy flat check
		//
		vec3f start;
		if (gravity > 0)
			start.Z = maxs.Z + 1;
		else
			start.Z = mins.Z - 1;

		bool doneCheck = false;
		for	(int x = 0; x <= 1 && !doneCheck; x++)
		{
			for	(int y = 0; y <= 1 && !doneCheck; y++)
			{
				start.X = x ? maxs.X : mins.X;
				start.Y = y ? maxs.Y : mins.Y;

				if (PointContents (start) != CONTENTS_SOLID)
					doneCheck = true;
			}
		}

		if (!doneCheck)
			return true; // passed above

		start.X = stop.X = (mins.X + maxs.X)*0.5;
		start.Y = stop.Y = (mins.Y + maxs.Y)*0.5;
		start.Z = mins.Z;

		tr (start, stop, NULL, CONTENTS_MASK_MONSTERSOLID);

		if (tr.Fraction == 1.0)
			return false;

		float mid = tr.EndPosition.Z, bottom = tr.EndPosition.Z;

		if (gravity < 0)
		{
			start.Z = mins.Z;
			stop.Z = start.Z - STEPSIZE - STEPSIZE;
			mid = bottom = tr.EndPosition.Z + entMins.Z;
		}
		else
		{
			start.Z = maxs.Z;
			stop.Z = start.Z + STEPSIZE + STEPSIZE;
			mid = bottom = tr.EndPosition.Z - entMaxs.Z;
		}

		for	(int x = 0; x <= 1; x++)
		{
			for	(int y = 0; y <= 1; y++)
			{
				start.X = stop.X = x ? maxs.X : mins.X;
				start.Y = stop.Y = y ? maxs.Y : mins.Y;
				
				tr (start, stop, NULL, CONTENTS_MASK_MONSTERSOLID);

				if (gravity > 0)
				{
					if (tr.Fraction != 1.0 && tr.EndPosition.Z < bottom)
						bottom = tr.EndPosition.Z;
					if (tr.Fraction == 1.0 || tr.EndPosition.Z - mid > STEPSIZE)
						return false;
				}
				else
				{
					if (tr.Fraction != 1.0 && tr.EndPosition.Z > bottom)
						bottom = tr.EndPosition.Z;
					if (tr.Fraction == 1.0 || mid - tr.EndPosition.Z > STEPSIZE)
						return false;
				}
			}
		}

		return true;		// we can land on it, it's ok
	}

	return false;
}

void DetermineBBox (const char *classname, vec3f &mins, vec3f &maxs)
{
	// FIXME - cache this stuff
	CMonsterEntity *Mon = CreateMonster(vec3fOrigin, vec3fOrigin, classname);
	
	mins = Mon->GetMins();
	maxs = Mon->GetMaxs();

	Mon->Free ();
	Mon->GetGameEntity()->RemovalFrames = 0;
}

CMonsterEntity *CreateFlyMonster (vec3f origin, vec3f angles, vec3f mins, vec3f maxs, char *classname)
{
	if (mins.IsZero() && maxs.IsZero())
		DetermineBBox (classname, mins, maxs);

	if (!CheckSpawnPoint(origin, mins, maxs))
		return NULL;

	return CreateMonster (origin, angles, classname);
}

// This is just a wrapper for CreateMonster that looks down height # of CMUs and sees if there
// are bad things down there or not
//
// this is from m_move.c

CMonsterEntity *CreateGroundMonster (vec3f origin, vec3f angles, vec3f entMins, vec3f entMaxs, const char *classname, int height)
{
	vec3f		mins, maxs;

	// if they don't provide us a bounding box, figure it out
	if (entMins.IsZero() && entMaxs.IsZero())
		DetermineBBox (classname, mins, maxs);
	else
	{
		mins = entMins;
		maxs = entMaxs;
	}

	// check the ground to make sure it's there, it's relatively flat, and it's not toxic
	if (!CheckGroundSpawnPoint(origin, mins, maxs, height, -1))
		return NULL;

	CMonsterEntity *newEnt = CreateMonster (origin, angles, classname);
	if (!newEnt)
		return NULL;

	return newEnt;
}

// ****************************
// SPAWNGROW stuff
// ****************************

const int SPAWNGROW_LIFESPAN		= 3;

CSpawnGrow::CSpawnGrow () :
	IThinkableEntity ()
	{
	};

CSpawnGrow::CSpawnGrow (sint32 Index) :
	IBaseEntity (Index),
	IThinkableEntity (Index)
	{
	};

void CSpawnGrow::SaveFields (CFile &File)
{
	IThinkableEntity::SaveFields (File);

	File.Write<FrameNumber> (Wait);
}

void CSpawnGrow::LoadFields (CFile &File)
{
	IThinkableEntity::LoadFields (File);

	Wait = File.Read<FrameNumber> ();
}

void CSpawnGrow::Think ()
{
	State.GetAngles().Set (frand()*360, frand()*360, frand()*360);

	if ((Level.Frame < Wait) && (State.GetFrame() < 2))
		State.GetFrame()++;

	if (Level.Frame >= Wait)
	{
		if (State.GetEffects() & FX_SPHERETRANS)
		{
			Free ();
			return;
		}
		else if (State.GetFrame() > 0)
			State.GetFrame()--;
		else
		{
			Free ();
			return;
		}
	}
	NextThink = Level.Frame + ServerFramesPerSecond;
};

void CSpawnGrow::Spawn (vec3f Origin, sint8 Size)
{
	CSpawnGrow *Grow = QNewEntityOf CSpawnGrow;
	Grow->State.GetOrigin() = Origin;
	Grow->State.GetAngles().Set (frand()*360, frand()*360, frand()*360);
	Grow->GetSolid() = SOLID_NOT;
	Grow->State.GetRenderEffects() = RF_IR_VISIBLE;
	Grow->ClassName = "spawngro";

	FrameNumber lifespan;
	if (Size <= 1)
	{
		lifespan = SPAWNGROW_LIFESPAN;
		Grow->State.GetModelIndex() = ModelIndex("models/items/spawngro2/tris.md2");
	}
	else if (Size == 2)
	{
		Grow->State.GetModelIndex() = ModelIndex("models/items/spawngro3/tris.md2");
		lifespan = 20;
	}
	else
	{
		Grow->State.GetModelIndex() = ModelIndex("models/items/spawngro/tris.md2");
		lifespan = SPAWNGROW_LIFESPAN;
	}

	Grow->Wait = Level.Frame + lifespan;
	Grow->NextThink = Level.Frame + ServerFramesPerSecond;
	if (Size != 2)
		Grow->State.GetEffects() |= FX_SPHERETRANS;
		
	Grow->Link ();
};

IMPLEMENT_SAVE_SOURCE (CSpawnGrow);

// predictive calculator
// target is who you want to shoot
// start is where the shot comes from
// bolt_speed is how fast the shot is
// eye_height is a boolean to say whether or not to adjust to targets eye_height
// offset is how much time to miss by
void PredictAim (IBaseEntity *target, vec3f start, float bolt_speed, bool eye_height, float offset, vec3f *aimdir, vec3f *aimpoint)
{
	if (!target || !target->GetInUse())
	{
		if (aimdir)
			*aimdir = vec3fOrigin;
		return;
	}

	vec3f dir = target->State.GetOrigin() - start;
	if (eye_height)
		dir[2] += target->ViewHeight;
	float dist = dir.Length();
	float time = dist / bolt_speed;

	vec3f vec = target->State.GetOrigin().MultiplyAngles (time - offset, entity_cast<IPhysicsEntity>(target)->Velocity);

	if (eye_height)
		vec[2] += target->ViewHeight;

	if (aimdir)
		*aimdir = (vec - start).GetNormalized();
	
	if (aimpoint)
		*aimpoint = vec;
}

#endif
