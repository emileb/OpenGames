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
// cc_entity_ptr.cpp
// 
//

#include "Local.h"

const nullentity_t nullentity_t::value  = nullentity_t();
const nullentity_t *nullentity = &nullentity_t::value;	// Null entity

/**
\fn	CEntityPtrLinkList &UsageList ()

\brief	Return static list.

\author	Paril
\date	12/06/2010

\return	Static entity pointer list. 
**/
CEntityPtrLinkList &UsageList ()
{
	static CEntityPtrLinkList _L;
	return _L;
}

/**
\fn	void FixAllEntityPtrs (IBaseEntity *Entity)

\brief	Function to clear entity pointers that point to Entity. 

\author	Paril
\date	12/06/2010

\param [in,out]	Entity	If non-null, the entity. 
**/
void FixAllEntityPtrs (IBaseEntity *Entity)
{
	if (UsageList().List.find(Entity) == UsageList().List.end())
		return;

	std::list<void*> &v = (*UsageList().List.find(Entity)).second;
	
	for (std::list<void*>::iterator it = v.begin(); it != v.end(); ++it)
	{
		void *tehPtr = (*it);
		Mem_Zero (tehPtr, sizeof(entity_ptr<IBaseEntity>));
	}

	UsageList().List.erase(Entity);
}
