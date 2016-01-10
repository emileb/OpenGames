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
// cc_junk_entities.cpp
// Entities that share a "Junk" class
//

#include "Local.h"

CJunkList *JunkList;

// Saves currently allocated body numbers
void SaveJunk (CFile &File)
{
	if (!JunkList)
		return; // ????

	File.Write<size_t> (JunkList->ClosedList.size());
	for (TJunkList::iterator it = JunkList->ClosedList.begin(); it != JunkList->ClosedList.end(); ++it)
		File.Write<sint32> ((*it));

	File.Write<size_t> (JunkList->OpenList.size());
	for (TJunkList::iterator it = JunkList->OpenList.begin(); it != JunkList->OpenList.end(); ++it)
		File.Write<sint32> ((*it));
}

// Loads the bodyqueue numbers into allocationzzz
void LoadJunk (CFile &File)
{
	if (!JunkList)
		return; // ????

	size_t num = File.Read<size_t> ();

	for (size_t i = 0; i < num; i++)
		JunkList->ClosedList.push_back (File.Read<sint32> ());

	num = File.Read<size_t> ();

	for (size_t i = 0; i < num; i++)
		JunkList->OpenList.push_back (File.Read<sint32> ());
}

void Init_Junk()
{
	JunkList = QNew (TAG_LEVEL) CJunkList;
}

void Shutdown_Junk ()
{
	if (JunkList)
		QDelete JunkList;
	JunkList = NULL;
}

IJunkEntity::IJunkEntity () :
IBaseEntity()
{
	EntityFlags |= EF_JUNK;
};
IJunkEntity::IJunkEntity (sint32 Index) :
IBaseEntity(Index)
{
	EntityFlags |= EF_JUNK;
};

void IJunkEntity::Die ()
{
	// Take us out of the closed list
	JunkList->ClosedList.remove (State.GetNumber());

	// Add us to the open list
	JunkList->OpenList.push_back (State.GetNumber());

	// Disappear us
	State.GetModelIndex() = 0;
	State.GetEffects() = 0;
	GetSvFlags() = SVF_NOCLIENT;
}

CGibEntity::CGibEntity () :
IBaseEntity(),
ITossProjectile(),
IJunkEntity()
{
};

CGibEntity::CGibEntity (sint32 Index) :
IBaseEntity(Index),
ITossProjectile(Index),
IJunkEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CGibEntity)

/*
=================
Misc functions
=================
*/

void CGibEntity::ClipGibVelocity ()
{
	Velocity.X = Clamp<float> (Velocity.X, -300, 300);
	Velocity.Y = Clamp<float> (Velocity.Y, -300, 300);
	Velocity.Z = Clamp<float> (Velocity.Z, 200, 500); // always some upwards
}

bool CGibEntity::Run ()
{
	return IBounceProjectile::Run();
}

void CGibEntity::Think ()
{
	Die ();
}

void CGibEntity::Spawn (IBaseEntity *Owner, MediaIndex gibIndex, sint32 Damage, EGibType type, EEntityStateEffects effects)
{
	CGibEntity *Junk = JunkList->GetFreeJunk<CGibEntity>();

	vec3f size = Owner->GetSize() * 0.5f;

	vec3f origin = Owner->GetAbsMin() + size;

	Junk->State.GetOrigin().Set (origin.X + crand() * size.X,
							origin.Y + crand() * size.Y,
							origin.Z + crand() * size.Z);

	Junk->State.GetModelIndex() = gibIndex;
	Junk->GetSvFlags() = SVF_DEADMONSTER;
	Junk->GetMins().Clear ();
	Junk->GetMaxs().Clear ();
	Junk->GetSolid() = SOLID_NOT;
	Junk->State.GetEffects() = effects;
	Junk->State.GetRenderEffects() = RF_IR_VISIBLE;

	Junk->BackOff = (type == GIB_ORGANIC) ? 1.0f : 1.5f;
	float vscale = (type == GIB_ORGANIC) ? 0.5f : 1.0f;

	vec3f vd = VelocityForDamage (Damage);

	vec3f velocity = ((Owner->EntityFlags & EF_PHYSICS) ? (entity_cast<IPhysicsEntity>(Owner)->Velocity) : vec3fOrigin);
	velocity.MultiplyAngles (vscale, vd);
	Junk->Velocity = velocity;
	Junk->ClipGibVelocity ();

	Junk->AngularVelocity.Set (crand()*600, crand()*600, crand()*600);
	Junk->NextThink = Level.Frame + 100 + frand()*100;

	Junk->Link ();
}

