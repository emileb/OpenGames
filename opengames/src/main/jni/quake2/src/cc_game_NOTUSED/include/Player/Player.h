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
// cc_playerentity.h
// Player entity
//

#if !defined(CC_GUARD_PLAYERENTITY_H) || !INCLUDE_GUARDS
#define CC_GUARD_PLAYERENTITY_H

class IWeaponBase;
class CInventory;
class CAmmo;
class CTech;
class CFlag;
class CArmor;
class IBaseItem;

#include "Items/Inventory.h"

class CPlayerState
{
	friend class CClient;
protected:
	SPlayerState	*playerState; // Private so no one mucks with it if they shouldn't

public:
	CPlayerState (SPlayerState *playerState);

	void			Initialize (SPlayerState *playerState);
	
	SPMoveState	*GetPMove (); // Direct pointer
	void			SetPMove (SPMoveState *newState);

	// Unless, of course, you use the vec3f class :D
	vec3f			&GetViewAngles ();
	vec3f			&GetViewOffset ();

	vec3f			&GetGunAngles ();
	vec3f			&GetGunOffset ();
	vec3f			&GetKickAngles ();

	MediaIndex		&GetGunIndex ();

	sint32			&GetGunFrame ();

	colorf			&GetViewBlend ();
	colorb			GetViewBlendB (); // Name had to be different

	float			&GetFov ();

	ERenderDefFlags	&GetRdFlags ();

	sint16			&GetStat (uint8 index);
	void			CopyStats (sint16 *Stats);
	sint16			*GetStats ();

	void			Clear ();
};

/**
\typedef	uint8 EGender

\brief	Defines an alias representing a gender.
**/
typedef uint8 EGender;

/**
\enum	

\brief	Values that represent a gender. 
**/
enum
{
	GENDER_MALE,
	GENDER_FEMALE,
	GENDER_NEUTRAL
};

/**
\typedef	uint8 EClientState

\brief	Defines an alias representing state of a client's connection.
**/
typedef uint8 EClientState;

/**
\enum	

\brief	Values that represent the state of a client's connection. 
**/
enum
{
	SVCS_FREE,		// can be reused for a new connection
	SVCS_CONNECTED,	// has been assigned to a svClient_t, but not in game yet
	SVCS_SPAWNED	// client is fully in game
};

void LoadWeapon (CFile &File, IWeaponBase **Weapon);
void SaveWeapon (CFile &File, IWeaponBase *Weapon);

#include "Player/Userinfo.h"

/**
\typedef	uint8 EHandedness

\brief	Defines an alias representing the handedness of a player.
**/
typedef uint8 EHandedness;

/**
\enum	

\brief	Values that represent the handedness of a player. 
**/
enum
{
	RIGHT_HANDED,
	LEFT_HANDED,
	CENTER_HANDED
};

// client data that stays across multiple level loads
class CPersistentData
{
public:
	CPersistentData ();
	~CPersistentData();

	CUserInfo		UserInfo;
	String			Name;
	IPAddress		IP;
	EHandedness		Hand;

	EClientState 	State;			// a loadgame will leave valid entities that
									// just don't have a connection yet

	// values saved and restored from edicts when changing levels
	sint32			Health;
	sint32			MaxHealth;
	sint32			SavedFlags;

	CInventory		Inventory;

	// ammo capacities
	sint32			MaxAmmoValues[AMMOTAG_MAX];

	IWeaponBase			*Weapon, *LastWeapon;
	CArmor			*Armor; // Current armor.
#if CLEANCTF_ENABLED
	CFlag			*Flag; // Set if holding a flag
#endif
	CTech			*Tech; // Set if holding a tech

	sint32			PowerCubeCount;	// used for tracking the cubes in coop games
	sint32			Score;			// for calculating total unit Score in coop games

	uint8			GameHelpChanged;
	uint8			HelpChanged;

	bool			Spectator;			// client is a Spectator

	colorf			ViewBlend; // View blending
	SUserCmd			UserCmd;		// User command, stored each ClientThink for using in modules.

	void Save (CFile &File);
	void Load (CFile &File);
	void Clear ();
};

// All players have a copy of this class.
class CMenuState
{
public:
	/**
	\typedef	uint8 EMenuKeys
	
	\brief	Defines an alias representing a menu key.
	**/
	typedef uint8 EMenuKeys;

	/**
	\enum	
	
	\brief	Values that represent menu keys. 
	**/
	enum
	{
		KEY_NONE,
		KEY_LEFT,
		KEY_RIGHT
	};

	CPlayerEntity		*Player; // A pointer to the entity that is running the show (needed?)
	sint8				Cursor; // Cursor position (relative to order)
	EMenuKeys			Key; // Key hit this frame
	// CurrentMenu is set to a copy of whatever menu class
	// you need
	class CMenu			*CurrentMenu;
	bool				InMenu;

	CMenuState			();
	CMenuState			(CPlayerEntity *Player);

	void Initialize		(CPlayerEntity *Player);

	void OpenMenu		(); // Do this AFTER setting CurrentMenu
	void CloseMenu		();

	void SelectNext		(); // invnext
	void SelectPrev		(); // invprev
	void Select			(); // invuse

	void Clear ()
	{
		Player = NULL;
		Cursor = -1;
		Key = KEY_NONE;
		CurrentMenu = NULL;
		InMenu = false;
	}
};

// client data that stays across deathmatch respawns
class CRespawnData
{
public:
	CRespawnData ()
	{
		Clear ();
	};

	void Save (CFile &File)
	{
		CoopRespawn.Save (File);
		File.Write<FrameNumber> (EnterFrame);
		File.Write<sint32> (Score);
		File.Write<vec3f> (CmdAngles);
		File.Write<bool> (Spectator);
		File.Write<EGender> (Gender);
		File.Write<EGamePrintLevel> (MessageLevel);
	}

	void Load (CFile &File)
	{
		CoopRespawn.Load (File);
		EnterFrame = File.Read<FrameNumber> ();
		Score = File.Read<sint32> ();
		CmdAngles = File.Read<vec3f> ();
		Spectator = File.Read<bool> ();
		Gender = File.Read<EGender> ();
		MessageLevel = File.Read<EGamePrintLevel> ();
	}

	CPersistentData		CoopRespawn;	// what to set client->Persistent to on a respawn
	FrameNumber			EnterFrame;		// Level.Frame the client entered the game
	sint32				Score;			// frags, etc
	vec3f				CmdAngles;		// angles sent over in the last command

	bool				Spectator;		// client is a Spectator
	EGender				Gender;
	EGamePrintLevel		MessageLevel;
#if CLEANCODE_IRC
	CIRCClient			IRC;
#endif

	CMenuState			MenuState;

#if CLEANCTF_ENABLED
//ZOID
	struct SCTFData
	{
		ETeamIndex		Team;					// CTF team
		uint8			State;
		FrameNumber		LastHurtCarrier,
						LastReturnedFlag,
						FlagSince,
						LastFraggedCarrier;

		bool			IDState;
		bool			Voted;					// for elections
		bool			Admin;
	} CTF;
//ZOID
#endif

	// Module data
	CModuleDataContainer	ModuleData;
	
	void Clear ()
	{
		CoopRespawn.Clear ();
		EnterFrame = 0;
		Score = 0;
		CmdAngles.Clear();
		Spectator = false;
		Gender = GENDER_NEUTRAL;
		MessageLevel = 0;
		MenuState.Clear ();
#if CLEANCTF_ENABLED
		Mem_Zero (&CTF, sizeof(CTF));
#endif
	}
};

/**
\typedef	uint8 ELayoutFlags

\brief	Defines an alias representing the layout flags.
**/
typedef uint8 ELayoutFlags;

/**
\enum	

\brief	Values that represent flags for a client's layout system. 
**/
enum
{
	LF_SHOWSCORES		= BIT(0),
	LF_SHOWINVENTORY	= BIT(1),
	LF_SHOWHELP			= BIT(2),
	LF_UPDATECHASE		= BIT(3),

	LF_SCREEN_MASK		= (LF_SHOWSCORES | LF_SHOWINVENTORY | LF_SHOWHELP),
};

/**
\typedef	sint16 EDamageType

\brief	Defines an alias representing the type of damage dealt to a player.
**/
typedef uint8 EDamageType;

/**
\enum	

\brief	Values that represent the type of damage dealt to a player. 
**/
enum
{
	DT_ARMOR,			// damage absorbed by armor
	DT_POWERARMOR,		// damage absorbed by power armor
	DT_BLOOD,			// damage taken out of health
	DT_KNOCKBACK,		// impact damage

	DT_MAX
};

/**
\typedef	uint8 EWeaponState

\brief	Defines an alias representing state of a weapon.
**/
typedef uint8 EWeaponState;

/**
\enum	

\brief	Values that represent the state of a weapon.
		This could be used for animations and such.
**/
enum
{
	WS_ACTIVATING,
	WS_IDLE,
	WS_FIRING,
	WS_DEACTIVATING
};

/**
\typedef	uint8 EPowerArmorType

\brief	Defines an alias representing the type of power armor worn.
**/
typedef uint8 EPowerArmorType;

/**
\enum	

\brief	Values that represent the type of power armor worn.
**/
enum
{
	POWER_ARMOR_NONE,
	POWER_ARMOR_SCREEN,
	POWER_ARMOR_SHIELD
};

class CPlayerNoise : public virtual IBaseEntity
{
public:
	FrameNumber	Time;

	CPlayerNoise () :
	  IBaseEntity ()
	{
		EntityFlags |= EF_NOISE;
	};

	CPlayerNoise (sint32 Index) :
	  IBaseEntity (Index)
	{
		EntityFlags |= EF_NOISE;
	};

	void SaveFields (CFile &File)
	{
		File.Write<FrameNumber> (Time);
	}

	void LoadFields (CFile &File)
	{
		Time = File.Read<FrameNumber> ();
	}

	IMPLEMENT_SAVE_HEADER(CPlayerNoise);
};

/**
\typedef	uint8 EAnimPriority

\brief	Defines an alias representing the animation types.
**/
typedef uint8 EAnimPriority;

/**
\enum	

\brief	Values that represent animation types. 
**/
enum
{
	ANIM_BASIC,
	ANIM_WAVE,
	ANIM_JUMP,
	ANIM_PAIN,
	ANIM_ATTACK,
	ANIM_DEATH,
	ANIM_REVERSE
};

// view pitching times
const int	DAMAGE_TIME		= 5;
const int	FALL_TIME		= 3;

class CClient
{
public:
	void Write (CFile &File);	
	void Load (CFile &File);

	void WriteClientStructure (CFile &File);
	static void ReadClientStructure (CFile &File, sint32 index);
	void RepositionClient (SServerClient *client);

protected:
	SServerClient		*client; // Private so no one messes it up!

public:
	CPlayerState	PlayerState;
	CPersistentData	Persistent;
	CRespawnData	Respawn;

	vec3f			KickAngles;	// weapon kicks
	vec3f			KickOrigin;
	vec3f			ViewAngle;			// aiming direction
	vec3f			DamageFrom;		// origin for vector calculation
	colorf			DamageBlend;
	IBaseEntity		*mynoise;		// can go in client only
	IBaseEntity		*mynoise2;
	vec3f			OldViewAngles;
	vec3f			OldVelocity;
	float			ViewDamage[2];
	FrameNumber		ViewDamageTime;
	float			KillerYaw;			// when dead, look at killer
	SPMoveState		OldPMove;	// for detecting out-of-pmove changes
	ELayoutFlags	LayoutFlags;
	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	sint32			DamageValues[DT_MAX];
	EButtons		Buttons;
	EButtons		LatchedButtons;
	IWeaponBase			*NewWeapon;
	EWeaponState	WeaponState;
	FrameNumber		FallTime;
	float			FallValue;		// for view drop on fall
	float			BonusAlpha;
	float			BobTime;			// so off-ground doesn't change it
	uint8			PowerArmorTime;
	bool			PowerArmorEnabled;
	MediaIndex		WeaponSound;

#if ROGUE_FEATURES
	class CRogueBaseSphere	*OwnedSphere;
#endif

	// animation vars
	struct SClientAnimation
	{
		uint16			EndFrame;
		EAnimPriority	Priority;
		bool			Duck, Run;
	} Anim;

	// powerup timers
	struct SClientTimers
	{
		FrameNumber			QuadDamage,
							Invincibility,
							Rebreather,
							EnvironmentSuit
#if XATRIX_FEATURES
						,	QuadFire
#endif

#if ROGUE_FEATURES
						, Double,
							IR,
							Nuke,
							Tracker
#endif
							;

		sint16				SilencerShots;

		uint8				MachinegunShots;	// for weapon raising
		bool				BreatherSound;

		FrameNumber			PickupMessageTime;
		FrameNumber			RespawnTime;		// can respawn when time > this
	} Timers;

	struct SClientGrenadeData
	{
		bool			BlewUp;
		bool			Thrown;
		FrameNumber	Time;
	} Grenade;

	struct SClientFlood
	{
		FrameNumber		LockTill; // locked from talking
		FrameNumber		When[10]; // when messages were said
		uint8			WhenHead; // head pointer for when said
	} Flood;

	struct SClientChase
	{
		CPlayerEntity	*Target;
		uint8			Mode;
	} Chase;

#if CLEANCTF_ENABLED
//ZOID
	struct SCTFGrapple
	{
		class CGrappleEntity	*Entity;
		EGrappleState			State;
		FrameNumber				ReleaseTime;
	} Grapple;
//ZOID
#endif

	struct SClientTech
	{
		// Tech-specific fields
		FrameNumber	RegenTime;

		// Global fields
		FrameNumber	SoundTime;
		FrameNumber	LastTechMessage;
	} Tech;

	CClient (SServerClient *client);

	sint32			&GetPing ();
	void			Clear ();
};

/**
\typedef	uint8 ENoiseType

\brief	Defines an alias representing type of a player noise.
**/
typedef uint8 ENoiseType;

/**
\enum	

\brief	Values that represent a type of player noise. 
**/
enum
{
	PNOISE_SELF,
	PNOISE_WEAPON,
	PNOISE_IMPACT
};

// Players don't think or have (game) controlled physics.
// PhysicsEntity inherited for velocity.
class CPlayerEntity : public IHurtableEntity, public IPhysicsEntity
{
public:
	enum
	{
		FRAME_stand01,
		FRAME_stand02,
		FRAME_stand03,
		FRAME_stand04,
		FRAME_stand05,
		FRAME_stand06,
		FRAME_stand07,
		FRAME_stand08,
		FRAME_stand09,
		FRAME_stand10,
		FRAME_stand11,
		FRAME_stand12,
		FRAME_stand13,
		FRAME_stand14,
		FRAME_stand15,
		FRAME_stand16,
		FRAME_stand17,
		FRAME_stand18,
		FRAME_stand19,
		FRAME_stand20,
		FRAME_stand21,
		FRAME_stand22,
		FRAME_stand23,
		FRAME_stand24,
		FRAME_stand25,
		FRAME_stand26,
		FRAME_stand27,
		FRAME_stand28,
		FRAME_stand29,
		FRAME_stand30,
		FRAME_stand31,
		FRAME_stand32,
		FRAME_stand33,
		FRAME_stand34,
		FRAME_stand35,
		FRAME_stand36,
		FRAME_stand37,
		FRAME_stand38,
		FRAME_stand39,
		FRAME_stand40,
		FRAME_run1,
		FRAME_run2,
		FRAME_run3,
		FRAME_run4,
		FRAME_run5,
		FRAME_run6,
		FRAME_attack1,
		FRAME_attack2,
		FRAME_attack3,
		FRAME_attack4,
		FRAME_attack5,
		FRAME_attack6,
		FRAME_attack7,
		FRAME_attack8,
		FRAME_pain101,
		FRAME_pain102,
		FRAME_pain103,
		FRAME_pain104,
		FRAME_pain201,
		FRAME_pain202,
		FRAME_pain203,
		FRAME_pain204,
		FRAME_pain301,
		FRAME_pain302,
		FRAME_pain303,
		FRAME_pain304,
		FRAME_jump1,
		FRAME_jump2,
		FRAME_jump3,
		FRAME_jump4,
		FRAME_jump5,
		FRAME_jump6,
		FRAME_flip01,
		FRAME_flip02,
		FRAME_flip03,
		FRAME_flip04,
		FRAME_flip05,
		FRAME_flip06,
		FRAME_flip07,
		FRAME_flip08,
		FRAME_flip09,
		FRAME_flip10,
		FRAME_flip11,
		FRAME_flip12,
		FRAME_salute01,
		FRAME_salute02,
		FRAME_salute03,
		FRAME_salute04,
		FRAME_salute05,
		FRAME_salute06,
		FRAME_salute07,
		FRAME_salute08,
		FRAME_salute09,
		FRAME_salute10,
		FRAME_salute11,
		FRAME_taunt01,
		FRAME_taunt02,
		FRAME_taunt03,
		FRAME_taunt04,
		FRAME_taunt05,
		FRAME_taunt06,
		FRAME_taunt07,
		FRAME_taunt08,
		FRAME_taunt09,
		FRAME_taunt10,
		FRAME_taunt11,
		FRAME_taunt12,
		FRAME_taunt13,
		FRAME_taunt14,
		FRAME_taunt15,
		FRAME_taunt16,
		FRAME_taunt17,
		FRAME_wave01,
		FRAME_wave02,
		FRAME_wave03,
		FRAME_wave04,
		FRAME_wave05,
		FRAME_wave06,
		FRAME_wave07,
		FRAME_wave08,
		FRAME_wave09,
		FRAME_wave10,
		FRAME_wave11,
		FRAME_point01,
		FRAME_point02,
		FRAME_point03,
		FRAME_point04,
		FRAME_point05,
		FRAME_point06,
		FRAME_point07,
		FRAME_point08,
		FRAME_point09,
		FRAME_point10,
		FRAME_point11,
		FRAME_point12,
		FRAME_crstnd01,
		FRAME_crstnd02,
		FRAME_crstnd03,
		FRAME_crstnd04,
		FRAME_crstnd05,
		FRAME_crstnd06,
		FRAME_crstnd07,
		FRAME_crstnd08,
		FRAME_crstnd09,
		FRAME_crstnd10,
		FRAME_crstnd11,
		FRAME_crstnd12,
		FRAME_crstnd13,
		FRAME_crstnd14,
		FRAME_crstnd15,
		FRAME_crstnd16,
		FRAME_crstnd17,
		FRAME_crstnd18,
		FRAME_crstnd19,
		FRAME_crwalk1,
		FRAME_crwalk2,
		FRAME_crwalk3,
		FRAME_crwalk4,
		FRAME_crwalk5,
		FRAME_crwalk6,
		FRAME_crattak1,
		FRAME_crattak2,
		FRAME_crattak3,
		FRAME_crattak4,
		FRAME_crattak5,
		FRAME_crattak6,
		FRAME_crattak7,
		FRAME_crattak8,
		FRAME_crattak9,
		FRAME_crpain1,
		FRAME_crpain2,
		FRAME_crpain3,
		FRAME_crpain4,
		FRAME_crdeath1,
		FRAME_crdeath2,
		FRAME_crdeath3,
		FRAME_crdeath4,
		FRAME_crdeath5,
		FRAME_death101,
		FRAME_death102,
		FRAME_death103,
		FRAME_death104,
		FRAME_death105,
		FRAME_death106,
		FRAME_death201,
		FRAME_death202,
		FRAME_death203,
		FRAME_death204,
		FRAME_death205,
		FRAME_death206,
		FRAME_death301,
		FRAME_death302,
		FRAME_death303,
		FRAME_death304,
		FRAME_death305,
		FRAME_death306,
		FRAME_death307,
		FRAME_death308,
	};

	CClient					Client;
	bool					NoClip;
	bool					TossPhysics;
	FrameNumber				FlySoundDebounceTime;
	FrameNumber				DamageDebounceTime;
	FrameNumber				AirFinished;
	FrameNumber				NextDrownTime;
	sint32					NextDrownDamage;
	FrameNumber				PainDebounceTime;

	CPlayerEntity (sint32 Index);
	~CPlayerEntity ();

	void SaveFields (CFile &File)
	{
		IHurtableEntity::SaveFields (File);
		IPhysicsEntity::SaveFields (File);
	
		// Write the player data first
		File.Write<bool> (NoClip);
		File.Write<bool> (TossPhysics);
		File.Write<FrameNumber> (FlySoundDebounceTime);
		File.Write<FrameNumber> (DamageDebounceTime);
		File.Write<FrameNumber> (AirFinished);
		File.Write<FrameNumber> (NextDrownTime);
		File.Write<sint32> (NextDrownDamage);
		File.Write<FrameNumber> (PainDebounceTime);

		// Write client data
		//Client.Write (File);
	}

	void LoadFields (CFile &File)
	{
		IHurtableEntity::LoadFields (File);
		IPhysicsEntity::LoadFields (File);
	
		// Read the player data first
		NoClip = File.Read<bool> ();
		TossPhysics = File.Read<bool> ();
		FlySoundDebounceTime = File.Read<FrameNumber> ();
		DamageDebounceTime = File.Read<FrameNumber> ();
		AirFinished = File.Read<FrameNumber> ();
		NextDrownTime = File.Read<FrameNumber> ();
		NextDrownDamage = File.Read<sint32> ();
		PainDebounceTime = File.Read<FrameNumber> ();

		// Read client data
		//Client.Load (File);
	}

	String ClientTeam ();
	bool OnSameTeamAs (CPlayerEntity *Player);

	/**
	\fn	sint32 CheckPowerArmor (vec3f &Point, vec3f &Normal, sint32 Damage, EDamageFlags DamageFlags)
	
	\brief	Checks power armor and makes adjustments based on that. 
	
	\author	Paril
	\date	13/06/2010
	
	\param [in,out]	Point	The point. 
	\param [in,out]	Normal	The normal. 
	\param	Damage			The damage. 
	\param	DamageFlags		The damage flags. 
	
	\return	The amount of damage saved from the power armor. 
	**/
	sint32 CheckPowerArmor (vec3f &Point, vec3f &Normal, sint32 Damage, EDamageFlags DamageFlags);

	/**
	\fn	virtual void TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker, vec3f Dir,
		vec3f Point, vec3f Normal, sint32 Damage, sint32 KnockBack, EDamageFlags DamageFlags,
		EMeansOfDeath MeansOfDeath)
	
	\brief	Makes this entity take damage. 
	
	\author	Paril
	\date	13/06/2010
	
	\param [in,out]	Inflictor	If non-null, the inflictor. 
	\param [in,out]	Attacker	If non-null, the attacker. 
	\param	Dir					The dir. 
	\param	Point				The point. 
	\param	Normal				The normal. 
	\param	Damage				The damage. 
	\param	KnockBack			The knock back. 
	\param	DamageFlags			The damage flags. 
	\param	MeansOfDeath		The means of death. 
	**/
	void TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker,
					vec3f Dir, vec3f Point, vec3f Normal, sint32 Damage,
					sint32 KnockBack, EDamageFlags DamageFlags, EMeansOfDeath MeansOfDeath);

	bool			Run ();
	void			BeginServerFrame ();

	void			BeginDeathmatch ();
	void			Begin ();
	bool			Connect (const char *userinfo, CUserInfo &UserInfo);
	void			Disconnect ();
	void			Obituary (IBaseEntity *Attacker);

	void			SpectatorRespawn ();
	void			CopyToBodyQueue ();
	void			Respawn ();
	void			PutInServer ();
	void			InitPersistent ();
	void			InitItemMaxValues ();
	void			UserinfoChanged (String userinfo);
	void			FetchEntData ();

	// EndServerFrame-related functions
	inline float	CalcRoll (vec3f &forward, vec3f &right);
	inline void		DamageFeedback (vec3f &forward, vec3f &right);
	inline void		CalcViewOffset (vec3f &forward, vec3f &right, vec3f &up, float xyspeed);
	inline void		CalcGunOffset (vec3f &forward, vec3f &right, vec3f &up, float xyspeed);
	inline void		CalcBlend ();
	inline void		FallingDamage ();
	inline void		WorldEffects ();
	EPowerArmorType	PowerArmorType ();
	void			SetClientEffects ();
	inline void		SetClientEvent (float xyspeed);
	inline void		SetClientSound ();
	inline void		SetClientFrame (float xyspeed);
	void			SetStats ();
	void			SetSpectatorStats ();

#if CLEANCTF_ENABLED
	void			SetCTFStats ();
	void			CTFSetIDView ();
	void			CTFScoreboardMessage (bool reliable);
#endif

	bool			ApplyStrengthSound();
	bool			ApplyHaste();
	void			ApplyHasteSound();
	bool			HasRegeneration();

	void			DeadDropTech ();
	void			TossClientWeapon ();
	void			MoveToIntermission ();
	void			ClientThink (SUserCmd *ucmd);

	void			DeathmatchScoreboardMessage (bool reliable);
	void			EndServerFrame ();
	void			LookAtKiller (IBaseEntity *Inflictor, IBaseEntity *Attacker);
	void			ShowScores ();
	void			ShowHelp ();
	void			HelpComputer ();
	bool			CheckFlood ();

	void			InitResp ();
	static void		SaveClientData ();
	static void		BackupClientData ();
	static void		RestoreClientData ();
	void			SelectSpawnPoint (vec3f &origin, vec3f &angles);
	class CSpotBase	*SelectCoopSpawnPoint ();
#if ROGUE_FEATURES
	class CSpotBase	*SelectLavaCoopSpawnPoint ();
#endif

#if CLEANCTF_ENABLED
	void			CTFDeadDropFlag ();
	class CSpotBase	*SelectCTFSpawnPoint ();
	void			CTFAssignTeam ();
	void			CTFAssignSkin (CUserInfo &s);
	bool			CTFStart ();
#endif

	void			Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);

	// Printing routines
	void			PrintToClient (EGamePrintLevel printLevel, const char *fmt, ...);

	void			UpdateChaseCam();
	void			ChaseNext();
	void			ChasePrev();
	void			GetChaseTarget ();

	void			TossHead (sint32 Damage);

	void			P_ProjectSource (vec3f distance, anglef angles, vec3f &result);
	void			PlayerNoiseAt (vec3f Where, ENoiseType type);

	void			PushInDirection (vec3f vel, ESpawnflags flags);

#if ROGUE_FEATURES
	void			RemoveAttackingPainDaemons ();
#endif

	/**
	\fn	void CastTo (ECastFlags CastFlags)
	
	\brief	Perform a cast to this entity.
	
	\author	Paril
	\date	29/05/2010
	
	\param	CastFlags	The cast flags. 
	**/
	void			CastTo (ECastFlags CastFlags);

	/**
	\fn	void StuffText (const char *text)

	\attention	Be careful with this function - do not abuse it.
	
	\brief	Stuff text to player's command buffer. 
	
	\author	Paril
	\date	29/05/2010
	
	\param	text	The text. 
	**/
	void			StuffText (const char *text);

	bool			IsSpawned ();

	/**
	\fn	void TouchTriggers ()
	
	\brief	Cause all triggers that this entity is touching to activate.
	**/
	void		TouchTriggers ();

	IMPLEMENT_SAVE_HEADER(CPlayerEntity)
};

void ClientEndServerFrames ();

// Global player events
class PlayerEvents
{
public:

	class PlayerDeathEventArgs : public EventArgs
	{
	public:
		IBaseEntity *Attacker;
		IBaseEntity *Inflictor;

		PlayerDeathEventArgs(IBaseEntity *attacker, IBaseEntity *inflictor) :
		  Attacker(attacker),
		  Inflictor(inflictor)
		{
		}
	};


	typedef void (*PlayerEventHandler) (CPlayerEntity *Sender);
	typedef void (*PlayerDeathEventHandlerType) (CPlayerEntity *sender, PlayerDeathEventArgs args);
	typedef Event<PlayerEventHandler, CPlayerEntity *> PlayerEvent;
	typedef Event<PlayerDeathEventHandlerType, CPlayerEntity*, PlayerDeathEventArgs> PlayerDeathEvent;

	static PlayerEvent PlayerConnected;
	static PlayerEvent PlayerDisconnected;
	static PlayerEvent PlayerBeginServerFrame;
	static PlayerEvent PlayerEndServerFrame;
	static PlayerEvent PlayerThink;
	static PlayerDeathEvent PlayerDeath;
};

#else
FILE_WARNING
#endif
