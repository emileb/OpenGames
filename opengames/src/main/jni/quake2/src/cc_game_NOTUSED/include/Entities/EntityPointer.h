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
// cc_entity_ptr.h
// 
//

#if !defined(CC_GUARD_CC_ENTITY_PTR_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ENTITY_PTR_H

/**
\class	safe_bool_base

\brief	Safe bool base.
		Internal for boolean operations for entity_ptrs.

\author	Paril
\date	12/06/2010
**/
class safe_bool_base
{
protected:
	typedef void (safe_bool_base::*bool_type)() const;
	void this_type_does_not_support_comparisons() const {}

	safe_bool_base() {}
	safe_bool_base(const safe_bool_base&) {}
	safe_bool_base& operator=(const safe_bool_base&) {return *this;}
	~safe_bool_base() {}
};

/**
\class	safe_bool

\brief	Safe bool. 
		Internal for boolean operations for entity_ptrs.

\author	Paril
\date	12/06/2010
**/
template <typename T=void> class safe_bool : public safe_bool_base
{
public:
	operator bool_type() const
	{
		return (static_cast<const T*>(this))->boolean_test()
			? &safe_bool::this_type_does_not_support_comparisons : 0;
	}
protected:
	~safe_bool() {}
};

/**
\class	safe_bool<void>

\brief	Safe bool. 
		Internal for boolean operations for entity_ptrs.

\author	Paril
\date	12/06/2010
**/
template<> class safe_bool<void> : public safe_bool_base
{
public:
	operator bool_type() const
	{
		return boolean_test()==true ? 
			&safe_bool::this_type_does_not_support_comparisons : 0;
	}
protected:
	virtual bool boolean_test() const=0;
	virtual ~safe_bool() {}
};

/**
\fn	template <typename T, typename U> void operator==(const safe_bool<T>& lhs,
	const safe_bool<U>& rhs)

\brief	Equality operator. 
		Internal for boolean operations for entity_ptrs.

\author	Paril
\date	12/06/2010

\param	lhs	The left hand side. 
\param	rhs	The right hand side. 
**/
template <typename T, typename U> 
void operator==(const safe_bool<T>& lhs,const safe_bool<U>& rhs)
{
	lhs.this_type_does_not_support_comparisons();	
	return false;
}

/**
\fn	template <typename T,typename U> void operator!=(const safe_bool<T>& lhs,
	const safe_bool<U>& rhs)

\brief	Inequality operator. 
		Internal for boolean operations for entity_ptrs.

\author	Paril
\date	12/06/2010

\param	lhs	The left hand side. 
\param	rhs	The right hand side. 
**/
template <typename T,typename U> 
void operator!=(const safe_bool<T>& lhs,const safe_bool<U>& rhs)
{
	lhs.this_type_does_not_support_comparisons();
	return false;	
}

/**
\class	CEntityPtrLinkList

\brief	List of entities linked to entity_ptrs.
		Used for unattaching entities from ptrs.

\author	Paril
\date	12/06/2010
**/
class CEntityPtrLinkList
{
public:
	/**
	\typedef	std::map<IBaseEntity*, std::list<void*> > TEntityPtrUsageList
	
	\brief	Defines an alias representing list of entity pointer usages.
	**/
	typedef std::map<IBaseEntity*, std::list<void*> > TEntityPtrUsageList;
	TEntityPtrUsageList List;	// The entity ptr usage list.

	/**
	\fn	void AddEntity (IBaseEntity *Entity, void *Ptr)
	
	\brief	Adds an entity to the list with the entity_ptr 'Ptr'
	
	\author	Paril
	\date	12/06/2010
	
	\param [in,out]	Entity	If non-null, the entity. 
	\param [in,out]	Ptr		If non-null, the pointer. 
	**/
	void AddEntity (IBaseEntity *Entity, void *Ptr)
	{
		TEntityPtrUsageList::iterator it = List.find(Entity);

		if (it != List.end())
			(*it).second.push_back (Ptr);
		else
		{
			std::list<void*> PtrList;
			PtrList.push_back(Ptr);

			List[Entity] = PtrList;
		}
	}

	/**
	\fn	void RemoveEntity (IBaseEntity *Entity, void *Ptr)
	
	\brief	Removes an entity from the list of entity_ptr 'Ptr'.
	
	\author	Paril
	\date	12/06/2010
	
	\param [in,out]	Entity	If non-null, the entity. 
	\param [in,out]	Ptr		If non-null, the pointer. 
	**/
	void RemoveEntity (IBaseEntity *Entity, void *Ptr)
	{
		TEntityPtrUsageList::iterator it = List.find(Entity);

		if (it == List.end())
			CC_ASSERT_EXPR(0, "Wtf?");
		else
		{
			(*it).second.remove (Ptr);

			if (!(*it).second.size())
				List.erase (Entity);
		}
	}
};
	
/**
\fn	CEntityPtrLinkList &UsageList ()

\brief	Return static list.

\author	Paril
\date	12/06/2010

\return	Static entity pointer list. 
**/
CEntityPtrLinkList &UsageList ();

/**
\struct	nullentity_t

\brief	Null entity pointer. Acts as a replacement for NULL, as NULL does not function with it. 

\author	Paril
\date	12/06/2010
**/
struct nullentity_t
{
	static const nullentity_t value;
};

extern const nullentity_t *nullentity;	// Null entity

/**
\class	entity_ptr

\brief	Entity pointer.
		Testing a theory.

\author	Paril
\date	01/06/2010
**/
template <class TType>
class entity_ptr : public safe_bool<entity_ptr<TType> >
{
	TType		*GameEntity;	// The CleanCode entity
	SEntity		*ServerEntity;	// The server entity

public:
	/**
	\fn	entity_ptr()
	
	\brief	Default constructor. 
	
	\author	Paril
	\date	01/06/2010
	**/
	entity_ptr() :
	  GameEntity(NULL),
	  ServerEntity(NULL)
	  {
	  }

	/**
	\fn	~entity_ptr()
	
	\brief	Destructur. Removes the entity ptr from the list.
	
	\author	Paril
	\date	12/06/2010
	**/
	~entity_ptr()
	{
		if (GameEntity)
			UsageList().RemoveEntity(GameEntity, this);
	}

	/**
	\fn	entity_ptr(TType *GameEntity)
	
	\brief	Constructor. Constructs an entity_ptr from a game entity. 
	
	\author	Paril
	\date	01/06/2010
	
	\param [in,out]	GameEntity	If non-null, the game entity. 
	**/
	entity_ptr(TType *GameEntity) :
	  GameEntity(GameEntity)
	  {
		  if (GameEntity)
		  {
			  ServerEntity = GameEntity->GetGameEntity();
			  UsageList().AddEntity (GameEntity, this);
		  }
	  };

	/**
	\fn	entity_ptr(const entity_ptr &Ptr)
	
	\brief	Copy constructor.
	
	\author	Paril
	\date	12/06/2010
	
	\param	Ptr	The pointer. 
	**/
	entity_ptr(const entity_ptr &Ptr)
	{
		GameEntity = Ptr.GameEntity;
		ServerEntity = Ptr.ServerEntity;

		if (GameEntity)
			UsageList().AddEntity (GameEntity, this);
	}

	/**
	\fn	entity_ptr(SEntity *ServerEntity)
	
	\brief	Constructor. Constructs an entity_ptr from a server entity. 
	
	\author	Paril
	\date	01/06/2010
	
	\param [in,out]	ServerEntity	If non-null, the server entity. 
	**/
	entity_ptr(SEntity *ServerEntity) :
	  ServerEntity(ServerEntity)
	  {
		  if (ServerEntity)
		  {
			  GameEntity = entity_cast<TType>(ServerEntity->Entity);

			  if (GameEntity)
				  UsageList().AddEntity (GameEntity, this);
		  }
	  }

	/**
	\fn	entity_ptr &operator= (TType *NewGameEntity)
	
	\brief	Copy operator. Assignment to a new game entity.
	
	\author	Paril
	\date	12/06/2010
	
	\param [in,out]	NewGameEntity	If non-null, the new game entity. 
	
	\return	A shallow copy of this object. 
	**/
	entity_ptr &operator= (TType *NewGameEntity)
	{
		if (GameEntity)
			UsageList().RemoveEntity (GameEntity, this);

		GameEntity = NewGameEntity;
		if (GameEntity)
		{
			ServerEntity = NewGameEntity->GetGameEntity();
			UsageList().AddEntity (GameEntity, this);
		}
		else
			ServerEntity = NULL;

		return *this;
	}

	/**
	\fn	entity_ptr &operator= (SEntity *NewServerEntity)
	
	\brief	Copy operator. Assignment to a server entity.
	
	\author	Paril
	\date	12/06/2010
	
	\param [in,out]	NewServerEntity	If non-null, the new server entity. 
	
	\return	A shallow copy of this object. 
	**/
	entity_ptr &operator= (SEntity *NewServerEntity)
	{
		if (GameEntity)
			UsageList().RemoveEntity (GameEntity, this);

		ServerEntity = NewServerEntity;
		if (ServerEntity)
		{
			GameEntity = entity_cast<TType>(ServerEntity->Entity);
			UsageList().AddEntity (GameEntity, this);
		}
		else
			GameEntity = NULL;

		return *this;
	}

	/**
	\fn	entity_ptr &operator= (const entity_ptr &Ptr)
	
	\brief	Copy operator.
	
	\author	Paril
	\date	12/06/2010
	
	\param	Ptr	The pointer. 
	
	\return	A shallow copy of this object. 
	**/
	entity_ptr &operator= (const entity_ptr &Ptr)
	{
		if (GameEntity)
			UsageList().RemoveEntity (GameEntity, this);

		ServerEntity = Ptr.ServerEntity;
		GameEntity = Ptr.GameEntity;
		if (GameEntity)
			  UsageList().AddEntity (GameEntity, this);

		return *this;
	}
	
	/**
	\fn	bool operator== (const IBaseEntity *Right) const
	
	\brief	Equality operator.
	
	\author	Paril
	\date	12/06/2010
	
	\param	Right	The right. 
	
	\return	True if this entity_ptr points to 'Right'.
	**/
	bool operator== (const IBaseEntity *Right) const
	{
		return (IsValid() && (GameEntity == Right));
	}

	/**
	\fn	bool operator!= (const IBaseEntity *Right) const
	
	\brief	Inequality operator. 
	
	\author	Paril
	\date	12/06/2010
	
	\param	Right	The right. 
	
	\return	True if this entity_ptr does not point to 'Right'.
	**/
	bool operator!= (const IBaseEntity *Right) const
	{
		return !(operator== (Right));
	}

	/**
	\fn	bool operator== (const entity_ptr &Right) const
	
	\brief	Equality operator. 
	
	\author	Paril
	\date	12/06/2010
	
	\param	Right	The right. 
	
	\return	True if this entity_ptr points to the same entity. 
	**/
	bool operator== (const entity_ptr &Right) const
	{
		return ((!Right.IsValid() && !IsValid()) || (Right.GetGameEntity() == GetGameEntity()));
	}

	/**
	\fn	bool operator!= (const entity_ptr &Right) const
	
	\brief	Inequality operator. 
	
	\author	Paril
	\date	12/06/2010
	
	\param	Right	The right. 
	
	\return	True if this entity_ptr does not point to the same entity. 
	**/
	bool operator!= (const entity_ptr &Right) const
	{
		return !((!Right.IsValid() && !IsValid()) || (Right.GetGameEntity() == GetGameEntity()));
	}

	/**
	\fn	bool boolean_test() const
	
	\brief	Safe bool 'operator bool()' test.
	
	\return	true if the test passes, false if the test fails. 
	**/
	bool boolean_test() const
	{
		return IsValid();
	}

	/**
	\fn	void Clear ()
	
	\brief	Clears this object to its blank/initial state.
	**/
	void Clear ()
	{
		UsageList().RemoveEntity (GameEntity, this);
		GameEntity = NULL;
		ServerEntity = NULL;
	}

	/**
	\fn	entity_ptr operator= (const nullentity_t *)
	
	\brief	Empty operator. Acts as = NULL. 
	
	\author	Paril
	\date	01/06/2010
				
	\param	blank	The blank. 
	
	\return	This object. 
	**/
	entity_ptr operator = (const nullentity_t *)
	{
		if (GameEntity)
			Clear ();
		return *this;
	}

	/**
	\fn	TType *operator * () const
	
	\brief	Dereference operator. Dereferencing an entity_ptr yields the game entity. 
	
	\author	Paril
	\date	01/06/2010
	
	\return	The game entity.
	**/
	TType *operator * () const
	{
		return GameEntity;
	}

	/**
	\fn	TType *GetGameEntity () const
	
	\brief	Gets the game entity. 
	
	\return	null if it fails, else the game entity. 
	**/
	TType *GetGameEntity () const
	{
		return GameEntity;
	}

	/**
	\fn	SEntity *GetServerEntity () const
	
	\brief	Gets the server entity. 
	
	\return	null if it fails, else the server entity. 
	**/
	SEntity *GetServerEntity () const
	{
		return ServerEntity;
	}

	/**
	\fn	bool IsValid () const
	
	\brief	Query if this entity pointer is valid. 
	
	\return	true if valid, false if not. 
	**/
	bool IsValid () const
	{
		return (ServerEntity && ServerEntity->Server.InUse && !ServerEntity->FreeTime && GameEntity && !GameEntity->Freed);
	}

	/**
	\fn	TType *operator-> () const
	
	\brief	Member access operator.
	
	\author	Paril
	\date	01/06/2010
	
	\return	Game entity pointer for member access.
	**/
	TType *operator-> () const
	{
		return GameEntity;
	}

	/**
	\fn	void Write (CFile &File) const
	
	\brief	Writes the entity number for the entity this ptr points to, if any.
	
	\author	Paril
	\date	12/06/2010
	
	\param [in,out]	File	The CFile & to write. 
	**/
	void Write (CFile &File) const
	{
		sint32 EntityNumber = -1;
		if (IsValid())
			EntityNumber = GameEntity->State.GetNumber();
		File.Write<sint32>(EntityNumber);
	}

	/**
	\fn	static entity_ptr Read (CFile &File)
	
	\brief	Reads an entity_ptr from a file and returns it.
	
	\author	Paril
	\date	12/06/2010
	
	\param [in,out]	File	The CFile & to read. 
	
	\return	. 
	**/
	static entity_ptr Read (CFile &File)
	{
		sint32 EntityNumber = File.Read<sint32> ();
		entity_ptr Ptr;
		
		if (EntityNumber != -1)
			Ptr = entity_ptr(Game.Entities[EntityNumber].Entity);
		return Ptr;
	}
};

/**
\fn	template <class TType> static bool operator== (const IBaseEntity *Left,
	const entity_ptr<TType> &Right)

\brief	Equality operator.

\author	Paril
\date	12/06/2010

\param	Left	The left. 
\param	Right	The right. 

\return	true if the 'Left' is the entity contained in 'Right'. 
**/
template <class TType>
static bool operator== (const IBaseEntity *Left, const entity_ptr<TType> &Right)
{
	return (Right.IsValid() && (*Right == Left));
}

/**
\fn	template <class TType> static bool operator!= (const IBaseEntity *Left,
	const entity_ptr<TType> &Right)

\brief	Inequality operator. 

\author	Paril
\date	12/06/2010

\param	Left	The left. 
\param	Right	The right. 

\return	true if the 'Left' is not the entity contained in 'Right'. 
**/
template <class TType>
static bool operator!= (const IBaseEntity *Left, const entity_ptr<TType> &Right)
{
	return !(Right.IsValid() && (*Right == Left));
}

/**
\fn	void FixAllEntityPtrs (IBaseEntity *Entity)

\brief	Function to clear entity pointers that point to Entity.

\author	Paril
\date	12/06/2010

\param [in,out]	Entity	If non-null, the entity. 
**/
void FixAllEntityPtrs (IBaseEntity *Entity);

#else
FILE_WARNING
#endif