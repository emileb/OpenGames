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
// cc_junk_entities.h
// Entities that share a "Junk" class
//

#if !defined(CC_GUARD_JUNK_ENTITIES_H) || !INCLUDE_GUARDS
#define CC_GUARD_JUNK_ENTITIES_H

const int MAX_JUNK = 30;

typedef std::list<sint32> TJunkList;
class CJunkList
{
public:
	sint32				NumAllocatedJunk;

	// OpenList = Junk that is ready to use
	// ClosedList = Junk that is already in use
	TJunkList			OpenList, ClosedList;

	CJunkList () :
	NumAllocatedJunk (0),
	OpenList(),
	ClosedList()
	{
	};

	template <class JunkClassType>
	// Re-sets the entity structure if the
	// entity is not already freed
	JunkClassType *ReallocateEntity (sint32 number)
	{
		JunkClassType *Junk;
		if (Game.Entities[number].Entity)
		{
			Game.Entities[number].Entity->Free();
			QDelete Game.Entities[number].Entity;
			Junk = QNewEntityOf JunkClassType(number);

CC_DISABLE_DEPRECATION
			G_InitEdict (&Game.Entities[number]);
CC_ENABLE_DEPRECATION

			Game.Entities[number].Entity = Junk;
		}
		else
		{
			Junk = QNewEntityOf JunkClassType(number);

CC_DISABLE_DEPRECATION
			G_InitEdict (&Game.Entities[number]);
CC_ENABLE_DEPRECATION

			Game.Entities[number].Entity = Junk;
		}
		return Junk;
	};

	template <class JunkClassType>
	JunkClassType *AllocateEntity ()
	{
		return QNewEntityOf JunkClassType;
	};

	template <class JunkClassType>
	JunkClassType *GetFreeJunk ()
	{
		// Check the open list
		if (OpenList.size())
		{
			// Pop it off the front
			sint32 number = OpenList.front();
			OpenList.pop_front();

			// Throw it in the closed list
			ClosedList.push_back (number);
			return ReallocateEntity<JunkClassType>(number);
		}
		else if (NumAllocatedJunk < MAX_JUNK)
		{
			// Create it
			JunkClassType *Junk = AllocateEntity<JunkClassType>();

			// Throw into closed list
			ClosedList.push_back (Junk->State.GetNumber());

			NumAllocatedJunk++;
			return Junk;
		}
		else
		{
			if (ClosedList.size())
			{
				// Has to be something in closed list
				// Pop the first one off and return that.
				// This should, effectively, remove the last body.
				sint32 number = ClosedList.front();
				ClosedList.pop_front();

				// Revision
				// Push this body to the end of the closed list so we get recycled last
				ClosedList.push_back (number);

				JunkClassType *Junk = ReallocateEntity<JunkClassType>(number);
				Junk->State.GetEvent() = EV_OTHER_TELEPORT;

				return Junk;
			}
		}
		return NULL;
	};
};
extern CJunkList *JunkList;

class IJunkEntity : public virtual IBaseEntity
{
public:
	IJunkEntity ();
	IJunkEntity (sint32 Index);

	virtual void SaveFields (CFile &File)
	{
	}

	virtual void LoadFields (CFile &File)
	{
	}

	void Die (); // CALL THIS WHEN A JUNK IS FREED INSTEAD OF FREE()!
};

/**
\typedef	uint8 EGibType

\brief	Defines an alias representing the type of a gib.
**/
typedef uint8 EGibType;

/**
\enum	

\brief	Values that represent the type of a gib. 
**/
enum
{
	GIB_ORGANIC,
	GIB_METALLIC
};

class CGibEntity : public IJunkEntity, public ITossProjectile, public IThinkableEntity
{
public:
	CGibEntity ();
	CGibEntity (sint32 Index);

	IMPLEMENT_SAVE_HEADER (CGibEntity)

	void SaveFields (CFile &File)
	{
		IThinkableEntity::SaveFields (File);
		IBounceProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IThinkableEntity::LoadFields (File);
		IBounceProjectile::LoadFields (File);
	}

	void ClipGibVelocity ();

	void Think ();

	bool Run ();
	static void Spawn (IBaseEntity *Owner, MediaIndex gibIndex, sint32 Damage, EGibType type, EEntityStateEffects effects = FX_GIB);
};

void Init_Junk();
void Shutdown_Junk ();

#else
FILE_WARNING
#endif
