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
// cc_techs.h
// 
//

#if !defined(CC_GUARD_TECHS_H) || !INCLUDE_GUARDS
#define CC_GUARD_TECHS_H

class CTech : public IBaseItem
{
	uint32		TechNumber;

public:

	/**
	\typedef	uint8 ETechType
	
	\brief	Defines an alias representing the type of a tech.
	**/
	typedef uint8 ETechType;

	/**
	\enum	
	
	\brief	Values that represent the types of a tech. 
	**/
	enum
	{
		TECH_PASSIVE,			// Tech's effect requires time; happens in ClientEndServerFrame
		TECH_AGGRESSIVE,		// Tech's effect will have a direct effect on any damage going in or out of the player.
		TECH_CUSTOM				// Tech's effect is hardcoded
	};

	ETechType	TechType;	// Type of tech

	CTech (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   uint32 TechNumber, ETechType TechType);

	CTech (const char *Classname, const char *Model, const char *Image, const char *Name, CTech::ETechType TechType, uint32 TechNumber);

	CItemEntity *DropItem (IBaseEntity *Entity);

	bool	Pickup (class CItemEntity *Item, CPlayerEntity *Other);
	void	Drop (CPlayerEntity *Player);
	void	Use (CPlayerEntity *Player);

	inline uint32 GetTechNumber ()
	{
		return TechNumber;
	};

	virtual void DoPassiveTech		(CPlayerEntity *Player) {};
	
	// Left is always the player who is being checked for the tech.
	// Calculated is true if all of the armor save values have been calculated; otherwise it is false.
	//   Calculated would be used, for example, in Strength, you would only apply the effect on false Calculated
	//   and on Resistance, after.
	// "Damage" is a reference to the current damage variable. Calculated = false means it's modifying "damage",
	// true means it's modifying "take".
	// Defending is if the Left is defending the Right's shot.
	// Rest are self explanatory.
	virtual void DoAggressiveTech	(	CPlayerEntity *Left, IBaseEntity *Right, bool Calculated, sint32 &Damage, sint32 &Knockback, EDamageFlags &DamageFlags,
										EMeansOfDeath &Mod, bool Defending	) {};
};

/**
\typedef	uint8 ECTFTechIndexes

\brief	Defines an alias representing the CTF tech indexes.
**/
typedef uint8 ECTFTechIndexes;

/**
\enum	

\brief	List of tech numbers.
		Add to the end of the list to add a new tech.
**/
enum
{
	CTFTECH_RESISTANCE_NUMBER = 1,
	CTFTECH_STRENGTH_NUMBER,
	CTFTECH_HASTE_NUMBER,
	CTFTECH_REGEN_NUMBER,

#if AMMO_REGEN_TECH
	CTFTECH_AMMOREGEN_NUMBER,
#endif
};

const FrameNumber CTF_TECH_TIMEOUT			= 600;  // seconds before techs spawn again

void SetupTechSpawn ();
void AddTechsToList ();
void ResetTechs ();

#else
FILE_WARNING
#endif
