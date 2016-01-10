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
// cc_baseentity.cpp
// Base entity class code
//

#include "Local.h"
#include <algorithm>
#include "Items/Items.h"

int CEntityField::GetItemIndex (IBaseItem *Item) const
{
	return Item->GetIndex();
}

/**
\fn	IBaseEntity *GetGameEntity (sint32 Index)

\brief	Gets the game entity for index 'Index'.
		Required magic.

\author	Paril
\date	29/05/2010

\param	Index	Zero-based index of the entity. 

\return	null if it fails, else the game entity. 
**/
IBaseEntity *GetGameEntity (sint32 Index)
{
	return Game.Entities[Index].Entity;
}

CEntityField::CEntityField (const char *Name, size_t Offset, EFieldType FieldType, TValidateFieldFunction ValidateField) :
  Name(String(Name).ToLower()),
  Offset(Offset),
  FieldType(FieldType),
  StrippedFields(FieldType & ~(FT_GAME_ENTITY | FT_SAVABLE | FT_NOSPAWN)),
  ValidateField(ValidateField)
{
};

CEntityState::CEntityState () :
  state(NULL)
{
};

CEntityState::CEntityState (SEntityState *state) :
  state(state)
{
};

void		CEntityState::Initialize (SEntityState *state)
{
	this->state = state;
};

/**
\fn	sint32 &CEntityState::GetNumber ()

	\brief	Gets the entity's number.

\return	The number. 
**/
sint32		&CEntityState::GetNumber		()
{
	return state->Number;
}

/**
\fn	vec3f &CEntityState::GetOrigin ()

	\brief	Gets the entity's origin. 

\return	The origin. 
**/
vec3f	&CEntityState::GetOrigin		()
{
	return state->Origin;
}

/**
\fn	vec3f &CEntityState::GetAngles ()

\brief	Gets the entity's angles. 

\return	The angles. 
**/
vec3f	&CEntityState::GetAngles		()
{
	return state->Angles;
}

/**
\fn	vec3f &CEntityState::GetOldOrigin ()

\brief	Gets the entity's old origin. 

\return	The old origin. 
**/
vec3f	&CEntityState::GetOldOrigin	()
{
	return state->OldOrigin;
}

/**
\fn	sint32 &CEntityState::GetModelIndex (uint8 index)

\brief	Gets a model index. 

\author	Paril
\date	29/05/2010

\param	index	One-based index.
				Defaults to 1; 2 is modelIndex2, etc.

\return	The model index. 
**/
sint32		&CEntityState::GetModelIndex	(uint8 index)
{
	switch (index)
	{
	case 1:
		return state->ModelIndex;
	case 2:
		return state->ModelIndex2;
	case 3:
		return state->ModelIndex3;
	case 4:
		return state->ModelIndex4;
	default:
		CC_ASSERT_EXPR(0, "index for GetModelIndex is out of bounds");
		return state->ModelIndex;
	}
}

/**
\fn	sint32 &CEntityState::GetFrame ()

\brief	Gets the entity's animation frame. 

\return	The frame. 
**/
sint32		&CEntityState::GetFrame		()
{
	return state->Frame;
}

/**
\fn	sint32 &CEntityState::GetSkinNum ()

\brief	Gets the entity's skin number. 

\return	The skin number. 
**/
sint32	&CEntityState::GetSkinNum		()
{
	return state->SkinNum;
}

/**
\fn	EEntityStateEffects &CEntityState::GetEffects ()

\brief	Gets the entity's effects. 

\return	The effects. 
**/
EEntityStateEffects	&CEntityState::GetEffects		()
{
	return state->Effects;
}

/**
\fn	EEntityStateRenderEffects &CEntityState::GetRenderEffects ()

\brief	Gets the entity's render effects. 

\return	The render effects. 
**/
EEntityStateRenderEffects		&CEntityState::GetRenderEffects	()
{
	return state->RenderFx;
}

/**
\fn	MediaIndex &CEntityState::GetSound ()

\brief	Gets the entity's sound. 

\return	The sound. 
**/
MediaIndex	&CEntityState::GetSound		()
{
	return (MediaIndex&)state->Sound;
}

/**
\fn	EEventEffect &CEntityState::GetEvent ()

\brief	Gets the entity's event. 

\return	The event. 
**/
EEventEffect	&CEntityState::GetEvent			()
{
	return state->Event;
}

/**
\fn	void G_InitEdict (SEntity *ServerEntity)

\brief	Initialize entity.

\deprecated	Deprecated. Do not use.
			Function is not required (only internally)

\author	Paril
\date	29/05/2010

\param	ServerEntity	Entity to initialize. 
**/
void G_InitEdict (SEntity *ServerEntity)
{
	ServerEntity->Server.InUse = true;
	ServerEntity->FreeTime = 0;
	ServerEntity->AwaitingRemoval = false;
	ServerEntity->RemovalFrames = 0;
	memset (&ServerEntity->Server.State, 0, sizeof(ServerEntity->Server.State));
	ServerEntity->Server.State.Number = ServerEntity - Game.Entities;
}

template <typename TCont, typename TType>
void listfill (TCont &List, TType Data, size_t numElements)
{
	List.clear ();

	for (size_t i = 0; i < numElements; i++)
		List.push_back (&Data[i]);
}

/**
\fn	void InitEntityLists ()

\brief	Initialises the entity lists.
**/
void InitEntityLists ()
{
	listfill <TEntitiesContainer, SEntity*> (Level.Entities.Open, Game.Entities, Game.MaxEntities);
	Level.Entities.Closed.clear();

	// Keep the first few entities in the closed list
	for (uint8 i = 0; i < (1 + Game.MaxClients); i++)
	{
		Level.Entities.Closed.push_back (Level.Entities.Open.front());
		Level.Entities.Open.pop_front();
	}
}

/**
\fn	SEntity *CLevelLocals::CEntityList::GetEntityFromList ()

\brief	Gets a free entity from Open, pushes into Closed.
		Internal function.

\return	null if it fails, else the entity from list. 
**/
SEntity *CLevelLocals::CEntityList::GetEntityFromList ()
{
	if (Open.empty())
		return NULL;

	// Take entity off of list, obeying FreeTime
	SEntity *ent = NULL;

	TEntitiesContainer::iterator it;
	for (it = Open.begin(); it != Open.end(); ++it)
	{
		SEntity *check = (*it);

		// the first couple seconds of server time can involve a lot of
		// freeing and allocating, so relax the replacement policy
		if (!check->Server.InUse && (check->FreeTime < 20 || Level.Frame - check->FreeTime > 5))
		{
			ent = check;
			break;
		}
	}

	if (ent == NULL)
		return NULL;

	Open.erase (it);
	
	// Put into closed
	Closed.push_back (ent);
	return ent; // Give it to us
}

/**
\fn	void CLevelLocals::CEntityList::RemoveEntityFromOpen (SEntity *ent)

\brief	Removes the entity 'ent' from Open and pushes into Closed. 

\author	Paril
\date	29/05/2010

\param [in,out]	ent	If non-null, the entity. 
**/
void CLevelLocals::CEntityList::RemoveEntityFromOpen (SEntity *ent)
{
	Open.remove (ent);
	Closed.push_back (ent);
}

/**
\fn	void CLevelLocals::CEntityList::RemoveEntityFromList (SEntity *ent)

\brief	Removes the entity 'ent' from Closed, pushes into front of Open. 

\author	Paril
\date	29/05/2010

\param [in,out]	ent	If non-null, the entity. 
**/
void CLevelLocals::CEntityList::RemoveEntityFromList (SEntity *ent)
{
	// Take entity out of list
	Closed.remove (ent);

	// Push into Open
	Open.push_front (ent);
}

#include "Player/BodyQueue.h"

/**
\fn	SEntity *G_Spawn ()

\deprecated	Use CreateEntityFromClassname instead.

\brief	Either finds a free edict, or allocates a new one.
		Try to avoid reusing an entity that was recently freed, because it
		can cause the client to think the entity morphed into something else
		instead of being removed and recreated, which can cause interpolated
		angles and bad trails.

\return	null if it fails, else. 
**/
SEntity *G_Spawn ()
{
	SEntity *e = Level.Entities.GetEntityFromList ();
	
	if (e == NULL)
	{
		GameError ("ED_Alloc: no free edicts");
		return NULL;
	}

CC_DISABLE_DEPRECATION
	G_InitEdict (e);
CC_ENABLE_DEPRECATION

	if (GameAPI.GetNumEdicts() < e->Server.State.Number + 1)
		GameAPI.GetNumEdicts() = e->Server.State.Number + 1;

	return e;
}

/**
\fn	void G_FreeEdict (SEntity *ServerEntity)

\deprecated	Use IBaseEntity::Free instead.

\brief	Marks the entity as freed. 

\author	Paril
\date	29/05/2010

\param [in,out]	ServerEntity	If non-null, the entity. 
**/
void G_FreeEdict (SEntity *ServerEntity)
{
CC_DISABLE_DEPRECATION
	gi.unlinkentity (ServerEntity);		// unlink from world
CC_ENABLE_DEPRECATION

	// Paril, hack
	IBaseEntity *Entity = ServerEntity->Entity;

	Mem_Zero (ServerEntity, sizeof(*ServerEntity));
	if (Entity)
		ServerEntity->Entity = Entity;
	ServerEntity->FreeTime = Level.Frame;
	ServerEntity->Server.InUse = false;
	ServerEntity->Server.State.Number = ServerEntity - Game.Entities;

	if (!ServerEntity->AwaitingRemoval)
	{
		ServerEntity->AwaitingRemoval = true;
		ServerEntity->RemovalFrames = 2;
	}
}

typedef std::vector <IBaseEntity*> TPrivateEntitiesContainer;
TPrivateEntitiesContainer PrivateEntities;

/**
\fn	void InitPrivateEntities ()

\brief	Initialises the private entities list. 
**/
void InitPrivateEntities ()
{
	PrivateEntities.clear ();
}

/**
\fn	void RunPrivateEntities ()

\brief	Runs all private entities.
**/
void			RunPrivateEntities ()
{
	TPrivateEntitiesContainer::iterator it = PrivateEntities.begin();

	while (it != PrivateEntities.end())
	{
		IBaseEntity *Entity = (*it);
		
		Level.CurrentEntity = Entity;

		IThinkableEntity *Thinkable = (!Entity->Freed && (Entity->EntityFlags & EF_THINKABLE)) ? Entity->ThinkEntity : NULL;

		if (Thinkable) 
			Thinkable->PreThink ();

		Entity->Run ();	

		if (Thinkable)
			Thinkable->RunThink ();

		// Were we freed?
		// This has to be processed after thinking and running, because
		// the entity still has to be intact after that
		if (Entity->Freed)
		{
			it = PrivateEntities.erase (it);
			QDelete Entity;
		}
		else
			++it;
	}
};

/**
\fn	IBaseEntity::IBaseEntity ()

\brief	Default constructor. Creates a new entity from a free entity. 

\author	Paril
\date	29/05/2010
**/
IBaseEntity::IBaseEntity ()
{
CC_DISABLE_DEPRECATION
	gameEntity = G_Spawn ();
CC_ENABLE_DEPRECATION
	gameEntity->Entity = this;
	ClassName = "noclass";

	Freed = false;
	EntityFlags |= EF_BASE;

	State.Initialize (&gameEntity->Server.State);
};

/**
\fn	IBaseEntity::IBaseEntity (sint32 Index)

\brief	Constructor. This constructor initializes an entity using a specific entity index.
		
		\attention	If writing a new entity class, you NEED (and this is 100% REQUIRED)
		to call this constructor, even if IBaseEntity isn't a direct base class. This is due to a
		C++98 limitation. 

\author	Paril
\date	29/05/2010

\param	Index	Zero-based index of the. 
**/
IBaseEntity::IBaseEntity (sint32 Index)
{
	if (Index < 0)
	{
		Freed = false;
		EntityFlags |= (EF_PRIVATE | EF_BASE);

		gameEntity = NULL;
		PrivateEntities.push_back (this);
	}
	else
	{
		gameEntity = &Game.Entities[Index];
		gameEntity->Entity = this;
		gameEntity->Server.State.Number = Index;

		Freed = false;
		EntityFlags |= EF_BASE;
		State.Initialize (&gameEntity->Server.State);
	}
}

bool ShuttingDownEntities = false;
/**
\fn	IBaseEntity::~IBaseEntity ()

\brief	Destructor. 

\author	Paril
\date	29/05/2010
**/
IBaseEntity::~IBaseEntity ()
{
	if (!ShuttingDownEntities)
	{
		if (gameEntity)
		{
			gameEntity->Entity = NULL;

	CC_DISABLE_DEPRECATION
			if (!Freed && !(EntityFlags & EF_JUNK))
				G_FreeEdict (gameEntity); // "delete" the entity
	CC_ENABLE_DEPRECATION
		}
		else
		{
			for (TPrivateEntitiesContainer::iterator it = PrivateEntities.begin(); it < PrivateEntities.end(); ++it)
			{
				if ((*it) == this)
				{
					PrivateEntities.erase (it);
					break;
				}
			}
		}
	}
};

/**
\fn	void IBaseEntity::WriteBaseEntity (CFile &File)

\brief	Writes this entity's base members to a file. 

\author	Paril
\date	29/05/2010

\param [in,out]	File	The file. 
**/
void IBaseEntity::WriteBaseEntity (CFile &File)
{
	File.Write<bool> (Freed);
	File.Write<EEntityFlags> (EntityFlags);
	File.Write<EEdictFlags> (Flags);

	File.Write (ClassName);

	File.Write<bool> (Team.HasTeam);

	if (Team.HasTeam)
	{
		File.Write<sint32> ((Team.Chain && Team.Chain->gameEntity) ? Team.Chain->State.GetNumber() : -1);
		File.Write<sint32> ((Team.Master && Team.Master->gameEntity) ? Team.Master->State.GetNumber() : -1);
	}

	GroundEntity.Write (File);

	File.Write<sint32> (GroundEntityLinkCount);
	File.Write<uint32> (SpawnFlags);

	Enemy.Write (File);

	File.Write<sint32> (ViewHeight);
}

/**
\fn	void IBaseEntity::ReadBaseEntity (CFile &File)

\brief	Reads this entity's base members from a file. 

\author	Paril. 
\date	29/05/2010. 

\param [in,out]	File	The file. 
**/
void IBaseEntity::ReadBaseEntity (CFile &File)
{
	Freed = File.Read<bool> ();
	EntityFlags = File.Read<uint32> ();
	Flags = File.Read<EEdictFlags> ();

	ClassName = File.ReadString ();

	Team.HasTeam = File.Read<bool> ();

	if (Team.HasTeam)
	{
		sint32 ChainNumber = File.Read<sint32> ();
		sint32 MasterNumber = File.Read<sint32> ();

		if (ChainNumber != -1)
			Team.Chain = Game.Entities[ChainNumber].Entity;
		if (MasterNumber != -1)
			Team.Master = Game.Entities[MasterNumber].Entity;
	}

	GroundEntity = entity_ptr<IBaseEntity>::Read(File);

	GroundEntityLinkCount = File.Read<sint32> ();
	SpawnFlags = File.Read<uint32> ();

	Enemy = entity_ptr<IBaseEntity>::Read(File);

	ViewHeight = File.Read<sint32> ();
}

/**
\fn	IBaseEntity *IBaseEntity::GetOwner ()

\brief	Gets the owner of this entity.
See "Entities - Owners" for details. 

\return	null if it fails, else the entity. 
**/
IBaseEntity		*IBaseEntity::GetOwner	()
{
	return (gameEntity->Server.Owner) ? gameEntity->Server.Owner->Entity : NULL;
}

/**
\fn	void IBaseEntity::SetOwner (IBaseEntity *Entity)
	
\brief	Sets the owner of this entity.
		See "Entities - Owners" for details.
	
\author	Paril
\date	29/05/2010
	
\param [in,out]	Entity	If non-null, the new owner.
						If null, owner is set to nothing.
**/
void			IBaseEntity::SetOwner	(IBaseEntity *Entity)
{
	if (!Entity || !Entity->gameEntity)
	{
		gameEntity->Server.Owner = NULL;
		return;
	}

	gameEntity->Server.Owner = Entity->gameEntity;
}

/**
\fn	EBrushContents &IBaseEntity::GetClipmask ()

\brief	Gets the clipmask of this entity. 

\return	The clipmask. 
**/
EBrushContents	&IBaseEntity::GetClipmask	()
{
	return gameEntity->Server.ClipMask;
}

/**
\fn	ESolidType &IBaseEntity::GetSolid ()

\brief	Gets the solidity of this entity. 

\return	The solidity. 
**/
ESolidType		&IBaseEntity::GetSolid ()
{
	return gameEntity->Server.Solid;
}

/**
\fn	vec3f &IBaseEntity::GetMins ()

\brief	Gets the min bounds of this entity. 

\return	The mins. 
**/
vec3f			&IBaseEntity::GetMins ()
{
	return gameEntity->Server.Mins;
}

/**
\fn	vec3f &IBaseEntity::GetMaxs ()

\brief	Gets the max bounds of this entity. 

\return	The maxs. 
**/
vec3f			&IBaseEntity::GetMaxs ()
{

	return gameEntity->Server.Maxs;
}

/**
\fn	vec3f &IBaseEntity::GetAbsMin ()

\brief	Gets the absolute mins of this entity. 

\return	The absolute mins (essentially origin + mins). 
**/
vec3f			&IBaseEntity::GetAbsMin ()
{
	return gameEntity->Server.AbsMin;
}

/**
\fn	vec3f &IBaseEntity::GetAbsMax ()

\brief	Gets the absolute maxs of this entity. 

\return	The absolute maxs (essentially origin + maxs). 
**/
vec3f			&IBaseEntity::GetAbsMax ()
{
	return gameEntity->Server.AbsMax;
}

/**
\fn	vec3f &IBaseEntity::GetSize ()

\brief	Gets the size of this entity. 

\return	The size. 
**/
vec3f			&IBaseEntity::GetSize ()
{
	return gameEntity->Server.Size;
}

/**
\fn	EServerFlags &IBaseEntity::GetSvFlags ()

\brief	Gets the server flags of this entity. 

\return	The server flags. 
**/
EServerFlags	&IBaseEntity::GetSvFlags ()
{
	return gameEntity->Server.ServerFlags;
}

/**
\fn	sint32 IBaseEntity::GetAreaNum (bool second)

\brief	Gets this entity's area number. 

\author	Paril
\date	29/05/2010

\param	second	true if you want areaNum2. 

\return	areaNum2 if second, otherwise areaNum. 
**/
sint32				IBaseEntity::GetAreaNum (bool second)
{
	return ((second) ? gameEntity->Server.AreaNum2 : gameEntity->Server.AreaNum);
}

/**
\fn	SAreaLink *IBaseEntity::GetArea ()

\brief	Gets this entity's area pointer. 

\return	null if it fails, else the area. 
**/
SAreaLink			*IBaseEntity::GetArea ()
{
	return &gameEntity->Server.Area;
}

/**
\fn	void IBaseEntity::ClearArea ()

\brief	Clears this entity's area. 
**/
void			IBaseEntity::ClearArea ()
{
	Mem_Zero (&gameEntity->Server.Area, sizeof(gameEntity->Server.Area));
}

/**
\fn	sint32 IBaseEntity::GetLinkCount ()

\brief	Gets this entity's link count. 

\return	The link count. 
**/
sint32				IBaseEntity::GetLinkCount ()
{
	return gameEntity->Server.LinkCount;
}

/**
\fn	bool &IBaseEntity::GetInUse ()

\brief	Gets the usage state of this entity. 

\return	True if in-use, false if not. 
**/
bool			&IBaseEntity::GetInUse ()
{
	return (bool&)gameEntity->Server.InUse;
}

CC_DISABLE_DEPRECATION
/**
\fn	void IBaseEntity::Link ()

\brief	Links this entity to the world.
		See "Entities - Linking". 
**/
void			IBaseEntity::Link ()
{
	gi.linkentity (gameEntity);
}

/**
\fn	void IBaseEntity::Unlink ()
	
\brief	Unlinks this entity from the world.
		See "Entities - Linking"
**/
void			IBaseEntity::Unlink ()
{
	gi.unlinkentity (gameEntity);
}
CC_ENABLE_DEPRECATION

/**
\fn	void IBaseEntity::Free ()

\brief	Frees this entity. 
**/
void			IBaseEntity::Free ()
{
	if (gameEntity)
	{
		Unlink ();

		Mem_Zero (gameEntity, sizeof(*gameEntity));
		gameEntity->Entity = this;
		gameEntity->FreeTime = Level.Frame;
		GetInUse() = false;
		gameEntity->Server.State.Number = gameEntity - Game.Entities;

		if (!(EntityFlags & EF_JUNK))
		{
			if (Level.Frame == 0)
				Level.Entities.RemoveEntityFromList (gameEntity);
			else if (!gameEntity->AwaitingRemoval)
			{
				gameEntity->AwaitingRemoval = true;
				gameEntity->RemovalFrames = 2;
			}
		}
	}

	FixAllEntityPtrs (this);
	Freed = true;
}

/**
\fn	void IBaseEntity::PlaySound (ESoundChannel Channel, MediaIndex Sound, uint8 Volume,
	EAttenuation Attenuation, uint8 TimeOfs)

\brief	Plays a sound 'soundIndex' on channel 'channel' with volume 'volume', attenuation
		'attenuation' and time offset 'timeOfs'. 

\author	Paril
\date	29/05/2010

\param	Channel		The channel. 
\param	Sound		The sound. 
\param	Volume		The volume. 
\param	Attenuation	The attenuation. 
\param	TimeOfs		The time ofs. 
**/
void	IBaseEntity::PlaySound (ESoundChannel Channel, MediaIndex Sound, uint8 Volume, EAttenuation Attenuation, uint8 TimeOfs)
{
	if ((Channel != CHAN_AUTO) && (Channel < CHAN_MAX))
	{
		if (PlayedSounds[Channel-1])
			return;
		else
			PlayedSounds[Channel-1] = true;
	}

	PlaySoundFrom (this, Channel, Sound, Volume, Attenuation, TimeOfs);
};

/**
\fn	void IBaseEntity::PlayPositionedSound (vec3f Origin, ESoundChannel Channel, MediaIndex Sound,
	uint8 Volume, EAttenuation Attenuation, uint8 TimeOfs)

\brief	Plays a positioned sound 'soundIndex' at position 'Origin' on channel 'channel' with
		volume 'volume', attenuation 'attenuation' and time offset 'timeOfs'. 

\author	Paril
\date	29/05/2010

\param	Origin		The origin. 
\param	Channel		The channel. 
\param	Sound		Sound index of the sound. 
\param	Volume		The volume. 
\param	Attenuation	The attenuation. 
\param	TimeOfs		The time offset. 
**/
void	IBaseEntity::PlayPositionedSound (vec3f Origin, ESoundChannel Channel, MediaIndex Sound, uint8 Volume, EAttenuation Attenuation, uint8 TimeOfs)
{
	if ((Channel != CHAN_AUTO) && (Channel < CHAN_MAX))
	{
		if (PlayedSounds[Channel-1])
			return;
		else
			PlayedSounds[Channel-1] = true;
	}

	PlaySoundAt (Origin, this, Channel, Sound, Volume, Attenuation, TimeOfs);
};

/**
\fn	void IBaseEntity::KillBox ()

\brief	Creates a killbox around this player, instantly gibbing anything inside of it. 
**/
void	IBaseEntity::KillBox ()
{
	while (1)
	{
		CTrace tr (State.GetOrigin(), GetMins(), GetMaxs(), State.GetOrigin(), NULL, CONTENTS_MASK_PLAYERSOLID);
		if (!tr.Entity)
			break;

		if ((tr.Entity->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(tr.Entity)->CanTakeDamage)
		{
			// nail it
			entity_cast<IHurtableEntity>(tr.Entity)->TakeDamage (this, this, vec3fOrigin, State.GetOrigin(),
																vec3fOrigin, 100000, 0, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
		}

		if (tr.Entity->GetSolid())
			break;
	}
};

/**
\fn	void IBaseEntity::SplashDamage (IBaseEntity *Attacker, float Damage, IBaseEntity *Ignore,
	float Radius, EMeansOfDeath MeansOfDeath)

\brief	Performs splash damage around this entity. 

\author	Paril
\date	29/05/2010

\param [in,out]	Attacker	If non-null, the attacker. 
\param	Damage				The damage. 
\param [in,out]	Ignore		If non-null, the entity to ignore. 
\param	Radius				The radius. 
\param	MeansOfDeath		The means of death.  
**/
void IBaseEntity::SplashDamage (IBaseEntity *Attacker, float Damage, IBaseEntity *Ignore, float Radius, EMeansOfDeath MeansOfDeath)
{
	IHurtableEntity	*Entity = NULL;

	while ((Entity = FindRadius<IHurtableEntity, EF_HURTABLE> (Entity, State.GetOrigin(), Radius)) != NULL)
	{
		if (Entity == Ignore)
			continue;
		if (!Entity->CanTakeDamage)
			continue;

		float points = Damage - 0.5 * (State.GetOrigin() - Entity->State.GetOrigin().MultiplyAngles (0.5f, Entity->GetMins() + Entity->GetMaxs())).Length();

		if (Entity == Attacker)
			points *= 0.5;
		if ((points > 0) && Entity->DamageCanReach (this))
			Entity->TakeDamage (this, Attacker, Entity->State.GetOrigin() - State.GetOrigin(), State.GetOrigin(), vec3fOrigin, (sint32)points, (sint32)points, DAMAGE_RADIUS, MeansOfDeath);
	}
}

static inline void BuildBoxPoints(vec3f p[8], vec3f &org, vec3f &mins, vec3f &maxs)
{
	p[0] = org + mins;
	p[1] = p[0] - vec3f(mins.X, 0, 0);
	p[2] = p[0] - vec3f(0, mins.Y, 0);
	p[3] = p[0] - vec3f(mins.X, mins.Y, 0);

	p[4] = org + maxs;
	p[5] = p[4] - vec3f(maxs.X, 0, 0);
	p[6] = p[0] - vec3f(0, maxs.Y, 0);
	p[7] = p[0] - vec3f(maxs.X, maxs.Y, 0);
}

/**
\fn	bool IBaseEntity::CanSee (IBaseEntity *Other)

\brief	Queries if we can see 'Other'. This is an entire bbox trace check, not just visibility. 

\author	Paril
\date	29/05/2010

\param [in,out]	Other	If non-null, the other. 

\return	true if it succeeds, false if it fails. 
**/
bool IBaseEntity::CanSee (IBaseEntity *Other)
{
	// bmodels need special checking because their origin is 0,0,0
	if ((EntityFlags & EF_PHYSICS) && (entity_cast<IPhysicsEntity>(this))->PhysicsType == PHYSICS_PUSH)
		return false; // bmodels not supported

	vec3f	targpoints[8];
	BuildBoxPoints(targpoints, State.GetOrigin(), GetMins(), GetMaxs());
	
	vec3f viewpoint = Other->State.GetOrigin() + vec3f(0,0,Other->ViewHeight);
	for (uint8 i = 0; i < 8; i++)
	{
		CTrace trace (viewpoint, targpoints[i], Other, CONTENTS_MASK_SOLID);
		if (trace.Fraction == 1.0)
			return true;
	}

	return false;
}

/**
\fn	void IBaseEntity::TouchTriggers ()

\brief	Cause all triggers that this entity is touching to activate.
**/
void IBaseEntity::TouchTriggers ()
{
	TBoxEdictsEntityList Entities = BoxEdicts (GetAbsMin(), GetAbsMax(), true);

	// be careful, it is possible to have an entity in this
	// list removed before we get to it (killtriggered)
	for (TBoxEdictsEntityList::iterator it = Entities.begin(); it < Entities.end(); ++it)
	{
		IBaseEntity *TouchedEntity = (*it);

		if (!TouchedEntity || !TouchedEntity->GetInUse())
			continue;

		if (TouchedEntity->EntityFlags & EF_TOUCHABLE)
		{
			(entity_cast<ITouchableEntity>(TouchedEntity))->Touch (this, NULL, NULL);
			continue;
		}

		if (!GetInUse())
			break;
	}
}

#if ROGUE_FEATURES

/**
\class	CNukePlayerCallBack

\brief	Nuke callback.
		Sets a player's nuke timer based off distance to nuke.

\author	Paril
\date	29/05/2010
**/
class CNukePlayerCallBack : public CForEachPlayerCallback
{
public:
	IBaseEntity *Inflictor;

	CNukePlayerCallBack (IBaseEntity *Inflictor) :
	Inflictor(Inflictor)
	{
	};

	void Callback (CPlayerEntity *Player)
	{
		if (Player->Client.Timers.Nuke != Level.Frame + 20)
		{
			CTrace tr (Inflictor->State.GetOrigin(), Player->State.GetOrigin(), Inflictor, CONTENTS_MASK_SOLID);

			if (tr.Fraction == 1.0)
				Player->Client.Timers.Nuke = Level.Frame + 20;
			else
				Player->Client.Timers.Nuke = Max<>(Player->Client.Timers.Nuke, Level.Frame + (RangeFrom (Player->State.GetOrigin(), Inflictor->State.GetOrigin()) < 2048) ? 15 : 10);
		}
	}
};

/**
\fn	void IBaseEntity::NukeSplashDamage (IBaseEntity *Attacker, float Damage, IBaseEntity *Ignore,
	float Radius, EMeansOfDeath MeansOfDeath)

\brief	Performs nuke splash damage around this entity. Nuke damage ignores walls and makes
		entities not gib (vaporize). 

\author	Paril
\date	29/05/2010

\param [in,out]	Attacker	If non-null, the attacker. 
\param	Damage				The damage. 
\param [in,out]	Ignore		If non-null, the ignore. 
\param	Radius				The radius. 
\param	MeansOfDeath		The means of death. 
**/
void IBaseEntity::NukeSplashDamage (IBaseEntity *Attacker, float Damage, IBaseEntity *Ignore, float Radius, EMeansOfDeath MeansOfDeath)
{
	float	points;
	IHurtableEntity	*Entity = NULL;
	vec3f	v;
	vec3f	dir;
	float	len;

	float killzone = Radius;
	float killzone2 = Radius*2.0;

	while ((Entity = FindRadius<IHurtableEntity, EF_HURTABLE> (Entity, State.GetOrigin(), Radius)) != NULL)
	{
// ignore nobody
		if (Entity == Ignore)
			continue;
		if (!Entity->CanTakeDamage)
			continue;
		if (!Entity->GetInUse())
			continue;

		v = (Entity->GetMins() + Entity->GetMaxs());
		v = Entity->State.GetOrigin().MultiplyAngles (0.5f, v);
		v = State.GetOrigin() - v;

		len = v.Length();

		if (len <= killzone)
			points = 10000;
		else if (len <= killzone2)
			points = (Damage/killzone)*(killzone2 - len);
		else
			points = 0;

		if (points > 0)
		{
			if (Entity->EntityFlags & EF_PLAYER)
				(entity_cast<CPlayerEntity>(Entity))->Client.Timers.Nuke = Level.Frame + 20;
			dir = Entity->State.GetOrigin() - State.GetOrigin();
			Entity->TakeDamage (this, Attacker, dir, State.GetOrigin(), vec3fOrigin, (int)points, (int)points, DAMAGE_RADIUS, MeansOfDeath);
		}
	}

	CNukePlayerCallBack (this).Query ();
}
#endif

#include "Utility/TemporaryEntities.h"

/**
\fn	void IBaseEntity::BecomeExplosion (bool Grenade)

\brief	Visually 'turns' the entity into an explosion and frees it. 

\author	Paril
\date	29/05/2010

\param	Grenade	true if grenade explosion, false if rocket. 
**/
void IBaseEntity::BecomeExplosion (bool Grenade)
{
	if (Grenade)
		CGrenadeExplosion (State.GetOrigin()).Send();
	else
		CRocketExplosion (State.GetOrigin()).Send();
	Free ();
}

/**
\fn	IMapEntity::IMapEntity ()

\brief	Default constructor. 

\author	Paril
\date	29/05/2010
**/
IMapEntity::IMapEntity () : 
IBaseEntity()
{
	EntityFlags |= EF_MAP;
};

/**
\fn	IMapEntity::IMapEntity (sint32 Index)

\brief	Entity constructor. 

\author	Paril
\date	29/05/2010

\param	Index	Zero-based index of the entity. 
**/
IMapEntity::IMapEntity (sint32 Index) : 
IBaseEntity(Index)
{
	EntityFlags |= EF_MAP;
};

ENTITYFIELDS_BEGIN(IMapEntity)
{
	CEntityField ("spawnflags",		EntityMemberOffset(IBaseEntity,SpawnFlags),		FT_UINT | FT_SAVABLE),
	CEntityField ("origin",			GameEntityMemberOffset(Server.State.Origin),	FT_VECTOR | FT_GAME_ENTITY),
	CEntityField ("angles",			GameEntityMemberOffset(Server.State.Angles),	FT_VECTOR | FT_GAME_ENTITY),
	CEntityField ("angle",			GameEntityMemberOffset(Server.State.Angles),	FT_YAWANGLE | FT_GAME_ENTITY),
	CEntityField ("light",			0,												FT_IGNORE),
	CEntityField ("team",			EntityMemberOffset(IBaseEntity,Team.TeamString),	FT_STRING),
};
ENTITYFIELDS_END(IMapEntity)

const CEntityField IMapEntity::FieldsForParsing_Map[] =
{
	CEntityField ("targetname",		EntityMemberOffset(IMapEntity,TargetName),		FT_STRING | FT_SAVABLE),
};
const size_t IMapEntity::FieldsForParsingSize_Map = ArrayCount(IMapEntity::FieldsForParsing_Map);

/**
\fn	bool IMapEntity::ParseField (const char *Key, const char *Value)

\brief	Parse a single field from an entity. See "Entities - Saving/Loading". 
\author	Paril. 
\date	29/05/2010. 

\param	Key		The key. 
\param	Value	The value. 

\return	true if it succeeds, false if it fails. 
**/
bool			IMapEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<IMapEntity, IBaseEntity> (this, Key, Value))
		return true;
	else
	{
		for (size_t i = 0; i < IMapEntity::FieldsForParsingSize_Map; i++)
		{
			if (!(IMapEntity::FieldsForParsing_Map[i].FieldType & FT_NOSPAWN) && (strcmp (Key, IMapEntity::FieldsForParsing_Map[i].Name.CString()) == 0))
			{
				IMapEntity::FieldsForParsing_Map[i].Create<IMapEntity> (this, Value);
				return true;
			}
		}
	}

	// Couldn't find it here
	return false;
};

void IMapEntity::SaveFields (CFile &File)
{
	for (size_t i = 0; i < IMapEntity::FieldsForParsingSize_Map; i++)
	{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
		if (IMapEntity::FieldsForParsing_Map[i].FieldType & FT_SAVABLE)
		{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
			IMapEntity::FieldsForParsing_Map[i].Save<IMapEntity> (this, File);
		}
	}
};

void IMapEntity::LoadFields (CFile &File)
{
	for (size_t i = 0; i < IMapEntity::FieldsForParsingSize_Map; i++)
	{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
		if (IMapEntity::FieldsForParsing_Map[i].FieldType & FT_SAVABLE)
		{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
			IMapEntity::FieldsForParsing_Map[i].Load<IMapEntity> (this, File);
		}
	}
};

/**
\fn	bool IMapEntity::CheckValidity ()

\brief	Checks the validity of this entity. An entity that was invalid will not spawn, and will
		be subsequently freed. 

\return	true if valid, false if not. 
**/
bool				IMapEntity::CheckValidity ()
{
	// Remove things (except the world) from different skill levels or deathmatch
	if (this != World)
	{
		if (!CvarList[CV_MAP_DEBUG].Boolean())
		{
			if (Game.GameMode & GAME_DEATHMATCH)
			{
				if ( SpawnFlags & SPAWNFLAG_NOT_DEATHMATCH )
				{
					Free ();
					return false;
				}
			}
			else
			{
				if ( /* ((Game.GameMode & GAME_COOPERATIVE) && (SpawnFlags & SPAWNFLAG_NOT_COOP)) || */
					((CvarList[CV_SKILL].Integer() == 0) && (SpawnFlags & SPAWNFLAG_NOT_EASY)) ||
					((CvarList[CV_SKILL].Integer() == 1) && (SpawnFlags & SPAWNFLAG_NOT_MEDIUM)) ||
					((CvarList[CV_SKILL].Integer() >= 2) && (SpawnFlags & SPAWNFLAG_NOT_HARD))
					)
					{
						Free ();
						return false;
					}
			}

			SpawnFlags &= ~(SPAWNFLAG_NOT_EASY|SPAWNFLAG_NOT_MEDIUM|SPAWNFLAG_NOT_HARD|SPAWNFLAG_NOT_COOP|SPAWNFLAG_NOT_DEATHMATCH);
		}
	}
	return true;
};

/**
\fn	void IMapEntity::ParseFields ()

\brief	Parse the fields from an entity.
		See "Entities - Saving/Loading". 
**/
void IMapEntity::ParseFields ()
{
	if (!Level.ParseData.size())
		return;

	// Go through all the dictionary pairs
	TKeyValuePairContainer::iterator it = Level.ParseData.begin();
	while (it != Level.ParseData.end())
	{
		CKeyValuePair *PairPtr = (*it);
		if (ParseField (PairPtr->Key.CString(), PairPtr->Value.CString()))
			Level.ParseData.erase (it++);
		else
			++it;
	}

	// Since this is the last part, go through the rest of the list now
	// and report ones that are still there.
	if (Level.ParseData.size())
	{
		for (it = Level.ParseData.begin(); it != Level.ParseData.end(); ++it)
		{
			CKeyValuePair *PairPtr = (*it);
			MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "\"%s\" is not a field (value = \"%s\")\n", PairPtr->Key.CString(), PairPtr->Value.CString());
		}
	}
};
