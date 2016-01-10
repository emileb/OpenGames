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
// cc_ban.h
// Simple ban system
//

#ifdef BAN_BASIC_INFO
#ifndef MAX_IP_BLOCK_LEN
const int MAX_IP_BLOCK_LEN = 3;	// Maximum length of an ip block
const int MAX_IP_LEN = (MAX_IP_BLOCK_LEN * 4 + 3);	// Maximum length of an ip address

/**
\struct	IPAddress

\brief	Just a structure holding an IP address

\author	Paril
\date	24/05/2010
**/
struct IPAddress
{
	char str[MAX_IP_LEN];
};

#endif
#else
#if !defined(CC_GUARD_BAN_H) || !INCLUDE_GUARDS
#define CC_GUARD_BAN_H

/**
\typedef	uint8 EBanTypeFlags

\brief	Defines an alias representing the ban type flags.
**/
typedef uint8 EBanTypeFlags;

/**
\enum	

\brief	Values that represent ban type flags.
**/
enum
{
	BAN_SQUELCH		=	BIT(0), // Banned from talking
	BAN_SPECTATOR	=	BIT(1), // Banned from moving to Spectator mode
	BAN_ENTER		=	BIT(2), // Banned from entering the game
};

/**
\class	BanIndex

\brief	A single ban index.

\author	Paril
\date	24/05/2010
**/
class BanIndex
{
public:
	bool			IP;	/* true if this index is an IP address, false if it's a name */
	union
	{
		IPAddress	*Address;	/* The IP address */
		char		*Name;	/* The name */
	};

	EBanTypeFlags	Flags;	/* The flags */

	/**
	\fn	~BanIndex ()
	
	\brief	Destructor 
	
	\author	Paril
	\date	24/05/2010
	**/
	~BanIndex ()
	{
		if (!IP)
			QDelete Name;
	};
};

/**
\typedef	std::vector<BanIndex*> TBanIndexContainer

\brief	Defines an alias representing the ban index container.
**/
typedef std::vector<BanIndex*> TBanIndexContainer;

/**
\class	CBanList

\brief	List of ban indices. 

\author	Paril
\date	24/05/2010
**/
class CBanList
{
public:
	TBanIndexContainer	BanList;	/* List of bans */
	bool Changed;	/* true if the data has changed */

	/**
	\fn	void Clear ()
	
	\brief	Clears the ban list to its blank/initial state. 
	**/
	void Clear ();

	/**
	\fn	void LoadFromFile ()
	
	\brief	Loads from file. 
	**/
	void LoadFromFile ();

	/**
	\fn	void SaveList ()
	
	\brief	Saves the list. 
	**/
	void SaveList ();

	/**
	\fn	bool InList (IPAddress Adr)
	
	\brief	Test if the IP address 'Adr' is in the list
	
	\author	Paril
	\date	24/05/2010
	
	\param	Adr	The IP address. 
	
	\return	true if it is in the list, false if it is not. 
	**/
	bool InList (IPAddress Adr);

	/**
	\fn	bool InList (const char *Name)
	
	\brief	Test if the player 'Name' is in the list
	
	\author	Paril
	\date	24/05/2010
	
	\param	Name The player's name. 
	
	\return	true if it is in the list, false if it is not. 
	**/
	bool InList (const char *Name);

	/**
	\fn	bool AddToList (IPAddress Adr, EBanTypeFlags Flags)
	
	\brief	Adds the IP address 'Adr' to the list with the flags 'Flags'.
	
	\author	Paril
	\date	24/05/2010
	
	\param	Adr		The IP address. 
	\param	Flags	The flags of the ban. 
	
	\return	true if it succeeds, false if it fails. 
	**/
	bool AddToList (IPAddress Adr, EBanTypeFlags Flags);

	/**
	\fn	bool AddToList (const char *Name, EBanTypeFlags Flags)
	
	\brief	Adds to the player 'Name' to the list with the flags 'Flags'. 
	
	\author	Paril
	\date	24/05/2010
	
	\param	Name	The name of the player. 
	\param	Flags	The flags of the ban. 
	
	\return	true if it succeeds, false if it fails. 
	**/
	bool AddToList (const char *Name, EBanTypeFlags Flags);

	/**
	\fn	bool RemoveFromList (IPAddress Adr)
	
	\brief	Removes the IP address 'Adr' from the ban list.
	
	\author	Paril
	\date	24/05/2010
	
	\param	Adr	The IP address. 
	
	\return	true if it succeeds, false if it fails. 
	**/
	bool RemoveFromList (IPAddress Adr);

	/**
	\fn	bool RemoveFromList (const char *Name)
	
	\brief	Removes the player 'Name' from the ban list. 
	
	\author	Paril
	\date	24/05/2010
	
	\param	Name	The name. 
	
	\return	true if it succeeds, false if it fails. 
	**/
	bool RemoveFromList (const char *Name);

	/**
	\fn	bool ChangeBan (IPAddress Adr, EBanTypeFlags Flags)
	
	\brief	Change the existing ban of IP address 'Adr''s flags to 'Flags'
	
	\author	Paril
	\date	24/05/2010
	
	\param	Adr		The IP address. 
	\param	Flags	The flags. 
	
	\return	true if it succeeds, false if it fails. 
	**/
	bool ChangeBan (IPAddress Adr, EBanTypeFlags Flags);

	/**
	\fn	bool ChangeBan (const char *Name, EBanTypeFlags Flags)
	
	\brief	Change the existing ban of player 'Name''s flags to 'Flags' 
	
	\author	Paril
	\date	24/05/2010
	
	\param	Name	The player's name. 
	\param	Flags	The flags. 
	
	\return	true if it succeeds, false if it fails. 
	**/
	bool ChangeBan (const char *Name, EBanTypeFlags Flags);

	/**
	\fn	bool IsSquelched (IPAddress IP)
	
	\brief	Query if 'IP' is squelched. 
	
	\author	Paril
	\date	24/05/2010
	
	\param	IP	The ip address. 
	
	\return	true if squelched, false if not. 
	
	\note Call these with CPersistentData::IP if after spawning
	**/
	bool IsSquelched (IPAddress IP);

	/**
	\fn	bool IsBannedFromSpectator (IPAddress IP)
	
	\brief	Query if 'IP' is banned from spectator mode. 
	
	\author	Paril
	\date	24/05/2010
	
	\param	IP	The ip. 
	
	\return	true if banned from spectator mode, false if not. 
	
	\note Call these with CPersistentData::IP if after spawning
	**/
	bool IsBannedFromSpectator (IPAddress IP);

	/**
	\fn	bool IsBanned (IPAddress IP)
	
	\brief	Query if 'IP' is banned. 
	
	\author	Paril
	\date	24/05/2010
	
	\param	IP	The ip. 
	
	\return	true if banned, false if not. 
	
	\note Call these with CPersistentData::IP if after spawning
	**/
	bool IsBanned (IPAddress IP);

	/**
	\fn	bool IsSquelched (const char *Name)
	
	\brief	Query if player 'Name' is squelched. 
	
	\author	Paril
	\date	24/05/2010
	
	\param	Name	The name. 
	
	\return	true if squelched, false if not. 
	**/
	bool IsSquelched (const char *Name);

	/**
	\fn	bool IsBannedFromSpectator (const char *Name)
	
	\brief	Query if player 'Name' is banned from spectator. 
	
	\author	Paril
	\date	24/05/2010
	
	\param	Name	The name. 
	
	\return	true if banned from spectator, false if not. 
	**/
	bool IsBannedFromSpectator (const char *Name);

	/**
	\fn	bool IsBanned (const char *Name)
	
	\brief	Query if player 'Name' is banned. 
	
	\author	Paril
	\date	24/05/2010
	
	\param	Name	The name. 
	
	\return	true if banned, false if not. 
	**/
	bool IsBanned (const char *Name);
};

extern CBanList	Bans;	// Bans list, implemented in cc_ban.cpp
#else
FILE_WARNING
#endif
#endif

