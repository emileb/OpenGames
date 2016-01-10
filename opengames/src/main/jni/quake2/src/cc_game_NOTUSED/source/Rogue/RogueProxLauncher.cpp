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
// cc_rogue_prox_launcher.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES

#include "Weapons/WeaponMain.h"
#include "Utility/TemporaryEntities.h"
#include "Rogue/RogueProxLauncher.h"

// Prox
class CProxField : public ITouchableEntity
{
public:
	class CProx			*Prox;

	CProxField () :
	  ITouchableEntity ()
	  {
	  };

	CProxField (sint32 Index) :
	  IBaseEntity (Index),
	  ITouchableEntity (Index)
	  {
	  };

	void SaveFields (CFile &File);
	void LoadFields (CFile &File);

	IMPLEMENT_SAVE_HEADER(CProxField);

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);
};

const int PROX_TIME_TO_LIVE		= 450;		// 450, 300, 150, 100
const int PROX_TIME_DELAY		= 5;
const int PROX_BOUND_SIZE		= 96;
const int PROX_DAMAGE_RADIUS	= 192;
const int PROX_HEALTH			= 20;
const int PROX_DAMAGE			= 90;
const float STOP_EPSILON		= 0.1f;

class CProx : public IBounceProjectile, public IHurtableEntity, public ITouchableEntity, public IThinkableEntity
{
public:
	/**
	\typedef	uint8 EProxThinkType
	
	\brief	Defines an alias representing think state of the proximity mine.
	**/
	typedef uint8 EProxThinkType;

	/**
	\enum	
	
	\brief	Values that represent the think state of a prox mine. 
	**/
	enum
	{
		PROXTHINK_NONE,

		PROXTHINK_SEEK,
		PROXTHINK_OPEN,
		PROXTHINK_EXPLODE,
	};

	EProxThinkType		ThinkType;
	CProxField			*Field;
	CPlayerEntity		*Firer;
	int					Damage;
	FrameNumber		Wait;
	
	CProx () :
	  IBounceProjectile (),
	  ITouchableEntity (),
	  IHurtableEntity (),
	  IThinkableEntity ()
	  {
	  };

	CProx (sint32 Index) :
	  IBaseEntity (Index),
	  IBounceProjectile (Index),
	  ITouchableEntity (Index),
	  IHurtableEntity (Index),
	  IThinkableEntity (Index)
	  {
	  };

	void SaveFields (CFile &File)
	{
		IBounceProjectile::SaveFields (File);
		ITouchableEntity::SaveFields (File);
		IHurtableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);

		File.Write<EProxThinkType> (ThinkType);
		File.Write<sint32> ((Field != NULL && Field->GetInUse()) ? Field->State.GetNumber() : -1);
		File.Write<sint32> ((Firer != NULL && Firer->GetInUse()) ? Firer->State.GetNumber() : -1);
		File.Write<int> (Damage);
		File.Write<FrameNumber> (Wait);
	};

	void LoadFields (CFile &File)
	{
		IBounceProjectile::LoadFields (File);
		ITouchableEntity::LoadFields (File);
		IHurtableEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);

		ThinkType = File.Read<EProxThinkType> ();

		sint32 index = File.Read<sint32>();
		if (index != -1)
			Field = entity_cast<CProxField>(Game.Entities[index].Entity);

		index = File.Read<sint32>();
		if (index != -1)
			Firer = entity_cast<CPlayerEntity>(Game.Entities[index].Entity);

		Damage = File.Read<int> ();
		Wait = File.Read<FrameNumber> ();
	};

	IMPLEMENT_SAVE_HEADER(CProx);

	bool Run ()
	{
		return (PhysicsType == PHYSICS_BOUNCE) ? IBounceProjectile::Run() : false;
	}

	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
	{
		// if set off by another prox, delay a little (chained explosions)
		if (Inflictor->ClassName == "prox")
			Explode ();
		else
		{
			ThinkType = PROXTHINK_EXPLODE;
			NextThink = Level.Frame + ServerFramesPerSecond;
		}

		CanTakeDamage = false;
	}

	void Explode ()
	{
		// free the trigger field
		//PMM - changed teammaster to "mover" .. owner of the field is the prox
		if (Field && Field->GetOwner() == this)
			Field->Free ();

		IBaseEntity *Owner = this;
		if (Firer)
		{
			Owner = Firer;
			Firer->PlayerNoiseAt (State.GetOrigin(), PNOISE_IMPACT);
		}

		// play quad sound if appopriate
		if (Damage > PROX_DAMAGE)
			PlaySound (CHAN_ITEM, SoundIndex("items/damage3.wav"));

		CanTakeDamage = false;
		SplashDamage (Owner, Damage, this, PROX_DAMAGE_RADIUS, MOD_PROX);

		if (GroundEntity)
			CGrenadeExplosion (State.GetOrigin().MultiplyAngles (-0.02f, Velocity)).Send();
		else
			CRocketExplosion (State.GetOrigin().MultiplyAngles (-0.02f, Velocity)).Send();

		Free ();
	}

	void Seek ()
	{
		if (Level.Frame > Wait)
			Explode ();
		else
		{
			if(++State.GetFrame() > 13)
				State.GetFrame() = 9;
			
			ThinkType = PROXTHINK_SEEK;
			NextThink = Level.Frame + ServerFramesPerSecond;
		}
	}

	void Open ()
	{
		IBaseEntity *search = NULL;

		if (State.GetFrame() == 9)	// end of opening animation
		{
			// set the owner to NULL so the owner can shoot it, etc.  needs to be done here so the owner
			// doesn't get stuck on it while it's opening if fired at point blank wall
			State.GetSound() = 0;
			SetOwner(NULL);

			if (Field)
				Field->Touchable = true;

			while ((search = FindRadius<EF_BASE> (search, State.GetOrigin(), PROX_DAMAGE_RADIUS+10)) != NULL)
			{
				if (search->ClassName.IsNullOrEmpty())			// tag token and other weird shit
					continue;
				if (search->ClassName == "prox" ||
					search->ClassName == "prox_field" ||
					search->ClassName == "tesla")
					continue;

				// if it's a monster or player with health > 0
				// or it's a player start point
				// and we can see it
				// blow up
				if (
					(
						(((search->EntityFlags & EF_MONSTER) || (search->EntityFlags & EF_PLAYER)) && (entity_cast<IHurtableEntity>(search)->Health > 0))	|| 
						(
							(Game.GameMode & GAME_DEATHMATCH) && 
							(
							(!strcmp(search->ClassName.CString(), "info_player_deathmatch")) ||
							(!strcmp(search->ClassName.CString(), "info_player_start")) ||
							(!strcmp(search->ClassName.CString(), "info_player_coop")) ||
							(!strcmp(search->ClassName.CString(), "misc_teleporter_dest"))
							)
						)
					) 
					&& (IsVisible (search, this))
				   )
				{
					PlaySound (CHAN_VOICE, SoundIndex("weapons/proxwarn.wav"));
					Explode ();
					return;
				}
			}

			//if (strong_mines && (strong_mines->value))
			//	ent->wait = level.time + PROX_TIME_TO_LIVE;
			//else
			{
				switch (Damage/PROX_DAMAGE)
				{
					case 1:
						Wait = Level.Frame + PROX_TIME_TO_LIVE;
						break;
					case 2:
						Wait = Level.Frame + 300;
						break;
					case 4:
						Wait = Level.Frame + 150;
						break;
					case 8:
						Wait = Level.Frame + 100;
						break;
					default:
						Wait = Level.Frame + PROX_TIME_TO_LIVE;
						break;
				}
			}

			ThinkType = PROXTHINK_SEEK;
			NextThink = Level.Frame + 2;
		}
		else
		{
			if (State.GetFrame() == 0)
					PlaySound (CHAN_VOICE, SoundIndex("weapons/proxopen.wav"));

			State.GetFrame()++;
			ThinkType = PROXTHINK_OPEN;
			NextThink = Level.Frame + 1;	
		}
	}

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
		if (surf && (surf->Flags & SURF_TEXINFO_SKY))
		{
			Free ();
			return;
		}

		if (plane->Normal)
		{
			if (PointContents (State.GetOrigin().MultiplyAngles (-10.0f, plane->Normal)) & (CONTENTS_SLIME|CONTENTS_LAVA))
			{
				Explode ();
				return;
			}
		}

		bool DoneStick = true;

		if (Other->EntityFlags & EF_HURTABLE)
		{
			if (Other != Firer)
				Explode ();
			return;
		}
		else if (Other != World)
		{
			bool StickOK = false;
			//Here we need to check to see if we can stop on this entity.
			//Note that plane can be NULL

			//PMM - code stolen from g_phys (ClipVelocity)
			if (!plane->Normal) // this happens if you hit a point object, maybe other cases
			{
				Explode ();
				return;
			}

			if ((Other->EntityFlags & EF_PHYSICS) && (entity_cast<IPhysicsEntity>(Other)->PhysicsType == PHYSICS_PUSH) && (plane->Normal.Z > 0.7f))
				StickOK = true;

			float backoff = (Velocity | plane->Normal) * 1.5f;
			vec3f out;

			for (int i = 0; i < 3; i++)
			{
				float change = plane->Normal[i]*backoff;
				out[i] = Velocity[i] - change;
				if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
					out[i] = 0;
			}

			if (out[2] > 60)
				return;

			DoneStick = false;

			// if we're here, we're going to stop on an entity
			if (!StickOK) // no-stick.  teflon time
			{
				if (plane->Normal.Z > 0.7)
				{
					Explode();
					return;
				}
				return;
			}
		}
		else if (Other->State.GetModelIndex() != 1)
			return;

		if (PointContents (State.GetOrigin()) & (CONTENTS_LAVA|CONTENTS_SLIME))
		{
			Explode ();
			return;
		}

		vec3f dir = plane->Normal.ToAngles();

		CProxField *ProxField = QNewEntityOf CProxField;

		ProxField->State.GetOrigin() = State.GetOrigin();
		ProxField->GetMins().Set (-PROX_BOUND_SIZE);
		ProxField->GetMaxs().Set (PROX_BOUND_SIZE);
		ProxField->GetSolid() = SOLID_TRIGGER;
		ProxField->SetOwner(this);
		ProxField->ClassName = "prox_field";
		ProxField->Prox = this;
		ProxField->Link ();

		Velocity.Clear ();
		AngularVelocity.Clear ();

		// rotate to vertical
		dir.X += 90;
		State.GetAngles() = dir;
		CanTakeDamage = true;
		PhysicsType = (DoneStick) ? PHYSICS_NONE : PHYSICS_BOUNCE;		// either bounce or none, depending on whether we stuck to something
		Field = ProxField;
		Health = PROX_HEALTH;
		NextThink = Level.Frame + 1;
		ThinkType = PROXTHINK_OPEN;
		Touchable = false;
		GetSolid() = SOLID_BBOX;

		Link ();
	}

	void Think ()
	{
		switch (ThinkType)
		{
		case PROXTHINK_SEEK:
			Seek ();
			break;
		case PROXTHINK_OPEN:
			Open ();
			break;
		case PROXTHINK_EXPLODE:
			Explode ();
			break;
		};
	}

	static void Spawn (CPlayerEntity *Player, vec3f Start, vec3f AimDir, int DamageMultiplier, int Speed)
	{
		vec3f	dir = AimDir.ToAngles();

		anglef angles = dir.ToVectors ();
		
		CProx	*Prox = QNewEntityOf CProx;
		Prox->State.GetOrigin() = Start;
		Prox->Velocity =	(AimDir * Speed)
							.MultiplyAngles (200 + crand() * 10.0f, angles.Up)
							.MultiplyAngles (crand() * 10.0f, angles.Right);
		Prox->State.GetAngles() = dir - vec3f(90, 0, 0);
		Prox->PhysicsType = PHYSICS_BOUNCE;
		Prox->GetSolid() = SOLID_BBOX; 
		Prox->State.GetEffects() |= FX_GRENADE;
		Prox->GetClipmask() = CONTENTS_MASK_SHOT|CONTENTS_LAVA|CONTENTS_SLIME;
		Prox->State.GetRenderEffects() |= RF_IR_VISIBLE;
		
		Prox->GetMins().Set (-6, -6, -6);
		Prox->GetMaxs().Set (6, 6, 6);

		Prox->State.GetModelIndex() = ModelIndex ("models/weapons/g_prox/tris.md2");
		Prox->SetOwner(Player);
		Prox->Firer = Player;
		Prox->Touchable = true;
		Prox->ThinkType = PROXTHINK_EXPLODE;
		Prox->Damage = PROX_DAMAGE * DamageMultiplier;
		Prox->ClassName = "prox";

		switch (DamageMultiplier)
		{
		case 1:
			Prox->NextThink = Level.Frame + PROX_TIME_TO_LIVE;
			break;
		case 2:
			Prox->NextThink = Level.Frame + 300;
			break;
		case 4:
			Prox->NextThink = Level.Frame + 150;
			break;
		case 8:
			Prox->NextThink = Level.Frame + 100;
			break;
		default:
			Prox->NextThink = Level.Frame + PROX_TIME_TO_LIVE;
			break;
		}

		Prox->Link ();
	}
};

void CProxField::SaveFields (CFile &File)
{
	ITouchableEntity::SaveFields (File);

	File.Write<sint32> ((Prox != NULL && Prox->GetInUse()) ? Prox->State.GetNumber() : -1);
};

void CProxField::LoadFields (CFile &File)
{
	ITouchableEntity::LoadFields (File);

	sint32 index = File.Read<sint32>();
	if (index != -1)
		Prox = entity_cast<CProx>(Game.Entities[index].Entity);
};

void CProxField::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	if (!(Other->EntityFlags & EF_HURTABLE))
		return;

	// trigger the prox mine if it's still there, and still mine.
	if (Other == Prox) // don't set self off
		return;

	if (Prox->ThinkType != CProx::PROXTHINK_SEEK) // we're set to blow!
		return;

	if (Other->ClassName == "prox")
		return;

	if (Prox->Field == this)
	{
		PlaySound (CHAN_VOICE, SoundIndex ("weapons/proxwarn.wav"));
		Prox->ThinkType = CProx::PROXTHINK_EXPLODE;
		Prox->NextThink = Level.Frame + PROX_TIME_DELAY;
		return;
	}

	GetSolid() = SOLID_NOT;
	Free ();
}

IMPLEMENT_SAVE_SOURCE (CProx);
IMPLEMENT_SAVE_SOURCE (CProxField);

// The launcher
CProxLauncher::CProxLauncher() :
IWeaponBase(5, 1, "models/weapons/v_plaunch/tris.md2", 0, 5, 6, 16,
		17, 59, 60, 64)
{
}

bool CProxLauncher::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 6:
		return true;
	}
	return false;
}

bool CProxLauncher::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 34:
	case 51:
	case 59:
		return true;
	}
	return false;
}

void CProxLauncher::Fire (CPlayerEntity *Player)
{
	vec3f	offset (8, 8, Player->ViewHeight-8), start;

	FireAnimation (Player);

	anglef angles = Player->Client.ViewAngle.ToVectors();
	Player->P_ProjectSource (offset, angles, start);

	Player->Client.KickOrigin = angles.Forward * -2;
	Player->Client.KickAngles.X = -1;

	CProx::Spawn (Player, start, angles.Forward, DamageMultiplier, 600);

	Muzzle (Player, MZ_GRENADE);
	AttackSound (Player);

	Player->PlayerNoiseAt (start, PNOISE_WEAPON);
	DepleteAmmo(Player, 1);

	Player->Client.PlayerState.GetGunFrame()++;
}

WEAPON_DEFS (CProxLauncher);

LINK_ITEM_TO_CLASS (weapon_proxlauncher, CItemEntity);

void CProxLauncher::CreateItem (CItemList *List)
{
	QNew (TAG_GENERIC) CWeaponItem
		("weapon_proxlauncher", "models/weapons/g_plaunch/tris.md2", FX_ROTATE, "misc/w_pkup.wav", "w_proxlaunch", "Prox Launcher",
		ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, &Weapon,
		NItems::Prox, 1, "#w_plauncher.md2");
};

#endif
