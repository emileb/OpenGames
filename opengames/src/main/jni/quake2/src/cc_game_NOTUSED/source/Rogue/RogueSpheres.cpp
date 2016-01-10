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
// cc_rogue_spheres.cpp
// 
//

#include "Local.h"
#include "Items/Items.h"

#if ROGUE_FEATURES

#include "Rogue/RogueWeaponry.h"

const int DEFENDER_LIFESPAN		= 300;
const int HUNTER_LIFESPAN		= 300;
const int VENGEANCE_LIFESPAN	= 300;
const int MINIMUM_FLY_TIME		= 150;

void CRogueBaseSphere::SaveFields (CFile &File)
{
	IFlyMissileProjectile::SaveFields (File);
	IHurtableEntity::SaveFields (File);
	ITouchableEntity::SaveFields (File);
	IThinkableEntity::SaveFields (File);

	File.Write<FrameNumber> (Wait);
	File.Write<ESphereType> (SphereType);
	File.Write<ESphereFlags> (Flags);
	File.Write<sint32> ((OwnedPlayer) ? OwnedPlayer->State.GetNumber() : -1);
	File.Write<sint32> ((SphereEnemy) ? SphereEnemy->State.GetNumber() : -1);
	File.Write<vec3f> (SavedGoal);

	File.Write<sint32> ((Item) ? (Item->GetIndex()) : -1);
}

void CRogueBaseSphere::LoadFields (CFile &File)
{
	IFlyMissileProjectile::LoadFields (File);
	IHurtableEntity::LoadFields (File);
	ITouchableEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);

	Wait = File.Read<FrameNumber> ();
	SphereType = File.Read<ESphereType> ();
	Flags = File.Read<ESphereFlags> ();

	sint32 Index = File.Read<sint32> ();
	OwnedPlayer = (Index == -1) ? NULL : entity_cast<CPlayerEntity>(Game.Entities[Index].Entity);

	Index = File.Read<sint32> ();
	SphereEnemy = (Index == -1) ? NULL : entity_cast<IHurtableEntity>(Game.Entities[Index].Entity);

	SavedGoal = File.Read<vec3f> ();

	Index = File.Read<sint32> ();
	Item = (Index == -1) ? NULL : GetItemByIndex(Index);
}

void CRogueBaseSphere::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	if ((SphereType == SPHERE_DEFENDER) || !SphereEnemy)
		Explode ();
}

void CRogueBaseSphere::Explode ()
{
	if (OwnedPlayer && !(SphereFlags & SPHERE_DOPPLEGANGER))
		OwnedPlayer->Client.OwnedSphere = NULL;

	BecomeExplosion (false);
}

void CRogueBaseSphere::Fly ()
{
	if (Level.Frame >= Wait)
	{
		Explode ();
		return;
	}

	vec3f dest = GetOwner()->State.GetOrigin();
	dest.Z = GetOwner()->GetAbsMax().Z + 4;

	if (Level.Frame % 10)
	{
		if (!IsVisible (this, GetOwner()))
		{
			State.GetOrigin() = dest;
			Link ();
			return;
		}
	}

	Velocity = (dest - State.GetOrigin()) * 5;
}

void CRogueBaseSphere::Chase (bool stupidChase)
{
	if (Level.Frame >= Wait || (SphereEnemy && SphereEnemy->Health < 1))
	{
		Explode ();
		return;
	}

	vec3f dest = SphereEnemy->State.GetOrigin();
	if (SphereEnemy->EntityFlags & EF_PLAYER)
		dest.Z += SphereEnemy->ViewHeight;

	if (IsVisible (this, SphereEnemy) || stupidChase)
	{
		// if moving, hunter sphere uses active sound
		if (!stupidChase)
			State.GetSound() = SoundIndex ("spheres/h_active.wav");

		vec3f dir = (dest - State.GetOrigin()).GetNormalized();
		State.GetAngles() = dir.ToAngles();
		Velocity = dir * 500;
		SavedGoal = dest;
	}
	else if (SavedGoal.IsZero())
	{
		vec3f dir = (SphereEnemy->State.GetOrigin() - State.GetOrigin()).GetNormalized();
		State.GetAngles() = dir.ToAngles();

		// if lurking, hunter sphere uses lurking sound
		State.GetSound() = SoundIndex ("spheres/h_lurk.wav");
		Velocity.Clear();
	}
	else
	{
		vec3f dir = SavedGoal - State.GetOrigin();
		float dist = dir.Normalize();

		if (dist > 1)
		{
			State.GetAngles() = dir.ToAngles();

			if(dist > 500)			
				Velocity = dir * 500;
			else if (dist < 20)
				Velocity = dir * (dist / 0.1f);
			else
				Velocity = dir * dist;

			// if moving, hunter sphere uses active sound
			if (!stupidChase)
				State.GetSound() = SoundIndex ("spheres/h_active.wav");
		}
		else
		{
			dir = SphereEnemy->State.GetOrigin() - State.GetOrigin();
			dist = dir.Normalize();
			State.GetAngles() = dir.ToAngles();

			// if not moving, hunter sphere uses lurk sound
			if (!stupidChase)
				State.GetSound() = SoundIndex ("spheres/h_lurk.wav");

			Velocity.Clear();
		}
	}
}

void CRogueBaseSphere::BaseTouch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf, EMeansOfDeath Mod)
{
	if (SphereFlags & SPHERE_DOPPLEGANGER)
	{
		if (Other == Team.Master)
			return;

		CanTakeDamage = false;
		SetOwner(Team.Master);
		Team.Master = NULL;
	}
	else
	{
		if (Other == GetOwner())
			return;

		// PMM - don't blow up on bodies
		if (!strcmp(Other->ClassName.CString(), "bodyque"))
			return;
	}

	if (surf && (surf->Flags & SURF_TEXINFO_SKY))
	{
		Free ();
		return;
	}

	if (Other->EntityFlags & EF_HURTABLE)
	{
		entity_cast<IHurtableEntity>(Other)->TakeDamage (this, GetOwner(), Velocity, State.GetOrigin(), plane->Normal,
			10000, 1, DAMAGE_DESTROY_ARMOR, Mod);
	}
	else
		SplashDamage (GetOwner(), 512, GetOwner(), 256, Mod);

	Explode ();
}

void CRogueBaseSphere::OwnSphere (CPlayerEntity *Player)
{
	if (Player->Client.OwnedSphere && Player->Client.OwnedSphere->GetInUse())
		Player->Client.OwnedSphere->Free ();

	Player->Client.OwnedSphere = this;
}

// DEFENDER
void CRogueDefenderSphere::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Other == GetOwner() || !(Other->EntityFlags & EF_HURTABLE))
		return;

	if (Other->EntityFlags & EF_HURTABLE)
		SphereEnemy = entity_cast<IHurtableEntity>(Other);
}

void CRogueDefenderSphere::Shoot (IHurtableEntity *At)
{
	if (!(At->GetInUse()) || At->Health <= 0)
		return;
	if (At == GetOwner())
		return;
	if (AttackFinished > Level.Frame)
		return;
	if (!IsVisible(this, At))
		return;

	CGreenBlasterProjectile::Spawn (GetOwner(), State.GetOrigin() + vec3f(0, 0, 2), (At->State.GetOrigin() - State.GetOrigin()).GetNormalized(), 10, 1000, FX_BLASTER | FX_TRACKER);
	AttackFinished = Level.Frame + 4;
}

void CRogueDefenderSphere::Think ()
{
	if (!GetOwner())
	{
		Free ();
		return;
	}

	// if we've exited the level, just remove ourselves.
	if (Level.Intermission.Time)
	{	
		Explode ();
		return;
	}	

	if (entity_cast<IHurtableEntity>(GetOwner())->Health <= 0)
	{
		Explode ();
		return;
	}

	if ((++State.GetFrame()) > 19)
		State.GetFrame() = 0;

	if (SphereEnemy)
	{
		if (SphereEnemy->Health > 0)
			Shoot (SphereEnemy);
		else
			SphereEnemy = NULL;
	}

	Fly ();

	if (GetInUse())
		NextThink = Level.Frame + ServerFramesPerSecond;
}

CRogueDefenderSphere *CRogueDefenderSphere::Create (IBaseItem *Item, IBaseEntity *Owner, ESphereFlags Flags)
{
	CRogueDefenderSphere *Sphere = CreateBaseSphere<CRogueDefenderSphere> (Item, Owner, SPHERE_DEFENDER, Flags);

	Sphere->State.GetModelIndex() = ModelIndex("models/items/defender/tris.md2");
	Sphere->State.GetModelIndex(2) = ModelIndex("models/items/shell/tris.md2");
	Sphere->State.GetSound() = SoundIndex ("spheres/d_idle.wav");
	Sphere->Wait = Level.Frame + DEFENDER_LIFESPAN;

	return Sphere;
}

IMPLEMENT_SAVE_SOURCE(CRogueDefenderSphere);

// VENGEANCE
void CRogueVengeanceSphere::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (SphereEnemy)
		return;

	if (!(SphereFlags & SPHERE_DOPPLEGANGER))
	{
		if (OwnedPlayer->Health >= 25)
			return;
		if (Other == GetOwner())
			return;
	}
	else
		Wait = Level.Frame + MINIMUM_FLY_TIME;

	if ((Wait - Level.Frame) < MINIMUM_FLY_TIME)
		Wait = Level.Frame + MINIMUM_FLY_TIME;

	State.GetEffects() |= FX_ROCKET;
	Touchable = true;
	if (Other->EntityFlags & EF_HURTABLE)
		SphereEnemy = entity_cast<IHurtableEntity>(Other);
}

void CRogueVengeanceSphere::Think ()
{
	// if we've exited the level, just remove ourselves.
	if (Level.Intermission.Time)
	{	
		Explode ();
		return;
	}	

	if (!(GetOwner()) && !(SphereFlags & SPHERE_DOPPLEGANGER))
	{
		Free ();
		return;
	}

	if (SphereEnemy)
		Chase (true);
	else
		Fly ();

	if (GetInUse())
		NextThink = Level.Frame + ServerFramesPerSecond;
}

void CRogueVengeanceSphere::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	BaseTouch (Other, plane, surf, (SphereFlags & SPHERE_DOPPLEGANGER) ? MOD_DOPPLE_VENGEANCE : MOD_VENGEANCE_SPHERE);
}

CRogueVengeanceSphere *CRogueVengeanceSphere::Create (IBaseItem *Item, IBaseEntity *Owner, ESphereFlags Flags)
{
	CRogueVengeanceSphere *Sphere = CreateBaseSphere<CRogueVengeanceSphere> (Item, Owner, SPHERE_VENGEANCE, Flags);

	Sphere->State.GetModelIndex() = ModelIndex("models/items/vengnce/tris.md2");
	Sphere->State.GetSound() = SoundIndex ("spheres/v_idle.wav");
	Sphere->Wait = Level.Frame + VENGEANCE_LIFESPAN;
	Sphere->Touchable = false;
	Sphere->AngularVelocity.Set (30, 30, 0);

	return Sphere;
}

IMPLEMENT_SAVE_SOURCE(CRogueVengeanceSphere);

void BodyGib (CPlayerEntity *Player)
{
	Player->PlaySound (CHAN_BODY, SoundIndex("misc/udeath.wav"));
	for (int n = 0; n < 4; n++)
		CGibEntity::Spawn (Player, GameMedia.Gib_SmallMeat, 50, GIB_ORGANIC);
	CGibEntity::Spawn (Player, GameMedia.Gib_Skull, 50, GIB_ORGANIC);
}

// VENGEANCE
void CRogueHunterSphere::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (SphereEnemy)
		return;

	if (!(SphereFlags & SPHERE_DOPPLEGANGER))
	{
		if (GetOwner() && (OwnedPlayer->Health > 0))
			return;

		if (Other == GetOwner())
			return;
	}
	else
		Wait = Level.Frame + MINIMUM_FLY_TIME;

	if ((Wait - Level.Frame) < MINIMUM_FLY_TIME)
		Wait = Level.Frame + MINIMUM_FLY_TIME;

	State.GetEffects() |= (FX_BLASTER | FX_TRACKER);
	Touchable = true;
	if (Other->EntityFlags & EF_HURTABLE)
		SphereEnemy = entity_cast<IHurtableEntity>(Other);

	if ((SphereFlags & SPHERE_DOPPLEGANGER)  || !OwnedPlayer)
		return;

	if (!DeathmatchFlags.dfForceRespawn.IsEnabled())
	{
		vec3f dir = Other->State.GetOrigin() - State.GetOrigin();
		float dist = dir.Length();

		if (GetOwner() && (dist >= 192))
		{
			// detach owner from body and send him flying
			PhysicsType = PHYSICS_FLYMISSILE;

			// gib like we just died, even though we didn't, really.
			BodyGib(OwnedPlayer);

			// move the sphere to the owner's current viewpoint.
			// we know it's a valid spot (or will be momentarily)
			State.GetOrigin() = OwnedPlayer->State.GetOrigin() + vec3f(0, 0, OwnedPlayer->ViewHeight);

			// move the player's origin to the sphere's new origin
			OwnedPlayer->State.GetOrigin() = State.GetOrigin();
			OwnedPlayer->State.GetAngles() = State.GetAngles();
			OwnedPlayer->Client.ViewAngle = State.GetAngles();
			
			OwnedPlayer->GetMins().Set(-5, -5, -5);
			OwnedPlayer->GetMaxs().Set(5, 5, 5);
			OwnedPlayer->Client.PlayerState.GetFov() = 140;
			OwnedPlayer->State.GetModelIndex() = 0;
			OwnedPlayer->State.GetModelIndex(2) = 0;
			OwnedPlayer->ViewHeight = 8;
			OwnedPlayer->GetSolid() = SOLID_NOT;
			OwnedPlayer->Flags |= FL_SAM_RAIMI;
			OwnedPlayer->Link ();

			// PMM - set bounding box so we don't clip out of world
			GetSolid() = SOLID_BBOX;
			Link ();
		}
	}
}

void CRogueHunterSphere::ChangeYaw (float IdealYaw)
{
	float current = AngleModf (State.GetAngles().Y);

	if (current == IdealYaw)
		return;

	float move = IdealYaw - current;
	if (IdealYaw > current)
	{
		if (move >= 180)
			move = move - 360;
	}
	else
	{
		if (move <= -180)
			move = move + 360;
	}

	if (move > 0)
	{
		if (move > 40)
			move = 40;
	}
	else
	{
		if (move < -40)
			move = -40;
	}
	
	State.GetAngles().Y = AngleModf(current + move);
}

void CRogueHunterSphere::Think ()
{
	// if we've exited the level, just remove ourselves.
	if (Level.Intermission.Time)
	{	
		Explode ();
		return;
	}	

	if (!GetOwner() && !(SphereFlags & SPHERE_DOPPLEGANGER))
	{
		Free ();
		return;
	}

	float IdealYaw = 0;

	if (GetOwner())
		IdealYaw = OwnedPlayer->State.GetAngles().Y;
	else if (SphereEnemy)		// fired by doppleganger
		IdealYaw = (SphereEnemy->State.GetOrigin() - State.GetOrigin()).ToAngles().Y;

	ChangeYaw (IdealYaw);

	if (SphereEnemy)
	{
		Chase (false);

		// deal with sam raimi cam
		if (OwnedPlayer && (OwnedPlayer->Flags & FL_SAM_RAIMI)) 
		{
			if (GetInUse())
			{
				OwnedPlayer->LookAtKiller (this, SphereEnemy);

				// owner is flying with us, move him too
				OwnedPlayer->PhysicsType = PHYSICS_FLYMISSILE;
				OwnedPlayer->ViewHeight = State.GetOrigin().Z - OwnedPlayer->State.GetOrigin().Z;
				OwnedPlayer->State.GetOrigin() = State.GetOrigin();
				OwnedPlayer->Velocity = Velocity;
				OwnedPlayer->GetMins().Clear();
				OwnedPlayer->GetMaxs().Clear();
				OwnedPlayer->Link ();
			}
			else	// sphere timed out
			{
				OwnedPlayer->Velocity.Clear();
				OwnedPlayer->PhysicsType = PHYSICS_NONE;
				OwnedPlayer->Link ();
			}
		}
	}
	else 
		Fly ();

	if (GetInUse())
		NextThink = Level.Frame + ServerFramesPerSecond;
}

void CRogueHunterSphere::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	BaseTouch (Other, plane, surf, (SphereFlags & SPHERE_DOPPLEGANGER) ? MOD_DOPPLE_VENGEANCE : MOD_VENGEANCE_SPHERE);
}

CRogueHunterSphere *CRogueHunterSphere::Create (IBaseItem *Item, IBaseEntity *Owner, ESphereFlags Flags)
{
	CRogueHunterSphere *Sphere = CreateBaseSphere<CRogueHunterSphere> (Item, Owner, SPHERE_VENGEANCE, Flags);

	Sphere->State.GetModelIndex() = ModelIndex("models/items/hunter/tris.md2");
	Sphere->State.GetSound() = SoundIndex ("spheres/h_idle.wav");
	Sphere->Wait = Level.Frame + HUNTER_LIFESPAN;
	Sphere->Touchable = false;

	return Sphere;
}

IMPLEMENT_SAVE_SOURCE(CRogueHunterSphere);

void CDoppleGangerBody::Think ()
{
	if (Abs(IdealYaw - AngleModf(State.GetAngles().Y)) < 2)
	{
		if (TurnTime < Level.Frame)
		{
			if (frand() < 0.10f)
			{
				IdealYaw = frand() * 360.0;
				TurnTime = Level.Frame + 10;
			}
		}
	}
	else
	{
		float current = AngleModf (State.GetAngles().Y);

		if (current != IdealYaw)
		{
			float move = IdealYaw - current;
			if (IdealYaw > current)
			{
				if (move >= 180)
					move = move - 360;
			}
			else
			{
				if (move <= -180)
					move = move + 360;
			}
			if (move > 0)
			{
				if (move > 30)
					move = 30;
			}
			else
			{
				if (move < -30)
					move = -30;
			}
	
			State.GetAngles().Y = AngleModf(current+move);
		}
	}

	if ((++State.GetFrame()) > CPlayerEntity::FRAME_stand40)
		State.GetFrame() = CPlayerEntity::FRAME_stand01;

	NextThink = Level.Frame + ServerFramesPerSecond;
}

IMPLEMENT_SAVE_SOURCE (CDoppleGangerBody);

void CDoppleGanger::Think ()
{
	Body->BecomeExplosion (false);
	BecomeExplosion (false);
}

void CDoppleGanger::Pain (IBaseEntity *Other, sint32 Damage)
{
	Enemy = Other;
}

void CDoppleGanger::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	if (Enemy && (Enemy != PlayerOwner))
	{
		if ((Enemy->State.GetOrigin() - State.GetOrigin()).Length() > 768)
			CRogueHunterSphere::Create (NULL, this, SPHERE_DOPPLEGANGER)->Pain (Attacker, 0);
		else
			CRogueVengeanceSphere::Create (NULL, this, SPHERE_DOPPLEGANGER)->Pain (Attacker, 0);
	}

	Body->BecomeExplosion (false);
	BecomeExplosion (false);
}

void CDoppleGanger::Spawn (CPlayerEntity *Owner, vec3f Start, vec3f AimDir)
{
	vec3f		dir = AimDir.ToAngles();

	CDoppleGanger *base = QNewEntityOf CDoppleGanger;
	base->State.GetOrigin() = Start;
	base->State.GetAngles() = dir;
	base->Velocity.Clear ();
	base->AngularVelocity.Clear();
	base->PhysicsType = PHYSICS_TOSS;
	base->GetSolid() = SOLID_BBOX;
	base->State.GetRenderEffects() |= RF_IR_VISIBLE;
	base->State.GetAngles().X = 0;
	base->GetMins().Set (-16, -16, -24);
	base->GetMaxs().Set (16, 16, 32);
	base->State.GetModelIndex() = 0;
	base->PlayerOwner = Owner;
	base->CanTakeDamage = true;
	base->Health = 30;
	base->NextThink = Level.Frame + 300;
	base->Link ();

	base->Body = QNewEntityOf CDoppleGangerBody;
	base->Body->State.GetAngles() = Owner->State.GetAngles();
	base->Body->State.GetAngles().X = 0;
	base->Body->State.GetEffects() = Owner->State.GetEffects();
	base->Body->State.GetFrame() = Owner->State.GetFrame();
	base->Body->State.GetModelIndex() = Owner->State.GetModelIndex();
	base->Body->State.GetRenderEffects() = Owner->State.GetRenderEffects();
	base->Body->State.GetSkinNum() = Owner->State.GetSkinNum();
	base->Body->State.GetSound() = 0;
	base->Body->State.GetEvent() = 0;
	base->Body->State.GetOrigin() = Owner->State.GetOldOrigin() = Start + vec3f(0, 0, 8);
	base->Body->NextThink = Level.Frame + ServerFramesPerSecond;
	base->Body->Link ();
}

IMPLEMENT_SAVE_SOURCE(CDoppleGanger);

#endif
