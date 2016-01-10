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
// cc_rogue_items.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES

#include "Entities/TargetEntities.h"
#include "Utility/TemporaryEntities.h"
#include "Weapons/WeaponMain.h"
#include "Rogue/RogueTesla.h"

CDoubleDamage::CDoubleDamage(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

void CDoubleDamage::DoPickup (CItemEntity *Item, CPlayerEntity *Other)
{
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		if (!(Item->SpawnFlags & DROPPED_ITEM) )
			SetRespawn (Item, 600);

		if (DeathmatchFlags.dfInstantItems.IsEnabled())
			Use (Other);
	}
}

void CDoubleDamage::Use (CPlayerEntity *Player)
{
	if (Player->Client.Timers.Double > Level.Frame)
		Player->Client.Timers.Double += 300;
	else
		Player->Client.Timers.Double = Level.Frame + 300;

	Player->Client.Persistent.Inventory -= this;

	Player->PlaySound (CHAN_ITEM, SoundIndex("misc/ddamage1.wav"));
}

CIRGoggles::CIRGoggles(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

void CIRGoggles::DoPickup (CItemEntity *Item, CPlayerEntity *Other)
{
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		if (!(Item->SpawnFlags & DROPPED_ITEM) )
			SetRespawn (Item, 600);

		if (DeathmatchFlags.dfInstantItems.IsEnabled())
			Use (Other);
	}
}

void CIRGoggles::Use (CPlayerEntity *Player)
{
	if (Player->Client.Timers.IR > Level.Frame)
		Player->Client.Timers.IR += 600;
	else
		Player->Client.Timers.IR = Level.Frame + 600;

	Player->Client.Persistent.Inventory -= this;

	Player->PlaySound (CHAN_ITEM, SoundIndex("misc/ir_start.wav"));
}

// Nuke
const int NUKE_DELAY			= 40;
const int NUKE_TIME_TO_LIVE		= 60;
const int NUKE_RADIUS			= 512;
const int NUKE_DAMAGE			= 400;
const int NUKE_QUAKE_TIME		= 30;
const int NUKE_QUAKE_STRENGTH	= 100;

class CNukeEntity : public IBounceProjectile, public IThinkableEntity, public ITouchableEntity, public IHurtableEntity
{
public:
	/**
	\typedef	uint8 ENukeThinkType
	
	\brief	Defines an alias representing think state of the nuke.
	**/
	typedef uint8 ENukeThinkType;

	/**
	\enum	
	
	\brief	Values that represent the think state of the nuke. 
	**/
	enum
	{
		NUKETHINK_NONE,
		NUKETHINK_QUAKE,
		NUKETHINK_SPIN,
	};

	ENukeThinkType		ThinkType;
	FrameNumber		LastQuakeTime, QuakeTime, Wait;
	MediaIndex			NoiseIndex;
	CPlayerEntity		*ThrowerPlayer;
	IBaseEntity			*Thrower;
	int					Damage, DamageRadius;

	CNukeEntity () :
	  IBounceProjectile (),
	  IThinkableEntity (),
	  ITouchableEntity (),
	  IHurtableEntity ()
	  {
	  };

	CNukeEntity (sint32 Index) :
	  IBaseEntity (Index),
	  IBounceProjectile (Index),
	  IThinkableEntity (Index),
	  ITouchableEntity (Index),
	  IHurtableEntity (Index)
	  {
	  };

	void SaveFields (CFile &File)
	{
		IBounceProjectile::SaveFields (File);
		IThinkableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
		IHurtableEntity::SaveFields (File);

		File.Write<ENukeThinkType> (ThinkType);
		File.Write<FrameNumber> (LastQuakeTime);
		File.Write<FrameNumber> (QuakeTime);
		File.Write<FrameNumber> (Wait);
		File.Write<MediaIndex> (NoiseIndex);
		File.Write<sint32> ((ThrowerPlayer != NULL && ThrowerPlayer->GetInUse()) ? ThrowerPlayer->State.GetNumber() : -1);
		File.Write<sint32> ((Thrower != NULL && Thrower->GetInUse()) ? Thrower->State.GetNumber() : -1);
		File.Write<int> (Damage);
		File.Write<int> (DamageRadius);
	};

	void LoadFields (CFile &File)
	{
		IBounceProjectile::LoadFields (File);
		IThinkableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
		IHurtableEntity::LoadFields (File);

		ThinkType = File.Read<ENukeThinkType> ();

		ThinkType = File.Read<ENukeThinkType> ();
		LastQuakeTime = File.Read<FrameNumber> ();
		QuakeTime = File.Read<FrameNumber> ();
		Wait = File.Read<FrameNumber> ();
		NoiseIndex = File.Read<MediaIndex> ();

		sint32 index = File.Read<sint32>();
		if (index != -1)
			ThrowerPlayer = entity_cast<CPlayerEntity>(Game.Entities[index].Entity);

		index = File.Read<sint32>();
		if (index != -1)
			Thrower = Game.Entities[index].Entity;

		Damage = File.Read<int> ();
		DamageRadius = File.Read<int> ();
	};

	IMPLEMENT_SAVE_HEADER(CNukeEntity);

	bool Run ()
	{
		return IBounceProjectile::Run();
	}

	void Quake ()
	{
		if (LastQuakeTime < Level.Frame)
		{
			PlaySoundAt (State.GetOrigin(), this, CHAN_AUTO, NoiseIndex, 191, ATTN_NONE);
			LastQuakeTime = Level.Frame + 5;
		}

		CEarthQuakeShakePlayers(NUKE_QUAKE_STRENGTH).Query();

		if (Level.Frame < QuakeTime)
			NextThink = Level.Frame + ServerFramesPerSecond;
		else
			Free ();
	}

	void Explode ()
	{
		if (ThrowerPlayer)
			ThrowerPlayer->PlayerNoiseAt (State.GetOrigin(), PNOISE_IMPACT);

		NukeSplashDamage (Thrower, Damage, this, DamageRadius, MOD_NUKE);

		if (Damage > NUKE_DAMAGE)
			PlaySound (CHAN_AUTO, SoundIndex("items/damage3.wav"));

		PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundIndex ("weapons/grenlx1a.wav"));

		CRocketExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_PVS, State.GetOrigin()), State.GetOrigin()).Send();
		CNukeExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, State.GetOrigin()), State.GetOrigin()).Send();

		// become a quake
		GetSvFlags() |= SVF_NOCLIENT;
		NoiseIndex = SoundIndex ("world/rumble.wav");
		ThinkType = NUKETHINK_QUAKE;
		QuakeTime = Level.Frame + NUKE_QUAKE_TIME;
		NextThink = Level.Frame + ServerFramesPerSecond;
		LastQuakeTime = 0;
	}

	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
	{
		CanTakeDamage = false;

		if (Attacker && !(strcmp(Attacker->ClassName.CString(), "nuke")))
		{
			Free ();	
			return;
		}

		Explode ();
	}

	void Spin ()
	{
		float default_atten = 1.8f, attenuation = default_atten;
		EMuzzleFlash		muzzleflash = MZ_NUKE1;

		int damage_multiplier = Damage / NUKE_DAMAGE;
		switch (damage_multiplier)
		{
		case 1:
			attenuation = default_atten/1.4;
			muzzleflash = MZ_NUKE1;
			break;
		case 2:
			attenuation = default_atten/2.0;
			muzzleflash = MZ_NUKE2;
			break;
		case 4:
			attenuation = default_atten/3.0;
			muzzleflash = MZ_NUKE4;
			break;
		case 8:
			attenuation = default_atten/5.0;
			muzzleflash = MZ_NUKE8;
			break;
		}

		if (Wait < Level.Frame)
			Explode ();
		else if (Level.Frame >= (Wait - NUKE_TIME_TO_LIVE))
		{
			if (++State.GetFrame() > 11)
				State.GetFrame() = 6;

			if (PointContents (State.GetOrigin()) & (CONTENTS_SLIME|CONTENTS_LAVA))
			{
				Explode ();
				return;
			}

			ThinkType = NUKETHINK_SPIN;
			NextThink = Level.Frame + ServerFramesPerSecond;
			Health = 1;
			SetOwner(NULL);

			CMuzzleFlash (State.GetOrigin(), State.GetNumber(), MZ_NUKE4);

			if (QuakeTime <= Level.Frame)
			{
				QuakeTime = Level.Frame + (((Wait - Level.Frame) <= (NUKE_TIME_TO_LIVE/2.0)) ? 3 : 5);
				PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundIndex ("weapons/nukewarn2.wav"), 255, attenuation);
			}
		}
		else
		{
			if (QuakeTime <= Level.Frame)
			{
				PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundIndex ("weapons/nukewarn2.wav"), 255, attenuation);
				QuakeTime = Level.Frame + 10;
			}
			NextThink = Level.Frame + ServerFramesPerSecond;
		}
	}

	void Think ()
	{
		switch (ThinkType)
		{
		case NUKETHINK_QUAKE:
			Quake ();
			break;
		case NUKETHINK_SPIN:
			Spin ();
			break;
		};
	}

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
		PlaySound (CHAN_VOICE, SoundIndex (frand() > 0.5f ? "weapons/hgrenb1a.wav" : "weapons/hgrenb2a.wav"));
	}

	static void Spawn (IBaseEntity *Entity, vec3f Start, vec3f AimDir, int Speed)
	{
		int DamModifier = 1;
		anglef angles = AimDir.ToAngles().ToVectors ();

		CNukeEntity *Nuke = QNewEntityOf CNukeEntity;

		if (Entity->EntityFlags & EF_PLAYER)
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(Entity);
			
			if (Player->Client.Persistent.Weapon)
				DamModifier = Player->Client.Persistent.Weapon->DamageMultiplier;
		
			Nuke->ThrowerPlayer = Player;
		}

		Nuke->State.GetOrigin() = Start;
		Nuke->Velocity =	(AimDir * Speed)
							.MultiplyAngles (200 + crand() * 10.0f, angles.Up)
							.MultiplyAngles (crand() * 10.0f, angles.Right);

		Nuke->AngularVelocity.Clear();
		Nuke->State.GetAngles().Clear();
		Nuke->GetClipmask() = CONTENTS_MASK_SHOT;
		Nuke->GetSolid() = SOLID_BBOX;
		Nuke->State.GetEffects() |= FX_GRENADE;
		Nuke->State.GetRenderEffects() |= RF_IR_VISIBLE;
		Nuke->GetMins().Set (-8, -8, 0);
		Nuke->GetMaxs().Set (8, 8, 16);
		Nuke->State.GetModelIndex() = ModelIndex ("models/weapons/g_nuke/tris.md2");
		Nuke->SetOwner(Entity);
		Nuke->Thrower = Entity;
		Nuke->NextThink = Level.Frame + ServerFramesPerSecond;
		Nuke->Wait = Level.Frame + NUKE_DELAY + NUKE_TIME_TO_LIVE;
		Nuke->ThinkType = NUKETHINK_SPIN;
		Nuke->Touchable = true;

		Nuke->Health = 10000;
		Nuke->CanTakeDamage = true;
		Nuke->Damage = NUKE_DAMAGE * DamModifier;
		if (DamModifier == 1)
			Nuke->DamageRadius = NUKE_RADIUS;
		else
			Nuke->DamageRadius = NUKE_RADIUS + NUKE_RADIUS*(0.25*(float)DamModifier);

		Nuke->ClassName = "nuke";

		Nuke->Link ();
	}
};

IMPLEMENT_SAVE_SOURCE (CNukeEntity);

CAMBomb::CAMBomb (const char *Classname, const char *WorldModel, const char *Icon, const char *Name) :
IBaseItem (Classname, WorldModel, 0, "misc/am_pkup.wav", Icon, Name, ITEMFLAG_DROPPABLE|ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_USABLE),
CAmmo (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, 1, 0)
{
}

bool	CAMBomb::Pickup (CItemEntity *Item, CPlayerEntity *Other)
{
	int		quantity = Other->Client.Persistent.Inventory.Has (this);

	if (quantity >= 1)
		return false;

	if ((Game.GameMode & GAME_COOPERATIVE) && (Flags & ITEMFLAG_STAY_COOP) && (quantity > 0))
		return false;

	Other->Client.Persistent.Inventory += this;

	if (Game.GameMode & GAME_DEATHMATCH)
	{
		if (!(Item->SpawnFlags & DROPPED_ITEM) )
			SetRespawn (Item, 300);
	}

	return true;
}

void	CAMBomb::Use (CPlayerEntity *Player)
{
	Player->Client.Persistent.Inventory -= this;
	Player->Client.Persistent.Inventory.ValidateSelectedItem ();

	CNukeEntity::Spawn (Player, Player->State.GetOrigin(), Player->Client.ViewAngle.ToVectors().Forward, 100);
}

void	CAMBomb::Drop (CPlayerEntity *Player)
{
	sint32 count = Quantity;
	CItemEntity *dropped = DropItem(Player);

	if (count > Player->Client.Persistent.Inventory.Has(this))
		count = Player->Client.Persistent.Inventory.Has(this);

	dropped->AmmoCount = count;

	Player->Client.Persistent.Inventory.Remove (this, count);
}

#include "Rogue/RogueMonsterSpawning.h"

class CDoppleGangerItem : public IBasePowerUp
{
public:
	CDoppleGangerItem (const char *Classname, const char *WorldModel, const char *Icon, const char *Name) :
	  IBasePowerUp(Classname, WorldModel, FX_ROTATE, "items/pkup.wav", Icon, Name, ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, POWERFLAG_STORE)
	  {
	  };

	void DoPickup (CItemEntity *Item, CPlayerEntity *Other)
	{
		if (!(Game.GameMode & GAME_DEATHMATCH))		// item is DM only
			return;

		if (Other->Client.Persistent.Inventory.Has(this) >= 1)
			return;

		Other->Client.Persistent.Inventory += this;

		if (!(Item->SpawnFlags & DROPPED_ITEM) )
			SetRespawn (Item, 900);

		return;
	};

	void Use (CPlayerEntity *Player)
	{
		anglef angles = vec3f(0, Player->Client.ViewAngle.Y, 0).ToVectors ();

		vec3f createPt = Player->State.GetOrigin().MultiplyAngles (48, angles.Forward);

		vec3f spawnPt;
		if (!FindSpawnPoint(createPt, Player->GetMins(), Player->GetMaxs(), spawnPt, 32))
			return;

		if (!CheckGroundSpawnPoint(spawnPt, Player->GetMins(), Player->GetMaxs(), 64, -1))
			return;

		Player->Client.Persistent.Inventory -= this;
		Player->Client.Persistent.Inventory.ValidateSelectedItem ();

		CSpawnGrow::Spawn (spawnPt, 0);
		CDoppleGanger::Spawn (Player, spawnPt, angles.Forward);
	};
};

template <class TSphereType, FrameNumber RespawnTime>
class CSphereItem : public IBasePowerUp
{
public:
	CSphereItem (const char *Classname, const char *WorldModel, const char *Icon, const char *Name) :
	  IBasePowerUp (Classname, WorldModel, FX_ROTATE, "items/pkup.wav", Icon, Name, ITEMFLAG_GRABBABLE|ITEMFLAG_POWERUP|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, POWERFLAG_STORE)
	{
	};

	void DoPickup (CItemEntity *Item, CPlayerEntity *Other)
	{
		if (Other->Client.OwnedSphere)
			return;

		int quantity = Other->Client.Persistent.Inventory.Has(this);
		if ((CvarList[CV_SKILL].Integer() == 1 && quantity >= 2) || (CvarList[CV_SKILL].Integer() >= 2 && quantity >= 1))
			return;

		if ((Game.GameMode & GAME_COOPERATIVE) && (Flags & ITEMFLAG_STAY_COOP) && (quantity > 0))
			return;

		Other->Client.Persistent.Inventory += this;

		if (Game.GameMode & GAME_DEATHMATCH)
		{
			if (!(Item->SpawnFlags & DROPPED_ITEM) )
				SetRespawn (Item, RespawnTime);
			if (DeathmatchFlags.dfInstantItems.IsEnabled())
				Use (Other);
		}

		return;
	};

	void Use (CPlayerEntity *Player)
	{
		if (Player->Client.OwnedSphere)
		{
			Player->PrintToClient (PRINT_HIGH, "Only one sphere at a time!\n");
			return;
		}

		Player->Client.Persistent.Inventory -= this;
		Player->Client.Persistent.Inventory.ValidateSelectedItem ();

		TSphereType::Create (this, Player, 0);
	};
};

LINK_ITEM_TO_CLASS (item_double, CItemEntity);
LINK_ITEM_TO_CLASS (item_ir_goggles, CItemEntity);

LINK_ITEM_TO_CLASS (ammo_prox, CAmmoEntity);
LINK_ITEM_TO_CLASS (ammo_nuke, CItemEntity);
LINK_ITEM_TO_CLASS (ammo_flechettes, CItemEntity);
LINK_EXISTING_ITEM_TO_NEW_CLASS (ammo_nails, "ammo_flechettes", CItemEntity);
LINK_ITEM_TO_CLASS (ammo_tesla, CItemEntity);
LINK_ITEM_TO_CLASS (ammo_disruptor, CItemEntity);

LINK_ITEM_TO_CLASS (key_nuke_container, CItemEntity);
LINK_ITEM_TO_CLASS (key_nuke, CItemEntity);

LINK_ITEM_TO_CLASS (item_sphere_vengeance, CItemEntity);
LINK_ITEM_TO_CLASS (item_sphere_hunter, CItemEntity);
LINK_ITEM_TO_CLASS (item_sphere_defender, CItemEntity);
LINK_ITEM_TO_CLASS (item_doppleganger, CItemEntity);

void AddRogueItemsToList ()
{
	NItems::Double = QNew (TAG_GENERIC) CDoubleDamage ("item_double", "models/items/ddamage/tris.md2", FX_ROTATE, "items/pkup.wav", "p_double", "Double Damage", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	NItems::IRGoggles = QNew (TAG_GENERIC) CIRGoggles ("item_ir_goggles", "models/items/goggles/tris.md2", FX_ROTATE, "items/pkup.wav", "p_ir", "IR Goggles", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);

	NItems::Prox = QNew (TAG_GENERIC) CAmmo("ammo_prox", "models/ammo/am_prox/tris.md2", "a_prox", "Prox", 5, AMMOTAG_PROX);
	NItems::Flechettes = QNew (TAG_GENERIC) CAmmo("ammo_flechettes", "models/ammo/am_flechette/tris.md2", "a_flechettes", "Flechettes", 50, AMMOTAG_FLECHETTES);
	NItems::Rounds = QNew (TAG_GENERIC) CAmmo("ammo_disruptor", "models/ammo/am_disr/tris.md2", "a_disruptor", "Rounds", 50, AMMOTAG_ROUNDS); 
	NItems::Tesla = QNew (TAG_GENERIC) CAmmoWeapon("ammo_tesla", "models/ammo/am_tesl/tris.md2", 0, "misc/am_pkup.wav", "a_tesla", "Tesla",
		ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_USABLE|ITEMFLAG_GRABBABLE|ITEMFLAG_WEAPON, &CTeslaWeapon::Weapon, 1, "#a_grenades.md2", 5, AMMOTAG_TESLA);

	QNew (TAG_GENERIC) CAMBomb("ammo_nuke", "models/weapons/g_nuke/tris.md2", "p_nuke", "A-M Bomb");

	QNew (TAG_GENERIC) CKey("key_nuke_container", "models/weapons/g_nuke/tris.md2", FX_ROTATE, "items/pkup.wav", "i_contain", "Antimatter Pod", ITEMFLAG_GRABBABLE|ITEMFLAG_KEY|ITEMFLAG_STAY_COOP);
	QNew (TAG_GENERIC) CKey("key_nuke", "models/weapons/g_nuke/tris.md2", FX_ROTATE, "items/pkup.wav", "i_nuke", "Antimatter Bomb", ITEMFLAG_GRABBABLE|ITEMFLAG_KEY|ITEMFLAG_STAY_COOP);

	QNew (TAG_GENERIC) CSphereItem<CRogueVengeanceSphere, 600> ("item_sphere_vengeance", "models/items/vengnce/tris.md2", "p_vengeance", "Vengeance Sphere");
	QNew (TAG_GENERIC) CSphereItem<CRogueHunterSphere, 1200> ("item_sphere_hunter", "models/items/hunter/tris.md2", "p_hunter", "Hunter Sphere");
	QNew (TAG_GENERIC) CSphereItem<CRogueDefenderSphere, 600> ("item_sphere_defender", "models/items/defender/tris.md2", "p_defender", "Defender Sphere");
	QNew (TAG_GENERIC) CDoppleGangerItem ("item_doppleganger", "models/items/dopple/tris.md2", "p_doppleganger", "Doppleganger");
}

#endif