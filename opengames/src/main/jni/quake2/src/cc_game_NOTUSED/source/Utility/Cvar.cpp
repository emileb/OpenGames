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
// cc_cvar.cpp
// A small wrapper for cvars that make managing and using cvars a bit easier.
//

#include "Local.h"

CC_DISABLE_DEPRECATION

CCvar::CCvar () :
	cVar(NULL),
	mainValue(NULL),
	floatVal(0),
	intVal(0)
{
}
CCvar::CCvar (const char *cvarName, const char *defaultValue, ECvarFlags flags) :
	cVar(NULL),
	mainValue(NULL),
	floatVal(0),
	intVal(0)
{
	Register(cvarName, defaultValue, flags);
}
CCvar::CCvar (const char *cvarName, sint32 defaultValue, ECvarFlags flags) :
	cVar(NULL),
	mainValue(NULL),
	floatVal(0),
	intVal(0)
{
	Register(cvarName, defaultValue, flags);
}
CCvar::CCvar (const char *cvarName, float defaultValue, ECvarFlags flags) :
	cVar(NULL),
	mainValue(NULL),
	floatVal(0),
	intVal(0)
{
	Register(cvarName, defaultValue, flags);
}

void CCvar::Update()
{
	mainValue = cVar->String;
	floatVal = cVar->FloatVal;
	intVal = floatVal;
}

void CCvar::Register(const char *cvarName, const char *defaultValue, ECvarFlags flags)
{
	cVar = gi.cvar ((char*)cvarName, (char*)defaultValue, flags);

	mainValue = cVar->String;
	floatVal = cVar->FloatVal;
	intVal = floatVal;
}

void CCvar::Register(const char *cvarName, float defaultValue, ECvarFlags flags)
{
	cVar = gi.cvar ((char*)cvarName, ToString(defaultValue).CString(), flags);

	mainValue = cVar->String;
	floatVal = cVar->FloatVal;
	intVal = floatVal;
}

void CCvar::Register(const char *cvarName, sint32 defaultValue, ECvarFlags flags)
{
	cVar = gi.cvar ((char*)cvarName, ToString(defaultValue).CString(), flags);

	mainValue = cVar->String;
	floatVal = cVar->FloatVal;
	intVal = floatVal;
}

void CCvar::Set (const char *value, bool Force)
{
	if (!Force)
		cVar = gi.cvar_set (cVar->Name, (char*)value);
	else
		cVar = gi.cvar_forceset (cVar->Name, (char*)value);
}

void CCvar::Set (float value, bool Force)
{
	String val = ToString(value);

	if (!Force)
		cVar = gi.cvar_set (cVar->Name, (char*)val.CString());
	else
		cVar = gi.cvar_forceset (cVar->Name, (char*)val.CString());
}

void CCvar::Set (sint32 value, bool Force)
{
	String val = ToString(value);

	if (!Force)
		cVar = gi.cvar_set (cVar->Name, (char*)val.CString());
	else
		cVar = gi.cvar_forceset (cVar->Name, (char*)val.CString());
}

float CCvar::Float ()
{
	Update ();
	return floatVal;
}

sint32 CCvar::Integer ()
{
	Update ();
	return intVal;
}

char *CCvar::StringValue ()
{
	Update ();
	return mainValue;
}

bool CCvar::Boolean (bool MustBeOne)
{
	Update ();
	return (MustBeOne ? (Integer() == 1) : (Float() != 0));
}

bool CCvar::Modified ()
{
	bool modified = (cVar->Modified == 1);

	cVar->Modified = 0;
	return modified;
}

CC_ENABLE_DEPRECATION

CCvar	CvarList[CV_TOTAL_CVARS];

static SCVarDefaultValue CvarDefaults[] =
{
	/*
		CVAR ENUM		|      NAME      |  DEFAULT VALUE         |         FLAGS
	*/
	
	// An enum of CV_TOTAL_CVARS means register but do not store
	{ CV_TOTAL_CVARS,			"gamename",				GAMENAME,		CVAR_SERVERINFO | CVAR_LATCH_SERVER		},
	{ CV_TOTAL_CVARS,			"gamedate",				BuildDate(),	CVAR_SERVERINFO | CVAR_LATCH_SERVER		},

	{ CV_DEATHMATCH,			"deathmatch",			"0",			CVAR_SERVERINFO | CVAR_LATCH_SERVER		},
	{ CV_COOP,					"coop",					"0",			CVAR_LATCH_SERVER						},
	{ CV_DMFLAGS,				"dmflags",				"0",			CVAR_SERVERINFO							},
	{ CV_SKILL,					"skill",				"1",			CVAR_LATCH_SERVER						},
	{ CV_FRAG_LIMIT,			"fraglimit",			"0",			CVAR_SERVERINFO							},
	{ CV_TIME_LIMIT,			"timelimit",			"0",			CVAR_SERVERINFO							},
	{ CV_PASSWORD,				"password",				"",				CVAR_USERINFO							},
	{ CV_SPECTATOR_PASSWORD,	"spectator_password",	"",				CVAR_USERINFO							},
	{ CV_NEEDPASS,				"needpass",				"0",			CVAR_SERVERINFO							},
	{ CV_MAXCLIENTS,			"maxclients",			"4",			CVAR_SERVERINFO | CVAR_LATCH_SERVER		},
	{ CV_MAXSPECTATORS,			"maxspectators",		"4",			CVAR_SERVERINFO							},
	{ CV_MAXENTITIES,			"maxentities",			"1024",			CVAR_LATCH_SERVER						},
	{ CV_SELECT_EMPTY,			"g_select_empty",		"0",			CVAR_ARCHIVE							},
	{ CV_DEDICATED,				"dedicated",			"0",			CVAR_READONLY							},
	{ CV_DEVELOPER,				"developer",			"0",			0										},
	{ CV_FILTERBAN,				"filterban",			"1",			0										},
	{ CV_GRAVITY,				"sv_gravity",			"800",			0										},
	{ CV_ROLLSPEED,				"sv_rollspeed",			"200",			0										},
	{ CV_ROLLANGLE,				"sv_rollangle",			"2",			0										},
	{ CV_GUN_X,					"gun_x",				"0",			0										},
	{ CV_GUN_Y,					"gun_y",				"0",			0										},
	{ CV_GUN_Z,					"gun_z",				"0",			0										},
	{ CV_RUN_PITCH,				"run_pitch",			"0.002",		0										},
	{ CV_RUN_ROLL,				"run_roll",				"0.005",		0										},
	{ CV_BOB_UP,				"bob_up",				"0.005",		0										},
	{ CV_BOB_PITCH,				"bob_pitch",			"0.002",		0										},
	{ CV_BOB_ROLL,				"bob_roll",				"0.002",		0										},
	{ CV_CHEATS,				"cheats",				"0",			CVAR_SERVERINFO | CVAR_LATCH_SERVER		},
	{ CV_FLOOD_MSGS,			"flood_msgs",			"4",			0										},
	{ CV_FLOOD_PER_SECOND,		"flood_persecond",		"4",			0										},
	{ CV_FLOOD_DELAY,			"flood_waitdelay",		"10",			0										},
	{ CV_MAPLIST,				"sv_maplist",			"",				0										},
	{ CV_MAP_DEBUG,				"map_debug",			"0",			CVAR_LATCH_SERVER						},
	{ CV_AIRACCELERATE,			"sv_airaccelerate",		"0",			CVAR_SERVERINFO							},
	{ CV_CC_TECHFLAGS,			"cc_techflags",			"0",			CVAR_LATCH_SERVER						},

#if CLEANCTF_ENABLED
	{ CV_CTF,					"ctf",					"0",			CVAR_LATCH_SERVER | CVAR_SERVERINFO		},
	{ CV_CTF_FORCEJOIN,			"ctf_forcejoin",		"",				0										},
	{ CV_MATCH_LOCK,			"matchlock",			"1",			CVAR_SERVERINFO							},
	{ CV_ELECT_PERCENTAGE,		"electpercentage",		"66",			0										},
	{ CV_ADMIN_PASSWORD,		"admin_password",		"",				0										},
	{ CV_WARP_LIST,				"warp_list",			"q2ctf1 q2ctf2 q2ctf3 q2ctf4 q2ctf5",	0				},
	{ CV_CAPTURE_LIMIT,			"capturelimit",			"0",			CVAR_SERVERINFO							},
	{ CV_INSTANT_WEAPONS,		"capturelimit",			"0",			CVAR_SERVERINFO							},
#endif
};

void Cvar_Register ()
{
	for (size_t i = 0; i < ArrayCount(CvarDefaults); ++i)
	{
		if (CvarDefaults[i].cvarIndex == CV_TOTAL_CVARS)
			CCvar (CvarDefaults[i].cvarName, CvarDefaults[i].cvarDefaultValue, CvarDefaults[i].cvarFlags);
		else
			CvarList[CvarDefaults[i].cvarIndex].Register (CvarDefaults[i].cvarName, CvarDefaults[i].cvarDefaultValue, CvarDefaults[i].cvarFlags);
	}
}
