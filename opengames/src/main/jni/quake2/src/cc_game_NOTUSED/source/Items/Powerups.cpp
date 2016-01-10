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
// cc_powerups.cpp
// Powerups
//

#include "Local.h"
#include "Items/Items.h"

IBasePowerUp::IBasePowerUp(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBaseItem(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags),
PowerupFlags(PowerupFlags)
{
};

// Powerups!
bool IBasePowerUp::Pickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	if (PowerupFlags & POWERFLAG_STORE)
	{
		if (Other->Client.Persistent.Inventory.Has(this) > 0 &&
			(!(PowerupFlags & POWERFLAG_STACK) ||
			(PowerupFlags & (POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP) && (Game.GameMode & GAME_COOPERATIVE)) ||
			(Game.GameMode & GAME_COOPERATIVE) && (Flags & ITEMFLAG_STAY_COOP)))
			return false;

		Other->Client.Persistent.Inventory += this;
	}

	DoPickup (Item, Other);
	return true;
}

void IBasePowerUp::DoPickup (class CItemEntity *Item, CPlayerEntity *Other)
{
}

void IBasePowerUp::Use (CPlayerEntity *Player)
{
}

void IBasePowerUp::Drop (CPlayerEntity *Player)
{
	if (PowerupFlags & POWERFLAG_STORE)
	{	
		DropItem (Player);
		Player->Client.Persistent.Inventory -= this;
	}
}

CMegaHealth::CMegaHealth(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

CBackPack::CBackPack(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

CQuadDamage::CQuadDamage(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

CInvulnerability::CInvulnerability(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

CSilencer::CSilencer(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

CRebreather::CRebreather(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

CEnvironmentSuit::CEnvironmentSuit(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

CBandolier::CBandolier(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

CAdrenaline::CAdrenaline(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

CAncientHead::CAncientHead(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

CPowerShield::CPowerShield(const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags) :
IBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, PowerupFlags)
{
};

class CMegaHealthEntity : public CItemEntity
{
public:
	bool			MegaHealthThinking;
	CPlayerEntity	*Player;

	CMegaHealthEntity () :
	  IBaseEntity(),
	  CItemEntity(),
	  Player(NULL)
	  {
		  MegaHealthThinking = false;
	  };

	CMegaHealthEntity (sint32 Index) :
	  IBaseEntity (Index),
	  CItemEntity(Index),
	  Player(NULL)
	  {
		  MegaHealthThinking = false;
	  };

	void Think ()
	{
		if (MegaHealthThinking)
		{
			if (Player->Health > Player->MaxHealth
		
			&& ((
#if CLEANCTF_ENABLED
			!(Game.GameMode & GAME_CTF) && 
#endif
			!DeathmatchFlags.dfDmTechs.IsEnabled()) || !Player->HasRegeneration())
				)
			{
				NextThink = Level.Frame + 10;
				Player->Health -= 1;
				return;
			}

			if (!(SpawnFlags & DROPPED_ITEM) && (Game.GameMode & GAME_DEATHMATCH))
				LinkedItem->SetRespawn (this, 200);
			else
				Free ();
			MegaHealthThinking = false;
		}
		else
			CItemEntity::Think ();
	};

	void Spawn (IBaseItem *item)
	{
		if ((Game.GameMode & GAME_DEATHMATCH) && (DeathmatchFlags.dfNoHealth.IsEnabled() || DeathmatchFlags.dfNoItems.IsEnabled()))
		{
			Free ();
			return;
		}

		LinkedItem = item;
		NextThink = Level.Frame + 2;    // items start after other solids
		ThinkState = ITS_DROPTOFLOOR;
		PhysicsType = PHYSICS_NONE;

		State.GetEffects() = item->EffectFlags;
		State.GetRenderEffects() = RF_GLOW | RF_IR_VISIBLE;
	};
};


// Seperate powerup classes
void CMegaHealth::DoPickup (CItemEntity *Item, CPlayerEntity *Other)
{
	CMegaHealthEntity *MegaHealth = entity_cast<CMegaHealthEntity>(Item);

	if ((!DeathmatchFlags.dfDmTechs.IsEnabled()
#if CLEANCTF_ENABLED
		&& !(Game.GameMode & GAME_CTF)
#endif
		)|| ((
#if CLEANCTF_ENABLED
		(Game.GameMode & GAME_CTF) || 
#endif
		DeathmatchFlags.dfDmTechs.IsEnabled()) && !Other->HasRegeneration()))
	{
		MegaHealth->MegaHealthThinking = true;
		MegaHealth->NextThink = Level.Frame + 50;
		MegaHealth->Player = Other;
		MegaHealth->ShouldRespawn = true;
		MegaHealth->GetSvFlags() |= SVF_NOCLIENT;
		MegaHealth->GetSolid() = SOLID_NOT;

		Other->Health += 100;
	}
#if CLEANCTF_ENABLED
	else if (!(MegaHealth->SpawnFlags & DROPPED_ITEM) && (Game.GameMode & GAME_DEATHMATCH))
		MegaHealth->LinkedItem->SetRespawn (Item, 300);
#endif
}

void CBackPack::DoPickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	// Increase their max ammo, if applicable
	for (sint32 i = 0; i < AMMOTAG_MAX; i++)
	{
		if (Other->Client.Persistent.MaxAmmoValues[i] < maxBackpackAmmoValues[i])
			Other->Client.Persistent.MaxAmmoValues[i] = maxBackpackAmmoValues[i];
	}

	// Give them some more ammo
	NItems::Bullets->AddAmmo (Other, NItems::Bullets->Quantity);
	NItems::Shells->AddAmmo (Other, NItems::Shells->Quantity);
	NItems::Grenades->AddAmmo (Other, NItems::Grenades->Quantity);
	NItems::Cells->AddAmmo (Other, NItems::Cells->Quantity);
	NItems::Slugs->AddAmmo (Other, NItems::Slugs->Quantity);
	NItems::Rockets->AddAmmo (Other, NItems::Rockets->Quantity);

	if (!(Item->SpawnFlags & DROPPED_ITEM) && (Game.GameMode & GAME_DEATHMATCH))
		SetRespawn (Item, 1800);
}

static sint32	quad_drop_timeout_hack;

void CQuadDamage::DoPickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		if (!(Item->SpawnFlags & DROPPED_ITEM) )
			SetRespawn (Item, 600);
		if (Item->SpawnFlags & DROPPED_PLAYER_ITEM)
			quad_drop_timeout_hack = (Item->NextThink - Level.Frame);

		if (DeathmatchFlags.dfInstantItems.IsEnabled())
			Use (Other);
	}
}

void CQuadDamage::Use (CPlayerEntity *Player)
{
	sint32 timeOut = 300;

	if (quad_drop_timeout_hack)
	{
		timeOut = quad_drop_timeout_hack;
		quad_drop_timeout_hack = 0;
	}

	if (Player->Client.Timers.QuadDamage > Level.Frame)
		Player->Client.Timers.QuadDamage += timeOut;
	else
		Player->Client.Timers.QuadDamage = Level.Frame + timeOut;

	Player->Client.Persistent.Inventory -= this;

	Player->PlaySound (CHAN_ITEM, SoundIndex("items/damage.wav"));
}

void CInvulnerability::DoPickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		if (!(Item->SpawnFlags & DROPPED_ITEM) )
			SetRespawn (Item, 300);
		if (DeathmatchFlags.dfInstantItems.IsEnabled() || (Item->SpawnFlags & DROPPED_PLAYER_ITEM))
			Use (Other);
	}
}

void CInvulnerability::Use (CPlayerEntity *Player)
{
	Player->Client.Persistent.Inventory -= this;

	if (Player->Client.Timers.Invincibility > Level.Frame)
		Player->Client.Timers.Invincibility += 300;
	else
		Player->Client.Timers.Invincibility = Level.Frame + 300;

	Player->PlaySound (CHAN_ITEM, SoundIndex("items/protect.wav"));
}

void CSilencer::DoPickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		if (!(Item->SpawnFlags & DROPPED_ITEM) )
			SetRespawn (Item, 300);
		if (DeathmatchFlags.dfInstantItems.IsEnabled() || (Item->SpawnFlags & DROPPED_PLAYER_ITEM))
			Use (Other);
	}
}

void CSilencer::Use (CPlayerEntity *Player)
{
	Player->Client.Persistent.Inventory -= this;
	Player->Client.Timers.SilencerShots += 30;
}

void CRebreather::DoPickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		if (!(Item->SpawnFlags & DROPPED_ITEM) )
			SetRespawn (Item, 600);
		if (DeathmatchFlags.dfInstantItems.IsEnabled() || (Item->SpawnFlags & DROPPED_PLAYER_ITEM))
			Use (Other);
	}
}

void CRebreather::Use (CPlayerEntity *Player)
{
	Player->Client.Persistent.Inventory -= this;

	if (Player->Client.Timers.Rebreather > Level.Frame)
		Player->Client.Timers.Rebreather += 300;
	else
		Player->Client.Timers.Rebreather = Level.Frame + 300;
}

void CEnvironmentSuit::DoPickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		if (!(Item->SpawnFlags & DROPPED_ITEM) )
			SetRespawn (Item, 600);
		if (DeathmatchFlags.dfInstantItems.IsEnabled() || (Item->SpawnFlags & DROPPED_PLAYER_ITEM))
			Use (Other);
	}
}

void CEnvironmentSuit::Use (CPlayerEntity *Player)
{
	Player->Client.Persistent.Inventory -= this;

	if (Player->Client.Timers.EnvironmentSuit > Level.Frame)
		Player->Client.Timers.EnvironmentSuit += 300;
	else
		Player->Client.Timers.EnvironmentSuit = Level.Frame + 300;
}

void CBandolier::DoPickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	// Increase their max ammo, if applicable
	for (sint32 i = 0; i < AMMOTAG_MAX; i++)
	{
		if (Other->Client.Persistent.MaxAmmoValues[i] < maxBandolierAmmoValues[i])
			Other->Client.Persistent.MaxAmmoValues[i] = maxBandolierAmmoValues[i];
	}

	// Give them some more ammo
	NItems::Bullets->AddAmmo (Other, NItems::Bullets->Quantity);
	NItems::Shells->AddAmmo (Other, NItems::Shells->Quantity);

	if (!(Item->SpawnFlags & DROPPED_ITEM) && (Game.GameMode & GAME_DEATHMATCH))
		SetRespawn (Item, 600);
}

void CAdrenaline::DoPickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	if (!(Game.GameMode & GAME_DEATHMATCH))
		Other->MaxHealth += 1;

	if (Other->Health < Other->MaxHealth)
		Other->Health = Other->MaxHealth;

	if (!(Item->SpawnFlags & DROPPED_ITEM) && (Game.GameMode & GAME_DEATHMATCH))
		SetRespawn (Item, 600);
}

void CAncientHead::DoPickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	Other->MaxHealth += 2;

	if (!(Item->SpawnFlags & DROPPED_ITEM) && (Game.GameMode & GAME_DEATHMATCH))
		SetRespawn (Item, 600);
}

void CPowerShield::DoPickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		if (!(Item->SpawnFlags & DROPPED_ITEM) )
			SetRespawn (Item, 600);

		// auto-use for DM only if we didn't already have one
		if (!Other->Client.Persistent.Inventory.Has(this))
			Use (Other);
	}
}

void CPowerShield::Use (CPlayerEntity *Player)
{
	if (Player->Client.PowerArmorEnabled)
	{
		Player->Client.PowerArmorEnabled = false;
		Player->PlaySound (CHAN_AUTO, SoundIndex("misc/power2.wav"));
	}
	else
	{
		if (!Player->Client.Persistent.Inventory.Has(NItems::Cells))
		{
			Player->PrintToClient (PRINT_HIGH, "No cells for %s.\n", Name);
			return;
		}
		Player->Client.PowerArmorEnabled = true;
		Player->PlaySound (CHAN_AUTO, SoundIndex("misc/power1.wav"));
	}
}

void CPowerShield::Drop (CPlayerEntity *Player)
{
	if ((Player->Client.PowerArmorEnabled) && (Player->Client.Persistent.Inventory.Has(this) == 1))
		Use (Player);
	IBasePowerUp::Drop (Player);
}

CPowerupEntity::CPowerupEntity() :
  IBaseEntity(),
  CItemEntity ()
  {
  };

CPowerupEntity::CPowerupEntity (sint32 Index) :
  IBaseEntity(Index),
  CItemEntity (Index)
  {
  };

void CPowerupEntity::Spawn (IBaseItem *item)
{
	if ((Game.GameMode & GAME_DEATHMATCH) && DeathmatchFlags.dfNoItems.IsEnabled())
	{
		Free ();
		return;
	}

	LinkedItem = item;
	NextThink = Level.Frame + 2;    // items start after other solids
	ThinkState = ITS_DROPTOFLOOR;
	PhysicsType = PHYSICS_NONE;

	State.GetEffects() = item->EffectFlags;
	State.GetRenderEffects() = RF_GLOW | RF_IR_VISIBLE;
};

LINK_ITEM_TO_CLASS (item_health_mega, CMegaHealthEntity);
LINK_ITEM_TO_CLASS (item_pack, CPowerupEntity);
LINK_ITEM_TO_CLASS (item_quad, CPowerupEntity);
LINK_ITEM_TO_CLASS (item_invulnerability, CPowerupEntity);
LINK_ITEM_TO_CLASS (item_silencer, CPowerupEntity);
LINK_ITEM_TO_CLASS (item_breather, CPowerupEntity);
LINK_ITEM_TO_CLASS (item_enviro, CPowerupEntity);
LINK_ITEM_TO_CLASS (item_bandolier, CPowerupEntity);
LINK_ITEM_TO_CLASS (item_adrenaline, CPowerupEntity);
LINK_ITEM_TO_CLASS (item_ancient_head, CPowerupEntity);
LINK_ITEM_TO_CLASS (item_power_shield, CPowerupEntity);
LINK_ITEM_TO_CLASS (item_power_screen, CPowerupEntity);

void AddPowerupsToList ()
{
	NItems::MegaHealth = QNew (TAG_GENERIC) CMegaHealth("item_health_mega", "models/items/mega_h/tris.md2", 0, "items/m_health.wav", "i_health", "MegaHealth", ITEMFLAG_HEALTH|ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE, 0);
	NItems::BackPack = QNew (TAG_GENERIC) CBackPack ("item_pack", "models/items/pack/tris.md2", FX_ROTATE, "items/pkup.wav", "i_pack", "Ammo Pack", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE, POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	NItems::Quad = QNew (TAG_GENERIC) CQuadDamage ("item_quad", "models/items/quaddama/tris.md2", FX_ROTATE, "items/pkup.wav", "p_quad", "Quad Damage", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	NItems::Invul = QNew (TAG_GENERIC) CInvulnerability ("item_invulnerability", "models/items/invulner/tris.md2", FX_ROTATE, "items/pkup.wav", "p_invulnerability", "Invulnerability", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	NItems::Silencer = QNew (TAG_GENERIC) CSilencer ("item_silencer", "models/items/silencer/tris.md2", FX_ROTATE, "items/pkup.wav", "p_silencer", "Silencer", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	NItems::Rebreather = QNew (TAG_GENERIC) CRebreather ("item_breather", "models/items/breather/tris.md2", FX_ROTATE, "items/pkup.wav", "p_rebreather", "Rebreather", ITEMFLAG_POWERUP|ITEMFLAG_STAY_COOP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	NItems::EnvironmentSuit = QNew (TAG_GENERIC) CEnvironmentSuit ("item_enviro", "models/items/enviro/tris.md2", FX_ROTATE, "items/pkup.wav", "p_envirosuit", "Environment Suit", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	NItems::Bandolier = QNew (TAG_GENERIC) CBandolier ("item_bandolier", "models/items/band/tris.md2", FX_ROTATE, "items/pkup.wav", "p_bandolier", "Bandolier", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE, POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	NItems::Adrenaline = QNew (TAG_GENERIC) CAdrenaline ("item_adrenaline", "models/items/adrenal/tris.md2", FX_ROTATE, "items/pkup.wav", "p_adrenaline", "Adrenaline", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE, 0);
	NItems::AncientHead = QNew (TAG_GENERIC) CAncientHead ("item_ancient_head", "models/items/c_head/tris.md2", FX_ROTATE, "items/pkup.wav", "i_fixme", "Ancient Head", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE, 0);
	NItems::PowerShield = QNew (TAG_GENERIC) CPowerShield ("item_power_shield", "models/items/armor/shield/tris.md2", FX_ROTATE, "misc/ar3_pkup.wav", "i_powershield", "Power Shield", ITEMFLAG_ARMOR|ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_USABLE|ITEMFLAG_DROPPABLE, POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	NItems::PowerScreen = QNew (TAG_GENERIC) CPowerShield ("item_power_screen", "models/items/armor/screen/tris.md2", FX_ROTATE, "misc/ar3_pkup.wav", "i_powerscreen", "Power Screen", ITEMFLAG_ARMOR|ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_USABLE|ITEMFLAG_DROPPABLE, POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
}
