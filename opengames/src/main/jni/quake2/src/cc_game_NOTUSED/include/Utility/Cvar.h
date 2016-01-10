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
// cc_cvar.h
// A small wrapper for cvars that make managing and using cvars a bit easier.
//

#if !defined(CC_GUARD_CVAR_H) || !INCLUDE_GUARDS
#define CC_GUARD_CVAR_H

class CCvar
{
	SCVar			*cVar;

	char			*mainValue;
	float			floatVal;
	sint32			intVal;

	// Updates the cvar values above. Private because it won't be called from anywhere else but here.
	void Update ();

public:
	// Constructors
	CCvar ();
	CCvar (const char *cvarName, const char *defaultValue, ECvarFlags flags = 0);
	CCvar (const char *cvarName, sint32 defaultValue, ECvarFlags flags = 0);
	CCvar (const char *cvarName, float defaultValue, ECvarFlags flags = 0);

	void Register (const char *cvarName, const char *defaultValue, ECvarFlags flags = 0);
	void Register (const char *cvarName, sint32 defaultValue, ECvarFlags flags = 0);
	void Register (const char *cvarName, float defaultValue, ECvarFlags flags = 0);

	float		Float ();
	sint32		Integer();
	char		*StringValue();
	bool		Boolean (bool MustBeOne = false);

	void Set (const char *value, bool Force = false);
	void Set (sint32 value, bool Force = false);
	void Set (float value, bool Force = false);

	bool Modified ();
};

/**
\typedef	uint32 ECvars

\brief	Defines an alias representing the list of cvars.
**/
typedef uint32 ECvars;

/**
\enum	

\brief	Values that represent the list of cvars. 
**/
enum
{
	CV_DEATHMATCH,
	CV_COOP,
	CV_DMFLAGS,
	CV_SKILL,
	CV_FRAG_LIMIT,
	CV_TIME_LIMIT,
	CV_PASSWORD,
	CV_SPECTATOR_PASSWORD,
	CV_NEEDPASS,
	CV_MAXCLIENTS,
	CV_MAXSPECTATORS,
	CV_MAXENTITIES,
	CV_SELECT_EMPTY,
	CV_DEDICATED,
	CV_DEVELOPER,
	CV_FILTERBAN,
	CV_GRAVITY,
	CV_ROLLSPEED,
	CV_ROLLANGLE,
	CV_GUN_X,
	CV_GUN_Y,
	CV_GUN_Z,
	CV_RUN_PITCH,
	CV_RUN_ROLL,
	CV_BOB_UP,
	CV_BOB_PITCH,
	CV_BOB_ROLL,
	CV_CHEATS,
	CV_FLOOD_MSGS,
	CV_FLOOD_PER_SECOND,
	CV_FLOOD_DELAY,
	CV_MAPLIST,
	CV_MAP_DEBUG,
	CV_AIRACCELERATE,
	CV_CC_TECHFLAGS,

#if CLEANCTF_ENABLED
	CV_CTF,
	CV_CTF_FORCEJOIN,
	CV_MATCH_LOCK,
	CV_ELECT_PERCENTAGE,
	CV_ADMIN_PASSWORD,
	CV_WARP_LIST,
	CV_CAPTURE_LIMIT,
	CV_INSTANT_WEAPONS,
#endif

	CV_TOTAL_CVARS
};

struct SCVarDefaultValue
{
	ECvars		cvarIndex;
	const char	*cvarName;
	const char	*cvarDefaultValue;
	ECvarFlags	cvarFlags;
};

extern CCvar	CvarList[CV_TOTAL_CVARS];

void Cvar_Register ();

#else
FILE_WARNING
#endif

