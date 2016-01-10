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
// New, improved, better, stable item system!
//

#if !defined(CC_GUARD_WEAPONMAIN_H) || !INCLUDE_GUARDS
#define CC_GUARD_WEAPONMAIN_H

#include "Items/Items.h"

class IWeaponBase
{
	friend void		InvalidateItemMedia ();
	friend void		ChainWeapons (std::vector<IWeaponBase*> &Weapons);
	MediaIndex		WeaponSoundIndex;
	MediaIndex		WeaponModelIndex;
	IWeaponBase		*NextWeapon, *PrevWeapon; // Chained weapons

protected:
	// Frames
	uint16			ActivationStart, ActivationNumFrames,
					FireStart,		FireNumFrames,
					IdleStart,		IdleNumFrames,
					DeactStart,		DeactNumFrames;

	uint16			ActivationEnd, FireEnd, IdleEnd, DeactEnd; // To save calls.

	IWeaponBase ();

public:
	bool			isQuad, isSilenced
#if XATRIX_FEATURES
				,	isQuadFire
#endif
#if ROGUE_FEATURES
				,	isDouble
#endif
		;

	uint8						DamageMultiplier;
	CWeaponItem					*Item; // The item that is linked to this weapon.
	const char					*WeaponSound;
	const char					*WeaponModelString; // Temporary
	MediaIndex					vwepIndex;
	std::pair <sint8, sint8>	ListOrder;

	inline IWeaponBase *GetNextWeapon ()
	{
		return NextWeapon;
	};

	inline IWeaponBase *GetPrevWeapon ()
	{
		return PrevWeapon;
	};

	inline int CalcQuadVal (int val)
	{
		return (val * DamageMultiplier);
	};

	inline MediaIndex		GetWeaponSound ()
	{
		return (WeaponSound) ?
			(WeaponSoundIndex) ? WeaponSoundIndex : (WeaponSoundIndex = SoundIndex(WeaponSound))
			: 0;
	};

	inline MediaIndex		GetWeaponModel ()
	{
		return (WeaponModelString) ?
			(WeaponModelIndex) ? WeaponModelIndex : (WeaponModelIndex = ModelIndex(WeaponModelString))
			: 0;
	};

	IWeaponBase(sint8 ListOrderHigh, sint8 ListOrderLow, const char *model, uint16 ActivationStart, uint16 ActivationEnd, uint16 FireStart, uint16 FireEnd,
				 uint16 IdleStart, uint16 IdleEnd, uint16 DeactStart, uint16 DeactEnd, const char *WeaponSound = NULL);

	// InitWeapon "clears" the previous weapon by introducing the current weapon.
	virtual void	InitWeapon (CPlayerEntity *Player);

	// Muzzle flash
	virtual void	Muzzle (CPlayerEntity *Player, sint32 muzzleNum);
	virtual void	FireAnimation (CPlayerEntity *Player);
	void			AttackSound (CPlayerEntity *Player); // Sound for quad and CTF techs

	// General animating function.
	// Doesn't need to be changed.
	virtual void	WeaponGeneric (CPlayerEntity *Player);

	// These two functions replace the need for an array for
	// pause and fire frames.
	virtual bool	CanFire	(CPlayerEntity *Player) = 0;
	virtual bool	CanStopFidgetting (CPlayerEntity *Player) = 0;

	// Ammo usage
	virtual void	DepleteAmmo(CPlayerEntity *Player, sint32 Amount);
	
	// This function is called when the player hits the attack button.
	// Returns "true" if the animation can go ahead (check for ammo, etc here)
	virtual void	OutOfAmmo (CPlayerEntity *Player);
	virtual bool	AttemptToFire (CPlayerEntity *Player); 

	// The function called to "fire"
	virtual void	Fire (CPlayerEntity *Player) = 0;

	void			ChangeWeapon (CPlayerEntity *Player);
	virtual void	Think (CPlayerEntity *Player);

	void			NoAmmoWeaponChange (CPlayerEntity *Player);

	virtual void	AddWeaponToItemList (CItemList *List) = 0;
	virtual void	InitWeaponVwepModel (sint32 TakeAway) = 0;

	virtual void	CreateItem (CItemList *List)
	{
	};

	virtual void Use (CWeaponItem *Wanted, CPlayerEntity *Player);
};

#define WEAPON_CLASS_DEFS(x) \
	void AddWeaponToItemList (CItemList *List); \
	void InitWeaponVwepModel (sint32 TakeAway); \
	void CreateItem (CItemList *List); \
	static x Weapon;

#define WEAPON_DEFS(x) \
	x x::Weapon; \
	void x::AddWeaponToItemList (CItemList *List) \
	{ \
		CreateItem (List); \
	}; \
	void x::InitWeaponVwepModel (sint32 TakeAway) \
	{ \
		vwepIndex = ModelIndex(Item->VWepModel) - TakeAway; \
	};

#include "Weapons/Blaster.h"
#include "Weapons/Shotgun.h"
#include "Weapons/SuperShotgun.h"
#include "Weapons/Machinegun.h"
#include "Weapons/Chaingun.h"
#include "Weapons/Handgrenade.h"
#include "Weapons/GrenadeLauncher.h"
#include "Weapons/RocketLauncher.h"
#include "Weapons/Hyperblaster.h"
#include "Weapons/Railgun.h"
#include "Weapons/Bfg.h"
#if CLEANCTF_ENABLED
#include "CTF/CtfWeaponGrapple.h"
#endif
#include "Weapons/DebugWeapons.h"

#else
FILE_WARNING
#endif
