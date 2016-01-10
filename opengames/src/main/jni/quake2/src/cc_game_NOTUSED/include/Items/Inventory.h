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
// cc_inventory.h
// Inventory
//

#if !defined(CC_GUARD_INVENTORY_H) || !INCLUDE_GUARDS
#define CC_GUARD_INVENTORY_H

class CInventory
{
private:
	typedef std::map <uint8, sint16> TInventoryMapType;
	TInventoryMapType Array;

public:
	sint32		SelectedItem;

	void	operator += (IBaseItem *Item);
	void	operator += (uint8 Index);
	void	operator -= (IBaseItem *Item);
	void	operator -= (uint8 Index);

	CInventory ();
	~CInventory ();

	// Note: All of these functions are for directly
	// adding items. This will NOT call the pickup
	// function! You need to add your own checks in for
	// ammo and such if you use these functions.
	void	Add (IBaseItem *Item, sint32 Num);
	void	Remove (IBaseItem *Item, sint32 Num);
	void	Add (uint8 Index, sint32 Num);
	void	Remove (uint8 Index, sint32 Num);

	// Draw the inventory
	void	Draw (CPlayerEntity *Player);

	// Validate items selected with functions below
	void	ValidateSelectedItem ();

	void	SelectNextItem (EItemFlags Flags);
	void	SelectPrevItem (EItemFlags Flags);

	sint32		Has	(uint8 Index);
	sint32		Has	(IBaseItem *Item);

	void	Set	(IBaseItem *Item, sint32 Num);
	void	Set	(uint8 Index, sint32 Num);

	void Save (CFile &File)
	{
		File.Write<size_t> (Array.size());

		for (TInventoryMapType::iterator it = Array.begin(); it != Array.end(); ++it)
			File.Write <std::pair <uint8, uint16> > (*it);

		File.Write<sint32> (SelectedItem);
	};

	void Load (CFile &File)
	{
		size_t size = File.Read<size_t> ();

		for (size_t i = 0; i < size; i++)
			Array.insert (File.Read <std::pair <uint8, uint16> > ());

		SelectedItem = File.Read<sint32> ();
	};

	void Clear ()
	{
		Array.clear();
		SelectedItem = -1;
	};
};

class CUseCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CUseListCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CDropCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CInventoryCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CInvUseCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CInvDropCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CWeapPrevCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CWeapNextCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CWeapLastCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CInvNextCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CInvPrevCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CInvNextWCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CInvPrevWCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CInvNextPCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CInvPrevPCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CGiveCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CSpawnCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

#else
FILE_WARNING
#endif
