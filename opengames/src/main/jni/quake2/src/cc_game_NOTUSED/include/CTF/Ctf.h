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

#if !defined(CC_GUARD_CTF_H) || !INCLUDE_GUARDS
#define CC_GUARD_CTF_H

/**
\fn	inline const char *CTF_VERSION()

\brief	The CTF base version.

\return	Current CTF version. 
**/
inline const char *CTF_VERSION() { return "1.09b"; }

/**
\typedef	sint8 ETeamIndex

\brief	Defines an alias representing index of a team.
**/
typedef sint8 ETeamIndex;

/**
\enum	

\brief	Values that represent teams. 
**/
enum
{
	CTF_TEAM1,
	CTF_TEAM2,

	CTF_NUM_TEAMS,

	CTF_NOTEAM = 2,

	// aliases
	CTF_RED = CTF_TEAM1,
	CTF_BLUE = CTF_TEAM2,
};

/**
\typedef	uint8 EGrappleState

\brief	Defines an alias representing state of the grappling hook.
**/
typedef uint8 EGrappleState;

/**
\enum	

\brief	Values that represent the state of a grappling hook. 
**/
enum
{
	CTF_GRAPPLE_STATE_FLY,
	CTF_GRAPPLE_STATE_PULL,
	CTF_GRAPPLE_STATE_HANG
};

/**
\typedef	uint8 EElectState

\brief	Defines an alias representing state of an election.
**/
typedef uint8 EElectState;

/**
\enum	

\brief	Values that represent the state of an election. 
**/
enum
{
	ELECT_NONE,
	ELECT_ADMIN,
	ELECT_MAP
};

/**
\class	CCTFGameLocals

\brief	CTF game data.

\author	Paril
\date	29/05/2010
**/
class CCTFGameLocals
{
public:
	CCTFGameLocals ()
	{
		Clear ();
	}

	sint32			Captures[CTF_NUM_TEAMS];
	sint32			TotalScore[CTF_NUM_TEAMS];		// these are only set when going into intermission!
	FrameNumber		LastFlagCaptureTime;
	ETeamIndex		LastCaptureTeam;

	EElectState		Election;			// election type
	CPlayerEntity	*ElectionTarget;	// for CTF.Admin election, who's being elected
	String			ElectionLevel;		// for map election, target level
	uint8			ElectionVotes;		// votes so far
	uint8			ElectionNeedVotes;	// votes needed
	FrameNumber		ElectionTimeEnd;	// remaining time until election times out
	String			ElectionMessage;	// election name

	void Clear ()
	{
		Captures[0] = Captures[1] = 0;
		TotalScore[0] = TotalScore[1] = 0;
		LastFlagCaptureTime = 0;
		LastCaptureTeam = 0;
		Election = 0;
		ElectionTarget = NULL;
		ElectionLevel.Clear();
		ElectionVotes = 0;
		ElectionNeedVotes = 0;
		ElectionTimeEnd = 0;
		ElectionMessage.Clear();
	}
};

const int CTF_CAPTURE_BONUS						= 15;	// what you get for capture
const int CTF_TEAM_BONUS						= 10;	// what your team gets for capture
const int CTF_RECOVERY_BONUS					= 1;	// what you get for recovery
const int CTF_FLAG_BONUS						= 0;	// what you get for picking up enemy flag
const int CTF_FRAG_CARRIER_BONUS				= 2;	// what you get for fragging enemy flag carrier
const int CTF_FLAG_RETURN_TIME					= 400;	// seconds until auto return

const int CTF_CARRIER_DANGER_PROTECT_BONUS		= 2;	// bonus for fraggin someone who has recently hurt your flag carrier
const int CTF_CARRIER_PROTECT_BONUS				= 1;	// bonus for fraggin someone while either you or your target are near your flag carrier
const int CTF_FLAG_DEFENSE_BONUS				= 1;	// bonus for fraggin someone while either you or your target are near your flag
const int CTF_RETURN_FLAG_ASSIST_BONUS			= 1;	// awarded for returning a flag that causes a capture to happen almost immediately
const int CTF_FRAG_CARRIER_ASSIST_BONUS			= 2;	// award for fragging a flag carrier if a capture happens almost immediately

const int CTF_TARGET_PROTECT_RADIUS				= 400;	// the radius around an object being defended where a target will be worth extra frags
const int CTF_ATTACKER_PROTECT_RADIUS			= 400;	// the radius around an object being defended where an Attacker will get extra frags when making kills

const int CTF_CARRIER_DANGER_PROTECT_TIMEOUT	= 80;
const int CTF_FRAG_CARRIER_ASSIST_TIMEOUT		= 100;
const int CTF_RETURN_FLAG_ASSIST_TIMEOUT		= 100;

const int CTF_AUTO_FLAG_RETURN_TIMEOUT			= 300;	// number of seconds before dropped flag auto-returns

const int CTF_GRAPPLE_SPEED						= 650; // speed of grapple in flight
const int CTF_GRAPPLE_PULL_SPEED				= 650;	// speed player is pulled at

/**
\fn	inline const char *CTFTeamSkin(ETeamIndex team)

\brief	Get CTF skin per team index.

\author	Paril
\date	29/05/2010

\param	team	The team index. 

\return	Team 1's skin if it fails, otherwise the proper skin. 
**/
inline const char *CTFTeamSkin(ETeamIndex team)
{
	switch (team)
	{
	case CTF_TEAM1:
		return "ctf_r";
	case CTF_TEAM2:
		return "ctf_b";
	default:
		return "ctf_r";
	}
}

/**
\fn	inline const char *CTFTeamName(ETeamIndex team)

\brief	Get CTF team name based on team index.

\author	Paril
\date	29/05/2010

\param	team	The team. 

\return	UNKNOWN if it fails, otherwise team name. 
**/
inline const char *CTFTeamName(ETeamIndex team)
{
	switch (team)
	{
	case CTF_TEAM1:
		return "RED";
	case CTF_TEAM2:
		return "BLUE";
	default:
		return "UNKNOWN";
	}
}

/**
\fn	inline ETeamIndex CTFOtherTeam(ETeamIndex team)

\brief	Gets CTF 'other' team index, opposite team of current.

\author	Paril
\date	29/05/2010

\param	team	The team. 

\return	-1 if it fails, otherwise proper team index. 
**/
inline ETeamIndex CTFOtherTeam(ETeamIndex team)
{
	switch (team)
	{
	case CTF_TEAM1:
		return CTF_TEAM2;
	case CTF_TEAM2:
		return CTF_TEAM1;
	default:
		return -1;
	}
}

/**
\fn	inline const char *CTFOtherTeamName(ETeamIndex team)

\brief	Gets CTF 'other' team name, opposite team of current.

\author	Paril
\date	29/05/2010

\param	team	The team. 

\return	UNKNOWN if it fails, otherwise team opposite.
**/
inline const char *CTFOtherTeamName(ETeamIndex team)
{
	return CTFTeamName(CTFOtherTeam(team));
}

/**
\fn	void CTFCalcScores ()

\brief	Tally's up all of the captures.
		Called when entering intermission.

\author	Paril
\date	29/05/2010
**/
void CTFCalcScores ();

/**
\fn	void CTFResetFlag (ETeamIndex Team)

\brief	Resets a team's flag back to the base.

\author	Paril
\date	29/05/2010

\param	Team	The team. 
**/
void CTFResetFlag (ETeamIndex Team);

/**
\fn	void CTFResetFlags()

\brief	Resets every team's flags. 

\author	Paril
\date	29/05/2010
**/
void CTFResetFlags();

/**
\fn	void CTFFragBonuses (CPlayerEntity *Target, CPlayerEntity *Attacker)

\brief	Calculate the bonuses for flag defense, flag carrier defense, etc. Note that bonuses are
		not cumaltive.  You get one, they are in importance order. 

\author	Paril
\date	29/05/2010

\param [in,out]	Target		If non-null, the target. 
\param [in,out]	Attacker	If non-null, the attacker. 
**/
void CTFFragBonuses (CPlayerEntity *Target, CPlayerEntity *Attacker);

/**
\fn	void CTFCheckHurtCarrier(CPlayerEntity *Target, CPlayerEntity *Attacker)

\brief	Check if the carrier was hurt.

\author	Paril
\date	29/05/2010

\param [in,out]	Target		If non-null, the target. 
\param [in,out]	Attacker	If non-null, the attacker. 
**/
void CTFCheckHurtCarrier(CPlayerEntity *Target, CPlayerEntity *Attacker);

/**
\fn	void CTFOpenJoinMenu(CPlayerEntity *Player)

\brief	Open join team menu.

\author	Paril
\date	29/05/2010

\param [in,out]	Player	If non-null, the player. 
**/
void CTFOpenJoinMenu(CPlayerEntity *Player);

/**
\class	CCTFSayTeamCommand

\brief	CTF say team command. 

\author	Paril
\date	29/05/2010
**/
class CCTFSayTeamCommand : public CGameCommandFunctor
{
	String OutMessage;

public:
	/**
	\fn	void FormatLocation ()

	\brief	Formats location string.
			Pushes what objects you are near.
	**/
	void FormatLocation();

	/**
	\fn	void FormatArmor ()

	\brief	Formats armor string.
			Pushes current armor values and types.
	**/
	void FormatArmor();

	/**
	\fn	void FormatHealth ()

	\brief	Formats health string.
			Pushes your health value.
	**/
	void FormatHealth();

	/**
	\fn	void FormatTech ()

	\brief	Formats tech string.
			Pushes which tech you currently have equipped.
	**/
	void FormatTech();

	/**
	\fn	void FormatWeapon ()

	\brief	Formats weapon string.
			Pushes your current weapon.
	**/
	void FormatWeapon();

	/**
	\fn	void FormatSight ()

	\brief	Formats the sight string.
			Pushes which players are visible to you.
	**/
	void FormatSight();

	void Execute ();
};

/**
\class	CCTFTeamCommand

\brief	CTF team command.
		Check or switch teams.

\author	Paril
\date	29/05/2010
**/
class CCTFTeamCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

/**
\class	CCTFIDCommand

\brief	CTF ID command.
		Toggles target identification.

\author	Paril
\date	29/05/2010
**/
class CCTFIDCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

/**
\class	CCTFVoteYesCommand

\brief	CTF vote yes command.
		Votes yes for elections.

\author	Paril
\date	29/05/2010
**/
class CCTFVoteYesCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

/**
\class	CCTFVoteNoCommand

\brief	CTF vote no command.
		Votes no for elections.

\author	Paril
\date	29/05/2010
**/
class CCTFVoteNoCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

/**
\class	CCTFAdminCommand

\brief	CTF admin command.
		Allows you to become admin and do admin stuff.

\author	Paril
\date	29/05/2010
**/
class CCTFAdminCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

/**
\class	CCTFWarpCommand

\brief	CTF warp command.
		Vote to warp to another level.

\author	Paril
\date	29/05/2010
**/
class CCTFWarpCommand : public CGameCommandFunctor
{
public:
	void Execute ();
};

/**
\class	CCTFBootCommand

\brief	CTF boot command.
		Vote to kick someone.

\author	Paril
\date	29/05/2010
**/
class CCTFBootCommand : public CPlayerListCommand
{
public:
	void Execute ();
};

/**
\class	CCTFObserverCommand

\brief	CTF observer command.
		Move to observer mode. 

\author	Paril
\date	29/05/2010
**/
class CCTFObserverCommand : public CPlayerListCommand
{
public:
	void Execute ();
};

/**
\class	CCTFPlayerListCommand

\brief	CTF player list command.
		Advanced playerlist command for CTF.

\author	Paril
\date	29/05/2010
**/
class CCTFPlayerListCommand : public CPlayerListCommand
{
public:
	void Execute ();
};

/**
\fn	bool CTFCheckRules()

\brief	Check CTF rules.

\return	true if level is finished, otherwise false. 
**/
bool CTFCheckRules();

/**
\fn	void CreateCTFStatusbar ()

\brief	Creates the CTF statusbar.
**/
void CreateCTFStatusbar ();

extern CCTFGameLocals ctfgame;

#else
FILE_WARNING
#endif
