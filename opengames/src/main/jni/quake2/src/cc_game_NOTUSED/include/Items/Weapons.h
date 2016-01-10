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
// cc_weapons.h
// New item system code
//

#if !defined(CC_GUARD_WEAPONS_H) || !INCLUDE_GUARDS
#define CC_GUARD_WEAPONS_H

// Forward declarations
class IWeaponBase;
class CAmmo;

// Class for weapon items.
class CWeaponItem : public virtual IBaseItem
{
public:
	CWeaponItem ();
	CWeaponItem (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   IWeaponBase *Weapon, CAmmo *Ammo, sint32 Amount, const char *VWepModel);

	class IWeaponBase		*Weapon;
	class CAmmo			*Ammo;
	sint32				Amount;
	const char			*VWepModel;

	virtual bool	Pickup (class CItemEntity *Item, CPlayerEntity *Other);
	virtual void	Use (CPlayerEntity *Player);
	virtual void	Drop (CPlayerEntity *Player);
};

// Class for ammo.
class CAmmo : public virtual IBaseItem
{
public:
	CAmmo ();
	CAmmo (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   sint32 Quantity, EAmmoTag Tag);
	CAmmo (const char *Classname, const char *WorldModel, const char *Icon, const char *Name, sint32 Quantity, EAmmoTag Tag);

	sint32			Quantity; // Number gotten when we pick this mother upper
	EAmmoTag	Tag; // YUCKY tag for ammo

	// Only thing different about ammo is how it's picked up.
	virtual bool	Pickup (class CItemEntity *Item, CPlayerEntity *Other);
	virtual void	Use (CPlayerEntity *Player);
	virtual void	Drop (CPlayerEntity *Player);

	// Member functions
	bool	AddAmmo (CPlayerEntity *Player, sint32 count);
	sint32	GetMax(CPlayerEntity *Player);
};

class CAmmoWeapon : public CWeaponItem, public CAmmo
{
public:
	CAmmoWeapon (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   IWeaponBase *Weapon, CAmmo *Ammo, sint32 Amount,
			   const char *VWepModel, sint32 Quantity, EAmmoTag Tag);

	CAmmoWeapon (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   IWeaponBase *Weapon, sint32 Amount, const char *VWepModel,
			   sint32 Quantity, EAmmoTag Tag);

	bool	Pickup (class CItemEntity *Item, CPlayerEntity *Other);
	void	Use (CPlayerEntity *Player);
	void	Drop (CPlayerEntity *Player);
};

extern sint32 maxBackpackAmmoValues[AMMOTAG_MAX];
extern sint32 maxBandolierAmmoValues[AMMOTAG_MAX];

void AddAmmoToList();
void DoWeaponVweps();

/**
\typedef	uint8 EWeaponVwepIndices

\brief	Defines an alias representing the weapon vwep indices.
**/
typedef uint8 EWeaponVwepIndices;

/**
\enum	

\brief	Values that represent visible weapon indexes. 
**/
enum
{
	WEAP_NONE,
	WEAP_BLASTER,
	WEAP_SHOTGUN,
	WEAP_SUPERSHOTGUN,
	WEAP_MACHINEGUN,
	WEAP_CHAINGUN,
	WEAP_GRENADES,
	WEAP_GRENADELAUNCHER,
	WEAP_ROCKETLAUNCHER,
	WEAP_HYPERBLASTER,
	WEAP_RAILGUN,
	WEAP_BFG,
#if CLEANCTF_ENABLED
	WEAP_GRAPPLE,
#endif

	WEAP_MAX
};

#else
FILE_WARNING
#endif
