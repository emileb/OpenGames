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
// cc_cmds.cpp
// Base class for commands
//

#include "Local.h"

/**
\fn	void CPlayerCommand::Run (CPlayerEntity *Player)
	
\brief	Runs a command.
	
\author	Paril
\date	29/05/2010
	
\param [in,out]	Player	If non-null, the player. 
**/
void CPlayerCommand::Run (CPlayerEntity *Player)
{
	if ((Flags & CMD_CHEAT) && !Game.CheatsEnabled && !(Game.GameMode & GAME_SINGLEPLAYER))
	{
		Player->PrintToClient (PRINT_HIGH, "Cheats must be enabled to use this command.\n");
		return;
	}
	if (!(Flags & CMD_SPECTATOR) && (Player->Client.Respawn.Spectator || Player->Client.Chase.Target))	
		return;

	Func->Execute();
};

typedef CCommand::TCommandListType TPlayerCommandListType;
typedef CCommand::THashedCommandListType THashedPlayerCommandListType;

TPlayerCommandListType CommandList;
THashedPlayerCommandListType CommandHashList;

static void Cmd_Game_DeleteAndRecurse (CCommand *Cmd)
{
	for (size_t z = 0; z < Cmd->SubCommands.List.size(); ++z)
	{
		Cmd_Game_DeleteAndRecurse (Cmd->SubCommands.List[z]);
		QDelete Cmd->SubCommands.List[z];
	}
}

void Cmd_Game_RemoveAll ()
{
	for (size_t i = 0; i < CommandList.size(); ++i)
		Cmd_Game_DeleteAndRecurse(CommandList[i]);

	CommandList.clear();
	CommandHashList.clear();
}

CPlayerCommand *Cmd_FindCommand (const char *commandName)
{
	return FindCommand <CPlayerCommand, TPlayerCommandListType, THashedPlayerCommandListType, THashedPlayerCommandListType::iterator, 1> (commandName, CommandList, CommandHashList);
}

CPlayerCommand &Cmd_AddCommand_Internal (const char *commandName, CGameCommandFunctor *Functor, ECmdTypeFlags Flags)
{
	// Make sure the function doesn't already exist
	if (CC_ASSERT_EXPR (!Cmd_FindCommand(commandName), "Tried to re-add a command, fatal error"))
		return *static_cast<CPlayerCommand*>(CommandList[0]);

	// We can add it!
	CommandList.push_back (QNew (TAG_GENERIC) CPlayerCommand (commandName, Functor, Flags));

	// Link it in the hash tree
	CommandHashList.insert (std::make_pair<size_t, size_t> (Com_HashGeneric (commandName, MAX_CMD_HASH), CommandList.size()-1));

	return *static_cast<CPlayerCommand*>(CommandList[CommandList.size()-1]);
}

/**
\fn	void Cmd_RunCommand (const char *commandName, CPlayerEntity *Player)

\brief	Run command.
		Called by game API.

\author	Paril
\date	29/05/2010

\param	commandName		Name of the command. 
\param [in,out]	Player	If non-null, the player. 
**/
void Cmd_RunCommand (const char *commandName, CPlayerEntity *Player)
{
	static CPlayerCommand *Command;

	if ((Command = Cmd_FindCommand(commandName)) != NULL)
	{
		static_cast<CGameCommandFunctor*>(Command->Func)->Player = Player;
		Command->Run(Player);
	}
	else
		Player->PrintToClient (PRINT_HIGH, "Unknown command \"%s\"\n", commandName);
}

void SearchForRandomMonster (CMonsterEntity *Entity)
{
	static std::vector <CMonsterEntity *> ChosenMonsters;
	for (TEntitiesContainer::iterator it = Level.Entities.Closed.begin(); it != Level.Entities.Closed.end(); ++it)
	{
		SEntity *ent = (*it);

		if (!ent->Entity || !ent->Entity->GetInUse())
			continue;
		if (!(ent->Entity->EntityFlags & EF_MONSTER))
			continue;
		if (ent->Entity == Entity)
			continue;
		if (!IsVisible(ent->Entity, Entity))
			continue;

		CMonsterEntity *WantedMonster = entity_cast<CMonsterEntity>(ent->Entity);
		if (WantedMonster->Health <= 0)
			continue;
		if (!(WantedMonster->GetSvFlags() & SVF_MONSTER))
			continue;

		ChosenMonsters.push_back (WantedMonster);
	}

	// Pick a random one
	if (!ChosenMonsters.size())
		return;

	CMonsterEntity *RandomPick = ChosenMonsters[irandom(ChosenMonsters.size())];
	Entity->Enemy = RandomPick;
	Entity->Monster->FoundTarget ();
	if (Entity->Monster->MonsterFlags & MF_HAS_SIGHT)
		Entity->Monster->Sight ();

	ChosenMonsters.clear ();
}

class CTestCommand : public CGameCommandFunctor
{
public:
	void Execute ()
	{
		if (ArgCount() < 3)
			return;
	
		Player->State.GetOrigin().Set (GetNextArgf(), GetNextArgf(), GetNextArgf());
		Player->Link ();
	};

	// Subcommands
	class CTestCommandTwo : public CGameCommandFunctor
	{
	public:
		void Execute ()
		{
			for (TEntitiesContainer::iterator it = Level.Entities.Closed.begin(); it != Level.Entities.Closed.end(); ++it)
			{
				if (!(*it)->Entity || !((*it)->Entity->EntityFlags & EF_MONSTER))
					continue;

				CMonsterEntity *Monster = entity_cast<CMonsterEntity>((*it)->Entity);
				if (Monster->Health <= 0)
					continue;

				SearchForRandomMonster (Monster);
			}
		};
	};
};

/**
\fn	void AddTestDebugCommands ()

\brief	Adds test debug commands

\author	Paril
\date	29/05/2010
**/
void AddTestDebugCommands ()
{
	Cmd_AddCommand<CTestCommand> ("test")
		.AddSubCommand<CTestCommand::CTestCommandTwo> ("two");
}
