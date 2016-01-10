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
// cc_enum.h
// A file to hold all global enums.
//

#if !defined(CC_GUARD_CC_ENUM_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ENUM_H

/**
\typedef	uint32 EItemFlags

\brief	Defines an alias representing item flags.
**/
typedef sint32 EItemFlags;

/**
\enum	

\brief	Values that represent item flags. 
**/
enum
{
	ITEMFLAG_NONE				= 0,
	ITEMFLAG_WEAPON				= BIT(0),
	ITEMFLAG_AMMO				= BIT(1),
	ITEMFLAG_HEALTH				= BIT(2),
	ITEMFLAG_ARMOR				= BIT(3),
	ITEMFLAG_STAY_COOP			= BIT(4),
	ITEMFLAG_KEY				= BIT(5),
	ITEMFLAG_POWERUP			= BIT(6),
	ITEMFLAG_GRABBABLE			= BIT(7),
	ITEMFLAG_USABLE				= BIT(8),
	ITEMFLAG_DROPPABLE			= BIT(9),
	ITEMFLAG_TECH				= BIT(10),
	ITEMFLAG_NOT_GIVEABLE		= BIT(11), // item cannot be give'd
	ITEMFLAG_MELEE				= BIT(12),
};

/**
\typedef	uint8 EAmmoTag
	
\brief	Defines an alias representing ammo tags.
**/
typedef uint8 EAmmoTag;

/**
\enum	
	
\brief	Values that represent ammo tags. 
**/
enum
{
	AMMOTAG_SHELLS,
	AMMOTAG_BULLETS,
	AMMOTAG_GRENADES,
	AMMOTAG_ROCKETS,
	AMMOTAG_CELLS,
	AMMOTAG_SLUGS,

#if XATRIX_FEATURES
	AMMOTAG_MAGSLUGS,
	AMMOTAG_TRAP,
#endif
#if ROGUE_FEATURES
	AMMOTAG_PROX,
	AMMOTAG_FLECHETTES,
	AMMOTAG_TESLA,
	AMMOTAG_ROUNDS,
#endif

	AMMOTAG_MAX
};

/**
\typedef	uint16 EDamageFlags

\brief	Defines an alias representing the damage flags.
**/
typedef uint16 EDamageFlags;

/**
\enum	

\brief	Values that represent damage flags. 
**/
enum
{
	DAMAGE_RADIUS				= BIT(0), // Indirect damage
	DAMAGE_NO_ARMOR				= BIT(1), // Goes through armor
	DAMAGE_ENERGY				= BIT(2), // Energy-based (blaster)
	DAMAGE_NO_KNOCKBACK			= BIT(3), // Don't add knockback
	DAMAGE_BULLET				= BIT(4), // Bullet damage (used for ricochets)
	DAMAGE_NO_PROTECTION		= BIT(5), // Always damages
	DAMAGE_DESTROY_ARMOR		= BIT(6), // Damage is done to armor and health.
	DAMAGE_NO_REG_ARMOR			= BIT(7), // Damage skips regular armor
	DAMAGE_NO_POWER_ARMOR		= BIT(8), // Damage skips power armor
};

// some weapon value constants
const int DEFAULT_BULLET_HSPREAD			= 300;
const int DEFAULT_BULLET_VSPREAD			= 500;
const int DEFAULT_SHOTGUN_HSPREAD			= 1000;
const int DEFAULT_SHOTGUN_VSPREAD			= 500;
const int DEFAULT_DEATHMATCH_SHOTGUN_COUNT	= 12;
const int DEFAULT_SHOTGUN_COUNT				= 12;
const int DEFAULT_SSHOTGUN_COUNT			= 20;

#else
FILE_WARNING
#endif