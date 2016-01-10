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

#include "Local.h"

#if CLEANCTF_ENABLED

#include "Weapons/WeaponMain.h"

void EndDMLevel ();

CCTFGameLocals ctfgame;

/**
\fn	void CreateCTFStatusbar ()

\brief	Creates the CTF statusbar.
**/
void CreateCTFStatusbar ()
{
	CStatusBar CTFBar;

	CTFBar.AddPoint_Y (-24, true);

	CTFBar.AddVirtualPoint_X (0);
	CTFBar.AddHealthNum ();
	CTFBar.AddVirtualPoint_X (50);
	CTFBar.AddPicStat (STAT_HEALTH_ICON);

	CTFBar.AddIf (STAT_AMMO_ICON);
	CTFBar.AddVirtualPoint_X (100);
	CTFBar.AddAmmoNum ();
	CTFBar.AddVirtualPoint_X (150);
	CTFBar.AddPicStat (STAT_AMMO_ICON);
	CTFBar.AddEndIf ();

	CTFBar.AddIf (STAT_ARMOR);
	CTFBar.AddVirtualPoint_X (200);
	CTFBar.AddArmorNum ();
	CTFBar.AddVirtualPoint_X (250);
	CTFBar.AddPicStat (STAT_ARMOR_ICON);
	CTFBar.AddEndIf ();

	CTFBar.AddIf (STAT_SELECTED_ICON);
	CTFBar.AddVirtualPoint_X (296);
	CTFBar.AddPicStat (STAT_SELECTED_ICON);
	CTFBar.AddEndIf ();

	CTFBar.AddPoint_Y (-50, true);

	CTFBar.AddIf (STAT_PICKUP_ICON);
	CTFBar.AddVirtualPoint_X (0);
	CTFBar.AddPicStat (STAT_PICKUP_ICON);
	CTFBar.AddVirtualPoint_X (26);
	CTFBar.AddPoint_Y (-42, true);
	CTFBar.AddStatString (STAT_PICKUP_STRING);
	CTFBar.AddPoint_Y (-50, true);
	CTFBar.AddEndIf ();

	CTFBar.AddIf (STAT_TIMER_ICON);
	CTFBar.AddVirtualPoint_X (246);
	CTFBar.AddNumStat (STAT_TIMER, 2);
	CTFBar.AddVirtualPoint_X (296);
	CTFBar.AddPicStat (STAT_TIMER_ICON);
	CTFBar.AddEndIf ();

	CTFBar.AddIf (STAT_HELPICON);
	CTFBar.AddVirtualPoint_X (148);
	CTFBar.AddPicStat (STAT_HELPICON);
	CTFBar.AddEndIf ();

	CTFBar.AddPoint_X (-50, true);
	CTFBar.AddPoint_Y (2, false);
	CTFBar.AddNumStat (STAT_FRAGS, 2);

	CTFBar.AddPoint_Y (-129, true);

	CTFBar.AddIf (STAT_TECH);
		CTFBar.AddPoint_X (-26, true);
		CTFBar.AddPicStat (STAT_TECH);
	CTFBar.AddEndIf();

	CTFBar.AddPoint_Y (-102, true);

	CTFBar.AddIf (STAT_CTF_TEAM1_PIC);
		CTFBar.AddPoint_X (-26, true);
		CTFBar.AddPicStat (STAT_CTF_TEAM1_PIC);
	CTFBar.AddEndIf ();
	CTFBar.AddPoint_X (-62, true);
	CTFBar.AddNumStat (STAT_CTF_TEAM1_CAPS, 2);

	CTFBar.AddIf (STAT_CTF_JOINED_TEAM1_PIC);
		CTFBar.AddPoint_Y (-104, true);
		CTFBar.AddPoint_X (-28, true);
		CTFBar.AddPicStat (STAT_CTF_JOINED_TEAM1_PIC);
	CTFBar.AddEndIf ();

	CTFBar.AddPoint_Y (-75, true);

	CTFBar.AddIf (STAT_CTF_TEAM2_PIC);
		CTFBar.AddPoint_X (-26, true);
		CTFBar.AddPicStat (STAT_CTF_TEAM2_PIC);
	CTFBar.AddEndIf ();
	CTFBar.AddPoint_X (-62, true);
	CTFBar.AddNumStat (STAT_CTF_TEAM2_CAPS, 2);

	CTFBar.AddIf (STAT_CTF_JOINED_TEAM2_PIC);
		CTFBar.AddPoint_Y (-77, true);
		CTFBar.AddPoint_X (-28, true);
		CTFBar.AddPicStat (STAT_CTF_JOINED_TEAM2_PIC);
	CTFBar.AddEndIf ();

	CTFBar.AddIf (STAT_CTF_FLAG_PIC);
		CTFBar.AddPoint_Y (26, false);
		CTFBar.AddPoint_X (-24, true);
		CTFBar.AddPicStat (STAT_CTF_FLAG_PIC);
	CTFBar.AddEndIf();

	CTFBar.AddIf (STAT_CTF_ID_VIEW);
		CTFBar.AddVirtualPoint_X (0);
		CTFBar.AddPoint_Y (-58, true);
		CTFBar.AddString ("Viewing", false, false);
		CTFBar.AddVirtualPoint_X (64);
		CTFBar.AddStatString (STAT_CTF_ID_VIEW);
	CTFBar.AddEndIf ();

	CTFBar.Send ();
}

/**
\fn	void CTFFragBonuses(CPlayerEntity *Target, CPlayerEntity *Attacker)

\brief	Calculate the bonuses for flag defense, flag carrier defense, etc. Note that bonuses are
		not cumaltive.  You get one, they are in importance order. 

\author	Paril
\date	29/05/2010

\param [in,out]	Target		If non-null, the target. 
\param [in,out]	Attacker	If non-null, the attacker. 
**/
void CTFFragBonuses(CPlayerEntity *Target, CPlayerEntity *Attacker)
{
	// no bonus for fragging yourself
	if (Target == Attacker)
		return;

	ETeamIndex otherteam = CTFOtherTeam(Target->Client.Respawn.CTF.Team);
	if (otherteam < 0)
		return; // whoever died isn't on a team

	// same team, if the flag at base, check if he has the enemy flag
	CFlag *flag_item = (Target->Client.Respawn.CTF.Team == CTF_TEAM1) ? NItems::RedFlag : NItems::BlueFlag;
	CFlag *enemy_flag_item = (flag_item == NItems::RedFlag) ? NItems::BlueFlag : NItems::RedFlag;

	// did the Attacker frag the flag carrier?
	if (Target->Client.Persistent.Inventory.Has(enemy_flag_item))
	{
		Attacker->Client.Respawn.CTF.LastFraggedCarrier = Level.Frame;
		Attacker->Client.Respawn.Score += CTF_FRAG_CARRIER_BONUS;
		Attacker->PrintToClient (PRINT_MEDIUM, "BONUS: %d points for fragging enemy flag carrier.\n",
			CTF_FRAG_CARRIER_BONUS);

		// the target had the flag, clear the hurt carrier
		// field on the other team
		for (uint8 i = 1; i <= Game.MaxClients; i++)
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>((Game.Entities + i)->Entity);
			if (Player->GetInUse() && Player->Client.Respawn.CTF.Team == otherteam)
				Player->Client.Respawn.CTF.LastHurtCarrier = 0;
		}
		return;
	}

	if (Target->Client.Respawn.CTF.LastHurtCarrier &&
		(Level.Frame - Target->Client.Respawn.CTF.LastHurtCarrier < CTF_CARRIER_DANGER_PROTECT_TIMEOUT) &&
		!Attacker->Client.Persistent.Inventory.Has(flag_item))
	{
		// Attacker is on the same team as the flag carrier and
		// fragged a guy who hurt our flag carrier
		Attacker->Client.Respawn.Score += CTF_CARRIER_DANGER_PROTECT_BONUS;
		BroadcastPrintf(PRINT_MEDIUM, "%s defends %s's flag carrier against an agressive enemy\n",
			Attacker->Client.Persistent.Name.CString(), 
			CTFTeamName(Attacker->Client.Respawn.CTF.Team));
		return;
	}

	// flag and flag carrier area defense bonuses
	// we have to find the flag and carrier entities

	// find the flag
	CFlagTransponder *AttackerTransponder = FindTransponder (Attacker->Client.Respawn.CTF.Team);

	if (!AttackerTransponder)
		return; // can't find Attacker's flag

	// find Attacker's team's flag carrier
	// ok we have the attackers flag and a pointer to the carrier

	// check to see if we are defending the base's flag
	if (((Target->State.GetOrigin() - AttackerTransponder->Base->State.GetOrigin()).Length() < CTF_TARGET_PROTECT_RADIUS ||
		(Attacker->State.GetOrigin() - AttackerTransponder->Base->State.GetOrigin()).Length() < CTF_TARGET_PROTECT_RADIUS ||
		AttackerTransponder->Base->CanSee(Target) || AttackerTransponder->Base->CanSee(Attacker)) &&
		Attacker->Client.Respawn.CTF.Team != Target->Client.Respawn.CTF.Team)
	{
		// we defended the base flag
		Attacker->Client.Respawn.Score += CTF_FLAG_DEFENSE_BONUS;
		if (AttackerTransponder->Base->GetSolid() == SOLID_NOT)
			BroadcastPrintf(PRINT_MEDIUM, "%s defends the %s base.\n",
				Attacker->Client.Persistent.Name.CString(), 
				CTFTeamName(Attacker->Client.Respawn.CTF.Team));
		else
			BroadcastPrintf(PRINT_MEDIUM, "%s defends the %s flag.\n",
				Attacker->Client.Persistent.Name.CString(), 
				CTFTeamName(Attacker->Client.Respawn.CTF.Team));
		return;
	}

	if (AttackerTransponder->Holder && AttackerTransponder->Holder != Attacker)
	{
		if ((Target->State.GetOrigin() - AttackerTransponder->Holder->State.GetOrigin()).Length() < CTF_ATTACKER_PROTECT_RADIUS ||
			(Attacker->State.GetOrigin() - AttackerTransponder->Holder->State.GetOrigin()).Length() < CTF_ATTACKER_PROTECT_RADIUS ||
			AttackerTransponder->Holder->CanSee(Target) || AttackerTransponder->Holder->CanSee(Attacker))
		{
			Attacker->Client.Respawn.Score += CTF_CARRIER_PROTECT_BONUS;
			BroadcastPrintf(PRINT_MEDIUM, "%s defends the %s's flag carrier.\n",
				Attacker->Client.Persistent.Name.CString(), 
				CTFTeamName(Attacker->Client.Respawn.CTF.Team));
			return;
		}
	}
}

/**
\fn	void CTFCheckHurtCarrier(CPlayerEntity *Target, CPlayerEntity *Attacker)

\brief	Check if the carrier was hurt.

\author	Paril
\date	29/05/2010

\param [in,out]	Target		If non-null, the target. 
\param [in,out]	Attacker	If non-null, the attacker. 
**/
void CTFCheckHurtCarrier(CPlayerEntity *Target, CPlayerEntity *Attacker)
{
	IBaseItem *flag_item;

	switch (Target->Client.Respawn.CTF.Team)
	{
	case CTF_TEAM1:
		flag_item = NItems::BlueFlag;
		break;
	case CTF_TEAM2:
		flag_item = NItems::RedFlag;
		break;
	default:
		flag_item = NULL;
		return;
	}

	if (Target->Client.Persistent.Inventory.Has(flag_item) &&
		Target->Client.Respawn.CTF.Team != Attacker->Client.Respawn.CTF.Team)
		Attacker->Client.Respawn.CTF.LastHurtCarrier = Level.Frame;
}


/*------------------------------------------------------------------------*/

/**
\fn	void CTFResetFlag (ETeamIndex Team)

\brief	Resets a team's flag back to the base.

\author	Paril
\date	29/05/2010

\param	Team	The team. 
**/
void CTFResetFlag (ETeamIndex Team)
{
	// Grab the transponder
	CFlagTransponder *Transponder = FindTransponder(Team);

	// If we're not at the base, free us and move us back
	if (Transponder->Flag != Transponder->Base)
	{
		Transponder->Flag->Free ();
		Transponder->Flag = Transponder->Base;
		Transponder->Location = CFlagTransponder::FLAG_AT_BASE;
		Transponder->Holder = NULL;
	}

	// Otherwise respawn
	Transponder->Base->GetSvFlags() &= ~SVF_NOCLIENT;
	Transponder->Base->GetSolid() = SOLID_TRIGGER;
	Transponder->Base->Link ();
	Transponder->Base->State.GetEvent() = EV_ITEM_RESPAWN;
}

/**
\fn	void CTFResetFlags()

\brief	Resets every team's flags. 

\author	Paril
\date	29/05/2010
**/
void CTFResetFlags()
{
	CTFResetFlag(CTF_TEAM1);
	CTFResetFlag(CTF_TEAM2);
}

/**
\fn	void CTFCalcScores()

\brief	Tally's up all of the captures.
		Called when entering intermission.

\author	Paril
\date	29/05/2010
**/
void CTFCalcScores()
{
	ctfgame.TotalScore[0] = ctfgame.TotalScore[1] = 0;

	for (uint8 i = 0; i < Game.MaxClients; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i+1].Entity);

		if (!Player->GetInUse())
			continue;

		switch (Player->Client.Respawn.CTF.Team)
		{
		case CTF_TEAM1:
		case CTF_TEAM2:
			ctfgame.TotalScore[Player->Client.Respawn.CTF.Team] += Player->Client.Respawn.Score;
			break;
		}
	}
}

void CCTFIDCommand::Execute ()
{
	Player->Client.Respawn.CTF.IDState = !Player->Client.Respawn.CTF.IDState;
	Player->PrintToClient (PRINT_HIGH, "%s player identification display\n", (Player->Client.Respawn.CTF.IDState) ? "Activating" : "Deactivating");
}

/*------------------------------------------------------------------------*/

void CCTFTeamCommand::Execute ()
{
	sint32 desired_team;

	String t = ArgGetConcatenatedString();
	if (!t[0])
	{
		Player->PrintToClient (PRINT_HIGH, "You are on the %s team.\n",
			CTFTeamName(Player->Client.Respawn.CTF.Team));
		return;
	}

	if (Q_stricmp (t.CString(), "red") == 0)
		desired_team = CTF_TEAM1;
	else if (Q_stricmp (t.CString(), "blue") == 0)
		desired_team = CTF_TEAM2;
	else
	{
		Player->PrintToClient (PRINT_HIGH, "Unknown team %s.\n", t.CString());
		return;
	}

	if (Player->Client.Respawn.CTF.Team == desired_team)
	{
		Player->PrintToClient (PRINT_HIGH, "You are already on the %s team.\n",
			CTFTeamName(Player->Client.Respawn.CTF.Team));
		return;
	}

////
	Player->GetSvFlags() = 0;
	Player->Flags &= ~FL_GODMODE;
	Player->Client.Respawn.CTF.Team = desired_team;
	Player->Client.Respawn.CTF.State = 0;
	Player->CTFAssignSkin(Player->Client.Persistent.UserInfo);

	if (Player->GetSolid() == SOLID_NOT)
	{ // Spectator
		Player->PutInServer();
		// add a teleportation effect
		Player->State.GetEvent() = EV_PLAYER_TELEPORT;
		// hold in place briefly
		Player->Client.PlayerState.GetPMove()->PMoveFlags = PMF_TIME_TELEPORT;
		Player->Client.PlayerState.GetPMove()->PMoveTime = 14;
		BroadcastPrintf(PRINT_HIGH, "%s joined the %s team.\n",
			Player->Client.Persistent.Name.CString(), CTFTeamName(desired_team));
		return;
	}

	Player->Health = 0;
	Player->Die (Player, Player, 100000, vec3fOrigin);
	// don't even bother waiting for death frames
	Player->IsDead = true;
	Player->Respawn ();

	Player->Client.Respawn.Score = 0;

	BroadcastPrintf(PRINT_HIGH, "%s changed to the %s team.\n",
		Player->Client.Persistent.Name.CString(), CTFTeamName(desired_team));
}

/*
======================================================================

SAY_TEAM

======================================================================
*/

const int HASHSIZE_CLASSNAMES = 256;

/**
\class	CLocName

\brief	Locational importance for items, for FormatLocation.

\author	Paril
\date	29/05/2010
**/
class CLocName
{
public:
	String			classname;
	uint32			hash;

	uint8			priority;

	CLocName (const char *name, uint8 priority) :
		classname(String(name)),
		priority(priority),
		hash(Com_HashGeneric (name, HASHSIZE_CLASSNAMES))
	{
	};
};

/**
\property	CLocName LocNames[] =

\brief	 This array is in 'importance order', it indicates what items are
		more important when reporting their names.
**/
CLocName LocNames[] = 
{
	CLocName(	"item_flag_team1",			1 ),
	CLocName(	"item_flag_team2",			1 ),
	CLocName(	"item_quad",				2 ), 
	CLocName(	"item_invulnerability",		2 ),
	CLocName(	"weapon_bfg",				3 ),
	CLocName(	"weapon_railgun",			4 ),
	CLocName(	"weapon_rocketlauncher",	4 ),
	CLocName(	"weapon_hyperblaster",		4 ),
	CLocName(	"weapon_chaingun",			4 ),
	CLocName(	"weapon_grenadelauncher",	4 ),
	CLocName(	"weapon_machinegun",		4 ),
	CLocName(	"weapon_supershotgun",		4 ),
	CLocName(	"weapon_shotgun",			4 ),
	CLocName(	"item_power_screen",		5 ),
	CLocName(	"item_power_shield",		5 ),
	CLocName(	"item_armor_body",			6 ),
	CLocName(	"item_armor_combat",		6 ),
	CLocName(	"item_armor_jacket",		6 ),
	CLocName(	"item_silencer",			7 ),
	CLocName(	"item_breather",			7 ),
	CLocName(	"item_enviro",				7 ),
	CLocName(	"item_adrenaline",			7 ),
	CLocName(	"item_bandolier",			8 ),
	CLocName(	"item_pack",				8 )
};

/**
\fn	void CCTFSayTeamCommand::FormatLocation ()

\brief	Formats location string.
		Pushes what objects you are near.
**/
void CCTFSayTeamCommand::FormatLocation ()
{
	IBaseEntity *hot = NULL;
	float hotdist = 999999, newdist;
	sint32 hotindex = 999;
	IBaseItem *item;
	sint32 nearteam = -1;
	bool hotsee = false;
	bool cansee;
	IBaseEntity *what = NULL;

	while ((what = FindRadius<EF_BASE>(what, Player->State.GetOrigin(), 1024, false)) != NULL)
	{
		// find what in loc_classnames
		uint32 hash = Com_HashGeneric (what->ClassName, HASHSIZE_CLASSNAMES);

		uint8 i;
		for (i = 0; i < ArrayCount(LocNames); i++)
		{
			if (hash == LocNames[i].hash && (what->ClassName == LocNames[i].classname))
				break;
		}

		if (i == ArrayCount(LocNames))
			continue;

		// something we can see get priority over something we can't
		cansee = what->CanSee(Player);
		if (cansee && !hotsee)
		{
			hotsee = true;
			hotindex = LocNames[i].priority;
			hot = what;

			hotdist = (what->State.GetOrigin() - Player->State.GetOrigin()).Length();
			continue;
		}
		// if we can't see this, but we have something we can see, skip it
		if (hotsee && !cansee)
			continue;
		if (hotsee && hotindex < LocNames[i].priority)
			continue;

		newdist = (what->State.GetOrigin() - Player->State.GetOrigin()).Length();
		if (newdist < hotdist || 
			(cansee && LocNames[i].priority < hotindex))
		{
			hot = what;
			hotdist = newdist;
			hotindex = i;
			hotsee = hot->CanSee(Player);
		}
	}

	if (!hot)
	{
		OutMessage += "nowhere";
		return;
	}

	// we now have the closest item
	// see if there's more than one in the map, if so
	// we need to determine what team is closest
	what = NULL;
	while ((what = CC_FindByClassName<IBaseEntity, EF_BASE> (what, hot->ClassName.CString())) != NULL)
	{
		if (what == hot)
			continue;
		// if we are here, there is more than one, find out if hot
		// is closer to red flag or blue flag

		CFlagTransponder *Transponders[2];
		Transponders[0] = FindTransponder(CTF_TEAM1);
		Transponders[1] = FindTransponder(CTF_TEAM2);

		if (Transponders[0] && Transponders[1])
		{
			hotdist = (hot->State.GetOrigin() - Transponders[0]->Base->State.GetOrigin()).Length();
			newdist = (hot->State.GetOrigin() - Transponders[1]->Base->State.GetOrigin()).Length();
			if (hotdist < newdist)
				nearteam = CTF_TEAM1;
			else if (hotdist > newdist)
				nearteam = CTF_TEAM2;
		}
		break;
	}

	if ((item = FindItemByClassname(hot->ClassName.CString())) == NULL)
	{
		OutMessage += "nowhere";
		return;
	}

	// in water?
	if (Player->WaterInfo.Level)
		OutMessage += "in the water ";

	// near or above
	vec3f v = Player->State.GetOrigin() - hot->State.GetOrigin();
	if (Abs(v.Z) > Abs(v.X) && Abs(v.Z) > Abs(v.Y))
		OutMessage += (v.Z > 0) ? "above " : "below ";
	else
		OutMessage += "near ";

	switch (nearteam)
	{
	case CTF_TEAM1:
		OutMessage += "the red ";
		break;
	case CTF_TEAM2:
		OutMessage += "the blue ";
		break;
	default:
		OutMessage += "the ";
		break;
	}

	OutMessage += item->Name;
}

/**
\fn	void CCTFSayTeamCommand::FormatArmor ()

\brief	Formats armor string.
		Pushes current armor values and types.
**/
void CCTFSayTeamCommand::FormatArmor ()
{
	IBaseItem			*Armor = Player->Client.Persistent.Armor;
	EPowerArmorType		PowerArmorType = Player->PowerArmorType ();

	if (PowerArmorType)
	{
		sint32 cells = Player->Client.Persistent.Inventory.Has(NItems::Cells);
		if (cells)
		{
			OutMessage += String::Format("%s with %i cells", ((PowerArmorType == POWER_ARMOR_SCREEN) ? "Power Screen" : "Power Shield"), cells);
			if (Armor)
				OutMessage += "and ";
		}
	}

	if (Armor)
		OutMessage += String::Format("%i units of %s", Player->Client.Persistent.Inventory.Has(Armor), Armor->Name);
	else if (!PowerArmorType)
		OutMessage += "no armor";
}

/**
\fn	void CCTFSayTeamCommand::FormatHealth ()

\brief	Formats health string.
		Pushes your health value.
**/
void CCTFSayTeamCommand::FormatHealth ()
{
	if (Player->Health <= 0)
		OutMessage += "dead";
	else
		OutMessage += String::Format("%i health", Player->Health);
}

/**
\fn	void CCTFSayTeamCommand::FormatTech ()

\brief	Formats tech string.
		Pushes which tech you currently have equipped.
**/
void CCTFSayTeamCommand::FormatTech ()
{
	// see if the player has a tech powerup
	if (Player->Client.Persistent.Tech)
		OutMessage += String::Format("the %s", Player->Client.Persistent.Tech->Name);
	else
		OutMessage += "no powerup";
}

/**
\fn	void CCTFSayTeamCommand::FormatWeapon ()

\brief	Formats weapon string.
		Pushes your current weapon.
**/
void CCTFSayTeamCommand::FormatWeapon ()
{
	if (Player->Client.Persistent.Weapon)
		OutMessage += Player->Client.Persistent.Weapon->Item->Name;
	else
		OutMessage += "none";
}

/**
\fn	void CCTFSayTeamCommand::FormatSight ()

\brief	Formats the sight string.
		Pushes which players are visible to you.
**/
void CCTFSayTeamCommand::FormatSight ()
{
	sint32 n = 0;
	static String nameStore;

	for (uint8 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *targ = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (!targ->GetInUse() || 
			targ == Player ||
			!targ->CanSee(Player))
			continue;

		if (nameStore.IsNullOrEmpty())
			nameStore = targ->Client.Persistent.Name;
		else
		{
			if (n)
				OutMessage += ", ";

			OutMessage += nameStore;
			nameStore = targ->Client.Persistent.Name;

			n++;
		}
	}

	if (n)
	{
		if (!nameStore.IsNullOrEmpty())
		{
			OutMessage += " and ";
			OutMessage += nameStore;
		}
	}
	else
		OutMessage += "no one";
}

bool CheckFlood(CPlayerEntity *Player);

/**
\class	CCTFSayPlayerCallback

\brief	CTF Say player callback.
		Prints the text to each player.

\author	Paril
\date	29/05/2010
**/
class CCTFSayPlayerCallback : public CForEachPlayerCallback
{
public:
	String			&Text;
	ETeamIndex		Team;

	CCTFSayPlayerCallback (String &Text, ETeamIndex Team) :
	  Text(Text),
	  Team(Team)
	{
	};

	CCTFSayPlayerCallback &operator= (CCTFSayPlayerCallback&) { return *this; }

	void Callback (CPlayerEntity *Player)
	{
		if (Player->Client.Respawn.CTF.Team == Team)
			Player->PrintToClient (PRINT_CHAT, "%s", Text.CString());
	}
};

void CCTFSayTeamCommand::Execute ()
{
	String msg = ArgGetConcatenatedString();
	
	if (Player->CheckFlood())
		return;

	OutMessage.Clear();

	if (msg[0] == '\"')
	{
		msg.Remove (0, 1);
		msg.Remove (msg.Count() - 1);
	}

	for (size_t pm = 0; pm < msg.Count(); pm++)
	{
		if (msg[pm] == '%')
		{
			switch (Q_tolower(msg[++pm]))
			{
			case 'l' :
				FormatLocation ();
				break;
			case 'a' :
				FormatArmor();
				break;
			case 'h' :
				FormatHealth();
				break;
			case 't' :
				FormatTech();
				break;
			case 'w' :
				FormatWeapon();
				break;
			case 'n' :
				FormatSight();
				break;
			default :
				OutMessage += msg[pm];
			}
		}
		else
			OutMessage += msg[pm];
	}

	msg = String::Format("(%s): %s", Player->Client.Persistent.Name.CString(), OutMessage.CString());
	CCTFSayPlayerCallback (msg, Player->Client.Respawn.CTF.Team);
}

/*QUAKED misc_ctf_banner (1 .5 0) (-4 -64 0) (4 64 248) TEAM2
The origin is the bottom of the banner.
The banner is 248 tall.
*/
enum
{
	SPAWNFLAG_BLUE = BIT(0)
};

class CMiscCTFBanner : public IMapEntity, public IThinkableEntity
{
public:
	CMiscCTFBanner () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity ()
	{
	};

	CMiscCTFBanner (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscCTFBanner)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);
	};

	void Think ()
	{
		State.GetFrame() = (State.GetFrame() + 1) % 16;
		NextThink = Level.Frame + ServerFramesPerSecond;
	};

	virtual void Spawn ()
	{
		GetSolid() = SOLID_NOT;
		State.GetModelIndex() = ModelIndex ("models/ctf/banner/tris.md2");
		if (SpawnFlags & SPAWNFLAG_BLUE) // team2
			State.GetSkinNum() = 1;

		State.GetFrame() = irandom(16);
		Link ();

		NextThink = Level.Frame + ServerFramesPerSecond;
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_ctf_banner", CMiscCTFBanner);

/*QUAKED misc_ctf_small_banner (1 .5 0) (-4 -32 0) (4 32 124) TEAM2
The origin is the bottom of the banner.
The banner is 124 tall.
*/
class CMiscCTFBannerSmall : public CMiscCTFBanner
{
public:
	CMiscCTFBannerSmall () :
	  IBaseEntity (),
	  CMiscCTFBanner ()
	{
	};

	CMiscCTFBannerSmall (sint32 Index) :
	  IBaseEntity (Index),
	  CMiscCTFBanner (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscCTFBannerSmall)

	void Spawn ()
	{
		GetSolid() = SOLID_NOT;
		State.GetModelIndex() = ModelIndex ("models/ctf/banner/small.md2");
		if (SpawnFlags & SPAWNFLAG_BLUE) // team2
			State.GetSkinNum() = 1;

		State.GetFrame() = irandom(16);
		Link ();

		NextThink = Level.Frame + ServerFramesPerSecond;
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_ctf_banner_small", CMiscCTFBannerSmall);

/**
\fn	bool CTFBeginElection(CPlayerEntity *Player, EElectState Type, String Message)

\brief	Begin an election. 

\author	Paril
\date	29/05/2010

\param [in,out]	Player	If non-null, the player. 
\param	Type			The type. 
\param	Message			The message. 

\return	true if it succeeds, false if it fails. 
**/
bool CTFBeginElection(CPlayerEntity *Player, EElectState Type, String Message)
{
	if (CvarList[CV_ELECT_PERCENTAGE].Integer() == 0)
	{
		Player->PrintToClient (PRINT_HIGH, "Elections are disabled, only an admin can process this action.\n");
		return false;
	}


	if (ctfgame.Election != ELECT_NONE)
	{
		Player->PrintToClient (PRINT_HIGH, "Election already in progress.\n");
		return false;
	}

	// clear votes
	uint8 count = 0;
	for (uint8 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *e = entity_cast<CPlayerEntity>((Game.Entities + i)->Entity);
		e->Client.Respawn.CTF.Voted = false;
		if (e->GetInUse())
			count++;
	}

	if (count < 2)
	{
		Player->PrintToClient (PRINT_HIGH, "Not enough players for election.\n");
		return false;
	}

	ctfgame.ElectionTarget = Player;
	ctfgame.Election = Type;
	ctfgame.ElectionVotes = 0;
	ctfgame.ElectionNeedVotes = (count * CvarList[CV_ELECT_PERCENTAGE].Integer()) / 100;
	ctfgame.ElectionTimeEnd = Level.Frame + 200; // twenty seconds for election
	ctfgame.ElectionMessage = Message;

	// tell everyone
	BroadcastPrintf(PRINT_CHAT, "%s\n", Message.CString());
	BroadcastPrintf(PRINT_HIGH, "Type YES or NO to vote on this request\n"
								"Votes: %d  Needed: %d  Time left: %ds\n",
								ctfgame.ElectionVotes, ctfgame.ElectionNeedVotes,
								(sint32)((ctfgame.ElectionTimeEnd - Level.Frame) / 10));

	return true;
}

/**
\fn	void CTFWinElection()

\brief	Called when an election has passed.
**/
void CTFWinElection()
{
	switch (ctfgame.Election)
	{
	case ELECT_ADMIN :
		ctfgame.ElectionTarget->Client.Respawn.CTF.Admin = true;
		BroadcastPrintf(PRINT_HIGH, "%s has become an admin.\n", ctfgame.ElectionTarget->Client.Persistent.Name.CString());
		ctfgame.ElectionTarget->PrintToClient (PRINT_HIGH, "Type 'admin' to access the adminstration menu.\n");
		break;

	case ELECT_MAP :
		BroadcastPrintf(PRINT_HIGH, "%s is warping to level %s.\n", 
			ctfgame.ElectionTarget->Client.Persistent.Name.CString(), ctfgame.ElectionLevel.CString());
		Level.ForceMap = ctfgame.ElectionLevel.CString();
		EndDMLevel();
		break;
	}
	ctfgame.Election = ELECT_NONE;
}

void CCTFVoteYesCommand::Execute ()
{
	if (ctfgame.Election == ELECT_NONE)
	{
		Player->PrintToClient (PRINT_HIGH, "No election is in progress.\n");
		return;
	}
	if (Player->Client.Respawn.CTF.Voted)
	{
		Player->PrintToClient (PRINT_HIGH, "You already CTF.Voted.\n");
		return;
	}
	if (ctfgame.ElectionTarget == Player)
	{
		Player->PrintToClient (PRINT_HIGH, "You can't vote for yourself.\n");
		return;
	}

	Player->Client.Respawn.CTF.Voted = true;

	ctfgame.ElectionVotes++;
	if (ctfgame.ElectionVotes == ctfgame.ElectionNeedVotes)
	{
		// the election has been won
		CTFWinElection();
		return;
	}
	BroadcastPrintf(PRINT_HIGH, "%s\n", ctfgame.ElectionMessage.CString());
	BroadcastPrintf(PRINT_CHAT, "Votes: %d  Needed: %d  Time left: %ds\n", ctfgame.ElectionVotes, ctfgame.ElectionNeedVotes,
		(sint32)((ctfgame.ElectionTimeEnd - Level.Frame)/10));
}

void CCTFVoteNoCommand::Execute ()
{
	if (ctfgame.Election == ELECT_NONE)
	{
		Player->PrintToClient (PRINT_HIGH, "No election is in progress.\n");
		return;
	}
	if (Player->Client.Respawn.CTF.Voted)
	{
		Player->PrintToClient (PRINT_HIGH, "You already CTF.Voted.\n");
		return;
	}
	if (ctfgame.ElectionTarget == Player)
	{
		Player->PrintToClient (PRINT_HIGH, "You can't vote for yourself.\n");
		return;
	}

	Player->Client.Respawn.CTF.Voted = true;

	BroadcastPrintf(PRINT_HIGH, "%s\n", ctfgame.ElectionMessage.CString());
	BroadcastPrintf(PRINT_CHAT, "Votes: %d  Needed: %d  Time left: %ds\n", ctfgame.ElectionVotes, ctfgame.ElectionNeedVotes,
		(sint32)((ctfgame.ElectionTimeEnd - Level.Frame)/10));
}

/*-----------------------------------------------------------------------*/

void CCTFObserverCommand::Execute ()
{
	// start as 'observer'
	if (Player->NoClip)
	{
		Player->PrintToClient (PRINT_HIGH, "You are already an observer.\n");
		return;
	}

	CGrapple::PlayerResetGrapple(Player);
	Player->CTFDeadDropFlag();
	Player->DeadDropTech();

	Player->NoClip = true;
	Player->GetSolid() = SOLID_NOT;
	Player->GetSvFlags() |= SVF_NOCLIENT;
	Player->Client.Respawn.CTF.Team = CTF_NOTEAM;
	Player->Client.PlayerState.GetGunIndex () = 0;
	Player->Client.Respawn.Score = 0;
	Player->Link();
	CTFOpenJoinMenu(Player);
}

/**
\fn	bool CTFCheckRules()

\brief	Check CTF rules.

\return	true if level is finished, otherwise false. 
**/
bool CTFCheckRules()
{
	if (ctfgame.Election != ELECT_NONE && ctfgame.ElectionTimeEnd <= Level.Frame)
	{
		BroadcastPrint (PRINT_CHAT, "Election timed out and has been cancelled.\n");
		ctfgame.Election = ELECT_NONE;
	}

	if (CvarList[CV_CAPTURE_LIMIT].Integer() && 
		(ctfgame.Captures[CTF_TEAM1] >= CvarList[CV_CAPTURE_LIMIT].Integer() ||
		ctfgame.Captures[CTF_TEAM2] >= CvarList[CV_CAPTURE_LIMIT].Integer())) {
		BroadcastPrint (PRINT_HIGH, "Capturelimit hit.\n");
		return true;
	}
	return false;
}

void CCTFPlayerListCommand::Execute ()
{
	if (!(Game.GameMode & GAME_CTF))
	{
		CPlayerListCommand::Execute ();
		return;
	}

	CTempMemoryBlock	tempStr = CTempHunkSystem::Allocator.GetBlock(80),
						text = CTempHunkSystem::Allocator.GetBlock(1400);

	// number, name, connect time, ping, Score, CTF.Admin
	*text.GetBuffer<char>() = 0;
	for (uint8 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *e2 = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (!e2->GetInUse())
			continue;

		Q_snprintfz(tempStr.GetBuffer<char>(), tempStr.GetSize(), "%3d %-16.16s %02d:%02d %4d %3d%s\n",
			i + 1,
			e2->Client.Persistent.Name.CString(),
			(Level.Frame - e2->Client.Respawn.EnterFrame) / 600,
			((Level.Frame - e2->Client.Respawn.EnterFrame) % 600)/10,
			e2->Client.GetPing(),
			e2->Client.Respawn.Score,
			e2->Client.Respawn.CTF.Admin ? " (admin)" : "");
		if (strlen(text.GetBuffer<char>()) + strlen(tempStr.GetBuffer<char>()) > text.GetSize() - 50)
		{
			Q_snprintfz(text.GetBuffer<char>()+strlen(text.GetBuffer<char>()), text.GetSize(), "And more...\n");
			Player->PrintToClient (PRINT_HIGH, "%s", text.GetBuffer<char>());
			return;
		}
		Q_strcatz(text.GetBuffer<char>(), tempStr.GetBuffer<char>(), text.GetSize());
	}
	Player->PrintToClient (PRINT_HIGH, "%s", text.GetBuffer<char>());
}

bool CharIsAnyOf (char Character, const char *Values)
{
	for (const char *c = Values; *c; ++c)
	{
		if (Character == *c)
			return true;
	}

	return false;
}

std::vector<String> Tokenize (String string, const char *Separators, const bool lower = false)
{
	std::vector<String> strings;
	size_t start = 0;

	for (size_t i = 0; i < string.Count(); ++i)
	{
		if (string[i] == 0 || CharIsAnyOf(string[i], Separators))
		{
			String str = string.Substring(start, (i - start));

			if (lower)
				str.ToLower();

			strings.push_back(str);
			start = i + 1;
		}
	}

	return strings;
}

void CCTFWarpCommand::Execute ()
{
	static std::vector<String> maps;
	static bool initialized = false;
	static const char *seps = " \t\n\r";

	if (ArgCount() < 2)
	{
		Player->PrintToClient (PRINT_HIGH, "Where do you want to warp to?\nAvailable levels are: %s\n", CvarList[CV_WARP_LIST].StringValue());
		return;
	}

	if (!initialized || CvarList[CV_WARP_LIST].Modified())
		maps = Tokenize(String(CvarList[CV_WARP_LIST].StringValue()), seps, true);

	std::vector<String>::iterator it = maps.begin();
	for (; it != maps.end(); ++it)
	{
		if (ArgGets(1).ToLower() == (*it))
			break;
	}

	if (it == maps.end())
	{
		Player->PrintToClient (PRINT_HIGH, "Unknown CTF Level.\nAvailable levels are: %s\n", CvarList[CV_WARP_LIST].StringValue());
		return;
	}

	if (Player->Client.Respawn.CTF.Admin)
	{
		BroadcastPrintf(PRINT_HIGH, "%s is warping to level %s.\n", 
			Player->Client.Persistent.Name.CString(), ArgGets(1).CString());
		Level.ForceMap = ArgGets(1);
		EndDMLevel();
		return;
	}

	if (CTFBeginElection(Player, ELECT_MAP, Player->Client.Persistent.Name + " has requested warping to level " + ArgGets(1)))
		ctfgame.ElectionLevel = ArgGets(1);
}

void CCTFBootCommand::Execute ()
{
	sint32 i;
	CTempMemoryBlock text = CTempHunkSystem::Allocator.GetBlock(80);

	if (!Player->Client.Respawn.CTF.Admin)
	{
		Player->PrintToClient (PRINT_HIGH, "You are not an admin.\n");
		return;
	}

	if (ArgCount() < 2)
	{
		Player->PrintToClient (PRINT_HIGH, "Who do you want to kick?\n");
		return;
	}

	if (ArgGets(1)[0] < '0' && ArgGets(1)[0] > '9')
	{
		Player->PrintToClient (PRINT_HIGH, "Specify the player number to kick.\n");
		return;
	}

	i = ArgGeti(1);
	if (i < 1 || i > Game.MaxClients)
	{
		Player->PrintToClient (PRINT_HIGH, "Invalid player number.\n");
		return;
	}

	CPlayerEntity *targ = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
	if (!targ->GetInUse())
	{
		Player->PrintToClient (PRINT_HIGH, "That player number is not connected.\n");
		return;
	}

	Q_snprintfz(text.GetBuffer<char>(), text.GetSize(), "kick %d\n", i - 1);
	gi.AddCommandString(text.GetBuffer<char>());
}
#endif

