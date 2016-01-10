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
// cc_cmds.h
// Base class for commands
//

#if !defined(CC_GUARD_CMDS_H) || !INCLUDE_GUARDS
#define CC_GUARD_CMDS_H

#include "Player/Arg.h"

const int MAX_COMMANDS = 128;
const int MAX_CMD_HASH = (MAX_COMMANDS/4);

/**
\typedef	uint8 ECmdTypeFlags

\brief	Defines an alias representing the command type flags.
**/
typedef uint8 ECmdTypeFlags;

/**
\enum	

\brief	Values that represent flags for commands. 
**/
enum
{
	CMD_NORMAL		= 0, // Doesn't do anything at all, just here for show.

	CMD_SPECTATOR	= 1, // Spectators can use this command
	CMD_CHEAT		= 2 // Cheats 1 only
};

/**
\class	CCommandFunctor

\brief	Command functor.
		This is the class that you should inherit commands from.
		See "Usage - Commands"

\author	Paril
\date	28/05/2010
**/
class CCommandFunctor
{
	uint32		FirstArg, // first argument matched (in subcommands, will always be the main command arg index)
				ThisArg, // argument matched to this functor
				CurArg; // argument, starting at CurArg, that can be changed with the functions below

public:
	/**
	\fn	inline String GetFirstArgs ()
	
	\brief	Gets the first argument that this command was called with.
	
	\return	The first argument (string). 
	**/
	inline String GetFirstArgs () { return ArgGets(FirstArg); }

	/**
	\fn	inline int GetFirstArgi ()
	
	\brief	Gets the first argument that this command was called with.
	
	\return	The first argument (integer). 
	**/
	inline int GetFirstArgi () { return ArgGeti(FirstArg); }

	/**
	\fn	inline float GetFirstArgf ()
	
	\brief	Gets the first argument that this command was called with.
	
	\return	The first argument (float). 
	**/
	inline float GetFirstArgf () { return ArgGetf(FirstArg); }

	/**
	\fn	inline String GetThisArgs ()
	
	\brief	Gets the argument that matched to this function.
	
	\return	The matched argument (string). 
	**/
	inline String GetThisArgs () { return ArgGets(ThisArg); }

	/**
	\fn	inline int GetThisArgi ()
	
	\brief	Gets the argument that matched to this function.
	
	\return	The matched argument (integer). 
	**/
	inline int GetThisArgi () { return ArgGeti(ThisArg); }

	/**
	\fn	inline float GetThisArgf ()
	
	\brief	Gets the argument that matched to this function.
	
	\return	The matched argument (float). 
	**/
	inline float GetThisArgf () { return ArgGetf(ThisArg); }

	/**
	\fn	inline String GetNextArgs ()
	
	\brief	Gets the next argument. 
	
	\return	The next argument (string). 
	**/
	inline String GetNextArgs () { return ArgGets(++CurArg); }

	/**
	\fn	inline int GetNextArgi ()
	
	\brief	Gets the next argument. 
	
	\return	The next argument (integer). 
	**/
	inline int GetNextArgi () { return ArgGeti(++CurArg); }

	/**
	\fn	inline float GetNextArgf ()
	
	\brief	Gets the next argument. 
	
	\return	The next argument (float). 
	**/
	inline float GetNextArgf () { return ArgGetf(++CurArg); }

	/**
	\fn	virtual void Execute() = 0
	
	\brief	The function operator. Must be overridden to do the task.
	
	\author	Paril
	\date	28/05/2010
	**/
	virtual void Execute() = 0;

	/**
	\fn	void SetPos (uint32 First, uint32 This, uint32 Cur)
	
	\brief	Internal function to set the argument positions. 
	
	\author	Paril
	\date	28/05/2010
	
	\param	First	The first argument index. 
	\param	This	Matched argument index. 
	\param	Cur		The current argument. 
	**/
	void SetPos (uint32 First, uint32 This, uint32 Cur) { FirstArg = First; ThisArg = This; CurArg = Cur; };
};

/**
\class	CGameCommandFunctor

\brief	Game command functor.
		This is the base class for game commands.

\author	Paril
\date	29/05/2010
**/
class CGameCommandFunctor : public CCommandFunctor
{
public:
	CPlayerEntity	*Player;	// The player calling this function

	/**
	\fn	virtual void Execute() = 0
	
	\brief	The function operator. Must be overridden to do the task.
	
	\author	Paril
	\date	29/05/2010
	**/
	virtual void Execute() = 0;
};

/**
\class	CCommand

\brief	Internal command class.

\author	Paril
\date	29/05/2010
**/
class CCommand
{
public:
	/**
	\typedef	std::vector<CCommand*> TCommandListType
	
	\brief	Defines an alias representing type of the command list.
	**/
	typedef std::vector<CCommand*> TCommandListType;

	/**
	\typedef	std::multimap<size_t, size_t> THashedCommandListType
	
	\brief	Defines an alias representing type of the hashed command list.
	**/
	typedef std::multimap<size_t, size_t> THashedCommandListType;

	/**
	\struct	SSubCommands
	
	\brief	Subcommands structure. 
	
	\author	Paril
	\date	29/05/2010
	**/
	struct SSubCommands
	{
		CCommand					*Owner; // Pointer to the command that owns us

		TCommandListType			List;
		THashedCommandListType		HashedList;
	}
							SubCommands;
	char					*Name;	// The name of the command
	ECmdTypeFlags			Flags;	// Command flags
	CCommandFunctor			*Func;	// The functor

	/**
	\fn	CCommand (const char *Name, CCommandFunctor *Func, ECmdTypeFlags Flags)
	
	\brief	Constructor.
	
	\author	Paril
	\date	29/05/2010
	
	\param	Name			The name of the command.
	\param [in,out]	Func	The functor to execute for this command. 
	\param	Flags			The flags. 
	**/
	CCommand (const char *Name, CCommandFunctor *Func, ECmdTypeFlags Flags) :
	  Name(Mem_StrDup(Name)),
	  Flags(Flags),
	  Func(Func)
	  {
	  };
	
	/**
	\fn	virtual ~CCommand ()
	
	\brief	Destructor.
	
	\author	Paril
	\date	29/05/2010
	**/
	virtual ~CCommand ()
	{
		QDelete[] Name;
		QDelete Func;
	};

	/**
	\fn	CCommand &GoUp ()
	
	\brief	Go up in the hierarchy of commands.
			Used for subcommands.
	
	\return	If no owner, this, otherwise owner. 
	**/
	CCommand &GoUp ()
	{
		if (!SubCommands.Owner)
			return *this;
		return *SubCommands.Owner;
	};

	virtual void *NewOfMe (const char *Name, CCommandFunctor *Func, ECmdTypeFlags Flags) = 0;

	/**
	\fn	template <class TType> CCommand &AddSubCommand (const char *Name, ECmdTypeFlags Flags = 0)
	
	\brief	Adds a subcommand with 'Name' and 'Flags'.
			The functor is described with 'TType'.
	
	\author	Paril
	\date	29/05/2010
	
	\param	Name	The name of the subcommand. 
	\param	Flags	The flags of the subcommand. 
	
	\return	The subcommand. 
	**/
	template <class TType>
	CCommand &AddSubCommand (const char *Name, ECmdTypeFlags Flags = 0)
	{
		TType *Functor = QNew (TAG_GENERIC) TType;
		CCommand *NewCommand = (CCommand*)NewOfMe (Name, Functor, Flags);

		// We can add it!
		SubCommands.List.push_back (NewCommand);

		// Link it in the hash tree
		SubCommands.HashedList.insert (std::make_pair<size_t, size_t> (Com_HashGeneric (Name, MAX_CMD_HASH), SubCommands.List.size()-1));
		SubCommands.List[SubCommands.List.size()-1]->SubCommands.Owner = this;

		return *SubCommands.List[SubCommands.List.size()-1];
	};
};

template <class TReturnValue>
inline TReturnValue *RecurseSubCommands (uint32 &depth, TReturnValue *Cmd)
{
	if (Q_stricmp (Cmd->Name, ArgGets(depth).CString()) == 0)
	{
		depth++;
		if (ArgCount() > depth)
		{
			for (uint32 i = 0; i < Cmd->SubCommands.List.size(); i++)
			{
				TReturnValue *Found = static_cast<TReturnValue*>(RecurseSubCommands (depth, Cmd->SubCommands.List[i]));
				if (Found)
					return Found;
			}
		}
		depth--;
		return Cmd;
	}
	return NULL;
}

template <class TReturnValue, class TListType, class THashListType, class TItType, uint32 startDepth>
inline TReturnValue *FindCommand (const char *commandName, TListType &List, THashListType &HashList)
{
	uint32 hash = Com_HashGeneric(commandName, MAX_CMD_HASH);

	for (TItType it = HashList.equal_range(hash).first; it != HashList.equal_range(hash).second; ++it)
	{
		TReturnValue *Command = static_cast<TReturnValue*>(List.at((*it).second));
		if (Q_stricmp (Command->Name, commandName) == 0)
		{
			if (ArgCount() > 1)
			{
				for (uint32 i = 0; i < List.at((*it).second)->SubCommands.List.size(); i++)
				{
					uint32 depth = startDepth;
					TReturnValue *Found = static_cast<TReturnValue*>(RecurseSubCommands (depth, List.at((*it).second)->SubCommands.List[i]));
					if (Found)
					{
						if (Found->Func)
							Found->Func->SetPos (1, depth, depth);
						return Found;
					}
				}
			}

			if (Command->Func)
				Command->Func->SetPos (1, 1, 1);
			return Command;
		}
	}
	return NULL;
}

/**
\class	CPlayerCommand

\brief	Player command.
		Internal class for game commands.

\author	Paril
\date	29/05/2010
**/
class CPlayerCommand : public CCommand
{
public:
	/**
	\fn	CPlayerCommand (const char *Name, CGameCommandFunctor *Func, ECmdTypeFlags Flags)
	
	\brief	Constructor.
	
	\author	Paril
	\date	29/05/2010
	
	\param	Name			The name. 
	\param [in,out]	Func	If non-null, the functor. 
	\param	Flags			The flags. 
	**/
	CPlayerCommand (const char *Name, CGameCommandFunctor *Func, ECmdTypeFlags Flags) :
	  CCommand (Name, Func, Flags)
	  {
	  };

	/**
	\fn	void Run (CPlayerEntity *Player)
	
	\brief	Runs a command.
	
	\author	Paril
	\date	29/05/2010
	
	\param [in,out]	Player	If non-null, the player. 
	**/
	void Run (CPlayerEntity *Player);

	void *NewOfMe (const char *Name, CCommandFunctor *RealFunc, ECmdTypeFlags Flags)
	{
		return QNew (TAG_GENERIC) CPlayerCommand (Name, static_cast<CGameCommandFunctor*>(RealFunc), Flags);
	}

	/**
	\fn	template <class TType> CPlayerCommand &AddSubCommand (const char *Name,
		ECmdTypeFlags Flags = 0)
	
	\brief	Adds a subcommand with 'Name' and 'Flags'.
			The functor is described with 'TType'.
	
	\author	Paril
	\date	29/05/2010
	
	\param	Name	The name of the subcommand. 
	\param	Flags	The flags of the subcommand. 
	
	\return	The subcommand. 
	**/
	template <class TType>
	CPlayerCommand &AddSubCommand (const char *Name, ECmdTypeFlags Flags = 0)
	{
		return static_cast<CPlayerCommand&>(CCommand::AddSubCommand<TType> (Name, Flags));
	};
};

/**
\fn	void Cmd_RunCommand (const char *commandName, CPlayerEntity *Player)

\brief	Run command.
		Called by game API.

\author	Paril
\date	29/05/2010

\param	commandName		Name of the command. 
\param [in,out]	Player	If non-null, the player. 
**/
void Cmd_RunCommand (const char *commandName, CPlayerEntity *Player);

CPlayerCommand &Cmd_AddCommand_Internal (const char *commandName, CGameCommandFunctor *Functor, ECmdTypeFlags Flags = 0);

void Cmd_Game_RemoveAll ();

/**
\fn	template <class TFunctor> CPlayerCommand &Cmd_AddCommand (const char *commandName,
	ECmdTypeFlags Flags = 0)

\brief	Adds a game command to the list.

\author	Paril
\date	29/05/2010

\param	commandName	Name of the command. 
\param	Flags		The flags. 

\return	The command. 
**/
template <class TFunctor>
CPlayerCommand &Cmd_AddCommand (const char *commandName, ECmdTypeFlags Flags = 0)
{
	TFunctor *Functor = QNew (TAG_GENERIC) TFunctor;
	return Cmd_AddCommand_Internal (commandName, Functor, Flags);
}

/**
\fn	void AddTestDebugCommands ()

\brief	Adds test debug commands

\author	Paril
\date	29/05/2010
**/
void AddTestDebugCommands ();

#include "Player/GameCommands.h"

#else
FILE_WARNING
#endif
