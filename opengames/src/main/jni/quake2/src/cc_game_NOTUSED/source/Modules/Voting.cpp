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
// Voting.cpp
// Voting plugin for CleanCode
//

#include "Local.h"
#include "Entities/TargetEntities.h"
#include "Player/Ban.h"

typedef uint8 EVoteType;

enum
{
	VOTE_NONE,
	VOTE_MAP,
	VOTE_NEXTMAP,
	VOTE_KICK,
	VOTE_BAN
};

typedef uint8 EVoteState;

enum
{
	VOTE_UNDECIDED,
	VOTE_NO,
	VOTE_YES
};

class CVoteData : public IModuleData
{
public:
	EVoteState VoteState;

	CVoteData () :
	  VoteState(VOTE_UNDECIDED)
	  {
	  };
};

/*abstract*/ class CVoteStateData
{
public:
	virtual String Details() = 0;
	virtual void VotePassed() = 0;
};

class CVoteMapData : public CVoteStateData
{
public:
	bool			NextMap;
	String			Map;

	CVoteMapData (bool isNextMap, String mapName) :
	  NextMap(isNextMap),
	  Map(mapName)
	{
	}

	String Details()
	{
		return String::Format("%s%s", ((!NextMap) ? "Change the map to " : "Change the next map to "), Map.CString());
	}

	void VotePassed ()
	{
		if (!NextMap)
			BeginIntermission (CreateTargetChangeLevel(Map.CString()));
		else
			Level.NextMap = Map;
	}
};

void OnPlayerDisconnectVoteCheck (CPlayerEntity *Player);

class CVoteKickBanData : public CVoteStateData
{
public:
	int				PlayerNumber;
	bool			Ban;

	CVoteKickBanData (bool ban, int number) :
	  Ban(ban),
	  PlayerNumber(number)
	{
		PlayerEvents::PlayerDisconnected += OnPlayerDisconnectVoteCheck;
	}

	~CVoteKickBanData ()
	{
		PlayerEvents::PlayerDisconnected -= OnPlayerDisconnectVoteCheck;
	}

	String Details()
	{
		return String::Format("%s%s", ((!Ban) ? "Kick " : "Ban "), entity_cast<CPlayerEntity>(Game.Entities[PlayerNumber].Entity)->Client.Persistent.Name.CString());
	}

	void VotePassed ()
	{
		if (Ban)
			Bans.AddToList(entity_cast<CPlayerEntity>(Game.Entities[PlayerNumber].Entity)->Client.Persistent.IP, BAN_ENTER);

		gi.AddCommandString(String::Format("kick %d\n", PlayerNumber - 1).CString());
	}
};

const FrameNumber VoteFrameCount = 450;
const FrameNumber VoteNotificationTimes = 3;
const FrameNumber VoteNotificationTime = (VoteFrameCount / VoteNotificationTimes);
const int PercentRequired = 65;

class CVote
{
public:
	bool				Voting;
	FrameNumber			EndTime;
	FrameNumber			NextNotifyTime;
	EVoteType			Type;
	CVoteStateData		*Data;
	String				Starter;

	CVote(EVoteType VoteType) :
	  Voting(false),
	  EndTime(0),
	  Type(VoteType),
	  Data(NULL)
	{
	};

	void StartVote (EVoteType VoteType, CVoteStateData *StateData, String starter)
	{
		EndTime = Level.Frame + VoteFrameCount;
		Voting = true;
		Type = VoteType;
		Data = StateData;
		NextNotifyTime = Level.Frame + VoteNotificationTime;
		Starter = starter;

		BroadcastPrintf (PRINT_CHAT, "%s proposed a vote: %s. Type vote yes or vote no to vote.\n", Starter.CString(), StateData->Details().CString());
	};

	void Finished()
	{
		Voting = false;
		QDelete Data;
		Data = NULL;
	};

	void Cancel()
	{
		Finished();
	}
};

static CVote CurrentVote (VOTE_NONE);

void OnPlayerDisconnectVoteCheck (CPlayerEntity *Player)
{
	if (!CurrentVote.Voting)
		return;

	CVoteKickBanData *data = static_cast<CVoteKickBanData*>(CurrentVote.Data);

	if (data->PlayerNumber == Player->State.GetNumber())
	{
		BroadcastPrintf (PRINT_CHAT, "Vote cancelled: Player to be kicked left the server.\n");
		CurrentVote.Cancel();
	}
}

class VotingModule : public IModule
{
public:
	VotingModule () :
	  IModule ()
	  {
	  }

	const char *GetName()
	{
		return "Voting Module\n  Version 1.0\n  By Paril";
	};

	void Init ();

	void Shutdown()
	{
	};

	void RunFrame()
	{
		if (CurrentVote.Voting)
		{
			if (Level.Frame == CurrentVote.EndTime)
			{
				int votedYes = 0, votedNo = 0, votedUndecided = 0;

				// Tally votes
				for (int x = 1; x <= Game.MaxClients; ++x)
				{
					CPlayerEntity *player = entity_cast<CPlayerEntity>(Game.Entities[x].Entity);

					if (player->Client.Persistent.State != SVCS_SPAWNED)
						continue;

					CVoteData *data = CModuleContainer::RequestModuleData<CVoteData>(player, this);

					switch (data->VoteState)
					{
					case VOTE_UNDECIDED:
						votedUndecided++;
						break;
					case VOTE_YES:
						votedYes++;
						break;
					case VOTE_NO:
						votedNo++;
						break;
					}

					data->VoteState = VOTE_UNDECIDED; // reset state
				}

				int neededVotes = ((votedYes + votedNo + votedUndecided) * PercentRequired) / 100;

				CurrentVote.Voting = false;

				if (votedYes >= neededVotes)
					CurrentVote.Data->VotePassed();

				BroadcastPrintf (PRINT_HIGH, "Vote %s! %i voted yes, %i voted no, %i undecided.\n", (votedYes >= neededVotes) ? "passed" : "failed", votedYes, votedNo, votedUndecided);

				CurrentVote.Finished();
				return;
			}
			else if (Level.Frame == CurrentVote.NextNotifyTime)
			{
				BroadcastPrintf (PRINT_HIGH, "%i seconds left on vote!\n", (CurrentVote.EndTime - Level.Frame) / 10);
				CurrentVote.NextNotifyTime = Level.Frame + VoteNotificationTime;
			}
		}
	};

	IModuleData *ModuleDataRequested ()
	{
		return QNew(TAG_GENERIC) CVoteData;
	};
} votemodule;

// Commands
class CCmdVoteCommand : public CGameCommandFunctor
{
public:
	void Execute ()
	{
		Player->PrintToClient (PRINT_HIGH,
			"Supported commands:\n" \
			"  vote yes        - Vote yes to a current vote\n" \
			"  vote no         - Vote no to a current vote\n" \
			"  vote view       - See the details of the vote\n" \
			"  vote map n      - Vote to change the current map\n" \
			"  vote nextmap n  - Vote to change the next map\n" \
			"  vote kick n     - Vote to kick a player\n" \
			"  vote ban n      - Vote to ban a player\n");
	}
};

class CCmdVoteMapCommand : public CGameCommandFunctor
{
public:
	void Execute()
	{
		if (CurrentVote.Voting)
		{
			Player->PrintToClient(PRINT_HIGH, "Vote already in progress.");
			return;
		}

		if (ArgCount() != 3)
		{
			Player->PrintToClient (PRINT_HIGH, "Syntax: vote map mapname\n");
			return;
		}

		CVoteMapData *voteData = QNew(TAG_GENERIC) CVoteMapData(false, ArgGets(2));
		CurrentVote.StartVote(VOTE_MAP, voteData, Player->Client.Persistent.Name);
	}
};

class CCmdVoteNextMapCommand : public CGameCommandFunctor
{
public:
	void Execute()
	{
		if (CurrentVote.Voting)
		{
			Player->PrintToClient(PRINT_HIGH, "Vote already in progress.");
			return;
		}

		if (ArgCount() != 3)
		{
			Player->PrintToClient (PRINT_HIGH, "Syntax: vote nextmap mapname\n");
			return;
		}

		CVoteMapData *voteData = QNew(TAG_GENERIC) CVoteMapData(true, ArgGets(2));
		CurrentVote.StartVote(VOTE_NEXTMAP, voteData, Player->Client.Persistent.Name);
	}
};

class CCmdVoteYesCommand : public CGameCommandFunctor
{
public:
	void Execute()
	{
		if (!CurrentVote.Voting)
		{
			Player->PrintToClient(PRINT_HIGH, "No vote in progress.");
			return;
		}

		CVoteData *data = CModuleContainer::RequestModuleData<CVoteData>(Player, &votemodule);

		if (data->VoteState != VOTE_UNDECIDED)
		{
			Player->PrintToClient(PRINT_HIGH, "You've already voted!\n");
			return;
		}

		data->VoteState = VOTE_YES;
		Player->PrintToClient(PRINT_HIGH, "You voted yes.\n");
	}
};

class CCmdVoteNoCommand : public CGameCommandFunctor
{
public:
	void Execute()
	{
		if (!CurrentVote.Voting)
		{
			Player->PrintToClient(PRINT_HIGH, "No vote in progress.");
			return;
		}

		CVoteData *data = CModuleContainer::RequestModuleData<CVoteData>(Player, &votemodule);

		if (data->VoteState != VOTE_UNDECIDED)
		{
			Player->PrintToClient(PRINT_HIGH, "You've already voted!\n");
			return;
		}

		data->VoteState = VOTE_NO;
		Player->PrintToClient(PRINT_HIGH, "You voted no.\n");
	}
};

class CCmdVoteViewCommand : public CGameCommandFunctor
{
public:
	void Execute()
	{
		if (!CurrentVote.Voting)
		{
			Player->PrintToClient(PRINT_HIGH, "No vote in progress.");
			return;
		}

		Player->PrintToClient(PRINT_HIGH, "Current vote: %s\n", CurrentVote.Data->Details().CString());
	}
};

template <bool ban, EVoteType voteType>
class CCmdVoteBanCommand : public CGameCommandFunctor
{
public:
	void Execute()
	{
		if (CurrentVote.Voting)
		{
			Player->PrintToClient(PRINT_HIGH, "Vote already in progress.");
			return;
		}

		if (ArgCount() != 3)
		{
			Player->PrintToClient (PRINT_HIGH, "Use \"players\" to check the player IDs for kick-by-ID. Syntax:\n  vote ban/kick n:id\n  vote ban/kick p:playerName\n\n  Example: vote ban/kick n:8\n  Example: vote ban/kick p:Paril\n");
			return;
		}

		String str = ArgGets(2);

		if (str.Count() < 3 ||
			str[1] != ':' ||
			(str[0] != 'p' && str[0] != 'n'))
		{
			Player->PrintToClient (PRINT_HIGH, "Syntax error. Type \"vote ban\" or \"vote kick\" to see syntax.\n");
			return;
		}

		int playerToKick = -1;

		if (str[0] == 'p')
		{
			String playerName = str.Substring(2).ToLower();

			if (playerName.IsNullOrEmpty())	
			{
				Player->PrintToClient (PRINT_HIGH, "Syntax error. Type \"vote ban\" or \"vote kick\" to see syntax.\n");
				return;
			}

			for (int i = 1; i <= Game.MaxClients; ++i)
			{
				if (entity_cast<CPlayerEntity>(Game.Entities[i].Entity)->Client.Persistent.Name.Clone().ToLower() == playerName)
				{
					if (playerToKick != -1)
					{
						Player->PrintToClient (PRINT_HIGH, "Multiple players exist by that name. Type \"vote ban\" or \"vote kick\" to see how to ban by player number instead.\n");
						return;
					}

					playerToKick = i;
				}
			}

			if (playerToKick == -1)
			{
				Player->PrintToClient (PRINT_HIGH, "Player does not exist.\n");
				return;
			}
		}
		else
		{
			String playerNum = str.Substring(2);

			for (size_t i = 0; i < playerNum.Count(); ++i)
			{
				if (playerNum[i] < '0' || playerNum[i] > '9')
				{
					Player->PrintToClient (PRINT_HIGH, "Invalid player number.\n");
					return;
				}
			}

			playerToKick = atoi(str.Substring(2).CString());

			if (playerToKick <= 0 || playerToKick > Game.MaxClients)
			{
				Player->PrintToClient (PRINT_HIGH, "Invalid player number.\n");
				return;
			}
		}

		CVoteKickBanData *voteData = QNew(TAG_GENERIC) CVoteKickBanData(ban, playerToKick);
		CurrentVote.StartVote(voteType, voteData, Player->Client.Persistent.Name);
	}
};

void VotingModule::Init ()
{
	Cmd_AddCommand<CCmdVoteCommand> ("vote", CMD_SPECTATOR)
		.AddSubCommand<CCmdVoteMapCommand> ("map", CMD_SPECTATOR).GoUp()
		.AddSubCommand<CCmdVoteNextMapCommand> ("nextmap", CMD_SPECTATOR).GoUp()
		.AddSubCommand<CCmdVoteBanCommand<false, VOTE_KICK> > ("kick", CMD_SPECTATOR).GoUp()
		.AddSubCommand<CCmdVoteBanCommand<true, VOTE_BAN> > ("ban", CMD_SPECTATOR).GoUp()
		.AddSubCommand<CCmdVoteYesCommand> ("yes", CMD_SPECTATOR).GoUp()
		.AddSubCommand<CCmdVoteNoCommand> ("no", CMD_SPECTATOR).GoUp()
		.AddSubCommand<CCmdVoteViewCommand> ("view", CMD_SPECTATOR).GoUp()
		;
};
