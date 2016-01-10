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
// cc_gameapi.cpp
// Contains overloads and other neat things
//

#include "Local.h"
#include "Utility/ExceptionHandler.h"
#include "Player/ServerCommands.h"

SGameImport	gi;

EBrushContents PointContents (vec3f start)
{
CC_DISABLE_DEPRECATION
	return gi.pointcontents(start);
CC_ENABLE_DEPRECATION
}

/**
\enum	

\brief	Values that represent area types (for BoxEdicts). 
**/
enum
{
	AREA_SOLID	= 1,
	AREA_TRIGGERS
};

TBoxEdictsEntityList BoxEdicts (vec3f mins, vec3f maxs, bool triggers)
{
	static SEntity *list[MAX_CS_EDICTS];
	Mem_Zero (list, sizeof(list));

CC_DISABLE_DEPRECATION
	int count = gi.BoxEdicts (mins, maxs, list, MAX_CS_EDICTS, (triggers) ? AREA_TRIGGERS : AREA_SOLID);
CC_ENABLE_DEPRECATION

	TBoxEdictsEntityList Ents;
	for (int i = 0; i < count; ++i)
	{
		if (list[i]->Entity)
			Ents.push_back (list[i]->Entity);
	}

	return Ents;
}

void ConfigString (sint32 configStringIndex, const char *configStringValue, ...)
{
	va_list		argptr;
	CTempMemoryBlock	string = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, configStringValue);
	vsnprintf (string.GetBuffer<char>(), string.GetSize() - 1, configStringValue, argptr);
	va_end (argptr);

	/*	if (Audience)
	{
		WriteChar (SVC_CONFIGSTRING);
		WriteShort (configStringIndex);
		WriteString (const_cast<char*>(configStringValue));
		Audience->CastTo (CASTFLAG_UNRELIABLE);
	}
	else
	{*/
CC_DISABLE_DEPRECATION
		gi.configstring (configStringIndex, string.GetBuffer<char>());
CC_ENABLE_DEPRECATION
	//}
}

void GameError (const char *fmt, ...)
{
	va_list		argptr;
	CTempMemoryBlock		text = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, fmt);
	vsnprintf (text.GetBuffer<char>(), text.GetSize() - 1, fmt, argptr);
	va_end (argptr);

	CC_ReportGameError (text.GetBuffer<char>());

CC_DISABLE_DEPRECATION
	gi.error (text.GetBuffer<char>());
CC_ENABLE_DEPRECATION
}

//
//
// Main Game API
//
//

const int MAX_INFO_STRING = 512;

bool CGameAPI::ClientConnect (CPlayerEntity *Player, char *userinfo)
{
	CUserInfo UserInfo = CUserInfo(userinfo);
	bool CanConnect = Player->Connect(userinfo, UserInfo);
	Q_snprintfz (userinfo, MAX_INFO_STRING, "%s", UserInfo.ToString().CString());
	return CanConnect;
}

void CGameAPI::ClientBegin (CPlayerEntity *Player)
{
	Player->Begin ();
}

void CGameAPI::ClientUserinfoChanged (CPlayerEntity *Player, char *userInfo)
{
	Player->UserinfoChanged (String(userInfo));
}

void CGameAPI::ClientDisconnect (CPlayerEntity *Player)
{
	Player->Disconnect ();
}

void CGameAPI::ClientThink (CPlayerEntity *Player, SUserCmd *cmd)
{
	Player->ClientThink (cmd);
}

GAME_CLASS	GameAPI;

/*
===========
ShutdownGame

called when the game is being unloaded.
============
*/
void ShutdownGame ()
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	GameAPI.Shutdown ();

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the Game.  This will happen every level load.
============
*/
void ClientBegin (SEntity *ent)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	GameAPI.ClientBegin (entity_cast<CPlayerEntity>(ent->Entity));

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
===========
ClientUserInfoChanged

called whenever the player updates a userinfo variable.

The game can override any of the settings in place
(forcing skins or names, etc) before copying it off.
============
*/
void ClientUserinfoChanged (SEntity *ent, char *userinfo)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	GameAPI.ClientUserinfoChanged (entity_cast<CPlayerEntity>(ent->Entity), userinfo);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}


/*
===========
ClientConnect

Called when a player begins connecting to the server.
The game can refuse entrance to a client by returning false.
If the client is allowed, the connection process will continue
and eventually get to ClientBegin()
Changing levels will NOT cause this to be called again, but
loadgames will.
============
*/
void FixDemoSetup ();

BOOL ClientConnect (SEntity *ent, char *userinfo)
{
	int returnVal = 0;

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	// Paril
	// R1CH made R1Q2 not call SpawnEntities in an attract loop.
	// Smart, but it breaks shit.
	if (!ent->Entity)
	{
		ServerPrint ("Fixing R1Q2 non-attract-loop code...\n");
		FixDemoSetup ();
	}

	returnVal = (GameAPI.ClientConnect (entity_cast<CPlayerEntity>(ent->Entity), userinfo) ? 1 : 0);

#if CC_USE_EXCEPTION_HANDLER && WIN32
CC_EXCEPTION_HANDLER_END_CUSTOM_BEGIN
	returnVal = 0;
CC_EXCEPTION_HANDLER_END_CUSTOM_END
#endif

	return returnVal;
}

/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.
============
*/
void ClientDisconnect (SEntity *ent)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	GameAPI.ClientDisconnect (entity_cast<CPlayerEntity>(ent->Entity));

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}


//==============================================================

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame.
==============
*/
void ClientThink (SEntity *ent, SUserCmd *ucmd)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	GameAPI.ClientThink (entity_cast<CPlayerEntity>(ent->Entity), ucmd);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
================
RunFrame

Advances the world by 0.1 seconds
================
*/
void RunFrame ()
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	GameAPI.RunFrame ();

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
============
InitGame

This will be called when the dll is first loaded, which
only happens when a new game is started or a save game
is loaded.
============
*/
void InitGame ()
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	GameAPI.Init ();

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
==============
SpawnEntities

Creates a server's entity / program execution context by
parsing textual entity definitions out of an ent file.
==============
*/
void SpawnEntities (char *ServerLevelName, char *entities, char *spawnpoint)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	GameAPI.SpawnEntities (ServerLevelName, entities, spawnpoint);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
=================
ServerCommand

ServerCommand will be called when an "sv" command is issued.
The game can issue gi.argc() / gi.argv() commands to get the rest
of the parameters
=================
*/
void ServerCommand ()
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	GameAPI.ServerCommand ();

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
=================
ClientCommand

game commands issued by clients
=================
*/
void ClientCommand (SEntity *ent)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif
	if (!ent->Server.Client || !ent->Entity || (entity_cast<CPlayerEntity>(ent->Entity)->Client.Persistent.State != SVCS_SPAWNED))
		return;		// not fully in game yet

	GameAPI.ClientCommand (entity_cast<CPlayerEntity>(ent->Entity));

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
============
WriteGame

This will be called whenever the game goes to a new level,
and when the user explicitly saves the Game.

Game information include cross level data, like multi level
triggers, help computer info, and all client states.

A single player death will automatically restore from the
last save position.
============
*/
void WriteGame (char *filename, BOOL autosave)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	GameAPI.WriteGame (filename, !!autosave);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

void ReadGame (char *filename)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	GameAPI.ReadGame (filename);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

//==========================================================

/*
=================
WriteLevel

=================
*/
void WriteLevel (char *filename)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN

#endif
	GameAPI.WriteLevel (filename);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}


/*
=================
ReadLevel

SpawnEntities will allready have been called on the
level the same way it was when the level was saved.

That is necessary to get the baselines
set up identically.

The server will have cleared all of the world links before
calling ReadLevel.

No clients are connected yet.
=================
*/
void ReadLevel (char *filename)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	GameAPI.ReadLevel (filename);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
=================
GetGameAPI

Returns a pointer to the structure with all entry points
and global variables
=================
*/

//
// functions exported by the game subsystem
//
struct SGameExport
{
	sint32			apiVersion;

	// the init function will only be called when a game starts,
	// not each time a level is loaded.  Persistant data for clients
	// and the server can be allocated in init
	void		(*Init) ();
	void		(*Shutdown) ();

	// each new level entered will cause a call to SpawnEntities
	void		(*SpawnEntities) (char *mapName, char *entString, char *spawnPoint);

	// Read/Write Game is for storing persistant cross level information
	// about the world state and the clients.
	// WriteGame is called every time a level is exited.
	// ReadGame is called on a loadgame.
	void		(*WriteGame) (char *fileName, BOOL autoSave);
	void		(*ReadGame) (char *fileName);

	// ReadLevel is called after the default map information has been
	// loaded with SpawnEntities
	void		(*WriteLevel) (char *filename);
	void		(*ReadLevel) (char *filename);

	BOOL		(*ClientConnect) (SEntity *ent, char *userInfo);
	void		(*ClientBegin) (SEntity *ent);
	void		(*ClientUserinfoChanged) (SEntity *ent, char *userInfo);
	void		(*ClientDisconnect) (SEntity *ent);
	void		(*ClientCommand) (SEntity *ent);
	void		(*ClientThink) (SEntity *ent, SUserCmd *cmd);

	void		(*RunFrame) ();

	// ServerCommand will be called when an "sv <command>" command is issued on the
	// server console.
	// The game can issue gi.argc() / gi.argv() commands to get the rest
	// of the parameters
	void		(*ServerCommand) ();

	//
	// global variables shared between game and server
	//

	// The edict array is allocated in the game dll so it
	// can vary in size from one game to another.
	// 
	// The size will be fixed when ge->Init() is called
	SEntity		*edicts;
	sint32		edictSize;
	sint32		numEdicts;		// current number, <= MAX_CS_EDICTS
	sint32		maxEdicts;
} globals =
{
	GAME_APIVERSION,
	InitGame,
	ShutdownGame,
	SpawnEntities,
	WriteGame,
	ReadGame,
	WriteLevel,
	ReadLevel,
	ClientConnect,
	ClientBegin,
	ClientUserinfoChanged,
	ClientDisconnect,
	ClientCommand,
	ClientThink,
	RunFrame,
	ServerCommand,
	NULL,
	sizeof(SEntity),
	0,
	0
};

SEntity *&CGameAPI::GetEntities ()
{
	return globals.edicts;
};

sint32 &CGameAPI::GetEdictSize ()
{
	return globals.edictSize;
};

sint32 &CGameAPI::GetNumEdicts()
{
	return globals.numEdicts;
};

sint32 &CGameAPI::GetMaxEdicts()
{
	return globals.maxEdicts;
};

extern "C"
{
	SGameExport *GetGameAPI (SGameImport *import)
	{
		gi = *import;
		return &globals;
	}
}

