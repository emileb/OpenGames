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
// cc_save.h
// 
//

#if !defined(CC_GUARD_SAVE_H) || !INCLUDE_GUARDS
#define CC_GUARD_SAVE_H

void ReadLevel (char *);
void WriteLevel (char *);

void ReadGame (char *);
void WriteGame (char *, BOOL);

// Entity table
class CEntityTableIndex
{
public:
	const char *Name;
	IBaseEntity *(*FuncPtr) (sint32 index);

	CEntityTableIndex (const char *Name, IBaseEntity *(*FuncPtr) (sint32 index));

	IBaseEntity *Create (sint32 number)
	{
		return FuncPtr (number);
	};
};

#define IMPLEMENT_SAVE_STRUCTURE(unique_name,DLLClassName) \
	IBaseEntity *LINK_RESOLVE_CLASSNAME(unique_name, _RecreateEntity) (sint32 Index) \
	{ \
		return QNewEntityOf DLLClassName(Index); \
	} \
	CEntityTableIndex LINK_RESOLVE_CLASSNAME(unique_name, __SaveData) \
	(TO_STRING(unique_name), LINK_RESOLVE_CLASSNAME(unique_name, _RecreateEntity));

#define IMPLEMENT_SAVE_SOURCE(DLLClassName) \
	IMPLEMENT_SAVE_STRUCTURE(DLLClassName,DLLClassName)

#define IMPLEMENT_SAVE_HEADER(DLLClassName) \
	virtual const char *SAVE_GetName () { return TO_STRING(DLLClassName); };

/**
\typedef	uint8 EIndexType

\brief	Defines an alias representing type of the index.
**/
typedef uint8 EIndexType;

/**
\enum	

\brief	Values that represent the types of indexes. 
**/
enum
{
	INDEX_MODEL,
	INDEX_SOUND,
	INDEX_IMAGE
};

void WriteIndex (CFile &File, MediaIndex Index, EIndexType IndexType);
void ReadIndex (CFile &File, MediaIndex &Index, EIndexType IndexType);

#else
FILE_WARNING
#endif
