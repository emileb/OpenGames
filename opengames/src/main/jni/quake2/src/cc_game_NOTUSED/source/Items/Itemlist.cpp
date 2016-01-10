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
// cc_itemlist.cpp
// Itemlist
// Basically the link between items and the real world
//

#include "Local.h"
#include "Weapons/WeaponMain.h"

namespace NItems
{
	// Armor
	CArmor *JacketArmor;
	CArmor *CombatArmor;
	CArmor *BodyArmor;
	CArmor *ArmorShard;

	// Health
	IBaseItem *StimPack;
	IBaseItem *SmallHealth;
	IBaseItem *LargeHealth;

	// Keys
	IBaseItem *PowerCube;
#if CLEANCTF_ENABLED
	CFlag *RedFlag;
	CFlag *BlueFlag;
#endif

	// Weapons
	CWeaponItem *Blaster;
	CWeaponItem *Shotgun;
	CWeaponItem *SuperShotgun;
	CWeaponItem *Machinegun;
	CWeaponItem *Chaingun;
	CWeaponItem *GrenadeLauncher;
	CWeaponItem *RocketLauncher;
	CWeaponItem *HyperBlaster;
	CWeaponItem *Railgun;
	CWeaponItem *BFG;
#if CLEANCTF_ENABLED
	CWeaponItem *Grapple;
#endif
#if XATRIX_FEATURES
	CWeaponItem *IonRipper;	
	CWeaponItem *Phalanx;	
#endif

	// Ammo
	CAmmo *Shells;
	CAmmo *Bullets;
	CAmmo *Slugs;
	CAmmo *Rockets;
	CAmmo *Cells;
	CAmmo *Grenades;
	CAmmo *Trap;
#if XATRIX_FEATURES
	CAmmo *MagSlugs;
#endif
#if ROGUE_FEATURES
	CAmmo *Prox;
	CAmmo *Flechettes;
	CAmmo *Rounds;
	IBaseItem *Tesla;
#endif

	// Powerups
	IBaseItem *MegaHealth;
	IBaseItem *BackPack;
	IBaseItem *Quad;
#if XATRIX_FEATURES
	IBaseItem *QuadFire;
#endif
#if ROGUE_FEATURES
	IBaseItem *Double;
	IBaseItem *IRGoggles;
#endif
	IBaseItem *Invul;
	IBaseItem *Silencer;
	IBaseItem *Rebreather;
	IBaseItem *EnvironmentSuit;
	IBaseItem *Bandolier;
	IBaseItem *Adrenaline;
	IBaseItem *AncientHead;
	IBaseItem *PowerShield;
	IBaseItem *PowerScreen;
};

CItemList *ItemList;

CItemList::CItemList() :
numItems(0)
{
};

void CItemList::AddItemToList (IBaseItem *Item)
{
	TempList.push_back (Item);
}
	
typedef std::pair<sint8, sint8> TWeaponMultiMapPairType;
typedef std::multimap<TWeaponMultiMapPairType, sint8> TWeaponMultiMapType;

void AddWeaponsToListLocations (CItemList *List);

void CItemList::SortAndFinalize ()
{
	// Sort
	uint32 sortOrder[] = {ITEMFLAG_ARMOR, ITEMFLAG_WEAPON, ITEMFLAG_AMMO, ITEMFLAG_POWERUP, ITEMFLAG_KEY};
	std::vector<bool> SortedValues (TempList.size(), false);

	for (int z = 0; z < 5; z++)
	{
		if (z == 1)
			AddWeaponsToListLocations (this);
		else
		{
			for (size_t i = 0; i < TempList.size(); ++i)
			{
				if (TempList[i]->Flags & ITEMFLAG_WEAPON)
					continue;

				if (!SortedValues[i] && (TempList[i]->Flags & sortOrder[z]))
				{
					SortedValues[i] = true;
					Items.push_back (TempList[i]);
				}
			}
		}
	}
	
	// Put everything else in the list now
	for (size_t i = 0; i < TempList.size(); ++i)
	{
		if (TempList[i]->Flags & ITEMFLAG_WEAPON)
			continue;

		if (!SortedValues[i])
			Items.push_back (TempList[i]);
	}

	// Finalize
	for (TItemListType::iterator it = Items.begin(); it < Items.end(); ++it)
	{
		IBaseItem *Item = (*it);

		Item->Index = numItems++;

		// Hash!
		if (Item->Classname)
			HashedClassnameItemList.insert (std::make_pair<size_t, size_t> (Com_HashGeneric(Item->Classname, MAX_ITEMS_HASH), Item->Index));
		if (Item->Name)
			HashedNameItemList.insert (std::make_pair<size_t, size_t> (Com_HashGeneric(Item->Name, MAX_ITEMS_HASH), Item->Index));
	}

	// Delete
	TempList.clear();
	TempList.resize(0);
}

void CItemList::SendItemNames ()
{
	for (uint8 i = 0; i < numItems; i++)
		ConfigString (Items[i]->GetConfigStringNumber(), Items[i]->Name);
}

IBaseItem *FindItem (const char *name)
{
	// Check through the itemlist
	static uint32 hash;
	hash = Com_HashGeneric(name, MAX_ITEMS_HASH);

	for (THashedItemListType::iterator it = ItemList->HashedNameItemList.equal_range(hash).first; it != ItemList->HashedNameItemList.equal_range(hash).second; ++it)
	{
		static IBaseItem *Item;
		Item = ItemList->Items.at((*it).second);

		if (Q_stricmp(Item->Name, name) == 0)
			return Item;
	}
	return NULL;
}

IBaseItem *FindItemByClassname (const char *name)
{
	// Check through the itemlist
	static uint32 hash;
	hash = Com_HashGeneric(name, MAX_ITEMS_HASH);

	for (THashedItemListType::iterator it = ItemList->HashedClassnameItemList.equal_range(hash).first; it != ItemList->HashedClassnameItemList.equal_range(hash).second; ++it)
	{
		static IBaseItem *Item;
		Item = ItemList->Items.at((*it).second);

		if (Q_stricmp(Item->Classname, name) == 0)
			return Item;
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void IBaseItem::Add (CPlayerEntity *Player, sint32 quantity)
///
/// \brief	Adds 'quantity' amount of this to 'Player' (ignores any max)
///
/// \author	Paril
/// \date	5/9/2009
///
/// \param	Player		 - If non-null, the entity to add the amount to. 
/// \param	quantity - The amount to add. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void IBaseItem::Add (CPlayerEntity *Player, sint32 quantity)
{
	Player->Client.Persistent.Inventory.Add(this, quantity);
}

IBaseItem *GetItemByIndex (uint32 Index)
{
	if (Index >= MAX_ITEMS || Index >= ItemList->Items.size() || Index < 0)
		return NULL;
	return ItemList->Items[Index];
}

sint32 GetNumItems ()
{
	return ItemList->numItems;
}

void InitItemlist ()
{
	ItemList = QNew (TAG_GENERIC) CItemList;

	AddAmmoToList();
	AddHealthToList();
	AddArmorToList();
	AddPowerupsToList();
	AddKeysToList();
#if CLEANCTF_ENABLED
	if (Game.GameMode & GAME_CTF)
		AddFlagsToList();
#endif

	if (DeathmatchFlags.dfDmTechs.IsEnabled()
#if CLEANCTF_ENABLED
	|| (Game.GameMode & GAME_CTF)
#endif
		)
		AddTechsToList();

	ItemList->SortAndFinalize ();
}

void SetItemNames ()
{
	ItemList->SendItemNames();
}

void InitItemMedia ()
{
}

// This is a required function that will
// go through each item and invalidate any variables that we used.
void InvalidateItemMedia ()
{
	for (sint32 i = 0; i < ItemList->numItems; i++)
	{
		IBaseItem *Item = ItemList->Items[i];

		Item->IconIndex = Item->PickupSoundIndex = 0;
		if (Item->Flags & ITEMFLAG_WEAPON)
		{
			CWeaponItem *Weapon = dynamic_cast<CWeaponItem*>(Item);
			Weapon->Weapon->WeaponModelIndex = Weapon->Weapon->WeaponSoundIndex = 0;
		}
	}
}
