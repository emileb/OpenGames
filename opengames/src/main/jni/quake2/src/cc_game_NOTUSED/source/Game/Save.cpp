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
// cc_save.cpp
// 
//

#include "Local.h"
#include "Utility/ExceptionHandler.h"
#include <ctime>

bool ReadingGame = false;
CClient **SaveClientData;

#define SAVE_USE_GZ

#ifdef SAVE_USE_GZ
const int SAVE_GZ_FLAGS = (FILEMODE_GZ | FILEMODE_COMPRESS_HIGH);
#else
const int SAVE_GZ_FLAGS = 0;
#endif

const uint32 MAGIC_NUMBER = 0xf2843dfa;

void WriteMagic (CFile &File)
{
	File.Write<uint32> (MAGIC_NUMBER);
}

void ReadMagic (CFile &File)
{
	CC_ASSERT_EXPR ((File.Read<uint32> () == MAGIC_NUMBER), "Magic number mismatch");
}

// Writes the magic number
#define WRITE_MAGIC WriteMagic(File);

// Reads the magic number.
#define READ_MAGIC ReadMagic(File);

typedef std::multimap<size_t, size_t> THashedEntityTableList;
typedef std::vector<CEntityTableIndex*> TEntityTableList;
const int MAX_ENTITY_TABLE_HASH = 256;

TEntityTableList &EntityTable ()
{
	static TEntityTableList EntityTableV;
	return EntityTableV;
};
THashedEntityTableList &EntityTableHash ()
{
	static THashedEntityTableList EntityTableHashV;
	return EntityTableHashV;
};

CEntityTableIndex::CEntityTableIndex (const char *Name, IBaseEntity *(*FuncPtr) (sint32 index)) :
  Name(Name),
  FuncPtr(FuncPtr)
{
	EntityTable().push_back (this);

	// Link it in the hash tree
	EntityTableHash().insert (std::make_pair<size_t, size_t> (Com_HashGeneric (Name, MAX_ENTITY_TABLE_HASH), EntityTable().size()-1));
};

IBaseEntity *CreateEntityFromTable (sint32 index, const char *Name)
{
	uint32 hash = Com_HashGeneric(Name, MAX_ENTITY_TABLE_HASH);

	for (THashedEntityTableList::iterator it = EntityTableHash().equal_range(hash).first; it != EntityTableHash().equal_range(hash).second; ++it)
	{
		CEntityTableIndex *TableIndex = EntityTable().at((*it).second);
		if (Q_stricmp (TableIndex->Name, Name) == 0)
			return TableIndex->FuncPtr(index);
	}

	CC_ASSERT_EXPR (0, "Couldn't find entity");
	return NULL;
};

extern bool ShuttingDownEntities;
bool RemoveAll (const SEntity *it)
{
	if (it && it->Entity && it->Entity->GetGameEntity() && (it->Server.State.Number <= Game.MaxClients || it->Server.InUse))
		QDelete it->Entity;
	return true;
}

void ClearSpawnPoints();

void DeallocateEntities ()
{
	ShuttingDownEntities = true;
	Level.Entities.Closed.remove_if (RemoveAll);
	ShuttingDownEntities = false;
	Mem_FreeTag (TAG_ENTITY);
	memset(Game.Entities, 0, sizeof(SEntity*) * Game.MaxEntities);
	ClearSpawnPoints ();
};

void WriteIndex (CFile &File, MediaIndex Index, EIndexType IndexType)
{
	sint32 len = 0;
	const char *str = NULL;

	if (!Index)
		len = 0;
	else
	{
		switch (IndexType)
		{
		case INDEX_MODEL:
			str = StringFromModelIndex(Index);
			break;
		case INDEX_SOUND:
			str = StringFromSoundIndex(Index);
			break;
		case INDEX_IMAGE:
			str = StringFromImageIndex(Index);
			break;
		};

		if (!str)
		{
			// SPECIAL CASE
			// World or inline model
			len = -2;
			File.Write<sint32> (len);
			File.Write<MediaIndex> (Index);

			return;
		}
		else
			len = strlen(str) + 1;
	}

	File.Write<sint32> (len);
	if (len > 1)
	{
		if (!str)
			CC_ASSERT_EXPR (0, "How'd this happen?");
		else
			File.WriteArray (str, len);
	}
};

void ReadIndex (CFile &File, MediaIndex &Index, EIndexType IndexType)
{
	MediaIndex In = 0;
	sint32 len = File.Read<sint32> ();

	if (len == -2)
		In = File.Read<MediaIndex> ();
	else if (len > 1)
	{
		char *tempBuffer = QNew (TAG_GENERIC) char[len];
		File.ReadArray (tempBuffer, len);

		switch (IndexType)
		{
		case INDEX_MODEL:
			In = ModelIndex(tempBuffer);
			break;
		case INDEX_SOUND:
			In = SoundIndex(tempBuffer);
			break;
		case INDEX_IMAGE:
			In = ImageIndex(tempBuffer);
			break;
		};

		QDelete[] tempBuffer;
	}

	Index = In;
};

#include <typeinfo>

void WriteEntireEntity (CFile &File, IBaseEntity *Entity)
{
	// Write the SEntity info first
	File.Write<SEntity> (*Entity->GetGameEntity());

	// Write special data
	if (Entity->State.GetNumber() > Game.MaxClients)
	{
		sint32 OwnerNumber = -1;

		if (Entity->GetOwner())
			OwnerNumber = Entity->GetOwner()->State.GetNumber();

		File.Write<sint32> (OwnerNumber);
	}

	WriteIndex (File, Entity->State.GetModelIndex(), INDEX_MODEL);
	WriteIndex (File, Entity->State.GetModelIndex(2), INDEX_MODEL);
	WriteIndex (File, Entity->State.GetModelIndex(3), INDEX_MODEL);
	WriteIndex (File, Entity->State.GetModelIndex(4), INDEX_MODEL);
	WriteIndex (File, Entity->State.GetSound(), INDEX_SOUND);

	// Write entity stuff
	if (Entity->State.GetNumber() > Game.MaxClients)
	{
		File.WriteString (Entity->SAVE_GetName ());

#if WIN32 && defined(_DEBUG)
		// FIXME HACK
		if (!(Entity->EntityFlags & EF_ITEM))
		{
			//if (!strstr(Q_strlwr(std::string(typeid(*Entity).name())).CString(), Q_strlwr(std::string(Entity->SAVE_GetName())).CString()))
			String realType = (String(typeid(*Entity).name()).ToLower());
			String savedType = String("class ") + String(Entity->SAVE_GetName()).ToLower();

			if (realType != savedType)
				DebugPrintf ("%s did not write correctly (wrote as %s)\n", typeid(*Entity).name(), Entity->SAVE_GetName());
		}
#endif
	}

	WRITE_MAGIC
}

void WriteFinalizedEntity (CFile &File, IBaseEntity *Entity)
{
	Entity->WriteBaseEntity (File);

	// Call SaveFields
	Entity->SaveFields (File);

	WRITE_MAGIC
}

void WriteEntities (CFile &File)
{
	for (TEntitiesContainer::iterator it = Level.Entities.Closed.begin(); it != Level.Entities.Closed.end(); ++it)
	{
		SEntity *ent = (*it);
		if (!ent->Entity || !ent->Entity->Savable())
			continue;

		IBaseEntity *Entity = ent->Entity;

		File.Write<sint32> (Entity->State.GetNumber());
		WriteEntireEntity (File, Entity);
	}
	File.Write<sint32> (-1);

	for (TEntitiesContainer::iterator it = Level.Entities.Closed.begin(); it != Level.Entities.Closed.end(); ++it)
	{
		IBaseEntity *Entity = (*it)->Entity;
		if (!Entity || !Entity->Savable())
			continue;
		WriteFinalizedEntity (File, Entity);
	}

	WRITE_MAGIC
}

// Writes out SServerClient
void WriteClient (CFile &File, CPlayerEntity *Player)
{
	Player->Client.WriteClientStructure (File);
	Player->Client.Write (File);

	WRITE_MAGIC
}

void WriteClients (CFile &File)
{
	for (sint8 i = 1; i <= Game.MaxClients; i++)
		WriteClient (File, entity_cast<CPlayerEntity>(Game.Entities[i].Entity));
}

void ReadRealEntity (CFile &File, sint32 number)
{
	// Get the SEntity for this number
	SEntity *ent = &Game.Entities[number];

	// Restore entity
	IBaseEntity *RestoreEntity = ent->Entity;

	// Read
	*ent = File.Read<SEntity> ();

	// Make a copy
	SEntity temp (*ent);

	// Null out pointers
	ent->Server.Owner = NULL;

	// Restore entity
	ent->Entity = RestoreEntity;

	// Read special data
	if (number > Game.MaxClients)
	{
		sint32 OwnerNumber = File.Read<sint32> ();
		ent->Server.Owner = (OwnerNumber == -1) ? NULL : &Game.Entities[OwnerNumber];
	}

	ReadIndex (File, (MediaIndex &)ent->Server.State.ModelIndex, INDEX_MODEL);
	ReadIndex (File, (MediaIndex &)ent->Server.State.ModelIndex2, INDEX_MODEL);
	ReadIndex (File, (MediaIndex &)ent->Server.State.ModelIndex3, INDEX_MODEL);
	ReadIndex (File, (MediaIndex &)ent->Server.State.ModelIndex4, INDEX_MODEL);
	ReadIndex (File, (MediaIndex &)ent->Server.State.Sound, INDEX_SOUND);

	// Read entity stuff
	if (number > Game.MaxClients)
	{
		IBaseEntity *Entity;
		char *tempBuffer = File.ReadString (TAG_GENERIC);

		Entity = CreateEntityFromTable (number, tempBuffer);
		QDelete[] tempBuffer;

		// Revision:
		// This will actually change some base members..
		// Restore them all here!
		SEntity *oldOwner = ent->Server.Owner;
		memcpy (ent, &temp, sizeof(SEntity));
		ent->Server.Owner = oldOwner;

		ent->Entity = Entity;
	}

	READ_MAGIC
}

void ReadFinalizeEntity (CFile &File, IBaseEntity *Entity)
{
	Entity->ReadBaseEntity (File);

	// Call LoadFields
	Entity->LoadFields (File);

	// let the server rebuild world links for this ent
	Entity->ClearArea ();
	Entity->Link (); // If this passes, Entity loaded fine!

	READ_MAGIC
}

void ReadEntities (CFile &File)
{
	std::vector <sint32> LoadedNumbers;
	while (true)
	{
		sint32 number = File.Read<sint32> ();

		if (number == -1)
			break;

		LoadedNumbers.push_back (number);
		ReadRealEntity (File, number);
	
		if (GameAPI.GetNumEdicts() < number + 1)
			GameAPI.GetNumEdicts() = number + 1;	
	}

	// Here, load any systems that need entity lists

	for (size_t i = 0; i < LoadedNumbers.size(); i++)
		ReadFinalizeEntity (File, Game.Entities[LoadedNumbers[i]].Entity);

	World = Game.Entities[0].Entity;

	READ_MAGIC
}

void ReadClient (CFile &File, sint32 i)
{
	CClient::ReadClientStructure (File, i);
	SaveClientData[i]->Load (File);

	READ_MAGIC
}

void ReadClients (CFile &File)
{
	SaveClientData = QNew (TAG_GENERIC) CClient*[Game.MaxClients];
	for (uint8 i = 0; i < Game.MaxClients; i++)
	{
		SaveClientData[i] = QNew (TAG_GENERIC) CClient(Game.Entities[1+i].Server.Client);
		ReadClient (File, i);
	}
}

void WriteLevelLocals (CFile &File)
{
	Level.Save (File);

	WRITE_MAGIC
}

void ReadLevelLocals (CFile &File)
{
	Level.Load (File);

	READ_MAGIC
}

void WriteGameLocals (CFile &File, bool autosaved)
{
	Game.AutoSaved = autosaved;
	Game.Save (File);
	Game.AutoSaved = false;

	WRITE_MAGIC
}

void ReadGameLocals (CFile &File)
{
	Game.Load (File);

	READ_MAGIC
}

void CGameAPI::WriteGame (char *filename, bool autosave)
{
	DebugPrintf ("Writing %sgame to %s...\n", (autosave) ? "autosaved " : "", filename);

	if (!autosave)
		CPlayerEntity::SaveClientData ();

	CFile File (filename, FILEMODE_WRITE | FILEMODE_CREATE | SAVE_GZ_FLAGS);

	if (!File.Valid())
	{
		GameError ("Couldn't open %s", filename);
		return; // Fix to engines who don't shutdown on gi.error
	}

	File.WriteString (BuildDate());

	WriteGameLocals (File, autosave);
	WriteClients (File);

	WRITE_MAGIC
}

#include "Player/Ban.h"
void InitVersion ();

void CGameAPI::ReadGame (char *filename)
{
	ReadingGame = true;
	DebugPrintf ("Reading game from %s...\n", filename);

	// Free any game-allocated memory before us
	Mem_FreeTag (TAG_GAME);
	CFile File (filename, FILEMODE_READ | SAVE_GZ_FLAGS);

	if (!File.Valid())
	{
		GameError ("Couldn't open %s", filename);
		return;
	}

	char *date = File.ReadString (TAG_GENERIC);
	if (strcmp (date, BuildDate()))
	{
		GameError ("Savegame from an older version.\n");
		return;
	}
	QDelete[] date;

	seedMT (time(NULL));

	Game.Entities = QNew (TAG_GAME) SEntity[Game.MaxEntities];
	GameAPI.GetEntities() = Game.Entities;
	ReadGameLocals (File);

	Game.Clients = QNew (TAG_GAME) SServerClient[Game.MaxClients];
	for (uint8 i = 0; i < Game.MaxClients; i++)
	{
		SEntity *ent = &Game.Entities[i+1];
		ent->Server.Client = Game.Clients + i;
	}

	ReadClients (File);

	Bans.LoadFromFile ();
#if !NO_VERSION_CHECKING
	InitVersion ();
#endif

	READ_MAGIC
}

void LoadBodyQueue (CFile &File);
void SaveBodyQueue (CFile &File);
void LoadJunk (CFile &File);
void SaveJunk (CFile &File);

void CGameAPI::WriteLevel (char *filename)
{
	DebugPrintf ("Writing level to %s...\n", filename);
	CFile File (filename, FILEMODE_WRITE | FILEMODE_CREATE | SAVE_GZ_FLAGS);

	if (!File.Valid())
	{
		GameError ("Couldn't open %s", filename);
		return;
	}

	// Do this fore and firstmost
	SaveLists (File);

	// write out edict size for checking
	File.Write<size_t> (sizeof(SEntity));

	// write out a function pointer for checking
	File.Write<size_t> (reinterpret_cast<size_t>(ReadClient));

	// write out level_locals_t
	WriteLevelLocals (File);

#if ROGUE_FEATURES
	SaveBadAreas (File);
#endif

	// write out all the entities
	WriteEntities (File);

	// Write out systems
	SaveBodyQueue (File);
	SaveJunk (File);

	WRITE_MAGIC
}

#include "Player/BodyQueue.h"

char	ReadConfigSt[MAX_CFGSTRINGS][MAX_CFGSTRLEN];
void ReadConfigStrings (char *filename)
{
	size_t len = strlen(filename);

	String temp (filename);
	temp[len-3] = 's';
	temp[len-2] = 'v';
	temp[len-1] = '2';

	CFile File (temp.CString(), FILEMODE_READ);

	File.Read (ReadConfigSt, sizeof(ReadConfigSt));

	// Pump to the indexing system
	ClearList ();
	ListConfigstrings ();
}

void SetClientFields ()
{
	if (ReadingGame)
	{
		ReadingGame = false;

		for (uint8 i = 0; i < Game.MaxClients; i++)
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i+1].Entity);
			Player->Client = *SaveClientData[i];
			Player->Client.RepositionClient (Game.Entities[i+1].Server.Client);
			QDelete SaveClientData[i];
		}
		QDelete[] SaveClientData;
		SaveClientData = NULL;
	}
}

void FireCrossLevelTargets ();

void CGameAPI::ReadLevel (char *filename)
{
	// Load configstrings
	//ReadConfigStrings (filename);
	DebugPrintf ("Reading level from %s...\n", filename);
	CFile File (filename, FILEMODE_READ | SAVE_GZ_FLAGS);

	if (!File.Valid())
	{
		GameError ("Couldn't open %s", filename);
		return;
	}

	// Shut down any systems that may need shutting down first
	ShutdownBodyQueue ();
	Shutdown_Junk ();

	// Do this fore and firstmost
	ReadLists (File);

	InitGameMedia ();

	if (!ReadingGame)
		CPlayerEntity::BackupClientData ();

	// Deallocate entities
	DeallocateEntities ();
	// free any dynamic memory allocated by loading the level
	// base state
	Mem_FreeTag (TAG_LEVEL);

	// Re-initialize the systems
	BodyQueue_Init (0);
	Init_Junk ();

	// wipe all the entities
	Mem_Zero (Game.Entities, Game.MaxEntities*sizeof(Game.Entities[0]));

	InitEntityLists ();

	InitEntities (); // Get the world and players setup

	// check edict size
	size_t size = File.Read<size_t> ();
	if (size != sizeof(SEntity))
	{
		GameError ("ReadLevel: mismatched edict size");
		return;
	}

	// check function pointer base address

#ifdef WIN32
	if (File.Read<size_t> () != reinterpret_cast<size_t>(ReadClient))
	{
		GameError ("ReadLevel: function pointers have moved");
		return;
	}
#else
	DebugPrintf("Function offsets %d\n", File.Read<size_t> () - reinterpret_cast<size_t>(ReadClient));
#endif

	// load the level locals
	ReadLevelLocals (File);

#if ROGUE_FEATURES
	LoadBadAreas (File);
#endif

	// load all the entities
	ReadEntities (File);

	// Put together systems
	LoadBodyQueue (File);
	LoadJunk (File);

	// mark all clients as unconnected
	for (uint8 i = 0; i < Game.MaxClients; i++)
	{
		SEntity *ent = &Game.Entities[i+1];
		ent->Server.Client = Game.Clients + i;

		CPlayerEntity *Player = entity_cast<CPlayerEntity>(ent->Entity);
		Player->Client.RepositionClient (ent->Server.Client);
		Player->Client.Persistent.State = SVCS_FREE;
	}

	// set client fields on player ents
	if (!ReadingGame)
		CPlayerEntity::RestoreClientData ();

	// if we're loading a game, apply that info now
	SetClientFields ();

	// do any load time things at this point
	FireCrossLevelTargets ();

	READ_MAGIC
}

