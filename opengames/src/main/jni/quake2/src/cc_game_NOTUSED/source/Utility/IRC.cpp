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
// cc_irc.cpp
// 
//

#include "Local.h"

#if CLEANCODE_IRC

CIRCClientServerChannel *CIRCClientServer::FindChannel (String ChannelName)
{
	for (TIRCChannels::iterator it = Channels.begin(); it != Channels.end(); ++it)
	{
		CIRCClientServerChannel &Channel = (*it);

		if (Channel.ChannelName == ChannelName)
			return &Channel;
	}
	return NULL;
};

void CIRCClientServerChannel::Join (CPlayerEntity *Player)
{
	Players.push_back (Player);

	Server->PushMessage(IRC::PRIVMSG, String::Format("%s :%s has joined channel %s", ChannelName.CString(), Player->Client.Respawn.IRC.Nick.CString(), ChannelName.CString()));
	for (TConnectedIRCPlayers::iterator it = Players.begin(); it != Players.end(); ++it)
		(*it)->PrintToClient (PRINT_CHAT, "[IRC] %s has joined channel %s\n", Player->Client.Respawn.IRC.Nick.CString(), ChannelName.CString());
	
	Player->Client.Respawn.IRC.Channel = ChannelName;
};

void CIRCClientServerChannel::Leave (CPlayerEntity *Player)
{
	Server->PushMessage(IRC::PRIVMSG, String::Format("%s :%s has left channel %s", ChannelName.CString(), Player->Client.Respawn.IRC.Nick.CString(), ChannelName.CString()));
	for (TConnectedIRCPlayers::iterator it = Players.begin(); it != Players.end(); ++it)
		(*it)->PrintToClient (PRINT_CHAT, "[IRC] %s has left channel %s\n", Player->Client.Respawn.IRC.Nick.CString(), ChannelName.CString());

	for (TConnectedIRCPlayers::iterator it = Players.begin(); it != Players.end(); ++it)
	{
		if ((*it) == Player)
		{
			Players.erase (it);
			break;
		}
	}

	Player->Client.Respawn.IRC.Channel.Clear();
};

bool CIRCClientServer::Connected ()
{
	return (IRCServer.status != IRC::DISCONNECTED);
};

void CIRCClientServer::Disconnect ()
{
	for (TConnectedIRCPlayers::iterator it = ConnectedPlayers.begin(); it != ConnectedPlayers.end(); ++it)
		(*it)->PrintToClient (PRINT_HIGH, "Disconnected from %s\n", IRCServer.server.c_str());

	irc_disconnect (&IRCServer, "Forced disconnect");
};

void CIRCClientServer::Connect (String HostName, String Nick, String User, String Pass, String RealName, int port)
{
	if (Connected ())
		return;

	IRCServer.port = port;
	IRCServer.server = HostName.CString();
	IRCServer.nick = Nick.CString();
	IRCServer.user = User.CString();
	IRCServer.pass = Pass.CString();
	IRCServer.real_name = RealName.CString();

    irc_ini_server(&IRCServer);

	irc_connect (&IRCServer);
	_net_setnonblocking (IRCServer.sock);
};

class CIRCListPlayerCallback : public CForEachPlayerCallback
{
public:
	CIRCClientServer		*Server;
	String					&chan;
	int						Count;

	CIRCListPlayerCallback (CIRCClientServer *Server, String &chan) :
	  Server(Server),
	  chan(chan),
	  Count(0)
	{
	};

	CIRCListPlayerCallback &operator= (CIRCListPlayerCallback&) { return *this; }

	void Callback (CPlayerEntity *Player)
	{
		Server->PushMessage (IRC::PRIVMSG, String::Format("%s :%i:   %s%s", chan.CString(), Count, Player->Client.Persistent.Name.CString(), (Player->Client.Respawn.IRC.Connected()) ? String::Format(" [In IRC as %s]", Player->Client.Respawn.IRC.Nick.CString()).CString() : ""));
	}
};

// One update frame
void CIRCClientServer::Update ()
{
	int retVal = irc_receive(&IRCServer, 0);
	if (retVal >= 0)
	{
		// If read characters is > 0 ...
		if (IRCServer.received > 0)
		{
			// If server pings, we pong.
			switch (IRCServer.msg.command)
			{
			case IRC::RPL_CREATED:
				HostName = String(IRCServer.msg.prefix.c_str());
				break;
			case IRC::PING:
				irc_pong(&IRCServer);
				break;
			case IRC::PRIVMSG:
				{
					// Get name
					String otherName = String(IRCServer.msg.prefix.c_str());
					String message = String(IRCServer.msg.param[1].c_str()).Substring(1);

					otherName = otherName.Substring(0, otherName.IndexOf('!'));

					bool isAction = (message.Count() > 6 && message[0] == 1 && (strncmp(message.CString()+1, "ACTION ", 7) == 0));

					// Trim whitespace
					while (message[0] == ' ' || message[0] == 1 || message[0] == 13)
						message.Remove(0, 1);
					while (message[message.Count()-1] == ' ' || message[message.Count()-1] == 1 || message[message.Count()-1] == 13)
						message.Remove (message.Count()-1, 1);

					// Trim "ACTION "
					if (isAction)
						message.Remove (0, 7);

					String channel = String(IRCServer.msg.param[0].c_str());
					CIRCClientServerChannel *Chan = FindChannel(channel);

					if (Q_strnicmp(message.CString(), "!Q2Serv ", 8) == 0)
					{
						String cmds = message.Substring(8);

						if (Q_stricmp(cmds.CString(), "players") == 0)
						{
							PushMessage (IRC::PRIVMSG, String::Format("%s :The following players are in the server:", channel.CString()));
							CIRCListPlayerCallback(this, channel).Query();
						}
						break;
					}
					else if (Q_stricmp(channel.CString(), "Q2Serv") == 0)
					{
						PushMessage (IRC::PRIVMSG, String::Format("%s :I am not a bot, just a redirection service. Commands list coming soon.", otherName.CString()));
						break;
					}

					for (TConnectedIRCPlayers::iterator it = Chan->Players.begin(); it != Chan->Players.end(); ++it)
					{
						if (!isAction)
							(*it)->PrintToClient (PRINT_CHAT, "[IRC] <%s> %s\n", otherName.CString(), message.CString());
						else
							(*it)->PrintToClient (PRINT_CHAT, "[IRC] %s %s\n", otherName.CString(), message.CString());
					}
				}
				break;
			case IRC::JOIN:
				{
					// Get name
					String otherName = String(IRCServer.msg.prefix.c_str());
					otherName = otherName.Substring(0, otherName.IndexOf('!'));

					if (otherName != IRCServer.nick.c_str())
					{
						String channel = String(IRCServer.msg.params.c_str());
						channel = channel.Substring(1, channel.Count()-2);
						CIRCClientServerChannel *Chan = FindChannel(channel);

						for (TConnectedIRCPlayers::iterator it = Chan->Players.begin(); it != Chan->Players.end(); ++it)
							(*it)->PrintToClient (PRINT_CHAT, "[IRC] %s has %s channel %s\n", otherName.CString(), (IRCServer.msg.command == IRC::JOIN) ? "joined" : "left", channel.CString());
					}
				}
				break;
			case IRC::PONG:
				CanSendMessages = true;
				break;
			}
		}

		IRCServer.received = 0;
	}
	else if (retVal < 0)
	{
		DebugPrint ("IRC Shit broke\n");
		Disconnect ();
	}

	// Send messages
	SendMsgQueue ();
};

void CIRCClientServer::SendMessage (CPlayerEntity *Player, String Message)
{
	// "/me "
	if (Message.StartsWith("/me "))
	{
		Message.Remove (0, 4);
		PushMessage(IRC::PRIVMSG, String::Format("%s :%cACTION >>>> %s %s", Player->Client.Respawn.IRC.Channel.CString(), 1, Player->Client.Respawn.IRC.Nick.CString(), Message.CString()));
	
		for (TConnectedIRCPlayers::iterator it = ConnectedPlayers.begin(); it != ConnectedPlayers.end(); ++it)
			(*it)->PrintToClient (PRINT_CHAT, "[IRC] %s %s\n", Player->Client.Respawn.IRC.Nick.CString(), Message.CString());
	}
	else
	{
		PushMessage(IRC::PRIVMSG, String::Format("%s :<%s> %s", Player->Client.Respawn.IRC.Channel.CString(), Player->Client.Respawn.IRC.Nick.CString(), Message.CString()));
	
		for (TConnectedIRCPlayers::iterator it = ConnectedPlayers.begin(); it != ConnectedPlayers.end(); ++it)
			(*it)->PrintToClient (PRINT_CHAT, "[IRC] <%s> %s\n", Player->Client.Respawn.IRC.Nick.CString(), Message.CString());
	}
};

void CIRCClientServer::PushMessage (int Cmd, String Str)
{
	IRCMsgQueue.push_back (TIRCMessage(Cmd, Str));

	size_t count = 0;
	for (size_t i = 0; i < IRCMsgQueue.size(); ++i)
	{
		if (IRCMsgQueue[i].first != IRC::PING)
			count++;
	}

	if ((count % 4) == 0)
		IRCMsgQueue.push_back (TIRCMessage(IRC::PING, ""));
};

void CIRCClientServer::SendMsgQueue ()
{
	if (!CanSendMessages)
		return;
	if (IRCMsgQueue.empty())
		return;

	for (TIRCMessageQueue::iterator it = IRCMsgQueue.begin(); it != IRCMsgQueue.end(); ++it)
	{
		if ((*it).first == IRC::PING)
		{
			CanSendMessages = false;
			irc_send_cmd (&IRCServer, IRC::PING, "%s", HostName.CString());
			break;
		}

		irc_send_cmd (&IRCServer, (*it).first, "%s", (*it).second.CString());
	}

	while (IRCMsgQueue.size())
	{
		if (IRCMsgQueue[0].first == IRC::PING)
		{
			IRCMsgQueue.erase(IRCMsgQueue.begin());
			break;
		}

		IRCMsgQueue.erase (IRCMsgQueue.begin());
	}
};

void CIRCClientServer::JoinChannel (CPlayerEntity *Player, String ChannelName)
{
	if (IRCServer.status != IRC::CONNECTED)
	{
		Player->PrintToClient (PRINT_HIGH, "[IRC] The server has not authenticated you yet; please wait a minute.\n");
		return;
	}

	CIRCClientServerChannel *Chan = NULL;

	if ((Chan = FindChannel(ChannelName)) != NULL)
	{
		// just join it, already opened
		Player->Client.Respawn.IRC.Channel = ChannelName;
		Chan->Join (Player);
	}
	else
	{
		CIRCClientServerChannel Channel;
		Channel.ChannelName = ChannelName;
		Channel.Server = this;

		// not created, make the channel
		PushMessage (IRC::JOIN, String::Format("%s", ChannelName.CString()));

		Channel.Join (Player);
		Channels.push_back (Channel);
	}
};

void CIRCClientServer::LeaveChannel (CPlayerEntity *Player, String ChannelName)
{
	if (ChannelName.IsNullOrEmpty())
		return;

	CIRCClientServerChannel *Chan = FindChannel(ChannelName);

	// remove the player from the list
	Chan->Leave (Player);

	// No players, just leave
	if (!Chan->Players.size())
	{
		PushMessage (IRC::PART, String::Format("%s", Chan->ChannelName.CString()));
		
		for (TIRCChannels::iterator it = Channels.begin(); it != Channels.end(); ++it)
		{
			if (&(*it) == Chan)
			{
				Channels.erase (it);
				break;
			}
		}
	}
};

TIRCServers	IRCServerList;

void UpdateIRCServers ()
{
	for (size_t i = 0; i < IRCServerList.size(); ++i)
		IRCServerList[i]->Update();
}

void CIRCClient::Disconnect ()
{
	if (!Player->Client.Respawn.IRC.Connected())
		return;

	CIRCClientServer *Server = IRCServerList[Player->Client.Respawn.IRC.ConnectedTo-1];

	// Leave the channel that we're in, if we are
	if (!Channel.IsNullOrEmpty())
		Server->LeaveChannel (Player, Channel);

	for (TConnectedIRCPlayers::iterator it = Server->ConnectedPlayers.begin(); it != Server->ConnectedPlayers.end(); ++it)
	{
		if ((*it) == Player)
		{
			Server->ConnectedPlayers.erase (it);
			break;
		}
	}
};

bool CIRCClient::Connected ()
{
	return (ConnectedTo != 0);
};

void CIRCClient::Connect (uint8 ServerIndex)
{
	CIRCClientServer *Server = IRCServerList[ServerIndex-1];

	// Add us to the list
	Server->ConnectedPlayers.push_back (Player);

	Player->PrintToClient (PRINT_HIGH, "You are now connected to %s\n", Server->IRCServer.server.c_str());

	Player->Client.Respawn.IRC.ConnectedTo = ServerIndex;
};

void CIRCClient::JoinChannel (String ChannelName)
{
	if (!Player->Client.Respawn.IRC.Connected())
		return;

	IRCServerList[Player->Client.Respawn.IRC.ConnectedTo-1]->JoinChannel (Player, ChannelName);
};

void CIRCClient::LeaveChannel ()
{
	if (!Player->Client.Respawn.IRC.Connected())
		return;

	IRCServerList[Player->Client.Respawn.IRC.ConnectedTo-1]->LeaveChannel (Player, Channel);
};

void CIRCClient::List ()
{
	String buf			("The following IRC servers are available to connect to.\n"
						"This also lists channels that are already open to connect to.\n"
						"You can join a server by going \"irc join n\", where n\n"
						"is the number that is listed here.\n"
						"To join a channel (even make a new one), type \"irc join <name>\", where\n"
						"<name> is the channel you want to join.\n\n");

	for (size_t i = 0; i < IRCServerList.size(); ++i)
	{
		buf += String::Format ("%i            %s\n", i+1, IRCServerList[i]->IRCServer.server.c_str());
		for (size_t z = 0; z < IRCServerList[i]->Channels.size(); ++z)
			buf += String::Format ("        %i    %s\n", IRCServerList[i]->Channels[z].Players.size(), IRCServerList[i]->Channels[z].ChannelName.CString());
	}

	Player->PrintToClient (PRINT_HIGH, "%s", buf.CString());
};

void CIRCClient::SendMessage (String Msg)
{
	if (!Player->Client.Respawn.IRC.Connected())
		return;

	IRCServerList[Player->Client.Respawn.IRC.ConnectedTo-1]->SendMessage (Player, Msg);
};

void CIRCCommand::CIRCConnectCommand::Execute ()
{
	// irc connect n nick
	if (ArgCount() < 3)
		return;

	uint8 ServerIndex = ArgGeti(2);

	if (ServerIndex > IRCServerList.size()+1)
		return;

	if (Player->Client.Respawn.IRC.Connected())
	{
		Player->PrintToClient (PRINT_HIGH, "[IRC] You are already connected to an IRC server. Use \"irc disconnect\" to leave the server.\n");
		return;
	}

	String Nick;
	if (ArgCount() < 4)
	{
		if (Player->Client.Respawn.IRC.Nick.IsNullOrEmpty())
			Nick = String::Format("%s|Q2", Player->Client.Persistent.Name.CString());
		else
			Nick = Player->Client.Respawn.IRC.Nick;
	}
	else if (ArgCount() >= 4)
		Nick = ArgGets(3);

	Player->Client.Respawn.IRC.Nick = Nick;
	Player->Client.Respawn.IRC.Connect (ServerIndex);
};

void CIRCCommand::CIRCJoinCommand::Execute ()
{
	// irc join channel
	if (ArgCount() < 3)
		return;

	Player->Client.Respawn.IRC.JoinChannel (ArgGets(2));
};

void CIRCCommand::CIRCListCommand::Execute ()
{
	Player->Client.Respawn.IRC.List ();
};

void CIRCCommand::CIRCSayCommand::Execute ()
{
	// irc say "xxx"
	if (ArgCount() < 3)
		return;

	Player->Client.Respawn.IRC.SendMessage (ArgGets(2));
};

void CIRCCommand::CIRCDisconnectCommand::Execute ()
{
	// irc disconnect
	if (ArgCount() < 2)
		return;

	Player->Client.Respawn.IRC.Disconnect ();
};

void CIRCCommand::CIRCLeaveCommand::Execute ()
{
	Player->Client.Respawn.IRC.LeaveChannel ();
};

void CSvIRCConnectToCommand::Execute ()
{
	// sv irc connect server port
	if (ArgCount() < 4)
		return;

	if (ArgGets(2) == "connect")
	{
		int port = 6667;
		if (ArgCount() >= 5)
			port = ArgGeti(4);
			
		CIRCClientServer *Server = QNew (TAG_GENERIC) CIRCClientServer;
		Server->Connect (ArgGets(3).CString(), "Q2Serv", "Q2Serv", "", "Q2Serv", port);
		
		IRCServerList.push_back (Server);
	}
};

void SvCmd_Irc_Disconnect ()
{
	// sv irc disconnect server
	if (ArgCount() < 3)
		return;

	for (TIRCServers::iterator it = IRCServerList.begin(); it != IRCServerList.end(); ++it)
	{
		if (Q_stricmp((*it)->IRCServer.server.c_str(), ArgGets(3).CString()) == 0)
		{
			IRCServerList.erase (it);
			break;
		}
	}
};

#endif