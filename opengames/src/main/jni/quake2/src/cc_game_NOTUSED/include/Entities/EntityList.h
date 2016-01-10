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
// cc_entitylist.h
// Resolves an entity from a classname
//

#if !defined(CC_GUARD_ENTITYLIST_H) || !INCLUDE_GUARDS
#define CC_GUARD_ENTITYLIST_H

const int MAX_CLASSNAME_CLASSES_HASH = 256;	// The size parameter for hashing

/**
\class	CClassnameToClassIndex

\brief	Class that links an entity classname to a spawn function.

\author	Paril
\date	12/06/2010
**/
class CClassnameToClassIndex
{
public:
	IMapEntity				*(*Spawn) (sint32 Index);	// spawn function
	const char				*Classname;					// classname
	uint32					HashValue;					// hash value
	CClassnameToClassIndex	*HashNext;					// next hash index

	/**
	\fn	CClassnameToClassIndex (IMapEntity *(*Spawn) (sint32 Index), const char *Classname)
	
	\brief	Constructor. 
	
	\author	Paril
	\date	12/06/2010
	
	\param [in,out]	Spawn	Spawn function. 
	\param [in,out]	Classname	Classname. 
	**/
	CClassnameToClassIndex (IMapEntity *(*Spawn) (sint32 Index), const char *Classname);
};

/**
\fn	IBaseEntity *ResolveMapEntity (SEntity *ServerEntity)

\brief	Resolves entity 'ServerEntity' to a new entity.

\author	Paril
\date	12/06/2010

\param [in,out]	ServerEntity	If non-null, the server entity. 

\return	null if it fails, else. 
**/
IBaseEntity *ResolveMapEntity (SEntity *ServerEntity);

#define LINK_RESOLVE_CLASSNAME(x,y) x##y

#define LINK_CLASSNAME_TO_CLASS(mapClassName,DLLClassName) \
	IMapEntity *LINK_RESOLVE_CLASSNAME(DLLClassName, _Spawn) (sint32 Index) \
	{ \
		DLLClassName *newClass = QNewEntityOf DLLClassName(Index); \
		newClass->ParseFields (); \
		\
		if (newClass->CheckValidity()) \
			newClass->Spawn (); \
		return newClass; \
	} \
	CClassnameToClassIndex LINK_RESOLVE_CLASSNAME(DLLClassName, _Linker) \
	(LINK_RESOLVE_CLASSNAME(DLLClassName, _Spawn), mapClassName); \
	IMPLEMENT_SAVE_STRUCTURE (DLLClassName,DLLClassName)

#define LINK_CLASSNAME_TO_EXISTING_CLASS(DLLClassID,mapClassName,DLLClassName) \
	IMapEntity *LINK_RESOLVE_CLASSNAME(DLLClassID, _Spawn) (sint32 Index) \
	{ \
		DLLClassName *newClass = QNewEntityOf DLLClassName(Index); \
		newClass->ParseFields (); \
		\
		if (newClass->CheckValidity()) \
			newClass->Spawn (); \
		return newClass; \
	} \
	CClassnameToClassIndex LINK_RESOLVE_CLASSNAME(DLLClassID, _Linker) \
	(LINK_RESOLVE_CLASSNAME(DLLClassID, _Spawn), mapClassName); \
	IMPLEMENT_SAVE_STRUCTURE (DLLClassID,DLLClassName)

/**
\fn	void InitEntities ()

\brief	Initializes entities that are constant.
**/
void InitEntities ();

#else
FILE_WARNING
#endif
