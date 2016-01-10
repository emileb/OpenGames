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
// cc_xatrix_supertank_boss5.cpp
// 
//

#include "Local.h"

#if XATRIX_FEATURES
#include "Monsters/Supertank.h"
#include "Xatrix/XatrixSupertankBoss5.h"

CBoss5::CBoss5 (uint32 ID) :
CSuperTank (ID)
{
};

void CBoss5::Spawn ()
{
	CSuperTank::Spawn ();
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/boss5/tris.md2");
	PowerArmorType = POWER_ARMOR_SHIELD;
	PowerArmorPower = 400;
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_boss5", CBoss5);
#endif
