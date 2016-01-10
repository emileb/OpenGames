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
// cc_trigger_entities.h
// 
//

class CTriggerBase : public IMapEntity, public IBrushModel, public ITouchableEntity, public IUsableEntity
{
public:
	enum
	{
		TRIGGER_THINK_NONE,
		TRIGGER_THINK_FREE,
		TRIGGER_THINK_WAIT,

		TRIGGER_THINK_CUSTOM
	};
	uint32			ThinkType;
	vec3f			MoveDir;
	FrameNumber		Wait;
	uint8			Sounds;

	CTriggerBase ();
	CTriggerBase (sint32 Index);

	ENTITYFIELD_VIRTUAL_DEFS
	ENTITYFIELDS_SAVABLE_VIRTUAL(CTriggerBase)

	bool Run ();

	virtual void Think ();
	virtual void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator);

	void		Init ();

	virtual bool CheckValidity ()
	{
		return IMapEntity::CheckValidity();
	};

	// the trigger was just activated
	// ent->Activator should be set to the Activator so it can be held through a delay
	// so wait for the delay time before firing
	void		Trigger ();

	virtual void Spawn () = 0;
};

class CTriggerMultiple : public CTriggerBase
{
public:
	bool		ActivateUse;

	CTriggerMultiple ();
	CTriggerMultiple (sint32 Index);

	ENTITYFIELDS_SAVABLE(CTriggerMultiple)

	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator);
	virtual void Spawn ();

	virtual bool CheckValidity ()
	{
		return IMapEntity::CheckValidity();
	};
};

/**
\enum	

\brief	Values that represent spawnflags pertaining to CTriggerPush. 
**/
enum
{
	PUSH_ONCE		= BIT(0),
#if ROGUE_FEATURES
	PUSH_START_OFF	= BIT(1),
	PUSH_SILENT		= BIT(2)
#endif
};
