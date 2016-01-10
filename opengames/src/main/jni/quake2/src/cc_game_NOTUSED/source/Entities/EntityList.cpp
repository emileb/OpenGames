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
// cc_entitylist.cpp
// Resolves an entity from a classname
//

#include "Local.h"
#include "Player/BodyQueue.h"
#include "Utility/Version.h"

const int MAX_CLASSNAME_CLASSES = 1024;

/**
\typedef	std::multimap<size_t, size_t> THashedEntityListType

\brief	Defines an alias representing type of the hashed entity list.
**/
typedef std::multimap<size_t, size_t> THashedEntityListType;

/**
\typedef	std::vector<CClassnameToClassIndex*> TEntityListType

\brief	Defines an alias representing type of the entity list.
**/
typedef std::vector<CClassnameToClassIndex*> TEntityListType;

/**
\class	CEntityList

\brief	List of entities and hashes.

\author	Paril
\date	12/06/2010
**/
class CEntityList
{
	TEntityListType			EntityList;			// List of entities
	THashedEntityListType	HashedEntityList;	// List of hashed entities

public:
	/**
	\fn	void AddToList (CClassnameToClassIndex *Entity)
	
	\brief	Adds 'Entity' index to the list.
	
	\author	Paril
	\date	12/06/2010
	
	\param [in,out]	Entity	If non-null, the entity. 
	**/
	void AddToList (CClassnameToClassIndex *Entity);

	/**
	\fn	IBaseEntity *Resolve (SEntity *ServerEntity)
	
	\brief	Resolves the entity 'ServerEntity''s classname to
			an entity and returns the created entity.
	
	\author	Paril
	\date	12/06/2010
	
	\param [in,out]	ServerEntity	If non-null, the entity. 
	
	\return	null if it fails, else the new entity. 
	**/
	IBaseEntity *Resolve (SEntity *ent);
};

// Construct on first use idiom
CEntityList &EntityList ()
{
	static CEntityList List;
	return List;
}

/**
\fn	CClassnameToClassIndex::CClassnameToClassIndex (IMapEntity *(*Spawn) (sint32 Index), const char *Classname)
	
\brief	Constructor. 
	
\author	Paril
\date	12/06/2010
	
\param [in,out]	Spawn		Spawn function. 
\param [in,out]	Classname	Classname. 
**/
CClassnameToClassIndex::CClassnameToClassIndex (IMapEntity	*(*Spawn) (sint32 Index), const char *Classname) :
  Spawn(Spawn),
  Classname(Classname)
{
	EntityList().AddToList (this);
};

/**
\fn	void CEntityList::AddToList (CClassnameToClassIndex *Entity)

\brief	Adds 'Entity' index to the list. 

\author	Paril
\date	12/06/2010

\param [in,out]	Entity	If non-null, the entity. 
**/
void CEntityList::AddToList (CClassnameToClassIndex *Entity)
{
	EntityList.push_back (Entity);

	// Link it in the hash tree
	HashedEntityList.insert (std::make_pair<size_t, size_t> (Com_HashGeneric (Entity->Classname, MAX_CLASSNAME_CLASSES_HASH), EntityList.size()-1));
};

/**
\fn	void SpawnWorld ()

\brief	Spawns the worldspawn.

\author	Paril
\date	12/06/2010
**/
void SpawnWorld ();

/**
\fn	IBaseEntity *CEntityList::Resolve (SEntity *ServerEntity)

\brief	Resolves the entity 'ServerEntity''s classname to an entity and returns the created
		entity. 

\author	Paril
\date	12/06/2010

\param [in,out]	ServerEntity	If non-null, the entity. 

\return	null if it fails, else the new entity. 
**/
IBaseEntity *CEntityList::Resolve (SEntity *ServerEntity)
{
	if (Level.ClassName == "worldspawn")
	{
		SpawnWorld ();
		return Game.Entities[0].Entity;
	}

	uint32 hash = Com_HashGeneric(Level.ClassName.CString(), MAX_CLASSNAME_CLASSES_HASH);

	for (THashedEntityListType::iterator it = HashedEntityList.equal_range(hash).first;
			it != HashedEntityList.equal_range(hash).second; ++it)
	{
		CClassnameToClassIndex *Table = EntityList.at((*it).second);
		if (Q_stricmp (Table->Classname, Level.ClassName.CString()) == 0)
			return Table->Spawn(ServerEntity->Server.State.Number);
	}

	return NULL;
}

/**
\fn	IBaseEntity *ResolveMapEntity (SEntity *ServerEntity)

\brief	Resolves entity 'ServerEntity' to a new entity. 

\author	Paril
\date	12/06/2010

\param [in,out]	ServerEntity	If non-null, the server entity. 

\return	null if it fails, else. 
**/
IBaseEntity *ResolveMapEntity (SEntity *ServerEntity)
{
	return EntityList().Resolve (ServerEntity);
};

/**
\fn	void ED_CallSpawn (SEntity *ServerEntity)

\brief	Calls the spawn function for a given entity.
		Internal.

\author	Paril
\date	29/05/2010

\param [in,out]	ServerEntity	If non-null, the entity. 
**/
void ED_CallSpawn (SEntity *ServerEntity)
{
	if (Level.ClassName.IsNullOrEmpty())
	{
		MapPrint (MAPPRINT_ERROR, NULL, vec3fOrigin, "NULL classname!\n");
		return;
	}

	// Check CleanCode stuff
	IBaseEntity *MapEntity = ResolveMapEntity(ServerEntity);

	if (!MapEntity)
	{
		MapPrint (MAPPRINT_ERROR, NULL, vec3fOrigin, "Invalid entity: %s (no spawn function)\n", Level.ClassName.CString());

CC_DISABLE_DEPRECATION
		G_FreeEdict (ServerEntity);
CC_ENABLE_DEPRECATION
		return;
	}

	// Did we get freed?
	if (MapEntity->Freed)
	{
		// We're done then
		MapEntity->GetGameEntity()->Entity = NULL;
		QDelete MapEntity;
		ServerEntity->Server.InUse = false;
		return;
	}

	// Link in the classname
	if (MapEntity->ClassName.IsNullOrEmpty())
		MapEntity->ClassName = Level.ClassName;

	if (CvarList[CV_MAP_DEBUG].Boolean())
	{
		if (MapEntity->SpawnFlags & SPAWNFLAG_NOT_EASY)
		{
			MapEntity->State.GetEffects() |= FX_COLOR_SHELL;
			MapEntity->State.GetRenderEffects() |= RF_SHELL_RED;
		}
		if (MapEntity->SpawnFlags & SPAWNFLAG_NOT_MEDIUM)
		{
			MapEntity->State.GetEffects() |= FX_COLOR_SHELL;
			MapEntity->State.GetRenderEffects() |= RF_SHELL_BLUE;
		}
		if (MapEntity->SpawnFlags & SPAWNFLAG_NOT_HARD)
		{
			MapEntity->State.GetEffects() |= FX_COLOR_SHELL;
			MapEntity->State.GetRenderEffects() |= RF_SHELL_GREEN;
		}
		if (MapEntity->SpawnFlags & SPAWNFLAG_NOT_DEATHMATCH)
		{
			MapEntity->State.GetEffects() |= FX_COLOR_SHELL;
			MapEntity->State.GetRenderEffects() |= RF_SHELL_DOUBLE;
		}
		if (MapEntity->SpawnFlags & SPAWNFLAG_NOT_COOP)
		{
			MapEntity->State.GetEffects() |= FX_COLOR_SHELL;
			MapEntity->State.GetRenderEffects() |= RF_SHELL_HALF_DAM;
		}
	}
}

/**
\fn	static void ED_ParseEdict (CParser &Data, SEntity *ServerEntity)

\brief	Parses an edict out of the given string, returning the new position ed should be a
		properly initialized empty edict. 

\author	Paril
\date	12/06/2010

\param [in,out]	Data			The parser reference. 
\param [in,out]	ServerEntity	The properly initialized server entity. 
**/
static void ED_ParseEdict (CParser &Data, SEntity *ServerEntity)
{
	bool	Initialized = false;

	Level.ParseData.clear();

	// Go through all the dictionary pairs
	while (true)
	{
		String			Key;

		// Parse key
		if (!Data.ParseToken (PSF_ALLOW_NEWLINES|PSF_TO_LOWER, Key))
			GameError ("ED_ParseEntity: EOF without closing brace");

		if (Key[0] == '}')
			break;

		String			Value;
		
		// Parse value	
		if (!Data.ParseToken (PSF_ALLOW_NEWLINES, Value) && Data.IsEOF())
			GameError ("ED_ParseEntity: EOF without closing brace");

		if (Value[0] == '}')
			GameError ("ED_ParseEntity: closing brace without data");

		Initialized = true;

		// Keynames with a leading underscore are used for utility comments,
		// and are immediately discarded by quake
		if (Key[0] == '_')
			continue;
		else if (Key == "classname")
			Level.ClassName = Value;
		else
			// push it in the list for the entity
			Level.ParseData.push_back (QNew (TAG_LEVEL) CKeyValuePair (Key, Value));
	}

	if (!Initialized)
		Mem_Zero (ServerEntity, sizeof(*ServerEntity));
}

#include "Entities/BrushModels.h"

#if ROGUE_FEATURES
/**
\fn	static void G_FixTeams ()

\brief	Fixes trains by moving them to the master of every team. 
**/
static void G_FixTeams ()
{
	int c = 0;
	int c2 = 0;
	for (int i = 1; i < GameAPI.GetNumEdicts(); i++)
	{
		IBaseEntity *e = Game.Entities[i].Entity;
		if (!e)
			continue;
		if (!e->GetInUse())
			continue;
		if (!e->Team.HasTeam)
			continue;

		if (e->ClassName == "func_train")
		{
			if (e->IsSlave())
			{
				// Paril
				// If all we contain is trains all the way down, don't bother
				bool isAllTrains = true;

				for (IBaseEntity *b = e->Team.Master; b; b = b->Team.Chain)
				{
					if (b->ClassName != "func_train")
					{
						isAllTrains = false;
						break;
					}	
				}

				if (isAllTrains)
					continue;

				IBaseEntity *chain = e;
				e->Team.Master = e;
				e->Team.Chain = NULL;

				c++;
				c2++;

				for (int j = 1; j < GameAPI.GetNumEdicts(); j++)
				{
					IBaseEntity *e2 = Game.Entities[j].Entity;
					if (!e2)
						continue;
					if (e2 == e)
						continue;
					if (!e2->GetInUse())
						continue;
					if (!e2->Team.HasTeam || e2->Team.TeamString.IsNullOrEmpty())
						continue;

					if (e->Team.TeamString == e2->Team.TeamString)
					{
						c2++;
						chain->Team.Chain = e2;
						e2->Team.Master = e;
						e2->Team.Chain = NULL;
						chain = e2;

						if (e2->EntityFlags & EF_BRUSHMODEL)
						{
							IBrushModel *Phys = entity_cast<IBrushModel>(e2);
							Phys->PhysicsType = PHYSICS_PUSH;
							Phys->MoveSpeed = Phys->Speed = entity_cast<IBrushModel>(e)->MoveSpeed;
						}
					}
				}
			}
		}
	}

	DebugPrintf ("%i teams repaired\n", c);
}
#endif

/**
\fn	static void G_FindTeams ()

\brief	Chain together all entities with a matching team field. All but the first be slaves. All
		but the last will have the teamchain field set to the next one. 
**/
static void G_FindTeams ()
{
	sint32		c = 0, c2 = 0;

	for (int i = 1; i < GameAPI.GetNumEdicts(); i++)
	{
		IBaseEntity *e = Game.Entities[i].Entity;
		if (!e)
			continue;
		if (!e->GetInUse())
			continue;
		if (e->Team.TeamString.IsNullOrEmpty())
			continue;
		if (e->IsSlave())
			continue;

		IBaseEntity *chain = e;
		e->Team.Master = e;
		e->Team.HasTeam = true;

		c++;
		c2++;
		for (int j = i + 1; j < GameAPI.GetNumEdicts(); j++)
		{
			IBaseEntity *e2 = Game.Entities[j].Entity;
			if (!e2)
				continue;
			if (!e2->GetInUse())
				continue;
			if (e2->Team.TeamString.IsNullOrEmpty())
				continue;
			if (e2->IsSlave())
				continue;

			if (e->Team.TeamString == e2->Team.TeamString)
			{
				c2++;
				chain->Team.Chain = e2;
				e2->Team.Master = e;
				e2->Team.HasTeam = true;
		
				chain = e2;
			}
		}
	}

	ServerPrintf ("%i teams with %i entities\n", c, c2);

#if ROGUE_FEATURES
	G_FixTeams ();
#endif

	for (int i = 0; i < GameAPI.GetNumEdicts(); i++)
	{
		IBaseEntity *e = Game.Entities[i].Entity;

		if (e && e->Team.HasTeam && !e->Team.TeamString.IsNullOrEmpty())
			e->Team.TeamString.Clear();
	}
}

#include "Utility/ExceptionHandler.h"

/**
\fn	void InitEntities ()

\brief	Initializes entities that are constant.
**/
void InitEntities ()
{
	// Set up the world
	SEntity *theWorld = &Game.Entities[0];

	if (!theWorld->Entity)
		theWorld->Entity = QNewEntityOf CWorldEntity(0);

	Game.Players.clear();

	// Set up the client entities
	for (sint32 i = 1; i <= Game.MaxClients; i++)
	{
		SEntity *ent = &Game.Entities[i];

		if (!ent->Entity)
		{
			CPlayerEntity *player = QNewEntityOf CPlayerEntity(i);
			ent->Entity = player;
			Game.Players.push_back(player);
		}
	}
}

char *gEntString;

void DeallocateEntities ();
void SetClientFields ();
void SetupTechSpawn ();

void FixDemoSetup ()
{
	// Deallocate entities
	DeallocateEntities ();

	InitEntityLists ();
	InitEntities ();
}

/**
\fn	void CGameAPI::SpawnEntities (char *ServerLevelName, char *Entities, char *SpawnPoint)

\brief	Creates a server's entity / program execution context by parsing textual entity
		definitions out of an ent file. 

\author	Paril
\date	12/06/2010

\param [in,out]	ServerLevelName	If non-null, name of the server level. 
\param [in,out]	Entities		If non-null, the entities. 
\param [in,out]	SpawnPoint		If non-null, the spawn point. 
**/
void CGameAPI::SpawnEntities (char *ServerLevelName, char *Entities, char *SpawnPoint)
{
	CTimer Timer;

	Level.EntityNumber = 0;
	InitMapCounter();

	sint32 skill_level = Clamp (CvarList[CV_SKILL].Integer(), 0, 3);
	if (CvarList[CV_SKILL].Integer() != skill_level)
		CvarList[CV_SKILL].Set (skill_level, true);

	CPlayerEntity::BackupClientData ();

	if (Q_stricmp (ServerLevelName + strlen(ServerLevelName) - 4, ".cin") == 0 || Q_stricmp (ServerLevelName + strlen(ServerLevelName) - 4, ".dm2") == 0)
	{
		Level.Demo = true;
		ShutdownBodyQueue ();
		Shutdown_Junk ();
	}

	// Deallocate entities
	DeallocateEntities ();

	Level.Clear ();

	Mem_FreeTag (TAG_LEVEL);
	gEntString = Mem_TagStrDup(Entities, TAG_LEVEL);

	char *oldEntities = Entities;
	bool FreeIt = false;
	Entities = CC_ParseSpawnEntities (ServerLevelName, Entities);
	if (oldEntities != Entities)
		FreeIt = true;

	Mem_Zero (Game.Entities, Game.MaxEntities * sizeof(Game.Entities[0]));
	InitEntityLists ();
	ClearTimers ();

	Level.ServerLevelName = ServerLevelName;

	Game.SpawnPoint = SpawnPoint;

	InitEntities ();

	// set client fields on player ents
	CPlayerEntity::RestoreClientData ();

	// Init module data
	CModuleContainer::InitModuleData();

	if (!Level.Demo)
	{
		Level.Inhibit = 0;

		// Parse ents
		CParser EntityParser (Entities, PSP_COMMENT_MASK);

		bool SpawnedWorld = false;
		while (true)
		{
			// Clear classname
			Level.ClassName.Clear ();

			// Parse the opening brace
			const char *token;

			if (!EntityParser.ParseToken (PSF_ALLOW_NEWLINES, &token))
				break;

			if (token[0] != '{')
				GameError ("ED_LoadFromFile: found %s when expecting {", token);

	CC_DISABLE_DEPRECATION
			SEntity *ent = (!SpawnedWorld) ? Game.Entities : G_Spawn();
	CC_ENABLE_DEPRECATION
			SpawnedWorld = true;

			ED_ParseEdict (EntityParser, ent);

			ED_CallSpawn (ent);

#if ROGUE_FEATURES
			if (ent->Entity)
				ent->Entity->State.GetRenderEffects() = RF_IR_VISIBLE;
#endif

			Level.EntityNumber++;

			if (!ent->Server.InUse)
			{
				Level.Inhibit++;
				CC_ASSERT_EXPR (!(ent->Entity && !ent->Entity->Freed), "Entity not inuse but freed!");
			}
		}

		ServerPrintf ("%i entities removed (out of %i total)\n", Level.Inhibit, Level.EntityNumber);

		G_FindTeams ();

		SetupTechSpawn();

#if ROGUE_FEATURES
		InitBadAreas ();
#endif
	}
	else
		ServerPrint ("Demo detected, skipping map init.\n");

#if !NO_VERSION_CHECKING
	InitVersion ();
#endif
	ServerPrintf ("Finished server initialization in "TIMER_STRING"\n", Timer.Get());

	if (FreeIt)
		QDelete[] Entities;

	PlayerTrail_Init ();
}

