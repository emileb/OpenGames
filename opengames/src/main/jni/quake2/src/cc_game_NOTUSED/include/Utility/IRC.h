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
// cc_irc.h
// 
//

#if CLEANCODE_IRC

#include "sircl.h"

// The client redirection service.
// This is a single IRC server connection.
typedef std::vector<class CPlayerEntity*> TConnectedIRCPlayers;

class CIRCClientServerChannel
{
public:
	String					ChannelName;
	TConnectedIRCPlayers	Players; // players on this channel
	class CIRCClientServer	*Server;

	void Join (class CPlayerEntity *Player);
	void Leave (class CPlayerEntity *Player);
};

typedef std::vector<CIRCClientServerChannel> TIRCChannels;

typedef std::pair<int, String> TIRCMessage;
typedef std::vector<TIRCMessage> TIRCMessageQueue;

class CIRCClientServer
{
public:
	irc_server				IRCServer;
	TConnectedIRCPlayers	ConnectedPlayers; // players using this server
	TIRCChannels			Channels;
	TIRCMessageQueue		IRCMsgQueue;
	bool					CanSendMessages;
	String					HostName;

	CIRCClientServer () :
	  CanSendMessages(true)
	{
	};

	CIRCClientServerChannel *FindChannel (String ChannelName);

	bool Connected ();
	void Connect (String HostName, String Nick, String User, String Pass, String RealName, int port = 6667);
	void Disconnect ();
	void SendMessage (class CPlayerEntity *Player, String Message);
	void Update ();
	void ListChannels (class CPlayerEntity *Player);
	void JoinChannel (class CPlayerEntity *Player, String ChannelName);
	void LeaveChannel (class CPlayerEntity *Player, String ChannelName);
	void PushMessage (int Cmd, String Str);
	void SendMsgQueue ();
};

typedef std::vector<CIRCClientServer*> TIRCServers;

// Player client class
class CIRCClient
{
public:
	class CPlayerEntity	*Player;
	String				Nick;
	String				Channel;
	uint8				ConnectedTo;

	bool Connected ();
	void Connect (uint8 ServerIndex);
	void JoinChannel (String ChannelName);
	void LeaveChannel ();
	void List ();
	void Disconnect ();
	void SendMessage (String Msg);
};

class CIRCCommand : public CGameCommandFunctor
{
public:
	void Execute () {};

	class CIRCConnectCommand : public CGameCommandFunctor
	{
	public:
		void Execute ();
	};

	class CIRCJoinCommand : public CGameCommandFunctor
	{
	public:
		void Execute ();
	};

	class CIRCSayCommand : public CGameCommandFunctor
	{
	public:
		void Execute ();
	};

	class CIRCDisconnectCommand : public CGameCommandFunctor
	{
	public:
		void Execute ();
	};

	class CIRCLeaveCommand : public CGameCommandFunctor
	{
	public:
		void Execute ();
	};

	class CIRCListCommand : public CGameCommandFunctor
	{
	public:
		void Execute ();
	};
};

class CSvIRCConnectToCommand : public CCommandFunctor
{
public:
	void Execute ();
};

void UpdateIRCServers ();

#endif