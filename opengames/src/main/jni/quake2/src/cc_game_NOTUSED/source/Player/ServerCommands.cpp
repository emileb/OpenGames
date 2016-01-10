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
// cc_servercommands.cpp
// ServerCommand & related functions
//

#include "Local.h"
#include "Utility/ExceptionHandler.h"
#include "Utility/Version.h"
#include "Player/ServerCommands.h"

typedef CCommand::TCommandListType TServerCommandListType;
typedef CCommand::THashedCommandListType THashedServerCommandListType;

TServerCommandListType ServerCommandList;
THashedServerCommandListType ServerCommandHashList;

static void Cmd_Server_DeleteAndRecurse (CCommand *Cmd)
{
	for (size_t z = 0; z < Cmd->SubCommands.List.size(); ++z)
	{
		Cmd_Server_DeleteAndRecurse (Cmd->SubCommands.List[z]);
		QDelete Cmd->SubCommands.List[z];
	}
}

void Cmd_Server_RemoveAll ()
{
	for (size_t i = 0; i < ServerCommandList.size(); ++i)
		Cmd_Server_DeleteAndRecurse(ServerCommandList[i]);

	ServerCommandList.clear();
	ServerCommandHashList.clear();
}

CServerCommand *SvCmd_FindCommand (const char *commandName)
{
	return FindCommand <CServerCommand, TServerCommandListType, THashedServerCommandListType, THashedServerCommandListType::iterator, 2> (commandName, ServerCommandList, ServerCommandHashList);
}

CServerCommand &SvCmd_AddCommand_Internal (const char *commandName, CCommandFunctor *Functor)
{
	// Make sure the function doesn't already exist
	if (CC_ASSERT_EXPR (!SvCmd_FindCommand(commandName), "Attempted to re-add a command to the list!\n"))
		return *static_cast<CServerCommand*>(ServerCommandList[0]);

	// We can add it!
	ServerCommandList.push_back (QNew (TAG_GENERIC) CServerCommand (commandName, Functor));

	// Link it in the hash tree
	ServerCommandHashList.insert (std::make_pair<size_t, size_t> (Com_HashGeneric (commandName, MAX_CMD_HASH), ServerCommandList.size()-1));

	return *static_cast<CServerCommand*>(ServerCommandList[ServerCommandList.size()-1]);
}

void SvCmd_RunCommand (const char *commandName)
{
	static CServerCommand *Command;

	if ((Command = SvCmd_FindCommand(commandName)) != NULL)
		Command->Func->Execute();
	else
		ServerPrintf ("Unknown server command \"%s\"\n", commandName);
}

class CServerCmdEntList : public CCommandFunctor
{
	struct SServerEntityListEntity
	{
		vec3f		Origin;
	};

	struct SServerEntityListIndex
	{
		const char	*className;
		uint32		Num;
		std::vector<SServerEntityListEntity> List;

		SServerEntityListIndex			*hashNext;
		uint32							hashValue;

		SServerEntityListIndex (const char *name) :
			className(name),
			hashValue (Com_HashGeneric(className, MAX_CS_EDICTS)),
			Num (0),
			List()
		{
		};
	};

	class CServerEntityList
	{
	public:
		SServerEntityListIndex		**List;
		SServerEntityListIndex		**HashedList;
		uint32						NumInList;

		CServerEntityList () :
			NumInList (0)
		{
			List = QNew (TAG_GENERIC) SServerEntityListIndex*[MAX_CS_EDICTS];
			HashedList = QNew (TAG_GENERIC) SServerEntityListIndex*[MAX_CS_EDICTS];
			Mem_Zero (List, sizeof(List));
			Mem_Zero (HashedList, sizeof(HashedList));
		};

		~CServerEntityList ()
		{
			for (uint32 i = 0; i < NumInList; i++)
				QDelete List[i];
			QDelete[] List;
			QDelete[] HashedList;
		};

		SServerEntityListIndex *Exists (const char *className)
		{
			uint32 Hash = Com_HashGeneric (className, MAX_CS_EDICTS);

			for (SServerEntityListIndex *Index = HashedList[Hash]; Index; Index = Index->hashNext)
			{
				if (Q_stricmp(Index->className, className) == 0)
					return Index;
			}
			return NULL;
		};

		SServerEntityListIndex *AddToList (const char *className)
		{
			SServerEntityListIndex *Ind = QNew (TAG_GENERIC) SServerEntityListIndex(className);
			List[NumInList] = Ind;
			List[NumInList]->hashNext = HashedList[List[NumInList]->hashValue];
			HashedList[List[NumInList]->hashValue] = List[NumInList];
			NumInList++;

			return Ind;
		};

		void Search (const char *WildCard)
		{
			for (TEntitiesContainer::iterator it = Level.Entities.Closed.begin(); it != Level.Entities.Closed.end(); ++it)
			{
				IBaseEntity *e = (*it)->Entity;

				if (!e || !e->GetInUse())
					continue;
				if (!Q_WildcardMatch (WildCard, e->ClassName.CString(), true))
					continue;

				SServerEntityListIndex *Index = Exists(e->ClassName.CString());

				if (!Index)
					Index = AddToList (e->ClassName.CString());

				vec3f Origin = (e->State.GetOrigin() == vec3fOrigin && (e->GetSolid() == SOLID_BSP)) ? e->GetAbsMin() : e->State.GetOrigin();

				SServerEntityListEntity index = {Origin};
				Index->List.push_back (index);

				Index->Num++;
			}
		};
	};

public:
	void Execute ()
	{
		String WildCard = (!ArgGets(2).IsNullOrEmpty()) ? ArgGets(2) : String("*");
		CServerEntityList tmp;
		tmp.Search (WildCard.CString());

		ServerPrintf ("Entity Stats\n"
				"             amount       origin             classname\n"
				"----         ------       ----------------   --------------------\n");

		sint32 newCount = 0;
		for (uint32 i = 0; i < tmp.NumInList; i++)
		{
			ServerPrintf (
				"#%3u          %5u                          %s\n",
				i, tmp.List[i]->Num, tmp.List[i]->className);

			for (size_t z = 0; z < tmp.List[i]->List.size(); z++)
			{
				SServerEntityListEntity &ent = tmp.List[i]->List[z];

				ServerPrintf (
				"[%2u]                         %4.0f %4.0f %4.0f\n",
				z+1, ent.Origin.X, ent.Origin.Y, ent.Origin.Z);
			}

			newCount += tmp.List[i]->Num;
		}

		ServerPrintf (	"----         ------       --------------------------\n"
						"Tally:       %5u          \n",					
		newCount);
	}
};

extern char *gEntString;
class CServerCmdDumpEntities : public CCommandFunctor
{
public:
	void Execute () 
	{
		CFile File (String::Format("/maps/ents/%s.ccent", Level.ServerLevelName.CString()).CString(), FILEMODE_CREATE | FILEMODE_WRITE);

		if (!File.Valid())
			return;

		File.Write (gEntString);
	}
};

#include "Player/Ban.h"
IPAddress CopyIP (const char *val);

class CServerCmdBan : public CCommandFunctor
{
public:
	void Execute ()
	{
		ServerPrintf (
		"Unknown ban command \"%s\"\n"
		"List of available ban commands:\n"
		"list                       List all current bans\n"
		"save                       Saves current bans to bans.lst\n"
		"load                       Loads bans from bans.lst\n"
		"name x [remove] flags      Ban name from server\n"
		"ip x [remove] flags        Ban IP from server\n\n"
		
		"Available ban flags:\n"
		"%u       Squelch\n"
		"%u       Spectator Mode\n"
		"%u       Entering Game\n"
		, GetNextArgs().CString(), BAN_SQUELCH, BAN_SPECTATOR, BAN_ENTER);
	}

	class CServerCmdBanLoad : public CCommandFunctor
	{
	public:
		void Execute ()
		{
			Bans.LoadFromFile ();
		}
	};

	class CServerCmdBanSave : public CCommandFunctor
	{
	public:
		void Execute ()
		{
			Bans.SaveList ();
		}
	};

	class CServerCmdBanList : public CCommandFunctor
	{
	public:
		void Execute ()
		{
			ServerPrintf (
				"Name/IP                   IP       Flags\n"
				"---------------------    ----      -----\n");

			for (TBanIndexContainer::iterator it = Bans.BanList.begin(); it < Bans.BanList.end(); ++it)
			{
				BanIndex *Index = *it;

				if (!Index->IP)
					continue;

				ServerPrintf ("%-24s Yes         %u\n", Index->Address->str, Index->Flags);
			}

			for (TBanIndexContainer::iterator it = Bans.BanList.begin(); it < Bans.BanList.end(); ++it)
			{
				BanIndex *Index = *it;

				if (Index->IP)
					continue;

				ServerPrintf ("%-24s No          %u\n", Index->Name, Index->Flags);
			}
		}
	};

	class CServerCmdBanName : public CCommandFunctor
	{
	public:
		void Execute ()
		{
			String cmd = GetNextArgs();

			if (cmd.CompareCaseInsensitive("remove"))
			{
				String name = GetNextArgs();
				if (Bans.RemoveFromList (name.CString()))
					ServerPrintf ("Removed %s from ban list\n", name.CString());
				else
					ServerPrintf ("%s not found in ban list\n", name.CString());
			}
			else
			{
				String name = cmd;
				const uint8 flags = GetNextArgi();

				if (Bans.InList (name.CString()))
				{
					if (Bans.ChangeBan (name.CString(), flags))
						ServerPrintf ("%s flags changed to %u\n", name.CString(), flags);
					else
						ServerPrintf ("%s already has flags %u\n", name.CString(), flags);
				}
				else if (Bans.AddToList (name.CString(), flags))
					ServerPrintf ("Added %s with flags %u to ban list\n", name.CString(), flags);
				else
					ServerPrintf ("%s not found in ban list\n", name.CString());
			}
		}
	};

	class CServerCmdBanIP : public CCommandFunctor
	{
	public:
		void Execute ()
		{
			String cmd = GetNextArgs();

			if (cmd.CompareCaseInsensitive("remove") == 0)
			{
				IPAddress Ip = CopyIP(GetNextArgs().CString());
				if (Bans.RemoveFromList (Ip))
					ServerPrintf ("Removed %s from ban list\n", Ip.str);
				else
					ServerPrintf ("%s not found in ban list\n", Ip.str);
			}
			else
			{
				IPAddress Ip = CopyIP(cmd.CString());
				const uint8 flags = GetNextArgi();

				if (Bans.InList (Ip))
				{
					if (Bans.ChangeBan (Ip, flags))
						ServerPrintf ("%s flags changed to %u\n", Ip.str, flags);
					else
						ServerPrintf ("%s already has flags %u\n", Ip.str, flags);
				}
				else if (Bans.AddToList (Ip, flags))
					ServerPrintf ("Added %s with flags %u to ban list\n", Ip.str, flags);
				else
					ServerPrintf ("%s not found in ban list\n", Ip.str);
			}
		}
	};
};

class CServerCmdTest : public CCommandFunctor
{
public:
	void Execute ()
	{
		ServerPrint ("Test\n");
	};
};

void SvCmd_Register ()
{
	Cmd_Server_RemoveAll();

	SvCmd_AddCommand<CServerCmdTest> ("test");
	SvCmd_AddCommand<CServerCmdEntList> ("entlist");
	SvCmd_AddCommand<CServerCmdIndexList> ("indexlist");
	SvCmd_AddCommand<CServerCmdDumpEntities> ("dump");
	SvCmd_AddCommand<CServerVersionCommand> ("cc_version");
	SvCmd_AddCommand<CServerCmdBan> ("ban")
		.AddSubCommand<CServerCmdBan::CServerCmdBanIP> ("ip").GoUp()
		.AddSubCommand<CServerCmdBan::CServerCmdBanName> ("name").GoUp()
		.AddSubCommand<CServerCmdBan::CServerCmdBanList> ("list").GoUp()
		.AddSubCommand<CServerCmdBan::CServerCmdBanSave> ("save").GoUp()
		.AddSubCommand<CServerCmdBan::CServerCmdBanLoad> ("load");

#if CLEANCODE_IRC
	SvCmd_AddCommand<CSvIRCConnectToCommand> ("irc");
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
void CGameAPI::ServerCommand ()
{
	InitArg ();
	SvCmd_RunCommand (ArgGets(1).CString());
	EndArg ();
}


