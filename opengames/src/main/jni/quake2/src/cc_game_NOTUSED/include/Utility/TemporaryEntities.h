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
// cc_tent.h
// A huge class containing containers to spawn temp entities with a single function call.
//

#if !defined(CC_GUARD_TENT_H) || !INCLUDE_GUARDS
#define CC_GUARD_TENT_H

#include "Utility/Colors.h"

// The NEW AND IMPROVED AND NEW AND IMPROVED AND CLASS-BASED AND IMPROVED TempEnt system
class CTempEntFlags
{
public:
	static const CTempEntFlags DefaultTempEntFlags;

	ECastType	Type;
	ECastFlags	Flags;
	vec3f		CastOrigin;

	CTempEntFlags (ECastType Type, ECastFlags Flags, vec3f CastOrigin) :
	  Type(Type),
	  Flags(Flags),
	  CastOrigin(CastOrigin)
	{
	};

	CTempEntFlags (const CTempEntFlags &right) :
	  Type(right.Type),
	  Flags(right.Flags),
	  CastOrigin(right.CastOrigin)
	{
	};
};

class CForEachPlayerMulticastCallback
{
public:
	sint32			Index;
	class CTempEnt	*TempEnt;

	virtual bool Callback (CPlayerEntity *Player) = 0;
	virtual void Query (bool MustBeInUse = true);
};

class CTempEnt
{
protected:
	CTempEntFlags		Flags;
	vec3f				Origin;
	bool				ToPlayerOrigin;
	CPlayerEntity		*Player;

	// This function must be overridden.
	// It sends out the actual tempent-based data to the player(s).
	virtual void SendData () = 0;

public:
	CTempEnt (vec3f Origin = vec3fOrigin, CPlayerEntity *Player = NULL) :
	  Flags(CTempEntFlags::DefaultTempEntFlags),
	  Origin(Origin),
	  Player(Player),
	  ToPlayerOrigin(true)
	{
	};

	CTempEnt (CTempEntFlags Flags, vec3f Origin = vec3fOrigin, CPlayerEntity *Player = NULL) :
	  Flags(Flags),
	  Origin(Flags.CastOrigin),
	  Player(Player),
	  ToPlayerOrigin(true)
	{
	};

	CTempEnt &AtPlayer (bool ToPlayerOrigin)
	{
		this->ToPlayerOrigin = ToPlayerOrigin;
		return *this;
	};

	CTempEnt &At (vec3f Origin)
	{
		this->Origin = Origin;
		return *this;
	};

	CTempEnt &To (CPlayerEntity *Player)
	{
		this->Player = Player;
		return *this;
	};

	CTempEnt &AssignFlags (CTempEntFlags Flags)
	{
		this->Flags = Flags;
		return *this;
	};

	// Sends to "Player" or "Origin" depending on "Flags".
	// Sends to all players if using multicast, no exceptions.
	void Send ();

	// Sends to players between first and last.
	// Can be done on both uni and multi.
	// Use to send to a range of players stored in a vector/list.
	template <class TIterator>
	void Send (TIterator first, TIterator last)
	{
		for (TIterator it = first; it != last; ++it)
			SendTo (*it);
	};

	// Sends to argument "Player".
	// Uses Flags.Flags only (forces unicast)
	void SendTo (CPlayerEntity *Player);

	// Writes the header
	virtual void SendHeader (ESVCType Enum);
};

class CExplosionBase : public CTempEnt
{
protected:
	vec3f	ExplosionOrigin;

	virtual void SendData () = 0;

public:
	CExplosionBase (vec3f ExplosionOrigin) :
	  CTempEnt (ExplosionOrigin),
	  ExplosionOrigin(ExplosionOrigin)
	{
	};

	CExplosionBase (CTempEntFlags Flags, vec3f ExplosionOrigin) :
	  CTempEnt (Flags, ExplosionOrigin),
	  ExplosionOrigin(ExplosionOrigin)
	{
	};

	CExplosionBase &ExplodeAt (vec3f ExplosionOrigin)
	{
		this->ExplosionOrigin = ExplosionOrigin;
		return *this;
	};
};

// The rocket explosion.
class CRocketExplosion : public CExplosionBase
{
protected:
	bool	Water, Particles;

	void SendData ();

public:
	CRocketExplosion (vec3f ExplosionOrigin, bool Water = false, bool Particles = true) :
	  CExplosionBase (ExplosionOrigin),
	  Water(Water),
	  Particles(Particles)
	{
	};

	CRocketExplosion (CTempEntFlags Flags, vec3f ExplosionOrigin, bool Water = false, bool Particles = true) :
	  CExplosionBase (Flags, ExplosionOrigin),
	  Water(Water),
	  Particles(Particles)
	{
	};

	CRocketExplosion &IsInWater (bool Water)
	{
		this->Water = Water;
		return *this;
	};

	CRocketExplosion &HasParticles (bool Particles)
	{
		this->Particles = Particles;
		return *this;
	};
};

// Grenade explosion
class CGrenadeExplosion : public CExplosionBase
{
protected:
	bool	Water;

	void SendData ();

public:
	CGrenadeExplosion (vec3f ExplosionOrigin, bool Water = false) :
	  CExplosionBase (ExplosionOrigin),
	  Water(Water)
	{
	};

	CGrenadeExplosion (CTempEntFlags Flags, vec3f ExplosionOrigin, bool Water = false) :
	  CExplosionBase (Flags, ExplosionOrigin),
	  Water(Water)
	{
	};

	CGrenadeExplosion &IsInWater (bool Water)
	{
		this->Water = Water;
		return *this;
	};
};

// BFG explosion
class CBFGExplosion : public CExplosionBase
{
protected:
	bool	Big;

	void SendData ();

public:
	CBFGExplosion (vec3f ExplosionOrigin, bool Big = false) :
	  CExplosionBase (ExplosionOrigin),
	  Big(Big)
	{
	};

	CBFGExplosion (CTempEntFlags Flags, vec3f ExplosionOrigin, bool Big = false) :
	  CExplosionBase (Flags, ExplosionOrigin),
	  Big(Big)
	{
	};

	CBFGExplosion &IsBig (bool Big)
	{
		this->Big = Big;
		return *this;
	};
};

// Plasma explosion
class CPlasmaExplosion : public CExplosionBase
{
protected:
	void SendData ();

public:
	CPlasmaExplosion (vec3f ExplosionOrigin) :
	  CExplosionBase (ExplosionOrigin)
	{
	};

	CPlasmaExplosion (CTempEntFlags Flags, vec3f ExplosionOrigin) :
	  CExplosionBase (Flags, ExplosionOrigin)
	{
	};
};

// Plasma explosion
class CTrackerExplosion : public CExplosionBase
{
protected:
	void SendData ();

public:
	CTrackerExplosion (vec3f ExplosionOrigin) :
	  CExplosionBase (ExplosionOrigin)
	{
	};

	CTrackerExplosion (CTempEntFlags Flags, vec3f ExplosionOrigin) :
	  CExplosionBase (Flags, ExplosionOrigin)
	{
	};
};

// Nuke explosion
class CNukeExplosion : public CExplosionBase
{
protected:
	void SendData ();

public:
	CNukeExplosion (vec3f ExplosionOrigin) :
	  CExplosionBase (ExplosionOrigin)
	{
	};

	CNukeExplosion (CTempEntFlags Flags, vec3f ExplosionOrigin) :
	  CExplosionBase (Flags, ExplosionOrigin)
	{
	};
};

// Base class for something that requires an origin and a normal
class CSplashBase : public CTempEnt
{
protected:
	vec3f		SplashOrigin, SplashNormal;

	virtual void SendData () = 0;

public:
	CSplashBase (vec3f SplashOrigin, vec3f SplashNormal) :
	  CTempEnt (SplashOrigin),
	  SplashOrigin(SplashOrigin),
	  SplashNormal(SplashNormal)
	{
	};

	CSplashBase (CTempEntFlags Flags, vec3f SplashOrigin, vec3f SplashNormal) :
	  CTempEnt (Flags, SplashOrigin),
	  SplashOrigin(SplashOrigin),
	  SplashNormal(SplashNormal)
	{
	};

	CSplashBase &SetSplashOrigin (vec3f SplashOrigin)
	{
		this->SplashOrigin = SplashOrigin;
		return *this;
	};

	CSplashBase &SetSplashNormal (vec3f SplashNormal)
	{
		this->SplashNormal = SplashNormal;
		return *this;
	};
};

// Gunshot ricochet
class CGunshotRicochet : public CSplashBase
{
protected:
	void SendData ();

public:
	CGunshotRicochet (vec3f SplashOrigin, vec3f SplashNormal) :
	  CSplashBase (SplashOrigin, SplashNormal)
	{
	};

	CGunshotRicochet (CTempEntFlags Flags, vec3f SplashOrigin, vec3f SplashNormal) :
	  CSplashBase (Flags, SplashOrigin, SplashNormal)
	{
	};
};

// Shotgun ricochet
class CShotgunRicochet : public CSplashBase
{
protected:
	void SendData ();

public:
	CShotgunRicochet (vec3f SplashOrigin, vec3f SplashNormal) :
	  CSplashBase (SplashOrigin, SplashNormal)
	{
	};

	CShotgunRicochet (CTempEntFlags Flags, vec3f SplashOrigin, vec3f SplashNormal) :
	  CSplashBase (Flags, SplashOrigin, SplashNormal)
	{
	};
};

/**
\typedef	uint8 EBloodType

\brief	Defines an alias representing type of blood temporary entity.
**/
typedef uint8 EBloodType;

/**
\enum	

\brief	Values that represent the type of blood temporary entities. 
**/
enum
{
	BT_BLOOD = TE_BLOOD,
	BT_MORE_BLOOD = TE_MOREBLOOD,
	BT_GREEN_BLOOD = TE_GREENBLOOD
};

// Blood
class CBlood : public CSplashBase
{
protected:
	EBloodType	Type;

	void SendData ();

public:
	CBlood (vec3f SplashOrigin, vec3f SplashNormal, EBloodType Type = BT_BLOOD) :
	  CSplashBase (SplashOrigin, SplashNormal),
	  Type(Type)
	{
	};

	CBlood (CTempEntFlags Flags, vec3f SplashOrigin, vec3f SplashNormal, EBloodType Type = BT_BLOOD) :
	  CSplashBase (Flags, SplashOrigin, SplashNormal),
	  Type(Type)
	{
	};

	CBlood &BloodType (EBloodType Type)
	{
		this->Type = Type;
		return *this;
	};
};

/**
\typedef	uint8 EBlasterType

\brief	Defines an alias representing type of blaster explosion.
**/
typedef uint8 EBlasterType;

/**
\enum	

\brief	Values that represent the type of blaster explosion. 
**/
enum
{
	BL_BLASTER = TE_BLASTER,
	BL_BLUE_HYPERBLASTER = TE_BLUEHYPERBLASTER,
	BL_FLECHETTE = TE_FLECHETTE,
	BL_GREEN_BLASTER = TE_BLASTER2
};

// Blaster
class CBlasterSplash : public CSplashBase
{
protected:
	EBlasterType	Type;

	void SendData ();

public:
	CBlasterSplash (vec3f SplashOrigin, vec3f SplashNormal, EBlasterType Type = BL_BLASTER) :
	  CSplashBase (SplashOrigin, SplashNormal),
	  Type(Type)
	{
	};

	CBlasterSplash (CTempEntFlags Flags, vec3f SplashOrigin, vec3f SplashNormal, EBlasterType Type = BL_BLASTER) :
	  CSplashBase (Flags, SplashOrigin, SplashNormal),
	  Type(Type)
	{
	};

	CBlasterSplash &BlasterType (EBlasterType Type)
	{
		this->Type = Type;
		return *this;
	};
};

/**
\typedef	uint8 ESplashType

\brief	Defines an alias representing type of splash particles.
**/
typedef uint8 ESplashType;

/**
\enum	

\brief	Values that represent the type of splash particles. 
**/
enum
{
	SPT_UNKNOWN,
	SPT_SPARKS,
	SPT_WATER,
	SPT_MUD,
	SPT_SLIME,
	SPT_LAVA,
	SPT_BLOOD
};

// Splash
class CSplash : public CSplashBase
{
protected:
	ESplashType		Color;
	uint8			Amount;

	virtual void SendData ();

public:
	CSplash (vec3f SplashOrigin, vec3f SplashNormal, ESplashType Color = SPT_UNKNOWN, uint8 Amount = 8) :
	  CSplashBase (SplashOrigin, SplashNormal),
	  Color(Color),
	  Amount(Amount)
	{
	};

	CSplash (CTempEntFlags Flags, vec3f SplashOrigin, vec3f SplashNormal, ESplashType Color = SPT_UNKNOWN, uint8 amount = 8) :
	  CSplashBase (Flags, SplashOrigin, SplashNormal),
	  Color(Color),
	  Amount(Amount)
	{
	};

	CSplash &SplashColor (ESplashType Color)
	{
		this->Color = Color;
		return *this;
	};

	CSplash &ParticleCount (uint8 Amount)
	{
		this->Amount = Amount;
		return *this;
	};
};

/**
\typedef	uint8 ESparkType

\brief	Defines an alias representing type of sparks.
**/
typedef uint8 ESparkType;

/**
\enum	

\brief	Values that represent the type of sparks. 
**/
enum
{
	ST_SPARKS = TE_SPARKS,
	ST_BULLET_SPARKS = TE_BULLET_SPARKS,
	ST_HEATBEAM_SPARKS = TE_HEATBEAM_SPARKS,
	ST_ELECTRIC_SPARKS = TE_ELECTRIC_SPARKS,

	// Ones that have amount/color
	ST_LASER_SPARKS = TE_LASER_SPARKS,
	ST_WELDING_SPARKS = TE_WELDING_SPARKS,
	ST_TUNNEL_SPARKS = TE_TUNNEL_SPARKS
};

// Sparks
class CSparks : public CSplash
{
protected:
	ESparkType		Type;

	void SendData ();

public:
	CSparks (vec3f SplashOrigin, vec3f SplashNormal, ESparkType Type = ST_SPARKS, ESplashType Color = SPT_UNKNOWN, uint8 Amount = 8) :
	  CSplash (SplashOrigin, SplashNormal, Color, Amount),
	  Type(Type)
	{
	};

	CSparks (CTempEntFlags Flags, vec3f SplashOrigin, vec3f SplashNormal, ESparkType Type = ST_SPARKS, ESplashType Color = SPT_UNKNOWN, uint8 Amount = 8) :
	  CSplash (Flags, SplashOrigin, SplashNormal, Color, Amount),
	  Type(Type)
	{
	};

	CSparks &SparkType (ESparkType Type)
	{
		this->Type = Type;
		return *this;
	};
};

// Shield sparks
class CShieldSparks : public CSplashBase
{
protected:
	bool	Screen;

	void SendData ();

public:
	CShieldSparks (vec3f SplashOrigin, vec3f SplashNormal, bool Screen = false) :
	  CSplashBase (SplashOrigin, SplashNormal),
	  Screen(Screen)
	{
	};

	CShieldSparks (CTempEntFlags Flags, vec3f SplashOrigin, vec3f SplashNormal, bool Screen = false) :
	  CSplashBase (Flags, SplashOrigin, SplashNormal),
	  Screen(Screen)
	{
	};

	CShieldSparks &IsScreen (bool Screen)
	{
		this->Screen = Screen;
		return *this;
	};
};

// Steam sparks
class CSteam : public CSplashBase
{
protected:
	void SendData ();
	
	uint8			Count;
	ESplashType		Color;
	sint16			Magnitude;
	sint16			ID;
	long			EndTime;

public:
	CSteam (vec3f SplashOrigin, vec3f SplashNormal, uint8 Count = 8, ESplashType Color = SPT_UNKNOWN, sint16 Magnitude = 12, sint16 ID = -1, long EndTime = 0) :
	  CSplashBase (SplashOrigin, SplashNormal),
	  Count(Count),
	  Color(Color),
	  Magnitude(Magnitude),
	  ID(ID),
	  EndTime(EndTime)
	{
	};

	CSteam (CTempEntFlags Flags, vec3f SplashOrigin, vec3f SplashNormal, uint8 Count = 8, ESplashType Color = SPT_UNKNOWN, sint16 Magnitude = 12, sint16 ID = -1, long EndTime = 0) :
	  CSplashBase (Flags, SplashOrigin, SplashNormal),
	  Count(Count),
	  Color(Color),
	  Magnitude(Magnitude),
	  ID(ID),
	  EndTime(EndTime)
	{
	};

	CSteam &ParticleCount (uint8 Count)
	{
		this->Count = Count;
		return *this;
	};

	CSteam &ParticleColor (ESplashType Color)
	{
		this->Color = Color;
		return *this;
	};
	
	CSteam &ParticleMagnitude (sint16 Magnitude)
	{
		this->Magnitude = Magnitude;
		return *this;
	};
	
	CSteam &SetID (sint16 ID)
	{
		this->ID = ID;
		return *this;
	};

	CSteam &Longetivity (long EndTime)
	{
		this->EndTime = EndTime;
		return *this;
	};
};

// Heatbeam steam
class CHeatSteam : public CSplashBase
{
protected:
	void SendData ();

public:
	CHeatSteam (vec3f SplashOrigin, vec3f SplashNormal) :
	  CSplashBase (SplashOrigin, SplashNormal)
	{
	};

	CHeatSteam (CTempEntFlags Flags, vec3f SplashOrigin, vec3f SplashNormal) :
	  CSplashBase (Flags, SplashOrigin, SplashNormal)
	{
	};
};

// Nuke explosion
class CChainfistSmoke : public CExplosionBase
{
protected:
	void SendData ();

public:
	CChainfistSmoke (vec3f ExplosionOrigin) :
	  CExplosionBase (ExplosionOrigin)
	{
	};

	CChainfistSmoke (CTempEntFlags Flags, vec3f ExplosionOrigin) :
	  CExplosionBase (Flags, ExplosionOrigin)
	{
	};
};

// Base trail code
class CTrailBase : public CTempEnt
{
protected:
	vec3f		TrailStart;
	vec3f		TrailEnd;
	sint16		EntityNumber;

	virtual void SendData () = 0;
	void SendTrailBase ();

public:
	CTrailBase (vec3f TrailStart, vec3f TrailEnd, sint16 EntityNumber = -1) :
	  CTempEnt (TrailStart),
	  TrailStart(TrailStart),
	  TrailEnd(TrailEnd),
	  EntityNumber(EntityNumber)
	{
	};

	CTrailBase (CTempEntFlags Flags, vec3f TrailStart, vec3f TrailEnd, sint16 EntityNumber = -1) :
	  CTempEnt (Flags, TrailStart),
	  TrailStart(TrailStart),
	  TrailEnd(TrailEnd),
	  EntityNumber(EntityNumber)
	{
	};

	CTrailBase &StartAt (vec3f TrailStart)
	{
		this->TrailStart = TrailStart;
		this->Origin = TrailStart;
		return *this;
	};

	CTrailBase &EndAt (vec3f TrailEnd)
	{
		this->TrailEnd = TrailEnd;
		return *this;
	};

	CTrailBase &FromEntity (sint16 EntityNumber)
	{
		this->EntityNumber = EntityNumber;
		return *this;
	};
};

// Rail trail
class CRailTrail : public CTrailBase
{
protected:
	void SendData ();

public:
	CRailTrail (vec3f TrailStart, vec3f TrailEnd) :
	  CTrailBase (TrailStart, TrailEnd)
	{
	};

	CRailTrail (CTempEntFlags Flags, vec3f TrailStart, vec3f TrailEnd) :
	  CTrailBase (Flags, TrailStart, TrailEnd)
	{
	};
};

// Bubble trail
class CBubbleTrail : public CTrailBase
{
protected:
	void SendData ();

public:
	CBubbleTrail (vec3f TrailStart, vec3f TrailEnd) :
	  CTrailBase (TrailStart, TrailEnd)
	{
	};

	CBubbleTrail (CTempEntFlags Flags, vec3f TrailStart, vec3f TrailEnd) :
	  CTrailBase (Flags, TrailStart, TrailEnd)
	{
	};
};

// Flesh cable
class CFleshCable : public CTrailBase
{
protected:
	void SendData ();

public:
	CFleshCable (vec3f TrailStart, vec3f TrailEnd, sint16 EntityNumber) :
	  CTrailBase (TrailStart, TrailEnd, EntityNumber)
	{
	};

	CFleshCable (CTempEntFlags Flags, vec3f TrailStart, vec3f TrailEnd, sint16 EntityNumber) :
	  CTrailBase (Flags, TrailStart, TrailEnd, EntityNumber)
	{
	};
};

// BFG laser
class CBFGLaser : public CTrailBase
{
protected:
	void SendData ();

public:
	CBFGLaser (vec3f TrailStart, vec3f TrailEnd) :
	  CTrailBase (TrailStart, TrailEnd)
	{
	};

	CBFGLaser (CTempEntFlags Flags, vec3f TrailStart, vec3f TrailEnd) :
	  CTrailBase (Flags, TrailStart, TrailEnd)
	{
	};
};

// Grapple cable
class CGrappleCable : public CTrailBase
{
protected:
	vec3f		Offset;

	void SendData ();

public:
	CGrappleCable (vec3f TrailStart, vec3f TrailEnd, sint16 EntityNumber, vec3f Offset = vec3fOrigin) :
	  CTrailBase (TrailStart, TrailEnd, EntityNumber),
	  Offset(Offset)
	{
	};

	CGrappleCable (CTempEntFlags Flags, vec3f TrailStart, vec3f TrailEnd, sint16 EntityNumber, vec3f Offset = vec3fOrigin) :
	  CTrailBase (Flags, TrailStart, TrailEnd, EntityNumber),
	  Offset(Offset)
	{
	};

	CGrappleCable &CableOffset (vec3f Offset)
	{
		this->Offset = Offset;
		return *this;
	};
};

// Lightning trail
class CLightning : public CTrailBase
{
protected:
	sint16		DestinationEntity;

	void SendData ();

public:
	CLightning (vec3f TrailStart, vec3f TrailEnd, sint16 SourceEntity, sint16 DestinationEntity) :
	  CTrailBase (TrailStart, TrailEnd, SourceEntity),
	  DestinationEntity(DestinationEntity)
	{
	};

	CLightning (CTempEntFlags Flags, vec3f TrailStart, vec3f TrailEnd, sint16 SourceEntity, sint16 DestinationEntity) :
	  CTrailBase (Flags, TrailStart, TrailEnd, SourceEntity),
	  DestinationEntity(DestinationEntity)
	{
	};

	CLightning &Source (sint16 SourceEntity)
	{
		this->EntityNumber = SourceEntity;
		return *this;
	};

	CLightning &Destination (sint16 DestinationEntity)
	{
		this->DestinationEntity = DestinationEntity;
		return *this;
	};
};

// Heat beam
class CHeatBeamLaser : public CTrailBase
{
protected:
	bool		Monster;

	void SendData ();

public:
	CHeatBeamLaser (vec3f TrailStart, vec3f TrailEnd, sint16 EntityNumber, bool Monster = false) :
	  CTrailBase (TrailStart, TrailEnd, EntityNumber),
	  Monster(Monster)
	{
	};

	CHeatBeamLaser (CTempEntFlags Flags, vec3f TrailStart, vec3f TrailEnd, sint16 EntityNumber, bool Monster = false) :
	  CTrailBase (Flags, TrailStart, TrailEnd, EntityNumber),
	  Monster(Monster)
	{
	};

	CHeatBeamLaser &IsMonster (bool Monster)
	{
		this->Monster = Monster;
		return *this;
	};
};

// Debug trail
class CDebugTrail : public CTrailBase
{
protected:
	void SendData ();

public:
	CDebugTrail (vec3f TrailStart, vec3f TrailEnd) :
	  CTrailBase (TrailStart, TrailEnd)
	{
	};

	CDebugTrail (CTempEntFlags Flags, vec3f TrailStart, vec3f TrailEnd) :
	  CTrailBase (Flags, TrailStart, TrailEnd)
	{
	};
};

// Force wall
class CForceWall : public CTrailBase
{
protected:
	NSColor::EColors		Color;

	void SendData ();

public:
	CForceWall (vec3f TrailStart, vec3f TrailEnd, NSColor::EColors Color) :
	  CTrailBase (TrailStart, TrailEnd),
	  Color(Color)
	{
	};

	CForceWall (CTempEntFlags Flags, vec3f TrailStart, vec3f TrailEnd, NSColor::EColors Color) :
	  CTrailBase (Flags, TrailStart, TrailEnd),
	  Color(Color)
	{
	};

	CForceWall &ParticleColor (NSColor::EColors Color)
	{
		this->Color = Color;
		return *this;
	};
};

// Flash light
class CFlashLight : public CExplosionBase
{
protected:
	sint16		EntityNumber;

	void SendData ();

public:
	CFlashLight (vec3f ExplosionOrigin, sint16 EntityNumber) :
	  CExplosionBase (ExplosionOrigin),
	  EntityNumber(EntityNumber)
	{
	};

	CFlashLight (CTempEntFlags Flags, vec3f ExplosionOrigin, sint16 EntityNumber) :
	  CExplosionBase (Flags, ExplosionOrigin),
	  EntityNumber(EntityNumber)
	{
	};

	CFlashLight &FromEntity (sint16 EntityNumber)
	{
		this->EntityNumber = EntityNumber;
		return *this;
	};
};

// Boss teleport
class CBossTeleport : public CExplosionBase
{
protected:
	void SendData ();

public:
	CBossTeleport (vec3f ExplosionOrigin) :
	  CExplosionBase (ExplosionOrigin)
	{
	};

	CBossTeleport (CTempEntFlags Flags, vec3f ExplosionOrigin) :
	  CExplosionBase (Flags, ExplosionOrigin)
	{
	};
};

// Teleport effect
class CTeleportEffect : public CExplosionBase
{
protected:
	void SendData ();

public:
	CTeleportEffect (vec3f ExplosionOrigin) :
	  CExplosionBase (ExplosionOrigin)
	{
	};

	CTeleportEffect (CTempEntFlags Flags, vec3f ExplosionOrigin) :
	  CExplosionBase (Flags, ExplosionOrigin)
	{
	};
};

// Teleport effect
class CWidowBeamOut : public CExplosionBase
{
protected:
	sint16			ID;

	void SendData ();

public:
	CWidowBeamOut (vec3f ExplosionOrigin, sint16 ID) :
	  CExplosionBase (ExplosionOrigin),
	  ID(ID)
	{
	};

	CWidowBeamOut (CTempEntFlags Flags, vec3f ExplosionOrigin, sint16 ID) :
	  CExplosionBase (Flags, ExplosionOrigin),
	  ID(ID)
	{
	};

	CWidowBeamOut &BeamID (sint16 ID)
	{
		this->ID = ID;
		return *this;
	};
};

// Widow splash
class CWidowSplash : public CExplosionBase
{
protected:
	void SendData ();

public:
	CWidowSplash (vec3f ExplosionOrigin) :
	  CExplosionBase (ExplosionOrigin)
	{
	};

	CWidowSplash (CTempEntFlags Flags, vec3f ExplosionOrigin) :
	  CExplosionBase (Flags, ExplosionOrigin)
	{
	};
};

// Muzzle flash
class CMuzzleFlash : public CExplosionBase
{
protected:
	sint16			EntityNumber;
	EMuzzleFlash	Flash;
	bool			Monster;

	void SendData ();

public:
	CMuzzleFlash (vec3f ExplosionOrigin, sint16 EntityNumber, EMuzzleFlash Flash, bool Monster = false) :
	  CExplosionBase (ExplosionOrigin),
	  EntityNumber(EntityNumber),
	  Flash(Flash),
	  Monster(Monster)
	{
	};

	CMuzzleFlash (CTempEntFlags Flags, vec3f ExplosionOrigin, sint16 EntityNumber, EMuzzleFlash Flash, bool Monster = false) :
	  CExplosionBase (Flags, ExplosionOrigin),
	  EntityNumber(EntityNumber),
	  Flash(Flash),
	  Monster(Monster)
	{
	};

	CMuzzleFlash &FromEntity (sint16 EntityNumber)
	{
		this->EntityNumber = EntityNumber;
		return *this;
	};

	CMuzzleFlash &FlashType (EMuzzleFlash Flash)
	{
		this->Flash = Flash;
		return *this;
	};

	CMuzzleFlash &IsMonsterFlash (bool Monster)
	{
		this->Monster = Monster;
		return *this;
	};
};

#else
FILE_WARNING
#endif
