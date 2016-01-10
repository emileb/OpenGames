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
// cc_rogue_tesla.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES

#include "Weapons/WeaponMain.h"
#include "Rogue/RogueTesla.h"
#include "Rogue/RogueWeaponry.h"
#include "Utility/TemporaryEntities.h"

// Tesla itself

const int TESLA_TIME_TO_LIVE			= 300;
const int TESLA_DAMAGE_RADIUS			= 128;
const int TESLA_DAMAGE					= 3;
const int TESLA_KNOCKBACK				= 8;
const int TESLA_ACTIVATE_TIME			= 30;
const int TESLA_EXPLOSION_DAMAGE_MULT	= 50;		// this is the amount the damage is multiplied by for underwater explosions
const int TESLA_EXPLOSION_RADIUS		= 200;

CTesla::CTesla () :
  IBounceProjectile (),
  ITouchableEntity (),
  IHurtableEntity (),
  IThinkableEntity (),
  BadArea(NULL)
  {
  };

CTesla::CTesla (sint32 Index) :
  IBaseEntity (Index),
  IBounceProjectile (Index),
  ITouchableEntity (Index),
  IHurtableEntity (Index),
  IThinkableEntity (Index),
  BadArea(NULL)
  {
  };

void CTesla::SaveFields (CFile &File)
{
	IBounceProjectile::SaveFields (File);
	ITouchableEntity::SaveFields (File);
	IHurtableEntity::SaveFields (File);
	IThinkableEntity::SaveFields (File);

	File.Write<ETeslaThinkType> (ThinkType);

	File.Write<sint32> ((Firer != NULL && Firer->GetInUse()) ? Firer->State.GetNumber() : -1);

	File.Write<bool> (DoExplosion);
	File.Write<int> (Damage);
	File.Write<FrameNumber> (RemoveTime);

	SaveBadArea (File, BadArea);
};

void CTesla::LoadFields (CFile &File)
{
	IBounceProjectile::LoadFields (File);
	ITouchableEntity::LoadFields (File);
	IHurtableEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);

	ThinkType = File.Read<ETeslaThinkType> ();

	sint32 index = File.Read<sint32>();
	if (index != -1)
		Firer = entity_cast<CPlayerEntity>(Game.Entities[index].Entity);

	DoExplosion = File.Read<bool>();
	Damage = File.Read<int>();
	RemoveTime = File.Read<FrameNumber>();

	BadArea = LoadBadArea (File);
};

bool CTesla::Run ()
{
	return IBounceProjectile::Run();
}

//TE_ELECTRIC_SPARKS

void CTesla::DamageEffect (vec3f &Dir, vec3f &Point, vec3f &Normal, sint32 &Damage, EDamageFlags &DamageFlags, EMeansOfDeath &MeansOfDeath)
{
	CSparks(Point, Normal, ST_ELECTRIC_SPARKS).Send();
}

void CTesla::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	Remove ();

	if (Attacker->EntityFlags & EF_MONSTER)
	{
		CMonsterEntity *Monster = entity_cast<CMonsterEntity>(Attacker);

		// Paril: Last ditch attempt at fixing this...
		Monster->Monster->FixInvalidEntities();
	}
}

void CTesla::Remove ()
{
	CanTakeDamage = false;
	SetOwner(Firer);	// Going away, set the owner correctly.
	Enemy = nullentity;

	// play quad sound if quadded and an underwater explosion
	if (DoExplosion && (Damage > (TESLA_DAMAGE * TESLA_EXPLOSION_DAMAGE_MULT)))
		PlaySound (CHAN_ITEM, SoundIndex("items/damage3.wav"));

	Explode ();
}

void CTesla::Explode ()
{
	if (GetOwner() && (GetOwner()->EntityFlags & EF_PLAYER))
		entity_cast<CPlayerEntity>(GetOwner())->PlayerNoiseAt (State.GetOrigin (), PNOISE_IMPACT);

	SplashDamage(GetOwner(), Damage, *Enemy, (DoExplosion) ? TESLA_DAMAGE_RADIUS : 0, MOD_G_SPLASH);

	vec3f origin = State.GetOrigin ().MultiplyAngles (-0.02f, Velocity);
	
	if (PointContents(origin) & CONTENTS_MASK_SOLID)
		origin = State.GetOrigin();
	
	if (GroundEntity)
		CGrenadeExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_PHS, State.GetOrigin()), origin, !!WaterInfo.Level).Send();
	else
		CRocketExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_PHS, State.GetOrigin()), origin, !!WaterInfo.Level).Send();

	Free (); // "delete" the entity
}

void CTesla::Blow ()
{
	Damage *= TESLA_EXPLOSION_DAMAGE_MULT;
	DoExplosion = true;
	Remove ();
}

void CTesla::Active ()
{		
	if (Level.Frame > RemoveTime)
	{
		Remove ();
		return;
	}

	vec3f start = State.GetOrigin() + vec3f(0, 0, 16);
	TBoxEdictsEntityList Entities = BoxEdicts(State.GetOrigin() + vec3f(-TESLA_DAMAGE_RADIUS, -TESLA_DAMAGE_RADIUS, GetMins().Z), State.GetOrigin() + vec3f(TESLA_DAMAGE_RADIUS, TESLA_DAMAGE_RADIUS, TESLA_DAMAGE_RADIUS), false);

	for (TBoxEdictsEntityList::iterator it = Entities.begin(); it < Entities.end(); ++it)
	{
		// if the tesla died while zapping things, stop zapping.
		if (!GetInUse())
			break;

		IBaseEntity *Hit = (*it);

		if (!Hit->GetInUse())
			continue;
		if (!(Hit->EntityFlags & EF_HURTABLE))
			continue;
		if (Hit == this)
			continue;

		IHurtableEntity *Hurtable = entity_cast<IHurtableEntity>(Hit);

		if (Hurtable->Health < 1)
			continue;

		// don't hit clients in single-player or coop
		if ((Hit->EntityFlags & EF_PLAYER) && !(Game.GameMode & GAME_DEATHMATCH))
			continue;
	
		CTrace tr (start, Hit->State.GetOrigin(), this, CONTENTS_MASK_SHOT);
		if (tr.Fraction == 1 || tr.Entity == Hit)
		{
			vec3f dir = Hit->State.GetOrigin() - start;
			
			// PMM - play quad sound if it's above the "normal" damage
			if (Damage > TESLA_DAMAGE)
				PlaySound (CHAN_ITEM, SoundIndex("items/damage3.wav"));

			// PGM - don't do knockback to walking monsters
			if ((Hit->EntityFlags & EF_MONSTER) && entity_cast<CMonsterEntity>(Hit)->Monster->AIFlags & (AI_FLY | AI_SWIM))
				Hurtable->TakeDamage (this, Firer, dir, tr.EndPosition, tr.Plane.Normal,
					Damage, 0, 0, MOD_TESLA);
			else
				Hurtable->TakeDamage (this, Firer, dir, tr.EndPosition, tr.Plane.Normal,
					Damage, TESLA_KNOCKBACK, 0, MOD_TESLA);

			CLightning(tr.EndPosition, start, State.GetNumber(), Hit->State.GetNumber()).Send();
		}
	}

	if (GetInUse())
		NextThink = Level.Frame + ServerFramesPerSecond;
}

void CTesla::DoneActivate ()
{
	if (PointContents (State.GetOrigin()) & CONTENTS_MASK_WATER)
	{
		Blow ();
		return;
	}

	// only check for spawn points in deathmatch
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		IBaseEntity *Search = NULL;
		while ((Search = FindRadius<EF_BASE> (Search, State.GetOrigin(), 1.5 * TESLA_DAMAGE_RADIUS)) != NULL)
		{
			// if it's a deathmatch start point
			// and we can see it
			// blow up
			if (!Search->ClassName.IsNullOrEmpty())
			{
				if ( ( (!strcmp(Search->ClassName.CString(), "info_player_deathmatch"))
					|| (!strcmp(Search->ClassName.CString(), "info_player_start"))
					|| (!strcmp(Search->ClassName.CString(), "info_player_coop"))
					|| (!strcmp(Search->ClassName.CString(), "misc_teleporter_dest"))) 
					&& (IsVisible (Search, this)))
				{
					Remove ();
					return;
				}
			}
		}
	}

	State.GetAngles().Clear();
	// clear the owner if in deathmatch
	if (Game.GameMode & GAME_DEATHMATCH)
		SetOwner(NULL);

	ThinkType = TESLATHINK_ACTIVE;
	NextThink = Level.Frame + ServerFramesPerSecond;
	RemoveTime = Level.Frame + TESLA_TIME_TO_LIVE;
}

void CTesla::Activate ()
{
	if (PointContents (State.GetOrigin()) & (CONTENTS_SLIME|CONTENTS_LAVA))
	{
		Remove ();
		return;
	}

	State.GetAngles().Clear ();

	if(!State.GetFrame())
		PlaySound (CHAN_VOICE, SoundIndex ("weapons/teslaopen.wav"));

	if ((++State.GetFrame()) > 14)
	{
		State.GetFrame() = 14;
		ThinkType = TESLATHINK_DONEACTIVATE;
		NextThink = Level.Frame + ServerFramesPerSecond;
	}
	else
	{
		if (State.GetFrame() > 9)
		{
			if (State.GetFrame() == 10)
			{
				//if (Firer)
				//	Firer->PlayerNoiseAt (State.GetOrigin(), PNOISE_WEAPON);

				State.GetSkinNum() = 1;
			}
			else if (State.GetFrame() == 12)
				State.GetSkinNum() = 2;
			else if (State.GetFrame() == 14)
				State.GetSkinNum() = 3;
		}
		
		NextThink = Level.Frame + ServerFramesPerSecond;
	}
}

void CTesla::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	if (plane->Normal)
	{
		if (PointContents(State.GetOrigin().MultiplyAngles (-20.0f, plane->Normal)) & (CONTENTS_SLIME|CONTENTS_LAVA))
		{
			Blow ();
			return;
		}
	}

	PlaySound (CHAN_VOICE, SoundIndex (frand() > 0.5f ? "weapons/hgrenb1a.wav" : "weapons/hgrenb2a.wav"));
}

void CTesla::Think ()
{
	switch (ThinkType)
	{
	case TESLATHINK_ACTIVE:
		Active ();
		break;
	case TESLATHINK_ACTIVATE:
		Activate ();
		break;
	case TESLATHINK_DONEACTIVATE:
		DoneActivate ();
		break;
	};
}

void CTesla::Spawn (CPlayerEntity *Player, vec3f Start, vec3f AimDir, int DamageMultiplier, int Speed)
{
	anglef angles = AimDir.ToAngles().ToVectors ();

	CTesla *Tesla = QNewEntityOf CTesla;
	Tesla->State.GetOrigin() = Start;
	Tesla->Velocity =	(AimDir * Speed)
						.MultiplyAngles (200 + crand() * 10.0f, angles.Up)
						.MultiplyAngles (crand() * 10.0f, angles.Right);

	Tesla->State.GetAngles().Clear();
	Tesla->GetSolid() = SOLID_BBOX;
	Tesla->State.GetEffects() |= FX_GRENADE;
	Tesla->State.GetRenderEffects() |= RF_IR_VISIBLE;
	Tesla->GetMins().Set (-12, -12, 0);
	Tesla->GetMaxs().Set (12, 12, 20);
	Tesla->State.GetModelIndex() = ModelIndex ("models/weapons/g_tesla/tris.md2");
	
	Tesla->SetOwner(Player);
	Tesla->Firer = Player;

	Tesla->ThinkType = TESLATHINK_ACTIVATE;
	Tesla->NextThink = Level.Frame + TESLA_ACTIVATE_TIME;

	Tesla->Touchable = true;

	Tesla->Health = (Game.GameMode & GAME_DEATHMATCH) ? 20 : 30;
	Tesla->CanTakeDamage = true;
	Tesla->Damage = TESLA_DAMAGE*DamageMultiplier;
	Tesla->ClassName = "tesla";
	Tesla->GetClipmask() = (CONTENTS_MASK_SHOT|CONTENTS_SLIME|CONTENTS_LAVA);

	Tesla->Link ();
}

IMPLEMENT_SAVE_SOURCE (CTesla);

// Tesla weapon
CTeslaWeapon::CTeslaWeapon() :
IWeaponBase(6, 2, "models/weapons/v_tesla/tris.md2", 0, 0, 0, 8,
		9, 32, 0, 0)
{
}

bool CTeslaWeapon::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 21:
		return true;
	}
	return false;
}

const int GRENADE_TIMER		= 30;
const int GRENADE_MINSPEED	= 400;
const int GRENADE_MAXSPEED	= 800;

void CTeslaWeapon::Hold (CPlayerEntity *Player)
{
	if (!Player->Client.Grenade.Time)
		Player->Client.Grenade.Time = Level.Frame + GRENADE_TIMER + 2;

	if (Player->Client.Buttons & BUTTON_ATTACK)
		return;

	Player->Client.PlayerState.GetGunFrame()++;
}

void CTeslaWeapon::FireGrenade (CPlayerEntity *Player, bool inHand)
{
	vec3f	offset (8, 8, Player->ViewHeight-8), start;

	Player->Client.Grenade.Thrown = true;

	anglef angles = Player->Client.ViewAngle.ToVectors();
	Player->P_ProjectSource (offset, angles, start);

	FrameNumber timer = (float)(Player->Client.Grenade.Time - Level.Frame) / 10;
	const sint32 speed = (Player->Client.Persistent.Weapon) ? 
		(GRENADE_MINSPEED + ((GRENADE_TIMER/10) - timer) * ((GRENADE_MAXSPEED - GRENADE_MINSPEED) / (GRENADE_TIMER/10)))
		: 25; // If we're dead, don't toss it 5 yards.
	CTesla::Spawn (Player, start, angles.Forward, DamageMultiplier, speed);

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

void CTeslaWeapon::Wait (CPlayerEntity *Player)
{
	Player->Client.Grenade.BlewUp = false;
	if (Level.Frame < Player->Client.Grenade.Time)
		return;

	if (!Player->IsDead)
		Player->Client.Grenade.Thrown = false;
	Player->Client.PlayerState.GetGunFrame()++;
}

void CTeslaWeapon::Fire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 1:
		Hold (Player);
		break;
	case 2:
		Player->Client.WeaponSound = 0;
		FireGrenade(Player, false);
		break;
	case 8:
		Wait(Player);
		break;
	}
}

bool CTeslaWeapon::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 1:
	case 2:
		return true;
	}
	return false;
}

void CTeslaWeapon::WeaponGeneric (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 2: case 3: case 4: case 5: case 6: case 7: case 8:
		Player->Client.PlayerState.GetGunIndex() = ModelIndex ("models/weapons/v_tesla2/tris.md2");
		break;
	default:
		Player->Client.PlayerState.GetGunIndex() = ModelIndex ("models/weapons/v_tesla/tris.md2");
		break;
	}

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

WEAPON_DEFS (CTeslaWeapon);

void CTeslaWeapon::CreateItem (CItemList *List)
{
};

#endif
