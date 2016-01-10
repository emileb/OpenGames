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
// cc_game.cpp
// Contains actual game code; the code needed to make Q2 go anywhere.
//

#include "Local.h"
#include "Player/Ban.h"
#include "Entities/TargetEntities.h"
#include "Player/BodyQueue.h"
#include "Player/ServerCommands.h"
#include "Utility/Version.h"
#include "Items/Itemlist.h"
#include <ctime>

void CLevelLocals::CEntityList::Save (CFile &File)
{
	File.Write<size_t> (Open.size());
	for (TEntitiesContainer::iterator it = Open.begin(); it != Open.end(); ++it)
		// Poop.
		// Entities can't be guarenteed a number till
		// they spawn the first time!
		File.Write<sint32> ((*it) - Game.Entities);

	File.Write<size_t> (Closed.size());
	for (TEntitiesContainer::iterator it = Closed.begin(); it != Closed.end(); ++it)
		File.Write<sint32> ((*it)->Server.State.Number);
};

void CLevelLocals::CEntityList::Load (CFile &File)
{
	size_t size = File.Read<size_t> ();

	Open.clear ();
	for (size_t i = 0; i < size; i++)
		Open.push_back (&Game.Entities[File.Read<sint32> ()]);

	size = File.Read<size_t> ();

	Closed.clear ();
	for (size_t i = 0; i < size; i++)
		Closed.push_back (&Game.Entities[File.Read<sint32> ()]);
};

void CLevelLocals::Save (CFile &File)
{
	File.Write<FrameNumber> (Frame);

	File.Write (FullLevelName);
	File.Write (ServerLevelName);
	File.Write (NextMap);
	File.Write (ForceMap);

	File.Write<SIntermissionState> (Intermission);

	File.Write<sint32> ((SightClient) ? SightClient->State.GetNumber() : -1);
	File.Write<SGoalList> (Secrets);
	File.Write<SGoalList> (Goals);
	File.Write<SMonsterCount> (Monsters);
	File.Write<uint8> (PowerCubeCount);
	File.Write<uint32> (Inhibit);
	File.Write<uint32> (EntityNumber);

#if ROGUE_FEATURES
	File.Write<FrameNumber> (DisguiseViolationFrametime);
	File.Write<sint32> ((DisguiseViolator) ? DisguiseViolator->State.GetNumber() : -1);
#endif

	Entities.Save (File);
};

void CLevelLocals::Load (CFile &File)
{
	Frame = File.Read<FrameNumber> ();

	FullLevelName = File.ReadString ();
	ServerLevelName = File.ReadString ();
	NextMap = File.ReadString ();
	ForceMap = File.ReadString ();

	Intermission = File.Read<SIntermissionState> ();

	sint32 Index = File.Read<sint32> ();
	if (Index != -1)
		SightClient = entity_cast<CPlayerEntity>(Game.Entities[Index].Entity);

	Secrets = File.Read<SGoalList> ();
	Goals = File.Read<SGoalList> ();
	Monsters = File.Read<SMonsterCount> ();
	PowerCubeCount = File.Read<uint8> ();
	Inhibit = File.Read<uint32> ();
	EntityNumber = File.Read<uint32> ();

#if ROGUE_FEATURES
	DisguiseViolationFrametime = File.Read<FrameNumber> ();

	Index = File.Read<sint32> ();
	if (Index != -1)
		DisguiseViolator = Game.Entities[Index].Entity;
#endif

	Entities.Load (File);
};

CGameLocals		Game;
CLevelLocals	Level;

/*
=================
EndDMLevel

The timelimit or fraglimit has been exceeded
=================
*/
void EndDMLevel ()
{
	char *s, *t, *f;
	static const char *seps = " ,\n\r";

	// stay on same level flag
	if (DeathmatchFlags.dfSameLevel.IsEnabled())
	{
		BeginIntermission (CreateTargetChangeLevel (Level.ServerLevelName.CString()));
		return;
	}

	if (!Level.ForceMap.IsNullOrEmpty())
	{
		BeginIntermission (CreateTargetChangeLevel (Level.ForceMap.CString()));
		return;
	}

	// see if it's in the map list
	if (*CvarList[CV_MAPLIST].StringValue())
	{
		s = Mem_StrDup(CvarList[CV_MAPLIST].StringValue());
		f = NULL;
		t = strtok(s, seps);

		while (t != NULL)
		{
			if (Q_stricmp(t, Level.ServerLevelName.CString()) == 0) {
				// it's in the list, go to the next one
				t = strtok(NULL, seps);

				if (t == NULL) { // end of list, go to first one
					if (f == NULL) // there isn't a first one, same level
						BeginIntermission (CreateTargetChangeLevel (Level.ServerLevelName.CString()) );
					else
						BeginIntermission (CreateTargetChangeLevel (f) );
				} else
					BeginIntermission (CreateTargetChangeLevel (t) );
				QDelete s;
				return;
			}
			if (!f)
				f = t;
			t = strtok(NULL, seps);
		}
		free(s);
	}

	if (!Level.NextMap.IsNullOrEmpty()) // go to a specific map
		BeginIntermission (CreateTargetChangeLevel (Level.NextMap.CString()) );
	else
	{	// search for a changelevel
		CTargetChangeLevel *Entity = CC_FindByClassName<CTargetChangeLevel, EF_BASE> (NULL, "target_changelevel");
		if (!Entity)
		{	// the map designer didn't include a changelevel,
			// so create a fake ent that goes back to the same level
			BeginIntermission (CreateTargetChangeLevel (Level.ServerLevelName.CString()) );
			return;
		}
		BeginIntermission (Entity);
	}
}

/*
=================
CheckDMRules
=================
*/
void CheckDMRules ()
{
	if (Level.Intermission.Time)
		return;

	if (!(Game.GameMode & GAME_DEATHMATCH))
		return;

#if CLEANCTF_ENABLED
//ZOID
	if ((Game.GameMode & GAME_CTF) && CTFCheckRules())
	{
		EndDMLevel ();
		return;
	}
//ZOID
#endif

	if (CvarList[CV_TIME_LIMIT].Float())
	{
		if (Level.Frame >= ((CvarList[CV_TIME_LIMIT].Float()*60)*10))
		{
			BroadcastPrint (PRINT_HIGH, "Timelimit hit.\n");
			EndDMLevel ();
			return;
		}
	}

	if (CvarList[CV_FRAG_LIMIT].Integer())
	{
		for (uint8 i = 0; i < Game.MaxClients; i++)
		{
			CPlayerEntity *cl = entity_cast<CPlayerEntity>(Game.Entities[i+1].Entity);
			if (!cl->GetInUse())
				continue;

			if (cl->Client.Respawn.Score >= CvarList[CV_FRAG_LIMIT].Integer())
			{
				BroadcastPrint (PRINT_HIGH, "Fraglimit hit.\n");
				EndDMLevel ();
				return;
			}
		}
	}
}

/*
=============
ExitLevel
=============
*/
void ExitLevel ()
{
	gi.AddCommandString (String::Format("gamemap \"%s\"\n", Level.Intermission.ChangeMap.CString()).CString());
#if STDCPP_LINUX_HACK
	if (Level.Intermission.ChangeMap)
	{
   	    QDelete[] Level.Intermission.ChangeMap;
	    Level.Intermission.ChangeMap = NULL;
	}
#else
	Level.Intermission.ChangeMap.Clear();
#endif
	Level.Intermission.ShouldExit = false;
	Level.Intermission.Time = 0;
	ClientEndServerFrames ();

	// clear some things before going to next level
	for (sint32 i = 0; i < Game.MaxClients; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[1 + i].Entity);
		if (!Player->GetInUse())
			continue;
		if (Player->Health > Player->Client.Persistent.MaxHealth)
			Player->Health = Player->Client.Persistent.MaxHealth;
	}
}

/*
=================
CheckNeedPass
=================
*/
inline void CheckNeedPass ()
{
	// if password or spectator_password has changed, update needpass
	// as needed
	if (CvarList[CV_PASSWORD].Modified() || CvarList[CV_SPECTATOR_PASSWORD].Modified()) 
	{
		sint32 need = 0;

		if (*CvarList[CV_PASSWORD].StringValue() && Q_stricmp(CvarList[CV_PASSWORD].StringValue(), "none"))
			need |= 1;
		if (*CvarList[CV_SPECTATOR_PASSWORD].StringValue() && Q_stricmp(CvarList[CV_SPECTATOR_PASSWORD].StringValue(), "none"))
			need |= 2;

		CvarList[CV_NEEDPASS].Set(need);
	}
}

void ClientEndServerFrames ()
{
	// calc the player views now that all pushing
	// and damage has been added
	for (sint32 i = 1; i <= Game.MaxClients ; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (!Player->GetInUse())
			continue;

		if (CvarList[CV_MAP_DEBUG].Boolean())
		{
			Mem_Zero (&Player->PlayedSounds, sizeof(Player->PlayedSounds));
			Player->BeginServerFrame ();
		}

		Player->EndServerFrame ();
	}
}

void CheckVersionReturnance ();

/*
================
RunFrame

Advances the world by 0.1 seconds
================
*/

/**
\fn	void RunPrivateEntities ()

\brief	Runs all private entities.
**/
void			RunPrivateEntities ();

#if defined(WIN32)
#include <crtdbg.h>
#endif

void ProcessEntity (SEntity *ent)
{
	if (!ent->Server.InUse)
	{
		if (ent->Server.State.Number > (Game.MaxClients + BODY_QUEUE_SIZE))
		{
			if (!ent->AwaitingRemoval)
			{
				ent->RemovalFrames = 2;
				ent->AwaitingRemoval = true;
			}
		}
		return;
	}

	if (ent->Entity)
	{
		IBaseEntity *Entity = ent->Entity;
		
		Mem_Zero (&Entity->PlayedSounds, sizeof(Entity->PlayedSounds));
		
		Level.CurrentEntity = Entity;
		Entity->State.GetOldOrigin() = Entity->State.GetOrigin();

		// if the ground entity moved, make sure we are still on it
		if ((!Entity->GroundEntity) || (Entity->GroundEntity->GetLinkCount() != Entity->GroundEntityLinkCount))
		{
			Entity->GroundEntity = nullentity;
			if ((Entity->EntityFlags & EF_MONSTER) && !(entity_cast<CMonsterEntity>(Entity)->Monster->AIFlags & (AI_SWIM | AI_FLY)))
				(entity_cast<CMonsterEntity>(Entity))->Monster->CheckGround ();
		}

		IThinkableEntity *Thinkable = (!Entity->Freed && (Entity->EntityFlags & EF_THINKABLE)) ? Entity->ThinkEntity : NULL;

		if (Thinkable) 
			Thinkable->PreThink ();

		Entity->Run ();

		if (Thinkable)
			Thinkable->RunThink ();

		// Were we freed?
		// This has to be processed after thinking and running, because
		// the entity still has to be intact after that
		if (Entity->Freed)
		{
			if (!ent->AwaitingRemoval)
			{
				ent->RemovalFrames = 2;
				ent->AwaitingRemoval = true;
			}
		}
	}
}

/**
\fn	bool RemoveEntity (SEntity *ent)

\brief	remove_if callback. Removes the entity 'ent' from the list if it needs to be removed. 

\author	Paril
\date	29/05/2010

\param [in,out]	ent	If non-null, the entity. 

\return	true if removed, false if not. 
**/
bool RemoveEntity (SEntity *ent)
{
	if (!ent || ent->Server.State.Number <= (Game.MaxClients + BODY_QUEUE_SIZE))
		return false;

	if (!ent->Entity || ent->AwaitingRemoval)
	{
		if (!ent->RemovalFrames)
		{
			ent->AwaitingRemoval = false;

			QDelete ent->Entity;
			ent->Entity = NULL;

			// Push into Open
			Level.Entities.Open.push_front (ent);
			return true;
		}

		ent->RemovalFrames--;
	}

	return false;
}
void CGameAPI::RunFrame ()
{
#if !NO_VERSION_CHECKING
	CheckVersionReturnance ();
#endif

	if (Level.Frame >= 3 && CvarList[CV_MAP_DEBUG].Boolean())
	{
		Level.Frame ++;
		// Run the players only
		// build the SPlayerState structures for all players
		ClientEndServerFrames ();
		return;
	}

	Level.Frame++;

	if (Level.Frame == 2)
		EndMapCounter();
	// exit intermissions

	if (Level.Intermission.ShouldExit)
	{
		if (Level.Intermission.ShouldExitOnNextFrame)
		{
			Level.Intermission.ShouldExitOnNextFrame = false;
			return;
		}
		ExitLevel ();
		return;
	}

	//
	// treat each object in turn
	// even the world gets a chance to think
	//
	std::for_each (Level.Entities.Closed.begin(), Level.Entities.Closed.end(), ProcessEntity);
	Level.Entities.Closed.remove_if (RemoveEntity);
#ifdef _DEBUG
	CC_ASSERT_EXPR ((Level.Entities.Closed.size() + Level.Entities.Open.size()) == (uint32)GameAPI.GetMaxEdicts(), "Entities don't equal max!");
#endif

	RunPrivateEntities ();
	RunTimers ();

	// see if it is time to end a deathmatch
	CheckDMRules ();

	// see if needpass needs updated
	CheckNeedPass ();

	// build the SPlayerState structures for all players
	ClientEndServerFrames ();

	if (CvarList[CV_DMFLAGS].Modified())
		DeathmatchFlags.UpdateFlags(CvarList[CV_DMFLAGS].Integer());

#if CLEANCODE_IRC
	UpdateIRCServers ();
#endif

#if ROGUE_FEATURES
	RunBadAreas ();
#endif

	// choose a client for monsters to target this frame
	// Only do it when we have spawned everything
	if (!(Game.GameMode & GAME_DEATHMATCH) && Level.Frame > 20) // Paril, lol
		AI_SetSightClient ();

	CModuleContainer::RunModules();
}

void SetupGamemode ()
{
	sint32 dmInt = CvarList[CV_DEATHMATCH].Integer(),
		coopInt = CvarList[CV_COOP].Integer();

	// Did we request deathmatch?
	if (dmInt)
	{
		// Did we also request coop?
		if (coopInt)
		{
			// Which one takes priority?
			if (dmInt > coopInt)
			{
				// We want deathmatch
				CvarList[CV_COOP].Set (0, false);
				CvarList[CV_DEATHMATCH].Set (1, false);
				// Let it fall through
			}
			else if (coopInt > dmInt)
			{
				// We want coop
				CvarList[CV_DEATHMATCH].Set (0, false);
				CvarList[CV_COOP].Set (1, false);
				Game.GameMode = GAME_COOPERATIVE;
				return;
			}
			// We don't know what we want, forcing DM
			else
			{
				CvarList[CV_COOP].Set (0, false);
				DebugPrintf		("CleanCode Warning: Both deathmatch and coop are 1; forcing to deathmatch.\n"
								 "Did you know you can make one take priority if you intend to only set one?\n"
								 "If deathmatch is 1 and you want to switch to coop, just type \"coop 2\" and change maps!\n");
				// Let it fall through
			}
		}
		Game.GameMode = GAME_DEATHMATCH;
	}
	// Did we request cooperative?
	else if (coopInt)
	{
		// All the above code handles the case if deathmatch is true.
		Game.GameMode = GAME_COOPERATIVE;
		return;
	}
	else
	{
		Game.GameMode = GAME_SINGLEPLAYER;
		return;
	}

	// If we reached here, we wanted deathmatch
#if CLEANCTF_ENABLED
	if (CvarList[CV_CTF].Integer())
		Game.GameMode |= GAME_CTF;
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

// Registers all cvars and commands
void G_Register ()
{
	Cvar_Register ();

	SetupArg ();
	Cmd_Register ();
	SvCmd_Register ();

	CModuleContainer::InitModules();
}

String ConfigTimeString ()
{
	return String::Format("%s (running on "COMBINED_BUILD_STRING")", TimeStamp());
}

void CGameAPI::Init ()
{
	CTimer LoadTimer;

	ServerPrintf ("==== InitGame ====\nRunning CleanCode Quake2 version "CLEANCODE_VERSION_PRINT", built on %s\nInitializing Game...\n", CLEANCODE_VERSION_PRINT_ARGS, ConfigTimeString().CString());

	seedMT (time(NULL));

	// Register cvars/commands
	G_Register();

	// File-system
	FS_Init (4);

	// Setup the gamemode
	SetupGamemode ();

	DeathmatchFlags.UpdateFlags(CvarList[CV_DMFLAGS].Integer());

	// items
	InitItemlist ();

	Game.HelpMessages[0].Clear();
	Game.HelpMessages[1].Clear();

	// initialize all entities for this game
	Game.MaxEntities = CvarList[CV_MAXENTITIES].Integer();
	Game.Entities = QNew (TAG_GAME) SEntity[Game.MaxEntities];
	GameAPI.GetEntities() = Game.Entities;
	GameAPI.GetMaxEdicts() = Game.MaxEntities;

	// initialize all clients for this game
	Game.MaxClients = CvarList[CV_MAXCLIENTS].Integer();
	Game.Clients = QNew (TAG_GAME) SServerClient[Game.MaxClients];
	GameAPI.GetNumEdicts() = Game.MaxClients + 1;

	// Vars
	Game.MaxSpectators = CvarList[CV_MAXSPECTATORS].Integer();
	Game.CheatsEnabled = (CvarList[CV_CHEATS].Integer()) ? true : false;

	Bans.LoadFromFile ();

#if 0
	LoadModules ();
	InitializeModules ();
#endif

	// Set R1 protocol flags
	Game.R1Protocol = (CCvar("protocol", "").Integer() == ENHANCED_PROTOCOL_VERSION);

	ServerPrintf ("\nGame initialized in "TIMER_STRING".\n", LoadTimer.Get());
}

void CGameAPI::Shutdown ()
{
	ServerPrint ("==== ShutdownGame ====\n");

	ShutdownBodyQueue ();
	Shutdown_Junk ();

	CModuleContainer::ShutdownModules();

	Bans.SaveList ();

	Mem_FreeTag (TAG_GAME);
	Mem_FreeTag (TAG_LEVEL);
	Mem_FreeTag (TAG_ENTITY);
}
