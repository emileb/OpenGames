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
// cc_memory.cpp
// 
//

#include "Local.h"
#include <memory>

CC_DISABLE_DEPRECATION

CTempHunkSystem CTempHunkSystem::Allocator(MAX_COMPRINT);

const uint32 HEADER_MAGIC_CONSTANT = (('E'<<24)+('N'<<16)+('E'<<8)+'G');

struct SMemSentinel
{
	uint32				Magic;
	void				*Header;

	bool Check (void *CheckHeader)
	{
		return (Magic == HEADER_MAGIC_CONSTANT && Header == CheckHeader);
	};
};

struct SMemHeader
{
	SMemSentinel		SentinelHeader;
	void				*Address;
	size_t				Size, RealSize;
	const char			*FileName;
	EMemoryTag			TagNum;
	uint16				FileLine;
	bool				Array;

	bool Check (bool IsArray)
	{
		return (IsArray == Array && SentinelHeader.Check(this) && ((SMemSentinel*)(((uint8*)this) + RealSize - sizeof(SMemSentinel)))->Check(this));
	};
};

static void *Mem_TagAlloc (size_t Size, const sint32 TagNum, const char *FileName, const int Line, bool IsArray)
{
	size_t RealSize = Size + sizeof(SMemHeader) + sizeof(SMemSentinel);
	SMemHeader *Mem = (SMemHeader*)((TagNum == TAG_GENERIC) ? malloc(RealSize) : gi.TagMalloc(RealSize, TagNum));
	SMemSentinel *Footer = (SMemSentinel*)(((uint8*)Mem) + RealSize - sizeof(SMemSentinel));

	Mem->SentinelHeader.Header = Footer->Header = Mem;
	Mem->TagNum = TagNum;
	Mem->Size = Size;
	Mem->FileName = FileName;
	Mem->FileLine = Line;
	Footer->Magic = Mem->SentinelHeader.Magic = HEADER_MAGIC_CONSTANT;
	Mem->RealSize = RealSize;
	Mem->Array = IsArray;
	Mem->Address = (((uint8*)Mem) + sizeof(SMemHeader));
	Mem_Zero (Mem->Address, Size);

	return Mem->Address;
}

static void Mem_TagFree (void *Pointer, bool IsArray)
{
	if (!Pointer)
		return;

	SMemHeader *Header = (SMemHeader*)(((uint8*)Pointer) - sizeof(SMemHeader));

	if (!Header->Check(IsArray))
		assert (0);

	if (Header->TagNum == TAG_GENERIC)
		free (Header);
	else
		gi.TagFree (Header);
}

void Mem_FreeTag (const sint32 TagNum)
{
	gi.FreeTags (TagNum);
}
CC_ENABLE_DEPRECATION

#ifdef WIN32
#pragma warning(disable : 4290)
#endif

void *operator new(size_t Size, const sint32 TagNum, const int Line, const char *FileName)
{
	return Mem_TagAlloc(Size, TagNum, FileName, Line, false);
}

void *operator new[](size_t Size, const sint32 TagNum, const int Line, const char *FileName)
{
	return Mem_TagAlloc(Size, TagNum, FileName, Line, true);
}

void operator delete(void *Pointer, const sint32 TagNum, const int Line, const char *FileName)
{
	if (Pointer == NULL)
	{
		CC_ASSERT_EXPR (0, "Attempted to free NULL");
		return;
	}

	Mem_TagFree (Pointer, false);
}

void operator delete[](void *Pointer, const sint32 TagNum, const int Line, const char *FileName)
{
	if (Pointer == NULL)
	{
		CC_ASSERT_EXPR (0, "Attempted to free NULL");
		return;
	}

	Mem_TagFree (Pointer, true);
}

void *operator new (size_t Size) throw (std::bad_alloc)
{
	return Mem_TagAlloc(Size, TAG_GENERIC, "null", 0, false);
}

void *operator new[] (size_t Size) throw (std::bad_alloc)
{
	return Mem_TagAlloc(Size, TAG_GENERIC, "null", 0, true);
}

void *operator new (size_t Size, const std::nothrow_t &nothrow_constant) throw()
{
	return Mem_TagAlloc(Size, TAG_GENERIC, "null", 0, false);
}

void *operator new[] (size_t Size, const std::nothrow_t &nothrow_constant) throw()
{
	return Mem_TagAlloc(Size, TAG_GENERIC, "null", 0, true);
}

void operator delete (void *Pointer) throw ()
{
	Mem_TagFree (Pointer, false);
}

void operator delete (void *Pointer, const std::nothrow_t &nothrow_constant) throw()
{
	Mem_TagFree (Pointer, false);
}

void operator delete[] (void *Pointer) throw ()
{
	Mem_TagFree (Pointer, true);
}

void operator delete[] (void *Pointer, const std::nothrow_t &nothrow_constant) throw()
{
	Mem_TagFree (Pointer, true);
}

/*
================
Mem_TagStrDup
================
*/
char *Mem_TagStrDup (const char *in, const sint32 tagNum)
{
	char	*out = QNew(tagNum) char[(size_t)(strlen (in) + 1)];
	Q_strncpyz (out, in, (size_t)(strlen (in) + 1));

	return out;
}
