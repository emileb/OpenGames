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
// cc_playerentity.cpp
// Player entity
//

#include "Local.h"
#include "Menu/Menu.h"
#include "Player/Ban.h"
#include "Player/BodyQueue.h"
#include "Weapons/WeaponMain.h"

#if 0
#include "Player/PlayerMove.h"
#endif

// Events declaration
PlayerEvents::PlayerEvent PlayerEvents::PlayerConnected;
PlayerEvents::PlayerEvent PlayerEvents::PlayerDisconnected;
PlayerEvents::PlayerEvent PlayerEvents::PlayerBeginServerFrame;
PlayerEvents::PlayerEvent PlayerEvents::PlayerEndServerFrame;
PlayerEvents::PlayerEvent PlayerEvents::PlayerThink;
PlayerEvents::PlayerDeathEvent PlayerEvents::PlayerDeath;

IMPLEMENT_SAVE_SOURCE(CPlayerNoise);

CPersistentData::CPersistentData ()
{
	Clear ();
}

CPersistentData::~CPersistentData()
{
}

void CPersistentData::Save (CFile &File)
{
	UserInfo.Save (File);
	File.Write (Name);

	File.Write<IPAddress> (IP);
	File.Write<EHandedness> (Hand);

	File.Write<sint32> (Health);
	File.Write<sint32> (MaxHealth);
	File.Write<sint32> (SavedFlags);

	Inventory.Save (File);
	File.WriteArray<sint32> (MaxAmmoValues, AMMOTAG_MAX);

	SaveWeapon (File, Weapon);
	SaveWeapon (File, LastWeapon);

	File.Write<sint32> ((Armor) ? Armor->GetIndex() : -1);

	File.Write<sint32> (PowerCubeCount);
	File.Write<sint32> (Score);
	File.Write<uint8> (GameHelpChanged);
	File.Write<uint8> (HelpChanged);
	File.Write<bool> (Spectator);
	File.Write<colorf> (ViewBlend);
}

void CPersistentData::Load (CFile &File)
{
	UserInfo.Load (File);
	Name = File.ReadString ();

	IP = File.Read<IPAddress> ();
	Hand = File.Read<EHandedness> ();

	Health = File.Read<sint32> ();
	MaxHealth = File.Read<sint32> ();
	SavedFlags = File.Read<sint32> ();

	Inventory.Load (File);
	File.ReadArray (MaxAmmoValues, AMMOTAG_MAX);

	LoadWeapon (File, &Weapon);
	LoadWeapon (File, &LastWeapon);

	sint32 Index = File.Read<sint32> ();

	if (Index != -1)
		Armor = dynamic_cast<CArmor*>(GetItemByIndex(Index));

	PowerCubeCount = File.Read<sint32> ();
	Score = File.Read<sint32> ();
	GameHelpChanged = File.Read<uint8> ();
	HelpChanged = File.Read<uint8> ();
	Spectator = File.Read<bool> ();
	ViewBlend = File.Read<colorf> ();
}

void CPersistentData::Clear ()
{
	UserInfo.Clear();
	Name.Clear();
	// Paril: DO NOT CLEAR IP
	//Mem_Zero (&IP, sizeof(IP));
	Hand = 0;
	State = 0;
	Health = 0;
	MaxHealth = 0;
	SavedFlags = 0;
	Inventory.Clear();
	Weapon = NULL;
	LastWeapon  = NULL;
	Armor = NULL;
#if CLEANCTF_ENABLED
	Flag = NULL;
#endif
	Tech = NULL;
	PowerCubeCount = 0;
	Score = 0;
	GameHelpChanged = 0;
	HelpChanged = 0;
	Spectator = false;
	ViewBlend.Set (0, 0, 0, 0);

	Mem_Zero (&MaxAmmoValues, sizeof(MaxAmmoValues));
}

CPlayerState::CPlayerState (SPlayerState *playerState) :
playerState(playerState)
{
};

void			CPlayerState::Initialize (SPlayerState *playerState)
{
	this->playerState = playerState;
};

SPMoveState	*CPlayerState::GetPMove ()
{
	return &playerState->PMove;
} // Direct pointer
void			CPlayerState::SetPMove (SPMoveState *newState)
{
	memcpy (&playerState->PMove, newState, sizeof(SPMoveState));
}

// Unless, of course, you use the vec3f class :D
vec3f			&CPlayerState::GetViewAngles ()
{
	return playerState->ViewAngles;
}
vec3f			&CPlayerState::GetViewOffset ()
{
	return playerState->ViewOffset;
}

vec3f			&CPlayerState::GetGunAngles ()
{
	return playerState->GunAngles;
}
vec3f			&CPlayerState::GetGunOffset ()
{
	return playerState->GunOffset;
}
vec3f			&CPlayerState::GetKickAngles ()
{
	return playerState->KickAngles;
}

MediaIndex		&CPlayerState::GetGunIndex ()
{
	return (MediaIndex&)playerState->GunIndex;
}

sint32		&CPlayerState::GetGunFrame ()
{
	return playerState->GunFrame;
}

colorf			&CPlayerState::GetViewBlend ()
{
	return playerState->ViewBlend;
}
colorb			CPlayerState::GetViewBlendB ()
{
	// Convert it to a color uint8
	return playerState->ViewBlend;
} // Name had to be different

float			&CPlayerState::GetFov ()
{
	return playerState->Fov;
}

ERenderDefFlags	&CPlayerState::GetRdFlags ()
{
	return playerState->RenderDefFlags;
}

sint16			&CPlayerState::GetStat (uint8 index)
{
	if (CC_ASSERT_EXPR (!(index < 0 || index > 32), "GetStat() index out of bounds"))
		return playerState->Stats[0];

	return playerState->Stats[index];
}

void CPlayerState::CopyStats (EStatIndex *Stats)
{
	memcpy (playerState->Stats, Stats, sizeof(playerState->Stats));
}

EStatIndex *CPlayerState::GetStats ()
{
	return playerState->Stats;
}

void			CPlayerState::Clear ()
{
	Mem_Zero (playerState, sizeof(&playerState));
}

CClient::CClient (SServerClient *client) :
client(client),
PlayerState(&client->PlayerState)
{
	Clear ();
};

void CClient::Write (CFile &File)
{
	File.Write<vec3f> (KickAngles);
	File.Write<vec3f> (KickOrigin);
	File.Write<vec3f> (ViewAngle);
	File.Write<vec3f> (DamageFrom);
	File.Write<colorf> (DamageBlend);
	File.Write<vec3f> (OldViewAngles);
	File.Write<vec3f> (OldVelocity);
	File.Write<float> (ViewDamage[0]);
	File.Write<float> (ViewDamage[1]);
	File.Write<FrameNumber> (ViewDamageTime);
	File.Write<float> (KillerYaw);
	File.Write<SPMoveState> (OldPMove);
	File.Write<ELayoutFlags> (LayoutFlags);
	File.WriteArray<sint32> (DamageValues, DT_MAX);
	File.Write<EButtons> (Buttons);
	File.Write<EButtons> (LatchedButtons);
	File.Write<EWeaponState> (WeaponState);
	File.Write<FrameNumber> (FallTime);
	File.Write<float> (FallValue);
	File.Write<float> (BonusAlpha);
	File.Write<float> (BobTime);
	File.Write<uint8> (PowerArmorTime);
	File.Write<bool> (PowerArmorEnabled);
	WriteIndex (File, WeaponSound, INDEX_SOUND);

	File.Write<SClientAnimation> (Anim);
	File.Write<SClientTimers> (Timers);
	File.Write<SClientGrenadeData> (Grenade);
	File.Write<SClientFlood> (Flood);

	SaveWeapon (File, NewWeapon);

#if ROGUE_FEATURES
	File.Write<sint32> ((OwnedSphere != NULL) ? OwnedSphere->State.GetNumber() : -1);
#endif

	Persistent.Save (File);
	Respawn.Save (File);
}

void CClient::Load (CFile &File)
{
	KickAngles = File.Read<vec3f> ();
	KickOrigin = File.Read<vec3f> ();
	ViewAngle = File.Read<vec3f> ();
	DamageFrom = File.Read<vec3f> ();
	DamageBlend = File.Read<colorf> ();
	OldViewAngles = File.Read<vec3f> ();
	OldVelocity = File.Read<vec3f> ();
	ViewDamage[0] = File.Read<float> ();
	ViewDamage[1] = File.Read<float> ();
	ViewDamageTime = File.Read<FrameNumber> ();
	KillerYaw = File.Read<float> ();
	OldPMove = File.Read<SPMoveState> ();
	LayoutFlags = File.Read<ELayoutFlags> ();
	File.ReadArray<sint32> (DamageValues, DT_MAX);
	Buttons = File.Read<EButtons> ();
	LatchedButtons = File.Read<EButtons> ();
	WeaponState = File.Read<EWeaponState> ();
	FallTime = File.Read<FrameNumber> ();
	FallValue = File.Read<float> ();
	BonusAlpha = File.Read<float> ();
	BobTime = File.Read<float> ();
	PowerArmorTime = File.Read<uint8> ();
	PowerArmorEnabled = File.Read<bool> ();
	ReadIndex (File, WeaponSound, INDEX_SOUND);

	Anim = File.Read<SClientAnimation> ();
	Timers = File.Read<SClientTimers> ();
	Grenade = File.Read<SClientGrenadeData> ();
	Flood = File.Read<SClientFlood> ();

	LoadWeapon (File, &NewWeapon);

#if ROGUE_FEATURES
	sint32 Index = File.Read<sint32> ();
	if (Index != -1)
		OwnedSphere = entity_cast<CRogueBaseSphere>(Game.Entities[Index].Entity);
#endif

	Persistent.Load (File);
	Respawn.Load (File);
}

void CClient::WriteClientStructure (CFile &File)
{
	File.Write (client, sizeof(*client));
}

void CClient::ReadClientStructure (CFile &File, sint32 index)
{
	SServerClient *ptr = &Game.Clients[index];
	File.Read (ptr, sizeof(*ptr));
}

void CClient::RepositionClient (SServerClient *client)
{
	this->client = client;
	PlayerState.playerState = &client->PlayerState;
}

sint32 &CClient::GetPing ()
{
	return client->Ping;
}

extern bool ReadingGame;
void CClient::Clear ()
{
	if (!ReadingGame)
	{
		Mem_Zero (client, sizeof(*client));

		PlayerState.Initialize (&client->PlayerState);
		KickAngles.Clear ();
		KickOrigin.Clear ();
		ViewAngle.Clear ();
		DamageFrom.Clear ();
		DamageBlend.Set (0,0,0,0);
		mynoise = NULL;
		mynoise2 = NULL;
		OldViewAngles.Clear ();
		OldVelocity.Clear ();
		ViewDamage[0] = ViewDamage[1] = 0;
		ViewDamageTime = 0;
		KillerYaw = 0;
		Persistent.Clear ();
		Respawn.Clear ();
		Mem_Zero (&OldPMove, sizeof(OldPMove));
		LayoutFlags = 0;
		Buttons = 0;
		LatchedButtons = 0;
		NewWeapon = NULL;
		WeaponState = 0;
		FallTime = 0;
		FallValue = 0;
		BonusAlpha = 0;
		BobTime = 0;
		PowerArmorTime = 0;
		WeaponSound = 0;

		Mem_Zero (&Anim, sizeof(Anim));
		Mem_Zero (&Timers, sizeof(Timers));
		Mem_Zero (&Grenade, sizeof(Grenade));
		Mem_Zero (&Flood, sizeof(Flood));
		Mem_Zero (&Chase, sizeof(Chase));
	#if CLEANCTF_ENABLED
		Mem_Zero (&Grapple, sizeof(Grapple));
	#endif
		Mem_Zero (&Tech, sizeof(Tech));

		Mem_Zero (&DamageValues, sizeof(DamageValues));
	}
}

// Players have a special way of allocating the entity.
// We won't automatically allocate it since it already exists
CPlayerEntity::CPlayerEntity (sint32 Index) :
IBaseEntity(Index),
IHurtableEntity(Index),
IPhysicsEntity(Index),
Client(&Game.Clients[State.GetNumber()-1]),
NoClip(false),
TossPhysics(false)
{
	EntityFlags |= EF_PLAYER;
	PhysicsType = PHYSICS_WALK;
};

CPlayerEntity::~CPlayerEntity ()
{
};

bool CPlayerEntity::Run ()
{
	BeginServerFrame ();
	return true;
};

/*
==============
BeginServerFrame

This will be called for all players
==============
*/
void CPlayerEntity::BeginServerFrame ()
{
	if (Level.Intermission.Time)
		return;

	if ((Game.GameMode & GAME_DEATHMATCH) &&  
#if CLEANCTF_ENABLED
		!(Game.GameMode & GAME_CTF) &&
#endif
		Client.Persistent.Spectator != Client.Respawn.Spectator &&
		(Level.Frame - Client.Timers.RespawnTime) >= 50)
	{
		SpectatorRespawn();
		return;
	}

	// run weapon animations
	if (!Client.Respawn.Spectator && Client.Persistent.Weapon)
	{
#if CLEANCTF_ENABLED
		if (!(Game.GameMode & GAME_CTF) || ((Game.GameMode & GAME_CTF) && !NoClip))
#endif
		Client.Persistent.Weapon->Think (this);
	}

	if (IsDead)
	{
		// wait for any button just going down
		if ( Level.Frame > Client.Timers.RespawnTime)
		{
			sint32 buttonMask;
			// in deathmatch, only wait for attack button
			if (Game.GameMode & GAME_DEATHMATCH)
				buttonMask = BUTTON_ATTACK;
			else
				buttonMask = -1;

			if ((Client.LatchedButtons & buttonMask) ||
				((Game.GameMode & GAME_DEATHMATCH) && DeathmatchFlags.dfForceRespawn.IsEnabled() ) )
			{
				Respawn();
				Client.LatchedButtons = 0;
			}
		}
		return;
	}

	// add player trail so monsters can follow
	if (!(Game.GameMode & GAME_DEATHMATCH) && !IsVisible (State.GetOrigin() + vec3f(0, 0, ViewHeight), PlayerTrail_LastSpot()->Origin, this))
		PlayerTrail_Add (State.GetOldOrigin());

	Client.LatchedButtons = 0;
	PlayerEvents::PlayerBeginServerFrame(this);
};

void CPlayerEntity::Respawn ()
{
	if (!(Game.GameMode & GAME_SINGLEPLAYER))
	{
		// Spectator's don't leave bodies
		if (!NoClip)
			CopyToBodyQueue ();
		GetSvFlags() &= ~SVF_NOCLIENT;
		PutInServer ();

		// add a teleportation effect
		State.GetEvent() = EV_PLAYER_TELEPORT;

		// hold in place briefly
		Client.PlayerState.GetPMove()->PMoveFlags = PMF_TIME_TELEPORT;
		Client.PlayerState.GetPMove()->PMoveTime = 14;

		Client.Timers.RespawnTime = Level.Frame;
		return;
	}

	// restart the entire server
	gi.AddCommandString ("menu_loadgame\n");
}

#include "Utility/TemporaryEntities.h"

/* 
 * only called when Persistent.Spectator changes
 * note that Respawn.Spectator should be the opposite of Persistent.Spectator here
 */
void CPlayerEntity::SpectatorRespawn ()
{
	// if the user wants to become a Spectator, make sure he doesn't
	// exceed max_spectators

	if (Client.Persistent.Spectator)
	{
		String value = Client.Persistent.UserInfo.GetValueFromKey ("spectator");
		if (*CvarList[CV_SPECTATOR_PASSWORD].StringValue() && 
			Q_stricmp(CvarList[CV_PASSWORD].StringValue(), "none") && 
			value != CvarList[CV_SPECTATOR_PASSWORD].StringValue())
		{
			PrintToClient (PRINT_HIGH, "Spectator password incorrect.\n");
			Client.Persistent.Spectator = false;
			StuffText ("spectator 0\n");
			return;
		}

		// count spectators
		sint32 numspec = 0;
		for (sint32 i = 1; i <= Game.MaxClients; i++)
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
			if (Player->GetInUse() && Player->Client.Persistent.Spectator)
				numspec++;
		}

		if (numspec >= Game.MaxSpectators)
		{
			PrintToClient (PRINT_HIGH, "Server Spectator limit is full.");
			Client.Persistent.Spectator = false;
			// reset his Spectator var
			StuffText ("spectator 0\n");
			return;
		}
	}
	else
	{
		// he was a Spectator and wants to join the game
		// he must have the right password
		String value = Client.Persistent.UserInfo.GetValueFromKey ("password");
		if (*CvarList[CV_PASSWORD].StringValue() && Q_stricmp(CvarList[CV_PASSWORD].StringValue(), "none") && 
			value != CvarList[CV_PASSWORD].StringValue())
		{
			PrintToClient (PRINT_HIGH, "Password incorrect.\n");
			Client.Persistent.Spectator = true;
			StuffText ("spectator 1\n");
			return;
		}
	}

	// clear client on respawn
	Client.Respawn.Score = Client.Persistent.Score = 0;

	GetSvFlags() &= ~SVF_NOCLIENT;
	PutInServer ();

	// add a teleportation effect
	if (!Client.Persistent.Spectator)
	{
		// send effect
		CMuzzleFlash(State.GetOrigin(), State.GetNumber(), MZ_LOGIN).Send();

		// hold in place briefly
		Client.PlayerState.GetPMove()->PMoveFlags = PMF_TIME_TELEPORT;
		Client.PlayerState.GetPMove()->PMoveTime = 14;
	}

	Client.Timers.RespawnTime = Level.Frame;

	if (Client.Persistent.Spectator) 
		BroadcastPrintf (PRINT_HIGH, "%s has moved to the sidelines\n", Client.Persistent.Name.CString());
	else
		BroadcastPrintf (PRINT_HIGH, "%s joined the game\n", Client.Persistent.Name.CString());
}

/*
===========
PutInServer

Called when a player connects to a server or respawns in
a CvarList[CV_DEATHMATCH].
============
*/
void CPlayerEntity::PutInServer ()
{
	static const vec3f		mins (-16, -16, -2);
	static const vec3f		maxs (16, 16, 32);
	sint32						index;
	vec3f					spawn_origin, spawn_angles;
	sint32						i;
	static CPersistentData		saved;
	static CRespawnData			Respawn;

	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	SelectSpawnPoint (spawn_origin, spawn_angles);

	index = State.GetNumber()-1;

	CUserInfo UserInfo = Client.Persistent.UserInfo;
	switch (Game.GameMode)
	{
	// deathmatch wipes most client data every spawn
	default:
			Respawn = Client.Respawn;
			InitPersistent ();
			UserinfoChanged (UserInfo.ToString());
		break;
	case GAME_COOPERATIVE:
			Respawn = Client.Respawn;

			Respawn.CoopRespawn.GameHelpChanged = Client.Persistent.GameHelpChanged;
			Respawn.CoopRespawn.HelpChanged = Client.Persistent.HelpChanged;
			Client.Persistent = Respawn.CoopRespawn;
			UserinfoChanged (UserInfo.ToString());
			if (Respawn.Score > Client.Persistent.Score)
				Client.Persistent.Score = Respawn.Score;
		break;
	case GAME_SINGLEPLAYER:
			Respawn.Clear ();
		break;
	}

	// clear everything but the persistant data
	saved = Client.Persistent;
	Client.Clear ();

	Client.Persistent = saved;
	if (Client.Persistent.Health <= 0)
	{
		InitPersistent();
	
		// Paril, this fixes occasions where a demo/cin is played first.
		if (Game.GameMode & GAME_COOPERATIVE)
			UserinfoChanged (saved.UserInfo.ToString());
	}
	Client.Respawn = Respawn;
	Client.Persistent.State = SVCS_SPAWNED;

	// copy some data from the client to the entity
	FetchEntData ();

	// clear entity values
	GroundEntity = nullentity;
	CanTakeDamage = true;
	NoClip = false;
	TossPhysics = false;
	ViewHeight = 22;
	GetInUse() = true;
	ClassName = "player";
	Mass = 200;
	GetSolid() = SOLID_BBOX;
	IsDead = false;
	AirFinished = Level.Frame + 120;
	GetClipmask() = CONTENTS_MASK_PLAYERSOLID;
	WaterInfo.OldLevel = WATER_NONE;
	WaterInfo.Level = WATER_NONE;
	WaterInfo.Type = 0;
	AffectedByKnockback = true;
	GetSvFlags() &= ~SVF_DEADMONSTER;
	if (!Client.Respawn.MenuState.Player)
		Client.Respawn.MenuState.Initialize (this);

#if CLEANCODE_IRC
	Client.Respawn.IRC.Player = this;
#endif

#if ROGUE_FEATURES
	Flags &= ~FL_SAM_RAIMI;		// PGM - turn off sam raimi flag
#endif

	GetMins() = mins;
	GetMaxs() = maxs;
	Velocity.Clear ();

	// clear playerstate values
	Client.PlayerState.Clear ();

	Client.PlayerState.GetPMove()->Origin = (spawn_origin * 8);

//ZOID
	Client.PlayerState.GetPMove()->PMoveFlags &= ~PMF_NO_PREDICTION;
//ZOID

	if ((Game.GameMode & GAME_DEATHMATCH) && DeathmatchFlags.dfFixedFov.IsEnabled())
		Client.PlayerState.GetFov () = 90;
	else
	{
		float fov = atof(Client.Persistent.UserInfo.GetValueFromKey("fov").CString());
		if (fov < 1)
			fov = 90;
		else if (fov > 160)
			fov = 160;

		Client.PlayerState.GetFov () = fov;
	}

	//client->playerState.gunIndex = ModelIndex(client->Persistent.weapon->view_model);

	// clear entity state values
	State.GetEffects() = 0;
	State.GetModelIndex() = 255;		// will use the skin specified model
	State.GetModelIndex(2) = 255;		// custom gun model
	// sknum is player num and weapon number
	// weapon number will be added in changeweapon
	State.GetSkinNum()--;
	State.GetFrame() = 0;

	State.GetOrigin() = (spawn_origin + vec3f(0,0,1));
	State.GetOldOrigin() = State.GetOrigin();

	// set the delta angle
	for (i = 0; i < 3; i++)
		Client.PlayerState.GetPMove()->DeltaAngles[i] = ANGLE2SHORT(spawn_angles[i] - Client.Respawn.CmdAngles[i]);

	State.GetAngles().Set (0, spawn_angles[YAW], 0);
	Client.PlayerState.GetViewAngles() = State.GetAngles();
	Client.ViewAngle = State.GetAngles();

#if CLEANCTF_ENABLED
//ZOID
	if ((Game.GameMode & GAME_CTF) && CTFStart())
		return;
//ZOID
#endif

	// spawn a Spectator
	if (Client.Persistent.Spectator)
	{
		Client.Chase.Target = NULL;

		Client.Respawn.Spectator = true;

		NoClip = true;
		GetSolid() = SOLID_NOT;
		GetSvFlags() |= SVF_NOCLIENT;
		Client.PlayerState.GetGunIndex () = 0;
		Link ();
		return;
	}
	else
		Client.Respawn.Spectator = false;

	KillBox ();
	Link ();

	// force the current weapon up
	Client.NewWeapon = Client.Persistent.Weapon;
	Client.Persistent.Weapon->ChangeWeapon(this);
}

/*
==============
InitClientPersistant

This is only called when the game first initializes in single player,
but is called after each death and level change in deathmatch
==============
*/
void CPlayerEntity::InitPersistent ()
{
	Client.Persistent.Clear ();

	if (!CvarList[CV_MAP_DEBUG].Boolean())
	{
		NItems::Blaster->Add(this, 1);
		Client.Persistent.Weapon = &CBlaster::Weapon;
		Client.Persistent.LastWeapon = Client.Persistent.Weapon;
		Client.Persistent.Inventory.SelectedItem = Client.Persistent.Weapon->Item->GetIndex();
	}
	else
	{
		FindItem("Surface Picker")->Add(this, 1);
		Client.Persistent.Weapon = &CSurfacePicker::Weapon;
		Client.Persistent.LastWeapon = Client.Persistent.Weapon;
		Client.Persistent.Inventory.SelectedItem = Client.Persistent.Weapon->Item->GetIndex();
	}

#if CLEANCTF_ENABLED
	if (Game.GameMode & GAME_CTF)
		NItems::Grapple->Add(this, 1);
	Client.Persistent.Flag = NULL;
#endif

	Client.Persistent.Tech = NULL;
	Client.Persistent.Armor = NULL;

	Client.Persistent.Health			= 100;
	Client.Persistent.MaxHealth			= 100;

	InitItemMaxValues();
}

void CPlayerEntity::InitItemMaxValues ()
{
	Client.Persistent.MaxAmmoValues[AMMOTAG_SHELLS] = 100;
	Client.Persistent.MaxAmmoValues[AMMOTAG_BULLETS] = 200;
	Client.Persistent.MaxAmmoValues[AMMOTAG_GRENADES] = 50;
	Client.Persistent.MaxAmmoValues[AMMOTAG_ROCKETS] = 50;
	Client.Persistent.MaxAmmoValues[AMMOTAG_CELLS] = 200;
	Client.Persistent.MaxAmmoValues[AMMOTAG_SLUGS] = 50;
#if XATRIX_FEATURES
	Client.Persistent.MaxAmmoValues[AMMOTAG_MAGSLUGS] = 50;
	Client.Persistent.MaxAmmoValues[AMMOTAG_TRAP] = 5;
#endif
#if ROGUE_FEATURES
	Client.Persistent.MaxAmmoValues[AMMOTAG_PROX] = 50;
	Client.Persistent.MaxAmmoValues[AMMOTAG_FLECHETTES] = 200;
	Client.Persistent.MaxAmmoValues[AMMOTAG_TESLA] = 50;
	Client.Persistent.MaxAmmoValues[AMMOTAG_ROUNDS] = 100;
#endif
}

/*
===========
UserinfoChanged

called whenever the player updates a userinfo variable.

The game can override any of the settings in place
(forcing skins or names, etc) before copying it off.
============
*/
void CPlayerEntity::UserinfoChanged (String userinfo)
{
	CUserInfo UserInfo;

	// check for malformed or illegal info strings
	if (!CUserInfo::Validate (userinfo))
		UserInfo.Update (String("\\name\\badinfo\\skin\\male/grunt"));
	else
		UserInfo.Update (userinfo);

	// set name
	Client.Persistent.Name = UserInfo.GetValueFromKey ("name");

	// set Spectator
	String s = UserInfo.GetValueFromKey ("spectator");
	// spectators are only supported in deathmatch
	Client.Persistent.Spectator = ((Game.GameMode & GAME_DEATHMATCH) && s.Count() && s != "0");

	// set skin
	s = UserInfo.GetValueFromKey ("skin");
	sint32 playernum = State.GetNumber() - 1;

	// combine name and skin into a configstring
#if CLEANCTF_ENABLED
//ZOID
	if (Game.GameMode & GAME_CTF)
		CTFAssignSkin(UserInfo);
	else
//ZOID
#endif
		ConfigString (CS_PLAYERSKINS+playernum, String::Format("%s\\%s", Client.Persistent.Name.CString(), s.CString()).CString());

	// fov
	if ((Game.GameMode & GAME_DEATHMATCH) && DeathmatchFlags.dfFixedFov.IsEnabled())
		Client.PlayerState.GetFov () = 90;
	else
		Client.PlayerState.GetFov () = Clamp<float> (atof(UserInfo.GetValueFromKey ("fov").CString()), 1, 160);

	// handedness
	s = UserInfo.GetValueFromKey ("hand");
	if (s.Count())
		Client.Persistent.Hand = atoi(s.CString());

	// IP
	// Paril: removed. could be changed any time in-game!

	// Gender
	s = UserInfo.GetValueFromKey ("gender");
	if (s.Count())
	{
		switch (s[0])
		{
		case 'm':
		case 'M':
			Client.Respawn.Gender = GENDER_MALE;
			break;
		case 'f':
		case 'F':
			Client.Respawn.Gender = GENDER_FEMALE;
			break;
		default:
			Client.Respawn.Gender = GENDER_NEUTRAL;
			break;
		}
	}
	else
		Client.Respawn.Gender = GENDER_MALE;

	// MSG command
	s = UserInfo.GetValueFromKey ("msg");
	if (s.Count())
		Client.Respawn.MessageLevel = atoi (s.CString());

	// save off the userinfo in case we want to check something later
	Client.Persistent.UserInfo = UserInfo;
}

#if CLEANCTF_ENABLED
void CPlayerEntity::CTFAssignSkin (CUserInfo &s)
{
	sint32 playernum = State.GetNumber()-1;
	String t = s.GetValueFromKey ("skin");

	if (t.IndexOf('/') != 1)
		t.Remove (t.IndexOf('/') + 1);
	else
		t = "male/";

	switch (Client.Respawn.CTF.Team)
	{
	case CTF_TEAM1:
	case CTF_TEAM2:
		// Paril, Issue 5 part 1 fix
		ConfigString (CS_PLAYERSKINS+playernum, String::Format("%s\\%s%s", Client.Persistent.Name.CString(), t.CString(), CTFTeamSkin(Client.Respawn.CTF.Team)).CString());
		break;
	default:
		// Paril, Issue 5 part 1 fix
		ConfigString (CS_PLAYERSKINS+playernum, String::Format("%s\\%s", Client.Persistent.Name.CString(), s.GetValueFromKey (String("skin")).CString()).CString());
		break;
	}
}

bool CPlayerEntity::CTFStart ()
{
	if (Client.Respawn.CTF.Team != CTF_NOTEAM)
		return false;

	if (!DeathmatchFlags.dfCtfForceJoin.IsEnabled())
	{
		// start as 'observer'
		Client.Respawn.CTF.Team = CTF_NOTEAM;
		NoClip = true;
		GetSolid() = SOLID_NOT;
		GetSvFlags() |= SVF_NOCLIENT;
		Client.PlayerState.GetGunIndex () = 0;
		Link ();

		CTFOpenJoinMenu(this);
		return true;
	}
	return false;
}
#endif

void CPlayerEntity::FetchEntData ()
{
	Health = Client.Persistent.Health;
	MaxHealth = Client.Persistent.MaxHealth;
	Flags |= Client.Persistent.SavedFlags;
	if (Game.GameMode & GAME_COOPERATIVE)
		Client.Respawn.Score = Client.Persistent.Score;
}

// ==============================================
// Follows is the entire p_view.cpp file!

float	bobmove;
sint32		bobcycle;		// odd cycles are right foot going forward
float	bobfracsin;		// sin(bobfrac*M_PI)

/*
===============
CalcRoll
===============
*/
inline float CPlayerEntity::CalcRoll (vec3f &velocity, vec3f &right)
{
	float	side = Abs(velocity | right);
	float	sign = side < 0 ? -1 : 1;

	if (side < CvarList[CV_ROLLSPEED].Float())
		side = side * CvarList[CV_ROLLANGLE].Float() / CvarList[CV_ROLLSPEED].Float();
	else
		side = CvarList[CV_ROLLANGLE].Float();
	
	return side*sign;
}

/*
===============
DamageFeedback
===============
*/
inline void CPlayerEntity::DamageFeedback (vec3f &forward, vec3f &right)
{
	static	const vec3f	PowerColor (0, 1, 0);
	static	const vec3f	ArmorColor (1, 1, 1);
	static	const vec3f	BloodColor (1, 0, 0);

	// flash the backgrounds behind the status numbers
	Client.PlayerState.GetStat (STAT_FLASHES) = 0;
	if (Client.DamageValues[DT_BLOOD])
		Client.PlayerState.GetStat (STAT_FLASHES) = Client.PlayerState.GetStat(STAT_FLASHES) | 1;
	if (Client.DamageValues[DT_ARMOR] && !(Flags & FL_GODMODE) && (Client.Timers.Invincibility <= Level.Frame))
		Client.PlayerState.GetStat (STAT_FLASHES) = Client.PlayerState.GetStat(STAT_FLASHES) | 2;

	// total points of damage shot at the player this frame
	sint32 count = (Client.DamageValues[DT_BLOOD] + Client.DamageValues[DT_ARMOR] + Client.DamageValues[DT_POWERARMOR]);
	if (count == 0)
		return;		// didn't take any damage

	// start a pain animation if still in the player model
	if (Client.Anim.Priority < ANIM_PAIN && State.GetModelIndex() == 255)
	{
		Client.Anim.Priority = ANIM_PAIN;
		if (Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED)
		{
			State.GetFrame() = FRAME_crpain1 - 1;
			Client.Anim.EndFrame = FRAME_crpain4;
		}
		else
		{
			switch (irandom(3))
			{
			case 0:
				State.GetFrame() = FRAME_pain101 - 1;
				Client.Anim.EndFrame = FRAME_pain104;
				break;
			case 1:
				State.GetFrame() = FRAME_pain201 - 1;
				Client.Anim.EndFrame = FRAME_pain204;
				break;
			case 2:
				State.GetFrame() = FRAME_pain301 - 1;
				Client.Anim.EndFrame = FRAME_pain304;
				break;
			}
		}
	}

	sint32 realcount = count;
	if (count < 10)
		count = 10;	// always make a visible effect

	// play an apropriate pain sound
	if ((Level.Frame > PainDebounceTime) && !(Flags & FL_GODMODE) && (Client.Timers.Invincibility <= Level.Frame))
	{
		PainDebounceTime = Level.Frame + 7;

		sint32 l = Clamp<sint32>(((floorf((Max<>(0, Health-1)) / 25))), 0, 3);
		PlaySound (CHAN_VOICE, GameMedia.Player.Pain[l][(irandom(2))]);
	}

	// the total alpha of the blend is always proportional to count
	if (Client.DamageBlend.A < 0)
		Client.DamageBlend.A = 0;
	Client.DamageBlend.A += count*0.01f;
	if (Client.DamageBlend.A < 0.2f)
		Client.DamageBlend.A = 0.2f;
	if (Client.DamageBlend.A > 0.6f)
		Client.DamageBlend.A = 0.6f;		// don't go too saturated

	// the color of the blend will vary based on how much was absorbed
	// by different armors
	colorf NewColor (0, 0, 0, Client.DamageBlend.A);
	if (Client.DamageValues[DT_POWERARMOR])
		NewColor.G = NewColor.G + (float)Client.DamageValues[DT_POWERARMOR]/realcount;
	if (Client.DamageValues[DT_ARMOR])
	{
		NewColor.R = NewColor.R + (float)Client.DamageValues[DT_ARMOR]/realcount;
		NewColor.G = NewColor.G + (float)Client.DamageValues[DT_ARMOR]/realcount;
		NewColor.B = NewColor.B + (float)Client.DamageValues[DT_ARMOR]/realcount;
	}
	if (Client.DamageValues[DT_BLOOD])
		NewColor.R = NewColor.R + (float)Client.DamageValues[DT_BLOOD]/realcount;

	Client.DamageBlend = NewColor;


	//
	// calculate view angle kicks
	//
	float Kick = Abs(Client.DamageValues[DT_KNOCKBACK]);
	if (Kick && (Health > 0))	// kick of 0 means no view adjust at all
	{
		Kick *= 100 / Health;

		if (Kick < count*0.5)
			Kick = count*0.5;
		if (Kick > 50)
			Kick = 50;

		vec3f v = (Client.DamageFrom - State.GetOrigin ()).GetNormalized();
		
		Client.ViewDamage[1] = Kick * (v | right) * 0.3f;
		Client.ViewDamage[0] = Kick * -(v | forward) * 0.3f;
		Client.ViewDamageTime = Level.Frame + DAMAGE_TIME;
	}

	//
	// clear totals
	//
	Mem_Zero (&Client.DamageValues, sizeof(Client.DamageValues));
}

/*
===============
SV_CalcViewOffset
===============
*/
inline void CPlayerEntity::CalcViewOffset (vec3f &forward, vec3f &right, vec3f &up, float xyspeed)
{
	float		bob;
	float		ratio;
	vec3f		v (0, 0, 0);

	// if dead, fix the angle and don't add any kick
	if (IsDead)
	{
		Client.PlayerState.GetViewAngles().Set (-15, Client.KillerYaw, 40);
		Client.PlayerState.GetKickAngles().Clear ();
	}
	else
	{
		// Base angles
		vec3f angles = Client.KickAngles;

		// add angles based on damage kick
		ratio = (float)(Client.ViewDamageTime - Level.Frame) / DAMAGE_TIME;
		if (ratio < 0)
		{
			ratio = 0;
			Client.ViewDamage[0] = Client.ViewDamage[1] = 0;
		}
		angles.X += ratio * Client.ViewDamage[0];
		angles.Z += ratio * Client.ViewDamage[1];

		// add pitch based on fall kick
		ratio = (float)(Client.FallTime - Level.Frame) / FALL_TIME;
		if (ratio < 0)
			ratio = 0;
		angles.X += ratio * Client.FallValue;

		// add angles based on velocity
		float delta = Velocity | forward;
		angles.X += delta*CvarList[CV_RUN_PITCH].Float();
		
		delta = Velocity | right;
		angles.Z += delta*CvarList[CV_RUN_ROLL].Float();

		// add angles based on bob

		delta = bobfracsin * CvarList[CV_BOB_PITCH].Float() * xyspeed;
		if (Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED)
			delta *= 6;		// crouching
		angles.X += delta;
		delta = bobfracsin * CvarList[CV_BOB_ROLL].Float() * xyspeed;
		if (Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED)
			delta *= 6;		// crouching
		if (bobcycle & 1)
			delta = -delta;
		angles.Z += delta;

		Client.PlayerState.GetKickAngles() = angles;
	}

//===================================

	// add view height
	v.Z += ViewHeight;

	// add fall height
	ratio = (float)(Client.FallTime - Level.Frame) / FALL_TIME;
	if (ratio < 0)
		ratio = 0;
	v.Z -= ratio * Client.FallValue * 0.4;

	// add bob height
	bob = bobfracsin * xyspeed * CvarList[CV_BOB_UP].Float();
	if (bob > 6)
		bob = 6;
	v.Z += bob;

	// add kick offset
	v += Client.KickOrigin;

	// absolutely bound offsets
	// so the view can never be outside the player box
	if (v.X < -14)
		v.X = -14;
	else if (v.X > 14)
		v.X = 14;
	if (v.Y < -14)
		v.Y = -14;
	else if (v.Y > 14)
		v.Y = 14;
	if (v.Z < -22)
		v.Z = -22;
	else if (v.Z > 30)
		v.Z = 30;

	Client.PlayerState.GetViewOffset() = v;
}

/*
==============
SV_CalcGunOffset
==============
*/
inline void CPlayerEntity::CalcGunOffset (vec3f &forward, vec3f &right, vec3f &up, float xyspeed)
{
	// gun angles from bobbing
	vec3f angles	(	(bobcycle & 1) ? (-Client.PlayerState.GetGunAngles ().Z) : (xyspeed * bobfracsin * 0.005), 
						(bobcycle & 1) ? (-Client.PlayerState.GetGunAngles ().Y) : (xyspeed * bobfracsin * 0.01),
						xyspeed * bobfracsin * 0.005
					);

	// gun angles from delta movement
	for (sint32 i = 0; i < 3; i++)
	{
		float delta = Client.OldViewAngles[i] - angles[i];
		if (delta > 180)
			delta -= 360;
		if (delta < -180)
			delta += 360;
		if (delta > 45)
			delta = 45;
		if (delta < -45)
			delta = -45;
		if (i == YAW)
			angles[ROLL] += 0.1*delta;
		angles[i] += 0.2 * delta;
	}

	// copy it out
	Client.PlayerState.GetGunAngles() = angles;

	// gun height
	Client.PlayerState.GetGunOffset().Clear ();

	angles.Clear ();
	// gun_x / gun_y / gun_z are development tools
	for (sint32 i = 0; i < 3; i++)
	{
		angles[i] += forward[i] * CvarList[CV_GUN_Y].Float();
		angles[i] += right[i] * CvarList[CV_GUN_X].Float();
		angles[i] += up[i] * -CvarList[CV_GUN_Z].Float();
	}

	Client.PlayerState.GetGunAngles() = angles;
}


/*
=============
SV_AddBlend
=============
*/
// Not a part of CPlayerEntity
static inline void SV_AddBlend (colorf color, colorf &v_blend)
{
/*	if (color.A <= 0)
		return;

	uint8 a2 = v_blend.A  + color.A;	// new total alpha
	float a3 = (float)(v_blend.A) / (float)(a2);		// fraction of color from old

	v_blend.R = (float)(v_blend.R)*a3 + color.R*(1-a3);
	v_blend.G = (float)(v_blend.G)*a3 + color.G*(1-a3);
	v_blend.B = (float)(v_blend.B)*a3 + color.B*(1-a3);
	v_blend.A = a2;*/
	if (color.A <= 0)
		return;
	float a2 = v_blend.A + (1-v_blend.A)*color.A;	// new total alpha
	float a3 = v_blend.A/a2;		// fraction of color from old

	v_blend.R = v_blend.R*a3 + color.R*(1-a3);
	v_blend.G = v_blend.G*a3 + color.G*(1-a3);
	v_blend.B = v_blend.B*a3 + color.B*(1-a3);
	v_blend.A = a2;
}


/*
=============
SV_CalcBlend
=============
*/
static const colorf LavaColor (1.0f, 0.3f, 0.0f, 0.6f);
static const colorf SlimeColor (0.0f, 0.1f, 0.05f, 0.6f);
static const colorf WaterColor (0.5f, 0.3f, 0.2f, 0.4f);
static const colorf QuadColor (0, 0, 1, 0.08f);
#if XATRIX_FEATURES
static const colorf QuadFireColor (1, 0.2f, 0.5f, 0.08f);
#endif
#if ROGUE_FEATURES
static const colorf DoubleColor (0.9f, 0.7f, 0, 0.08f);
static const colorf IRColor (0, 0, 0, 0.2f);
#endif
static const colorf InvulColor (1, 1, 0, 0.08f);
static const colorf EnviroColor (0, 1, 0, 0.08f);
static const colorf BreatherColor (0.4f, 1, 0.4f, 0.04f);
static const colorf ClearColor (0,0,0,0);
static colorf BonusColor (0.85f, 0.7f, 0.3f, 0);

inline void CPlayerEntity::CalcBlend ()
{
	// add for contents
	vec3f	vieworg = State.GetOrigin() + Client.PlayerState.GetViewOffset();
	EBrushContents contents = PointContents (vieworg);

	if (contents & (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER) )
		Client.PlayerState.GetRdFlags () |= RDF_UNDERWATER;
	else
		Client.PlayerState.GetRdFlags () &= ~RDF_UNDERWATER;

	// add for powerups
	if (Client.Timers.QuadDamage > Level.Frame)
	{
		sint32 remaining = Client.Timers.QuadDamage - Level.Frame;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/damage2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (QuadColor, Client.Persistent.ViewBlend);
	}
#if ROGUE_FEATURES
	else if (Client.Timers.Double > Level.Frame)
	{
		sint32 remaining = Client.Timers.Double - Level.Frame;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("misc/ddamage2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (DoubleColor, Client.Persistent.ViewBlend);
	}
#endif
#if XATRIX_FEATURES
	else if (Client.Timers.QuadFire > Level.Frame)
	{
		sint32 remaining = Client.Timers.QuadFire - Level.Frame;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/quadfire2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (QuadFireColor, Client.Persistent.ViewBlend);
	}
#endif
	else if (Client.Timers.Invincibility > Level.Frame)
	{
		sint32 remaining = Client.Timers.Invincibility - Level.Frame;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/protect2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (InvulColor, Client.Persistent.ViewBlend);
	}
	else if (Client.Timers.EnvironmentSuit > Level.Frame)
	{
		sint32 remaining = Client.Timers.EnvironmentSuit - Level.Frame;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/airout.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (EnviroColor, Client.Persistent.ViewBlend);
	}
	else if (Client.Timers.Rebreather > Level.Frame)
	{
		sint32 remaining = Client.Timers.Rebreather - Level.Frame;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/airout.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (BreatherColor, Client.Persistent.ViewBlend);
	}

#if ROGUE_FEATURES
//PGM
	if (Client.Timers.Nuke > Level.Frame)
		SV_AddBlend (colorf (1, 1, 1, (Client.Timers.Nuke - Level.Frame) / 20.0), Client.Persistent.ViewBlend);

	if (Client.Timers.IR > Level.Frame)
	{
		sint32 remaining = Client.Timers.IR - Level.Frame;

		if(remaining > 30 || (remaining & 4))
		{
			Client.PlayerState.GetRdFlags() |= RDF_IRGOGGLES;
			SV_AddBlend (IRColor, Client.Persistent.ViewBlend);
		}
		else
			Client.PlayerState.GetRdFlags() &= ~RDF_IRGOGGLES;
	}
	else
		Client.PlayerState.GetRdFlags() &= ~RDF_IRGOGGLES;
//PGM
#endif

	// add for damage
	if (Client.DamageBlend.A)
		SV_AddBlend (Client.DamageBlend, Client.Persistent.ViewBlend);

	// drop the damage value
	if (Client.DamageBlend.A)
	{
		Client.DamageBlend.A -= 0.06f;
		if (Client.DamageBlend.A < 0)
			Client.DamageBlend.A = 0;
	}

	// add bonus and drop the value
	if (Client.BonusAlpha)
	{
		BonusColor.A = Client.BonusAlpha;
		SV_AddBlend (BonusColor, Client.Persistent.ViewBlend);

		Client.BonusAlpha -= 0.1f;
		if (Client.BonusAlpha < 0)
			Client.BonusAlpha = 0;
	}

	if (contents & (CONTENTS_LAVA))
		SV_AddBlend (LavaColor, Client.Persistent.ViewBlend);
	else if (contents & CONTENTS_SLIME)
		SV_AddBlend (SlimeColor, Client.Persistent.ViewBlend);
	else if (contents & CONTENTS_WATER)
		SV_AddBlend (WaterColor, Client.Persistent.ViewBlend);

	Client.PlayerState.GetViewBlend () = Client.Persistent.ViewBlend;
	Client.Persistent.ViewBlend.Set(ClearColor);
}


/*
=================
P_FallingDamage
=================
*/
inline void CPlayerEntity::FallingDamage ()
{
	if (DeathmatchFlags.dfNoFallingDamage.IsEnabled())
		return;

	if (State.GetModelIndex() != 255)
		return;		// not in the player model

	if (NoClip)
		return;

#if CLEANCTF_ENABLED
//ZOID
	// never take damage if just release grapple or on grapple
	if (Level.Frame - Client.Grapple.ReleaseTime <= 2 ||
		(Client.Grapple.Entity && 
		Client.Grapple.State > CTF_GRAPPLE_STATE_FLY))
		return;
//ZOID
#endif
	float	delta;

	if ((Client.OldVelocity.Z < 0) && (Velocity.Z > Client.OldVelocity.Z) && (!GroundEntity))
		delta = Client.OldVelocity.Z;
	else
	{
		if (!GroundEntity)
			return;
		delta = Velocity.Z - Client.OldVelocity.Z;
	}
	delta = delta*delta * 0.0001;

	// never take falling damage if completely underwater
	if (WaterInfo.Level == WATER_UNDER)
		return;
	if (WaterInfo.Level == WATER_WAIST)
		delta *= 0.25;
	if (WaterInfo.Level == WATER_FEET)
		delta *= 0.5;

	if (delta < 1)
		return;

	if (delta < 15)
	{
		State.GetEvent() = EV_FOOTSTEP;
		return;
	}

	Client.FallValue = delta*0.5;
	if (Client.FallValue > 40)
		Client.FallValue = 40;
	Client.FallTime = Level.Frame + FALL_TIME;

	if (delta > 30)
	{
		if (Health > 0)
			State.GetEvent() = (delta >= 55) ? EV_FALLFAR : EV_FALL;

		PainDebounceTime = Level.Frame;	// no normal pain sound
		sint32 Damage = (delta-30)/2;
		if (Damage < 1)
			Damage = 1;

		TakeDamage (World, World, upOrigin, State.GetOrigin(), vec3fOrigin, Damage, 0, 0, MOD_FALLING);
	}
	else
	{
		State.GetEvent() = EV_FALLSHORT;
		return;
	}
}

/*
=============
P_WorldEffects
=============
*/
inline void CPlayerEntity::WorldEffects ()
{
	if (NoClip)
	{
		AirFinished = Level.Frame + 120;	// don't need air
		return;
	}

	//
	// if just entered a water volume, play a sound
	//
	if (!WaterInfo.OldLevel && WaterInfo.Level)
	{
		PlayerNoiseAt (State.GetOrigin(), PNOISE_SELF);
		if (WaterInfo.Type & CONTENTS_LAVA)
			PlaySound (CHAN_BODY, SoundIndex("player/lava_in.wav"));
		else if (WaterInfo.Type & CONTENTS_SLIME)
			PlaySound (CHAN_BODY, SoundIndex("player/watr_in.wav"));
		else if (WaterInfo.Type & CONTENTS_WATER)
			PlaySound (CHAN_BODY, SoundIndex("player/watr_in.wav"));

		// clear damage_debounce, so the pain sound will play immediately
		PainDebounceTime = Level.Frame - 1;

		WaterInfo.OldLevel = WaterInfo.Level;
	}

	//
	// if just completely exited a water volume, play a sound
	//
	if (WaterInfo.OldLevel && !WaterInfo.Level)
	{
		PlayerNoiseAt (State.GetOrigin(), PNOISE_SELF);
		PlaySound (CHAN_BODY, SoundIndex("player/watr_out.wav"));

		WaterInfo.OldLevel = WATER_NONE;
	}

	//
	// check for head just going under water
	//
	if (WaterInfo.OldLevel != WATER_UNDER && WaterInfo.Level == WATER_UNDER)
	{
		PlaySound (CHAN_BODY, SoundIndex("player/watr_un.wav"));
		WaterInfo.OldLevel = WATER_UNDER;
	}

	//
	// check for head just coming out of water
	//
	if (WaterInfo.OldLevel == WATER_UNDER && WaterInfo.Level != WATER_UNDER)
	{
		if (AirFinished < Level.Frame)
		{	// gasp for air
			PlaySound (CHAN_VOICE, SoundIndex("player/gasp1.wav"));
			PlayerNoiseAt (State.GetOrigin(), PNOISE_SELF);
		}
		else if (AirFinished < Level.Frame + 110) // just break surface
			PlaySound (CHAN_VOICE, SoundIndex("player/gasp2.wav"));

		WaterInfo.OldLevel = WaterInfo.Level;
	}

	bool UsingEnvironmentSuit = (Client.Timers.EnvironmentSuit > Level.Frame);

	//
	// check for drowning
	//
	if (WaterInfo.Level == WATER_UNDER)
	{
		// breather or envirosuit give air
		if ((Client.Timers.Rebreather > Level.Frame) || UsingEnvironmentSuit)
		{
			AirFinished = Level.Frame + 100;

			if (((Client.Timers.Rebreather - Level.Frame) % 25) == 0)
			{
				PlaySound (CHAN_AUTO, SoundIndex((!Client.Timers.BreatherSound) ? "player/u_breath1.wav" : "player/u_breath2.wav"));
				Client.Timers.BreatherSound = !Client.Timers.BreatherSound;
				PlayerNoiseAt (State.GetOrigin(), PNOISE_SELF);
			}
		}

		// if out of air, start drowning
		if (AirFinished < Level.Frame)
		{
			// drown!
			if (NextDrownTime < Level.Frame && Health > 0)
			{
				NextDrownTime = Level.Frame + 10;

				// take more damage the longer underwater
				NextDrownDamage += 2;
				if (NextDrownDamage > 15)
					NextDrownDamage = 15;

				// play a gurp sound instead of a normal pain sound
				if (Health <= NextDrownDamage)
					PlaySound (CHAN_VOICE, SoundIndex("player/drown1.wav"));
				else
					PlaySound (CHAN_VOICE, GameMedia.Player.Gurp[(irandom(2))]);

				PainDebounceTime = Level.Frame;

				TakeDamage (World, World, vec3fOrigin, State.GetOrigin(), vec3fOrigin, NextDrownDamage, 0, DAMAGE_NO_ARMOR, MOD_WATER);
			}
		}
	}
	else
	{
		AirFinished = Level.Frame + 120;
		NextDrownDamage = 2;
	}

	//
	// check for sizzle damage
	//
	if (WaterInfo.Level && (WaterInfo.Type & (CONTENTS_LAVA|CONTENTS_SLIME)))
	{
		if (WaterInfo.Type & CONTENTS_LAVA)
		{
			if (Health > 0
				&& PainDebounceTime <= Level.Frame
				&& Client.Timers.Invincibility < Level.Frame)
			{
				PlaySound (CHAN_VOICE, SoundIndex((irandom(2)) ? "player/burn1.wav" : "player/burn2.wav"));
				PainDebounceTime = Level.Frame + 10;
			}

			// take 1/3 damage with envirosuit
			TakeDamage (World, World, vec3fOrigin, State.GetOrigin(), vec3fOrigin, (UsingEnvironmentSuit) ? 1*WaterInfo.Level : 3*WaterInfo.Level, 0, 0, MOD_LAVA);
		}

		if (WaterInfo.Type & CONTENTS_SLIME)
		{
			if (!UsingEnvironmentSuit) // no damage from slime with envirosuit
				TakeDamage (World, World, vec3fOrigin, State.GetOrigin(), vec3fOrigin, 1*WaterInfo.Level, 0, 0, MOD_SLIME);
		}
	}
}

/*
===============
G_SetClientEffects
===============
*/
EPowerArmorType CPlayerEntity::PowerArmorType ()
{
	if (!Client.PowerArmorEnabled)
		return POWER_ARMOR_NONE;
	else if (Client.Persistent.Inventory.Has(NItems::PowerShield) > 0)
		return POWER_ARMOR_SHIELD;
	else if (Client.Persistent.Inventory.Has(NItems::PowerScreen) > 0)
		return POWER_ARMOR_SCREEN;

	return POWER_ARMOR_NONE;
}

inline void CPlayerEntity::SetClientEffects ()
{
	State.GetEffects() = State.GetRenderEffects() = 0;

#if ROGUE_FEATURES
	State.GetRenderEffects() = RF_IR_VISIBLE;
#endif

	if (Health <= 0 || Level.Intermission.Time)
		return;

#if ROGUE_FEATURES
	if (Flags & FL_DISGUISED)
		State.GetRenderEffects() |= RF_USE_DISGUISE;
#endif

	if (Client.PowerArmorTime)
	{
		sint32 pa_type = PowerArmorType ();
		if (pa_type == POWER_ARMOR_SCREEN)
			State.GetEffects() |= FX_POWERSCREEN;
		else if (pa_type == POWER_ARMOR_SHIELD)
		{
			State.GetEffects() |= FX_COLOR_SHELL;
			State.GetRenderEffects() |= RF_SHELL_GREEN;
		}

		Client.PowerArmorTime--;
	}

#if CLEANCTF_ENABLED
	if (Game.GameMode & GAME_CTF)
	{
		State.GetEffects() &= ~(FX_FLAG1 | FX_FLAG2);
		if (Client.Persistent.Flag)
		{
			if (Health > 0)
				State.GetEffects() |= Client.Persistent.Flag->EffectFlags;
			State.GetModelIndex(3) = ModelIndex(Client.Persistent.Flag->WorldModel);
		}
		else
			State.GetModelIndex(3) = 0;
	}
#endif

	if (Client.Timers.QuadDamage > Level.Frame)
	{
		sint32 remaining = Client.Timers.QuadDamage - Level.Frame;
		if (remaining > 30 || (remaining & 4) )
			State.GetEffects() |= FX_QUAD;
	}

#if XATRIX_FEATURES
	if (Client.Timers.QuadFire > Level.Frame)
	{
		sint32 remaining = Client.Timers.QuadFire - Level.Frame;
		if (remaining > 30 || (remaining & 4) )
			State.GetEffects() |= FX_DOUBLE; // Paril disting.
	}
#endif

#if ROGUE_FEATURES
	if (Client.Timers.Double > Level.Frame)
	{
		sint32 remaining = Client.Timers.Double - Level.Frame;
		if (remaining > 30 || (remaining & 4) )
			State.GetEffects() |= FX_DOUBLE;
	}
	if ((Client.OwnedSphere) && (Client.OwnedSphere->SpawnFlags == 1))
		State.GetEffects() |= FX_HALF_DAMAGE;

	if (Client.Timers.Tracker > Level.Frame)
		State.GetEffects() |= FX_TRACKERTRAIL;
#endif

	if (Client.Timers.Invincibility > Level.Frame)
	{
		sint32 remaining = Client.Timers.Invincibility - Level.Frame;
		if (remaining > 30 || (remaining & 4) )
			State.GetEffects() |= FX_PENT;
	}

	// show cheaters!!!
	if (Flags & FL_GODMODE)
	{
		State.GetEffects() |= FX_COLOR_SHELL;
		State.GetRenderEffects() |= (RF_SHELL_RED|RF_SHELL_GREEN|RF_SHELL_BLUE);
	}
}


/*
===============
G_SetClientEvent
===============
*/
inline void CPlayerEntity::SetClientEvent (float xyspeed)
{
	if (State.GetEvent())
		return;

	if (GroundEntity && xyspeed > 225)
	{
		if ((sint32)(Client.BobTime+bobmove) != bobcycle )
			State.GetEvent() = EV_FOOTSTEP;
	}
}

/*
===============
G_SetClientSound
===============
*/
inline void CPlayerEntity::SetClientSound ()
{
	if (Client.Persistent.GameHelpChanged != Game.HelpChanged)
	{
		Client.Persistent.GameHelpChanged = Game.HelpChanged;
		Client.Persistent.HelpChanged = 1;
	}

	// help beep (no more than three times)
	if (Client.Persistent.HelpChanged && Client.Persistent.HelpChanged <= 3 && !(Level.Frame&63) )
	{
		Client.Persistent.HelpChanged++;
		PlaySound (CHAN_VOICE, SoundIndex ("misc/pc_up.wav"), 255, ATTN_STATIC);
	}

	if (WaterInfo.Level && (WaterInfo.Type & (CONTENTS_LAVA|CONTENTS_SLIME)))
		State.GetSound() = GameMedia.FrySound();
	else if (Client.Persistent.Weapon && Client.Persistent.Weapon->GetWeaponSound ())
		State.GetSound() = Client.Persistent.Weapon->GetWeaponSound ();
	else if (Client.WeaponSound)
		State.GetSound() = Client.WeaponSound;
	else
		State.GetSound() = 0;
}

/*
===============
G_SetClientFrame
===============
*/
inline void CPlayerEntity::SetClientFrame (float xyspeed)
{
	bool		duck = (Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED);
	bool		run = !!(xyspeed);
	bool		isNewAnim = false;

	if (State.GetModelIndex() != 255)
		return;		// not in the player model

	// check for stand/duck and stop/go transitions
	if ((duck != Client.Anim.Duck && Client.Anim.Priority < ANIM_DEATH) ||
		(run != Client.Anim.Run && Client.Anim.Priority == ANIM_BASIC) ||
		(!GroundEntity && Client.Anim.Priority <= ANIM_WAVE))
		isNewAnim = true;

	if (!isNewAnim)
	{
		if(Client.Anim.Priority == ANIM_REVERSE)
		{
			if(State.GetFrame() > Client.Anim.EndFrame)
			{
				State.GetFrame()--;
				return;
			}
		}
		else if (State.GetFrame() < Client.Anim.EndFrame)
		{
			// continue an animation
			State.GetFrame()++;
			return;
		}

		if (Client.Anim.Priority == ANIM_DEATH)
			return;		// stay there
		if (Client.Anim.Priority == ANIM_JUMP)
		{
			if (!GroundEntity)
				return;		// stay there
			Client.Anim.Priority = ANIM_WAVE;

			State.GetFrame() = FRAME_jump3;
			Client.Anim.EndFrame = FRAME_jump6;
			return;
		}
	}

	// return to either a running or standing frame
	Client.Anim.Priority = ANIM_BASIC;
	Client.Anim.Duck = duck;
	Client.Anim.Run = run;

	if (!GroundEntity)
	{
#if CLEANCTF_ENABLED
//ZOID: if on grapple, don't go into jump frame, go into standing
//frame
		if (Client.Grapple.Entity)
		{
			State.GetFrame() = FRAME_stand01;
			Client.Anim.EndFrame = FRAME_stand40;
		}
		else
		{
//ZOID
#endif
		Client.Anim.Priority = ANIM_JUMP;
		if (State.GetFrame() != FRAME_jump2)
			State.GetFrame() = FRAME_jump1;
		Client.Anim.EndFrame = FRAME_jump2;
#if CLEANCTF_ENABLED
	}
#endif
	}
	else if (run)
	{	// running
		if (duck)
		{
			State.GetFrame() = FRAME_crwalk1;
			Client.Anim.EndFrame = FRAME_crwalk6;
		}
		else
		{
			State.GetFrame() = FRAME_run1;
			Client.Anim.EndFrame = FRAME_run6;
		}
	}
	else
	{	// standing
		if (duck)
		{
			State.GetFrame() = FRAME_crstnd01;
			Client.Anim.EndFrame = FRAME_crstnd19;
		}
		else
		{
			State.GetFrame() = FRAME_stand01;
			Client.Anim.EndFrame = FRAME_stand40;
		}
	}
}

/*
=================
ClientEndServerFrame

Called for each player at the end of the server frame
and right after spawning
=================
*/
void CPlayerEntity::EndServerFrame ()
{
	if (!gameEntity)
		return;

	float	BobTime;
	sint32		i;

	//
	// If the origin or velocity have changed since ClientThink(),
	// update the pmove values.  This will happen when the client
	// is pushed by a bmodel or kicked by an explosion.
	// 
	// If it wasn't updated here, the view position would lag a frame
	// behind the body position when pushed -- "sinking into plats"
	//

	Client.PlayerState.GetPMove()->Origin = (State.GetOrigin() * 8);
	Client.PlayerState.GetPMove()->Velocity = (Velocity * 8);

	//
	// If the end of unit layout is displayed, don't give
	// the player any normal movement attributes
	//
	if (Level.Intermission.Time)
	{
		Client.PlayerState.GetViewBlend ().A = 0;
		Client.PlayerState.GetFov () = 90;
		SetStats ();
		return;
	}

	anglef angles = Client.ViewAngle.ToVectors ();

	// burn from lava, etc
	WorldEffects ();

	//
	// set model angles from view angles so other things in
	// the world can tell which direction you are looking
	//
	State.GetAngles().Set (
		(Client.ViewAngle.X > 180) ? (-360 + Client.ViewAngle.X)/3 : Client.ViewAngle.X/3,
		Client.ViewAngle.Y,
		CalcRoll (Velocity, angles.Right)*4
		);

	//
	// calculate speed and cycle to be used for
	// all cyclic walking effects
	//
	float xyspeed = sqrtf(Velocity.X*Velocity.X + Velocity.Y*Velocity.Y);

	if (xyspeed < 5 || Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED)
	{
		bobmove = 0;
		Client.BobTime = 0;	// start at beginning of cycle again
	}
	else if (GroundEntity)
	{
		// so bobbing only cycles when on ground
		if (xyspeed > 210)
			bobmove = 0.25;
		else if (xyspeed > 100)
			bobmove = 0.125;
		else
			bobmove = 0.0625;
	}
	
	BobTime = (Client.BobTime += bobmove);

	if (Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED)
		BobTime *= 4;

	bobcycle = (sint32)BobTime;
	bobfracsin = Abs(sinf(BobTime*M_PI));

	// detect hitting the floor
	FallingDamage ();

	// apply all the damage taken this frame
	DamageFeedback (angles.Forward, angles.Right);

	// determine the view offsets
	CalcViewOffset (angles.Forward, angles.Right, angles.Up, xyspeed);

	// determine the gun offsets
	CalcGunOffset (angles.Forward, angles.Right, angles.Up, xyspeed);

	// determine the full screen color blend
	// must be after viewoffset, so eye contents can be
	// accurately determined
	CalcBlend ();

	// chase cam stuff
	if (Client.Respawn.Spectator)
		SetSpectatorStats();
	else
#if CLEANCTF_ENABLED
//ZOID
	if (!Client.Chase.Target)
//ZOID
#endif
		SetStats ();

#if CLEANCTF_ENABLED
//ZOID
//update chasecam follower stats
	for (i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *e = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (!e->GetInUse() || e->Client.Chase.Target != this)
			continue;

		e->Client.PlayerState.CopyStats (Client.PlayerState.GetStats());
		e->Client.PlayerState.GetStat (STAT_LAYOUTS) = 1;

		// Paril: was this break a mistake on Zoid's part?
		//break;
	}
//ZOID
#endif

	SetClientEvent (xyspeed);

	SetClientEffects ();

	SetClientSound ();

	SetClientFrame (xyspeed);

	Client.OldVelocity = Velocity;
	Client.OldViewAngles = Client.PlayerState.GetViewAngles();

	// clear weapon kicks
	Client.KickOrigin.Clear();
	Client.KickAngles.Clear();

	if ((
#if CLEANCTF_ENABLED
		(Game.GameMode & GAME_CTF) || 
#endif
		DeathmatchFlags.dfDmTechs.IsEnabled()) && (Client.Persistent.Tech && (Client.Persistent.Tech->TechType == CTech::TECH_PASSIVE)))
		Client.Persistent.Tech->DoPassiveTech (this);

	// if the scoreboard is up, update it
	if ((Client.LayoutFlags & LF_SHOWSCORES) && !(Level.Frame & 31) )
		DeathmatchScoreboardMessage (false);
	else if (Client.Respawn.MenuState.InMenu && !(Level.Frame & 4))
		Client.Respawn.MenuState.CurrentMenu->Draw (false);
	
	PlayerEvents::PlayerEndServerFrame(this);
}

#if CLEANCTF_ENABLED
void CPlayerEntity::CTFDeadDropFlag ()
{
	if (Client.Persistent.Flag)
	{
		Client.Persistent.Flag->DropItem (this);
		Client.Persistent.Inventory.Set (Client.Persistent.Flag, 0);
		BroadcastPrintf (PRINT_HIGH, "%s lost the %s flag!\n", Client.Persistent.Name.CString(), CTFTeamName(Client.Persistent.Flag->team));
		Client.Persistent.Flag = NULL;
	}
}

/*
==================
CTFScoreboardMessage
==================
*/
void CPlayerEntity::CTFScoreboardMessage (bool reliable)
{
	CStatusBar				Bar;
	size_t					len;
	static sint32			sorted[2][MAX_CS_CLIENTS];
	static sint32			sortedscores[2][MAX_CS_CLIENTS];
	static sint32			Score, total[2], totalscore[2];
	sint32					last[2];
	sint32					n, j, k;
	CTempMemoryBlock		str = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);
	sint32					team;

	// sort the clients by team and Score
	total[0] = total[1] = 0;
	last[0] = last[1] = 0;
	totalscore[0] = totalscore[1] = 0;
	for (sint32 i = 0; i < Game.MaxClients; i++)
	{
		CPlayerEntity *cl_ent = entity_cast<CPlayerEntity>((Game.Entities + 1 + i)->Entity);
		if (!cl_ent->GetInUse())
			continue;
		if (cl_ent->Client.Respawn.CTF.Team == CTF_TEAM1)
			team = 0;
		else if (cl_ent->Client.Respawn.CTF.Team == CTF_TEAM2)
			team = 1;
		else
			continue; // unknown team?

		Score = cl_ent->Client.Respawn.Score;
		for (j = 0; j < total[team]; j++)
		{
			if (Score > sortedscores[team][j])
				break;
		}
		for (k = total[team]; k > j; k--)
		{
			sorted[team][k] = sorted[team][k-1];
			sortedscores[team][k] = sortedscores[team][k-1];
		}
		sorted[team][j] = i;
		sortedscores[team][j] = Score;
		totalscore[team] += Score;
		total[team]++;
	}

	// print level name and exit rules
	// add the clients in sorted order
	len = 0;

	// team one
	// Team 1
	Bar.AddIf (STAT_CTF_TEAM1_HEADER);
		Bar.AddVirtualPoint_X (8);
		Bar.AddVirtualPoint_Y (8);
		Bar.AddPicStat (STAT_CTF_TEAM1_HEADER);
	Bar.AddEndIf ();
	Bar.AddVirtualPoint_X (40);
	Bar.AddVirtualPoint_Y (28);
	Q_snprintfz (str.GetBuffer<char>(), str.GetSize(), "%4d/%-3d", totalscore[0], total[0]);
	Bar.AddString (str.GetBuffer<char>(), false, false);
	Bar.AddVirtualPoint_X (98);
	Bar.AddVirtualPoint_Y (12);
	Bar.AddNumStat (STAT_CTF_TEAM1_CAPS, 2);

	// Team2
	Bar.AddIf (STAT_CTF_TEAM2_HEADER);
		Bar.AddVirtualPoint_X (168);
		Bar.AddVirtualPoint_Y (8);
		Bar.AddPicStat (STAT_CTF_TEAM2_HEADER);
	Bar.AddEndIf ();
	Bar.AddVirtualPoint_X (200);
	Bar.AddVirtualPoint_Y (28);
	Q_snprintfz (str.GetBuffer<char>(), str.GetSize(), "%4d/%-3d", totalscore[1], total[1]);
	Bar.AddString (str.GetBuffer<char>(), false, false);
	Bar.AddVirtualPoint_X (256);
	Bar.AddVirtualPoint_Y (12);
	Bar.AddNumStat (STAT_CTF_TEAM2_CAPS, 2);

	len = Bar.Length ();

	for (sint32 i = 0; i < 16; i++)
	{
		if (i >= total[0] && i >= total[1])
			break; // we're done

		// left side
		if (i < total[0])
		{
			CPlayerEntity *cl_ent = entity_cast<CPlayerEntity>((Game.Entities + 1 + sorted[0][i])->Entity);

			Bar.AddClientBlock (0, 42 + i * 8, sorted[0][i], cl_ent->Client.Respawn.Score, Clamp<sint32>(cl_ent->Client.GetPing(), 0, 999));

			if (cl_ent->Client.Persistent.Flag == NItems::BlueFlag)
			{
				Bar.AddVirtualPoint_X (56);
				Bar.AddVirtualPoint_Y (42 + i * 8);
				Bar.AddPic ("sbfctf2");
			}

			len = Bar.Length ();
			last[0] = i;
		}

		// right side
		if (i < total[1])
		{
			CPlayerEntity *cl_ent = entity_cast<CPlayerEntity>((Game.Entities + 1 + sorted[1][i])->Entity);

			Bar.AddClientBlock (160, 42 + i * 8, sorted[1][i], cl_ent->Client.Respawn.Score, Clamp<sint32>(cl_ent->Client.GetPing(), 0, 999));

			if (cl_ent->Client.Persistent.Flag == NItems::RedFlag)
			{
				Bar.AddVirtualPoint_X (216);
				Bar.AddVirtualPoint_Y (42 + i * 8);
				Bar.AddPic ("sbfctf1");
			}

			len = Bar.Length ();
			last[1] = i;
		}
	}

	// put in spectators if we have enough room
	if (last[0] > last[1])
		j = last[0];
	else
		j = last[1];
	j = (j + 2) * 8 + 42;

	k = n = 0;
	if ((MAX_COMPRINT / 4) - len > 50)
	{
		for (sint32 i = 0; i < Game.MaxClients; i++)
		{
			CPlayerEntity *cl_ent = entity_cast<CPlayerEntity>((Game.Entities + 1 + i)->Entity);
			if (!cl_ent->GetInUse() ||
				cl_ent->GetSolid() != SOLID_NOT ||
				cl_ent->Client.Respawn.CTF.Team != CTF_NOTEAM)
				continue;

			if (!k)
			{
				k = 1;
				Bar.AddVirtualPoint_X (0);
				Bar.AddVirtualPoint_Y (j);
				Bar.AddString ("Spectators", true, false);
				len = Bar.Length();
				j += 8;
			}

			Bar.AddClientBlock ((n & 1) ? 160 : 0, j, i, cl_ent->Client.Respawn.Score, Clamp<sint32>(cl_ent->Client.GetPing(), 0, 999));
			len = Bar.Length();
			
			if (n & 1)
				j += 8;
			n++;
		}
	}

	if (total[0] - last[0] > 1) // couldn't fit everyone
	{
		Bar.AddVirtualPoint_X (8);
		Bar.AddVirtualPoint_Y (42 + (last[0]+1)*8);
		Q_snprintfz (str.GetBuffer<char>(), str.GetSize(), "..and %d more", total[0] - last[0] - 1);
		Bar.AddString (str.GetBuffer<char>(), false, false);
	}
	if (total[1] - last[1] > 1) // couldn't fit everyone
	{
		Bar.AddVirtualPoint_X (168);
		Bar.AddVirtualPoint_Y (42 + (last[1]+1)*8);
		Q_snprintfz (str.GetBuffer<char>(), str.GetSize(), "..and %d more", total[1] - last[1] - 1);
		Bar.AddString (str.GetBuffer<char>(), false, false);
	}

	Bar.SendMsg (this, reliable);
}
#endif

/*
==================
DeathmatchScoreboardMessage
==================
*/
void CPlayerEntity::DeathmatchScoreboardMessage (bool reliable)
{
#if CLEANCTF_ENABLED
//ZOID
	if (Game.GameMode & GAME_CTF)
	{
		CTFScoreboardMessage (reliable);
		return;
	}
//ZOID
#endif

	CStatusBar				Scoreboard;
	static sint32			sorted[MAX_CS_CLIENTS];
	static sint32			sortedscores[MAX_CS_CLIENTS];
	sint32					Score, total;
	CPlayerEntity			*Killer = (Enemy && (Enemy->EntityFlags & EF_PLAYER)) ? entity_cast<CPlayerEntity>(*Enemy) : NULL;

	// sort the clients by Score
	total = 0;
	for (sint32 i = 0; i < Game.MaxClients ; i++)
	{
		CPlayerEntity *cl_ent = entity_cast<CPlayerEntity>((Game.Entities + 1 + i)->Entity);
		if (!cl_ent->GetInUse() || cl_ent->Client.Respawn.Spectator)
			continue;
		Score = cl_ent->Client.Respawn.Score;
		sint32 j = 0;
		for ( ; j < total; j++)
		{
			if (Score > sortedscores[j])
				break;
		}
		sint32 k = total;
		for ( ; k > j; k--)
		{
			sorted[k] = sorted[k-1];
			sortedscores[k] = sortedscores[k-1];
		}
		sorted[j] = i;
		sortedscores[j] = Score;
		total++;
	}

	// add the clients in sorted order
	if (total > 12)
		total = 12;

	for (sint32 i = 0; i < total; i++)
	{
		sint32		x, y;
		const char	*tag = NULL;
		CPlayerEntity *cl_ent = entity_cast<CPlayerEntity>((Game.Entities + 1 + sorted[i])->Entity);

		if (!cl_ent)
			continue;

		x = (i>=6) ? 160 : 0;
		y = 32 + 32 * (i%6);

		// add a dogtag
		if (cl_ent == this)
			tag = "tag1";
		else if (cl_ent == Killer)
			tag = "tag2";

		if (tag)
		{
			Scoreboard.AddVirtualPoint_X (x+32);
			Scoreboard.AddVirtualPoint_Y (y);
			Scoreboard.AddPic (tag);
		}

		// send the layout
		Scoreboard.AddClientBlock (x, y, sorted[i], cl_ent->Client.Respawn.Score, cl_ent->Client.GetPing(), (Level.Frame - cl_ent->Client.Respawn.EnterFrame)/600);
	}

	Scoreboard.SendMsg (this, reliable);
}

void CPlayerEntity::SetStats ()
{
	if (CvarList[CV_MAP_DEBUG].Boolean())
	{
		Client.PlayerState.GetStat (STAT_PICKUP_STRING) = CS_POINTING_SURFACE;
		Client.PlayerState.GetStat (STAT_TIMER_ICON) = CS_POINTING_SURFACE-1;
	}
	else
	{
		//
		// health
		//
		Client.PlayerState.GetStat (STAT_HEALTH_ICON) = GameMedia.Hud.HealthPic;
		Client.PlayerState.GetStat (STAT_HEALTH) = Health;

		//
		// ammo
		//
		if (Client.Persistent.Weapon)
		{
			if (Client.Persistent.Weapon->Item->Ammo)
			{
				Client.PlayerState.GetStat (STAT_AMMO_ICON) = Client.Persistent.Weapon->Item->Ammo->GetIconIndex();
				Client.PlayerState.GetStat (STAT_AMMO) = Client.Persistent.Inventory.Has(Client.Persistent.Weapon->Item->Ammo->GetIndex());
			}
			else
			{
				Client.PlayerState.GetStat (STAT_AMMO_ICON) = 0;
				Client.PlayerState.GetStat (STAT_AMMO) = 0;
			}
		}
		else
		{
			Client.PlayerState.GetStat (STAT_AMMO_ICON) = 0;
			Client.PlayerState.GetStat (STAT_AMMO) = 0;
		}
		
		//
		// armor
		//
		sint32			cells = 0;
		sint32			power_armor_type = PowerArmorType ();
		if (power_armor_type)
		{
			cells = Client.Persistent.Inventory.Has(NItems::Cells);
			if (cells == 0)
			{
				// ran out of cells for power armor
				Client.PowerArmorEnabled = false;
				PlaySound (CHAN_ITEM, SoundIndex("misc/power2.wav"));
				power_armor_type = 0;
			}
		}

		CArmor *Armor = Client.Persistent.Armor;
		if (power_armor_type && (!Armor || (Level.Frame & 8) ) )
		{	// flash between power armor and other armor icon
			Client.PlayerState.GetStat (STAT_ARMOR_ICON) = NItems::PowerShield->GetIconIndex();
			Client.PlayerState.GetStat (STAT_ARMOR) = cells;
		}
		else if (Armor)
		{
			Client.PlayerState.GetStat (STAT_ARMOR_ICON) = Armor->GetIconIndex();
			Client.PlayerState.GetStat (STAT_ARMOR) = Client.Persistent.Inventory.Has(Armor);
		}
		else
		{
			Client.PlayerState.GetStat (STAT_ARMOR_ICON) = 0;
			Client.PlayerState.GetStat (STAT_ARMOR) = 0;
		}

		//
		// pickup message
		//
		if (Level.Frame > Client.Timers.PickupMessageTime)
		{
			Client.PlayerState.GetStat (STAT_PICKUP_ICON) = 0;
			Client.PlayerState.GetStat (STAT_PICKUP_STRING) = 0;
		}

		//
		// timers
		//
		if (Client.Timers.QuadDamage > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::Quad->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.QuadDamage - Level.Frame)/10;
		}
#if ROGUE_FEATURES
		else if (Client.Timers.Double > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::Double->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.Double - Level.Frame)/10;
		}
#endif
#if XATRIX_FEATURES
		else if (Client.Timers.QuadFire > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::QuadFire->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.QuadFire - Level.Frame)/10;
		}
#endif
		else if (Client.Timers.Invincibility > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::Invul->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.Invincibility - Level.Frame)/10;
		}
		else if (Client.Timers.EnvironmentSuit > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::EnvironmentSuit->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.EnvironmentSuit - Level.Frame)/10;
		}
		else if (Client.Timers.Rebreather > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::Rebreather->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.Rebreather - Level.Frame)/10;
		}
#if ROGUE_FEATURES
		else if (Client.OwnedSphere)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = Client.OwnedSphere->Item->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.OwnedSphere->Wait - Level.Frame) / 10;
		}
		else if (Client.Timers.IR > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::IRGoggles->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.IR - Level.Frame)/10;
		}
#endif
		// Paril, show silencer
		else if (Client.Timers.SilencerShots)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::Silencer->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = Client.Timers.SilencerShots;
		}
		// Paril
		else
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = 0;
			Client.PlayerState.GetStat (STAT_TIMER) = 0;
		}

		//
		// selected item
		//
		Client.PlayerState.GetStat (STAT_SELECTED_ICON) = (Client.Persistent.Inventory.SelectedItem == -1) ? 0 : GetItemByIndex(Client.Persistent.Inventory.SelectedItem)->GetIconIndex();
		Client.PlayerState.GetStat (STAT_SELECTED_ITEM) = Client.Persistent.Inventory.SelectedItem;

		//
		// layouts
		//
		Client.PlayerState.GetStat (STAT_LAYOUTS) = 0;

		if (Client.Persistent.Health <= 0 || Client.Respawn.MenuState.InMenu ||
			(Level.Intermission.Time || (Client.LayoutFlags & LF_SHOWSCORES)) || 
			(!(Game.GameMode & GAME_DEATHMATCH)) && (Client.LayoutFlags & LF_SHOWHELP))
			Client.PlayerState.GetStat (STAT_LAYOUTS) = Client.PlayerState.GetStat(STAT_LAYOUTS) | 1;
		if ((Client.LayoutFlags & LF_SHOWINVENTORY) && Client.Persistent.Health > 0)
			Client.PlayerState.GetStat (STAT_LAYOUTS) = Client.PlayerState.GetStat(STAT_LAYOUTS) | 2;

		//
		// frags
		//
		Client.PlayerState.GetStat (STAT_FRAGS) = Client.Respawn.Score;

		//
		// help icon / current weapon if not shown
		//
		Client.PlayerState.GetStat (STAT_HELPICON) = (Client.Persistent.HelpChanged && (Level.Frame&8) ) ? GameMedia.Hud.HelpPic :
			(((Client.Persistent.Hand == CENTER_HANDED || Client.PlayerState.GetFov() > 91)
			&& Client.Persistent.Weapon && Client.Persistent.Weapon->Item) ? Client.Persistent.Weapon->Item->GetIconIndex() : 0);

		Client.PlayerState.GetStat (STAT_SPECTATOR) = 0;
		Client.PlayerState.GetStat (STAT_TECH) = 0;

		if (Client.Persistent.Tech)
			Client.PlayerState.GetStat (STAT_TECH) = Client.Persistent.Tech->GetIconIndex();

	#if CLEANCTF_ENABLED
	//ZOID
		if (Game.GameMode & GAME_CTF)
			SetCTFStats();
	//ZOID
	#endif
	}
}

/*
===============
G_SetSpectatorStats
===============
*/
void CPlayerEntity::SetSpectatorStats ()
{
	if (!Client.Chase.Target)
		SetStats ();

	Client.PlayerState.GetStat (STAT_SPECTATOR) = 1;

	// layouts are independant in Spectator
	Client.PlayerState.GetStat (STAT_LAYOUTS) = 0;

	if (Client.Persistent.Health <= 0 || Level.Intermission.Time || (Client.LayoutFlags & LF_SHOWSCORES))
		Client.PlayerState.GetStat (STAT_LAYOUTS) = Client.PlayerState.GetStat(STAT_LAYOUTS) | 1;
	if ((Client.LayoutFlags & LF_SHOWINVENTORY) && Client.Persistent.Health > 0)
		Client.PlayerState.GetStat (STAT_LAYOUTS) = Client.PlayerState.GetStat(STAT_LAYOUTS) | 2;

	Client.PlayerState.GetStat (STAT_CHASE) = 
		(Client.Chase.Target && Client.Chase.Target->GetInUse()) ? (CS_PLAYERSKINS + (Client.Chase.Target->State.GetNumber() - 1)) : 0;
}

#if CLEANCTF_ENABLED
void CPlayerEntity::SetCTFStats()
{
	// logo headers for the frag display
	Client.PlayerState.GetStat (STAT_CTF_TEAM1_HEADER) = ImageIndex ("ctfsb1");
	Client.PlayerState.GetStat (STAT_CTF_TEAM2_HEADER) = ImageIndex ("ctfsb2");

	// if during intermission, we must blink the team header of the winning team
	if (Level.Intermission.Time && (Level.Frame & 8))
	{
		// blink 1/8th second
		// note that ctfgame.total[12] is set when we go to intermission
		if (ctfgame.Captures[CTF_TEAM1] > ctfgame.Captures[CTF_TEAM2])
			Client.PlayerState.GetStat (STAT_CTF_TEAM1_HEADER) = 0;
		else if (ctfgame.Captures[CTF_TEAM2] > ctfgame.Captures[CTF_TEAM1])
			Client.PlayerState.GetStat (STAT_CTF_TEAM2_HEADER) = 0;
		else if (ctfgame.TotalScore[CTF_TEAM1] > ctfgame.TotalScore[CTF_TEAM2]) // frag tie breaker
			Client.PlayerState.GetStat (STAT_CTF_TEAM1_HEADER) = 0;
		else if (ctfgame.TotalScore[CTF_TEAM2] > ctfgame.TotalScore[CTF_TEAM1]) 
			Client.PlayerState.GetStat (STAT_CTF_TEAM2_HEADER) = 0;
		else
		{
			// tie game!
			Client.PlayerState.GetStat (STAT_CTF_TEAM1_HEADER) = 0;
			Client.PlayerState.GetStat (STAT_CTF_TEAM2_HEADER) = 0;
		}
	}

	// figure out what icon to display for team logos
	// three states:
	//   flag at base
	//   flag taken
	//   flag dropped

	CFlagTransponder *Transponder = FindTransponder(CTF_TEAM1);
	if (!Transponder)
		Client.PlayerState.GetStat (STAT_CTF_TEAM1_PIC) = ImageIndex ("i_ctf1");
	else
	{
		switch (Transponder->Location)
		{
		case CFlagTransponder::FLAG_AT_BASE:
		default:
			Client.PlayerState.GetStat (STAT_CTF_TEAM1_PIC) = ImageIndex ("i_ctf1");
			break;
		case CFlagTransponder::FLAG_DROPPED:
			Client.PlayerState.GetStat (STAT_CTF_TEAM1_PIC) = ImageIndex ("i_ctf1d");
			break;
		case CFlagTransponder::FLAG_TAKEN:
			Client.PlayerState.GetStat (STAT_CTF_TEAM1_PIC) = ImageIndex ("i_ctf1t");
			break;
		};
	}

	Transponder = FindTransponder(CTF_TEAM2);
	if (!Transponder)
		Client.PlayerState.GetStat (STAT_CTF_TEAM2_PIC) = ImageIndex ("i_ctf2");
	else
	{
		switch (Transponder->Location)
		{
		case CFlagTransponder::FLAG_AT_BASE:
		default:
			Client.PlayerState.GetStat (STAT_CTF_TEAM2_PIC) = ImageIndex ("i_ctf2");
			break;
		case CFlagTransponder::FLAG_DROPPED:
			Client.PlayerState.GetStat (STAT_CTF_TEAM2_PIC) = ImageIndex ("i_ctf2d");
			break;
		case CFlagTransponder::FLAG_TAKEN:
			Client.PlayerState.GetStat (STAT_CTF_TEAM2_PIC) = ImageIndex ("i_ctf2t");
			break;
		};
	}

	if (ctfgame.LastFlagCaptureTime && Level.Frame - ctfgame.LastFlagCaptureTime < 50)
	{
		if (ctfgame.LastCaptureTeam == CTF_TEAM1)
		{
			if (!(Level.Frame & 8))
				Client.PlayerState.GetStat (STAT_CTF_TEAM1_PIC) = 0;
		}
		else
		{
			if (!(Level.Frame & 8))
				Client.PlayerState.GetStat (STAT_CTF_TEAM2_PIC) = 0;
		}
	}

	Client.PlayerState.GetStat (STAT_CTF_TEAM1_CAPS) = ctfgame.Captures[CTF_TEAM1];
	Client.PlayerState.GetStat (STAT_CTF_TEAM2_CAPS) = ctfgame.Captures[CTF_TEAM2];

	Client.PlayerState.GetStat (STAT_CTF_FLAG_PIC) = 0;
	if (Client.Respawn.CTF.Team == CTF_TEAM1 &&
		(Client.Persistent.Flag == NItems::BlueFlag) &&
		(Level.Frame & 8))
		Client.PlayerState.GetStat (STAT_CTF_FLAG_PIC) = ImageIndex ("i_ctf2");

	else if (Client.Respawn.CTF.Team == CTF_TEAM2 &&
		(Client.Persistent.Flag == NItems::RedFlag) &&
		(Level.Frame & 8))
		Client.PlayerState.GetStat (STAT_CTF_FLAG_PIC) = ImageIndex ("i_ctf1");

	Client.PlayerState.GetStat (STAT_CTF_JOINED_TEAM1_PIC) = 0;
	Client.PlayerState.GetStat (STAT_CTF_JOINED_TEAM2_PIC) = 0;

	if (Client.Respawn.CTF.Team == CTF_TEAM1)
		Client.PlayerState.GetStat (STAT_CTF_JOINED_TEAM1_PIC) = ImageIndex ("i_ctfj");
	else if (Client.Respawn.CTF.Team == CTF_TEAM2)
		Client.PlayerState.GetStat (STAT_CTF_JOINED_TEAM2_PIC) = ImageIndex ("i_ctfj");

	Client.PlayerState.GetStat (STAT_CTF_ID_VIEW) = 0;
	if (Client.Respawn.CTF.IDState)
		CTFSetIDView();
}

void CPlayerEntity::CTFSetIDView()
{
	Client.PlayerState.GetStat (STAT_CTF_ID_VIEW) = 0;

	vec3f oldForward;
	anglef angles = Client.ViewAngle.ToVectors();
	oldForward = angles.Forward;
	angles.Forward = (angles.Forward * 1024) + State.GetOrigin();

	CTrace tr (State.GetOrigin(), angles.Forward, this, CONTENTS_MASK_SOLID);
	if (tr.Fraction < 1 && tr.Entity && (tr.Entity->State.GetNumber() >= 1 && tr.Entity->State.GetNumber() <= Game.MaxClients))
	{
		Client.PlayerState.GetStat (STAT_CTF_ID_VIEW) = CS_PLAYERSKINS + (State.GetNumber() - 1);
		return;
	}

	angles.Forward = oldForward;
	CPlayerEntity *best = NULL;
	float bd = 0;
	for (sint32 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *who = entity_cast<CPlayerEntity>((Game.Entities + i)->Entity);
		if (!who->GetInUse() || who->GetSolid() == SOLID_NOT)
			continue;

		float d = angles.Forward | (who->State.GetOrigin() - State.GetOrigin()).GetNormalizedFast ();
		if (d > bd && CanSee(who))
		{
			bd = d;
			best = who;
		}
	}
	if (bd > 0.90)
		Client.PlayerState.GetStat (STAT_CTF_ID_VIEW) = CS_PLAYERSKINS + (best->State.GetNumber() - 1);
}
#endif

void CPlayerEntity::MoveToIntermission ()
{
	if (!(Game.GameMode & GAME_SINGLEPLAYER))
		Client.LayoutFlags |= LF_SHOWSCORES;

	State.GetOrigin() = Level.Intermission.Origin;

	Client.PlayerState.GetPMove()->Origin = (Level.Intermission.Origin * 8);
	Client.PlayerState.GetViewAngles() = Level.Intermission.Angles;
	Client.PlayerState.GetPMove()->PMoveType = PMT_FREEZE;
	Client.PlayerState.GetGunIndex () = 0;

	Client.PlayerState.GetViewBlend ().A = 0;
	Client.PlayerState.GetRdFlags () &= ~RDF_UNDERWATER;

	// clean up powerup info
	Client.Timers.QuadDamage = 0;
	Client.Timers.Invincibility = 0;
	Client.Timers.Rebreather = 0;
	Client.Timers.EnvironmentSuit = 0;

#if ROGUE_FEATURES
	Client.PlayerState.GetRdFlags() &= ~RDF_IRGOGGLES;		// PGM
	Client.Timers.IR = 0;					// PGM
	Client.Timers.Nuke = 0;					// PMM
	Client.Timers.Double = 0;				// PMM
#endif

#if XATRIX_FEATURES
	Client.Timers.QuadFire = 0;
#endif
	Client.Grenade.BlewUp = Client.Grenade.Thrown = false;
	Client.Grenade.Time = 0;

	ViewHeight = 0;
	State.GetModelIndex() = State.GetModelIndex(2) = State.GetModelIndex(3) = 0;
	State.GetEffects() = 0;
	State.GetSound() = 0;
	GetSolid() = SOLID_NOT;

	// add the layout
	Enemy = nullentity;
	if (!(Game.GameMode & GAME_SINGLEPLAYER))
		DeathmatchScoreboardMessage (true);
}

bool CPlayerEntity::ApplyStrengthSound()
{
	if (Client.Persistent.Tech && (Client.Persistent.Tech->GetTechNumber() == CTFTECH_STRENGTH_NUMBER))
	{
		if (Client.Tech.SoundTime < Level.Frame)
		{
			Client.Tech.SoundTime = Level.Frame + 10;
			PlaySound (CHAN_AUTO, SoundIndex((Client.Timers.QuadDamage > Level.Frame) ? "ctf/tech2x.wav" : "ctf/tech2.wav"), (Client.Timers.SilencerShots) ? 51 : 255);
		}
		return true;
	}
	return false;
}

bool CPlayerEntity::ApplyHaste()
{
	return (Client.Persistent.Tech && (Client.Persistent.Tech->GetTechNumber() == CTFTECH_HASTE_NUMBER));
}

void CPlayerEntity::ApplyHasteSound()
{
	if (Client.Persistent.Tech && (Client.Persistent.Tech->GetTechNumber() == CTFTECH_HASTE_NUMBER) && Client.Tech.SoundTime < Level.Frame)
	{
		Client.Tech.SoundTime = Level.Frame + 10;
		PlaySound (CHAN_AUTO, SoundIndex("ctf/tech3.wav"), (Client.Timers.SilencerShots) ? 51 : 255);
	}
}

bool CPlayerEntity::HasRegeneration()
{
	return (Client.Persistent.Tech && (Client.Persistent.Tech->GetTechNumber() == CTFTECH_REGEN_NUMBER));
}


/*
==================
LookAtKiller
==================
*/
void CPlayerEntity::LookAtKiller (IBaseEntity *Inflictor, IBaseEntity *Attacker)
{
	vec3f dir;
	if (Attacker && (Attacker != World) && (Attacker != this))
		dir = Attacker->State.GetOrigin() - State.GetOrigin();
	else if (Inflictor && (Inflictor != World) && (Inflictor != this))
		dir = Inflictor->State.GetOrigin() - State.GetOrigin();
	else
	{
		Client.KillerYaw = State.GetAngles().Y;
		return;
	}

	Client.KillerYaw = (dir.X) ? (180 / M_PI * atan2f(dir.Y, dir.X)) : (((dir.Y > 0) ? 90 : (dir.Y < 0 ? 270 : 0)));
	if (Client.KillerYaw < 0)
		Client.KillerYaw += 360;
}

void CPlayerEntity::DeadDropTech ()
{
	if (!Client.Persistent.Tech)
		return;

	CItemEntity *dropped = Client.Persistent.Tech->DropItem(this);
	// hack the velocity to make it bounce random
	dropped->Velocity.X = (frand() * 600) - 300;
	dropped->Velocity.Y = (frand() * 600) - 300;
	dropped->NextThink = Level.Frame + CTF_TECH_TIMEOUT;
	dropped->SetOwner (NULL);
	Client.Persistent.Inventory.Set(Client.Persistent.Tech, 0);

	Client.Persistent.Tech = NULL;
}

void CPlayerEntity::TossClientWeapon ()
{
	if (!(Game.GameMode & GAME_DEATHMATCH))
		return;

	CWeaponItem *Item = (Client.Persistent.Weapon) ? Client.Persistent.Weapon->Item : NULL;

	if (Item)
	{
		if (!Item->Ammo)
			Item = NULL;
		if (Item && !Client.Persistent.Inventory.Has(Item->Ammo))
			Item = NULL;
		if (Item && !Item->WorldModel)
			Item = NULL;
	}

	bool quad = (!DeathmatchFlags.dfQuadDrop.IsEnabled()) ? false : (bool)(Client.Timers.QuadDamage > (Level.Frame + 10));
#if XATRIX_FEATURES
	bool quadfire = (!DeathmatchFlags.dfQuadFireDrop.IsEnabled()) ? false : (bool)(Client.Timers.QuadFire > (Level.Frame + 10));
#endif
	float spread = (Item && quad) ? 22.5f : 0.0f;

	if (Item)
	{
		Client.ViewAngle.Y -= spread;
		CItemEntity *drop = Item->DropItem (this);
		Client.ViewAngle.Y += spread;
		drop->SpawnFlags |= DROPPED_PLAYER_ITEM;
		drop->AmmoCount = Client.Persistent.Weapon->Item->Ammo->Quantity;
	}

	if (quad)
	{
		Client.ViewAngle.Y += spread;
		CItemEntity *drop = NItems::Quad->DropItem (this);
		Client.ViewAngle.Y -= spread;
		drop->SpawnFlags |= DROPPED_PLAYER_ITEM;

		drop->NextThink = Level.Frame + (Client.Timers.QuadDamage - Level.Frame);
		drop->ThinkState = ITS_FREE;
	}

#if XATRIX_FEATURES
	if (quadfire)
	{
		Client.ViewAngle.Y += spread;
		CItemEntity *drop = NItems::QuadFire->DropItem (this);
		Client.ViewAngle.Y -= spread;
		drop->SpawnFlags |= DROPPED_PLAYER_ITEM;

		drop->NextThink = Level.Frame + (Client.Timers.QuadFire - Level.Frame);
		drop->ThinkState = ITS_FREE;
	}
#endif
}

#if 1
CPlayerEntity	*pm_passent;

// pmove doesn't need to know about passent and contentmask
STrace	PM_trace (float *start, float *mins, float *maxs, float *end)
{
CC_DISABLE_DEPRECATION
	return gi.trace(start, mins, maxs, end, pm_passent->GetGameEntity(), (pm_passent->Health > 0) ? CONTENTS_MASK_PLAYERSOLID : CONTENTS_MASK_DEADSOLID);
CC_ENABLE_DEPRECATION
}
#endif

/**
\fn	void CPlayerEntity::TouchTriggers ()

\brief	Cause all triggers that this entity is touching to activate.
**/
void CPlayerEntity::TouchTriggers ()
{
	// dead things don't activate triggers!
	if (CanTakeDamage && (Health <= 0))
		return;

	IBaseEntity::TouchTriggers();
}

void CPlayerEntity::ClientThink (SUserCmd *ucmd)
{
#if 1
	SPMove		pm;
#else
	pMoveNew_t	pm;
#endif

	Level.CurrentEntity = this;

	if (Level.Intermission.Time)
	{
		Client.PlayerState.GetPMove()->PMoveType = PMT_FREEZE;
		// can exit intermission after five seconds
		if ((Level.Frame > Level.Intermission.Time + 50) && (ucmd->Buttons & BUTTON_ANY))
			Level.Intermission.ShouldExit = true;
		return;
	}

#if 1
	pm_passent = this;
#endif
	
	Client.Persistent.UserCmd = *ucmd; // Copy for module use.
//ZOID
	static sint32 oldbuttons;
	oldbuttons = Client.Buttons;
	Client.Buttons = ucmd->Buttons;
	Client.LatchedButtons |= Client.Buttons & ~oldbuttons;

	if (Client.Chase.Target)
	{
		for (sint32 i = 0; i < 3; i++)
			Client.Respawn.CmdAngles[i] = SHORT2ANGLE(ucmd->Angles[i]);

		if (Client.LatchedButtons & BUTTON_ATTACK)
		{
			if (Client.Chase.Target)
				ChaseNext();
			else
				GetChaseTarget();
			Client.LatchedButtons &= ~BUTTON_ATTACK;
		}

		if (ucmd->UpMove >= 10)
		{
			if (!(Client.PlayerState.GetPMove()->PMoveFlags & PMF_JUMP_HELD))
			{
				Client.PlayerState.GetPMove()->PMoveFlags |= PMF_JUMP_HELD;

				Client.LatchedButtons = 0;

				if (Client.Chase.Target)
				{
					Client.Chase.Target = NULL;
					Client.PlayerState.GetPMove()->PMoveFlags &= ~PMF_NO_PREDICTION;
					Client.PlayerState.GetGunIndex () = 0;
					Client.PlayerState.GetGunFrame() = 0;
				}
				else
					GetChaseTarget();
			}
		}
		else
			Client.PlayerState.GetPMove()->PMoveFlags &= ~PMF_JUMP_HELD;
		return;
	}
//ZOID

	// set up for pmove
	Mem_Zero (&pm, sizeof(pm));
 	

	if (NoClip)
		Client.PlayerState.GetPMove()->PMoveType = PMT_SPECTATOR;
	else if (State.GetModelIndex() != 255)
		Client.PlayerState.GetPMove()->PMoveType = PMT_GIB;
	else if (IsDead)
		Client.PlayerState.GetPMove()->PMoveType = PMT_DEAD;
	else
		Client.PlayerState.GetPMove()->PMoveType = PMT_NORMAL;

	Client.PlayerState.GetPMove()->Gravity = CvarList[CV_GRAVITY].Float() * GravityMultiplier;

	// Likely, before pmove is called it will be the most useful
	// Otherwise, just use EndServerFrame instead.
	PlayerEvents::PlayerThink(this);

	pm.State = *Client.PlayerState.GetPMove();

	pm.State.Origin = (State.GetOrigin() * 8);
	pm.State.Velocity = (Velocity * 8);

	if (memcmp (&Client.OldPMove, &pm.State, sizeof(pm.State)))
	pm.SnapInitial = true;

	pm.Command = *ucmd;

#if 1
	pm.Trace = PM_trace;
CC_DISABLE_DEPRECATION
	pm.PointContents = gi.pointcontents;
CC_ENABLE_DEPRECATION
#endif

	// perform a pmove
#if 1
	gi.Pmove (&pm);
#else
	SV_Pmove (this, &pm, CvarList[CV_AIRACCELERATE].Float());
#endif

	// save results of pmove
	Client.PlayerState.SetPMove (&pm.State);
	Client.OldPMove = pm.State;

	State.GetOrigin() = vec3f(pm.State.Origin) * 0.125f;
	Velocity = vec3f(pm.State.Velocity) * 0.125f;

	GetMins() = pm.Mins;
	GetMaxs() = pm.Maxs;

	Client.Respawn.CmdAngles.Set (SHORT2ANGLE(ucmd->Angles.X), SHORT2ANGLE(ucmd->Angles.Y), SHORT2ANGLE(ucmd->Angles.Z));

	if (GroundEntity && !pm.GroundEntity && Velocity[2] > 0 && (pm.Command.UpMove >= 10) && (pm.WaterLevel == WATER_NONE))
	{
		PlaySound (CHAN_VOICE, GameMedia.Player.Jump);
		PlayerNoiseAt (State.GetOrigin(), PNOISE_SELF);
	}

	ViewHeight = pm.ViewHeight;

#if ROGUE_FEATURES
		if (Flags & FL_SAM_RAIMI)
			ViewHeight = 8;
#endif

	if (WaterInfo.Level != pm.WaterLevel)
	{
		WaterInfo.OldLevel = WaterInfo.Level;
		WaterInfo.Level	= pm.WaterLevel;
	}

	WaterInfo.Type = pm.WaterType;
	GroundEntity = (pm.GroundEntity) ? pm.GroundEntity->Entity : NULL;

	if (GroundEntity)
		GroundEntityLinkCount = GroundEntity->GetLinkCount();

	if (IsDead)
		Client.PlayerState.GetViewAngles().Set (-15, Client.KillerYaw, 40);
	else
	{
		Client.ViewAngle.Set (pm.ViewAngles);
		Client.PlayerState.GetViewAngles().Set (pm.ViewAngles);
	}

#if CLEANCTF_ENABLED
//ZOID
	if (Client.Grapple.Entity)
		Client.Grapple.Entity->GrapplePull ();
//ZOID
#endif

	Link ();
	GravityMultiplier = 1.0f;

	if (!NoClip && !(CvarList[CV_MAP_DEBUG].Boolean()))
		TouchTriggers ();

	// touch other objects
	if (!CvarList[CV_MAP_DEBUG].Boolean())
	{
		for (sint32 i = 0; i < pm.NumTouch; i++)
		{
			SEntity *Other = pm.TouchEnts[i];

			if (Other->Entity)
			{
				if ((Other->Entity->EntityFlags & EF_TOUCHABLE) && Other->Entity->GetInUse())
				{
					ITouchableEntity *Touchered = entity_cast<ITouchableEntity>(Other->Entity);

					if (Touchered->Touchable)
						Touchered->Touch (this, NULL, NULL);
				}
				continue;
			}
		}
	}

	// save light level the player is standing on for
	// monster sighting AI
	// Paril: Removed. See definition of SUserCmd::lightlevel for more info.

	if (Client.Respawn.Spectator
#if CLEANCTF_ENABLED
		|| ((Game.GameMode & GAME_CTF) && NoClip)
#endif
		)
	{
		if (Client.LatchedButtons & BUTTON_ATTACK)
		{
			if (Client.Chase.Target)
				ChaseNext();
			else
				GetChaseTarget();
		}

		if (ucmd->UpMove >= 10)
		{
			if (!(Client.PlayerState.GetPMove()->PMoveFlags & PMF_JUMP_HELD))
			{
				Client.PlayerState.GetPMove()->PMoveFlags |= PMF_JUMP_HELD;

				Client.LatchedButtons = 0;

				if (Client.Chase.Target)
				{
					Client.Chase.Target = NULL;
					Client.PlayerState.GetPMove()->PMoveFlags &= ~PMF_NO_PREDICTION;
				}
				else
					GetChaseTarget();
			}
		}
		else
			Client.PlayerState.GetPMove()->PMoveFlags &= ~PMF_JUMP_HELD;
	}

	// update chase cam if being followed
	for (sint32 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *Other = entity_cast<CPlayerEntity>((Game.Entities + i)->Entity);
		if (Other->GetInUse() && Other->Client.Chase.Target == this)
			Other->UpdateChaseCam();
	}
}

#if CLEANCTF_ENABLED
void CPlayerEntity::CTFAssignTeam()
{
	sint32 i;
	sint32 team1count = 0, team2count = 0;

	Client.Respawn.CTF.State = 0;

	if (!DeathmatchFlags.dfCtfForceJoin.IsEnabled())
	{
		Client.Respawn.CTF.Team = CTF_NOTEAM;
		return;
	}

	for (i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *player = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);

		if (!player->GetInUse() || player == this)
			continue;

		switch (player->Client.Respawn.CTF.Team)
		{
		case CTF_TEAM1:
			team1count++;
			break;
		case CTF_TEAM2:
			team2count++;
		}
	}
	if (team1count < team2count)
		Client.Respawn.CTF.Team = CTF_TEAM1;
	else if (team2count < team1count)
		Client.Respawn.CTF.Team = CTF_TEAM2;
	else if (irandom(2))
		Client.Respawn.CTF.Team = CTF_TEAM1;
	else
		Client.Respawn.CTF.Team = CTF_TEAM2;
}
#endif

void CPlayerEntity::InitResp ()
{
#if CLEANCTF_ENABLED
//ZOID
	sint32 Team = Client.Respawn.CTF.Team;
	bool IDState = Client.Respawn.CTF.IDState;
//ZOID
#endif

	Client.Respawn.Clear ();

#if CLEANCTF_ENABLED
//ZOID
	Client.Respawn.CTF.Team = Team;
	Client.Respawn.CTF.IDState = IDState;
//ZOID
#endif

	Client.Respawn.EnterFrame = Level.Frame;
	Client.Respawn.CoopRespawn = Client.Persistent;

#if CLEANCTF_ENABLED
//ZOID
	if ((Game.GameMode & GAME_CTF) && Client.Respawn.CTF.Team < CTF_TEAM1)
		CTFAssignTeam();
//ZOID
#endif
}

/*
==================
SaveClientData

Some information that should be persistant, like health, 
is still stored in the edict structure, so it needs to
be mirrored out to the client structure before all the
edicts are wiped.
==================
*/
CPersistentData *SavedClients;

void CPlayerEntity::BackupClientData ()
{
	SavedClients = QNew (TAG_GENERIC) CPersistentData[Game.MaxClients];
	for (sint32 i = 0; i < Game.MaxClients; i++)
	{
		if (!Game.Entities[1+i].Entity)
			return; // Not set up

		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[1+i].Entity);

		SavedClients[i] = Player->Client.Persistent;
		SavedClients[i].Health = Player->Health;
		SavedClients[i].MaxHealth = Player->MaxHealth;
		SavedClients[i].SavedFlags = (Player->Flags & (FL_GODMODE|FL_NOTARGET));
		if (Game.GameMode & GAME_COOPERATIVE)
			SavedClients[i].Score = Player->Client.Respawn.Score;
	}
}

void CPlayerEntity::SaveClientData ()
{
	for (sint32 i = 0; i < Game.MaxClients; i++)
	{
		if (!Game.Entities[1+i].Entity)
			return; // Not set up

		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[1+i].Entity);

		Player->Client.Persistent.Health = Player->Health;
		Player->Client.Persistent.MaxHealth = Player->MaxHealth;
		Player->Client.Persistent.SavedFlags = (Player->Flags & (FL_GODMODE|FL_NOTARGET));
		if (Game.GameMode & GAME_COOPERATIVE)
			Player->Client.Persistent.Score = Player->Client.Respawn.Score;
	}
}

void CPlayerEntity::RestoreClientData ()
{
	for (sint32 i = 0; i < Game.MaxClients; i++)
	{
		// Reset the entity states
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i+1].Entity);
		Player->Client.Persistent = SavedClients[i];
		Player->Health = SavedClients[i].Health;
		Player->MaxHealth = SavedClients[i].MaxHealth;
		Player->Flags = SavedClients[i].SavedFlags;
		if (Game.GameMode & GAME_COOPERATIVE)
			Player->Client.Respawn.Score = SavedClients[i].Score;
		Game.Entities[i+1].Server.Client = Game.Clients + i;
	}

	QDelete[] SavedClients;
	SavedClients = NULL;
}

void CPlayerEntity::TossHead (sint32 Damage)
{
	if (irandom(2))
	{
		State.GetModelIndex() = GameMedia.Gib_Head[1];
		State.GetSkinNum() = 1;		// second skin is player
	}
	else
	{
		State.GetModelIndex() = GameMedia.Gib_Skull;
		State.GetSkinNum() = 0;
	}

	State.GetFrame() = 0;
	GetMins().Set (-16, -16, 0);
	GetMaxs().Set (16);

	CanTakeDamage = false;
	GetSolid() = SOLID_NOT;
	State.GetEffects() = FX_GIB;
	State.GetSound() = 0;

	Velocity += VelocityForDamage (Damage);

	Link ();
}

EMeansOfDeath meansOfDeath;

void CPlayerEntity::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	CanTakeDamage = true;
	TossPhysics = true;

	State.GetModelIndex(2) = State.GetModelIndex(3) = 0;	// remove linked weapon model and ctf flag

	State.GetAngles().Set (0, State.GetAngles().Y, 0);

	State.GetSound() = 0;
	Client.WeaponSound = 0;

	GetMaxs().Z = -8;

	GetSvFlags() |= SVF_DEADMONSTER;

	if (!IsDead)
	{
		Client.Timers.RespawnTime = Level.Frame + 10;
		LookAtKiller (Inflictor, Attacker);
		Client.PlayerState.GetPMove()->PMoveType = PMT_DEAD;
		Obituary (Attacker);

#if CLEANCTF_ENABLED
		if (Attacker && (Attacker->EntityFlags & EF_PLAYER))
		{
			CPlayerEntity *PlayerAttacker = entity_cast<CPlayerEntity>(Attacker);

			// if at start and same team, clear
			if ((Game.GameMode & GAME_CTF) && (meansOfDeath == MOD_TELEFRAG) &&
				(Client.Respawn.CTF.State < 2) &&
				(Client.Respawn.CTF.Team == PlayerAttacker->Client.Respawn.CTF.Team))
			{
				PlayerAttacker->Client.Respawn.Score--;
				Client.Respawn.CTF.State = 0;
			}

			if (Game.GameMode & GAME_CTF)
				CTFFragBonuses(this, PlayerAttacker);
		}
#endif
		TossClientWeapon ();

#if CLEANCTF_ENABLED
		if ((Game.GameMode & GAME_CTF) || Client.Grapple.Entity || Client.Persistent.Flag)
		{
			CGrapple::PlayerResetGrapple(this);
			CTFDeadDropFlag();
		}
#endif
		if (
#if CLEANCTF_ENABLED
			(Game.GameMode & GAME_CTF) || 
#endif
			DeathmatchFlags.dfDmTechs.IsEnabled()) 
			DeadDropTech();

		if (Game.GameMode & GAME_DEATHMATCH)
			ShowHelp ();		// show scores

		// clear inventory
		// this is kind of ugly, but it's how we want to handle keys in coop
		for (sint32 n = 0; n < GetNumItems(); n++)
		{
			if ((Game.GameMode & GAME_COOPERATIVE) && (GetItemByIndex(n)->Flags & ITEMFLAG_KEY))
				Client.Respawn.CoopRespawn.Inventory.Set(n, Client.Persistent.Inventory.Has(n));
			Client.Persistent.Inventory.Set(n, 0);
		}
	}

	// remove powerups
	Client.Timers.QuadDamage = 0;
	Client.Timers.Invincibility = 0;
	Client.Timers.Rebreather = 0;
	Client.Timers.EnvironmentSuit = 0;
	Client.PowerArmorEnabled = false;

#if ROGUE_FEATURES
	Client.Timers.Double = 0;

	// if there's a sphere around, let it know the player died.
	// vengeance and hunter will die if they're not attacking,
	// defender should always die
	if (Client.OwnedSphere)
		Client.OwnedSphere->Die (this, this, 0, vec3fOrigin);

	// if we've been killed by the tracker, GIB!
	if ((meansOfDeath & ~MOD_FRIENDLY_FIRE) == MOD_TRACKER)
	{
		Health = -100;
		Damage = 400;
	}

	// make sure no trackers are still hurting us.
	if (Client.Timers.Tracker)
		RemoveAttackingPainDaemons ();
#endif
	
	PlayerEvents::PlayerDeath (this, PlayerEvents::PlayerDeathEventArgs(Attacker, Inflictor));

	if (Health < -40)
	{
#if ROGUE_FEATURES
		// don't toss gibs if we got vaped by the nuke
		if (!((Health < -80) && (meansOfDeath == MOD_NUKE)))
#endif
		{
			// gib
			PlaySound (CHAN_BODY, SoundIndex ("misc/udeath.wav"));
			for (sint32 n = 0; n < 4; n++)
				CGibEntity::Spawn (this, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
			TossHead (Damage);
		}

		CanTakeDamage = false;

		Client.Anim.Priority = ANIM_DEATH;
		Client.Anim.EndFrame = 0;

	}
	else
	{	// normal death
		if (!IsDead)
		{
			static sint32 i;

			i = (i+1)%3;
			// start a death animation
			Client.Anim.Priority = ANIM_DEATH;
			if (Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED)
			{
				State.GetFrame() = FRAME_crdeath1 - 1;
				Client.Anim.EndFrame = FRAME_crdeath5;
			}
			else
			{
				switch (i)
				{
				case 0:
					State.GetFrame() = FRAME_death101 - 1;
					Client.Anim.EndFrame = FRAME_death106;
					break;
				case 1:
					State.GetFrame() = FRAME_death201 - 1;
					Client.Anim.EndFrame = FRAME_death206;
					break;
				case 2:
					State.GetFrame() = FRAME_death301 - 1;
					Client.Anim.EndFrame = FRAME_death308;
					break;
				}
			}
			PlaySound (CHAN_VOICE, GameMedia.Player.Death[(irandom(4))]);
		}
	}

	IsDead = true;

	Link ();
};

void CPlayerEntity::PrintToClient (EGamePrintLevel printLevel, const char *fmt, ...)
{
	va_list		argptr;
	CTempMemoryBlock		text = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, fmt);
	vsnprintf (text.GetBuffer<char>(), text.GetSize(), fmt, argptr);
	va_end (argptr);

	ClientPrintf (gameEntity, printLevel, "%s", text.GetBuffer<char>());
};

inline const char *GetChaseMode (uint8 mode)
{
	switch (mode)
	{
	case 0:
		return "Tight Chase";
	case 1:
		return "Freeform Chase";
	default:
		return "FPS Chase";
	};
}

void CPlayerEntity::UpdateChaseCam()
{
	CPlayerEntity *targ;

	// is our chase target gone?
	if (!Client.Chase.Target->GetInUse()
		|| Client.Chase.Target->Client.Respawn.Spectator || Client.Chase.Target->Client.Chase.Target)
	{
		CPlayerEntity *old = Client.Chase.Target;
		ChaseNext();
		if (Client.Chase.Target == old)
		{
			Client.Chase.Target = NULL;
			Client.PlayerState.GetPMove()->PMoveFlags &= ~PMF_NO_PREDICTION;
			return;
		}
	}

	targ = Client.Chase.Target;
	Client.PlayerState.GetGunIndex() = 0;

	switch (Client.Chase.Mode)
	{
	case 0:
		{
			vec3f ownerv = targ->State.GetOrigin();
			vec3f oldgoal = State.GetOrigin();
			ownerv.Z += targ->ViewHeight;

			vec3f angles = targ->Client.ViewAngle;

			if(angles.X > 56)
				angles.X = 56;

			anglef angleVecs = angles.ToVectors ();
			angleVecs.Forward.NormalizeFast ();
			vec3f o = ownerv.MultiplyAngles (-30, angleVecs.Forward);

			if(o.Z < targ->State.GetOrigin().Z + 20)
				o.Z = targ->State.GetOrigin().Z + 20;

			if (!targ->GroundEntity)
				o.Z += 16;

			CTrace trace (ownerv, o, targ, CONTENTS_MASK_SOLID);

			vec3f goal = trace.EndPosition.MultiplyAngles (2, angleVecs.Forward);
			o = goal + vec3f(0, 0, 6);
			trace (goal, o, targ, CONTENTS_MASK_SOLID);

			if (trace.Fraction < 1)
				goal = trace.EndPosition - vec3f(0, 0, 6);

			o = goal - vec3f(0, 0, 6);
			trace (goal, o, targ, CONTENTS_MASK_SOLID);

			if(trace.Fraction < 1)
				goal = trace.EndPosition + vec3f(0, 0, 6);

			if (targ->IsDead)
				Client.PlayerState.GetPMove()->PMoveType = PMT_DEAD;
			else
				Client.PlayerState.GetPMove()->PMoveType = PMT_FREEZE;

			State.GetOrigin() = goal;

			for (sint32 i = 0; i < 3; i++)
				Client.PlayerState.GetPMove()->DeltaAngles[i] = ANGLE2SHORT(targ->Client.ViewAngle[i] - Client.Respawn.CmdAngles[i]);

			if (targ->IsDead)
				Client.PlayerState.GetViewAngles().Set (-15, targ->Client.KillerYaw, 40);
			else
			{
				angles = targ->Client.ViewAngle + targ->Client.KickAngles;
				Client.PlayerState.GetViewAngles().Set (angles);
				Client.ViewAngle = angles;
			}
		}
		break;
	case 1:
		{
			Client.PlayerState.GetFov () = 90;

			if(Client.Respawn.CmdAngles.X > 89)
				Client.Respawn.CmdAngles.X = 89;

			if(Client.Respawn.CmdAngles.X < -89)
				Client.Respawn.CmdAngles.X = -89;

			vec3f ownerv = targ->State.GetOrigin() + vec3f(0, 0, targ->ViewHeight);

			anglef angleVecs = Client.PlayerState.GetViewAngles().ToVectors ();
			angleVecs.Forward.NormalizeFast ();
			vec3f o = ownerv.MultiplyAngles (-150, angleVecs.Forward);

			if (!targ->GroundEntity)
				o.Z += 16;

			CTrace trace(ownerv, o, targ, CONTENTS_MASK_SOLID);

			vec3f goal = trace.EndPosition.MultiplyAngles (2, angleVecs.Forward);
			o = goal + vec3f(0, 0, 6);

			trace (goal, o, targ, CONTENTS_MASK_SOLID);

			if(trace.Fraction < 1)
				goal = trace.EndPosition - vec3f(0, 0, 6);

			o = goal - vec3f(0, 0, 6);

			trace (goal, o, targ, CONTENTS_MASK_SOLID);

			if(trace.Fraction < 1)
				goal = trace.EndPosition + vec3f(0, 0, 6);

			if (targ->IsDead)
				Client.PlayerState.GetPMove()->PMoveType = PMT_DEAD;
			else
				Client.PlayerState.GetPMove()->PMoveType = PMT_FREEZE;

			State.GetOrigin() = goal;

			for (sint32 i = 0; i < 3; i++)
				Client.PlayerState.GetPMove()->DeltaAngles[i] = ANGLE2SHORT(targ->Client.ViewAngle[i] - Client.Respawn.CmdAngles[i]);

			Client.PlayerState.GetViewAngles() = Client.Respawn.CmdAngles;
		}
		break;
	default:
		{
			vec3f ownerv = targ->State.GetOrigin();
			vec3f angles = targ->Client.ViewAngle;

			anglef angleVecs = angles.ToVectors ();
			angleVecs.Forward.NormalizeFast ();
			vec3f o = ownerv.MultiplyAngles (16, angleVecs.Forward);
			o.Z += targ->ViewHeight;

			State.GetOrigin() = o;
			Client.PlayerState.GetFov () = targ->Client.PlayerState.GetFov();

			Client.PlayerState.GetGunIndex() = targ->Client.PlayerState.GetGunIndex();
			Client.PlayerState.GetGunAngles() = targ->Client.PlayerState.GetGunAngles();
			Client.PlayerState.GetGunFrame() = targ->Client.PlayerState.GetGunFrame();
			Client.PlayerState.GetGunOffset() = targ->Client.PlayerState.GetGunOffset();

			for (sint32 i = 0; i < 3; i++)
				Client.PlayerState.GetPMove()->DeltaAngles[i] = ANGLE2SHORT(targ->Client.ViewAngle[i] - Client.Respawn.CmdAngles[i]);

			if (targ->IsDead)
				Client.PlayerState.GetViewAngles().Set (-15, targ->Client.KillerYaw, 40);
			else
			{
				angles = targ->Client.ViewAngle + targ->Client.KickAngles;
				Client.PlayerState.GetViewAngles().Set (angles);
				Client.ViewAngle = angles;
			}
		}
		break;
	};

	ViewHeight = 0;
	Client.PlayerState.GetPMove()->PMoveFlags |= PMF_NO_PREDICTION;
	Link ();

	if ((!(Client.LayoutFlags & LF_SHOWSCORES) && !Client.Respawn.MenuState.InMenu &&
		!(Client.LayoutFlags & LF_SHOWINVENTORY) && !(Client.LayoutFlags & LF_SHOWHELP) &&
		!(Level.Frame & 31)) || (Client.LayoutFlags & LF_UPDATECHASE))
	{
		CStatusBar Chasing;
		String temp = String::Format("Chasing %s\n%s", targ->Client.Persistent.Name.CString(), GetChaseMode(Client.Chase.Mode));

		Chasing.AddVirtualPoint_X (0);
		Chasing.AddVirtualPoint_Y (-68);
		Chasing.AddString (temp, true, false);
		Chasing.SendMsg (this, false);

		Client.LayoutFlags &= ~LF_UPDATECHASE;
	}
}

void CPlayerEntity::ChaseNext()
{
	if (!Client.Chase.Target)
		return;

	switch (Client.Chase.Mode)
	{
	case 0:
	case 1:
		Client.Chase.Mode++;
		Client.LayoutFlags |= LF_UPDATECHASE;
		return;
	};

	sint32 i = Client.Chase.Target->State.GetNumber();
	CPlayerEntity *e;
	do
	{
		i++;
		if (i > Game.MaxClients)
			i = 1;
		e = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (!e->GetInUse())
			continue;
		if (e->NoClip)
			continue;
		if (!e->Client.Respawn.Spectator)
			break;
	} while (e != Client.Chase.Target);

	Client.Chase.Target = e;
	Client.Chase.Mode = 0;
	Client.LayoutFlags |= LF_UPDATECHASE;
}

void CPlayerEntity::ChasePrev()
{
	if (!Client.Chase.Target)
		return;

	sint32 i = Client.Chase.Target->State.GetNumber();
	CPlayerEntity *e;
	do
	{
		i--;
		if (i < 1)
			i = Game.MaxClients;
		e = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (!e->GetInUse())
			continue;
		if (e->NoClip)
			continue;
		if (!e->Client.Respawn.Spectator)
			break;
	} while (e != Client.Chase.Target);

	Client.Chase.Target = e;
	Client.Chase.Mode = 0;
	Client.LayoutFlags |= LF_UPDATECHASE;
}

void CPlayerEntity::GetChaseTarget()
{
	for (sint32 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *Other = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (Other->GetInUse() && !Other->Client.Respawn.Spectator && !Other->NoClip)
		{
			Client.Chase.Target = Other;
			Client.LayoutFlags |= LF_UPDATECHASE;
			Client.Chase.Mode = 0;
			UpdateChaseCam();
			return;
		}
	}
	PrintToClient(PRINT_CENTER, "No other players to chase.");
}

void CPlayerEntity::P_ProjectSource (vec3f distance, anglef angles, vec3f &result)
{
	switch (Client.Persistent.Hand)
	{
	case LEFT_HANDED:
		distance.Y *= -1;
		break;
	case CENTER_HANDED:
		distance.Y = 0;
		break;
	default:
		break;
	}

	G_ProjectSource (State.GetOrigin(), distance, angles, result);
}

void CPlayerEntity::PlayerNoiseAt (vec3f Where, ENoiseType type)
{
	if (type == PNOISE_WEAPON)
	{
		if (Client.Timers.SilencerShots)
		{
			Client.Timers.SilencerShots--;
			return;
		}
	}

	if (Game.GameMode & GAME_DEATHMATCH)
		return;

	if (Flags & FL_NOTARGET)
		return;

#if ROGUE_FEATURES
	if (Flags & FL_DISGUISED)
	{
		if (type == PNOISE_WEAPON)
		{
			Level.DisguiseViolator = this;
			Level.DisguiseViolationFrametime = Level.Frame + 5;
		}
		else
			return;
	}
#endif

	if (!Client.mynoise)
	{
		CPlayerNoise *noise = QNewEntityOf CPlayerNoise;
		noise->ClassName = "player_noise";
		noise->GetMins().Set (-8);
		noise->GetMaxs().Set (8);
		noise->SetOwner (this);
		noise->GetSvFlags() = SVF_NOCLIENT;
		Client.mynoise = noise;

		noise = QNewEntityOf CPlayerNoise;
		noise->ClassName = "player_noise";
		noise->GetMins().Set (-8);
		noise->GetMaxs().Set (8);
		noise->SetOwner (this);
		noise->GetSvFlags() = SVF_NOCLIENT;
		Client.mynoise2 = noise;
	}

	CPlayerNoise *noise;
	if (type == PNOISE_SELF || type == PNOISE_WEAPON)
	{
		noise = entity_cast<CPlayerNoise>(Client.mynoise);
		Level.SoundEntity = noise;
		Level.SoundEntityFrame = Level.Frame;
	}
	else // type == PNOISE_IMPACT
	{
		noise = entity_cast<CPlayerNoise>(Client.mynoise2);
		Level.SoundEntity2 = noise;
		Level.SoundEntity2Frame = Level.Frame;
	}

	noise->State.GetOrigin() = Where;
	noise->GetAbsMin() = (Where - noise->GetMins());
	noise->GetAbsMax() = (Where + noise->GetMaxs());
	noise->Time = Level.Frame;
	noise->Link ();
}

void CPlayerEntity::BeginDeathmatch ()
{
CC_DISABLE_DEPRECATION
	G_InitEdict (gameEntity);
CC_ENABLE_DEPRECATION

	InitResp();

	// locate ent at a spawn point
	PutInServer();

	if (Level.Intermission.Time)
		MoveToIntermission();
	else
		// send effect
		CMuzzleFlash(State.GetOrigin (), State.GetNumber(), MZ_LOGIN).Send();

	BroadcastPrintf (PRINT_HIGH, "%s entered the game\n", Client.Persistent.Name.CString());

	// make sure all view stuff is valid
	EndServerFrame();
}

void CPlayerEntity::Begin ()
{
	gameEntity->Server.Client = Game.Clients + (State.GetNumber()-1);

	if (Game.GameMode & GAME_DEATHMATCH)
	{
		BeginDeathmatch ();
		return;
	}

	// if there is already a body waiting for us (a loadgame), just
	// take it, otherwise spawn one from scratch
	if (GetInUse())
	{
		// the client has cleared the client side viewangles upon
		// connecting to the server, which is different than the
		// state when the game is saved, so we need to compensate
		// with deltaangles
		for (sint32 i = 0; i < 3; i++)
			Client.PlayerState.GetPMove()->DeltaAngles[i] = ANGLE2SHORT(Client.PlayerState.GetViewAngles()[i]);

		// also re-build the weapon model
		Client.PlayerState.GetGunIndex() = Client.Persistent.Weapon->GetWeaponModel();
	}
	else
	{
		// a spawn point will completely reinitialize the entity
		// except for the persistant data that was initialized at
		// ClientConnect() time

CC_DISABLE_DEPRECATION
		G_InitEdict (gameEntity);
CC_ENABLE_DEPRECATION

		ClassName = "player";
		InitResp ();
		PutInServer ();
	}

	if (Level.Intermission.Time)
		MoveToIntermission ();
	else
	{
		// send effect if in a multiplayer game
		if (Game.MaxClients > 1)
		{
			CMuzzleFlash(State.GetOrigin(), State.GetNumber(), MZ_LOGIN).Send();
			BroadcastPrintf (PRINT_HIGH, "%s entered the game\n", Client.Persistent.Name.CString());
		}
	}

	// make sure all view stuff is valid
	EndServerFrame ();

	Client.Persistent.State = SVCS_SPAWNED;
}

IPAddress CopyIP (const char *val)
{
	// Do we have a :?
	String str (val);

	size_t loc = str.IndexOf (':');

	if (loc != -1)
		str = str.Substring(0, loc);

	IPAddress Adr;

	CC_ASSERT_EXPR (!(str.Count() > sizeof(Adr.str)), "IP copied is longer than sizeof(Adr.str)");
	Q_snprintfz (Adr.str, sizeof(Adr.str), "%s", str.CString());

	return Adr;
}

bool CPlayerEntity::Connect (const char *userinfo, CUserInfo &UserInfo)
{
	// check to see if they are on the banned IP list
	String value = UserInfo.GetValueFromKey (String("ip"));
	IPAddress Adr = CopyIP (value.CString());

	if (Bans.IsBanned(Adr) || Bans.IsBanned(UserInfo.GetValueFromKey (String("name")).CString()))
	{
		UserInfo.SetValueForKey(String("rejmsg"), String("Connection refused: You are banned from this server."));
		return false;
	}

	// check for a Spectator
	value = UserInfo.GetValueFromKey (String("spectator"));
	if ((Game.GameMode & GAME_DEATHMATCH) && value.Count() && value != "0")
	{
		sint32 i, numspec;

		if (Bans.IsBannedFromSpectator(Adr) || Bans.IsBannedFromSpectator(UserInfo.GetValueFromKey (String("name")).CString()))
		{
			UserInfo.SetValueForKey (String("rejmsg"), String("Not permitted to enter spectator mode"));
			return false;
		}
		if (*CvarList[CV_SPECTATOR_PASSWORD].StringValue() && 
			strcmp(CvarList[CV_SPECTATOR_PASSWORD].StringValue(), "none") && 
			value != CvarList[CV_SPECTATOR_PASSWORD].StringValue())
		{
			UserInfo.SetValueForKey (String("rejmsg"), String("Spectator password required or incorrect."));
			return false;
		}

		// count spectators
		for (i = numspec = 0; i < Game.MaxClients; i++)
		{
			CPlayerEntity *Ent = entity_cast<CPlayerEntity>(Game.Entities[i+1].Entity);
			if (Ent->GetInUse() && Ent->Client.Persistent.Spectator)
				numspec++;
		}

		if (numspec >= Game.MaxSpectators)
		{
			UserInfo.SetValueForKey (String("rejmsg"), String("Server Spectator limit is full."));
			return false;
		}
	}
	else
	{
		// check for a password
		value = UserInfo.GetValueFromKey (String("password"));
		if (*CvarList[CV_PASSWORD].StringValue() && strcmp(CvarList[CV_PASSWORD].StringValue(), "none") && 
			value != CvarList[CV_PASSWORD].StringValue())
		{
			UserInfo.SetValueForKey (String("rejmsg"), String("Password required or incorrect."));
			return false;
		}
	}

	// they can connect
	gameEntity->Server.Client = Game.Clients + (State.GetNumber()-1);

	// if there is already a body waiting for us (a loadgame), just
	// take it, otherwise spawn one from scratch
	// Paril: Fix for an engine bug that causes
	// players to be kicked for the same IP but
	// takes over an existing spot.
	bool SameConnection = (GetInUse() && ((Game.GameMode & GAME_DEATHMATCH) && Client.Persistent.UserInfo.ToString().CompareCaseInsensitive(userinfo) == 0));

	if (!GetInUse() || SameConnection)
	{
		// clear the respawning variables
#if CLEANCTF_ENABLED
//ZOID -- force team join
		Client.Respawn.CTF.Team = -1;
		Client.Respawn.CTF.IDState = false; 
//ZOID
#endif
		InitResp ();

		if (!Game.AutoSaved || !Client.Persistent.Weapon)
			InitPersistent();
		else if (SameConnection)
		{
			GetInUse() = false;
			InitPersistent();
		}
	}

	UserinfoChanged (userinfo);
	Client.Persistent.IP = Adr;

	if (Game.MaxClients > 1)
	{
		// Tell the entire game that someone connected
		BroadcastPrintf (PRINT_MEDIUM, "%s connected\n", Client.Persistent.Name.CString());
		
		// But only tell the server the IP
		ServerPrintf ("%s @ %s connected\n", Client.Persistent.Name.CString(), UserInfo.GetValueFromKey (String("ip")).CString());
	}

	GetSvFlags() = 0; // make sure we start with known default
	Client.Persistent.State = SVCS_CONNECTED;

	PlayerEvents::PlayerConnected(this);

	return true;
}

void CPlayerEntity::Disconnect ()
{
	if (!gameEntity->Server.Client)
		return;

	Client.Persistent.State = SVCS_FREE;
	BroadcastPrintf (PRINT_HIGH, "%s disconnected\n", Client.Persistent.Name.CString());

#if CLEANCTF_ENABLED
//ZOID
	if (Game.GameMode & GAME_CTF)
		CTFDeadDropFlag();
//ZOID
#endif

	if (
#if CLEANCTF_ENABLED
		(Game.GameMode & GAME_CTF) || 
#endif
		DeathmatchFlags.dfDmTechs.IsEnabled()) 
		DeadDropTech();

#if ROGUE_FEATURES
	if(Client.Timers.Tracker)
		RemoveAttackingPainDaemons ();

	if (Client.OwnedSphere)
	{
		if (Client.OwnedSphere->GetInUse())
			Client.OwnedSphere->Free ();
		Client.OwnedSphere = NULL;
	}
#endif

	// send effect
	CMuzzleFlash(State.GetOrigin(), State.GetNumber(), MZ_LOGIN).Send();

	Unlink ();
	State.GetModelIndex() = 0;
	GetSolid() = SOLID_NOT;
	GetInUse() = false;
	ClassName = "disconnected";

	ConfigString (CS_PLAYERSKINS+(State.GetNumber()-1), "");

#if CLEANCODE_IRC
	if (Client.Respawn.IRC.Connected())
		Client.Respawn.IRC.Disconnect();
#endif

	PlayerEvents::PlayerDisconnected(this);
}

inline const char *MonsterAOrAn (const char *Name)
{
	static const char *_a = "a";
	static const char *_an = "an";

	switch (Q_tolower(Name[0]))
	{
	case 'a':
	case 'e':
	case 'i':
	case 'o':
	case 'u':
		return _an;
	default:
		return _a;
	};
}

void CPlayerEntity::Obituary (IBaseEntity *Attacker)
{
	static String message, message2;
	message.Clear();
	message2.Clear();

	if (Attacker == this)
	{
		// Paril, Issue 3 fix
		switch (meansOfDeath & ~MOD_FRIENDLY_FIRE)
		{
		case MOD_HELD_GRENADE:
			message = "tried to put the pin back in";
			break;
		case MOD_HG_SPLASH:
		case MOD_G_SPLASH:
			switch (Client.Respawn.Gender)
			{
			case GENDER_MALE:
				message = "tripped on his own grenade";
				break;
			case GENDER_FEMALE:
				message = "tripped on her own grenade";
				break;
			default:
				message = "tripped on its own grenade";
				break;
			}
			break;
		case MOD_R_SPLASH:
			switch (Client.Respawn.Gender)
			{
			case GENDER_MALE:
				message = "blew himself up";
				break;
			case GENDER_FEMALE:
				message = "blew herself up";
				break;
			default:
				message = "blew itself up";
				break;
			}
			break;
		case MOD_BFG_BLAST:
			message = "should have used a smaller gun";
			break;

#if XATRIX_FEATURES
		case MOD_TRAP:
			switch (Client.Respawn.Gender)
			{
			case GENDER_MALE:
				message = "got sucked into his own trap";
				break;
			case GENDER_FEMALE:
				message = "got sucked into her own trap";
				break;
			default:
				message = "got sucked into it's own trap";
				break;
			}
			break;
#endif

#if ROGUE_FEATURES
		case MOD_DOPPLE_EXPLODE:
			switch (Client.Respawn.Gender)
			{
			case GENDER_MALE:
				message = "got caught in his own trap";
				break;
			case GENDER_FEMALE:
				message = "got caught in her own trap";
				break;
			default:
				message = "got caught in it's own trap";
				break;
			}
			break;
#endif
		default:
			switch (Client.Respawn.Gender)
			{
			case GENDER_MALE:
				message = "killed himself";
				break;
			case GENDER_FEMALE:
				message = "killed herself";
				break;
			default:
				message = "killed itself";
				break;
			}
			break;
		}
		if (Game.GameMode & GAME_DEATHMATCH)
			Client.Respawn.Score--;
		BroadcastPrintf (PRINT_MEDIUM, "%s %s.\n", Client.Persistent.Name.CString(), message.CString());
	}
	else if (Attacker && (Attacker->EntityFlags & EF_PLAYER))
	{
		CPlayerEntity *PlayerAttacker = entity_cast<CPlayerEntity>(Attacker);
		bool endsInS = (PlayerAttacker->Client.Persistent.Name[PlayerAttacker->Client.Persistent.Name.Count()-1] == 's');

		// Paril, Issue 3 fix
		switch (meansOfDeath & ~MOD_FRIENDLY_FIRE)
		{
		case MOD_BLASTER:
			message = "was blasted by";
			break;
		case MOD_SHOTGUN:
			message = "was gunned down by";
			break;
		case MOD_SSHOTGUN:
			message = "was blown away by";
			message2 = (endsInS) ? "' super shotgun" : "'s super shotgun";
			break;
		case MOD_MACHINEGUN:
			message = "was machinegunned by";
			break;
		case MOD_CHAINGUN:
			message = "was cut in half by";
			message2 = (endsInS) ? "' chaingun" : "'s chaingun";
			break;
		case MOD_GRENADE:
			message = "was popped by";
			message2 = (endsInS) ? "' grenade" : "'s grenade";
			break;
		case MOD_G_SPLASH:
			message = "was shredded by";
			message2 = (endsInS) ? "' shrapnel" : "'s shrapnel";
			break;
		case MOD_ROCKET:
			message = "ate";
			message2 = (endsInS) ? "' rocket" : "'s rocket";
			break;
		case MOD_R_SPLASH:
			message = "almost dodged";
			message2 = (endsInS) ? "' rocket" : "'s rocket";
			break;
		case MOD_HYPERBLASTER:
			message = "was melted by";
			message2 = (endsInS) ? "' hyperblaster" : "'s hyperblaster";
			break;
		case MOD_RAILGUN:
			message = "was railed by";
			break;
		case MOD_BFG_LASER:
			message = "saw the pretty lights from";
			message2 = (endsInS) ? "' BFG" : "'s BFG";
			break;
		case MOD_BFG_BLAST:
			message = "was disintegrated by";
			message2 = (endsInS) ? "' BFG blast" : "'s BFG blast";
			break;
		case MOD_BFG_EFFECT:
			message = "couldn't hide from";
			message2 = (endsInS) ? "' BFG" : "'s BFG";
			break;
		case MOD_HANDGRENADE:
			message = "caught";
			message2 = (endsInS) ? "' handgrenade" : "'s handgrenade";
			break;
		case MOD_HG_SPLASH:
			message = "didn't see";
			message2 = (endsInS) ? "' handgrenade" : "'s handgrenade";
			break;
		case MOD_HELD_GRENADE:
			message = "feels";
			message2 = (endsInS) ? "' pain" : "'s pain";
			break;
		case MOD_TELEFRAG:
			message = "tried to invade";
			message2 = (endsInS) ? "' personal space" : "'s personal space";
			break;
		case MOD_EXPLOSIVE:
		case MOD_BARREL:
			message = "was blown to smithereens by";
			break;

#if CLEANCTF_ENABLED
//ZOID
		case MOD_GRAPPLE:
			message = "was caught by";
			message2 = "'s grapple";
			break;
//ZOID
#endif

#if XATRIX_FEATURES
		case MOD_RIPPER:
			message = "was ripped to shreds by";
			message2 = "'s ripper gun";
			break;
		case MOD_TRAP:
			message = "was caught in trap by";
			break;
#endif

#if ROGUE_FEATURES
		case MOD_CHAINFIST:
			message = "was shredded by";
			message2 = "'s ripsaw";
			break;
		case MOD_DISINTEGRATOR:
			message = "lost his grip courtesy of";
			message2 = "'s disintegrator";
			break;
		case MOD_ETF_RIFLE:
			message = "was perforated by";
			break;
		case MOD_HEATBEAM:
			message = "was scorched by";
			message2 = "'s plasma beam";
			break;
		case MOD_TESLA:
			message = "was enlightened by";
			message2 = "'s tesla mine";
			break;
		case MOD_PROX:
			message = "got too close to";
			message2 = "'s proximity mine";
			break;
		case MOD_NUKE:
			message = "was nuked by";
			message2 = "'s antimatter bomb";
			break;
		case MOD_VENGEANCE_SPHERE:
			message = "was purged by";
			message2 = "'s vengeance sphere";
			break;
		case MOD_DEFENDER_SPHERE:
			message = "had a blast with";
			message2 = "'s defender sphere";
			break;
		case MOD_HUNTER_SPHERE:
			message = "was killed like a dog by";
			message2 = "'s hunter sphere";
			break;
		case MOD_TRACKER:
			message = "was annihilated by";
			message2 = "'s disruptor";
			break;
		case MOD_DOPPLE_EXPLODE:
			message = "was blown up by";
			message2 = "'s doppleganger";
			break;
		case MOD_DOPPLE_VENGEANCE:
			message = "was purged by";
			message2 = "'s doppleganger";
			break;
		case MOD_DOPPLE_HUNTER:
			message = "was hunted down by";
			message2 = "'s doppleganger";
			break;
#endif
		}
		BroadcastPrintf (PRINT_MEDIUM,"%s %s %s%s.\n", Client.Persistent.Name.CString(), message.CString(), PlayerAttacker->Client.Persistent.Name.CString(), message2.CString());
		if (Game.GameMode & GAME_DEATHMATCH)
			PlayerAttacker->Client.Respawn.Score++;
	}
	else if (Attacker && (Attacker->EntityFlags & EF_MONSTER))
	{
		// Paril, Issue 3 fix
		switch (meansOfDeath & ~MOD_FRIENDLY_FIRE)
		{
		case MOD_BLASTER:
			message = "was blasted by";
			break;
		case MOD_SHOTGUN:
			message = "was gunned down by";
			break;
		case MOD_SSHOTGUN:
			message = "was blown away by";
			message2 = "'s super shotgun";
			break;
		case MOD_MACHINEGUN:
			message = "was machinegunned by";
			break;
		case MOD_CHAINGUN:
			message = "was cut in half by";
			message2 = "'s chaingun";
			break;
		case MOD_GRENADE:
			message = "was popped by";
			message2 = "'s grenade";
			break;
		case MOD_G_SPLASH:
			message = "was shredded by";
			message2 = "'s shrapnel";
			break;
		case MOD_ROCKET:
			message = "ate";
			message2 = "'s rocket";
			break;
		case MOD_R_SPLASH:
			message = "almost dodged";
			message2 = "'s rocket";
			break;
		case MOD_HYPERBLASTER:
			message = "was melted by";
			message2 = "'s hyperblaster";
			break;
		case MOD_RAILGUN:
			message = "was railed by";
			break;
		case MOD_BFG_LASER:
			message = "saw the pretty lights from";
			message2 = "'s BFG";
			break;
		case MOD_BFG_BLAST:
			message = "was disintegrated by";
			message2 = "'s BFG blast";
			break;
		case MOD_BFG_EFFECT:
			message = "couldn't hide from";
			message2 = "'s BFG";
			break;
		case MOD_EXPLOSIVE:
		case MOD_BARREL:
			message = "was blown to smithereens by";
			break;
#if XATRIX_FEATURES
		case MOD_RIPPER:
			message = "was ripped to shreds by";
			message2 = "'s ripper gun";
			break;
#endif
		default:
			message = "was killed by";
			break;
		};

		CMonsterEntity *Monster = entity_cast<CMonsterEntity>(Attacker);

		if (Game.GameMode & GAME_DEATHMATCH)
			Client.Respawn.Score--;
		BroadcastPrintf (PRINT_MEDIUM, "%s %s %s %s%s.\n", Client.Persistent.Name.CString(), message.CString(), MonsterAOrAn(Monster->Monster->GetMonsterName()), Monster->Monster->GetMonsterName(), message2.CString());
	}
	else
	{
		// Paril, Issue 3 fix
		switch (meansOfDeath & ~MOD_FRIENDLY_FIRE)
		{
		case MOD_SUICIDE:
			message = "suicides";
			break;
		case MOD_FALLING:
			message = "cratered";
			break;
		case MOD_CRUSH:
			message = "was squished";
			break;
		case MOD_WATER:
			message = "sank like a rock";
			break;
		case MOD_SLIME:
			message = "melted";
			break;
		case MOD_LAVA:
			message = "does a back flip into the lava";
			break;
		case MOD_EXPLOSIVE:
		case MOD_BARREL:
			message = "blew up";
			break;
		case MOD_EXIT:
			message = "found a way out";
			break;
		case MOD_TARGET_LASER:
			message = "saw the light";
			break;
		case MOD_TARGET_BLASTER:
			message = "got blasted";
			break;
		case MOD_BOMB:
		case MOD_SPLASH:
		case MOD_TRIGGER_HURT:
			message = "was in the wrong place";
			break;
		default:
			message = "died";
			break;
		}

		if (Game.GameMode & GAME_DEATHMATCH)
			Client.Respawn.Score--;
		BroadcastPrintf (PRINT_MEDIUM, "%s %s.\n", Client.Persistent.Name.CString(), message.CString());
	}
}

#if ROGUE_FEATURES
#include "Entities/BrushModels.h"
#include "Entities/TriggerEntities.h"
#endif

void CPlayerEntity::PushInDirection (vec3f vel, ESpawnflags flags)
{
	if (Health > 0)
	{
		Velocity = vel;

		// don't take falling damage immediately from this
		Client.OldVelocity = Velocity;
		if (
#if ROGUE_FEATURES
			(!(flags & PUSH_SILENT)) &&
#endif
			FlySoundDebounceTime < Level.Frame)
		{
			FlySoundDebounceTime = Level.Frame + 15;
			PlaySound (CHAN_AUTO, GameMedia.FlySound());
		}
	}
}

#if ROGUE_FEATURES
void CPlayerEntity::RemoveAttackingPainDaemons ()
{
	IBaseEntity *tracker = NULL;

	do
	{
		tracker = CC_FindByClassName<IBaseEntity, EF_BASE> (tracker, "pain daemon");

		if (tracker && tracker->Enemy == this)
			tracker->Free ();
	}
	while (tracker);

	Client.Timers.Tracker = 0;
}
#endif

void CPlayerEntity::ShowScores ()
{
	Client.LayoutFlags &= ~(LF_SHOWINVENTORY | LF_SHOWHELP);
	if (Client.Respawn.MenuState.InMenu)
	{
		Client.Respawn.MenuState.CloseMenu();
		return;
	}

	if (Game.GameMode & GAME_SINGLEPLAYER)
		return;

	if (Client.LayoutFlags & LF_SHOWSCORES)
	{
		Client.LayoutFlags &= ~LF_SHOWSCORES;
		Client.LayoutFlags |= LF_UPDATECHASE;
		return;
	}

	Client.LayoutFlags |= LF_SHOWSCORES;
	DeathmatchScoreboardMessage (true);
}

void CPlayerEntity::ShowHelp ()
{
	if (Level.Intermission.Time)
		return;

	if (Game.GameMode & GAME_DEATHMATCH)
	{
		ShowScores ();
		return;
	}

	Client.LayoutFlags &= ~(LF_SHOWSCORES | LF_SHOWINVENTORY);

	if (Client.Respawn.MenuState.InMenu)
	{
		Client.Respawn.MenuState.CloseMenu();
		return;
	}

	if ((Client.LayoutFlags & LF_SHOWHELP) && (Client.Persistent.GameHelpChanged == Game.HelpChanged))
	{
		Client.LayoutFlags &= ~LF_SHOWHELP;
		return;
	}

	Client.LayoutFlags |= LF_SHOWHELP;
	Client.Persistent.HelpChanged = 0;
	HelpComputer ();
}

bool CPlayerEntity::CheckFlood ()
{
	if (CvarList[CV_FLOOD_MSGS].Integer())
	{
		if (Level.Frame < Client.Flood.LockTill)
		{
			PrintToClient (PRINT_HIGH, "You can't talk for %d more seconds\n",
				(sint32)((Client.Flood.LockTill - Level.Frame)/10));
			return true;
		}
		sint32 i = Client.Flood.WhenHead - CvarList[CV_FLOOD_MSGS].Integer() + 1;
		if (i < 0)
			i = (sizeof(Client.Flood.When)/sizeof(Client.Flood.When[0])) + i;
		if (Client.Flood.When[i] && 
			((Level.Frame - Client.Flood.When[i])/10) < CvarList[CV_FLOOD_PER_SECOND].Integer())
		{
			Client.Flood.LockTill = Level.Frame + (CvarList[CV_FLOOD_DELAY].Float() * 10);
			PrintToClient (PRINT_CHAT, "Flood protection:  You can't talk for %d seconds.\n",
				CvarList[CV_FLOOD_DELAY].Integer());
			return true;
		}
		Client.Flood.WhenHead = (Client.Flood.WhenHead + 1) %
			(sizeof(Client.Flood.When)/sizeof(Client.Flood.When[0]));
		Client.Flood.When[Client.Flood.WhenHead] = Level.Frame;
	}
	return false;
}

/**
\fn	void CPlayerEntity::CastTo (ECastFlags CastFlags)

\brief	Perform a cast to this player. 

\author	Paril
\date	29/05/2010

\param	CastFlags	The cast flags. 
**/
void CPlayerEntity::CastTo (ECastFlags CastFlags)
{
	Cast (CastFlags, this);
}

/**
\fn	void CPlayerEntity::StuffText (const char *text)

\attention	Be careful with this function - do not abuse it.
	
\brief	Stuff text to player's command buffer. 
	
\author	Paril
\date	29/05/2010
	
\param	text	The text. 
**/
void CPlayerEntity::StuffText (const char *text)
{
   	WriteByte (SVC_STUFFTEXT);	        
	WriteString (text);
    CastTo (CASTFLAG_RELIABLE);	
}

bool CPlayerEntity::IsSpawned ()
{
	return (GetInUse() && Client.Persistent.State == SVCS_SPAWNED);
}

String CPlayerEntity::ClientTeam ()
{
	String val = Client.Persistent.UserInfo.GetValueFromKey(String("skin")).ToLower();

	size_t slash = val.IndexOf('/');
	if (slash == -1)
		return val;

	if (DeathmatchFlags.dfModelTeams.IsEnabled())
	{
		val.Remove (slash);
		return val;
	}

	val.Remove (0, 1);
	return val;
}

bool CPlayerEntity::OnSameTeamAs (CPlayerEntity *Player)
{
	if (Game.GameMode & GAME_COOPERATIVE)
		return true;
	if (!(DeathmatchFlags.dfSkinTeams.IsEnabled() || DeathmatchFlags.dfModelTeams.IsEnabled()))
		return false;

	return ClientTeam() == Player->ClientTeam();
}

/**
\fn	sint32 CPlayerEntity::CheckPowerArmor (vec3f &Point, vec3f &Normal, sint32 Damage,
	EDamageFlags DamageFlags)

\brief	Checks power armor and makes adjustments based on that. 

\author	Paril
\date	13/06/2010

\param [in,out]	Point	The point. 
\param [in,out]	Normal	The normal. 
\param	Damage			The damage. 
\param	DamageFlags		The damage flags. 

\return	The amount of damage saved from the power armor. 
**/
sint32 CPlayerEntity::CheckPowerArmor (vec3f &Point, vec3f &Normal, sint32 Damage, EDamageFlags DamageFlags)
{
	if (!Damage)
		return 0;
	if (DamageFlags & DAMAGE_NO_ARMOR)
		return 0;

	static const sint32	index = NItems::Cells->GetIndex();

	sint32			DamagePerCell;
	bool			ScreenSparks = false;

	EPowerArmorType Type = PowerArmorType ();

	if (Type == POWER_ARMOR_NONE)
		return 0;

	sint32 Power = Client.Persistent.Inventory.Has(index);

	if (!Power)
		return 0;

	switch (Type)
	{
	case POWER_ARMOR_NONE:
	default:
		return 0;
	case POWER_ARMOR_SCREEN:
		{
			vec3f		vec;

			// only works if damage point is in front
			vec = Point - State.GetOrigin();
			vec.Normalize ();
			if ((vec | State.GetAngles().ToVectors().Forward) <= 0.3)
				return 0;

			DamagePerCell = 1;
			ScreenSparks = true;
			Damage /= 3;
		}
		break;
	case POWER_ARMOR_SHIELD:
		DamagePerCell = 2;
		Damage = (2 * Damage) / 3;
		break;
	};

	sint32 Saved = Power * DamagePerCell;
	if (!Saved)
		return 0;
	if (Saved > Damage)
		Saved = Damage;

	CShieldSparks(Point, Normal, ScreenSparks).Send();

	sint32 PowerUsed = Saved / DamagePerCell;
	if (!PowerUsed)
		PowerUsed = 1;

	Client.Persistent.Inventory.Remove(index, PowerUsed);
	Client.PowerArmorTime = 2;

	return Saved;
}

/**
\fn	void CPlayerEntity::TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker, vec3f Dir,
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
void CPlayerEntity::TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker,
							vec3f Dir, vec3f Point, vec3f Normal, sint32 Damage,
							sint32 KnockBack, EDamageFlags DamageFlags, EMeansOfDeath MeansOfDeath)
{
	if (CvarList[CV_MAP_DEBUG].Boolean())
		return;

	sint32			take;
	sint32			save;
	sint32			asave = 0;
	sint32			psave = 0;

	// Needed?
	if (!CanTakeDamage)
		return;

	// friendly fire avoidance
	// if enabled you can't hurt teammates (but you can hurt yourself)
	// knockback still occurs
	if ((this != Attacker) && ((Game.GameMode & GAME_DEATHMATCH) && (DeathmatchFlags.dfSkinTeams.IsEnabled() || DeathmatchFlags.dfModelTeams.IsEnabled()) || Game.GameMode & GAME_COOPERATIVE))
	{
		if (Attacker && (Attacker->EntityFlags & EF_PLAYER))
		{
			if (OnSameTeamAs (entity_cast<CPlayerEntity>(Attacker)))
			{
				if (DeathmatchFlags.dfNoFriendlyFire.IsEnabled()
#if ROGUE_FEATURES
					&& (MeansOfDeath != MOD_NUKE)
#endif
					)
					Damage = 0;
				else
					MeansOfDeath |= MOD_FRIENDLY_FIRE;
			}
		}
	}
	meansOfDeath = MeansOfDeath;

	// easy mode takes half damage
#if ROGUE_FEATURES
	if ((CvarList[CV_SKILL].Integer() == 0 && !(Game.GameMode & GAME_DEATHMATCH)) ||
		(Client.OwnedSphere) && (Client.OwnedSphere->SpawnFlags == 1))
#else
	if (CvarList[CV_SKILL].Integer() == 0 && !(Game.GameMode & GAME_DEATHMATCH))
#endif
	{
		Damage *= 0.5;
		if (!Damage)
			Damage = 1;
	}

	Dir.Normalize ();

	if (DeathmatchFlags.dfDmTechs.IsEnabled()
#if CLEANCTF_ENABLED
	|| (Game.GameMode & GAME_CTF)
#endif
	)
	{
		if (Client.Persistent.Tech && (Client.Persistent.Tech->TechType == CTech::TECH_AGGRESSIVE))
			Client.Persistent.Tech->DoAggressiveTech (this, Attacker, false, Damage, KnockBack, DamageFlags, MeansOfDeath, true);

		if (Attacker->EntityFlags & EF_PLAYER)
		{
			CPlayerEntity *Atk = entity_cast<CPlayerEntity>(Attacker);
			if (Atk->Client.Persistent.Tech && (Atk->Client.Persistent.Tech->TechType == CTech::TECH_AGGRESSIVE))
				Atk->Client.Persistent.Tech->DoAggressiveTech (Atk, this, false, Damage, KnockBack, DamageFlags, MeansOfDeath, false);
		}
	}

	if (!AffectedByKnockback)
		KnockBack = 0;

// figure momentum add
	if (KnockBack && !(DamageFlags & DAMAGE_NO_KNOCKBACK))
		Velocity += Dir * ((Attacker == this) ? 1600.0f : 500.0f) * (float)KnockBack / Clamp<float> (Mass, 50, Mass);

	take = Damage;
	save = 0;

	// check for godmode
	if ( (Flags & FL_GODMODE) && !(DamageFlags & DAMAGE_NO_PROTECTION))
	{
		take = 0;
		save = Damage;
		CSparks (Point, Normal, (DamageFlags & DAMAGE_BULLET) ? ST_BULLET_SPARKS : ST_SPARKS, SPT_SPARKS).Send();
	}

	// check for invincibility
	if ((Client.Timers.Invincibility > Level.Frame) && !(DamageFlags & DAMAGE_NO_PROTECTION))
	{
		if (PainDebounceTime < Level.Frame)
		{
			PlaySound (CHAN_ITEM, SoundIndex("items/protect4.wav"));
			PainDebounceTime = Level.Frame + 20;
		}
		take = 0;
		save = Damage;
	}

#if CLEANCTF_ENABLED
//ZOID
//team armor protect
	if ((Game.GameMode & GAME_CTF) && (Attacker->EntityFlags & EF_PLAYER) &&
		(Client.Respawn.CTF.Team == (entity_cast<CPlayerEntity>(Attacker))->Client.Respawn.CTF.Team) &&
		(this != Attacker) && DeathmatchFlags.dfCtfArmorProtect.IsEnabled())
		psave = asave = 0;
	else
	{
//ZOID
#endif
		psave = CheckPowerArmor (Point, Normal, take, DamageFlags);
		take -= psave;

		if (Client.Persistent.Armor)
		{
			asave = Client.Persistent.Armor->CheckArmor (this, Point, Normal, take, DamageFlags);
			take -= asave;
		}
#if CLEANCTF_ENABLED
	}
#endif

	//treat cheat/powerup savings the same as armor
	asave += save;

	if (DeathmatchFlags.dfDmTechs.IsEnabled()
#if CLEANCTF_ENABLED
	|| (Game.GameMode & GAME_CTF)
#endif
	)
	{
		if (Client.Persistent.Tech && (Client.Persistent.Tech->TechType == CTech::TECH_AGGRESSIVE))
			Client.Persistent.Tech->DoAggressiveTech (this, Attacker, true, take, KnockBack, DamageFlags, MeansOfDeath, true);

		if (Attacker->EntityFlags & EF_PLAYER)
		{
			CPlayerEntity *Atk = entity_cast<CPlayerEntity>(Attacker);
			if (Atk->Client.Persistent.Tech && (Atk->Client.Persistent.Tech->TechType == CTech::TECH_AGGRESSIVE))
				Atk->Client.Persistent.Tech->DoAggressiveTech (Atk, this, true, Damage, KnockBack, DamageFlags, MeansOfDeath, false);
		}
	}

	// team damage avoidance
	if (!(DamageFlags & DAMAGE_NO_PROTECTION) && CheckTeamDamage (Attacker))
		return;

// ROGUE - this option will do damage both to the armor and person. originally for DPU rounds
	if (DamageFlags & DAMAGE_DESTROY_ARMOR)
	{
		if(!(Flags & FL_GODMODE) && !(DamageFlags & DAMAGE_NO_PROTECTION) &&
		   !(Client.Timers.Invincibility > Level.Frame))
			take = Damage;
	}
// ROGUE

#if CLEANCTF_ENABLED
//ZOID
	if ((Game.GameMode & GAME_CTF) && (Attacker->EntityFlags & EF_PLAYER))
		CTFCheckHurtCarrier(this, entity_cast<CPlayerEntity>(Attacker));
//ZOID
#endif

// do the damage
	if (take)
	{
		DamageEffect (Dir, Point, Normal, take, DamageFlags, MeansOfDeath);
		Health -= take;
	}

#if ROGUE_FEATURES
	if(Client.OwnedSphere)
		Client.OwnedSphere->Pain (Attacker, 0);
#endif

	if (Health <= 0)
	{
		AffectedByKnockback = false;
		Killed (Inflictor, Attacker, take, Point);
		return;
	}

	if (take)
		Pain (Attacker, take);

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	Client.DamageValues[DT_POWERARMOR] += psave;
	Client.DamageValues[DT_ARMOR] += asave;
	Client.DamageValues[DT_BLOOD] += take;
	Client.DamageValues[DT_KNOCKBACK] += KnockBack;
	Client.DamageFrom = Point;
}


