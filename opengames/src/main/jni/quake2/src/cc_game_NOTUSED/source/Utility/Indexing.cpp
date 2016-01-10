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
// cc_indexing.cpp
// New indexing system
//

#include "Local.h"

CIndexList	ModelList, SoundList, ImageList;

CIndexList::CIndexList () :
numIndexes(0),
List(),
HashList(),
firstIndex(0)
{
}

sint32 CIndexList::GetIndex (const char *String)
{
	// Check through the itemlist
	uint32 hash = Com_HashGeneric(String, MAX_INDEXES);

	for (THashedMediaIndexListType::iterator it = HashList.equal_range(hash).first; it != HashList.equal_range(hash).second; ++it)
	{
		CIndex *Index = List.at((*it).second);
		if (Q_stricmp (Index->Name.CString(), String) == 0)
			return Index->Index;
	}
	return -1;
}

sint32 CIndexList::AddToList (const char *String, MediaIndex Index)
{
	if (!firstIndex)
		firstIndex = Index;

	CIndex *NewIndex = QNew (TAG_GENERIC) CIndex;
	NewIndex->Index = Index;
	NewIndex->Name = String;

	List.push_back (NewIndex);

	ListMap[Index] = NewIndex;

	// Link it in the hash tree
	HashList.insert (std::make_pair<size_t, size_t> (Com_HashGeneric (String, MAX_INDEXES), List.size()-1));

	numIndexes++;
	return Index;
}

inline bool OverFlow ()
{
	return ((ModelList.numIndexes + SoundList.numIndexes + ImageList.numIndexes) >= 255);
}

const char *StringFromSoundIndex (MediaIndex Index)
{
	if (SoundList.ListMap.find(Index) == SoundList.ListMap.end())
		return NULL;
	return (*SoundList.ListMap.find(Index)).second->Name.CString();
}

const char *StringFromModelIndex (MediaIndex Index)
{
	if (Index <= ModelList.firstIndex || Index == 255)
		return NULL;
	if (ModelList.ListMap.find(Index) == ModelList.ListMap.end())
		return NULL;
	return (*ModelList.ListMap.find(Index)).second->Name.CString();
}

const char *StringFromImageIndex (MediaIndex Index)
{
	if (ImageList.ListMap.find(Index) == ImageList.ListMap.end())
		return NULL;
	return (*ImageList.ListMap.find(Index)).second->Name.CString();
}

CC_DISABLE_DEPRECATION

MediaIndex IndexerBase (const char *String, CIndexList &List, sint32 (*indexer) (const char *str))
{
	// Do we exist?
	sint32 Index = List.GetIndex(String);
	if (Index == -1)
	{
		if (OverFlow())
		{
			ServerPrintf ("Index overflow registering \"%s\"\n", String);
			return 0;
		}

		return List.AddToList (String, indexer(String));
	}
	return Index;
}

MediaIndex ModelIndex (const char *String)
{
	return IndexerBase (String, ModelList, gi.modelindex);
}

MediaIndex SoundIndex (const char *String)
{
	return IndexerBase (String, SoundList, gi.soundindex);
}

MediaIndex ImageIndex (const char *String)
{
	return IndexerBase (String, ImageList, gi.imageindex);
}

MediaIndex ModelIndexf (const char *String, ...)
{
	va_list		argptr;
	CTempMemoryBlock string = CTempHunkSystem::Allocator.GetBlock(MAX_QPATH);

	va_start (argptr, String);
	vsnprintf (string.GetBuffer<char>(), string.GetSize() - 1, String, argptr);
	va_end (argptr);

	// Do we exist?
	return ModelIndex(string.GetBuffer<char>());
}

MediaIndex SoundIndexf (const char *String, ...)
{
	va_list		argptr;
	CTempMemoryBlock string = CTempHunkSystem::Allocator.GetBlock(MAX_QPATH);

	va_start (argptr, String);
	vsnprintf (string.GetBuffer<char>(), string.GetSize() - 1, String, argptr);
	va_end (argptr);

	// Do we exist?
	return SoundIndex(string.GetBuffer<char>());
}

MediaIndex ImageIndexf (const char *String, ...)
{
	va_list		argptr;
	CTempMemoryBlock string = CTempHunkSystem::Allocator.GetBlock(MAX_QPATH);

	va_start (argptr, String);
	vsnprintf (string.GetBuffer<char>(), string.GetSize() - 1, String, argptr);
	va_end (argptr);

	// Do we exist?
	return ImageIndex(string.GetBuffer<char>());
}

void CIndexList::Clear ()
{
	for (uint8 i = 0; i < numIndexes; i++)
		QDelete List[i];

	numIndexes = firstIndex = 0;
	List.clear();
	HashList.clear();
}

void CIndexList::Save (CFile &File)
{
	File.Write<> (firstIndex);
	File.Write<> (List.size());

	for (size_t i = 0; i < List.size(); ++i)
	{
		File.Write (List[i]->Name);
		File.Write<> (List[i]->Index);
	}
}

void CIndexList::Read (CFile &File)
{
	Clear ();

	firstIndex = File.Read<uint8> ();

	size_t len = File.Read<size_t> ();

	for (size_t i = 0; i < len; ++i)
	{
		String str = File.ReadString();
		MediaIndex index = File.Read<MediaIndex> ();

		AddToList (str.CString(), index);
	}
}

void ClearList ()
{
	ModelList.Clear();
	ImageList.Clear();
	SoundList.Clear();
}

void ReadLists (CFile &File)
{
	ModelList.Read(File);
	ImageList.Read(File);
	SoundList.Read(File);
}

void SaveLists (CFile &File)
{
	ModelList.Save(File);
	ImageList.Save(File);
	SoundList.Save(File);
}

extern char	ReadConfigSt[MAX_CFGSTRINGS][MAX_CFGSTRLEN];
void ListConfigstrings ()
{
	bool firstIndex = false;
	for (uint16 i = 1; i <= MAX_CS_MODELS; i++)
	{
		if (!strlen(ReadConfigSt[CS_MODELS+i]))
			continue;
		else if (CS_MODELS+i == 33)
			ModelList.firstIndex = 0;
		else if (ReadConfigSt[CS_MODELS+i][0] == '*')
			ModelList.firstIndex = atoi(ReadConfigSt[CS_MODELS+i] + 1);
		else
		{
			if (!firstIndex)
			{
				ModelList.firstIndex++;
				firstIndex = true;
			}
			ModelList.AddToList (ReadConfigSt[CS_MODELS+i], gi.modelindex(ReadConfigSt[CS_MODELS+i]));
		}
	}
	for (uint16 i = 1; i <= MAX_CS_SOUNDS; i++)
	{
		if (!strlen(ReadConfigSt[CS_SOUNDS+i]))
			continue;
		SoundList.AddToList (ReadConfigSt[CS_SOUNDS+i], gi.soundindex(ReadConfigSt[CS_SOUNDS+i]));
	}
	for (uint16 i = 1; i <= MAX_CS_IMAGES; i++)
	{
		if (!strlen(ReadConfigSt[CS_IMAGES+i]))
			continue;
		ImageList.AddToList (ReadConfigSt[CS_IMAGES+i], gi.imageindex(ReadConfigSt[CS_IMAGES+i]));
	}
}

void CServerCmdIndexList::Execute ()
{
	ServerPrintf ("Models: (%u) + %u inline\n", ModelList.numIndexes, ModelList.firstIndex);
	for (uint8 i = 0; i < ModelList.numIndexes; i++)
		ServerPrintf ("%s\n", ModelList.List[i]->Name.CString());

	ServerPrintf ("\nSounds: (%u)\n", SoundList.numIndexes);
	for (uint8 i = 0; i < SoundList.numIndexes; i++)
		ServerPrintf ("%s\n", SoundList.List[i]->Name.CString());

	ServerPrintf ("\nImages: (%u)\n", ImageList.numIndexes);
	for (uint8 i = 0; i < ImageList.numIndexes; i++)
		ServerPrintf ("%s\n", ImageList.List[i]->Name.CString());

	ServerPrintf ("\nTotal: %u\n", ModelList.numIndexes + SoundList.numIndexes + ImageList.numIndexes);
}

CC_ENABLE_DEPRECATION

