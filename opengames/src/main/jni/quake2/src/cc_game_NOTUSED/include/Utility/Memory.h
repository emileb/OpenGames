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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//
// cc_memory.h
//

#if !defined(CC_GUARD_MEMORY_H) || !INCLUDE_GUARDS
#define CC_GUARD_MEMORY_H

#include <exception>

/**
\typedef	sint16 EMemoryTag

\brief	Defines an alias representing the memory tags.
**/
typedef sint16 EMemoryTag;

/**
\enum	

\brief	Values that represent memory tags. 
**/
enum
{
	TAG_GENERIC = -1,
	TAG_GAME	= 765,
	TAG_LEVEL,
	TAG_ENTITY
};

char						*Mem_TagStrDup (const char *in, const sint32 tagNum);

inline char *Mem_StrDup(const char *in)
{
	return Mem_TagStrDup(in, TAG_GENERIC);
}

void *operator new(size_t Size, const sint32 TagNum, const int Line, const char *FileName);
void *operator new[](size_t Size, const sint32 TagNum, const int Line, const char *FileName);
void operator delete(void *Pointer, const sint32 TagNum, const int Line, const char *FileName);
void operator delete[](void *Pointer, const sint32 TagNum, const int Line, const char *FileName);

void Mem_FreeTag (const sint32 TagNum);

#define QNew(TagNum)	new((TagNum), (__LINE__), (__FILE__))
#define QDelete	delete

inline void Mem_Zero (void *ptr, size_t size)
{
	memset (ptr, 0, size);
}

class CTempHunkSystem
{
public:
	static CTempHunkSystem Allocator;

	// A constant memory block. No members may be modified.
	class CMemoryBlock
	{
	public:
		CMemoryBlock (const CMemoryBlock &copy) :
		  Start(copy.Start),
		  End(copy.End),
		  Size(copy.Size),
		  system(copy.system)
		{
		}

	private:
		friend class CTempHunkSystem;
		friend class CTempMemoryBlock;

		CTempHunkSystem		*system;

		const void		*Start;
		const void		*End;
		const size_t	Size;

		CMemoryBlock (CTempHunkSystem *_system, const void *_Start, const size_t _Size) :
		  Start(_Start),
		  End((void*)(((uint8*)_Start) + _Size - 1)),
		  Size(_Size),
		  system(_system)
		{
		}

		CMemoryBlock (CTempHunkSystem *_system, const void *_Start, const void *_End) :
		  Start(_Start),
		  End(_End),
		  Size(reinterpret_cast<size_t>((uint8*)_End) - reinterpret_cast<size_t>((uint8*)_Start) + 1),
		  system(_system)
		{
		}

		CMemoryBlock() :
		  Start(NULL),
		  End(NULL),
		  Size(0),
		  system(NULL)
		{
		}

		bool operator== (const CMemoryBlock &left) const
		{
			return (left.Start == Start && left.Size == Size);
		}

		CMemoryBlock &operator= (const CMemoryBlock &left)
		{
			return (*this = CMemoryBlock(left));
		}

		// Splits this memory block into two unique memory blocks.
		std::pair<CMemoryBlock, CMemoryBlock> Split (const size_t Length) const
		{
			if (Size <= Length)
				assert(0); // wat

			void *LeftStart = (void*)((uint8*)Start);
			void *LeftEnd = (void*)((uint8*)Start + Length - 1);
			void *RightStart = (void*)((uint8*)LeftEnd + 1);
			void *RightEnd = (void*)(uint8*)End;

			return std::make_pair<> (CMemoryBlock(system, LeftStart, LeftEnd), CMemoryBlock(system, RightStart, RightEnd));
		}

		bool Attachable (CMemoryBlock &Other) const
		{
			// Is this block the left or the right?
			if (reinterpret_cast<size_t>((uint8*)Other.Start) < reinterpret_cast<size_t>((uint8*)Start))
			{
				// the other block could be the start of this block
				if ((uint8*)Other.End == ((uint8*)Start - 1))
					return true;
			}
			else
			{
				// the other block could be the end of this block
				if ((uint8*)End == ((uint8*)Other.Start - 1))
					return true;
			}

			return false;
		}

		// Re-attaches two memory blocks into one memory block
		static CMemoryBlock Attach (CMemoryBlock Left, CMemoryBlock Right)
		{
			// make sure Left is really the left block.
			if (reinterpret_cast<size_t>((uint8*)Right.Start) < reinterpret_cast<size_t>((uint8*)Left.Start))
				return Attach(Right, Left); // swap it over.

#if !defined(NDEBUG)			
			// fatal if this occurs though
			if (!Left.Attachable(Right))
				assert(0);
#endif

			return CMemoryBlock(Left.system, Left.Start, Left.Size + Right.Size);
		}

	public:

		void Dispose ()
		{
			// send back to system
			system->JoinBlock(*this);
		}

		template<typename T>
		const T			*GetBuffer () const
		{
			return (T*)Start;
		}

		const size_t	GetSize () const
		{
			return Size;
		}
	};

private:
	typedef std::list<CMemoryBlock> TBlockList;

	TBlockList		Open;
	TBlockList		Closed;

public:

	CTempHunkSystem (const size_t startBlock)
	{
		if (startBlock)
			AddBlock (startBlock);
	}

	// For adding existing blocks
	void AddBlock (const void *Start, const void *End)
	{
		// fixme debug checking here plz

		Open.push_back(CMemoryBlock(this, Start, End));
	}
	
	// For adding existing blocks
	void AddBlock (const void *Start, const size_t Length)
	{
		// fixme debug checking here plz

		Open.push_back(CMemoryBlock(this, Start, Length));
	}

	// For adding existing blocks
	void AddBlock (const CMemoryBlock &Block)
	{
		// fixme debug checking here plz

		Open.push_back(Block);
	}

	// For adding a brand new block
	CMemoryBlock AddBlock (const size_t Length, const bool ToClosed = false)
	{
		CMemoryBlock blk = CMemoryBlock(this, new uint8[Length], Length);

		if (!ToClosed)
			Open.push_back(blk);
		else
			Closed.push_back(blk);

		return blk;
	}

	void MoveToClosed (const CMemoryBlock &Block)
	{
		for (TBlockList::iterator it = Open.begin(); it != Open.end(); ++it)
		{
			CMemoryBlock &block = (*it);

			if (block == Block)
			{
				Open.erase(it);
				Closed.push_back(Block);
				return;
			}
		}
	}

	void MoveToOpen(const CMemoryBlock &Block)
	{
		for (TBlockList::iterator it = Closed.begin(); it != Closed.end(); ++it)
		{
			CMemoryBlock &block = (*it);

			if (block == Block)
			{
				Closed.erase(it);
				Open.push_back(Block);
				return;
			}
		}
	}

	void RemoveFromOpen (const CMemoryBlock &Block)
	{
		for (TBlockList::iterator it = Open.begin(); it != Open.end(); ++it)
		{
			CMemoryBlock &block = (*it);

			if (block == Block)
			{
				Open.erase(it);
				return;
			}
		}
	}

	void RemoveFromClosed (const CMemoryBlock &Block)
	{
		for (TBlockList::iterator it = Closed.begin(); it != Closed.end(); ++it)
		{
			CMemoryBlock &block = (*it);

			if (block == Block)
			{
				Closed.erase(it);
				return;
			}
		}
	}

	bool SuitableBlockExists (const size_t ReqLength, CMemoryBlock **Block)
	{
		for (TBlockList::iterator it = Open.begin(); it != Open.end(); ++it)
		{
			CMemoryBlock &block = (*it);

			if (block.Size >= ReqLength)
			{
				*Block = &block;
				return true;
			}
		}

		Block = NULL;
		return false;
	}

	CMemoryBlock Attach (CMemoryBlock Left, CMemoryBlock Right)
	{
		// assumes both are already in Open
		RemoveFromOpen(Left);
		RemoveFromOpen(Right);

		CMemoryBlock Attached = CMemoryBlock::Attach(Left, Right);
		AddBlock(Attached);
		return Attached;
	}

	bool RecursiveJoin (CMemoryBlock Block)
	{
		// search for a suitable joiner
		for (TBlockList::iterator it = Open.begin(); it != Open.end(); ++it)
		{
			CMemoryBlock &block = (*it);

			if (Block.Attachable(block))
				return RecursiveJoin(Attach(Block, block));
		}

		return false; // done
	}

	// Re-add the block back into the collective
	void JoinBlock (CMemoryBlock Block)
	{
		RemoveFromClosed(Block);
		AddBlock(Block);

		RecursiveJoin(Block);
	}

	CMemoryBlock GetBlock (const size_t ReqLength)
	{
		if (ReqLength == 0)
			return CMemoryBlock();

		// First, see if the block already exists somewhere
		CMemoryBlock *ExistingBlock;

		if (SuitableBlockExists(ReqLength, &ExistingBlock))
		{
			// Yes, we have a block.
			// Is it exactly the length we need (lucky!)?
			if (ExistingBlock->Size == ReqLength)
			{
				// Sweet!
				CMemoryBlock Block = CMemoryBlock(*ExistingBlock);

				MoveToClosed(Block);
				return Block;
			}
			else
			{
				// No, so we have to break it up.
				CMemoryBlock Block = CMemoryBlock(*ExistingBlock);
				ExistingBlock = NULL;

				RemoveFromOpen(Block);
				
				std::pair<CMemoryBlock, CMemoryBlock> split = Block.Split(ReqLength);
				Closed.push_back(split.first);
				Open.push_back(split.second);

				return split.first;
			}
		}

		return AddBlock(ReqLength, true);
	}
};

class CTempMemoryBlock
{
	CTempHunkSystem::CMemoryBlock Block;

public:

	CTempMemoryBlock (CTempHunkSystem::CMemoryBlock Block) :
	  Block(Block)
	  {
	  };

	~CTempMemoryBlock ()
	{
		Block.Dispose();
	}

	template<typename T>
	T *GetBuffer()
	{
		return (T*)Block.Start;
	}

	size_t GetSize()
	{
		return Block.Size;
	}
};

#else
FILE_WARNING
#endif

