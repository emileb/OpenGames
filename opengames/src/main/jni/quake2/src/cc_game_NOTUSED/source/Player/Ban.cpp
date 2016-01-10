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
// cc_ban.cpp
// Simple ban system
//

#include "Local.h"
#include "Player/Ban.h"

CBanList	Bans;	// The ban list

/**
\fn	sint32 ipcmp (IPAddress &filter, IPAddress &string)

\brief	Compares two IP addresses

\author	Paril
\date	24/05/2010

\param [in,out]	filter	The filter. 
\param [in,out]	string	The string. 

\return	. 
**/
sint32 ipcmp (IPAddress &filter, IPAddress &string)
{
	return !Q_WildcardMatch (filter.str, string.str, true);
}

/**
\fn	void CBanList::Clear ()
	
\brief	Clears the ban list to its blank/initial state. 
**/
void CBanList::Clear ()
{
	for (size_t i = 0; i < BanList.size(); i++)
		QDelete BanList.at(i);
	BanList.clear ();
}

/**
\fn	void CBanList::LoadFromFile ()

\brief	Loads from file. 
**/
void CBanList::LoadFromFile ()
{
	Bans.Clear ();
	CFile File (GAMENAME"\\bans.lst", FILEMODE_READ | FILEMODE_ASCII);

	if (!File.Valid())
		return;

	while (true)
	{
		if (File.EndOfFile())
			break;

		String line = File.ReadLine ();
		
		// Parse the line.
		CParser Parser (line.CString(), PSP_COMMENT_MASK);

		const char *token;
		if (!Parser.ParseToken (PSF_ALLOW_NEWLINES, &token))
			break;

		if (Q_stricmp (token, "name") == 0)
		{
			// Get the distance to the next token.
			// find_first_of operates on length so we need to calc the length
			// of the next token as well.
			if (!Parser.ParseToken (PSF_ALLOW_NEWLINES, &token))
				break;

			BanIndex *NewIndex = QNew (TAG_GAME) BanIndex;
			NewIndex->IP = false;
			NewIndex->Name = Mem_StrDup (token);

			if (!Parser.ParseDataType<EBanTypeFlags> (PSF_ALLOW_NEWLINES, &NewIndex->Flags, 1))
				break;

			BanList.push_back (NewIndex);
		}
		else if (Q_stricmp (token, "ip") == 0)
		{
			if (!Parser.ParseToken (PSF_ALLOW_NEWLINES, &token))
				break;

			BanIndex *NewIndex = QNew (TAG_GAME) BanIndex;
			NewIndex->IP = true;

			NewIndex->Address = QNew (TAG_GAME) IPAddress;
			Q_snprintfz (NewIndex->Address->str, sizeof(NewIndex->Address->str), "%s", token);

			if (!Parser.ParseDataType<EBanTypeFlags> (PSF_ALLOW_NEWLINES, &NewIndex->Flags, 1))
				break;

			BanList.push_back (NewIndex);
		}
	}
	
	ServerPrintf ("Loaded %u bans from file\n", Bans.BanList.size());
}

/**
\fn	void CBanList::SaveList ()
	
\brief	Saves the list. 
**/
void CBanList::SaveList ()
{
	// No changes detected
	if (!Changed)
	{
		ServerPrint ("No changes in ban file, skipping\n");
		return;
	}

	CFile File (GAMENAME"\\bans.lst", FILEMODE_WRITE | FILEMODE_CREATE | FILEMODE_ASCII);

	if (!File.Valid())
		return;

	for (size_t i = 0; i < BanList.size(); i++)
	{
		BanIndex *Index = BanList[i];

		File.Print ("%s %s %u\n",
			(Index->IP) ? "ip" : "name",
			(Index->IP) ? Index->Address->str : Index->Name,
			Index->Flags);
	}

	ServerPrintf ("Saved %u bans\n", Bans.BanList.size());
	Changed = false;
}

/**
\fn	bool CBanList::InList (IPAddress Adr)
	
\brief	Test if the IP address 'Adr' is in the list
	
\author	Paril
\date	24/05/2010
	
\param	Adr	The IP address. 
	
\return	true if it is in the list, false if it is not. 
**/
bool CBanList::InList (IPAddress Adr)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->Address, Adr))
			return true;
	}
	return false;
}

/**
\fn	bool CBanList::InList (const char *Name)
	
\brief	Test if the player 'Name' is in the list
	
\author	Paril
\date	24/05/2010
	
\param	Name The player's name. 
	
\return	true if it is in the list, false if it is not. 
**/
bool CBanList::InList (const char *Name)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
			return true;
	}
	return false;
}

/**
\fn	bool CBanList::AddToList (IPAddress Adr, EBanTypeFlags Flags)
	
\brief	Adds the IP address 'Adr' to the list with the flags 'Flags'.
	
\author	Paril
\date	24/05/2010
	
\param	Adr		The IP address. 
\param	Flags	The flags of the ban. 
	
\return	true if it succeeds, false if it fails. 
**/
bool CBanList::AddToList (IPAddress Adr, EBanTypeFlags Flags)
{
	if (InList(Adr))
		return false;

	Changed = true;

	BanIndex *NewIndex = QNew (TAG_GAME) BanIndex;
	NewIndex->IP = true;

	NewIndex->Address = QNew (TAG_GAME) IPAddress(Adr);
	NewIndex->Flags = Flags;

	BanList.push_back (NewIndex);
	return true;
}

/**
\fn	bool CBanList::AddToList (const char *Name, EBanTypeFlags Flags)
	
\brief	Adds to the player 'Name' to the list with the flags 'Flags'. 
	
\author	Paril
\date	24/05/2010
	
\param	Name	The name of the player. 
\param	Flags	The flags of the ban. 
	
\return	true if it succeeds, false if it fails. 
**/
bool CBanList::AddToList (const char *Name, EBanTypeFlags Flags)
{
	if (InList(Name))
		return false;

	Changed = true;

	BanIndex *NewIndex = QNew (TAG_GAME) BanIndex;
	NewIndex->IP = false;
	NewIndex->Name = QNew (TAG_GAME) char[strlen(Name)];
	Q_strncpyz (NewIndex->Name, Name, strlen(Name)+1);
	NewIndex->Name[strlen(Name)] = 0;
	NewIndex->Flags = Flags;

	BanList.push_back (NewIndex);
	return true;
}

/**
\fn	bool CBanList::RemoveFromList (IPAddress Adr)
	
\brief	Removes the IP address 'Adr' from the ban list.
	
\author	Paril
\date	24/05/2010
	
\param	Adr	The IP address. 
	
\return	true if it succeeds, false if it fails. 
**/
bool CBanList::RemoveFromList (IPAddress Adr)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->Address, Adr))
		{
			BanList.erase(it);
			Changed = true;
			return true;
		}
	}
	return false;
}

/**
\fn	bool CBanList::RemoveFromList (const char *Name)
	
\brief	Removes the player 'Name' from the ban list. 
	
\author	Paril
\date	24/05/2010
	
\param	Name	The name. 
	
\return	true if it succeeds, false if it fails. 
**/
bool CBanList::RemoveFromList (const char *Name)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
		{
			BanList.erase(it);
			Changed = true;
			return true;
		}
	}
	return false;
}

/**
\fn	bool CBanList::ChangeBan (IPAddress Adr, EBanTypeFlags Flags)
	
\brief	Change the existing ban of IP address 'Adr''s flags to 'Flags'
	
\author	Paril
\date	24/05/2010
	
\param	Adr		The IP address. 
\param	Flags	The flags. 
	
\return	true if it succeeds, false if it fails. 
**/
bool CBanList::ChangeBan (IPAddress Adr, EBanTypeFlags Flags)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->Address, Adr))
		{
			if (Index->Flags == Flags)
				return false;

			Index->Flags = Flags;
			Changed = true;
			return true;
		}
	}
	return false;
}

/**
\fn	bool CBanList::ChangeBan (const char *Name, EBanTypeFlags Flags)
	
\brief	Change the existing ban of player 'Name''s flags to 'Flags' 
	
\author	Paril
\date	24/05/2010
	
\param	Name	The player's name. 
\param	Flags	The flags. 
	
\return	true if it succeeds, false if it fails. 
**/
bool CBanList::ChangeBan (const char *Name, EBanTypeFlags Flags)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
		{
			if (Index->Flags == Flags)
				return false;

			Index->Flags = Flags;
			Changed = true;
			return true;
		}
	}
	return false;
}

/**
\fn	bool CBanList::IsSquelched (IPAddress IP)
	
\brief	Query if 'IP' is squelched. 
	
\author	Paril
\date	24/05/2010
	
\param	IP	The ip address. 
	
\return	true if squelched, false if not. 
	
\note Call these with CPersistentData::IP if after spawning
**/
bool CBanList::IsSquelched (IPAddress Adr)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->Address, Adr))
			return !!(Index->Flags & BAN_SQUELCH);
	}
	return false;
}

/**
\fn	bool CBanList::IsBannedFromSpectator (IPAddress IP)
	
\brief	Query if 'IP' is banned from spectator mode. 
	
\author	Paril
\date	24/05/2010
	
\param	IP	The ip. 
	
\return	true if banned from spectator mode, false if not. 
	
\note Call these with CPersistentData::IP if after spawning
**/
bool CBanList::IsBannedFromSpectator (IPAddress Adr)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->Address, Adr))
			return !!(Index->Flags & BAN_SPECTATOR);
	}
	return false;
}

/**
\fn	bool CBanList::IsBanned (IPAddress IP)
	
\brief	Query if 'IP' is banned. 
	
\author	Paril
\date	24/05/2010
	
\param	IP	The ip. 
	
\return	true if banned, false if not. 
	
\note Call these with CPersistentData::IP if after spawning
**/
bool CBanList::IsBanned (IPAddress Adr)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->Address, Adr))
			return !!(Index->Flags & BAN_ENTER);
	}
	return false;
}

/**
\fn	bool CBanList::IsSquelched (const char *Name)
	
\brief	Query if player 'Name' is squelched. 
	
\author	Paril
\date	24/05/2010
	
\param	Name	The name. 
	
\return	true if squelched, false if not. 
**/
bool CBanList::IsSquelched (const char *Name)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
			return !!(Index->Flags & BAN_SQUELCH);
	}
	return false;
}

/**
\fn	bool CBanList::IsBannedFromSpectator (const char *Name)
	
\brief	Query if player 'Name' is banned from spectator. 
	
\author	Paril
\date	24/05/2010
	
\param	Name	The name. 
	
\return	true if banned from spectator, false if not. 
**/
bool CBanList::IsBannedFromSpectator (const char *Name)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
			return !!(Index->Flags & BAN_SPECTATOR);
	}
	return false;
}

/**
\fn	bool CBanList::IsBanned (const char *Name)
	
\brief	Query if player 'Name' is banned. 
	
\author	Paril
\date	24/05/2010
	
\param	Name	The name. 
	
\return	true if banned, false if not. 
**/
bool CBanList::IsBanned (const char *Name)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
			return !!(Index->Flags & BAN_ENTER);
	}
	return false;
}

/**
\page Ban Files
Ban list looks as follows:
<code>
// Comments are ignored.
// First token needs to be either ip or name, followed by the IP address
// or name of the offender, respectively.
// Then flags! See cc_ban.h for flag numbers.
// Unknown flags are stored, but ignored by older versions.
name Paril 1
ip 252.6.10.6 5
</code>
**/

