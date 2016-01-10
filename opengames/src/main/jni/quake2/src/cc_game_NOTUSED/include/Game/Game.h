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
// cc_game.h
// 
//

#if !defined(CC_GUARD_CC_GAME_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_GAME_H

/**
\typedef	uint8 EGameMode

\brief	Defines an alias representing the game mode.
**/
typedef uint8 EGameMode;

/**
\enum	

\brief	Values that represent game modes. 
**/
enum
{
	GAME_SINGLEPLAYER		=	BIT(0),
	GAME_DEATHMATCH			=	BIT(1),
	GAME_COOPERATIVE		=	BIT(2),

#if CLEANCTF_ENABLED
	GAME_CTF				=	BIT(3)
#endif
};

/**
\typedef	uint16 ECrossLevelTriggerFlags

\brief	Defines an alias representing the cross level trigger flags.
**/
typedef uint16 ECrossLevelTriggerFlags;

/**
\enum	

\brief	Values that represent cross-level trigger flags. 
**/
enum
{
	SFL_CROSS_TRIGGER_1		= BIT(0),
	SFL_CROSS_TRIGGER_2		= BIT(1),
	SFL_CROSS_TRIGGER_3		= BIT(2),
	SFL_CROSS_TRIGGER_4		= BIT(3),
	SFL_CROSS_TRIGGER_5		= BIT(4),
	SFL_CROSS_TRIGGER_6		= BIT(5),
	SFL_CROSS_TRIGGER_7		= BIT(6),
	SFL_CROSS_TRIGGER_8		= BIT(7),
	SFL_CROSS_TRIGGER_MASK	= 255
};

//
// this structure is left intact through an entire game
// it should be initialized at dll load time, and read/written to
// the server.ssv file for savegames
//
class CGameLocals
{
public:
	String					HelpMessages[2];
	uint8					HelpChanged;	// flash F1 icon if non 0, play sound
								// and increment only if 1, 2, or 3

	SServerClient				*Clients;		// [maxclients]
	SEntity						*Entities;
	std::vector<CPlayerEntity*>	Players;

	// can't store spawnpoint in level, because
	// it would get overwritten by the savegame restore
	String					SpawnPoint;	// needed for coop respawns

	// store latched cvars here that we want to get at often
	uint8					MaxClients;
	uint8					MaxSpectators;
	sint32					MaxEntities;
	bool					CheatsEnabled;
	EGameMode				GameMode; // Game mode

	// cross level triggers
	ECrossLevelTriggerFlags	ServerFlags;
	bool					AutoSaved;

	bool					R1Protocol;

	CGameLocals () :
	  HelpChanged (0),
	  Clients (NULL),
	  MaxClients (0),
	  MaxSpectators (0),
	  MaxEntities (0),
	  CheatsEnabled (false),
	  GameMode (0),
	  ServerFlags (0),
	  AutoSaved (false),
	  SpawnPoint ()
	  {
		  HelpMessages[0] = HelpMessages[1] = "";
	  };

	void Save (CFile &File)
	{
		File.Write (HelpMessages[0]);
		File.Write (HelpMessages[1]);
		File.Write (SpawnPoint);
		File.Write<uint8> (HelpChanged);
		File.Write<uint8> (MaxClients);
		File.Write<uint8> (MaxSpectators);
		File.Write<sint32> (MaxEntities);
		File.Write<bool> (CheatsEnabled);
		File.Write<EGameMode> (GameMode);
		File.Write<ECrossLevelTriggerFlags> (ServerFlags);
		File.Write<bool> (AutoSaved);
	}

	void Load (CFile &File)
	{
		HelpMessages[0] = File.ReadString ();
		HelpMessages[1] = File.ReadString ();
		SpawnPoint = File.ReadString ();
		HelpChanged = File.Read<uint8> ();
		MaxClients = File.Read<uint8> ();
		MaxSpectators = File.Read<uint8> ();
		MaxEntities = File.Read<sint32> ();
		CheatsEnabled = File.Read<bool> ();
		GameMode = File.Read<EGameMode> ();
		ServerFlags = File.Read<ECrossLevelTriggerFlags> ();
		AutoSaved = File.Read<bool> ();
	}
};

extern	CGameLocals		Game;

//
// this structure is cleared as each map is entered
// it is read/written to the Level.sav file for savegames
//

struct SGoalList
{
	uint8		Total;
	uint8		Found;
};

class CKeyValuePair
{
public:
	String			Key;
	String			Value;

	CKeyValuePair (String Key, String Value) :
	  Key(Key),
	  Value(Value)
	{
	};
};

typedef std::list<CKeyValuePair*> TKeyValuePairContainer;
typedef std::list<SEntity*> TEntitiesContainer;

class CLevelLocals
{
public:
	void Clear ()
	{
		Frame = 0;
		FullLevelName.Clear ();
		ServerLevelName.Clear ();
		NextMap.Clear ();
		ForceMap.Clear ();
		Intermission.Clear();
		SightClient = NULL;
		SightEntity = NULL;
		SightEntityFrame = 0;
		SoundEntity = NULL;
		SoundEntityFrame = 0;
		SoundEntity2 = NULL;
		SoundEntity2Frame = 0;
		CurrentEntity = NULL;
		PowerCubeCount = 0;
		Inhibit = 0;
		EntityNumber = 0;
		ClassName.Clear ();
		ParseData.clear ();
		Demo = false;

		for (TKeyValuePairContainer::iterator it = ParseData.begin(); it != ParseData.end(); ++it)
			QDelete (*it);
		ParseData.clear();

		Secrets.Found = Secrets.Total = 0;
		Goals.Found = Goals.Total = 0;
		Monsters.Killed = Monsters.Total = 0;
		Entities.Open.clear ();
		Entities.Closed.clear ();
	};

	CLevelLocals ()
	{
		Clear ();
	};

	void Save (CFile &File);
	void Load (CFile &File);

	FrameNumber				Frame;
		
	String					FullLevelName;		// the descriptive name (Outer Base, etc)
	String					ServerLevelName;		// the server name (base1, etc)
	String					NextMap;		// go here when fraglimit is hit
	String					ForceMap;		// go here

	// intermission state
	struct SIntermissionState
	{
		FrameNumber		Time;		// time the intermission was started
#if !STDCPP_LINUX_HACK
		String			ChangeMap;
#else
		char*			ChangeMap;
#endif
		bool			ShouldExit;
		bool			ShouldExitOnNextFrame;
		vec3f			Origin;
		vec3f			Angles;
		void Clear() 
		{
			Time = 0;
			ChangeMap.Clear();
			ShouldExit = ShouldExitOnNextFrame = false;
			Origin.X = Origin.Y = Origin.Z = 0.0f;
		    Angles.X = Angles.Y = Angles.Z = 0.0f;
	    }
	}						Intermission;

	CPlayerEntity			*SightClient;	// changed once each frame for coop games

	IBaseEntity				*SightEntity;
	FrameNumber				SightEntityFrame;
	IBaseEntity				*SoundEntity;
	FrameNumber				SoundEntityFrame;
	IBaseEntity				*SoundEntity2;
	FrameNumber				SoundEntity2Frame;

	SGoalList				Secrets;
	SGoalList				Goals;

	struct SMonsterCount
	{
		uint16			Total;
		uint16			Killed;
	}						Monsters;
		
	IBaseEntity				*CurrentEntity;	// entity running from G_RunFrame

	uint8					PowerCubeCount;		// ugly necessity for coop
	uint32					Inhibit;
	uint32					EntityNumber;

	String					ClassName;
	TKeyValuePairContainer	ParseData;

	bool					Demo;

#if ROGUE_FEATURES
	IBaseEntity				*DisguiseViolator;
	FrameNumber				DisguiseViolationFrametime;
#endif

	// Entity list
	class CEntityList
	{
	public:
		TEntitiesContainer		Open, Closed;

		void Save (CFile &File);
		void Load (CFile &File);

		/**
		\fn	SEntity *GetEntityFromList ()
		
		\brief	Gets a free entity from Open, pushes into Closed.
				Internal function.
		
		\return	null if it fails, else the entity from list. 
		**/
		SEntity		*GetEntityFromList ();

		/**
		\fn	void RemoveEntityFromOpen (SEntity *ent)
		
		\brief	Removes the entity 'ent' from Open and pushes into Closed. 
		
		\author	Paril
		\date	29/05/2010
		
		\param [in,out]	ent	If non-null, the entity. 
		**/
		void		RemoveEntityFromOpen (SEntity *ent);

		/**
		\fn	void RemoveEntityFromList (SEntity *ent)
		
		\brief	Removes the entity 'ent' from Closed, pushes into front of Open. 
		
		\author	Paril
		\date	29/05/2010
		
		\param [in,out]	ent	If non-null, the entity. 
		**/
		void		RemoveEntityFromList (SEntity *ent);
	} Entities;

};

extern	CLevelLocals	Level;

#else
FILE_WARNING
#endif
