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
// cc_dmflags.h
// This is so that I don't have to keep doing DeathmatchFlags.Integer(). Should be faster than bitwise ops every frame!
//

#if !defined(CC_GUARD_DMFLAGS_H) || !INCLUDE_GUARDS
#define CC_GUARD_DMFLAGS_H

/**
\class	CDeathmatchFlags

\brief	Deathmatch flags handling class.

\author	Paril
\date	29/05/2010
**/
class CDeathmatchFlags
{
	/**
	\class	CDeathmatchFlag
	
	\brief	A single deathmatch flag.
	
	\author	Paril
	\date	29/05/2010
	**/
	class CDeathmatchFlag
	{
		const EDeathmatchFlags	Flag;		// The flag itself
		bool					Enabled;	// true if this flag is enabled

	public:
		CDeathmatchFlag (EDeathmatchFlags Flag) :
		  Flag (Flag),
		  Enabled (false)
		  {
		  };

		CDeathmatchFlag &operator= (CDeathmatchFlag&) { return *this; }

		/**
		\fn	inline bool IsEnabled ()
		
		\brief	Query if this dmflag is enabled. 
		
		\return	true if enabled, false if not. 
		**/
		inline bool IsEnabled ()
		{
			return Enabled;
		};

		/**
		\fn	void Check (EDeathmatchFlags WantedFlags)
		
		\brief	Updates the dmflag enabled value.
		
		\author	Paril
		\date	29/05/2010
		
		\param	WantedFlags	The wanted flags. 
		**/
		void Check (EDeathmatchFlags WantedFlags)
		{
			Enabled = !!(WantedFlags & Flag);
		};

		friend class CDeathmatchFlags;
	};

public:
	CDeathmatchFlag		dfNoHealth,
						dfNoItems,
						dfWeaponsStay,
						dfNoFallingDamage,
						dfInstantItems,
						dfSameLevel,
						dfSkinTeams,
						dfModelTeams,
						dfNoFriendlyFire,
						dfSpawnFarthest,
						dfForceRespawn,
						dfNoArmor,
						dfAllowExit,
						dfInfiniteAmmo,
						dfQuadDrop,
						dfFixedFov,

						dfQuadFireDrop,
						dfNoMines,
						dfNoStackDouble,
						dfNoNukes,
						dfNoSpheres,

#if CLEANCTF_ENABLED
						dfCtfNoTech,
						dfCtfForceJoin,
						dfCtfArmorProtect,
#endif

						dfDmTechs;

	/**
	\fn	CDeathmatchFlags()
	
	\brief	Default constructor.
			Initializes all dmflags to their default values.
	
	\author	Paril
	\date	29/05/2010
	**/
	CDeathmatchFlags();

	/**
	\fn	void UpdateFlags (EDeathmatchFlags wantedFlags)
	
	\brief	Updates the flags described by wantedFlags.
	
	\author	Paril
	\date	29/05/2010
	
	\param	wantedFlags	The wanted flags. 
	**/
	void UpdateFlags (EDeathmatchFlags wantedFlags);
};

extern CDeathmatchFlags DeathmatchFlags;

#else
FILE_WARNING
#endif

