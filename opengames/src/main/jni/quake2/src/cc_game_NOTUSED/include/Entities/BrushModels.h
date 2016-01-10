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
// cc_brushmodels.h
// 
//

#if !defined(CC_GUARD_BRUSHMODELS_H) || !INCLUDE_GUARDS
#define CC_GUARD_BRUSHMODELS_H

/**
\typedef	uint8 EBrushThinkType

\brief	Defines an alias representing an index of the current think state of a brush entity.
**/
typedef uint8 EBrushThinkType;

/**
\enum	

\brief	Values that represent the base think states for brushes. 
**/
enum
{
	BRUSHTHINK_NONE,

	BRUSHTHINK_MOVEBEGIN,
	BRUSHTHINK_MOVEFINAL,
	BRUSHTHINK_MOVEDONE,

	BRUSHTHINK_MOVEACCEL,

	BRUSHTHINK_AMOVEBEGIN,
	BRUSHTHINK_AMOVEFINAL,
	BRUSHTHINK_AMOVEDONE,

	BRUSHTHINK_CUSTOM_START,
};

/**
\typedef	uint32 EBrushType

\brief	Defines an alias representing what types of brush model an entity can be converted to.
**/
typedef uint32 EBrushType;

/**
\enum	

\brief	Values that represent brush model types. 
**/
enum
{
	BRUSH_BASE			= BIT(0),

	BRUSH_DOOR			= BIT(1),
	BRUSH_PLATFORM		= BIT(2),
	BRUSH_TRAIN			= BIT(3),
	BRUSH_ROTATINGDOOR	= BIT(4),
	BRUSH_MOVABLEWATER	= BIT(5),
	BRUSH_SECRETDOOR	= BIT(6),
	BRUSH_BUTTON		= BIT(7),
	BRUSH_ROTATING		= BIT(8),
	BRUSH_CONVEYOR		= BIT(9),
	BRUSH_WALL			= BIT(10),
	BRUSH_OBJECT		= BIT(11),
	BRUSH_EXPLOSIVE		= BIT(12),
};

/**
\typedef	uint8 EMoveState

\brief	Defines an alias representing current movement state.
**/
typedef uint8 EMoveState;

/**
\enum	

\brief	Values that represent current movement state. 
**/
enum
{
	STATE_TOP,
	STATE_BOTTOM,
	STATE_UP,
	STATE_DOWN
};

/**
\class	IBrushModel

\brief	Brush model entity interface.
		See "Entity Types - Brush Model"

\author	Paril
\date	30/05/2010
**/
class IBrushModel : public virtual IBaseEntity, public IThinkableEntity, public IStopPhysics
{
public:
	EBrushType			BrushType;			// Type of brush model

	char				*Model;				// The inline brush model
	float				Accel;				// Given acceleration
	float				Speed;				// Given speed
	float				Decel;				// Given deceleration
	sint32				Distance;			// Distance to target
	sint32				Damage;				// Squish damage
	uint8				Sounds;				// Sounds variable.

	FrameNumber			Wait;				// Wait

	FrameNumber			TouchDebounce;		// Touch debounce time

	// fixed data
	vec3f				StartOrigin;		// The move start origin
	vec3f				StartAngles;		// The move start angles
	vec3f				EndOrigin;			// The move end origin
	vec3f				EndAngles;			// The move end angles

	MediaIndex			SoundStart;			// The start sound
	MediaIndex			SoundMiddle;		// The middle sound
	MediaIndex			SoundEnd;			// The end sound
	vec3f				MoveDir;			// The move dir
	vec3f				Positions[2];		// Positions
	vec3f				MoveOrigin,			// Move origin
						MoveAngles;			// Move angles

	sint32				Lip;				// Movement lip in units

	// state data
	EMoveState			MoveState;			// Move state
	vec3f				Dir;				// The direction
	float				CurrentSpeed;		// The current speed
	float				AccelMoveSpeed,		// Acceleration move speed
						MoveSpeed,			// Current move speed
						MoveAccel,			// Current move acceleration
						MoveDecel;			// Current move deceleration
	float				NextSpeed;			// Next move speed
	float				RemainingDistance;	// The remaining distance
	float				DecelDistance;		// The decel distance
	uint8				EndFunc;			// The endfunc

	EBrushThinkType		ThinkType;			// Thinktype

	ENTITYFIELD_VIRTUAL_DEFS
	ENTITYFIELDS_SAVABLE_VIRTUAL(IBrushModel)

	IBrushModel ();
	IBrushModel (sint32 Index);

	/**
	\fn	void SetBrushModel ()
	
	\brief	Sets the brush model.
	**/
	void SetBrushModel ();

	/**
	\fn	inline void SetMoveDir ()
	
	\brief	Sets the move dir from angles, then clears angles.
	**/
	inline void SetMoveDir ()
	{
		IBaseEntity::SetMoveDir(MoveDir);
	}

	/**
	\fn	virtual void DoEndFunc ()
	
	\brief	The endfunc operation. This is called when a move is finished. EndFunc is set to the
			current EndFunc index; in this function you execute functions based on the EndFunc. 
	**/
	virtual void	DoEndFunc ()
	{
	};

	/**
	\fn	void MoveDone ()
	
	\brief	Called when a move is done.
	**/
	void MoveDone ();

	/**
	\fn	void MoveFinal ()
	
	\brief	Called when a move reaches it's final destination. 
	**/
	void MoveFinal ();

	/**
	\fn	void MoveBegin ()
	
	\brief	Called to begin a move. 
	**/
	void MoveBegin ();

	/**
	\fn	void MoveCalc (vec3f &Dest, uint32 EndFunc)
	
	\brief	Call to calculate a move to 'dest'. DoEndFunc will be executed at the end, with the
			EndFunc set to 'EndFunc'. 
	
	\author	Paril
	\date	30/05/2010
	
	\param [in,out]	Dest	Destination for the. 
	\param	EndFunc			The end func. 
	**/
	void MoveCalc (vec3f &Dest, uint32 EndFunc);

	/**
	\fn	void AngleMoveDone ()
	
	\brief	Called when an angular move is done. 
	**/
	void AngleMoveDone ();

	/**
	\fn	void AngleMoveFinal ()
	
	\brief	Called when an angular move has reached it's final destination. 
	**/
	void AngleMoveFinal ();

	/**
	\fn	void AngleMoveBegin ()
	
	\brief	Called to start an angular move. 
	**/
	void AngleMoveBegin ();

	/**
	\fn	void AngleMoveCalc (uint32 EndFunc)
	
	\brief	Called to calculate an angle movement. EndFunc is set to 'EndFunc'. 
	
	\author	Paril
	\date	30/05/2010
	
	\param	EndFunc	The end func. 
	**/
	void AngleMoveCalc (uint32 EndFunc);

	/**
	\fn	inline float AccelerationDistance (const float target, const float rate)
	
	\brief	Calculates acceleration distance based on target and rate. 
	
	\author	Paril
	\date	30/05/2010
	
	\param	target	Target accel/decel. 
	\param	rate	The rate. 
	
	\return	Acceleration distance in units. 
	**/
	inline float AccelerationDistance (const float target, const float rate)
	{
		return (target * ((target / rate) + 1) / 2);
	}

	/**
	\fn	void CalcAcceleratedMove()
	
	\brief	Calculates a move that has acceleration.
	**/
	void CalcAcceleratedMove();

	/**
	\fn	void Accelerate ()
	
	\brief	Accelerates. 
	**/
	void Accelerate ();

	/**
	\fn	void ThinkAccelMove ()
	
	\brief	Acceleration move thinking routine.The team has completed a frame of movement, so change
			the speed for the next frame. 
	**/
	void ThinkAccelMove ();

	virtual void Think ();
	bool Run ();
};

/**
\class	CPlatFormInsideTrigger

\brief	A PlatForm's inside trigger. See "Quake II Entities - Platforms" for more information.

\author	Paril
\date	30/05/2010
**/
class CPlatFormInsideTrigger : public ITouchableEntity
{
public:
	class CPlatForm	*Owner;

	CPlatFormInsideTrigger ();
	CPlatFormInsideTrigger (sint32 Index);

	ENTITYFIELDS_SAVABLE(CPlatFormInsideTrigger)

	virtual void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);
};

/**
\class	CPlatForm2InsideTrigger

\brief	A PlatForm2's inside trigger. Specific to Rogue. See "Quake II Entities - Platforms" for more information.

\author	Paril
\date	30/05/2010
**/
class CPlatForm2InsideTrigger : public CPlatFormInsideTrigger
{
public:
	CPlatForm2InsideTrigger ();
	CPlatForm2InsideTrigger (sint32 Index);

	ENTITYFIELDS_INHERIT(CPlatForm2InsideTrigger)

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);
};

/**
\enum	

\brief	Values that represent spawnflags pertaining to CPlatForm. 
**/
enum
{
	PLAT_LOW_TRIGGER	= BIT(0),
};

/**
\class	CPlatForm

\brief	A platform. This entity is mapped to the classname func_plat.
		See "Quake II Entities - Platforms" for more information.

\author	Paril
\date	30/05/2010
**/
class CPlatForm : public IMapEntity, public IBrushModel, public IBlockableEntity, public IUsableEntity
{
public:
	sint32					Height;
	CPlatFormInsideTrigger	*InsideTrigger;

	enum
	{
		PLATTHINK_GO_DOWN = BRUSHTHINK_CUSTOM_START,
		PLATTHINK_GO_UP
	};

	CPlatForm();
	CPlatForm(sint32 Index);

	bool Run ();
	virtual void Blocked (IBaseEntity *Other);
	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator);

	/**
	\fn	virtual void HitTop ()
	
	\brief	Called when the platform hits the top of it's move. 
	**/
	virtual void HitTop ();

	/**
	\fn	virtual void HitBottom ()
	
	\brief	Called when the platform hits the bottom of it's move. 
	**/
	virtual void HitBottom ();

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CPlatForm)

	enum
	{
		PLATENDFUNC_HITBOTTOM,
		PLATENDFUNC_HITTOP
	};

	virtual void DoEndFunc ();

	/**
	\fn	virtual void GoDown ()
	
	\brief	Causes the platform to go down. 
	**/
	virtual void GoDown ();

	/**
	\fn	virtual void GoUp ()
	
	\brief	Causes the platform to go up. 
	**/
	virtual void GoUp ();

	virtual void Think ();

	/**
	\fn	void SpawnInsideTrigger ()
	
	\brief	Spawns the inside trigger.
	**/
	void SpawnInsideTrigger ();

	virtual void Spawn ();
};

/**
\class	CDoorTrigger

\brief	Door trigger. See "Quake II Entities - Doors" for more information.

\author	Paril
\date	30/05/2010
**/
class CDoorTrigger : public ITouchableEntity
{
public:
	FrameNumber		TouchDebounce;

	CDoorTrigger ();
	CDoorTrigger (sint32 Index);

	ENTITYFIELDS_SAVABLE(CDoorTrigger)

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);
};

/**
\class	CDoor

\brief	Base class for doors. See "Quake II Entities - Doors" for more information.

\author	Paril
\date	30/05/2010
**/
class CDoor : public IMapEntity, public IBrushModel, public IHurtableEntity, public IBlockableEntity, public ITouchableEntity, public IUsableEntity
{
public:
#if ROGUE_FEATURES
	/**
	\typedef	uint8 EDoorUse
	
	\brief	Defines an alias representing the door use state.
	**/
	typedef uint8 EDoorUse;
	
	/**
	\enum	
	
	\brief	Values that represent a door's use state. 
	**/
	enum
	{
		DOORUSE_NONE,
		DOORUSE_NORMAL,
		DOORUSE_ACTIVATE
	};

	EDoorUse	UseType;
#endif

	enum
	{
		DOORTHINK_SPAWNDOORTRIGGER = BRUSHTHINK_CUSTOM_START,
		DOORTHINK_CALCMOVESPEED,

		DOORTHINK_GODOWN,

#if ROGUE_FEATURES
		DOORTHINK_SMARTWATER_GOUP,
#endif
	};

	CDoor();
	CDoor(sint32 Index);

	IMPLEMENT_SAVE_HEADER(CDoor)

	/**
	\fn	void UseAreaPortals (bool isOpen);
	
	\brief	Opens or closes area portals.
	
	\author	Paril
	\date	30/05/2010
	
	\param	isOpen		True if area portals are to be opened. 
	**/
	void UseAreaPortals (bool isOpen);

	bool Run ();

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (IBrushModel::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value) || IHurtableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	};

	virtual void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IBrushModel::SaveFields (File);
		IUsableEntity::SaveFields (File);
		IHurtableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IBrushModel::LoadFields (File);
		IUsableEntity::LoadFields (File);
		IHurtableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
	}

	enum
	{
		DOORENDFUNC_HITBOTTOM,
		DOORENDFUNC_HITTOP
	};

	/**
	\fn	virtual void HitTop ()
	
	\brief	Called when a door hits the top of it's movement.
	**/
	virtual void HitTop ();

	/**
	\fn	virtual void HitBottom ()
	
	\brief	Called when a door hits the bottom of it's movement. 
	**/
	virtual void HitBottom ();

	/**
	\fn	virtual void GoDown ()
	
	\brief	Called to make the door go down. 
	**/
	virtual void GoDown ();

	/**
	\fn	virtual void GoUp (IBaseEntity *Activator)
	
	\brief	Called to make the door go up. 
	
	\author	Paril
	\date	30/05/2010
	
	\param [in,out]	Activator	If non-null, the activator. 
	**/
	virtual void GoUp (IBaseEntity *Activator);

	virtual void DoEndFunc ();
	virtual void Think ();

#if ROGUE_FEATURES
	void SmartWaterGoUp ();
	void Activate ();
#endif

	virtual void Blocked (IBaseEntity *Other);
	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator);
	virtual void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	virtual void Pain (IBaseEntity *Other, sint32 Damage);
	virtual void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	/**
	\fn	void SpawnDoorTrigger ()
	
	\brief	Spawns the door trigger. 
	**/
	void SpawnDoorTrigger ();

	/**
	\fn	void CalcMoveSpeed ()
	
	\brief	Calculates movement speed. 
	**/
	void CalcMoveSpeed ();

	virtual void Spawn ();
};

/**
\class	CRotatingDoor

\brief	Rotating door. See "Quake II Entities - Doors" for more information.

\author	Paril
\date	30/05/2010
**/
class CRotatingDoor : public CDoor
{
public:
	CRotatingDoor ();
	CRotatingDoor (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CRotatingDoor)

	void GoDown();
	void GoUp (IBaseEntity *Activator);

	void Spawn ();
};

/**
\class	CMovableWater

\brief	Movable water. See "Quake II Entities - Doors" for more information.

\author	Paril
\date	30/05/2010
**/
class CMovableWater : public CDoor
{
public:
	CMovableWater ();
	CMovableWater (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CMovableWater)

	void Spawn ();
};

/**
\class	CDoorSecret

\brief	Door secret. See "Quake II Entities - Doors" for more information.

\author	Paril
\date	30/05/2010
**/
class CDoorSecret : public CDoor
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

	CDoorSecret ();
	CDoorSecret (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CDoorSecret)

	void Blocked (IBaseEntity *Other);
	void Use (IBaseEntity *Other, IBaseEntity *Activator);
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);

	void DoEndFunc ();
	void Think ();

	void Spawn ();
};

/**
\class	CButton

\brief	Button. See "Quake II Entities - Buttons" for more information.

\author	Paril
\date	30/05/2010
**/
class CButton : public IMapEntity, public IBrushModel, public IHurtableEntity, public ITouchableEntity, public IUsableEntity
{
public:
	enum
	{
		BUTTONTHINK_RETURN = BRUSHTHINK_CUSTOM_START,
	};

	CButton();
	CButton(sint32 Index);

	IMPLEMENT_SAVE_HEADER(CButton)

	bool Run ();

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (IBrushModel::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value) || IHurtableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	};

	virtual void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IBrushModel::SaveFields (File);
		IUsableEntity::SaveFields (File);
		IHurtableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IBrushModel::LoadFields (File);
		IUsableEntity::LoadFields (File);
		IHurtableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
	}

	enum
	{
		BUTTONENDFUNC_WAIT,
		BUTTONENDFUNC_DONE
	};

	virtual void DoEndFunc ();
	virtual void Think ();

	/**
	\fn	virtual void Fire ()
	
	\brief	Fires this button's targets. 
	**/
	virtual void Fire ();

	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator);
	virtual void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	virtual void Pain (IBaseEntity *Other, sint32 Damage);
	virtual void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	virtual void Spawn ();
};

/**
\class	CTrainBase

\brief	Train base. See "Quake II Entities - Trains" for more information.

\author	Paril
\date	30/05/2010
**/
class CTrainBase : public IMapEntity, public IBrushModel, public IBlockableEntity, public IUsableEntity
{
public:
	class CPathCorner	*TargetEntity;

	enum
	{
		TRAINTHINK_FIND = BRUSHTHINK_CUSTOM_START,
		TRAINTHINK_NEXT,
	};
	CTrainBase();
	CTrainBase(sint32 Index);

	IMPLEMENT_SAVE_HEADER(CTrainBase)

	virtual bool Run ();
	virtual bool ParseField (const char *Key, const char *Value);

	virtual void SaveFields (CFile &File);
	virtual void LoadFields (CFile &File);

	enum
	{
		TRAINENDFUNC_NONE,
		TRAINENDFUNC_WAIT,
	};

	/**
	\fn	void TrainWait ()
	
	\brief	Called when the train has done it's move. Waits if the path requires so, and selects the
			next target. 
	**/
	void TrainWait ();

	/**
	\fn	void Next ()
	
	\brief	Pick next target and resumes course. 
	**/
	void Next ();

	/**
	\fn	void Resume ()
	
	\brief	Resumes this train's course. 
	**/
	void Resume ();

	/**
	\fn	void Find ()
	
	\brief	Finds targets and starts it's journey. 
	**/
	void Find ();

	virtual void DoEndFunc ();
	virtual void Think ();

	virtual void Blocked (IBaseEntity *Other);
	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator);

	virtual void Spawn ();
};

/**
\class	CTrain

\brief	Train, mapped to func_train. See "Quake II Entities - Trains" for more information.

\author	Paril
\date	30/05/2010
**/
class CTrain : public CTrainBase
{
public:
	CTrain ();
	CTrain (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CTrain)

	void Spawn ();
};

/**
\class	CTriggerElevator

\brief	Elevator trigger. See "Quake II Entities - Trains" for more information.

\author	Paril
\date	30/05/2010
**/
class CTriggerElevator : public IMapEntity, public IThinkableEntity, public IUsableEntity
{
public:
	CTrain			*MoveTarget;	// The train to be moved

	CTriggerElevator ();
	CTriggerElevator (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CTriggerElevator)

	bool ParseField (const char *Key, const char *Value)
	{
		return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		File.Write<sint32> ((MoveTarget) ? MoveTarget->State.GetNumber() : -1);
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		sint32 Index = File.Read<sint32> ();
		if (Index != -1)
			MoveTarget = entity_cast<CTrain>(Game.Entities[Index].Entity);

		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);
	}

	void Think ();
	void Use (IBaseEntity *Other, IBaseEntity *Activator);

	void Spawn ();
};

/**
\class	CWorldEntity

\brief	World entity.

\author	Paril
\date	30/05/2010
**/
class CWorldEntity : public IMapEntity, public IBrushModel
{
public:
	String			Message, Gravity, Sky, NextMap;
	vec3f			SkyAxis;
	float			SkyRotate;

	CWorldEntity ();
	CWorldEntity (sint32 Index);

	ENTITYFIELD_DEFS
	IMPLEMENT_SAVE_HEADER(CWorldEntity)

	void SaveFields (CFile &File);
	void LoadFields (CFile &File);

	bool Run ();
	void Spawn ();
};

/**
\class	CRotatingBrush

\brief	Rotating brush. See "Quake II Entities - Others" for more information.

\author	Paril
\date	30/05/2010
**/
class CRotatingBrush : public IMapEntity, public IBrushModel, public IUsableEntity, public IBlockableEntity, public ITouchableEntity
{
public:
	bool Blockable;

	CRotatingBrush ();
	CRotatingBrush (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CRotatingBrush)

	void Use (IBaseEntity *Other, IBaseEntity *Activator);
	void Blocked (IBaseEntity *Other);
	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (IBrushModel::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IBrushModel::SaveFields (File);
		IUsableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IBrushModel::LoadFields (File);
		IUsableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
	}

#if ROGUE_FEATURES
	/**
	\enum	
	
	\brief	Values that represent a CRotatingBrush' think state. 
	**/
	enum
	{
		ROTATINGTHINK_ACCEL,
		ROTATINGTHINK_DECEL
	};

	/**
	\fn	void Accelerate ()
	
	\brief	Accelerates this brushes rotating velocity. 
	**/
	void Accelerate ();

	/**
	\fn	void Decelerate ()
	
	\brief	Decelerates this brushes rotating velocity. 
	**/
	void Decelerate ();

	void Think ();
#endif

	bool Run ();
	void Spawn ();
};

/**
\class	CConveyor

\brief	Conveyor belt. See "Quake II Entities - Others" for more information.

\author	Paril
\date	30/05/2010
**/
class CConveyor : public IMapEntity, public IBrushModel, public IUsableEntity
{
public:
	sint32			SavedSpeed;

	CConveyor ();
	CConveyor (sint32 Index);

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CConveyor)

	void Use (IBaseEntity *Other, IBaseEntity *Activator);

	bool Run ();
	void Spawn ();
};

/**
\class	CAreaPortal

\brief	An area portal. See "Quake II Entities - Area Portal" for more information.

\author	Paril
\date	30/05/2010
**/
class CAreaPortal : public IMapEntity, public IUsableEntity
{
public:
	bool		PortalState;
	uint8		Style;

	CAreaPortal ();
	CAreaPortal (sint32 Index);

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CAreaPortal)

	void Use (IBaseEntity *Other, IBaseEntity *Activator);

	bool Run ();
	void Spawn ();
};

/**
\class	CFuncWall

\brief	Func wall. See "Quake II Entities - Others" for more information.

\author	Paril
\date	30/05/2010
**/
class CFuncWall : public IMapEntity, public IBrushModel, public IUsableEntity
{
public:
	CFuncWall ();
	CFuncWall (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CFuncWall)

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (IBrushModel::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	virtual void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IBrushModel::SaveFields (File);
		IUsableEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IBrushModel::LoadFields (File);
		IUsableEntity::LoadFields (File);
	}

	void Use (IBaseEntity *Other, IBaseEntity *Activator);

	bool Run ();
	void Spawn ();
};

/**
\class	CFuncObject

\brief	A func_object. See "Quake II Entities - Others" for more information.

\author	Paril
\date	30/05/2010
**/
class CFuncObject : public IMapEntity, public IBrushModel, public ITossProjectile, public ITouchableEntity, public IUsableEntity
{
public:
	CFuncObject ();
	CFuncObject (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CFuncObject)

	void Think ();
	void Use (IBaseEntity *Other, IBaseEntity *Activator);
	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (IBrushModel::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	virtual void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IBrushModel::SaveFields (File);
		IUsableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
		ITossProjectile::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IBrushModel::LoadFields (File);
		IUsableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
		ITossProjectile::LoadFields (File);
	}

	bool Run ();
	void Spawn ();
};

/**
\class	CFuncExplosive

\brief	Func explosive. Solid brush that can be shot at. See "Quake II Entities - Explosive" for
		more information. 

\author	Paril
\date	30/05/2010
**/
class CFuncExplosive : public IMapEntity, public IBrushModel, public IUsableEntity, public IHurtableEntity
{
public:
	/**
	\typedef	uint8 EFuncExplosiveUseType
	
	\brief	Defines an alias representing a CFuncExplosive's use state.
	**/
	typedef uint8 EFuncExplosiveUseType;

	/**
	\enum	
	
	\brief	Values that represent a CFuncExplosive's use state. 
	**/
	enum
	{
		FUNCEXPLOSIVE_USE_NONE,
		FUNCEXPLOSIVE_USE_SPAWN,
		FUNCEXPLOSIVE_USE_EXPLODE,
#if ROGUE_FEATURES
		FUNCEXPLOSIVE_USE_ACTIVATE,
#endif
	};

	EFuncExplosiveUseType	UseType;		// Use state
	sint32					Explosivity;	// The explosivity of the brush

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CFuncExplosive)

	CFuncExplosive ();
	CFuncExplosive (sint32 Index);

	/**
	\fn	void DoSpawn ()
	
	\brief	Called to make a func_explosive solid. 
	**/
	void DoSpawn ();

	void Use (IBaseEntity *Other, IBaseEntity *Activator);
#if ROGUE_FEATURES
	/**
	\fn	void Activate (IBaseEntity *Other, IBaseEntity *Activator)
	
	\brief	Activates this func_explosive. 
	
	\author	Paril
	\date	30/05/2010
	
	\param [in,out]	Other		If non-null, the other. 
	\param [in,out]	Activator	If non-null, the activator. 
	**/
	void Activate (IBaseEntity *Other, IBaseEntity *Activator);
#endif
	void Pain (IBaseEntity *Other, sint32 Damage);
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);

	bool Run ();
	void Spawn ();
};

/**
\class	CKillbox

\brief	Killbox. See "Quake II Entities - Others" for more information.

\author	Paril
\date	30/05/2010
**/
class CKillbox : public IMapEntity, public IUsableEntity, public IBrushModel
{
public:
	CKillbox ();
	CKillbox (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CKillbox)

	bool ParseField (const char *Key, const char *Value)
	{
		return (IBrushModel::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
		IBrushModel::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
		IBrushModel::LoadFields (File);
	}

	void Use (IBaseEntity *Other, IBaseEntity *Activator);

	bool Run ()
	{
		return IBaseEntity::Run ();
	};

	void Spawn ();
};

#else
FILE_WARNING
#endif
