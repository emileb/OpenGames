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
// cc_bodyqueue.cpp
// Bodyqueue class
//

#include "Local.h"

class CBody : public IHurtableEntity, public ITossProjectile, public IThinkableEntity
{
public:
	class CBodyQueue *BodyQueueList; // Queue the body belongs to

	/**
	\fn	CBody ()
	
	\brief	Default constructor.
	
	\author	Paril
	\date	25/05/2010
	**/
	CBody ();

	/**
	\fn	CBody (sint32 Index)
	
	\brief	Constructor. 
	
	\author	Paril
	\date	25/05/2010
	
	\param	Index	Zero-based index of the entity in the entity list. 
	**/
	CBody (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CBody)

	bool			ParseField (const char *Key, const char *Value)
	{
		return true;
	};

	void			SaveFields (CFile &File)
	{
		IHurtableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
		ITossProjectile::SaveFields (File);
	};
	void			LoadFields (CFile &File);

	/**
	\fn	void TossHead (sint32 Damage)
	
	\brief	Turns this body into a head and throws it by a velocity derived from 'Damage'
	
	\author	Paril
	\date	25/05/2010
	
	\param	Damage	The damage dealt to the body.
	**/
	void TossHead (sint32 Damage);

	/**
	\fn	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
	
	\brief	Death handler. 
	
	\author	Paril
	\date	25/05/2010
	
	\param [in,out]	Inflictor	If non-null, the inflictor. 
	\param [in,out]	Attacker	If non-null, the attacker. 
	\param	Damage				The damage. 
	\param [in,out]	Point		The point of impact. 
	**/
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);

	/**
	\fn	void Think ()
	
	\brief	Think handler. Only done if we're a head. 
	**/
	void Think	();

	/**
	\fn	bool Run ()
	
	\brief	Runs this object.
	
	\return	true if it succeeds, false if it fails. 
	**/
	bool Run ();
};

/**
\fn	CBody::CBody ()
	
\brief	Default constructor.
	
\author	Paril
\date	25/05/2010
**/
CBody::CBody () :
IBaseEntity(),
IHurtableEntity(),
BodyQueueList(NULL)
{
};

/**
\fn	CBody::CBody (sint32 Index)
	
\brief	Constructor. 
	
\author	Paril
\date	25/05/2010
	
\param	Index	Zero-based index of the entity in the entity list. 
**/
CBody::CBody (sint32 Index) :
IBaseEntity(Index),
IHurtableEntity(Index),
BodyQueueList(NULL)
{
};

IMPLEMENT_SAVE_SOURCE (CBody)

/**
\fn	bool CBody::Run ()
	
\brief	Runs this object.
	
\return	true if it succeeds, false if it fails. 
**/
bool CBody::Run ()
{
	return (GetSvFlags() & SVF_NOCLIENT) ? false : ITossProjectile::Run();
}

/**
\fn	void CBody::TossHead (sint32 Damage)
	
\brief	Turns this body into a head and throws it by a velocity derived from 'Damage'
	
\author	Paril
\date	25/05/2010
	
\param	Damage	The damage dealt to the body.
**/
void CBody::TossHead (sint32 Damage)
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
	AffectedByKnockback = false;

	BackOff = 1.5f;	
	Velocity += VelocityForDamage (Damage);

	NextThink = Level.Frame + 100 + frand()*100;

	Link ();
}

/**
\typedef	std::list<CBody*> TBodyQueueList

\brief	Defines an alias representing list of body queues.
**/
typedef std::list<CBody*> TBodyQueueList;

class CBodyQueue
{
public:
	TBodyQueueList OpenList; // A list of entity numbers that are currently waiting a body
	TBodyQueueList ClosedList; // A list of entity numbers that are currently being used, in order of accessed.
	// If OpenList is empty, pop the first one off of ClosedList.

	/**
	\fn	CBodyQueue::CBodyQueue (sint32 MaxSize)

	\brief	Constructor.

	\author	Paril
	\date	25/05/2010

	\param	MaxSize Size of the body queue. 
	**/
	CBodyQueue (sint32 MaxSize);

	/**
	\fn	CBody *GrabFreeBody ()
	
	\brief	Grab free body.
	
	\return	Pointer to a CBody entity. This function will only fail if the bodyqueue's size is 0, which should never happen. 
	**/
	CBody *GrabFreeBody ();

	/**
	\fn	void CopyBodyToQueue (CPlayerEntity *Player)

	\brief	Creates the actual body entity and copies 'Player''s data into it. 

	\author	Paril
	\date	25/05/2010

	\param [in,out]	Player	The player that needs a body. 
	**/
	void CopyBodyToQueue (CPlayerEntity *Player);
};

CBodyQueue	*BodyQueue;

void	CBody::LoadFields (CFile &File)
{
	BodyQueueList = BodyQueue;
	
	IHurtableEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);
	ITossProjectile::LoadFields (File);
};

/**
\fn	void CBody::Think ()
	
\brief	Think handler. Only done if we're a head. 
**/
void CBody::Think ()
{
	// Take us out of the closed list
	BodyQueueList->ClosedList.remove (this);

	// Add us to the open list
	BodyQueueList->OpenList.push_back (this);

	// Disappear us
	State.GetModelIndex() = State.GetEffects() = 0;
	GetSvFlags() = SVF_NOCLIENT;
}

/**
\fn	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
	
\brief	Death handler. 
	
\author	Paril
\date	25/05/2010
	
\param [in,out]	Inflictor	If non-null, the inflictor. 
\param [in,out]	Attacker	If non-null, the attacker. 
\param	Damage				The damage. 
\param [in,out]	Point		The point of impact. 
**/
void CBody::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	if (Health < -40)
	{
		PlaySound(CHAN_BODY, SoundIndex ("misc/udeath.wav"));
		for (sint32 n = 0; n < 4; n++)
			CGibEntity::Spawn (this, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
			
		State.GetOrigin().Z -= 16;
		TossHead (Damage);
	}
}

/**
\fn	CBody *CBodyQueue::GrabFreeBody ()
	
\brief	Grab free body.
	
\return	Pointer to a CBody entity. This function will only fail if the bodyqueue's size is 0, which should never happen. 
**/
CBody *CBodyQueue::GrabFreeBody ()
{
	// If there's anything in the open List..
	if (OpenList.size())
	{
		// Pop it off the front
		CBody *Body = OpenList.front();
		OpenList.pop_front();

		// Throw it in the closed list
		ClosedList.push_back (Body);
		return Body;
	}
	// Has to be something in closed list
	// Pop the first one off and return that.
	// This should, effectively, remove the last body.
	CBody *Body = ClosedList.front();
	ClosedList.pop_front();

	// Revision
	// Push this body to the end of the closed list so we get recycled last
	ClosedList.push_back (Body);
	return Body;
}

/**
\fn	CBodyQueue::CBodyQueue (sint32 MaxSize)

\brief	Constructor.

\author	Paril
\date	25/05/2010

\param	MaxSize Size of the body queue. 
**/
CBodyQueue::CBodyQueue (sint32 MaxSize)
{
	// Add MaxSize entities to the body queue (reserved entities)
	for (sint32 i = 0; i < MaxSize; i++)
	{
		CBody *Body = QNewEntityOf CBody ();
		Body->ClassName = "bodyqueue";
		Body->BodyQueueList = this;
		Body->GetSvFlags() = SVF_NOCLIENT;
		Body->Link ();
		Body->GetInUse() = true;

		OpenList.push_back(Body);
	}
};

/**
\fn	void BodyQueue_Init (sint32 Reserve)

\brief	Initialize the body queue reserving 'Reserve' bodies. 

\author	Paril
\date	25/05/2010

\param	Reserve	The amount of bodies to reserve. 
**/
void BodyQueue_Init (sint32 Reserve)
{
	BodyQueue = QNew (TAG_LEVEL) CBodyQueue (Reserve);
}

/**
\fn	void SaveBodyQueue (CFile &File)

\brief	Saves currently allocated body numbers 

\author	Paril
\date	25/05/2010

\param [in,out]	File	The file. 
**/
void SaveBodyQueue (CFile &File)
{
	if (!BodyQueue)
		return; // ????

	File.Write<size_t> (BodyQueue->ClosedList.size());
	for (TBodyQueueList::iterator it = BodyQueue->ClosedList.begin(); it != BodyQueue->ClosedList.end(); ++it)
		File.Write<sint32> ((*it)->State.GetNumber());

	File.Write<size_t> (BodyQueue->OpenList.size());
	for (TBodyQueueList::iterator it = BodyQueue->OpenList.begin(); it != BodyQueue->OpenList.end(); ++it)
		File.Write<sint32> ((*it)->State.GetNumber());
}

/**
\fn	void LoadBodyQueue (CFile &File)

\brief	Loads the bodyqueue numbers into allocation 

\author	Paril
\date	25/05/2010

\param [in,out]	File	The file. 
**/
void LoadBodyQueue (CFile &File)
{
	if (!BodyQueue)
		return; // ????

	size_t num = File.Read <size_t> ();
	for (size_t i = 0; i < num; i++)
		BodyQueue->ClosedList.push_back (entity_cast<CBody>(Game.Entities[File.Read <sint32> ()].Entity));

	num = File.Read <size_t> ();
	for (size_t i = 0; i < num; i++)
		BodyQueue->OpenList.push_back (entity_cast<CBody>(Game.Entities[File.Read <sint32> ()].Entity));
}

/**
\fn	void ShutdownBodyQueue ()

\brief	Shutdown the body queue. Frees related entities and clears state. 

\author	Paril
\date	25/05/2010
**/
void ShutdownBodyQueue ()
{
	if (BodyQueue)
		QDelete BodyQueue;
	BodyQueue = NULL;
}

/**
\fn	void CPlayerEntity::CopyToBodyQueue ()

\brief	Copies player to body queue.
**/
void CPlayerEntity::CopyToBodyQueue ()
{
	BodyQueue->CopyBodyToQueue (this);
}

/**
\fn	void CBodyQueue::CopyBodyToQueue (CPlayerEntity *Player)

\brief	Creates the actual body entity and copies 'Player''s data into it. 

\author	Paril
\date	25/05/2010

\param [in,out]	Player	The player that needs a body. 
**/
void CBodyQueue::CopyBodyToQueue (CPlayerEntity *Player)
{
	// Grab a free body
	CBody *Body = GrabFreeBody();

	// Make sure it doesn't interpolate
	Body->State.GetEvent() = EV_OTHER_TELEPORT;

	Player->Unlink();
	Body->Unlink();

	Body->State.GetAngles() = Player->State.GetAngles();
	Body->State.GetAngles().X = 0;
	Body->State.GetEffects() = Player->State.GetEffects();
	Body->State.GetFrame() = Player->State.GetFrame();
	Body->State.GetModelIndex() = Player->State.GetModelIndex();
	Body->State.GetOldOrigin() = Player->State.GetOldOrigin();
	Body->State.GetOrigin() = Player->State.GetOrigin();
	Body->State.GetRenderEffects() = Player->State.GetRenderEffects();
	Body->State.GetSkinNum() = Player->State.GetSkinNum();

	Body->GetSvFlags() = Player->GetSvFlags();
	Body->GetMins() = Player->GetMins();
	Body->GetMaxs() = Player->GetMaxs();
	Body->GetAbsMin() = Player->GetAbsMin();
	Body->GetAbsMax() = Player->GetAbsMax();
	Body->GetSize() = Player->GetSize();
	Body->GetSolid() = Player->GetSolid();
	Body->GetClipmask() = Player->GetClipmask();
	Body->Velocity.Clear ();
	Body->SetOwner (Player->GetOwner());

	Body->BackOff = 1.0f;
	Body->CanTakeDamage = true;

	// Implied that Player is a dead-head (lol)
	if (!Player->CanTakeDamage)
		Body->TossHead (0);

	Body->Link();
}
