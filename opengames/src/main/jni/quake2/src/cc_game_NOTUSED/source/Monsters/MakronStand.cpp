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
// cc_makron_stand.cpp
// 
//

#include "Local.h"
#include "Utility/TemporaryEntities.h"
#include "Monsters/Makron.h"

/*QUAKED monster_boss3_stand (1 .5 0) (-32 -32 0) (32 32 90)

Just stands and cycles in one place until targeted, then teleports away.
*/
class CMonsterBoss3Stand : public IMapEntity, public IThinkableEntity, public IUsableEntity, public IStepPhysics
{
public:
	CMonsterBoss3Stand () :
	  IBaseEntity (),
	  IMapEntity (),
	  IThinkableEntity (),
	  IUsableEntity (),
	  IStepPhysics ()
	{
	};

	CMonsterBoss3Stand (sint32 Index) :
	  IBaseEntity (Index),
	  IMapEntity (Index),
	  IThinkableEntity (Index),
	  IUsableEntity (Index),
	  IStepPhysics (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMonsterBoss3Stand)

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
		IStepPhysics::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);
		IStepPhysics::LoadFields (File);
	}

	bool Run ()
	{
		return IStepPhysics::Run();
	};

	void Think ()
	{
		if (State.GetFrame() == CMakron::FRAME_stand260)
			State.GetFrame() = (CMakron::FRAME_stand201);
		else
			State.GetFrame()++;

		NextThink = Level.Frame + ServerFramesPerSecond;
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		CBossTeleport(State.GetOrigin()).Send();
		Free ();
	};

	void Spawn ()
	{
		if (Game.GameMode & GAME_DEATHMATCH)
		{
			Free ();
			return;
		}

		PhysicsType = PHYSICS_STEP;
		GetSolid() = SOLID_BBOX;
		State.GetModelIndex() = ModelIndex ("models/monsters/boss3/rider/tris.md2");
		State.GetFrame() = CMakron::FRAME_stand201;

		SoundIndex ("misc/bigtele.wav");

		GetMins().Set (32, -32, 0);
		GetMaxs().Set (32, 32, 90);

		NextThink = Level.Frame + ServerFramesPerSecond;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("monster_boss3_stand", CMonsterBoss3Stand);

