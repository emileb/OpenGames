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
// cc_misc_entities.cpp
// misc_* entities
//

#include "Local.h"
#include "Entities/BrushModels.h"
#include "Entities/MiscEntities.h"
#include "Utility/TemporaryEntities.h"

/*QUAKED misc_explobox (0 .5 .8) (-16 -16 0) (16 16 40)
Large exploding box.  You can override its mass (100),
health (80), and dmg (150).
*/
const int BARREL_STEPSIZE = 8;
class CMiscExploBox : public IMapEntity, public IStepPhysics, public IHurtableEntity, public IThinkableEntity, public ITouchableEntity
{
	bool						Dropped;
	entity_ptr<IBaseEntity>		Shooter;

public:
	sint32			Explosivity;
	sint32			Damage;

	CMiscExploBox () :
	  Dropped(false),
	  IBaseEntity(),
	  IMapEntity(),
	  IThinkableEntity(),
	  IHurtableEntity (),
	  ITouchableEntity (),
	  IStepPhysics(),
	  Explosivity(0),
	  Damage(0)
	{
	};

	CMiscExploBox (sint32 Index) : 
	  Dropped(false),
	  IBaseEntity(Index),
	  IMapEntity(Index),
	  IThinkableEntity(),
	  IHurtableEntity(Index),
	  ITouchableEntity (Index),
	  IStepPhysics(Index),
	  Explosivity(0),
	  Damage(0)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CMiscExploBox)

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
		if ((!Other->GroundEntity) || (Other->GroundEntity == this))
			return;
		if (!(Other->EntityFlags & EF_PHYSICS))
			return;

		float ratio = entity_cast<IPhysicsEntity>(Other)->Mass / Mass;
		float Yaw = ((State.GetOrigin() - Other->State.GetOrigin()).ToYaw ()*M_PI*2 / 360);
		vec3f move ( cosf(Yaw)*(2 * ratio),
							sinf(Yaw)*(2 * ratio),
							0);

		vec3f	oldOrigin = State.GetOrigin(),
				newOrigin = (oldOrigin + move);

		newOrigin.Z += BARREL_STEPSIZE;
		vec3f end (newOrigin);

		end.Z -= BARREL_STEPSIZE*2;

		CTrace trace (newOrigin, GetMins(), GetMaxs(), end, this, CONTENTS_MASK_MONSTERSOLID);

		if (trace.AllSolid)
			return;

		if (trace.StartSolid)
		{
			newOrigin[2] -= BARREL_STEPSIZE;
			trace (newOrigin, GetMins(), GetMaxs(), end, this, CONTENTS_MASK_MONSTERSOLID);
			if (trace.AllSolid || trace.StartSolid)
				return;
		}

	// check point traces down for dangling corners
		State.GetOrigin() = trace.EndPosition;

		GroundEntity = trace.Entity;
		GroundEntityLinkCount = trace.Entity->GetLinkCount();

		if (trace.Fraction == 1.0)
			GroundEntity = nullentity;

	// the move is ok
		Link ();
		TouchTriggers ();
	}

	void Think ()
	{
		if (!Dropped)
		{
			Dropped = true;

			CTrace		trace;
			vec3f		origin = State.GetOrigin();

			origin.Z += 1;
			vec3f end = origin;
			end.Z -= 256;
			
			trace (origin, GetMins(), GetMaxs(), end, this, CONTENTS_MASK_MONSTERSOLID);

			if (trace.Fraction == 1 || trace.AllSolid)
				return;

			State.GetOrigin() = trace.EndPosition;
			Link();
			return;
		}
		SplashDamage (*Shooter, Damage, NULL, Damage+40, MOD_BARREL);

		CGrenadeExplosion (State.GetOrigin ()).Send();
		Free ();
	};

	// immune to slime damage
	void TakeDamage (	IBaseEntity *Inflictor, IBaseEntity *Attacker,
							vec3f Dir, vec3f Point, vec3f Normal, sint32 Damage,
							sint32 Knockback, EDamageFlags DamageFlags, EMeansOfDeath MeansOfDeath)
	{
		if (MeansOfDeath == MOD_SLIME)
			return;

		IHurtableEntity::TakeDamage (Inflictor, Attacker, Dir, Point, Normal, Damage, Knockback, DamageFlags, MeansOfDeath);
	}

	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
	{
		CanTakeDamage = false;
		NextThink = Level.Frame + 2;
		Shooter = Attacker;
	};

	bool Run ()
	{
		return IStepPhysics::Run ();
	};

	void Spawn ()
	{
		if (Game.GameMode & GAME_DEATHMATCH)
		{
			Free ();
			return;
		}

		GetSolid() = SOLID_BBOX;

		State.GetModelIndex() = ModelIndex ("models/objects/barrels/tris.md2");

		GetMins().Set (-16, -16, 0);
		GetMaxs().Set (16, 16, 40);

		if (!Explosivity)
			Explosivity = 400;
		Mass = Explosivity;

		if (!Health)
			Health = 10;

		if (!Damage)
			Damage = 150;

		CanTakeDamage = true;
		Touchable = true;
		NextThink = Level.Frame + ServerFramesPerSecond;

		Link ();
	};
};

ENTITYFIELDS_BEGIN(CMiscExploBox)
{
	CEntityField ("mass", EntityMemberOffset(CMiscExploBox,Explosivity), FT_INT | FT_SAVABLE),
	CEntityField ("dmg", EntityMemberOffset(CMiscExploBox,Damage), FT_INT | FT_SAVABLE),

	CEntityField ("Dropped", EntityMemberOffset(CMiscExploBox,Dropped), FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CMiscExploBox)

bool			CMiscExploBox::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CMiscExploBox> (this, Key, Value))
		return true;

	return (IHurtableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void		CMiscExploBox::SaveFields (CFile &File)
{
	Shooter.Write(File);
	SaveEntityFields <CMiscExploBox> (this, File);
	IMapEntity::SaveFields (File);
	IHurtableEntity::SaveFields (File);
	ITouchableEntity::SaveFields (File);
	IStepPhysics::SaveFields (File);
}

void		CMiscExploBox::LoadFields (CFile &File)
{
	Shooter = entity_ptr<IBaseEntity>::Read(File);
	LoadEntityFields <CMiscExploBox> (this, File);
	IMapEntity::LoadFields (File);
	IHurtableEntity::LoadFields (File);
	ITouchableEntity::LoadFields (File);
	IStepPhysics::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("misc_explobox",CMiscExploBox);

/*QUAKED misc_viper (1 .5 0) (-16 -16 0) (16 16 32)
This is the Viper for the flyby bombing.
It is trigger_spawned, so you must have something use it for it to show up.
There must be a path for it to follow once it is activated.

"speed"		How fast the Viper should fly
*/

CMiscViper::CMiscViper() :
	IBaseEntity (),
	CTrainBase(),
	ITouchableEntity(),
	MyUse(true)
{
};

CMiscViper::CMiscViper(sint32 Index) :
	IBaseEntity (Index),
	CTrainBase(Index),
	ITouchableEntity(Index),
	MyUse(true)
{
};

void CMiscViper::SaveFields (CFile &File)
{
	File.Write<bool> (MyUse);
	CTrainBase::SaveFields (File);
	ITouchableEntity::SaveFields (File);
}

void CMiscViper::LoadFields (CFile &File)
{
	MyUse = File.Read<bool> ();
	CTrainBase::LoadFields (File);
	ITouchableEntity::LoadFields (File);
}

bool CMiscViper::Run ()
{
	return CTrainBase::Run ();
};

void CMiscViper::Use (IBaseEntity *Other, IBaseEntity *Activator)
{
	if (MyUse)
	{
		GetSvFlags() &= ~SVF_NOCLIENT;
		MyUse = false;
	}
	CTrainBase::Use (Other, Activator);
};

void CMiscViper::Spawn ()
{
	if (Target.IsNullOrEmpty())
	{
		MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No targetname\n");
		Free ();
		return;
	}

	if (!Speed)
		Speed = 300;

	PhysicsType = PHYSICS_PUSH;
	Touchable = true;
	GetSolid() = SOLID_NOT;
	State.GetModelIndex() = ModelIndex ("models/ships/viper/tris.md2");
	GetMins().Set (-16, -16, 0);
	GetMaxs().Set (16, 16, 32);

	NextThink = Level.Frame + ServerFramesPerSecond;
	ThinkType = TRAINTHINK_FIND;
	GetSvFlags() |= SVF_NOCLIENT;
	MoveSpeed = MoveAccel = MoveDecel = Speed;

	Link ();
};

LINK_CLASSNAME_TO_CLASS ("misc_viper", CMiscViper);

class CMiscStroggShip : public CMiscViper
{
public:
	CMiscStroggShip () :
	  IBaseEntity (),
	  CMiscViper ()
	  {
	  };

	CMiscStroggShip (sint32 Index) :
	  IBaseEntity (Index),
	  CMiscViper (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER (CMiscStroggShip);

	bool Run ()
	{
		return CTrainBase::Run ();
	};

	void Spawn ()
	{
		CMiscViper::Spawn ();
		State.GetModelIndex() = ModelIndex ("models/ships/strogg1/tris.md2");
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_strogg_ship", CMiscStroggShip);

/*QUAKED misc_banner (1 .5 0) (-4 -4 -4) (4 4 4)
The origin is the bottom of the banner.
The banner is 128 tall.
*/
class CMiscBanner : public IMapEntity, public IThinkableEntity
{
public:
	CMiscBanner () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity ()
	{
	};

	CMiscBanner (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscBanner)

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

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Think ()
	{
		State.GetFrame() = (State.GetFrame() + 1) % 16;
		NextThink = Level.Frame + ServerFramesPerSecond;
	};

	void Spawn ()
	{
		GetSolid() = SOLID_NOT;
		State.GetModelIndex() = ModelIndex ("models/objects/banner/tris.md2");
		State.GetFrame() = irandom(16);
		Link ();

		NextThink = Level.Frame + ServerFramesPerSecond;
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_banner", CMiscBanner);

/*QUAKED misc_blackhole (1 .5 0) (-8 -8 -8) (8 8 8)
*/
class CMiscBlackhole : public IMapEntity, public IThinkableEntity, public IUsableEntity
{
public:
	CMiscBlackhole () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity (),
	  IUsableEntity ()
	{
	};

	CMiscBlackhole (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index),
	  IUsableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscBlackhole)

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);
	}

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Think ()
	{
		if (++State.GetFrame() >= 19)
			State.GetFrame() = 0;
		NextThink = Level.Frame + ServerFramesPerSecond;
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		Free ();
	};

	void Spawn ()
	{
		GetSolid() = SOLID_NOT;
		GetMins().Set (-64, -64, 0);
		GetMaxs().Set (64, 64, 8);
		State.GetModelIndex() = ModelIndex ("models/objects/black/tris.md2");
		State.GetRenderEffects() = RF_TRANSLUCENT;
		NextThink = Level.Frame + 2;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_blackhole", CMiscBlackhole);

/*QUAKED misc_eastertank (1 .5 0) (-32 -32 -16) (32 32 32)
*/
class CMiscEasterTank : public IMapEntity, public IThinkableEntity
{
public:
	CMiscEasterTank () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity ()
	{
	};

	CMiscEasterTank (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscEasterTank)

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

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Think ()
	{
		if (++State.GetFrame() >= 293)
			State.GetFrame() = 254;

		NextThink = Level.Frame + ServerFramesPerSecond;
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		GetMins().Set (-32, -32, -16);
		GetMaxs().Set (32, 32, 32);
		State.GetModelIndex() = ModelIndex ("models/monsters/tank/tris.md2");
		State.GetFrame() = 254;
		NextThink = Level.Frame + 2;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_eastertank", CMiscEasterTank);

/*QUAKED misc_easterchick (1 .5 0) (-32 -32 0) (32 32 32)
*/
class CMiscEasterChick : public IMapEntity, public IThinkableEntity
{
public:
	CMiscEasterChick () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity ()
	{
	};

	CMiscEasterChick (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscEasterChick)

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

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Think ()
	{
		if (++State.GetFrame() >= 247)
			State.GetFrame() = 208;

		NextThink = Level.Frame + ServerFramesPerSecond;
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		GetMins().Set (-32, -32, 0);
		GetMaxs().Set (32, 32, 32);
		State.GetModelIndex() = ModelIndex ("models/monsters/bitch/tris.md2");
		State.GetFrame() = 208;
		NextThink = Level.Frame + 2;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_easterchick", CMiscEasterChick);

/*QUAKED misc_easterchick2 (1 .5 0) (-32 -32 0) (32 32 32)
*/
class CMiscEasterChick2 : public IMapEntity, public IThinkableEntity
{
public:
	CMiscEasterChick2 () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity ()
	{
	};

	CMiscEasterChick2 (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscEasterChick2)

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

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Think ()
	{
		if (++State.GetFrame() >= 287)
			State.GetFrame() = 248;

		NextThink = Level.Frame + ServerFramesPerSecond;
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		GetMins().Set (-32, -32, 0);
		GetMaxs().Set (32, 32, 32);
		State.GetModelIndex() = ModelIndex ("models/monsters/bitch/tris.md2");
		State.GetFrame() = 248;
		NextThink = Level.Frame + 2;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_easterchick2", CMiscEasterChick2);

/*QUAKED monster_commander_body (1 .5 0) (-32 -32 0) (32 32 48)
Not really a monster, this is the Tank Commander's decapitated body.
There should be a item_commander_head that has this as it's target.
*/
class CCommanderBody : public IMapEntity, public IThinkableEntity, public IUsableEntity, public ITossProjectile, public IHurtableEntity
{
public:
	bool Drop;

	CCommanderBody () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity (),
	  IUsableEntity (),
	  ITossProjectile (),
	  Drop(true)
	{
	};

	CCommanderBody (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index),
	  IUsableEntity (Index),
	  ITossProjectile (Index),
	  Drop(true)
	{
	};

	IMPLEMENT_SAVE_HEADER(CCommanderBody)

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (IUsableEntity::ParseField (Key, Value) || IHurtableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	};

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
		IHurtableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);
		IHurtableEntity::LoadFields (File);
	}

	bool Run ()
	{
		return (PhysicsType == PHYSICS_TOSS) ? ITossProjectile::Run() : IBaseEntity::Run();
	};

	void Think ()
	{
		if (!Drop)
		{
			NextThink = (++State.GetFrame() < 24) ? Level.Frame + ServerFramesPerSecond : 0;
			if (State.GetFrame() == 22)
				PlaySound (CHAN_BODY, SoundIndex ("tank/thud.wav"));
		}
		else
		{
			Drop = false;
			PhysicsType = PHYSICS_TOSS;
			State.GetOrigin().Z += 2;
		}
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		NextThink = Level.Frame + ServerFramesPerSecond;
		PlaySound (CHAN_BODY, SoundIndex ("tank/pain.wav"));
	};

	void Spawn ()
	{
		PhysicsType = PHYSICS_NONE;
		GetSolid() = SOLID_BBOX;
		State.GetModelIndex() = ModelIndex ("models/monsters/commandr/tris.md2");
		GetMins().Set (-32, -32, 0);
		GetMaxs().Set (32, 32, 48);
		CanTakeDamage = true;
		Flags = FL_GODMODE;
		State.GetRenderEffects() |= RF_FRAMELERP;
		Link ();

		SoundIndex ("tank/thud.wav");
		SoundIndex ("tank/pain.wav");

		NextThink = Level.Frame + 5;
	};
};

LINK_CLASSNAME_TO_CLASS ("monster_commander_body", CCommanderBody);

/*QUAKED misc_deadsoldier (1 .5 0) (-16 -16 0) (16 16 16) ON_BACK ON_STOMACH BACK_DECAP FETAL_POS SIT_DECAP IMPALED
This is the dead player model. Comes in 6 exciting different poses!
*/

/**
\enum	

\brief	Values that represent spawnflags pertaining to CMiscDeadSoldier. 
**/
enum
{
	DEADSOLDIER_ON_BACK			= BIT(0),
	DEADSOLDIER_ON_STOMACH		= BIT(1),
	DEADSOLDIER_BACK_DECAP		= BIT(2),
	DEADSOLDIER_FETAL_POS		= BIT(3),
	DEADSOLDIER_SIT_DECAP		= BIT(4),
	DEADSOLDIER_IMPALED			= BIT(5)
};

class CMiscDeadSoldier : public IMapEntity, public IHurtableEntity, public IThinkableEntity, public ITossProjectile
{
public:
	CMiscDeadSoldier () :
	  IBaseEntity (),
	  IMapEntity (),
	  IHurtableEntity (),
	  IThinkableEntity (),
	  ITossProjectile ()
	{
	};

	CMiscDeadSoldier (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IHurtableEntity (Index),
	  IThinkableEntity (Index),
	  ITossProjectile (Index)
	{
	};

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (IHurtableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	};

	IMPLEMENT_SAVE_HEADER(CMiscDeadSoldier)

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IHurtableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
		ITossProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IHurtableEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);
		ITossProjectile::LoadFields (File);
	}

	bool Run ()
	{
		switch (PhysicsType)
		{
		case PHYSICS_NONE:
		default:
			return IBaseEntity::Run();
		case PHYSICS_TOSS:
			return ITossProjectile::Run ();
		case PHYSICS_BOUNCE:
			return IBounceProjectile::Run ();
		};
	};

	void DamageEffect (vec3f &Dir, vec3f &Point, vec3f &Normal, sint32 &Damage, EDamageFlags &DamageFlags, EMeansOfDeath &MeansOfDeath)
	{
		CBlood(Point, Normal).Send();
	}

	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
	{
		if (Health > -80)
			return;

		PlaySound (CHAN_BODY, SoundIndex ("misc/udeath.wav"));
		for (sint32 n = 0; n < 4; n++)
			CGibEntity::Spawn (this, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
		TossHead (GameMedia.Gib_Head[1], Damage, GIB_ORGANIC);
	};

	void TossHead (MediaIndex gibIndex, sint32 Damage, sint32 type)
	{
		float	vscale;

		State.GetSkinNum() = 0;
		State.GetFrame() = 0;

		GetMins().Clear ();
		GetMaxs().Clear ();

		State.GetModelIndex(2) = 0;
		State.GetModelIndex() = gibIndex;
		GetSolid() = SOLID_NOT;
		State.GetEffects() |= FX_GIB;
		State.GetEffects() &= ~FX_FLIES;
		State.GetSound() = 0;
		AffectedByKnockback = false;
		GetSvFlags() &= ~SVF_MONSTER;
		CanTakeDamage = true;

		if (type == GIB_ORGANIC)
		{
			PhysicsType = PHYSICS_TOSS;
			vscale = 0.5;
			BackOff = 1.0f;
		}
		else
		{
			PhysicsType = PHYSICS_BOUNCE;
			BackOff = 1.5f;
			vscale = 1.0;
		}

		vec3f vd = VelocityForDamage (Damage);
		
		vec3f velocity (Velocity);
		velocity.MultiplyAngles (vscale, vd);
		Velocity = velocity;

		Velocity.X = Clamp<float> (Velocity.X, -300, 300);
		Velocity.Y = Clamp<float> (Velocity.Y, -300, 300);
		Velocity.Z = Clamp<float> (Velocity.Z, 200, 500); // always some upwards

		AngularVelocity.Y = crand()*600;

		NextThink = Level.Frame + 100 + frand()*100;

		Link();
	};

	void Think ()
	{
		Free ();
	};

	void Spawn ()
	{
		if (Game.GameMode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		PhysicsType = PHYSICS_NONE;
		GetSolid() = SOLID_BBOX;
		State.GetModelIndex() = ModelIndex ("models/deadbods/dude/tris.md2");

		// Defaults to frame 0
		if (SpawnFlags & DEADSOLDIER_ON_STOMACH)
			State.GetFrame() = 1;
		else if (SpawnFlags & DEADSOLDIER_BACK_DECAP)
			State.GetFrame() = 2;
		else if (SpawnFlags & DEADSOLDIER_FETAL_POS)
			State.GetFrame() = 3;
		else if (SpawnFlags & DEADSOLDIER_SIT_DECAP)
			State.GetFrame() = 4;
		else if (SpawnFlags & DEADSOLDIER_IMPALED)
			State.GetFrame() = 5;
		else
			State.GetFrame() = 0;

		GetMins().Set (-16, -16, 0);
		GetMaxs().Set (16, 16, 16);
		IsDead = true;
		CanTakeDamage = true;
		GetSvFlags() |= (SVF_MONSTER|SVF_DEADMONSTER);

		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_deadsoldier", CMiscDeadSoldier);

/*QUAKED misc_bigviper (1 .5 0) (-176 -120 -24) (176 120 72) 
This is a large stationary viper as seen in Paul's intro
*/
class CMiscBigViper : public IMapEntity
{
public:
	CMiscBigViper () :
	  IBaseEntity (),
	  IMapEntity ()
	{
	};

	CMiscBigViper (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscBigViper)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		GetMins().Set (-176, -120, -24);
		GetMaxs().Set (176, 120, 72);
		State.GetModelIndex() = ModelIndex ("models/ships/bigviper/tris.md2");
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_bigviper", CMiscBigViper);

/*QUAKED misc_viper_bomb (1 0 0) (-8 -8 -8) (8 8 8)
"dmg"	how much boom should the bomb make?
*/
class CMiscViperBomb : public IMapEntity, public IThinkableEntity, public ITouchableEntity, public IUsableEntity, public ITossProjectile
{
public:
	bool			PreThinkable;
	FrameNumber	TimeStamp;
	vec3f			MoveDir;
	sint32			Damage;

	CMiscViperBomb () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity (),
	  ITouchableEntity (),
	  IUsableEntity (),
	  ITossProjectile (),
	  PreThinkable (false),
	  TimeStamp (0),
	  MoveDir(),
	  Damage(0)
	{
	};

	CMiscViperBomb (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index),
	  ITouchableEntity (Index),
	  IUsableEntity (Index),
	  ITossProjectile (Index),
	  PreThinkable (false),
	  TimeStamp (0),
	  MoveDir(),
	  Damage(0)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CMiscViperBomb)

	bool Run ()
	{
		return (PhysicsType == PHYSICS_TOSS) ? ITossProjectile::Run() : IBaseEntity::Run();
	};

	void Think ()
	{
	};
	void PreThink ()
	{
		GroundEntity = nullentity;

		float diff = TimeStamp - Level.Frame;
		if (diff < -1.0)
			diff = -1.0;

		vec3f v = MoveDir * (1.0f + diff);
		v.Z = diff;

		diff = State.GetAngles().Z;

		vec3f angles = v.ToAngles();
		angles.Z = diff + 10;
		State.GetAngles() = angles;
	};

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
		UseTargets (*User, Message);

		State.GetOrigin().Z = GetAbsMin().Z + 1;
		SplashDamage (this, Damage, NULL, Damage+40, MOD_BOMB);
		BecomeExplosion (true);

		User = nullentity;
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		if (!Usable)
			return;

		GetSolid() = SOLID_BBOX;
		GetSvFlags() &= ~SVF_NOCLIENT;
		State.GetEffects() |= FX_ROCKET;
		Usable = false;
		PhysicsType = PHYSICS_TOSS;
		PreThinkable = true;
		Touchable = true;
		User = Activator;

		CMiscViper *viper = CC_FindByClassName<CMiscViper, EF_BASE> (NULL, "misc_viper");

		Velocity = viper->Dir * viper->MoveSpeed;

		TimeStamp = Level.Frame;
		MoveDir = viper->MoveDir;
	};


	void Spawn ()
	{
		PhysicsType = PHYSICS_NONE;
		Touchable = false;
		GetSolid() = SOLID_NOT;
		GetMins().Set (-8);
		GetMaxs().Set (8);

		State.GetModelIndex() = ModelIndex ("models/objects/bomb/tris.md2");

		if (!Damage)
			Damage = 1000;

		GetSvFlags() |= SVF_NOCLIENT;
		Link ();
	};
};

ENTITYFIELDS_BEGIN(CMiscViperBomb)
{
	CEntityField ("dmg", EntityMemberOffset(CMiscViperBomb,Damage), FT_INT | FT_SAVABLE),

	CEntityField ("PreThinkable", EntityMemberOffset(CMiscViperBomb,PreThinkable), FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("TimeStamp", EntityMemberOffset(CMiscViperBomb,TimeStamp), FT_FRAMENUMBER | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("MoveDir", EntityMemberOffset(CMiscViperBomb,MoveDir), FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CMiscViperBomb)

bool			CMiscViperBomb::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CMiscViperBomb> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void		CMiscViperBomb::SaveFields (CFile &File)
{
	SaveEntityFields <CMiscViperBomb> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
	ITouchableEntity::SaveFields (File);
	ITossProjectile::SaveFields (File);
}

void		CMiscViperBomb::LoadFields (CFile &File)
{
	LoadEntityFields <CMiscViperBomb> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
	ITouchableEntity::LoadFields (File);
	ITossProjectile::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("misc_viper_bomb", CMiscViperBomb);

/*QUAKED misc_satellite_dish (1 .5 0) (-64 -64 0) (64 64 128)
*/
class CMiscSattelite : public IMapEntity, public IThinkableEntity, public IUsableEntity
{
public:
	CMiscSattelite () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity (),
	  IUsableEntity ()
	{
	};

	CMiscSattelite (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index),
	  IUsableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscSattelite)

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);
	}

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Think ()
	{
		if (++State.GetFrame() < 38)
			NextThink = Level.Frame + ServerFramesPerSecond;
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		NextThink = Level.Frame + ServerFramesPerSecond;
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		GetMins().Set (-64, -64, 0);
		GetMaxs().Set (64, 64, 128);
		State.GetModelIndex() = ModelIndex ("models/objects/satellite/tris.md2");
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_satellite_dish", CMiscSattelite);

/*QUAKED light_mine1 (0 1 0) (-2 -2 -12) (2 2 12)
*/
class CLightMine1 : public IMapEntity
{
public:
	CLightMine1 () :
	  IBaseEntity (),
	  IMapEntity ()
	{
	};

	CLightMine1 (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CLightMine1)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		State.GetModelIndex() = ModelIndex ("models/objects/minelite/light1/tris.md2");
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("light_mine1", CLightMine1);

/*QUAKED light_mine2 (0 1 0) (-2 -2 -12) (2 2 12)
*/
class CLightMine2 : public IMapEntity
{
public:
	CLightMine2 () :
	  IBaseEntity (),
	  IMapEntity ()
	{
	};

	CLightMine2 (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CLightMine2)

	bool Run ()
	{
		return IBaseEntity::Run();
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		State.GetModelIndex() = ModelIndex ("models/objects/minelite/light2/tris.md2");
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("light_mine2", CLightMine2);

/*QUAKED misc_gib_arm (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
class CMiscGibArm : public IMapEntity, public IThinkableEntity, public ITossProjectile, public IHurtableEntity
{
public:
	CMiscGibArm () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity (),
	  ITossProjectile ()
	{
	};

	CMiscGibArm (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index),
	  ITossProjectile (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscGibArm)

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (IHurtableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	};

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IHurtableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
		ITossProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IHurtableEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);
		ITossProjectile::LoadFields (File);
	}

	bool Run ()
	{
		return ITossProjectile::Run();
	};

	void Think ()
	{
		Free ();
	};

	void Spawn ()
	{
		State.GetModelIndex() = GameMedia.Gib_Arm();
		GetSolid() = SOLID_NOT;
		State.GetEffects() |= FX_GIB;
		CanTakeDamage = true;
		PhysicsType = PHYSICS_TOSS;
		GetSvFlags() |= SVF_MONSTER;
		AngularVelocity.Set (frand()*200, frand()*200, frand()*200);
		NextThink = Level.Frame + 300;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_gib_arm", CMiscGibArm);

/*QUAKED misc_gib_leg (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
class CMiscGibLeg : public IMapEntity, public IThinkableEntity, public ITossProjectile, public IHurtableEntity
{
public:
	CMiscGibLeg () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity (),
	  ITossProjectile ()
	{
	};

	CMiscGibLeg (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index),
	  ITossProjectile (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscGibLeg)

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (IHurtableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	};

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IHurtableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
		ITossProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IHurtableEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);
		ITossProjectile::LoadFields (File);
	}

	bool Run ()
	{
		return ITossProjectile::Run();
	};

	void Think ()
	{
		Free ();
	};

	void Spawn ()
	{
		State.GetModelIndex() = GameMedia.Gib_Leg();
		GetSolid() = SOLID_NOT;
		State.GetEffects() |= FX_GIB;
		CanTakeDamage = true;
		PhysicsType = PHYSICS_TOSS;
		GetSvFlags() |= SVF_MONSTER;
		AngularVelocity.Set (frand()*200, frand()*200, frand()*200);
		NextThink = Level.Frame + 300;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_gib_leg", CMiscGibLeg);

/*QUAKED misc_gib_head (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
class CMiscGibHead : public IMapEntity, public IThinkableEntity, public ITossProjectile, public IHurtableEntity
{
public:
	CMiscGibHead () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity (),
	  ITossProjectile ()
	{
	};

	CMiscGibHead (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index),
	  ITossProjectile (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscGibHead)

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (IHurtableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	};

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IHurtableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
		ITossProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IHurtableEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);
		ITossProjectile::LoadFields (File);
	}

	bool Run ()
	{
		return ITossProjectile::Run();
	};

	void Think ()
	{
		Free ();
	};

	void Spawn ()
	{
		State.GetModelIndex() = GameMedia.Gib_Head[0];
		GetSolid() = SOLID_NOT;
		State.GetEffects() |= FX_GIB;
		CanTakeDamage = true;
		PhysicsType = PHYSICS_TOSS;
		GetSvFlags() |= SVF_MONSTER;
		AngularVelocity.Set (frand()*200, frand()*200, frand()*200);
		NextThink = Level.Frame + 300;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_gib_head", CMiscGibHead);

//=================================================================================

class CMiscModel : public IMapEntity
{
public:
	CMiscModel () :
	  IBaseEntity (),
	  IMapEntity ()
	  {
	  };

	CMiscModel (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER(CMiscModel)

	void Spawn ()
	{
		Free ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_model", CMiscModel);


