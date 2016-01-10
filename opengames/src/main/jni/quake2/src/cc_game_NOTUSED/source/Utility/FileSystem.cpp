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
// cc_filesystem.cpp
// 
//

#include "Local.h"

CFileHandleList *CFile::IndexList;

TPathListType PathList;

/**
\fn	void FS_AddPath (const char *pathName)

\brief	Adds path 'pathName' to the list of paths. 

\author	Paril
\date	25/05/2010

\param	pathName	Path name. 
**/
void FS_AddPath (const char *pathName)
{
	CPathIndex *path = QNew (TAG_GENERIC) CPathIndex (pathName);
	PathList.push_back (path);
}

/**
\fn	void FS_RemovePath (const char *pathName)

\brief	Removes path 'pathName' from the list of paths.

\author	Paril
\date	25/05/2010

\param	pathName	Path name.
**/
void FS_RemovePath (const char *pathName)
{
	for (TPathListType::iterator it = PathList.begin(); it < PathList.end(); ++it)
	{
		if ((*it)->pathName == pathName)
		{
			delete (*it);
			PathList.erase (it);
			return;
		}
	}
}

/**
\fn	void FS_ReorderPath (const char *pathName)

\brief	Re-orders path 'pathName' to the front of the list of paths

\author	Paril
\date	25/05/2010

\param	pathName	Path name. 
**/
void FS_ReorderPath (const char *pathName)
{
	for (TPathListType::iterator it = PathList.begin(); it < PathList.end(); ++it)
	{
		if ((*it)->pathName == pathName)
		{
			CPathIndex *Path = (*it);

			PathList.erase (it);
			PathList.insert (PathList.begin(), Path);
			return;
		}
	}

	CPathIndex *Path = QNew (TAG_GENERIC) CPathIndex (pathName);
	PathList.insert (PathList.begin(), Path);
}

/**
\fn	THandleIndexListType::iterator CFileHandleList::Create ()
	
\brief	Creates a new key and increases allocated handles.
		Returns the new key.
	
\return	Iterator to the new key. 
**/
THandleIndexListType::iterator CFileHandleList::Create ()
{
	FileHandle CreatedKey = numHandlesAllocated++;
	OpenList[CreatedKey] = CFileHandleIndex(CreatedKey);
	return OpenList.find(CreatedKey);
};

/**
\fn	THandleIndexListType::iterator CFileHandleList::MoveToClosed (THandleIndexListType::iterator it)
	
\brief	Moves the handle of iterator 'it' to the Closed list and returns the new iterator
	
\author	Paril
\date	25/05/2010
	
\param	it	The iterator index that needs to be moved to the closed list. 
	
\return	The iterator of the moved key. 
**/
THandleIndexListType::iterator CFileHandleList::MoveToClosed (THandleIndexListType::iterator it)
{
	FileHandle Key = (*it).first;
	ClosedList[Key] = (*it).second;
	OpenList.erase (it);
	return ClosedList.find(Key);
};

/**
\fn	THandleIndexListType::iterator CFileHandleList::MoveToOpen (THandleIndexListType::iterator it)
	
\brief	Moves the handle of iterator 'it' to the Open list and returns the new iterator
	
\author	Paril
\date	25/05/2010
	
\param	it	The iterator index that needs to be moved to the open list.
	
\return	The iterator of the moved key. 
**/
THandleIndexListType::iterator CFileHandleList::MoveToOpen (THandleIndexListType::iterator it)
{
	(*it).second.Clear ();

	FileHandle Key = (*it).first;
	OpenList[Key] = (*it).second;
	ClosedList.erase (it);
	return OpenList.find(Key);
};

/**
\fn	CFileHandleList::CFileHandleList (sint32 allocated = 0)
	
\brief	Constructor. 
	
\author	Paril
\date	25/05/2010
	
\param	allocated	Number of handles to create automatically. 
**/
CFileHandleList::CFileHandleList (sint32 allocated) :
	numHandlesAllocated (0)
{
	for (FileHandle i = 0; i < allocated; i++)
		Create ();
};

/**
\fn	THandleIndexListType::iterator CFileHandleList::GetFreeHandle ()
	
\brief	Gets a free handle. 
	
\return	Returns either a free key in Open or
		creates a new key and returns it.
**/
THandleIndexListType::iterator CFileHandleList::GetFreeHandle ()
{
	if (!OpenList.empty())
		// Re-use a key
		return MoveToClosed(OpenList.begin());
	else
		// Create a new key and use that
		return MoveToClosed(Create ());
};

/**
\fn	CFileHandleIndex *CFileHandleList::GetHandle (FileHandle Key)
	
\brief	Gets the CFileHandleIndex of a FileHandle 
	
\author	Paril
\date	25/05/2010
	
\param	Key	The key. 
	
\return	null if it fails, else the handle. 
**/
CFileHandleIndex *CFileHandleList::GetHandle (FileHandle Key)
{
	return &(*ClosedList.find(Key)).second;
};

/**
\fn	void CFileHandleList::PushFreeHandle (FileHandle Handle)
	
\brief	Pushes a key back into the Open list.
		Use this when you're done with a key
	
\author	Paril
\date	25/05/2010
	
\param	Handle	The handle. 
**/
void CFileHandleList::PushFreeHandle (FileHandle Handle)
{
	MoveToOpen (ClosedList.find(Handle));
};

/**
\fn	FileHandle CFileHandleList::FS_GetFreeHandle (CFileHandleIndex **handle)
	
\brief	Grabs a free handle 
	
\author	Paril
\date	25/05/2010
	
\param [in,out]	handle	If non-null, the handle. 
	
\return	. 
**/
FileHandle CFileHandleList::FS_GetFreeHandle (CFileHandleIndex **handle)
{
	THandleIndexListType::iterator it = CFile::IndexList->GetFreeHandle();

	*handle = &(*it).second;
	return (*it).first + 1;
}

/**
\fn	CFileHandleIndex *CFileHandleList::FS_GetHandle (FileHandle &fileNum)
	
\brief	Returns the handle pointer for handle "fileNum"
	
\author	Paril
\date	25/05/2010
	
\param [in,out]	fileNum	The file number. 
	
\return	null if it fails, else the handle. 
**/
CFileHandleIndex *CFileHandleList::FS_GetHandle (FileHandle &fileNum)
{
	CFileHandleIndex *hIndex;

	if (fileNum < 0 || fileNum > FS_MAX_FILE_INDICES)
		CFile::OutputError ("FS_GetHandle: invalid file number");

	hIndex = CFile::IndexList->GetHandle(fileNum-1);
	if (!hIndex->inUse)
		CFile::OutputError ("FS_GetHandle: invalid handle index");

	return hIndex;
}

/**
\fn	void CFindFiles::FindFiles (CFindFilesCallback *Callback)
	
\brief	Does the actual finding.
	
\author	Paril
\date	25/05/2010
	
\param [in,out]	Callback	If non-null, the callback to call for each file. 
**/
void CFindFiles::FindFiles (CFindFilesCallback *Callback)
{
	if (Filter.IsNullOrEmpty())
		Filter = "*";
	if (Extension.IsNullOrEmpty())
		Extension = "*";

	// Search through the path, one element at a time
	for (TPathListType::iterator it = PathList.begin(); it < PathList.end(); ++it)
	{
		CPathIndex *search = (*it);

		// Directory tree
		String dir = String::Format("%s/%s", search->pathName.CString(), Path.CString());

		TFindFilesType dirFiles;

		if (!Extension.IsNullOrEmpty())
		{
			String ext = String::Format("*.%s", Extension.CString());
			Sys_FindFiles(dirFiles, dir, ext, Recurse, true, false);
		}
		else
			Sys_FindFiles(dirFiles, dir, String("*"), Recurse, true, true);

		for (size_t i = 0; i < dirFiles.size(); i++)
		{
			// Match filter
			if (Filter.CString())
			{
				if (!Q_WildcardMatch(Filter.CString(), dirFiles[i].CString()+search->pathName.Count()+1, 1))
					continue;
			}

			// Found something
			const char *name = dirFiles[i].CString() + search->pathName.Count() + 1;

			// Ignore duplicates
			bool bFound = false;
			for (size_t z = 0; z < Files.size(); z++)
			{
				if (!Q_stricmp(Files[z].CString(), name))
					break;
			}

			if (!bFound)
				Files.push_back ((AddDir) ? dirFiles[i] : String(name));
		}
	}

	if (Callback)
	{
		for (size_t i = 0; i < Files.size(); i++)
			Callback->Query (Files[i]);
	}
};

/**
\fn	void FS_Init (sint32 maxHandles)

\brief	Initializes the file system.

\author	Paril
\date	25/05/2010

\param	maxHandles	The maximum handles. 
**/
void FS_Init (sint32 maxHandles)
{
	CFile::IndexList = QNew(TAG_GENERIC) CFileHandleList (maxHandles);
	FS_AddPath (".");
	if (strcmp(GAMENAME, "baseq2"))
		FS_AddPath (GAMENAME);
	FS_AddPath ("baseq2");
}

