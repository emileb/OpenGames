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
// cc_gameapi.h
// Contains overloads and other neat things
//

#if !defined(CC_GUARD_GAMEAPI_H) || !INCLUDE_GUARDS
#define CC_GUARD_GAMEAPI_H

// the "gameversion" client command will print this plus compile date
#if !defined(GAMENAME)
	#if (CC_GAME_MODE == GAME_ROGUE)
		#define GAMENAME	"rogue"
	#elif (CC_GAME_MODE == GAME_XATRIX)
		#define GAMENAME	"xatrix"
	#elif (CC_GAME_MODE == GAME_ORIGINAL_QUAKE2)
		#define GAMENAME	"baseq2"
	#else
		#define GAMENAME	"baseq2"
	#endif
#endif

// Game-defined entity structure
struct SEntity
{
	SServerEntity		Server;

	//
	// only used locally in game, not by server
	//

	FrameNumber			FreeTime;			// sv.time when the object was freed
	bool				AwaitingRemoval;
	uint8				RemovalFrames;
	
	// Paril
	IBaseEntity				*Entity;
};

const int GAME_APIVERSION		= 3;

//
// functions provided by the main engine
//
typedef float vec3_t[3];
struct SGameImport
{
	// special messages
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (BroadcastPrintf)
#endif
	void	(*bprintf) (EGamePrintLevel printlevel, const char *fmt, ...);
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (ServerPrintf or DebugPrintf)
#endif
	void	(*dprintf) (const char *fmt, ...);
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (ClientPrintf)
#endif
	void	(*cprintf) (SEntity *ent, EGamePrintLevel printLevel, const char *fmt, ...);
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (CenterPrintf)
#endif
	void	(*centerprintf) (SEntity *ent, const char *fmt, ...);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (PlaySoundFrom or IBaseEntity->PlaySound)
#endif
	void	(*sound) (SEntity *ent, sint32 channel, sint32 soundIndex, float volume, float attenuation, float timeOffset);
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (PlaySoundAt or IBaseEntity->PlayPositionedSound)
#endif
	void	(*positioned_sound) (vec3_t origin, SEntity *ent, sint32 channel, sint32 soundIndex, float volume, float attenuation, float timeOffset);

	// config strings hold all the index strings, the lightstyles,
	// and misc data like the sky definition and cdtrack.
	// All of the current configstrings are sent to clients when
	// they connect, and changes are sent to all connected clients.
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (ConfigString)
#endif
	void	(*configstring) (sint32 num, const char *string);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (GameError)
#endif
	void	(*error) (const char *fmt, ...);

	// the *index functions create configstrings and some internal server state
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (ModelIndex)
#endif
	sint32		(*modelindex) (const char *name);
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (SoundIndex)
#endif
	sint32		(*soundindex) (const char *name);
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (ImageIndex)
#endif
	sint32		(*imageindex) (const char *name);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (SetModel)
#endif
	void	(*setmodel) (SEntity *ent, const char *name);

	// collision detection
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (class CTrace)
#endif
	STrace	(*trace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, SEntity *passEnt, sint32 contentMask);
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (PointContents)
#endif
	sint32			(*pointcontents) (vec3_t point);
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (InArea)
#endif
	BOOL		(*inPVS) (vec3_t p1, vec3_t p2);
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (InArea)
#endif
	BOOL		(*inPHS) (vec3_t p1, vec3_t p2);
	void		(*SetAreaPortalState) (sint32 portalNum, BOOL open);
	BOOL		(*AreasConnected) (sint32 area1, sint32 area2);

	// an entity will never be sent to a client or used for collision
	// if it is not passed to linkentity.  If the size, position, or
	// solidity changes, it must be relinked.
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE ((IBaseEntity)->Link)
#endif
		void	(*linkentity) (SEntity *ent);
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE ((IBaseEntity)->Unlink)
#endif
	void	(*unlinkentity) (SEntity *ent);		// call before removing an interactive edict
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (BoxEdicts)
#endif
	sint32		(*BoxEdicts) (vec3_t mins, vec3_t maxs, SEntity **list,	sint32 maxCount, sint32 areaType);

	void	(*Pmove) (
	SPMove *pMove
	);		// player movement code common with client prediction

	// network messaging
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (Cast)
#endif
	void	(*multicast) (vec3_t origin, EMultiCast to);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (Cast)
#endif
	void	(*unicast) (SEntity *ent, BOOL reliable);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (WriteChar)
#endif
	void	(*WriteChar) (sint32 c);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (WriteByte)
#endif
	void	(*WriteByte) (sint32 c);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (WriteShort)
#endif
	void	(*WriteShort) (sint32 c);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (WriteLong)
#endif
	void	(*WriteLong) (sint32 c);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (WriteFloat)
#endif
	void	(*WriteFloat) (float f);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (WriteString)
#endif
	void	(*WriteString) (const char *s);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (WritePosition)
#endif
	void	(*WritePosition) (vec3_t pos);	// some fractional bits

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (WriteDir)
#endif
	void	(*WriteDir) (vec3_t pos);		// single uint8 encoded, very coarse

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (WriteAngle)
#endif
	void	(*WriteAngle) (float f);

	// managed memory allocation
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (QNew)
#endif
	void	*(*TagMalloc) (sint32 size, sint32 tag);
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (QDelete)
#endif
	void	(*TagFree) (void *block);
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (Mem_FreeTag)
#endif
	void	(*FreeTags) (sint32 tag);

	// console variable interaction
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (class CCvar)
#endif
	SCVar	*(*cvar) (const char *varName, const char *value, sint32 flags);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (class CCvar)
#endif
	SCVar	*(*cvar_set) (const char *varName, const char *value);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (class CCvar)
#endif
	SCVar	*(*cvar_forceset) (const char *varName, const char *value);

	// ClientCommand and ServerCommand parameter access
#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (ArgCount)
#endif
	sint32		(*argc) ();

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (ArgGets/ArgGeti/ArgGetf)
#endif
	char	*(*argv) (sint32 n);

#if !USE_DEPRECATED_GAME_IMPORTS
	CC_INSECURE_DEPRECATE (ArgGetConcatenatedString)
#endif
	char	*(*args) ();	// concatenation of all argv >= 1

	// add commands to the server console as if they were typed in
	// for map changing, etc
	void	(*AddCommandString) (const char *text);

	void	(*DebugGraph) (float value, sint32 color);
};

extern	SGameImport	gi;

// C++ wrapper
class CGameAPI
{
public:
	CGameAPI () {}
	virtual ~CGameAPI () {}

	virtual void Init ();
	virtual void Shutdown ();

	/**
	\fn	void SpawnEntities (char *ServerLevelName, char *Entities, char *SpawnPoint)

	\brief	Creates a server's entity / program execution context by parsing textual entity
			definitions out of an ent file. 

	\author	Paril
	\date	12/06/2010

	\param [in,out]	ServerLevelName	If non-null, name of the server level. 
	\param [in,out]	Entities		If non-null, the entities. 
	\param [in,out]	SpawnPoint		If non-null, the spawn point. 
	**/
	virtual void SpawnEntities (char *mapName, char *entString, char *spawnPoint);
	
	virtual void WriteGame (char *fileName, bool autosave);
	virtual void ReadGame (char *fileName);
	virtual void WriteLevel (char *fileName);
	virtual void ReadLevel (char *fileName);

	virtual bool ClientConnect (CPlayerEntity *Player, char *userinfo);
	virtual void ClientBegin (CPlayerEntity *Player);
	virtual void ClientUserinfoChanged (CPlayerEntity *Player, char *userinfo);
	virtual void ClientDisconnect (CPlayerEntity *Player);
	virtual void ClientCommand (CPlayerEntity *Player);
	virtual void ClientThink (CPlayerEntity *Player, SUserCmd *cmd);

	virtual void RunFrame ();

	virtual void ServerCommand ();

	SEntity *&GetEntities ();
	sint32 &GetEdictSize ();
	sint32 &GetNumEdicts();
	sint32 &GetMaxEdicts();
};

// If you make a new class for a game API, replace this.
#define GAME_CLASS	CGameAPI

extern GAME_CLASS	GameAPI;

/**
\typedef	uint8 ECastType

\brief	Defines an alias representing type of a cast.
**/
typedef uint8 ECastType;

/**
\enum	

\brief	Values that represent the types of a cast. 
**/
enum
{
	// Cast type
	CAST_MULTI,
	CAST_UNI
};

/**
\typedef	uint8 ECastFlags

\brief	Defines an alias representing the cast flags.
**/
typedef uint8 ECastFlags;

/**
\enum	

\brief	Values that represent cast flags. 
**/
enum
{
	// Paril: whoops...
	CASTFLAG_NONE = 0,
	CASTFLAG_UNRELIABLE = 1,
	CASTFLAG_PVS = 2,
	CASTFLAG_PHS = 4,
	CASTFLAG_RELIABLE = 8
};

void Cast (ECastType castType, ECastFlags castFlags, vec3f &Origin, CPlayerEntity *Ent, bool SuppliedOrigin);
void Cast (ECastFlags castFlags, CPlayerEntity *Ent);
void Cast (ECastFlags castFlags, vec3f &Origin);

EBrushContents PointContents (vec3f start);

typedef std::vector<IBaseEntity*> TBoxEdictsEntityList;
TBoxEdictsEntityList BoxEdicts (vec3f mins, vec3f maxs, bool triggers);

void ConfigString (sint32 configStringIndex, const char *configStringValue, ...);
void GameError (const char *fmt, ...);

CC_DISABLE_DEPRECATION

// Hearable false = PVS, otherwise PHS
inline bool InArea (bool Hearable, vec3f &p1, vec3f &p2)
{
	if (Hearable)
		return (gi.inPHS (p1, p2) == 1);
	return (gi.inPVS (p1, p2) == 1);
}

inline bool InHearableArea (vec3f &p1, vec3f &p2) { return InArea (true, p1, p2); }
inline bool InVisibleArea (vec3f &p1, vec3f &p2) { return InArea (false, p1, p2); }

CC_ENABLE_DEPRECATION

#else
FILE_WARNING
#endif
