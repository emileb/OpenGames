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
// cc_itemlist.h
// Itemlist
// Basically the link between items and the real world
//

#if !defined(CC_GUARD_ITEMLIST_H) || !INCLUDE_GUARDS
#define CC_GUARD_ITEMLIST_H

// Just to conform to the configstrings
const int MAX_ITEMS			= 256;
const int MAX_ITEMS_HASH	= (MAX_ITEMS/2);

typedef std::pair<size_t, size_t> THashedItemListPairType;
typedef std::multimap<size_t, size_t> THashedItemListType;
typedef std::vector<IBaseItem*> TItemListType;

// Generic itemlist.
class CItemList
{
public:
	sint32			numItems;

	CItemList();
	void SendItemNames ();

	// Revision 2.0
	// Instead of having the 9 functions I used to have to add
	// each TYPE of weapon, you create the item, and send it to here.
	void AddItemToList (IBaseItem *Item);

	// Sorts the list and adds the items
	void SortAndFinalize ();

	TItemListType			TempList;
	TItemListType			Items;

	// There are two hash tables for items; hashed by classname, and hashed by item name.
	THashedItemListType		HashedClassnameItemList;
	THashedItemListType		HashedNameItemList;
};

extern CItemList *ItemList;

void InitItemlist ();
IBaseItem *FindItemByClassname (const char *name);
IBaseItem *FindItem (const char *name);
IBaseItem *GetItemByIndex (uint32 Index);
sint32 GetNumItems ();

void InitItemMedia ();

// A non-yuck way to my yuck in Weapons!
namespace NItems
{
	// Armor
	extern CArmor *JacketArmor;
	extern CArmor *CombatArmor;
	extern CArmor *BodyArmor;
	extern CArmor *ArmorShard;

	// Health
	extern IBaseItem *StimPack;
	extern IBaseItem *SmallHealth;
	extern IBaseItem *LargeHealth;

	// Keys
	extern IBaseItem *PowerCube;
#if CLEANCTF_ENABLED
	extern CFlag *RedFlag;
	extern CFlag *BlueFlag;
#endif

	// Weapons
	extern class CWeaponItem *Blaster;
	extern class CWeaponItem *Shotgun;
	extern class CWeaponItem *SuperShotgun;
	extern class CWeaponItem *Machinegun;
	extern class CWeaponItem *Chaingun;
	extern class CWeaponItem *GrenadeLauncher;
	extern class CWeaponItem *RocketLauncher;
	extern class CWeaponItem *HyperBlaster;
	extern class CWeaponItem *Railgun;
	extern class CWeaponItem *BFG;
#if CLEANCTF_ENABLED
	extern class CWeaponItem *Grapple;
#endif
#if XATRIX_FEATURES
	extern class CWeaponItem *IonRipper;	
	extern class CWeaponItem *Phalanx;	
#endif

	// Ammo
	extern CAmmo *Shells;
	extern CAmmo *Bullets;
	extern CAmmo *Slugs;
	extern CAmmo *Rockets;
	extern CAmmo *Cells;
	extern CAmmo *Grenades;
	extern CAmmo *Trap;
#if XATRIX_FEATURES
	extern CAmmo *MagSlugs;
#endif
#if ROGUE_FEATURES
	extern CAmmo *Prox;
	extern CAmmo *Flechettes;
	extern CAmmo *Rounds;
	extern IBaseItem *Tesla;
#endif

	// Powerups
	extern IBaseItem *MegaHealth;
	extern IBaseItem *BackPack;
	extern IBaseItem *Quad;
#if XATRIX_FEATURES
	extern IBaseItem *QuadFire;
#endif
#if ROGUE_FEATURES
	extern IBaseItem *Double;
	extern IBaseItem *IRGoggles;
#endif
	extern IBaseItem *Invul;
	extern IBaseItem *Silencer;
	extern IBaseItem *Rebreather;
	extern IBaseItem *EnvironmentSuit;
	extern IBaseItem *Bandolier;
	extern IBaseItem *Adrenaline;
	extern IBaseItem *AncientHead;
	extern IBaseItem *PowerShield;
	extern IBaseItem *PowerScreen;
};

#else
FILE_WARNING
#endif
