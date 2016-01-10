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
// cc_menu.h
// Menu system
//

#if !defined(CC_GUARD_MENU_H) || !INCLUDE_GUARDS
#define CC_GUARD_MENU_H

class CMenuItem
{
public:
	sint32				x, y;
	bool			Selected;
	bool			Enabled;
	class CMenu		*Menu; // Menu that owns this object

	CMenuItem		(class CMenu *Menu, sint32 x, sint32 y);

	virtual void	Draw (CPlayerEntity *Player, CStatusBar *DrawState) = 0;

	virtual bool	CanSelect (CPlayerEntity *Player); // Returns false if the option can't be selected
	virtual void	Update (CPlayerEntity *Player); // Updates the control
	virtual bool	Select (CPlayerEntity *Player); // What happens when you hit enter (return true to close the menu)
};

typedef std::vector<CMenuItem*> TMenuItemsContainer;

class CMenu 
{
public:
	sint32							Cursor; // Where to position the cursor on start
	CPlayerEntity					*Player; // A pointer to the entity that is running the show (needed?)
	std::vector<CMenuItem*>			Items;

	CMenu						(CPlayerEntity *Player);
	~CMenu						();

	virtual bool				Open () = 0;
	virtual void				Close () = 0;

	void						DrawItems(CStatusBar *Bar);
	virtual void				Draw (bool reliable) = 0;
	void						AddItem (CMenuItem *Item);
};

class CMenuLeftCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

class CMenuRightCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

// Controls
#include "Menu/Controls.h"

#else
FILE_WARNING
#endif
