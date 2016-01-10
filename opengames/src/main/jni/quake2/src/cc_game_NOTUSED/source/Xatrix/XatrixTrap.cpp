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
// cc_xatrix_trap.cpp
// 
//

#include "Local.h"

#if XATRIX_FEATURES
#include "Weapons/WeaponMain.h"
#include "Xatrix/XatrixTrap.h"
#include "Utility/TemporaryEntities.h"

/*
=================
CTrapProjectile
=================
*/

CTrapProjectile::CTrapProjectile () :
  IBounceProjectile(),
  ITouchableEntity(),
  IThinkableEntity()
{
};

CTrapProjectile::CTrapProjectile (sint32 Index) :
  IBaseEntity (Index),
  IBounceProjectile(Index),
  ITouchableEntity(Index),
  IThinkableEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CTrapProjectile)

void CTrapProjectile::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
}

class CJustAModel : public IBaseEntity
{
public:
	CJustAModel () :
	  IBaseEntity ()
	  {
	  };

	CJustAModel (sint32 Index) :
	  IBaseEntity (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER(CJustAModel);
};

IMPLEMENT_SAVE_SOURCE(CJustAModel);

// Foodcube
CFoodCube::CFoodCube () :
  IBasePowerUp ("item_foodcube", "models/objects/trapfx/tris.md2", 0, "items/m_health.wav", "i_health", "Health", ITEMFLAG_HEALTH|ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE, 0)
{
};

void CFoodCube::DoPickup (CItemEntity *Item, CPlayerEntity *Other)
{
	Other->Health += Item->AmmoCount;
	Item->PlaySound (CHAN_ITEM, GetPickupSound());
};

#include "Xatrix/XatrixGekk.h"

void CTrapProjectile::Think ()
{
	if (DoFree)
	{
		Free ();
		return;
	}
	
	if (TimeStamp < Level.Frame)
	{
		BecomeExplosion(false);
		// note to self
		// cause explosion damage???
		return;
	}
	
	NextThink = Level.Frame + 1;
	
	if (!GroundEntity)
		return;

	// ok lets do the blood effect
	if (State.GetFrame() > 4)
	{
		if (State.GetFrame() == 5)
		{
			if (Wait == 64)
				PlaySound (CHAN_VOICE, SoundIndex ("weapons/trapdown.wav"), 255, ATTN_IDLE);

			Wait -= 2;
			Delay += Level.Frame;

			for (uint8 i = 0; i < 3; i++)
			{
				IBaseEntity *best;
				if (!TrapEntities[i])
				{
					best = QNewEntityOf CJustAModel;
					TrapEntities[i] = best;

					best->GetSolid() = SOLID_NOT;
					best->GetSvFlags() |= SVF_MONSTER;
					best->GetMins().Clear();
					best->GetMaxs().Clear();

					if ((Enemy->EntityFlags & EF_MONSTER) && (entity_cast<CMonsterEntity>(*Enemy)->Monster->MonsterID == CGekk::ID))
					{
						best->State.GetModelIndex() = ModelIndex ("models/objects/gekkgib/torso/tris.md2");	
						best->State.GetEffects() |= FX_GREENGIB;
					}
					else if (Mass >= 200)
					{
						best->State.GetModelIndex() = GameMedia.Gib_Chest;	
						best->State.GetEffects() |= FX_GIB;
					}
					else
					{
						best->State.GetModelIndex() = GameMedia.Gib_SmallMeat;	
						best->State.GetEffects() |= FX_GIB;
					}
				}
				else
					best = TrapEntities[i];

				vec3f vec;
				anglef angles = State.GetAngles().ToVectors ();

				vec = angles.Right.RotateAroundVector (angles.Up, (120.0f * i) + Delay)
						.MultiplyAngles (Wait/2, vec) + State.GetOrigin();
				best->State.GetOrigin() = vec + angles.Forward;
				best->State.GetOrigin().Z = State.GetOrigin().Z + Wait;
  				best->State.GetAngles() = State.GetAngles();

				best->Link ();
			}
				
			if (Wait < 19)
			{
				State.GetFrame() ++;
				for (int i = 0; i < 3; i++)
					TrapEntities[i]->Free ();
			}

			return;
		}
		State.GetFrame() ++;
		if (State.GetFrame() == 8)
		{
			NextThink = Level.Frame + 10;
			DoFree = true;

			CItemEntity *it_ent = QNewEntityOf CItemEntity;
			it_ent->ClassName = "item_foodcube";
			it_ent->SpawnFlags |= DROPPED_ITEM;
			it_ent->Spawn (FoodCubeItem);
			it_ent->State.GetOrigin() = State.GetOrigin() + vec3f(0, 0, 16);
			it_ent->Velocity.Z = 400;
			it_ent->AmmoCount = Mass;
			it_ent->Link ();

			return;
		}
		return;
	}
	
	State.GetEffects() &= ~FX_TRAP;
	if (State.GetFrame() >= 4)
	{
		State.GetEffects() |= FX_TRAP;
		GetMins().Clear();
		GetMaxs().Clear();

	}

	if (State.GetFrame() < 4)
		State.GetFrame()++;

	IHurtableEntity *target = NULL;
	IHurtableEntity *best = NULL;
	float oldlen = 99999;
	while ((target = FindRadius<IHurtableEntity, EF_HURTABLE> (target, State.GetOrigin (), 256)) != NULL)
	{
		if (target->Health <= 0)
			continue;
		if (!(target->EntityFlags & EF_PLAYER) && !(target->EntityFlags & EF_MONSTER))
			continue;
		if (!IsVisible (this, target))
		 	continue;

		if (!best)
		{
			best = target;
			continue;
		}

		float Len = (State.GetOrigin() - target->State.GetOrigin()).Length();
		if (Len < oldlen)
		{
			oldlen = Len;
			best = target;
		}
	}

	// pull the enemy in
	if (best)
	{
		if (best->GroundEntity)
		{
			best->State.GetOrigin().Z += 1;
			best->GroundEntity = nullentity;
			best->Link ();
		}

		vec3f vec = State.GetOrigin() - best->State.GetOrigin();

		if (best->EntityFlags & EF_PLAYER)
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(best);
			Player->Velocity = Player->Velocity.MultiplyAngles (250, vec.GetNormalized());
		}
		else
		{
			CMonsterEntity *Monster = entity_cast<CMonsterEntity>(best);
			Monster->Monster->IdealYaw = vec.ToYaw();	
			Monster->Monster->ChangeYaw ();
			Monster->Velocity = best->State.GetAngles().ToVectors().Forward * 256;
		}

		PlaySound(CHAN_VOICE, SoundIndex ("weapons/trapsuck.wav"), 255, ATTN_IDLE);
		
		if (vec.Length() < 32)
		{
			if (best->EntityFlags & EF_PHYSICS)
			{
				IPhysicsEntity *Phys = entity_cast<IPhysicsEntity>(best);
				
				if (Phys->Mass < 400)
				{
					best->TakeDamage (this, GetOwner(), vec3fOrigin, best->State.GetOrigin(), vec3fOrigin, 100000, 1, 0, MOD_TRAP);
					Enemy = best;
					Wait = 64;
					State.GetOldOrigin() = State.GetOrigin();
					TimeStamp = Level.Frame + 300;
					if (Game.GameMode & GAME_DEATHMATCH)
						Mass = Phys->Mass/4;
					else
						Mass = Phys->Mass/10;
					// ok spawn the food cube
					State.GetFrame() = 5;	
				}
			}
			else
			{
				BecomeExplosion(false);
				// note to self
				// cause explosion damage???
				return;
			}	
		}
	}
}

const int TRAP_RADIUS_DAMAGE	= 165;
const int TRAP_TIMER			= 50;
const int TRAP_MINSPEED			= 300;
const int TRAP_MAXSPEED			= 700;

void CTrapProjectile::Explode ()
{
	if (GetOwner() && (GetOwner()->EntityFlags & EF_PLAYER))
		entity_cast<CPlayerEntity>(GetOwner())->PlayerNoiseAt (State.GetOrigin (), PNOISE_IMPACT);

	if (Enemy)
	{
		IHurtableEntity *HurtEnemy = entity_cast<IHurtableEntity>(*Enemy);

		vec3f v = (HurtEnemy->GetMins() + HurtEnemy->GetMaxs());
		v = (State.GetOrigin () - HurtEnemy->State.GetOrigin().MultiplyAngles (0.5f, v));

		float points = ((float)Damage) - 0.5 * v.Length();
		vec3f dir = HurtEnemy->State.GetOrigin() - State.GetOrigin ();

		HurtEnemy->TakeDamage	(this, GetOwner(), dir, State.GetOrigin (), vec3fOrigin, (sint32)points, (sint32)points,
							DAMAGE_RADIUS, MOD_HANDGRENADE);
	}

	SplashDamage(GetOwner(), Damage, *Enemy, TRAP_RADIUS_DAMAGE, MOD_HELD_GRENADE);

	vec3f origin = State.GetOrigin ().MultiplyAngles (-0.02f, Velocity);
	if (GroundEntity)
		CGrenadeExplosion(origin, !!WaterInfo.Level).Send();
	else
		CRocketExplosion(origin, !!WaterInfo.Level).Send();

	Free (); // "delete" the entity
}

void CTrapProjectile::Spawn (IBaseEntity *Spawner, vec3f start, vec3f aimdir, int damage, float timer, sint32 speed)
{
	CTrapProjectile	*Grenade = QNewEntityOf CTrapProjectile;

	vec3f dir = aimdir.ToAngles();

	anglef angles = dir.ToVectors ();

	Grenade->State.GetOrigin() = start;
	aimdir *= speed;

	Grenade->Velocity = aimdir;
	Grenade->Velocity = Grenade->Velocity.MultiplyAngles (200 + crand() * 10.0f, angles.Up);
	Grenade->Velocity = Grenade->Velocity.MultiplyAngles (crand() * 10.0, angles.Right);

	Grenade->AngularVelocity.Set (0, 300, 0);
	Grenade->State.GetModelIndex() = ModelIndex ("models/weapons/z_trap/tris.md2");
	Grenade->SetOwner(Spawner);
	Grenade->NextThink = Level.Frame + 10;
	Grenade->ClassName = "htrap";
	Grenade->GetClipmask() = CONTENTS_MASK_SHOT;
	Grenade->GetSolid() = SOLID_BBOX;
	Grenade->GetMins().Set (-4, -4, 0);
	Grenade->GetMaxs().Set (4, 4, 8);
	Grenade->Touchable = true;
	Grenade->State.GetSound() = SoundIndex("weapons/traploop.wav");
	Grenade->TimeStamp = Level.Frame + 300;
	Grenade->Damage = damage;

	if (timer <= 0.0)
		Grenade->Explode ();
	else
		Grenade->Link();
}

bool CTrapProjectile::Run ()
{
	return IBounceProjectile::Run();
}

CTrap::CTrap() :
IWeaponBase(6, 1, "models/weapons/v_trap/tris.md2", 0, 0, 0, 16,
		17, 48, 0, 0)
{
}

bool CTrap::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 29:
	case 34:
	case 39:
	case 48:
		return true;
	}
	return false;
}

void CTrap::Hold (CPlayerEntity *Player)
{
	if (!Player->Client.Grenade.Time)
	{
		Player->Client.Grenade.Time = Level.Frame + TRAP_TIMER + 2;
		Player->Client.WeaponSound = SoundIndex("weapons/traploop.wav");
	}

	// they waited too long, detonate it in their hand
	if (!Player->Client.Grenade.BlewUp && (Level.Frame >= Player->Client.Grenade.Time))
	{
		Player->Client.WeaponSound = 0;
		FireGrenade (Player, true);
		Player->Client.Grenade.BlewUp = true;

		Player->Client.PlayerState.GetGunFrame() = 15;
		return;
	}

	if (Player->Client.Buttons & BUTTON_ATTACK)
		return;

	Player->Client.PlayerState.GetGunFrame()++;
}

void CTrap::FireGrenade (CPlayerEntity *Player, bool inHand)
{
	vec3f	offset (8, 8, Player->ViewHeight-8), forward, right, start;
	const sint32		damage = CalcQuadVal(125);

	Player->Client.Grenade.Thrown = true;

	anglef angles = Player->Client.ViewAngle.ToVectors ();
	Player->P_ProjectSource (offset, angles, start);

	float timer = (float)(Player->Client.Grenade.Time - Level.Frame) / 10;
	const sint32 speed = (Player->Client.Persistent.Weapon) ? 
		(TRAP_MINSPEED + ((TRAP_TIMER/10) - timer) * ((TRAP_MAXSPEED - TRAP_MINSPEED) / (TRAP_TIMER/10)))
		: 25; // If we're dead, don't toss it 5 yards.
	CTrapProjectile::Spawn (Player, start, angles.Forward, damage, timer, speed);

	Player->Client.Grenade.Time = Level.Frame + ((((
#if CLEANCTF_ENABLED
	(Game.GameMode & GAME_CTF) || 
#endif
	DeathmatchFlags.dfDmTechs.IsEnabled()) && Player->ApplyHaste())
#if XATRIX_FEATURES
	|| isQuadFire
#endif
	) ? 5 : 10);

	DepleteAmmo(Player, 1);

	if (Player->Health <= 0 || Player->IsDead || Player->State.GetModelIndex() != 255) // VWep animations screw up corpses
		return;

	AttackSound (Player);

	if (Player->Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED)
	{
		Player->Client.Anim.Priority = ANIM_ATTACK;
		Player->State.GetFrame() = CPlayerEntity::FRAME_crattak1 - 1;
		Player->Client.Anim.EndFrame = CPlayerEntity::FRAME_crattak3;
	}
	else
	{
		Player->Client.Anim.Priority = ANIM_REVERSE;
		Player->State.GetFrame() = CPlayerEntity::FRAME_wave08;
		Player->Client.Anim.EndFrame = CPlayerEntity::FRAME_wave01;
	}
	Player->Client.PlayerState.GetGunFrame()++;
}

void CTrap::Wait (CPlayerEntity *Player)
{
	Player->Client.Grenade.BlewUp = false;
	if (Level.Frame < Player->Client.Grenade.Time)
		return;

	if (!Player->IsDead)
		Player->Client.Grenade.Thrown = false;
	Player->Client.PlayerState.GetGunFrame()++;
}

void CTrap::Fire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 11:
		Hold (Player);
		break;
	case 12:
		Player->Client.WeaponSound = 0;
		FireGrenade(Player, false);
		break;
	case 15:
		Wait(Player);
		break;
	}
}

bool CTrap::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 5:
		Player->PlaySound (CHAN_WEAPON, SoundIndex("weapons/trapcock.wav"));
		return false;
	case 11:
	case 12:
	case 15:
		return true;
	}
	return false;
}

void CTrap::WeaponGeneric (CPlayerEntity *Player)
{
	// Idea from Brazen source
	sint32 newFrame = -1, newState = -1;

	switch (Player->Client.WeaponState)
	{
	case WS_ACTIVATING:
		newFrame = IdleStart;
		newState = WS_IDLE;
		break;
	case WS_IDLE:
		if (Player->Client.NewWeapon && Player->Client.NewWeapon != this)
		{
			// We want to go away!
			newState = WS_DEACTIVATING;
			newFrame = DeactStart;
		}
		else if ((Player->Client.Buttons|Player->Client.LatchedButtons) & BUTTON_ATTACK)
		{
			Player->Client.LatchedButtons &= ~BUTTON_ATTACK;

			// We want to attack!
			// First call, check AttemptToFire
			if (AttemptToFire(Player))
			{
				// Got here, we can fire!
				Player->Client.PlayerState.GetGunFrame() = FireStart;
				Player->Client.WeaponState = WS_FIRING;
				Player->Client.Grenade.Time = 0;

				// We need to check against us right away for first-frame firing
				WeaponGeneric(Player);
				return;
			}
			else
			{
				OutOfAmmo(Player);
				NoAmmoWeaponChange (Player);
			}
		}

		// Either we are still idle or a failed fire.
		if (newState == -1)
		{
			if (Player->Client.PlayerState.GetGunFrame() == IdleEnd)
				newFrame = IdleStart;
			else
			{
				if (CanStopFidgetting(Player) && (randomMT()&15))
					newFrame = Player->Client.PlayerState.GetGunFrame();
			}
		}
		break;
	case WS_FIRING:
		// Check if this is a firing frame.
		if (CanFire(Player))
		{
			Fire(Player);

			// Now, this call above CAN change the underlying frame and state.
			// We need this block to make sure we are still doing what we are supposed to.
			newState = Player->Client.WeaponState;
			newFrame = Player->Client.PlayerState.GetGunFrame();
		}

		// Only do this if we haven't been explicitely set a newFrame
		// because we might want to keep firing beyond this point
		if (newFrame == -1 && Player->Client.PlayerState.GetGunFrame() == FireEnd)
		{
			if (!Player->Client.Persistent.Inventory.Has(Item))
			{
				NoAmmoWeaponChange (Player);
				Player->Client.Grenade.Time = 0;
				Player->Client.Grenade.Thrown = false;
			}
			else
			{
				Player->Client.Grenade.Time = 0;
				Player->Client.Grenade.Thrown = false;
				newFrame = IdleStart;
				newState = WS_IDLE;
			}
		}
		break;
	case WS_DEACTIVATING:
		// Change weapon
		ChangeWeapon (Player);
		return;
		break;
	}

	if (newFrame != -1)
		Player->Client.PlayerState.GetGunFrame() = newFrame;
	if (newState != -1)
		Player->Client.WeaponState = newState;

	if (newFrame == -1 && newState == -1)
		Player->Client.PlayerState.GetGunFrame()++;
}

WEAPON_DEFS (CTrap);

void CTrap::CreateItem (CItemList *List)
{
};
#endif
