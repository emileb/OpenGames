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
// cc_rogue_misc_entities.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES
#include "Entities/BrushModels.h"
#include "Rogue/RogueMiscEntities.h"

class CMiscNukeCore : public IMapEntity, public IUsableEntity
{
public:
	CMiscNukeCore () :
	  IMapEntity (),
	  IUsableEntity ()
	  {
	  };

	CMiscNukeCore (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index)
	  {
	  };

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
	}

	IMPLEMENT_SAVE_HEADER(CMiscNukeCore);

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		GetSvFlags() ^= SVF_NOCLIENT;
	}

	void Spawn ()
	{
		State.GetModelIndex() = ModelIndex("models/objects/core/tris.md2");
		Usable = true;
		Link ();
	}
};

LINK_CLASSNAME_TO_CLASS ("misc_nuke_core", CMiscNukeCore);

#include "Utility/TemporaryEntities.h"

/*QUAKED func_door_secret2 (0 .5 .8) ? open_once 1st_left 1st_down no_shoot always_shoot slide_right slide_forward
Basic secret door. Slides back, then to the left. Angle determines direction.

FLAGS:
open_once = not implemented yet
1st_left = 1st move is left/right of arrow
1st_down = 1st move is forwards/backwards
no_shoot = not implemented yet
always_shoot = even if targeted, keep shootable
reverse_left = the sideways move will be to right of arrow
reverse_back = the to/from move will be forward

VALUES:
wait = # of seconds before coming back (5 default)
dmg  = damage to inflict when blocked (2 default)
*/

/**
\enum	

\brief	Values that represent spawnflags pertaining to CDoorSecret2. 
**/
enum
{
	SEC_OPEN_ONCE				= BIT(0),
	SEC_1ST_LEFT				= BIT(1),
	SEC_1ST_DOWN				= BIT(2),
	SEC_NO_SHOOT				= BIT(3),
	SEC_YES_SHOOT				= BIT(4),
	SEC_MOVE_RIGHT				= BIT(5),
	SEC_MOVE_FORWARD			= BIT(6)
};

class CDoorSecret2 : public CDoor
{
public:
	enum
	{
		DOORSECRETTHINK_6 = BRUSHTHINK_CUSTOM_START,
		DOORSECRETTHINK_4,
		DOORSECRETTHINK_2
	};

	enum
	{
		DOORSECRETENDFUNC_DONE,
		DOORSECRETENDFUNC_5,
		DOORSECRETENDFUNC_3,
		DOORSECRETENDFUNC_1
	};

	CDoorSecret2 () :
	  IBaseEntity(),
	  CDoor ()
	  {
		  BrushType |= BRUSH_SECRETDOOR;
	  };

	CDoorSecret2 (sint32 Index) :
	  IBaseEntity(Index),
	  CDoor(Index)
	  {
		  BrushType |= BRUSH_SECRETDOOR;
	  };

	IMPLEMENT_SAVE_HEADER(CDoorSecret2)

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
		if (!(Other->EntityFlags & EF_PLAYER))
			return;

		if (Level.Frame < TouchDebounce)
			return;

		TouchDebounce = Level.Frame + 20;

		(entity_cast<CPlayerEntity>(Other))->PrintToClient (PRINT_CENTER, "%s", Message.CString());
	}
	void Blocked (IBaseEntity *Other)
	{
		if (!IsSlave() && (Other->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(Other)->CanTakeDamage)
			entity_cast<IHurtableEntity>(Other)->TakeDamage (this, this, vec3fOrigin, Other->State.GetOrigin(), vec3fOrigin, Damage, 0, 0, MOD_CRUSH);
	}
	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		if (IsSlave())
			return;

		// trigger all paired doors
		for (IBaseEntity *ent = this; ent; ent = ent->Team.Chain)
		{
			if (ent->EntityFlags & EF_BRUSHMODEL)
				entity_cast<IBrushModel>(ent)->MoveCalc (StartOrigin, DOORSECRETENDFUNC_1);
		}
	}
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
	{
		Health = MaxHealth;
		CanTakeDamage = true;

		if (IsSlave() && Team.Master && (Team.Master->EntityFlags & EF_HURTABLE) && (entity_cast<IHurtableEntity>(Team.Master)->CanTakeDamage))
			entity_cast<IHurtableEntity>(Team.Master)->Die (Inflictor, Attacker, Damage, Point);
		else
			Use (Inflictor, Attacker);
	}

	void DoEndFunc ()
	{
		switch (EndFunc)
		{
			case DOORSECRETENDFUNC_DONE:
				if (TargetName.IsNullOrEmpty() || (SpawnFlags & SEC_YES_SHOOT))
				{
					Health = 1;
					CanTakeDamage = true;
				}
				break;
			case DOORSECRETENDFUNC_5:
				NextThink = Level.Frame + 10;
				ThinkType = DOORSECRETTHINK_6;
				break;
			case DOORSECRETENDFUNC_3:
				if (!(SpawnFlags & SEC_OPEN_ONCE))
				{
					NextThink = Level.Frame + Wait;
					ThinkType = DOORSECRETTHINK_4;
				}
				break;
			case DOORSECRETENDFUNC_1:
				NextThink = Level.Frame + 10;
				ThinkType = DOORSECRETTHINK_2;
				break;
		};
	};
	void Think ()
	{
		switch (ThinkType)
		{
		case DOORSECRETTHINK_6:
			MoveCalc (MoveOrigin, DOORSECRETENDFUNC_DONE);
			break;
		case DOORSECRETTHINK_4:
			MoveCalc (StartOrigin, DOORSECRETENDFUNC_5);
			break;
		case DOORSECRETTHINK_2:
			MoveCalc (EndOrigin, DOORSECRETENDFUNC_3);
			break;
		default:
			IBrushModel::Think ();
		}
	};

	void Spawn ()
	{
		SoundStart = SoundIndex  ("doors/dr1_strt.wav");
		SoundMiddle = SoundIndex  ("doors/dr1_mid.wav");
		SoundEnd = SoundIndex  ("doors/dr1_end.wav");

		PhysicsType = PHYSICS_PUSH;
		GetSolid() = SOLID_BSP;
		SetBrushModel ();

		if (TargetName.IsNullOrEmpty() || (SpawnFlags & SEC_YES_SHOOT))
		{
			Health = 1;
			MaxHealth = Health;
			CanTakeDamage = true;
		}

		Touchable = false;

		if (!Damage)
			Damage = 2;

		if (!Wait)
			Wait = 50;

		Accel =
		Decel =
		Speed = 50;

		// calculate positions
		anglef angles = State.GetAngles().ToVectors ();
		MoveOrigin = State.GetOrigin();
		MoveAngles = State.GetAngles();

		SetMoveDir ();
		PhysicsType = PHYSICS_PUSH;
		GetSolid() = SOLID_BSP;
		SetBrushModel ();

		float lrSize = 0, fbSize = 0;
		if(MoveAngles.Y == 0 || MoveAngles.Y == 180)
		{
			lrSize = GetSize().Y;
			fbSize = GetSize().X;
		}		
		else if(MoveAngles.Y == 90 || MoveAngles.Y == 270)
		{
			lrSize = GetSize().X;
			fbSize = GetSize().Y;
		}		
		else
			MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Secret door not at 0, 90, 180, 270!\n");

		if (SpawnFlags & SEC_MOVE_FORWARD)
			angles.Forward *= fbSize;
		else
			angles.Forward *= (fbSize * -1);

		if (SpawnFlags & SEC_MOVE_RIGHT)
			angles.Right *= lrSize;
		else
			angles.Right *= (lrSize * -1);

		if (SpawnFlags & SEC_1ST_DOWN)
		{
			StartOrigin = State.GetOrigin() + angles.Forward;
			EndOrigin = StartOrigin + angles.Right;
		}
		else
		{
			StartOrigin = State.GetOrigin() + angles.Right;
			EndOrigin = StartOrigin + angles.Forward;
		}

		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("func_door_secret2", CDoorSecret2);

/**
\enum	

\brief	Values that represent spawnflags pertaining to CFuncForceWall. 
**/
enum
{
	FWALL_START_ON		=		BIT(0),
};

/*QUAKED func_force_wall (1 0 1) ? start_on
A vertical particle force wall. Turns on and solid when triggered.
If someone is in the force wall when it turns on, they're telefragged.

start_on - forcewall begins activated. triggering will turn it off.
style - color of particles to use.
	208: green, 240: red, 241: blue, 224: orange
*/
class CFuncForceWall : public IMapEntity, public IUsableEntity, public IBrushModel
{
public:
	vec3f		Start, End, SendOrigin;
	uint8		Style;
	bool		Telefrag;

	CFuncForceWall () :
	  IMapEntity (),
	  IUsableEntity (),
	  IBrushModel ()
	  {
	  };

	CFuncForceWall (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index),
	  IBrushModel (Index)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CFuncForceWall)

	bool Run ()
	{
		return false;
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		if (!Telefrag)
		{
			Telefrag = 1;
			NextThink = 0;
			GetSolid() = SOLID_NOT;
			Link ();
		}
		else
		{
			Telefrag = false;
			NextThink = Level.Frame + ServerFramesPerSecond;
			GetSolid() = SOLID_BSP;
			KillBox();		// Is this appropriate?
			Link ();
		}
	};
	
	void Think ()
	{
		if (!Telefrag)
			CForceWall(Start, End, Style).Send();

		NextThink = Level.Frame + ServerFramesPerSecond;
	};

	void Spawn ()
	{
		SetBrushModel ();

		SendOrigin = (GetAbsMax() + GetAbsMin()) / 2;
		Start.Z = End.Z = GetAbsMax().Z;

		if (GetSize().X > GetSize().Y)
		{
			Start.X = GetAbsMin().X;
			End.X = GetAbsMax().X;
			Start.Y = End.Y = SendOrigin.Y;
		}
		else
		{
			Start.X = End.X = SendOrigin.X;
			Start.Y = GetAbsMin().Y;
			End.Y = GetAbsMax().Y;
		}
	
		if (!Style)
			Style = 208;

		PhysicsType = PHYSICS_NONE;
		Telefrag = true;

		if (SpawnFlags & FWALL_START_ON)
		{
			GetSolid() = SOLID_BSP;
			NextThink = Level.Frame + ServerFramesPerSecond;
		}
		else
			GetSolid() = SOLID_NOT;

		Usable = true;
		GetSvFlags() = SVF_NOCLIENT;
	
		Link ();
	};
};

ENTITYFIELDS_BEGIN(CFuncForceWall)
{
	CEntityField ("style",		EntityMemberOffset(CFuncForceWall,Style),			FT_BYTE | FT_SAVABLE),
		
	CEntityField ("Start",		EntityMemberOffset(CFuncForceWall,Start),			FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("End",		EntityMemberOffset(CFuncForceWall,End),				FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("SendOrigin",	EntityMemberOffset(CFuncForceWall,SendOrigin),		FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("Telefrag",	EntityMemberOffset(CFuncForceWall,Telefrag),		FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CFuncForceWall)

bool			CFuncForceWall::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CFuncForceWall> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IMapEntity::ParseField(Key, Value) || IUsableEntity::ParseField(Key, Value) || IBrushModel::ParseField(Key, Value));
};

void			CFuncForceWall::SaveFields (CFile &File)
{
	SaveEntityFields <CFuncForceWall> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
	IBrushModel::SaveFields (File);
}

void			CFuncForceWall::LoadFields (CFile &File)
{
	LoadEntityFields <CFuncForceWall> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
	IBrushModel::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("func_force_wall", CFuncForceWall);

/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for lightning.
*/
class CInfoTeleportDestination : public IMapEntity
{
public:
	CInfoTeleportDestination (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER(CInfoTeleportDestination)

	void Spawn ()
	{
	};
};

LINK_CLASSNAME_TO_CLASS ("info_teleport_destination", CInfoTeleportDestination);

#include "Rogue/RogueWeaponry.h"

/**
\enum	

\brief	Values that represent spawnflags pertaining to CPlatForm2. 
**/
enum
{
	PLAT2_TOGGLE			= BIT(1),
	PLAT2_TOP				= BIT(2),
	PLAT2_TRIGGER_TOP		= BIT(3),
	PLAT2_TRIGGER_BOTTOM	= BIT(4),
	PLAT2_BOX_LIFT			= BIT(5)
};

CPlatForm2::CPlatForm2() :
IBaseEntity(),
CPlatForm()
{
};

CPlatForm2::CPlatForm2(sint32 Index) :
IBaseEntity(Index),
CPlatForm(Index)
{
};

void CPlatForm2::Blocked (IBaseEntity *Other)
{
	IHurtableEntity *HurtableOther = (Other->EntityFlags & EF_HURTABLE) ? entity_cast<IHurtableEntity>(Other) : NULL;

	if (!(Other->GetSvFlags() & SVF_MONSTER) && !(Other->EntityFlags & EF_PLAYER) )
	{
		// give it a chance to go away on it's own terms (like gibs)
		if (HurtableOther && HurtableOther->CanTakeDamage)
			HurtableOther->TakeDamage (this, this, vec3fOrigin, Other->State.GetOrigin(), vec3fOrigin, 100000, 1, 0, MOD_CRUSH);

		// if it's still there, nuke it
		if (!Other->Freed)
			Other->BecomeExplosion(false);
		return;
	}

	if (HurtableOther && HurtableOther->CanTakeDamage)
	{
		if (HurtableOther->Health < 0)
			HurtableOther->TakeDamage (this, this, vec3fOrigin, Other->State.GetOrigin(), vec3fOrigin, 100, 1, 0, MOD_CRUSH);
		HurtableOther->TakeDamage (this, this, vec3fOrigin, Other->State.GetOrigin(), vec3fOrigin, Damage, 1, 0, MOD_CRUSH);
	}

	if (MoveState == STATE_UP)
		GoDown ();
	else if (MoveState == STATE_DOWN)
		GoUp ();
}

void CPlatForm2::Use (IBaseEntity *Other, IBaseEntity *Activator)
{
	if (RequiresActivation)
	{
		RequiresActivation = false;

		InsideTrigger = SpawnInsideTrigger();

		InsideTrigger->GetMaxs().X += 10;
		InsideTrigger->GetMaxs().Y += 10;
		InsideTrigger->GetMins().X -= 10;
		InsideTrigger->GetMins().Y -=10;

		InsideTrigger->Link ();
	
		GoDown ();
		return;
	}

	if (MoveState > STATE_BOTTOM)
		return;
	if ((LastMoveTime + 20) > Level.Frame)
		return;

	Operate (Activator);
};

void CPlatForm2::HitTop ()
{
	if (!IsSlave())
	{
		if (SoundEnd)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundEnd, 255, ATTN_STATIC);
		State.GetSound() = 0;
	}
	MoveState = STATE_TOP;

	if (PlatFlags & PLAT2_CALLED)
	{
		PlatFlags = PLAT2_WAITING;
		if (!(SpawnFlags & PLAT2_TOGGLE))
		{
			ThinkType = PLATTHINK_GO_DOWN;
			NextThink = Level.Frame + 50;
		}

		if (Game.GameMode & GAME_DEATHMATCH)
			LastMoveTime = Level.Frame - 10;
		else
			LastMoveTime = Level.Frame - 20;
	}
	else if ((SpawnFlags & PLAT2_TOP) && !(SpawnFlags & PLAT2_TOGGLE))
	{
		PlatFlags = 0;
		ThinkType = PLATTHINK_GO_DOWN;
		NextThink = Level.Frame + 20;
		LastMoveTime = Level.Frame;
	}
	else
	{
		PlatFlags = 0;
		LastMoveTime = Level.Frame;
	}

	UseTargets (this, Message);
}

void CPlatForm2::HitBottom ()
{
	if (!IsSlave())
	{
		if (SoundEnd)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundEnd, 255, ATTN_STATIC);
		State.GetSound() = 0;
	}
	MoveState = STATE_BOTTOM;
	
	if (PlatFlags & PLAT2_CALLED)
	{
		PlatFlags = PLAT2_WAITING;
		if (!(SpawnFlags & PLAT2_TOGGLE))
		{
			ThinkType = PLATTHINK_GO_UP;
			NextThink = Level.Frame + 50;
		}

		if (Game.GameMode & GAME_DEATHMATCH)
			LastMoveTime = Level.Frame - 10;
		else
			LastMoveTime = Level.Frame - 20;
	}
	else if ((SpawnFlags & PLAT2_TOP) && !(SpawnFlags & PLAT2_TOGGLE))
	{
		PlatFlags = 0;
		ThinkType = PLATTHINK_GO_UP;
		NextThink = Level.Frame + 20;
		LastMoveTime = Level.Frame;
	}
	else
	{
		PlatFlags = 0;
		LastMoveTime = Level.Frame;
	}

	KillDangerArea ();
	UseTargets (this, Message);
}

void CPlatForm2::SpawnDangerArea ()
{
	if (BadArea)
		return;

	vec3f mins = GetMins(), maxs = GetMaxs();
	maxs.Z = mins.Z + 64;

	BadArea = QNew (TAG_GAME) CBadArea (mins, maxs, 0, this);
}

void CPlatForm2::KillDangerArea ()
{
	if (BadArea)
	{
		BadArea->Remove = true;
		BadArea->Owner = NULL;
		BadArea = NULL;
	}
};

void CPlatForm2::DoEndFunc ()
{
	switch (EndFunc)
	{
	case PLATENDFUNC_HITBOTTOM:
		HitBottom ();
		break;
	case PLATENDFUNC_HITTOP:
		HitTop ();
		break;
	};
};

void CPlatForm2::GoDown ()
{
	if (!IsSlave())
	{
		if (SoundStart)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundStart, 255, ATTN_STATIC);
		State.GetSound() = SoundMiddle;
	}

	MoveState = STATE_DOWN;
	PlatFlags |= PLAT2_MOVING;

	MoveCalc (EndOrigin, PLATENDFUNC_HITBOTTOM);
}

void CPlatForm2::GoUp ()
{
	if (!IsSlave())
	{
		if (SoundStart)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundStart, 255, ATTN_STATIC);
		State.GetSound() = SoundMiddle;
	}

	MoveState = STATE_UP;
	PlatFlags |= PLAT2_MOVING;
	SpawnDangerArea ();

	MoveCalc (StartOrigin, PLATENDFUNC_HITTOP);
}

void CPlatForm2::Think ()
{
	switch (ThinkType)
	{
	case PLATTHINK_GO_DOWN:
		GoDown ();
		break;
	case PLATTHINK_GO_UP:
		GoUp ();
		break;
	default:
		IBrushModel::Think ();
	};
}

void CPlatForm2::Operate (IBaseEntity *Other)
{
	if (PlatFlags & PLAT2_MOVING)
		return;

	if ((LastMoveTime + 20) > Level.Frame)
		return;

	float platCenter = (InsideTrigger->GetAbsMin().Z + InsideTrigger->GetAbsMax().Z) / 2;

	sint32 otherState;

	if (MoveState == STATE_TOP)
	{
		otherState = STATE_TOP;
		if (SpawnFlags & PLAT2_BOX_LIFT)
		{
			if (platCenter > Other->State.GetOrigin().Z)
				otherState = STATE_BOTTOM;
		}
		else
		{
			if (InsideTrigger->GetAbsMax().Z > Other->State.GetOrigin().Z)
				otherState = STATE_BOTTOM;
		}
	}
	else
	{
		otherState = STATE_BOTTOM;
		if (Other->State.GetOrigin().Z > platCenter)
			otherState = STATE_TOP;
	}

	PlatFlags = PLAT2_MOVING;

	FrameNumber pauseTime = (Game.GameMode & GAME_DEATHMATCH) ? 3 : 5;

	if (MoveState != otherState)
	{
		PlatFlags |= PLAT2_CALLED;
		pauseTime = 1;
	}

	LastMoveTime = Level.Frame;
	
	if (MoveState == STATE_BOTTOM)
	{
		ThinkType = PLATTHINK_GO_UP;
		NextThink = Level.Frame + pauseTime;
	}
	else
	{
		ThinkType = PLATTHINK_GO_DOWN;
		NextThink = Level.Frame + pauseTime;
	}
}

CPlatForm2InsideTrigger::CPlatForm2InsideTrigger () :
CPlatFormInsideTrigger()
{
};

CPlatForm2InsideTrigger::CPlatForm2InsideTrigger (sint32 Index) :
IBaseEntity(Index),
CPlatFormInsideTrigger(Index)
{
};

IMPLEMENT_SAVE_SOURCE (CPlatForm2InsideTrigger)

void CPlatForm2InsideTrigger::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	if (!(Other->EntityFlags & EF_HURTABLE) || entity_cast<IHurtableEntity>(Other)->Health <= 0)
		return;
	if (!(Other->EntityFlags & EF_PLAYER) && !(Other->EntityFlags & EF_MONSTER))
		return;

	entity_cast<CPlatForm2>(Owner)->Operate (Other);
};

CPlatFormInsideTrigger *CPlatForm2::SpawnInsideTrigger ()
{
	CPlatForm2InsideTrigger	*trigger = QNewEntityOf CPlatForm2InsideTrigger;
	vec3f	tmin, tmax;

	//
	// middle trigger
	//	
	trigger->GetSolid() = SOLID_TRIGGER;
	trigger->Owner = this;

	tmin = GetMins();
	tmin.X += 25;
	tmin.Y += 25;

	tmax = GetMaxs();
	tmax.X -= 25;
	tmax.Y -= 25;
	tmax.Z += 8;

	tmin.Z = tmax.Z - (Positions[0].Z - Positions[1].Z + Lip);

	if (SpawnFlags & PLAT_LOW_TRIGGER)
		tmax.Z = tmin.Z + 8;

	if (tmax.X - tmin.X <= 0)
	{
		tmin.X = (GetMins().X + GetMaxs().X) *0.5;
		tmax.X = tmin.X + 1;
	}
	if (tmax.Y - tmin.Y <= 0)
	{
		tmin.Y = (GetMins().Y + GetMaxs().Y) *0.5;
		tmax.Y = tmin.Y + 1;
	}

	trigger->GetMins() = tmin;
	trigger->GetMaxs() = tmax;
	trigger->Touchable = true;

	trigger->Link ();

	return trigger;
};

void CPlatForm2::Spawn ()
{
	State.GetAngles().Clear ();
	GetSolid() = SOLID_BSP;
	PhysicsType = PHYSICS_PUSH;

	SetBrushModel ();

	if (!Speed)
		Speed = 20;
	else
		Speed *= 0.1f;

	if (!Accel)
		Accel = 5;
	else
		Accel *= 0.1f;

	if (!Decel)
		Decel = 5;
	else
		Decel *= 0.1f;

	if (!Damage)
		Damage = 2;

	if (!Lip)
		Lip = 8;

	if (Game.GameMode & GAME_DEATHMATCH)
	{
		Speed *= 2;
		Accel *= 2;
		Decel *= 2;
	}

	//PMM Added to kill things it's being blocked by 
	if (!Damage)
		Damage = 2;

	// pos1 is the top position, pos2 is the bottom
	Positions[0] = Positions[1] = State.GetOrigin ();
	Positions[1].Z -= (Height) ? Height : ((GetMaxs().Z - GetMins().Z) - Lip);

	MoveState = STATE_TOP;

	if (!TargetName.IsNullOrEmpty())
		RequiresActivation = true;
	else
	{
		RequiresActivation = false;

		InsideTrigger = SpawnInsideTrigger ();	// the "start moving" trigger	

		// PGM - debugging??
		InsideTrigger->GetMaxs().X += 10;
		InsideTrigger->GetMaxs().Y += 10;
		InsideTrigger->GetMins().X -= 10;
		InsideTrigger->GetMins().X -=10;

		InsideTrigger->Link ();

		if (!(SpawnFlags & PLAT2_TOP))
		{
			State.GetOrigin() = Positions[1];
			MoveState = STATE_BOTTOM;
		}	
	}

	Link ();

	MoveSpeed = Speed;
	MoveAccel = Accel;
	MoveDecel = Decel;
	StartOrigin = Positions[0];
	StartAngles = State.GetAngles();
	EndOrigin = Positions[1];
	EndAngles = State.GetAngles();

	SoundStart = SoundIndex ("plats/pt1_strt.wav");
	SoundMiddle = SoundIndex ("plats/pt1_mid.wav");
	SoundEnd = SoundIndex ("plats/pt1_end.wav");
};

void			CPlatForm2::SaveFields (CFile &File)
{
	File.Write<bool> (RequiresActivation);
	File.Write<FrameNumber> (LastMoveTime);
	File.Write<uint8> (PlatFlags);
	SaveBadArea (File, BadArea);
	CPlatForm::SaveFields (File);
};

void			CPlatForm2::LoadFields (CFile &File)
{
	RequiresActivation = File.Read<bool> ();
	LastMoveTime = File.Read<FrameNumber> ();
	PlatFlags = File.Read<uint8> ();
	BadArea = LoadBadArea (File);
	CPlatForm::LoadFields (File);
};

LINK_CLASSNAME_TO_CLASS ("func_plat2", CPlatForm2);

#include "Monsters/Infantry.h"
#include "Monsters/TurretEntities.h"

// invisible turret drivers so we can have unmanned turrets.
// originally designed to shoot at func_trains and such, so they
// fire at the center of the bounding box, rather than the entity's
// origin.

/*QUAKED turret_invisible_brain (1 .5 0) (-16 -16 -16) (16 16 16)
Invisible brain to drive the turret.

Does not search for targets. If targeted, can only be turned on once
and then off once. After that they are completely disabled.

"delay" the delay between firing (default ramps for skill level)
"Target" the turret breach
"Killtarget" the item you want it to attack.
Target the brain if you want it activated later, instead of immediately. It will wait 3 seconds
before firing to acquire the target.
*/
class CTurretBrain : public IMapEntity, public IUsableEntity, public IThinkableEntity
{
public:
	/**
	\typedef	uint8 ETurretBrainUseType
	
	\brief	Defines an alias representing use state of the turret brain.
	**/
	typedef uint8 ETurretBrainUseType;

	/**
	\typedef	uint8 ETurretBrainThinkType
	
	\brief	Defines an alias representing think state of the turret brain.
	**/
	typedef uint8 ETurretBrainThinkType;

	/**
	\enum	
	
	\brief	Values that represent the think/use states of the turret brain.
	**/
	enum
	{
		BRAINTHINK_NONE = 0,
		BRAINTHINK_LINK,
		BRAINTHINK_TURRET,

		BRAINUSE_NONE = 0,
		BRAINUSE_ACTIVATE,
		BRAINUSE_DEACTIVATE
	};

	ETurretBrainThinkType			ThinkType;
	ETurretBrainUseType				UseType;
	FrameNumber					Delay;
	CTurretBreach					*TargetedBreach;
	vec3f							MoveOrigin;
	FrameNumber					AttackFinished;
	FrameNumber					TrailTime;
	bool							LostSight;

	CTurretBrain () :
	  IMapEntity (),
	  IUsableEntity (),
	  IThinkableEntity ()
	  {
	  };

	CTurretBrain (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IUsableEntity (Index),
	  IThinkableEntity (Index)
	  {
	  };

	ENTITYFIELDS_SAVABLE(CTurretBrain)
	ENTITYFIELD_DEFS

	void Activate (IBaseEntity *Activator)
	{
		if (!Enemy)
			Enemy = Activator;

		// wait at least 3 seconds to fire.
		AttackFinished = Level.Frame + 30;
		UseType = BRAINUSE_DEACTIVATE;

		ThinkType = BRAINTHINK_LINK;
		NextThink = Level.Frame + ServerFramesPerSecond;
	};

	void Deactivate ()
	{
		ThinkType = BRAINTHINK_NONE;
		NextThink = 0;
	};

	void LinkTurret ()
	{
		if (!KillTarget.IsNullOrEmpty())
			Enemy = CC_PickTarget (KillTarget);

		ThinkType = BRAINTHINK_TURRET;
		NextThink = Level.Frame + ServerFramesPerSecond;

		TargetedBreach = entity_cast<CTurretBreach>(CC_PickTarget (Target));
		TargetedBreach->SetOwner(this);
		TargetedBreach->Team.Master->SetOwner(this);
		State.GetAngles() = TargetedBreach->State.GetAngles();

		vec3f vec = (TargetedBreach->State.GetOrigin() - State.GetOrigin());
		vec.Z = 0;
		MoveOrigin.X = vec.Length();

		vec = (State.GetOrigin() - TargetedBreach->State.GetOrigin());
		vec = vec.ToAngles();
		AnglesNormalize(vec);
		MoveOrigin.Y = vec.Y;

		MoveOrigin.Z = State.GetOrigin().Z - TargetedBreach->State.GetOrigin().Z;

		// add the driver to the end of them team chain
		IBaseEntity	*TeamEntity;
		for (TeamEntity = TargetedBreach->Team.Master; TeamEntity->Team.Chain; TeamEntity = TeamEntity->Team.Chain)
			;
		TeamEntity->Team.Chain = this;
		Team.Master = TargetedBreach->Team.Master;
		TargetedBreach->Enemy = Enemy;
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		switch (UseType)
		{
		case BRAINUSE_ACTIVATE:
			Activate (Activator);
			break;
		case BRAINUSE_DEACTIVATE:
			Deactivate ();
			break;
		};
	};

	void ThinkTurret ()
	{
		NextThink = Level.Frame + ServerFramesPerSecond;

		if (Enemy)
		{
			if (!Enemy->GetInUse())
				Enemy = nullentity;
			else if ((Enemy->EntityFlags & EF_HURTABLE) && entity_cast<IHurtableEntity>(*Enemy)->Health <= 0 && entity_cast<IHurtableEntity>(*Enemy)->CanTakeDamage)
				Enemy = nullentity;
		}

		if (!Enemy || !TargetedBreach->GetInUse() || TargetedBreach->Freed)
		{
			NextThink = 0;
			TargetedBreach = NULL;
			return;
		}

		vec3f endpos = (Enemy->GetAbsMax() + Enemy->GetAbsMin()) * 0.5f;
			
		CTrace trace (TargetedBreach->State.GetOrigin(), endpos, TargetedBreach, CONTENTS_MASK_SHOT);
		if (trace.Fraction == 1 || trace.Entity == Enemy)
		{
			if (LostSight)
			{
				TrailTime = Level.Frame;
				LostSight = !LostSight;
			}
		}
		else
		{
			LostSight = true;
			return;
		}

		// let the turret know where we want it to aim
		vec3f dir = (endpos - TargetedBreach->State.GetOrigin()).ToAngles();
		TargetedBreach->MoveAngles = dir;
		TargetedBreach->SetOwner(this);

		// decide if we should shoot
		if (Level.Frame < AttackFinished)
			return;

		FrameNumber reaction_time;
		if (Delay)
			reaction_time = Delay;
		else
			reaction_time = ((3 - CvarList[CV_SKILL].Integer()) * 10) * 10;
		if ((Level.Frame - TrailTime) < reaction_time)
			return;

		AttackFinished = Level.Frame + reaction_time + 10;
		TargetedBreach->ShouldFire = true;
	};

	void Think ()
	{
		switch (ThinkType)
		{
		case BRAINTHINK_LINK:
			LinkTurret ();
			break;
		case BRAINTHINK_TURRET:
			ThinkTurret ();
			break;
		};
	};

	void Spawn ()
	{
		if (KillTarget.IsNullOrEmpty())
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No killtarget\n");
			Free ();
			return;
		}
		if (Target.IsNullOrEmpty())
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No target\n");
			Free ();
			return;
		}

		if (!TargetName.IsNullOrEmpty())
			UseType = BRAINUSE_ACTIVATE;
		else
		{
			ThinkType = BRAINTHINK_LINK;
			NextThink = Level.Frame + ServerFramesPerSecond;
		}

		Link ();
	};
};

ENTITYFIELDS_BEGIN(CTurretBrain)
{
	CEntityField ("delay",		EntityMemberOffset(CTurretBrain,Delay),		FT_FRAMENUMBER | FT_SAVABLE),

	CEntityField ("ThinkType",	EntityMemberOffset(CTurretBrain,ThinkType),	FT_BYTE | FT_SAVABLE | FT_NOSPAWN),
	CEntityField ("UseType",	EntityMemberOffset(CTurretBrain,UseType),	FT_BYTE | FT_SAVABLE | FT_NOSPAWN),
	CEntityField ("TargetedBreach",	EntityMemberOffset(CTurretBrain,TargetedBreach),	FT_ENTITY | FT_SAVABLE | FT_NOSPAWN),
};
ENTITYFIELDS_END(CTurretBrain)

bool			CTurretBrain::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTurretBrain> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IMapEntity::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value));
};

void CTurretBrain::SaveFields (CFile &File)
{
	SaveEntityFields <CTurretBrain> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
	IThinkableEntity::SaveFields (File);
}

void CTurretBrain::LoadFields (CFile &File)
{
	LoadEntityFields <CTurretBrain> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("turret_invisible_brain", CTurretBrain);

// ***************************
// TRIGGER_DISGUISE
// ***************************

/*QUAKED trigger_disguise (.5 .5 .5) ? TOGGLE START_ON REMOVE
Anything passing through this trigger when it is active will
be marked as disguised.

TOGGLE - field is turned off and on when used.
START_ON - field is active when spawned.
REMOVE - field removes the disguise
*/

/**
\enum	

\brief	Values that represent spawnflags pertaining to CTriggerDisguise. 
**/
enum
{
	DISGUISE_TOGGLE,
	DISGUISE_START_ON,
	DISGUISE_REMOVE
};

#include "Entities/TriggerEntities.h"

class CTriggerDisguise : public CTriggerBase
{
public:
	CTriggerDisguise () :
	  CTriggerBase ()
	  {
	  };

	CTriggerDisguise (sint32 Index) :
	  IBaseEntity (Index),
	  CTriggerBase (Index)
	  {
	  };

	void SaveFields (CFile &File)
	{
		CTriggerBase::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		CTriggerBase::LoadFields (File);
	}

	IMPLEMENT_SAVE_HEADER(CTriggerDisguise);

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		GetSolid() = (GetSolid() == SOLID_NOT) ? SOLID_TRIGGER : SOLID_NOT;
		Link ();
	}

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
		if (Other->EntityFlags & EF_PLAYER)
		{
			if (SpawnFlags & DISGUISE_REMOVE)
				Other->Flags &= ~FL_DISGUISED;
			else
				Other->Flags |= FL_DISGUISED;
		}
	}

	void Spawn ()
	{
		GetSolid() = (SpawnFlags & DISGUISE_START_ON) ? SOLID_TRIGGER : SOLID_NOT;

		Touchable = Usable = true;
		PhysicsType = PHYSICS_NONE;
		GetSvFlags() = SVF_NOCLIENT;

		SetBrushModel ();
		Link ();
	}
};

LINK_CLASSNAME_TO_CLASS ("trigger_disguise", CTriggerDisguise);

#endif
