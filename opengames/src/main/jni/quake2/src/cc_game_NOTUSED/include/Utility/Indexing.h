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
// cc_indexing.h
// New indexing system
//

#if !defined(CC_GUARD_INDEXING_H) || !INCLUDE_GUARDS
#define CC_GUARD_INDEXING_H

typedef uint8 MediaIndex;

class CIndex
{
public:
	MediaIndex		Index;
	String			Name;
};

const int MAX_INDEXES = 256;

typedef std::multimap<size_t, size_t> THashedMediaIndexListType;
typedef std::map<uint8, CIndex*, std::less<uint8> > TMediaIndexToPtrList;
typedef std::vector<CIndex*> TMediaIndexListType;

class CIndexList
{
public:
	CIndexList();

	TMediaIndexListType			List;
	TMediaIndexToPtrList		ListMap;
	THashedMediaIndexListType	HashList;

	uint8		numIndexes, firstIndex;

	sint32		AddToList (const char *String, MediaIndex Index);
	sint32		GetIndex (const char *String);

	void	Clear ();
	void	Save (CFile &File);
	void	Read (CFile &File);
};

MediaIndex ModelIndex (const char *string);
MediaIndex SoundIndex (const char *string);
MediaIndex ImageIndex (const char *string);
MediaIndex ModelIndexf (const char *string, ...);
MediaIndex SoundIndexf (const char *string, ...);
MediaIndex ImageIndexf (const char *string, ...);

const char *StringFromSoundIndex (MediaIndex Index);
const char *StringFromModelIndex (MediaIndex Index);
const char *StringFromImageIndex (MediaIndex Index);

void ClearList ();
void ListConfigstrings ();
void ReadLists (CFile &File);
void SaveLists (CFile &File);

class CServerCmdIndexList : public CCommandFunctor
{
public:
	void Execute ();
};

#else
FILE_WARNING
#endif

