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
// cc_rogue_weaponry.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES

#include "Rogue/RogueWeaponry.h"
#include "Utility/TemporaryEntities.h"

bool CHeatBeam::DoDamage (IBaseEntity *Attacker, IHurtableEntity *Target, vec3f &Dir, vec3f &Point, vec3f &Normal)
{
	Target->TakeDamage (Attacker, Attacker, Dir, Point, Normal, (PointContents(Point) & CONTENTS_MASK_WATER) ? (Damage / 2) : Damage, Kick, DAMAGE_ENERGY, MeansOfDeath);
	return ThroughAndThrough;
}

void CHeatBeam::DoSolidHit	(CTrace *Trace)
{
	if (!(Trace->Surface->Flags & SURF_TEXINFO_SKY))
		CHeatSteam(Trace->EndPosition, Trace->Plane.Normal).Send();
}

void CHeatBeam::DoEffect	(vec3f &start, vec3f &end, bool water)
{
	if (water)
		CBubbleTrail(start, end).Send();

	CHeatBeamLaser(start, end, Firer->State.GetNumber(), MonsterBeam).Send();
}

void CHeatBeam::DoWaterHit	(CTrace *Trace)
{
	CSparks(Trace->EndPosition, Trace->Plane.Normal, ST_HEATBEAM_SPARKS).Send();
}

void CHeatBeam::DoFire(IBaseEntity *Entity, vec3f start, vec3f aimdir)
{
	vec3f end, from;
	vec3f lastWaterStart, lastWaterEnd;
	
	vec3f lastDrawFrom;
	bool DrawIsWater = false;

	// Calculate end
	if (!ModifyEnd(aimdir, start, end))
		end = start.MultiplyAngles (8192, aimdir);

	from = start;
	lastDrawFrom = from;

	sint32 Mask = CONTENTS_MASK_SHOT|CONTENTS_MASK_WATER;
	bool Water = false;
	IBaseEntity *Ignore = Entity;

	lastWaterStart = start;

	bool hitOutOfWater = false;
	if (PointContents(start) & CONTENTS_MASK_WATER)
	{
		// Copy up our point for the effect
		lastWaterEnd = start;
	
		// Special case if we started in water
		Water = true;
		DrawIsWater = true;
		Mask = CONTENTS_MASK_SHOT;

		// Find the exit point
		sint32 tries = 20; // Cover about 2000 units
		vec3f	stWater;

		stWater = from;
		lastWaterStart.Clear();
		
		while (tries > 0)
		{
			stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

			EBrushContents contents = PointContents(stWater);
			if (contents == 0) // "Clear" or solid
				break; // Got it
			else if (contents & CONTENTS_MASK_SOLID)
			{
				// This is a special case in case we run into a solid.
				// This basically means that the trace is done, so we can skip ahead right to the solid (act like
				// we're not in water)
				tries = 0;
				break;
			}
			tries --;
		}

		if (tries != 0)
		{
			// We reached air
			// Trace backwards and grab the water brush
			vec3f tempOrigin;
			tempOrigin = stWater.MultiplyAngles (-(HITSCANSTEP + 5), aimdir);
			CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

			if (tempTrace.Contents & CONTENTS_MASK_WATER)// All is good
			{
				// This is our end
				lastWaterStart = tempTrace.EndPosition;
				hitOutOfWater = true;
			}
		}
		// We didn't reach air if we got here.
		// Let water handle it, it will act as solid.
	}

	// Main loop
	while (1)
	{
		// Trace from start to our end
		CTrace Trace = DoTrace (from, end, Ignore, Mask);

		// Did we hit an entity?
		if (Trace.Entity && ((Trace.Entity->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(Trace.Entity)->CanTakeDamage))
		{
			// Convert to base entity
			IHurtableEntity *Target = entity_cast<IHurtableEntity>(Trace.Entity);

			// Hurt it
			// Revision
			// Startsolid mistake..
			if (Trace.StartSolid)
			{
				vec3f origin = Target->State.GetOrigin();
				if (!DoDamage (Entity, Target, aimdir, origin, Trace.Plane.Normal))
				{
					DoEffect (origin, lastDrawFrom, DrawIsWater);
					break; // We wanted to stop
				}

				// Set up the start from where we are now
				vec3f oldFrom = from;
				from = origin;

				// Revision: Stop on solids
				if (ThroughAndThrough && Target->GetSolid() == SOLID_BSP)
				{
					// Draw the effect
					DoEffect (from, oldFrom, false);

					DoSolidHit (&Trace);
					break;
				}

				DoEffect (from, oldFrom, Water);

				// and ignore the bastard
				Ignore = Target;

				// Continue our loop
				continue;
			}

			if (!DoDamage (Entity, Target, aimdir, Trace.EndPosition, Trace.Plane.Normal))
			{
				DoEffect (from, Trace.EndPosition, DrawIsWater);
				break; // We wanted to stop
			}

			// Set up the start from where we are now
			vec3f oldFrom = from;
			from = Trace.EndPosition;

			// Revision: Stop on solids
			if (ThroughAndThrough && Target->GetSolid() == SOLID_BSP)
			{
				// Draw the effect
				DoEffect (from, oldFrom, false);

				DoEffect (from, oldFrom, DrawIsWater);
				DoSolidHit (&Trace);
				break;
			}

			DoEffect (from, oldFrom, Water);

			// and ignore the bastard
			Ignore = Target;

			// Continue our loop
			continue;
		}
		// If we hit something in water...
		else if (Water)
		{
			Water = false;
			// Assume solid
			//if (Trace.Contents & CONTENTS_MASK_SOLID)
			{
				// If we didn't grab water last time...
				if (lastWaterStart == vec3fOrigin)
				{
					// We hit the ground!
					// Swap start and end points
					lastWaterStart = lastWaterEnd;
					lastWaterStart = lastWaterStart.MultiplyAngles (5, aimdir);

					// Set end point
					lastWaterEnd = Trace.EndPosition;

					// Draw the effect
					DoEffect (lastWaterStart, lastWaterEnd, true);

					DoSolidHit (&Trace);

					break; // We're done
				}
				// Otherwise we had found an exit point
				else
				{
					// Draw from water surface to water end
					DoEffect (lastWaterEnd, lastWaterStart, true);
	
					// We hit the ground!
					// Swap start and end points
					if (!hitOutOfWater)
						lastWaterStart = lastWaterEnd;
					else
						hitOutOfWater = false;

					// Set end point
					lastWaterEnd = Trace.EndPosition;

					// Draw the effect
					DoEffect (lastWaterStart, lastWaterEnd, false);

					DoSolidHit (&Trace);

					break; // We're done
				}
			}
			continue;
		}
		// If we hit non-transparent water
		else if ((Trace.Contents & CONTENTS_MASK_WATER) &&
			(Trace.Surface && !(Trace.Surface->Flags & (SURF_TEXINFO_TRANS33|SURF_TEXINFO_TRANS66))))
		{
			// Copy up our point for the effect
			lastWaterEnd = Trace.EndPosition;

			// Tell the system we're in water
			Water = true;

			// Draw the effect
			DoEffect (lastWaterStart, lastWaterEnd, false);

			DoWaterHit (&Trace);

			// Set up the start from where we are now
			from = lastDrawFrom = Trace.EndPosition;
			Mask = CONTENTS_MASK_SHOT;

			// Find the exit point
			sint32 tries = 20; // Cover about 2000 units
			vec3f	stWater = from;
			lastWaterStart.Clear();
			
			while (tries > 0)
			{
				stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

				EBrushContents contents = PointContents(stWater);
				if (contents == 0) // "Clear" or solid
					break; // Got it
				else if (contents & CONTENTS_MASK_SOLID)
				{
					// This is a special case in case we run into a solid.
					// This basically means that the trace is done, so we can skip ahead right to the solid (act like
					// we're not in water)
					tries = 0;
					break;
				}
				tries --;
			}

			if (tries != 0)
			{
				// We reached air
				// Trace backwards and grab the water brush
				vec3f tempOrigin = stWater.MultiplyAngles (-(HITSCANSTEP + 5), aimdir);
				CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

				if (tempTrace.Contents & CONTENTS_MASK_WATER) // All is good
				{
					// This is our end
					lastWaterStart = tempTrace.EndPosition;
					continue; // Head to the next area
				}
			}
			// We didn't reach air if we got here.
			// Let water handle it, it will act as solid.

			// Continue the loop
			continue;
		}
		// Transparent water
		else if ((Trace.Contents & CONTENTS_MASK_WATER) &&
			(Trace.Surface && (Trace.Surface->Flags & (SURF_TEXINFO_TRANS33|SURF_TEXINFO_TRANS66))))
		{
			// This won't count as "water" since we can see through it.
			// It has the same PVS, meaning we don't need to
			// do complex tracing.

			if (ThroughAndThrough)
			{
				// Keep going
				from = Trace.EndPosition.MultiplyAngles (0.1f, aimdir);

				// Water hit effect
				DoWaterHit (&Trace);
			}
			else
			{
				// Copy up our point for the effect
				lastWaterEnd = Trace.EndPosition;

				// Draw the effect we have so far
				DoEffect (from, Trace.EndPosition, false);

				// Water hit effect
				DoWaterHit (&Trace);

				// Set up water drawing
				Water = true;
				Mask = CONTENTS_MASK_SHOT;

				// Find the exit point
				sint32 tries = 20; // Cover about 2000 units
				vec3f	stWater = from;
				lastWaterStart.Clear();
				
				while (tries > 0)
				{
					stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

					EBrushContents contents = PointContents(stWater);
					if (contents == 0) // "Clear" or solid
						break; // Got it
					else if (contents & CONTENTS_MASK_SOLID)
					{
						// This is a special case in case we run into a solid.
						// This basically means that the trace is done, so we can skip ahead right to the solid (act like
						// we're not in water)
						tries = 0;
						break;
					}
					tries --;
				}

				if (tries != 0)
				{
					// We reached air
					// Trace backwards and grab the water brush
					vec3f tempOrigin = stWater.MultiplyAngles (-(HITSCANSTEP + 5), aimdir);
					CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

					if (tempTrace.Contents & CONTENTS_MASK_WATER) // All is good
					{
						// This is our end
						lastWaterStart = tempTrace.EndPosition;
						continue; // Head to the next area
					}
				}
				// We didn't reach air if we got here.
				// Let water handle it, it will act as solid.

				// Continue the loop
			}

			continue;
		}
		// Assume solid
		else
		{
			// Draw the effect
			DoEffect (lastDrawFrom, Trace.EndPosition, DrawIsWater);
			DoSolidHit (&Trace);
			break; // We're done
		}
	}
}

// An overload to handle transparent water
void CHeatBeam::Fire		(IBaseEntity *Entity, vec3f start, vec3f aimdir, sint32 Damage, sint32 kick, sint32 mod, bool Monster)
{
	CHeatBeam(Entity, Damage, kick, mod, Monster).DoFire (Entity, start, aimdir);
}

/*
================
CFlechette
================
*/

CFlechette::CFlechette () :
  IFlyMissileProjectile(),
  ITouchableEntity(),
  IThinkableEntity()
{
};

CFlechette::CFlechette (sint32 Index) :
  IBaseEntity (Index),
  IFlyMissileProjectile(Index),
  ITouchableEntity(Index),
  IThinkableEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CFlechette)

void CFlechette::Think ()
{
	Free();
}

void CFlechette::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
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
		entity_cast<IHurtableEntity>(Other)->TakeDamage (this, GetOwner(), Velocity, State.GetOrigin (), plane ? plane->Normal : vec3fOrigin, Damage, Kick, DAMAGE_NO_REG_ARMOR, MOD_ETF_RIFLE);
	else
		CBlasterSplash(State.GetOrigin(), plane ? plane->Normal : vec3fOrigin, BL_FLECHETTE).Send();

	Free (); // "delete" the entity
}

void CFlechette::Spawn	(IBaseEntity *Spawner, vec3f Start, vec3f Dir,
						sint32 Damage, sint32 Kick, sint32 Speed)
{
	CFlechette		*Bolt = QNewEntityOf CFlechette;

	Bolt->GetSvFlags() |= (SVF_DEADMONSTER | SVF_PROJECTILE);
	Bolt->State.GetOrigin() = Start;
	Bolt->State.GetOldOrigin() = Start;
	Bolt->State.GetAngles() = Dir.ToAngles();
	Bolt->Velocity = Dir.GetNormalizedFast() * Speed;

	Bolt->State.GetModelIndex() = ModelIndex ("models/proj/flechette/tris.md2");

	Bolt->SetOwner(Spawner);
	Bolt->NextThink = Level.Frame + 20;
	Bolt->Damage = Damage;
	Bolt->Kick = Kick;
	Bolt->GetClipmask() = CONTENTS_MASK_SHOT;
	Bolt->GetSolid() = SOLID_BBOX;
	Bolt->GetMins().Clear ();
	Bolt->GetMaxs().Clear ();
	Bolt->Touchable = true;
	Bolt->Link ();

	CTrace tr ((Spawner) ? Spawner->State.GetOrigin() : Start, Start, Bolt, CONTENTS_MASK_SHOT);
	if (tr.Fraction < 1.0)
	{
		Start = Start.MultiplyAngles (-10, Dir.GetNormalizedFast());
		Bolt->State.GetOrigin() = Start;
		Bolt->State.GetOldOrigin() = Start;

		if (tr.Entity)
			Bolt->Touch (tr.Entity, &tr.Plane, tr.Surface);
	}
	else if (Spawner && (Spawner->EntityFlags & EF_PLAYER))
		CheckDodge (Spawner, Start, Dir, Speed);
}

bool CFlechette::Run ()
{
	return IFlyMissileProjectile::Run();
}

/*
================
CDisruptorTracker
================
*/

const EDamageFlags TRACKER_DAMAGE_FLAGS	= (DAMAGE_NO_POWER_ARMOR | DAMAGE_ENERGY | DAMAGE_NO_KNOCKBACK);
const EDamageFlags TRACKER_IMPACT_FLAGS	= (DAMAGE_NO_POWER_ARMOR | DAMAGE_ENERGY);

const int TRACKER_DAMAGE_TIME		= 5;

class CDisruptorPainDaemon : public IThinkableEntity
{
public:
	sint32			Damage;
	FrameNumber		LifeTime;

	CDisruptorPainDaemon() :
	  IThinkableEntity ()
	  {
	  };

	CDisruptorPainDaemon(sint32 Index) :
	  IBaseEntity (Index),
	  IThinkableEntity (Index)
	  {
	  };

	void SaveFields (CFile &File)
	{
		File.Write<sint32> (Damage);
		File.Write<FrameNumber> (LifeTime);

		IThinkableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Damage = File.Read<sint32>();
		LifeTime = File.Read<FrameNumber>();

		IThinkableEntity::LoadFields (File);
	}

	IMPLEMENT_SAVE_HEADER (CDisruptorPainDaemon);

	void Think ()
	{
		static vec3f	PainNormal (0, 0, 1);

		if (!GetInUse())
			return;

		if ((Level.Frame - LifeTime) > TRACKER_DAMAGE_TIME)
		{
			if (!(Enemy->EntityFlags & EF_PLAYER))
				Enemy->State.GetEffects() &= ~FX_TRACKERTRAIL;
			Free ();
		}
		else
		{
			IHurtableEntity *Hurtable = entity_cast<IHurtableEntity>(*Enemy);
			if (!Hurtable) // our enemy is dead
			{
			    Free();
			    return;
			}
			if (Hurtable->Health > 0)
			{
				Hurtable->TakeDamage (this, GetOwner(), vec3fOrigin, Hurtable->State.GetOrigin(), PainNormal,
							Damage, 0, TRACKER_DAMAGE_FLAGS, MOD_TRACKER);
				
				// if we kill the player, we'll be removed.
				if (GetInUse())
				{
					// if we killed a monster, gib them.
					if (Hurtable->Health < 1)
					{
						int Take = (Hurtable->GibHealth) ? -Hurtable->GibHealth : 500;

						Hurtable->TakeDamage (this, GetOwner(), vec3fOrigin, Hurtable->State.GetOrigin(),
									PainNormal, Take, 0, TRACKER_DAMAGE_FLAGS, MOD_TRACKER);
					}

					if (Hurtable->EntityFlags & EF_PLAYER)
						entity_cast<CPlayerEntity>(Hurtable)->Client.Timers.Tracker = Level.Frame + 1;
					else
						Hurtable->State.GetEffects() |= FX_TRACKERTRAIL;
					
					NextThink = Level.Frame + ServerFramesPerSecond;
				}
			}
			else
			{
				if (!(Hurtable->EntityFlags & EF_PLAYER))
					Hurtable->State.GetEffects() &= ~FX_TRACKERTRAIL;

				Free ();
			}
		}
	}

	static void Spawn (IBaseEntity *Owner, IBaseEntity *Enemy, sint32 Damage)
	{
		if (!Enemy)
			return;

		CDisruptorPainDaemon	 *daemon = QNewEntityOf CDisruptorPainDaemon;

		daemon->ClassName = "pain daemon";
		daemon->NextThink = Level.Frame + ServerFramesPerSecond;
		daemon->LifeTime = Level.Frame;
		daemon->SetOwner(Owner);
		daemon->Enemy = Enemy;
		daemon->Damage = Damage;

		daemon->Link ();
	}
};

IMPLEMENT_SAVE_SOURCE (CDisruptorPainDaemon);

CDisruptorTracker::CDisruptorTracker () :
  IFlyMissileProjectile(),
  ITouchableEntity(),
  IThinkableEntity()
{
};

CDisruptorTracker::CDisruptorTracker (sint32 Index) :
  IBaseEntity (Index),
  IFlyMissileProjectile(Index),
  ITouchableEntity(Index),
  IThinkableEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CDisruptorTracker)

void CDisruptorTracker::Think ()
{
	if (DoFree)
	{
		Free ();
		return;
	}

	if ((!Enemy) || (!Enemy->GetInUse()) || (entity_cast<IHurtableEntity>(*Enemy)->Health < 1))
	{
		Explode (NULL);
		return;
	}

	vec3f dest = Enemy->State.GetOrigin();

	// PMM - try to hunt for center of enemy, if possible and not client
	if (Enemy->EntityFlags & EF_PLAYER)
		dest.Z += Enemy->ViewHeight;
	// paranoia
	else if (!(Enemy->GetAbsMin().IsZero() || Enemy->GetAbsMax().IsZero()))
		dest = vec3fOrigin.MultiplyAngles (0.5f, Enemy->GetAbsMin()).MultiplyAngles (0.5f, Enemy->GetAbsMax());

	vec3f dir = (dest - State.GetOrigin()).GetNormalized();
	State.GetAngles() = dir.ToAngles();
	Velocity = dir * Speed;

	NextThink = Level.Frame + ServerFramesPerSecond;
}

void CDisruptorTracker::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	if (Other == GetOwner())
		return;

	if (surf && (surf->Flags & SURF_TEXINFO_SKY))
	{
		Free ();
		return;
	}

	if (GetOwner()->EntityFlags & EF_PLAYER)
		entity_cast<CPlayerEntity>(GetOwner())->PlayerNoiseAt (State.GetOrigin(), PNOISE_IMPACT);

	if (Other && (Other->EntityFlags & EF_HURTABLE))
	{
		IHurtableEntity *Hurtable = entity_cast<IHurtableEntity>(Other);

		if (Hurtable->EntityFlags & (EF_MONSTER | EF_PLAYER))
		{
			if (Hurtable->Health > 0)
			{
				Hurtable->TakeDamage (this, GetOwner(), Velocity, State.GetOrigin(), (plane) ? plane->Normal : vec3fOrigin,
							0, (Damage*3), TRACKER_IMPACT_FLAGS, MOD_TRACKER);
				
				if ((Hurtable->EntityFlags & EF_MONSTER) && !(entity_cast<CMonsterEntity>(Hurtable)->Monster->AIFlags & (AI_FLY | AI_SWIM)))
					entity_cast<IPhysicsEntity>(Hurtable)->Velocity.Z += 140;
				
				CDisruptorPainDaemon::Spawn (GetOwner(), Other, (Damage * 0.1f) * 2);
			}
			else
				Hurtable->TakeDamage (this, GetOwner(), Velocity, State.GetOrigin(), (plane) ? plane->Normal : vec3fOrigin,
							(Damage * 4), (Damage * 3), TRACKER_IMPACT_FLAGS, MOD_TRACKER);
		}
		else
			Hurtable->TakeDamage (this, GetOwner(), Velocity, State.GetOrigin(), (plane) ? plane->Normal : vec3fOrigin,
						Damage, (Damage * 3), TRACKER_IMPACT_FLAGS, MOD_TRACKER);
	}

	Explode (plane);
}

void CDisruptorTracker::Explode (SBSPPlane *plane)
{
	CTrackerExplosion(State.GetOrigin()).Send();
	Free ();
}

void CDisruptorTracker::Spawn (IBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed, IBaseEntity *enemy)
{
	CDisruptorTracker	*Bolt = QNewEntityOf CDisruptorTracker;

	dir.Normalize();

	Bolt->State.GetOrigin() = Bolt->State.GetOldOrigin() = start;
	Bolt->State.GetAngles() = dir.ToAngles();
	Bolt->Velocity = dir * speed;
	Bolt->GetClipmask() = CONTENTS_MASK_SHOT;
	Bolt->GetSolid() = SOLID_BBOX;
	Bolt->Speed = speed;
	Bolt->State.GetEffects() = FX_TRACKER;
	Bolt->State.GetSound() = SoundIndex ("weapons/disrupt.wav");
	Bolt->GetMins() = Bolt->GetMaxs() = vec3fOrigin;
	
	Bolt->State.GetModelIndex() = ModelIndex ("models/proj/disintegrator/tris.md2");
	Bolt->Touchable = true;
	Bolt->Enemy = enemy;
	Bolt->SetOwner(Spawner);
	Bolt->Damage = Damage;
	Bolt->ClassName = "tracker";
	Bolt->Link();

	if (enemy)
	{
		Bolt->NextThink = Level.Frame + ServerFramesPerSecond;
		Bolt->DoFree = false;
	}
	else
	{
		Bolt->NextThink = Level.Frame + 100;
		Bolt->DoFree = true;
	}

	if (Spawner->EntityFlags & EF_PLAYER)
		CheckDodge (Spawner, Bolt->State.GetOrigin(), dir, speed);

	CTrace tr (Spawner->State.GetOrigin(), Bolt->State.GetOrigin(), Bolt, CONTENTS_MASK_SHOT);
	if (tr.Fraction < 1.0)
	{
		Bolt->State.GetOrigin() = Bolt->State.GetOrigin().MultiplyAngles (-10.0f, dir);
		Bolt->Touch (tr.Entity, NULL, NULL);
	}
}

bool CDisruptorTracker::Run ()
{
	return IFlyMissileProjectile::Run();
}

enum
{
	HYPER_FLAG		= BIT(0)
};

CGreenBlasterProjectile::CGreenBlasterProjectile () :
  IFlyMissileProjectile(),
  ITouchableEntity(),
  IThinkableEntity()
{
};

CGreenBlasterProjectile::CGreenBlasterProjectile (sint32 Index) :
  IBaseEntity (Index),
  IFlyMissileProjectile(Index),
  ITouchableEntity(Index),
  IThinkableEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CGreenBlasterProjectile)

void CGreenBlasterProjectile::Think ()
{
	Free();
}

void CGreenBlasterProjectile::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	if (Other == GetOwner())
		return;

	if (surf && (surf->Flags & SURF_TEXINFO_SKY))
	{
		Free ();
		return;
	}

	if (GetOwner() && (GetOwner()->EntityFlags & EF_PLAYER))
		entity_cast<CPlayerEntity>(GetOwner())->PlayerNoiseAt (State.GetOrigin(), PNOISE_IMPACT);

	if (Other->EntityFlags & EF_HURTABLE)
	{
		IHurtableEntity *Hurtable = entity_cast<IHurtableEntity>(Other);

		// the only time players will be firing blaster2 bolts will be from the 
		// defender sphere.
		EMeansOfDeath mod = (GetOwner() && (GetOwner()->EntityFlags & EF_PLAYER)) ? MOD_DEFENDER_SPHERE : MOD_BLASTER2;

		IHurtableEntity *Owner = (GetOwner() && (GetOwner()->EntityFlags & EF_HURTABLE)) ? entity_cast<IHurtableEntity>(GetOwner()) : NULL;
		bool WasDamageable = false;

		if (Owner)
		{
			WasDamageable = Owner->CanTakeDamage;
			Owner->CanTakeDamage = false;
		}

		if (Damage >= 5)
			SplashDamage (GetOwner(), Damage * 3, Other, 128, 0);
		Hurtable->TakeDamage (this, GetOwner(), Velocity, State.GetOrigin(), plane->Normal, Damage, 1, DAMAGE_ENERGY, mod);

		if (Owner)
			Owner->CanTakeDamage = WasDamageable;
	}
	else
	{
		//PMM - yeowch this will get expensive
		if (Damage >= 5)
			SplashDamage (GetOwner(), Damage * 3, GetOwner(), 128, 0);

		CBlasterSplash(State.GetOrigin(), (plane) ? plane->Normal : vec3fOrigin, BL_GREEN_BLASTER).Send();
	}

	Free ();
}

void CGreenBlasterProjectile::Spawn (IBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed, sint32 effect)
{
	CGreenBlasterProjectile		*Bolt = QNewEntityOf CGreenBlasterProjectile;

	Bolt->GetSvFlags() |= (SVF_DEADMONSTER | SVF_PROJECTILE);
	Bolt->State.GetOrigin() = start;
	Bolt->State.GetOldOrigin() = start;
	Bolt->State.GetAngles() = dir.ToAngles();
	Bolt->Velocity = dir.GetNormalizedFast() * speed;

	Bolt->State.GetEffects() = effect | ((effect) ? FX_TRACKER : 0);
	Bolt->State.GetModelIndex() = ModelIndex ("models/proj/laser2/tris.md2");

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

bool CGreenBlasterProjectile::Run ()
{
	return IFlyMissileProjectile::Run();
}

#endif
