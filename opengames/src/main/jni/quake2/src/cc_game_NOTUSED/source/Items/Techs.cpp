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
// cc_techs.cpp
// 
//

#include "Local.h"
#include "Weapons/WeaponMain.h"
#include "Entities/InfoEntities.h"

CTech::CTech (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   uint32 TechNumber, ETechType TechType) :
IBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags),
TechNumber(TechNumber),
TechType(TechType)
{
};

CTech::CTech (const char *Classname, const char *Model, const char *Image, const char *Name, CTech::ETechType TechType, uint32 TechNumber) :
IBaseItem (Classname, Model, FX_ROTATE, "items/pkup.wav", Image, Name, ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_TECH),
TechNumber(TechNumber),
TechType(TechType)
{
};
/*------------------------------------------------------------------------*/
/* TECH																	  */
/*------------------------------------------------------------------------*/


bool CTech::Pickup (class CItemEntity *Item, CPlayerEntity *Other)
{
	if (Other->Client.Persistent.Tech)
	{
		if (Level.Frame - Other->Client.Tech.LastTechMessage > 20)
		{
			Other->PrintToClient(PRINT_CENTER, "You already have a TECH powerup.");
			Other->Client.Tech.LastTechMessage = Level.Frame;
		}
		return false; // has this one
	}
	
	// client only gets one tech
	Other->Client.Persistent.Inventory.Set(this, 1);
	Other->Client.Persistent.Tech = this;
	Other->Client.Tech.RegenTime = Level.Frame;
	return true;
}

static CSpotBase *FindTechSpawn()
{
	return SelectRandomDeathmatchSpawnPoint();
}

class CResistanceTech : public CTech
{
public:
	CResistanceTech (const char *Classname, const char *Model, const char *Image, const char *Name, CTech::ETechType TechType, uint32 TechNumber) :
	CTech (Classname, Model, Image, Name, TechType, TechNumber)
	{
	};

	void DoAggressiveTech	(	CPlayerEntity *Left, IBaseEntity *Right, bool Calculated, sint32 &Damage, sint32 &Knockback, EDamageFlags &DamageFlags,
										EMeansOfDeath &Mod, bool Defending	)
	{
		if (!Calculated)
			return;

		if (Defending && Damage)
		{
			// make noise
			Left->PlaySound (CHAN_ITEM, SoundIndex("ctf/tech1.wav"), (Left->Client.Timers.SilencerShots) ? 51 : 255);
			Damage /= 2;
		}
	};
};

class CStrengthTech : public CTech
{
public:
	CStrengthTech (const char *Classname, const char *Model, const char *Image, const char *Name, CTech::ETechType TechType, uint32 TechNumber) :
	CTech (Classname, Model, Image, Name, TechType, TechNumber)
	{
	};

	void DoAggressiveTech	(	CPlayerEntity *Left, IBaseEntity *Right, bool Calculated, sint32 &Damage, sint32 &Knockback, EDamageFlags &DamageFlags,
										EMeansOfDeath &Mod, bool Defending	)
	{
		if (Calculated || (Left == Right))
			return;

		if (!Defending && Damage)
			Damage *= 2;
	};
};

class CRegenTech : public CTech
{
public:
	CRegenTech (const char *Classname, const char *Model, const char *Image, const char *Name, CTech::ETechType TechType, uint32 TechNumber) :
	CTech (Classname, Model, Image, Name, TechType, TechNumber)
	{
	};

	void DoPassiveTech	(CPlayerEntity *Player)
	{
		IBaseItem *index;
		bool noise = false;
		if (Player->Client.Tech.RegenTime < Level.Frame)
		{
			Player->Client.Tech.RegenTime = Level.Frame;
			if (Player->Health < 150)
			{
				Player->Health += 5;
				if (Player->Health > 150)
					Player->Health = 150;
				Player->Client.Tech.RegenTime += 5;
				noise = true;
			}
			index = Player->Client.Persistent.Armor;
			if (index && Player->Client.Persistent.Inventory.Has(index) < 150)
			{
				Player->Client.Persistent.Inventory.Add (index, 5);
				if (Player->Client.Persistent.Inventory.Has(index) > 150)
					Player->Client.Persistent.Inventory.Set(index, 150);
				Player->Client.Tech.RegenTime += 5;
				noise = true;
			}
		}
		if (noise && Player->Client.Tech.SoundTime < Level.Frame)
		{
			Player->Client.Tech.SoundTime = Level.Frame + 10;
			Player->PlaySound (CHAN_AUTO, SoundIndex("ctf/tech4.wav"), (Player->Client.Timers.SilencerShots) ? 51 : 255);
		}
	};
};

#if AMMO_REGEN_TECH

const int SHELL_REGEN_COUNT		= 3;
const int BULLET_REGEN_COUNT	= 6;
const int GRENADE_REGEN_COUNT	= 2;
const int CELL_REGEN_COUNT		= 3;
const int ROCKET_REGEN_COUNT	= 2;
const int SLUG_REGEN_COUNT		= 1;
const int AMMO_REGEN_TIME		= 17;

class CAmmoRegenTech : public CTech
{
public:
	CAmmoRegenTech (const char *Classname, const char *Model, const char *Image, const char *Name, CTech::ETechType TechType, uint32 TechNumber) :
	CTech (Classname, Model, Image, Name, TechType, TechNumber)
	{
	};

	void DoPassiveTech	(CPlayerEntity *Player)
	{
		const sint32 RegenAmts[] = {SHELL_REGEN_COUNT, BULLET_REGEN_COUNT, GRENADE_REGEN_COUNT, ROCKET_REGEN_COUNT, CELL_REGEN_COUNT, SLUG_REGEN_COUNT};

		bool noise = false;
		if (Player->Client.Tech.RegenTime < Level.Frame)
		{
			if (Player->Client.Persistent.Weapon)
			{
				if (Player->Client.Persistent.Weapon->Item)
				{
					CAmmo *Ammo = Player->Client.Persistent.Weapon->Item->Ammo;
	
					if (Ammo && Player->Client.Persistent.Inventory.Has(Ammo) && Player->Client.Persistent.Inventory.Has(Ammo) < Player->Client.Persistent.MaxAmmoValues[Ammo->Tag])
					{			
						noise = true;
						Ammo->AddAmmo (Player, RegenAmts[Ammo->Tag]);
					}
				}
			}

			Player->Client.Tech.RegenTime = Level.Frame + AMMO_REGEN_TIME;
		}
		if (noise && Player->Client.Tech.SoundTime < Level.Frame)
		{
			Player->Client.Tech.SoundTime = Level.Frame + AMMO_REGEN_TIME;
			Player->PlaySound (CHAN_AUTO, SoundIndex("ctf/tech5.wav"), (Player->Client.Timers.SilencerShots) ? 51 : 255);
		}
	};
};
#endif

std::vector<CTech*>		TechList;

void SpawnTech(IBaseItem *item, CSpotBase *spot);
class CTechEntity : public CItemEntity
{
public:
	bool AvoidOwner;

	CTechEntity() :
	    CItemEntity (),
	    AvoidOwner(true)
	  {
	  };

	CTechEntity (sint32 Index) :
	  IBaseEntity (Index),
	  CItemEntity(Index),
	  AvoidOwner(true)
	{
	};

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
		if (AvoidOwner && (Other == GetOwner()))
			return;

		CItemEntity::Touch (Other, plane, surf);
	};

	void Think ()
	{
		CSpotBase *spot;

		if ((spot = FindTechSpawn()) != NULL)
		{
			SpawnTech(LinkedItem, spot);
			Free ();
		}
		else
			NextThink = Level.Frame + CTF_TECH_TIMEOUT;
	};

	void Respawn ()
	{
		SpawnTech(LinkedItem, FindTechSpawn());
		Free ();
	};

	void BecomeExplosion (bool grenade)
	{
		Respawn(); // this frees self!
	};

	void Spawn ()
	{
		AvoidOwner = true;
	};
};

CItemEntity *CTech::DropItem (IBaseEntity *Entity)
{
	CTechEntity	*dropped = QNewEntityOf CTechEntity();

	dropped->ClassName = Classname;
	dropped->LinkedItem = this;
	dropped->SpawnFlags = DROPPED_ITEM;
	dropped->State.GetEffects() = EffectFlags;
	dropped->State.GetRenderEffects() = RF_GLOW | RF_IR_VISIBLE;
	dropped->GetMins().Set (-15);
	dropped->GetMaxs().Set (15);
	dropped->State.GetModelIndex() = ModelIndex(WorldModel);
	dropped->GetSolid() = SOLID_TRIGGER;
	dropped->SetOwner(Entity);

	anglef angles;

	if (Entity->EntityFlags & EF_PLAYER)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Entity);
		CTrace	trace;

		angles = Player->Client.ViewAngle.ToVectors ();
		vec3f offset (24, 0, -16);

		vec3f result;
		G_ProjectSource (Player->State.GetOrigin(), offset, angles, result);

		trace (Player->State.GetOrigin(), dropped->GetMins(), dropped->GetMaxs(),
			result, Player, CONTENTS_SOLID);
		dropped->State.GetOrigin() = trace.EndPosition;
	}
	else
	{
		anglef angles = Entity->State.GetAngles().ToVectors();
		dropped->State.GetOrigin() = Entity->State.GetOrigin();
	}

	angles.Forward *= 100;
	dropped->Velocity = angles.Forward;
	dropped->Velocity.Z = 300;

	dropped->NextThink = Level.Frame + 10;
	dropped->Link ();

	return dropped;
}

void CTech::Drop (CPlayerEntity *Player)
{
	CItemEntity *tech = DropItem(Player);
	tech->NextThink = Level.Frame + CTF_TECH_TIMEOUT;
	Player->Client.Persistent.Inventory.Set(this, 0);
	Player->Client.Persistent.Tech = NULL;
}

void SpawnTech(IBaseItem *Item, CSpotBase *Spot)
{
	CTechEntity *Tech = QNewEntityOf CTechEntity ();

	Tech->ClassName = Item->Classname;
	Tech->LinkedItem = Item;
	Tech->SpawnFlags = DROPPED_ITEM;
	Tech->State.GetEffects() = Item->EffectFlags;
	Tech->State.GetRenderEffects() = RF_GLOW | RF_IR_VISIBLE;
	Tech->GetMins().Set (-15);
	Tech->GetMaxs().Set (15);
	Tech->State.GetModelIndex() = ModelIndex(Item->WorldModel);
	Tech->GetSolid() = SOLID_TRIGGER;

	anglef angles = vec3f(0, frand()*360, 0).ToVectors();

	Tech->State.GetOrigin() = Spot->State.GetOrigin() + vec3f(0,0,16);
	angles.Forward *= 100;
	Tech->Velocity = angles.Forward;
	Tech->Velocity.Z = 300;

	Tech->NextThink = Level.Frame + CTF_TECH_TIMEOUT;

	Tech->Link ();
}

static void SpawnTechs()
{
	for (size_t i = 0; i < TechList.size(); i++)
	{
		if (!CvarList[CV_CC_TECHFLAGS].Integer() || (CvarList[CV_CC_TECHFLAGS].Integer() & BIT(TechList[i]->GetTechNumber())))
			SpawnTech (TechList[i], FindTechSpawn ());
	}
}

class CTechSpawner : public IThinkableEntity
{
public:
	CTechSpawner () :
	  IBaseEntity (-1),
	  IThinkableEntity ()
	{
		Spawn ();
	};

	bool Run ()
	{
		return false;
	};

	void Think ()
	{
		SpawnTechs ();
		Free ();
	};

	void Spawn ()
	{
		NextThink = Level.Frame + 20;
	};
};

void SetupTechSpawn()
{
	if (
#if CLEANCTF_ENABLED
		DeathmatchFlags.dfCtfNoTech.IsEnabled() || (!(Game.GameMode & GAME_CTF) && 
#endif
		(!DeathmatchFlags.dfDmTechs.IsEnabled())
		|| !(Game.GameMode & GAME_DEATHMATCH))
#if CLEANCTF_ENABLED
		)
#endif
		return;

	QNewEntityOf CTechSpawner;
}

void ResetTechs()
{
	for (TEntitiesContainer::iterator it = Level.Entities.Closed.begin()++; it != Level.Entities.Closed.end(); ++it)
	{
		IBaseEntity *Entity = (*it)->Entity;

		if (Entity && Entity->GetInUse() && (Entity->EntityFlags & EF_ITEM))
		{
			CItemEntity *Item = entity_cast<CItemEntity>(Entity);

			if (Item->Flags & ITEMFLAG_TECH)
				Item->Free ();
		}
	}
	SpawnTechs();
}

void	CTech::Use (CPlayerEntity *Player)
{
}

void AddTechsToList ()
{
	TechList.push_back (QNew (TAG_GENERIC) CResistanceTech ("item_tech1", "models/ctf/resistance/tris.md2",
														"tech1", "Disruptor Shield", CTech::TECH_AGGRESSIVE, CTFTECH_RESISTANCE_NUMBER));

	TechList.push_back (QNew (TAG_GENERIC) CStrengthTech ("item_tech2", "models/ctf/strength/tris.md2",
														"tech2", "Power Amplifier", CTech::TECH_AGGRESSIVE, CTFTECH_STRENGTH_NUMBER));

	TechList.push_back (QNew (TAG_GENERIC) CTech ("item_tech3", "models/ctf/haste/tris.md2",
														"tech3", "Time Accel", CTech::TECH_CUSTOM, CTFTECH_HASTE_NUMBER));

	TechList.push_back (QNew (TAG_GENERIC) CRegenTech ("item_tech4", "models/ctf/regeneration/tris.md2",
														"tech4", "AutoDoc", CTech::TECH_PASSIVE, CTFTECH_REGEN_NUMBER));

#if AMMO_REGEN_TECH
	TechList.push_back (QNew (TAG_GENERIC) CAmmoRegenTech ("item_tech5", "models/ctf/ammo/tris.md2",
														"tech5", "Ammo Regen", CTech::TECH_PASSIVE, CTFTECH_AMMOREGEN_NUMBER));
#endif
}
