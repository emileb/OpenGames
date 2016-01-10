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
// cc_weaponmain.cpp
// Main system/class for the weapons
//

#include "Local.h"
#include "Weapons/WeaponMain.h"

typedef std::vector<IWeaponBase*> TWeaponListType;

TWeaponListType &WeaponList ()
{
	static TWeaponListType WeaponList_;
	return WeaponList_;
};

typedef std::pair<sint8, sint8> TWeaponMultiMapPairType;
typedef std::multimap<TWeaponMultiMapPairType, sint8> TWeaponMultiMapType;

inline void ChainWeapons (std::vector<IWeaponBase*> &Weapons)
{
	if (Weapons.size() == 1)
		Weapons[0]->PrevWeapon = Weapons[0]->NextWeapon = Weapons[0];
	else
	{
		for (size_t i = 0; i < Weapons.size(); ++i)
		{
			if (i < Weapons.size()-1)
				Weapons[i]->NextWeapon = Weapons[i+1];
			else
				Weapons[i]->NextWeapon = Weapons[0];

			if (i > 0)
				Weapons[i]->PrevWeapon = Weapons[i-1];
			else
				Weapons[i]->PrevWeapon = Weapons[Weapons.size()-1];
		}
	}
};

void AddWeapons (CItemList *List)
{
	// Add them in player-specified order
	// Index, Order
	TWeaponMultiMapType Order;

	for (size_t i = 0; i < WeaponList().size(); i++)
		Order.insert (std::make_pair (WeaponList()[i]->ListOrder, i));

	for (TWeaponMultiMapType::iterator it = Order.begin(); it != Order.end(); ++it)
		WeaponList()[(*it).second]->AddWeaponToItemList (List);

	std::vector<IWeaponBase*> WeaponOrder;

	sint32 lastIndex = -1;
	for (TWeaponMultiMapType::iterator it = Order.begin(); it != Order.end(); ++it)
	{
		IWeaponBase *Weap = WeaponList()[(*it).second];

		if (lastIndex == -1 || ((*it).first.first == lastIndex))
		{
			WeaponOrder.push_back (Weap);
			lastIndex = (*it).first.first;

			if (it != --Order.end())
				continue;
		}

		ChainWeapons (WeaponOrder);

		WeaponOrder.clear();
		WeaponOrder.push_back (Weap);
		lastIndex = (*it).first.first;

		if (it == --Order.end())
			ChainWeapons (WeaponOrder);
	}
}

void AddWeaponsToListLocations (CItemList *List)
{
	// Add them in player-specified order
	// Index, Order
	TWeaponMultiMapType Order;

	for (size_t i = 0; i < WeaponList().size(); i++)
		Order.insert (std::make_pair (WeaponList()[i]->ListOrder, i));

	for (TWeaponMultiMapType::iterator it = Order.begin(); it != Order.end(); ++it)
	{
		if (WeaponList()[(*it).second]->Item)
			List->Items.push_back (WeaponList()[(*it).second]->Item);
	}
}

void DoWeaponVweps ()
{
	sint32 TakeAway = ModelIndex(NItems::Blaster->VWepModel) - 1;

	for (size_t i = 0; i < WeaponList().size(); i++)
		WeaponList()[i]->InitWeaponVwepModel (TakeAway);
}

void SaveWeapon (CFile &File, IWeaponBase *Weapon)
{
	File.Write<sint32> ((Weapon) ? Weapon->Item->GetIndex() : -1);
}

void LoadWeapon (CFile &File, IWeaponBase **Weapon)
{
	sint32 Index = File.Read<sint32> ();

	if (Index != -1)
	{
		IBaseItem *Item = GetItemByIndex(Index);

		if (!CC_ASSERT_EXPR (Item, "Loaded weapon with no weapon!"))
		{
			CWeaponItem *WItem = dynamic_cast<CWeaponItem*>(Item);
			*Weapon = WItem->Weapon;
		}			
	}
}

IWeaponBase::IWeaponBase(sint8 ListOrderHigh, sint8 ListOrderLow, const char *model, uint16 ActivationStart, uint16 ActivationEnd, uint16 FireStart, uint16 FireEnd,
				 uint16 IdleStart, uint16 IdleEnd, uint16 DeactStart, uint16 DeactEnd, const char *WeaponSound) :
  ListOrder(std::make_pair (ListOrderHigh, ListOrderLow)),
  ActivationStart(ActivationStart),
  ActivationEnd(ActivationEnd),
  FireStart(FireStart),
  FireEnd(FireEnd),
  IdleStart(IdleStart),
  IdleEnd(IdleEnd),
  DeactStart(DeactStart),
  DeactEnd(DeactEnd),
  WeaponSound(WeaponSound),
  WeaponModelString(model)
{
	ActivationNumFrames = (ActivationEnd - ActivationStart);
	FireNumFrames = (FireEnd - FireStart);
	IdleNumFrames = (IdleEnd - IdleStart);
	DeactNumFrames = (DeactEnd - DeactStart);

	WeaponList().push_back (this);
};

void IWeaponBase::InitWeapon (CPlayerEntity *Player)
{
	Player->Client.PlayerState.GetGunFrame() = ActivationStart;
	Player->Client.PlayerState.GetGunIndex() = GetWeaponModel();
	Player->Client.WeaponState = WS_ACTIVATING;
}

void IWeaponBase::WeaponGeneric (CPlayerEntity *Player)
{
	// Idea from Brazen source
	sint32 newFrame = -1, newState = -1;

	switch (Player->Client.WeaponState)
	{
	case WS_ACTIVATING:
		if (Player->Client.PlayerState.GetGunFrame() == ActivationEnd
#if CLEANCTF_ENABLED
			|| CvarList[CV_INSTANT_WEAPONS].Boolean()
#endif
			)
		{
			newFrame = IdleStart;
			newState = WS_IDLE;
		}
		break;
	case WS_IDLE:
		if (Player->Client.NewWeapon && Player->Client.NewWeapon != this)
		{
#if CLEANCTF_ENABLED
			if (CvarList[CV_INSTANT_WEAPONS].Boolean())
			{
				ChangeWeapon (Player);
				return;
			}
#endif

			// We want to go away!
			newState = WS_DEACTIVATING;
			newFrame = DeactStart;
		}
		else if ((Player->Client.Buttons|Player->Client.LatchedButtons) & BUTTON_ATTACK)
		{
			Player->Client.LatchedButtons &= ~BUTTON_ATTACK;

			// This here is ugly, but necessary so that machinegun/chaingun/hyperblaster
			// get the right acceptance on first-frame-firing
			Player->Client.Buttons |= BUTTON_ATTACK;

			// We want to attack!
			// First call, check AttemptToFire
			if (AttemptToFire(Player))
			{
				// Got here, we can fire!
				Player->Client.PlayerState.GetGunFrame() = FireStart;
				Player->Client.WeaponState = WS_FIRING;

				// We need to check against us right away for first-frame firing
				// Paril: Fix for weapons who have extra checking in their WeaponGeneric.
				IWeaponBase::WeaponGeneric(Player);
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
		if (newFrame == -1 && Player->Client.PlayerState.GetGunFrame() > FireEnd)
		{
			newFrame = IdleStart+1;
			newState = WS_IDLE;
		}
		break;
	case WS_DEACTIVATING:
		if (Player->Client.PlayerState.GetGunFrame() == DeactEnd)
		{
			// Change weapon
			ChangeWeapon (Player);
			return;
		}
		break;
	}

	if (newFrame != -1)
		Player->Client.PlayerState.GetGunFrame() = newFrame;
	if (newState != -1)
		Player->Client.WeaponState = newState;

	if ((newFrame == -1) && (newState == -1))
		Player->Client.PlayerState.GetGunFrame()++;
}

inline sint32 GetWeaponVWepIndex (CPlayerEntity *Player)
{
	return (Player->State.GetNumber() - 1) | ((Player->Client.Persistent.Weapon->vwepIndex & 0xff) << 8);
}

void IWeaponBase::ChangeWeapon (CPlayerEntity *Player)
{
	Player->Client.Persistent.LastWeapon = Player->Client.Persistent.Weapon;
	Player->Client.Persistent.Weapon = Player->Client.NewWeapon;
	Player->Client.NewWeapon = NULL;
	Player->Client.Timers.MachinegunShots = 0;

	// set visible model
	if (Player->Client.Persistent.Weapon && Player->State.GetModelIndex() == 255)
		Player->State.GetSkinNum() = GetWeaponVWepIndex(Player);

	if (!Player->Client.Persistent.Weapon)
	{	// dead
		Player->Client.PlayerState.GetGunIndex() = 0;
		if (!Player->Client.Grenade.Thrown && !Player->Client.Grenade.BlewUp && Player->Client.Grenade.Time >= Level.Frame) // We had a grenade cocked
		{
#if !DROP_DEATH_GRENADES
			Player->Client.Grenade.Time = Level.Frame;
#endif
			CHandGrenade::Weapon.FireGrenade(Player, false);
			Player->Client.Grenade.Time = 0;
		}
		return;
	}

	Player->Client.Persistent.Weapon->InitWeapon(Player);

	Player->Client.Anim.Priority = ANIM_PAIN;
	if (Player->Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED)
	{
		Player->State.GetFrame() = CPlayerEntity::FRAME_crpain1;
		Player->Client.Anim.EndFrame = CPlayerEntity::FRAME_crpain4;
	}
	else
	{
		Player->State.GetFrame() = CPlayerEntity::FRAME_pain301;
		Player->Client.Anim.EndFrame = CPlayerEntity::FRAME_pain304;
	}
}

void IWeaponBase::DepleteAmmo (CPlayerEntity *Player, sint32 Amount = 1)
{
	if (DeathmatchFlags.dfInfiniteAmmo.IsEnabled())
		return;

	if (Item)
	{
		CAmmo *Ammo = Item->Ammo;

		if (Ammo)
			Player->Client.Persistent.Inventory.Remove (Ammo, Amount);
	}
}

bool IWeaponBase::AttemptToFire (CPlayerEntity *Player)
{
	sint32 numAmmo = 0;
	sint32 quantity = 0;

	if (Item->Ammo)
	{
		numAmmo = Player->Client.Persistent.Inventory.Has(Item->Ammo);
		quantity = Item->Amount;
	}
	else
		return true;

	return !(numAmmo < quantity);
}

void IWeaponBase::OutOfAmmo (CPlayerEntity *Player)
{
	// Doesn't affect pain anymore!
	if (Level.Frame >= Player->DamageDebounceTime)
	{
		Player->PlaySound (CHAN_AUTO, SoundIndex("weapons/noammo.wav"));
		Player->DamageDebounceTime = Level.Frame + 10;
	}
}

#include "Utility/TemporaryEntities.h"

void IWeaponBase::Muzzle (CPlayerEntity *Player, sint32 muzzleNum)
{
	if (isSilenced)
		muzzleNum |= MZ_SILENCED;

	CMuzzleFlash(Player->State.GetOrigin(), Player->State.GetNumber(), muzzleNum).Send();
}

/*
=================
Think_Weapon

Called by ClientBeginServerFrame
=================
*/
void IWeaponBase::Think (CPlayerEntity *Player)
{
#if CLEANCTF_ENABLED
	if ((Game.GameMode & GAME_CTF) && !Player->Client.Respawn.CTF.Team)
		return;
#endif

	// if just died, put the weapon away
	if (Player->Health < 1)
	{
		Player->Client.NewWeapon = NULL;
		ChangeWeapon (Player);
		return;
	}

	// call active weapon think routine
	isQuad = (Player->Client.Timers.QuadDamage > Level.Frame);

	if (isQuad)
		DamageMultiplier = 4;
	else
		DamageMultiplier = 1;

#if ROGUE_FEATURES
	isDouble = (Player->Client.Timers.Double > Level.Frame);

	if (isDouble && ((DamageMultiplier == 4 && !DeathmatchFlags.dfNoStackDouble.IsEnabled()) || DamageMultiplier == 1))
		DamageMultiplier *= 2;
#endif

#if XATRIX_FEATURES
	isQuadFire = (Player->Client.Timers.QuadFire > Level.Frame);
#endif
	isSilenced = (Player->Client.Timers.SilencerShots) ? true : false;
	WeaponGeneric (Player);

#if XATRIX_FEATURES
	bool Applied = false;
#endif

	if (DeathmatchFlags.dfDmTechs.IsEnabled()
#if CLEANCTF_ENABLED
		|| (Game.GameMode & GAME_CTF)
#endif
	)
	{
		if (
#if CLEANCTF_ENABLED
			this != &CGrapple::Weapon && 
#endif
			Player->ApplyHaste())
		{
			WeaponGeneric(Player);
#if XATRIX_FEATURES
			Applied = true;
#endif
		}
	}

#if XATRIX_FEATURES
	if (!Applied && isQuadFire)
		WeaponGeneric(Player);
#endif
}

void IWeaponBase::AttackSound(CPlayerEntity *Player)
{
	if (
#if CLEANCTF_ENABLED
		(Game.GameMode & GAME_CTF) || 
#endif
		DeathmatchFlags.dfDmTechs.IsEnabled())
	{
		Player->ApplyHasteSound();

		if (!Player->ApplyStrengthSound())
		{	
			if (isQuad)
				Player->PlaySound (CHAN_ITEM, SoundIndex("items/damage3.wav"));
#if ROGUE_FEATURES
			else if (isDouble)
				Player->PlaySound (CHAN_ITEM, SoundIndex("misc/ddamage3.wav"));
#endif
		}
	}
	else if (isQuad)
		Player->PlaySound (CHAN_ITEM, SoundIndex("items/damage3.wav"));
#if ROGUE_FEATURES
	else if (isDouble)
		Player->PlaySound (CHAN_ITEM, SoundIndex("misc/ddamage3.wav"));
#endif
}

class CWeaponSwitcher
{
public:
	IWeaponBase					*Weapon;
	std::vector<CAmmo*>		NeededAmmo;
	std::vector<sint32>		NeededAmmoNumbers;
	std::vector<IBaseItem*>	NeededItems;
	bool					Explosive;

	CWeaponSwitcher (IWeaponBase *Weapon) :
	  Weapon (Weapon),
	  Explosive(false)
	{
	};

	CWeaponSwitcher &AddAmmo (CAmmo *Ammo, sint32 Amount)
	{
		NeededAmmo.push_back (Ammo);
		NeededAmmoNumbers.push_back (Amount);
		return *this;
	};

	CWeaponSwitcher &AddItem (IBaseItem *Item)
	{
		NeededItems.push_back (Item);
		return *this;
	};

	CWeaponSwitcher &SwitchExplosive ()
	{
		Explosive = !Explosive;
		return *this;
	};
};

typedef std::vector <CWeaponSwitcher> TWeaponSwitcherListType;

#if XATRIX_FEATURES
#include "Xatrix/XatrixIonRipper.h"
#include "Xatrix/XatrixPhalanx.h"
#endif
#if ROGUE_FEATURES
#include "Rogue/RogueProxLauncher.h"
#endif

inline TWeaponSwitcherListType &WeaponSwitchList ()
{
	// Ordered by priority.
	// Same weapon can appear multiple times.
	static TWeaponSwitcherListType List_;

	List_.push_back	(CWeaponSwitcher(&CBFG::Weapon).AddAmmo(NItems::Cells, 50).SwitchExplosive());
	List_.push_back	(CWeaponSwitcher(&CRailgun::Weapon).AddAmmo(NItems::Slugs, 1));
#if XATRIX_FEATURES
	List_.push_back	(CWeaponSwitcher(&CIonRipper::Weapon).AddAmmo(NItems::Cells, 40));
#endif
	List_.push_back	(CWeaponSwitcher(&CHyperBlaster::Weapon).AddAmmo(NItems::Cells, 20));
	List_.push_back	(CWeaponSwitcher(&CRocketLauncher::Weapon).AddAmmo(NItems::Rockets, 1).SwitchExplosive());
#if XATRIX_FEATURES
	List_.push_back	(CWeaponSwitcher(&CPhalanx::Weapon).AddAmmo(NItems::MagSlugs, 1).SwitchExplosive());
#endif
	List_.push_back	(CWeaponSwitcher(&CGrenadeLauncher::Weapon).AddAmmo(NItems::Grenades, 1).SwitchExplosive());
	List_.push_back	(CWeaponSwitcher(&CChaingun::Weapon).AddAmmo(NItems::Bullets, 50));
#if XATRIX_FEATURES
	List_.push_back	(CWeaponSwitcher(&CIonRipper::Weapon).AddAmmo(NItems::Cells, 2));
#endif
	List_.push_back	(CWeaponSwitcher(&CHyperBlaster::Weapon).AddAmmo(NItems::Cells, 1));
	List_.push_back	(CWeaponSwitcher(&CMachinegun::Weapon).AddAmmo(NItems::Bullets, 1));
	List_.push_back	(CWeaponSwitcher(&CSuperShotgun::Weapon).AddAmmo(NItems::Shells, 8));
	List_.push_back	(CWeaponSwitcher(&CShotgun::Weapon).AddAmmo(NItems::Shells, 1));
	List_.push_back	(CWeaponSwitcher(&CSuperShotgun::Weapon).AddAmmo (NItems::Shells, 1));
	List_.push_back	(CWeaponSwitcher(&CHandGrenade::Weapon).AddAmmo (NItems::Grenades, 1));
	List_.push_back	(CWeaponSwitcher(&CBlaster::Weapon));

	return List_;
}

void IWeaponBase::NoAmmoWeaponChange (CPlayerEntity *Player)
{
	// Dead?
	if (!Player->Client.Persistent.Weapon || Player->Health <= 0 || Player->IsDead)
		return;

	// Collect info on our current state
	bool AvoidExplosive = (Player->Health <= 20);

	CWeaponItem	*Chosen_Weapon = NULL;

	static TWeaponSwitcherListType &SwitchList = WeaponSwitchList();
	for (size_t i = 0; i < SwitchList.size(); i++)
	{
		CWeaponSwitcher *Start = &SwitchList[i];

		// Explosive and almost dead?
		if (AvoidExplosive && Start->Explosive)
			continue;

		// Do we have this weapon?
		if (!Player->Client.Persistent.Inventory.Has(Start->Weapon->Item))
			continue;

		// Do we have the ammo?
		bool BreakIt = false;
		for (size_t z = 0; z < Start->NeededAmmo.size(); z++)
		{
			if (!Player->Client.Persistent.Inventory.Has(Start->NeededAmmo[z]))
			{
				BreakIt = true;
				break;
			}

			// Ammo amounts?
			if (Player->Client.Persistent.Inventory.Has(Start->NeededAmmo[z]) < Start->NeededAmmoNumbers[z])
			{
				BreakIt = true;
				break;
			}
		}

		if (BreakIt)
			continue;

		// Do we have all the extra items?
		for (size_t z = 0; Start->NeededItems.size(); z++)
		{
			if (!Player->Client.Persistent.Inventory.Has(Start->NeededItems[z]))
			{
				BreakIt = true;
				break;
			}
		}

		if (BreakIt)
			continue;

		// Use it
		Chosen_Weapon = Start->Weapon->Item;
		break;
	}

	if (!Chosen_Weapon)
		Chosen_Weapon = NItems::Blaster;

	if (!Chosen_Weapon)
		return;

	Player->Client.NewWeapon = Chosen_Weapon->Weapon;

	// Do a quick check to see if we still even have the weapon we're holding.
	if ((Player->Client.Persistent.Weapon->Item && !Player->Client.Persistent.Inventory.Has(Player->Client.Persistent.Weapon->Item)))
		Player->Client.Persistent.Weapon->ChangeWeapon(Player);
}

void IWeaponBase::Use (CWeaponItem *Wanted, CPlayerEntity *Player)
{
	bool UsingItOrChain = !Player->Client.Persistent.Inventory.Has(Wanted);

	while (!UsingItOrChain)
	{
		Wanted = Wanted->Weapon->GetNextWeapon()->Item;

		if (Player->Client.Persistent.Weapon == Wanted->Weapon)
		{
			UsingItOrChain = true;
			break;
		}

		if (Wanted->Weapon == this)
			break;
	};

	// see if we're already using it
	if (UsingItOrChain)
	{
		if (!(GetNextWeapon() == this && GetPrevWeapon() == this))
		{
			while (true)
			{
				Wanted = Wanted->Weapon->GetNextWeapon()->Item;

				if (Wanted->Weapon == this)
					break; // nothing

				if (!Player->Client.Persistent.Inventory.Has(Wanted))
					continue;

				if (Wanted->Ammo && !CvarList[CV_SELECT_EMPTY].Integer() && !(Wanted->Flags & ITEMFLAG_AMMO))
				{
					if (!Player->Client.Persistent.Inventory.Has(Wanted->Ammo->GetIndex()))
						continue;

					if (Player->Client.Persistent.Inventory.Has(Wanted->Ammo->GetIndex()) < Wanted->Amount)
						continue;
				}

				break;
			};
		}
	}

	if (Player->Client.Persistent.Weapon == Wanted->Weapon)
		return;

	if (!Player->Client.Persistent.Inventory.Has(Wanted))
	{
		Player->PrintToClient (PRINT_HIGH, "Out of Item: %s.\n", Wanted->Name);
		return;
	}

	if (Wanted->Ammo && !CvarList[CV_SELECT_EMPTY].Integer() && !(Wanted->Flags & ITEMFLAG_AMMO))
	{
		if (!Player->Client.Persistent.Inventory.Has(Wanted->Ammo->GetIndex()))
		{
			Player->PrintToClient (PRINT_HIGH, "No %s for %s.\n", Wanted->Ammo->Name, Wanted->Name);
			return;
		}

		if (Player->Client.Persistent.Inventory.Has(Wanted->Ammo->GetIndex()) < Wanted->Amount)
		{
			Player->PrintToClient (PRINT_HIGH, "Not enough %s for %s.\n", Wanted->Ammo->Name, Wanted->Name);
			return;
		}
	}

	// change to this weapon when down
	Player->Client.NewWeapon = Wanted->Weapon;
}

void IWeaponBase::FireAnimation (CPlayerEntity *Player)
{
	// start the animation
	Player->Client.Anim.Priority = ANIM_ATTACK;
	if (Player->Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED)
	{
		Player->State.GetFrame() = CPlayerEntity::FRAME_crattak1 - 1;
		Player->Client.Anim.EndFrame = CPlayerEntity::FRAME_crattak9;
	}
	else
	{
		Player->State.GetFrame() = CPlayerEntity::FRAME_attack1 - 1;
		Player->Client.Anim.EndFrame = CPlayerEntity::FRAME_attack8;
	}
}
