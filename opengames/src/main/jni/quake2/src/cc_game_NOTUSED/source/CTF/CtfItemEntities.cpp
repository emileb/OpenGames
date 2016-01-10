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
// cc_ctfitementities.cpp
// CTF Item Entities
//

#include "Local.h"
#include "Items/Items.h"

#if CLEANCTF_ENABLED
typedef std::map <ETeamIndex, CFlagTransponder*> TTransponderListType;
TTransponderListType Transponders;

void ClearTransponders ()
{
	for (TTransponderListType::iterator it = Transponders.begin(); it != Transponders.end(); ++it)
		QDelete (*it).second;
	Transponders.clear ();
}

CFlagTransponder *FindTransponder (ETeamIndex Team)
{
	if (Transponders.find(Team) == Transponders.end())
		return NULL;

	return (*Transponders.find(Team)).second;
}

CFlagTransponder::CFlagTransponder (const ETeamIndex Team, class CFlagEntity *Flag) :
  Team(Team),
  Base(Flag),
  Flag(Flag),
  Holder(NULL),
  Location(FLAG_AT_BASE)
{
	Transponders[Team] = this;
};

void PrintTransponders ()
{
	for (TTransponderListType::iterator it = Transponders.begin(); it != Transponders.end(); ++it)
	{
		CFlagTransponder *Transponder = (*it).second;
		DebugPrintf ("Transponder for team %i:\n  Location: %i\n  Base: %i\n  Flag: %i\n  Holder: %i\n",
			Transponder->Team, Transponder->Location, Transponder->Base->State.GetNumber(),
			(Transponder->Flag) ? Transponder->Flag->State.GetNumber() : -1,
			(Transponder->Holder) ? Transponder->Holder->State.GetNumber() : -1);
	}
};

CFlagEntity::CFlagEntity () :
IBaseEntity(),
CItemEntity()
{
};

CFlagEntity::CFlagEntity (sint32 Index) :
IBaseEntity(Index),
CItemEntity(Index)
{
};

void CFlagEntity::Think ()
{
	switch (ThinkState)
	{
	case FTS_FLAGSETUP:
		{
			GetMins().Set (-15);
			GetMaxs().Set (15);

			State.GetModelIndex() = ModelIndex((!Model.IsNullOrEmpty()) ? Model.CString() : LinkedItem->WorldModel);
			GetSolid() = SOLID_TRIGGER;
			Touchable = false;
			PhysicsType = PHYSICS_NONE;

			vec3f dest = State.GetOrigin() + vec3f(0, 0, -128);
			CTrace tr (State.GetOrigin(), GetMins(), GetMaxs(), dest, this, CONTENTS_MASK_SOLID);
			if (tr.StartSolid)
			{
				MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Entity is inside a solid brush\n");
				Free ();
				return;
			}

			State.GetOrigin() = tr.EndPosition;

			Link ();

			NextThink = Level.Frame + ServerFramesPerSecond;
			ThinkState = FTS_FLAGTHINK;
		}
		break;
	case FTS_FLAGTHINK:
		if (GetSolid() != SOLID_NOT)
			State.GetFrame() = (173 + (((State.GetFrame() - 173) + 1) % 16));
		NextThink = Level.Frame + ServerFramesPerSecond;
		break;
	default:
		CItemEntity::Think ();
	};
};

bool CFlagEntity::Run ()
{
	return CItemEntity::Run ();
};

void CFlagEntity::Spawn (IBaseItem *Item, ETeamIndex Team)
{
//ZOID
//Don't spawn the flags unless enabled
	if (!(Game.GameMode & GAME_CTF))
	{
		Free ();
		return;
	}

	LinkedItem = Item;

	NextThink = Level.Frame + 2;    // items start after other solids
	ThinkState = FTS_FLAGSETUP;
	PhysicsType = PHYSICS_NONE;

	State.GetEffects() = Item->EffectFlags;
	State.GetRenderEffects() = RF_GLOW | RF_IR_VISIBLE;
	Transponder = QNew (TAG_GAME) CFlagTransponder (Team, this);
};

CRedFlagEntity::CRedFlagEntity () :
IBaseEntity(),
CFlagEntity()
{
};

CRedFlagEntity::CRedFlagEntity (sint32 Index) :
IBaseEntity(Index),
CFlagEntity(Index)
{
};

bool CRedFlagEntity::Run ()
{
	return CFlagEntity::Run ();
};

void CRedFlagEntity::BecomeExplosion (bool grenade)
{
	CTFResetFlag(CTF_TEAM1); // this will free self!
	BroadcastPrintf(PRINT_HIGH, "The %s flag has returned!\n",
		CTFTeamName(CTF_TEAM1));
};


CBlueFlagEntity::CBlueFlagEntity () :
IBaseEntity(),
CFlagEntity()
{
};

CBlueFlagEntity::CBlueFlagEntity (sint32 Index) :
IBaseEntity(Index),
CFlagEntity(Index)
{
};

bool CBlueFlagEntity::Run ()
{
	return CFlagEntity::Run ();
};

void CBlueFlagEntity::BecomeExplosion (bool grenade)
{
	CTFResetFlag(CTF_TEAM2); // this will free self!
	BroadcastPrintf(PRINT_HIGH, "The %s flag has returned!\n",
		CTFTeamName(CTF_TEAM2));
};

class CDroppedFlagEntity : public CFlagEntity
{
public:
	bool Red;

	CDroppedFlagEntity() :
	  CFlagEntity(),
	  Red(false)
	{
	};

	CDroppedFlagEntity(sint32 Index) :
	  IBaseEntity(Index),
	  CFlagEntity(Index),
	  Red(false)
	{
	};

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
		if (((Other == GetOwner()) && (NextThink - Level.Frame > CTF_AUTO_FLAG_RETURN_TIMEOUT-20)))
			return;

		CItemEntity::Touch (Other, plane, surf);
	};

	void Think ()
	{
		if (Red)
		{
			CTFResetFlag (CTF_TEAM1);
			BroadcastPrintf(PRINT_HIGH, "The %s flag has returned!\n",
				CTFTeamName(CTF_TEAM1));
		}
		else
		{
			CTFResetFlag (CTF_TEAM2);
			BroadcastPrintf(PRINT_HIGH, "The %s flag has returned!\n",
				CTFTeamName(CTF_TEAM2));
		}
	};

	void Spawn ()
	{
	};
};

CItemEntity *CFlag::DropItem (IBaseEntity *Entity)
{
	CDroppedFlagEntity	*dropped = QNewEntityOf CDroppedFlagEntity();

	dropped->ClassName = Classname;
	dropped->LinkedItem = this;
	dropped->SpawnFlags = DROPPED_ITEM;
	dropped->State.GetEffects() = EffectFlags;
	dropped->State.GetRenderEffects() = RF_GLOW | RF_IR_VISIBLE;
	dropped->GetMins().Set (-15);
	dropped->GetMaxs().Set (15);
	dropped->State.GetModelIndex() = ModelIndex(WorldModel);
	dropped->GetSolid() = SOLID_TRIGGER;
	dropped->SetOwner (Entity);

	anglef angles;

	if (Entity->EntityFlags & EF_PLAYER)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Entity);
		CTrace	trace;

		angles = Player->Client.ViewAngle.ToVectors ();
		vec3f offset (24, 0, -16);

		vec3f result;
		G_ProjectSource (Player->State.GetOrigin(), offset, angles, result);

		trace (Player->State.GetOrigin(), dropped->GetMins(), dropped->GetMaxs(),
			result, Player, CONTENTS_SOLID);
		dropped->State.GetOrigin() = trace.EndPosition;

		// Check to see which color we are
		dropped->Red = (Player->Client.Persistent.Flag->team == CTF_TEAM1);
		
		// Set up our transponder
		dropped->Transponder = FindTransponder(Player->Client.Persistent.Flag->team);

		// Tell the transponder that player dropped the flag
		if (dropped->Transponder)
		{
			dropped->Transponder->Location = CFlagTransponder::FLAG_DROPPED;
			dropped->Transponder->Flag = dropped;
			dropped->Transponder->Holder = NULL;
		}
	}
	else
	{
		angles = Entity->State.GetAngles().ToVectors();
		dropped->State.GetOrigin() = Entity->State.GetOrigin();
	}

	angles.Forward *= 100;
	dropped->Velocity = angles.Forward;
	dropped->Velocity.Z = 300;

	dropped->NextThink = Level.Frame + CTF_AUTO_FLAG_RETURN_TIMEOUT;
	dropped->Link ();

	return dropped;
};

LINK_ITEM_TO_CLASS (item_flag_team1, CRedFlagEntity);
LINK_ITEM_TO_CLASS (item_flag_team2, CBlueFlagEntity);

#endif
