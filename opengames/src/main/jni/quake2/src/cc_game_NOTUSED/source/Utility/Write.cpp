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
// cc_write.cpp
// "Write" functions to write and check for malformed writes.
//

#ifndef WIN32
#include <float.h>
#endif

#include "Local.h"

// Helper functions
/*
=============================================================================

	MATHLIB

=============================================================================
*/

const int NUMVERTEXNORMALS	= 162;

vec3f m_byteDirs[NUMVERTEXNORMALS] = {
	vec3f(-0.525731f,	0.000000f,	0.850651f),		vec3f(-0.442863f,	0.238856f,	0.864188f),		vec3f(-0.295242f,	0.000000f,	0.955423f),
	vec3f(-0.309017f,	0.500000f,	0.809017f),		vec3f(-0.162460f,	0.262866f,	0.951056f),		vec3f(0.000000f,		0.000000f,	1.000000f),
	vec3f(0.000000f,		0.850651f,	0.525731f),		vec3f(-0.147621f,	0.716567f,	0.681718f),		vec3f(0.147621f,		0.716567f,	0.681718f),
	vec3f(0.000000f,		0.525731f,	0.850651f),		vec3f(0.309017f,		0.500000f,	0.809017f),		vec3f(0.525731f,		0.000000f,	0.850651f),
	vec3f(0.295242f,		0.000000f,	0.955423f),		vec3f(0.442863f,		0.238856f,	0.864188f),		vec3f(0.162460f,		0.262866f,	0.951056f),
	vec3f(-0.681718f,	0.147621f,	0.716567f),		vec3f(-0.809017f,	0.309017f,	0.500000f),		vec3f(-0.587785f,	0.425325f,	0.688191f),
	vec3f(-0.850651f,	0.525731f,	0.000000f),		vec3f(-0.864188f,	0.442863f,	0.238856f),		vec3f(-0.716567f,	0.681718f,	0.147621f),
	vec3f(-0.688191f,	0.587785f,	0.425325f),		vec3f(-0.500000f,	0.809017f,	0.309017f),		vec3f(-0.238856f,	0.864188f,	0.442863f),
	vec3f(-0.425325f,	0.688191f,	0.587785f),		vec3f(-0.716567f,	0.681718f,	-0.147621f),	vec3f(-0.500000f,	0.809017f,	-0.309017f),
	vec3f(-0.525731f,	0.850651f,	0.000000f),		vec3f(0.000000f,		0.850651f,	-0.525731f),	vec3f(-0.238856f,	0.864188f,	-0.442863f),
	vec3f(0.000000f,		0.955423f,	-0.295242f),	vec3f(-0.262866f,	0.951056f,	-0.162460f),	vec3f(0.000000f,		1.000000f,	0.000000f),
	vec3f(0.000000f,		0.955423f,	0.295242f),		vec3f(-0.262866f,	0.951056f,	0.162460f),		vec3f(0.238856f,		0.864188f,	0.442863f),
	vec3f(0.262866f,		0.951056f,	0.162460f),		vec3f(0.500000f,		0.809017f,	0.309017f),		vec3f(0.238856f,		0.864188f,	-0.442863f),
	vec3f(0.262866f,		0.951056f,	-0.162460f),	vec3f(0.500000f,		0.809017f,	-0.309017f),	vec3f(0.850651f,		0.525731f,	0.000000f),
	vec3f(0.716567f,		0.681718f,	0.147621f),		vec3f(0.716567f,		0.681718f,	-0.147621f),	vec3f(0.525731f,		0.850651f,	0.000000f),
	vec3f(0.425325f,		0.688191f,	0.587785f),		vec3f(0.864188f,		0.442863f,	0.238856f),		vec3f(0.688191f,		0.587785f,	0.425325f),
	vec3f(0.809017f,		0.309017f,	0.500000f),		vec3f(0.681718f,		0.147621f,	0.716567f),		vec3f(0.587785f,		0.425325f,	0.688191f),
	vec3f(0.955423f,		0.295242f,	0.000000f),		vec3f(1.000000f,		0.000000f,	0.000000f),		vec3f(0.951056f,		0.162460f,	0.262866f),
	vec3f(0.850651f,		-0.525731f,	0.000000f),		vec3f(0.955423f,		-0.295242f,	0.000000f),		vec3f(0.864188f,		-0.442863f,	0.238856f),
	vec3f(0.951056f,		-0.162460f,	0.262866f),		vec3f(0.809017f,		-0.309017f,	0.500000f),		vec3f(0.681718f,		-0.147621f,	0.716567f),
	vec3f(0.850651f,		0.000000f,	0.525731f),		vec3f(0.864188f,		0.442863f,	-0.238856f),	vec3f(0.809017f,		0.309017f,	-0.500000f),
	vec3f(0.951056f,		0.162460f,	-0.262866f),	vec3f(0.525731f,		0.000000f,	-0.850651f),	vec3f(0.681718f,		0.147621f,	-0.716567f),
	vec3f(0.681718f,		-0.147621f,	-0.716567f),	vec3f(0.850651f,		0.000000f,	-0.525731f),	vec3f(0.809017f,		-0.309017f,	-0.500000f),
	vec3f(0.864188f,		-0.442863f,	-0.238856f),	vec3f(0.951056f,		-0.162460f,	-0.262866f),	vec3f(0.147621f,		0.716567f,	-0.681718f),
	vec3f(0.309017f,		0.500000f,	-0.809017f),	vec3f(0.425325f,		0.688191f,	-0.587785f),	vec3f(0.442863f,		0.238856f,	-0.864188f),
	vec3f(0.587785f,		0.425325f,	-0.688191f),	vec3f(0.688191f,		0.587785f,	-0.425325f),	vec3f(-0.147621f,	0.716567f,	-0.681718f),
	vec3f(-0.309017f,	0.500000f,	-0.809017f),	vec3f(0.000000f,		0.525731f,	-0.850651f),	vec3f(-0.525731f,	0.000000f,	-0.850651f),
	vec3f(-0.442863f,	0.238856f,	-0.864188f),	vec3f(-0.295242f,	0.000000f,	-0.955423f),	vec3f(-0.162460f,	0.262866f,	-0.951056f),
	vec3f(0.000000f,		0.000000f,	-1.000000f),	vec3f(0.295242f,		0.000000f,	-0.955423f),	vec3f(0.162460f,		0.262866f,	-0.951056f),
	vec3f(-0.442863f,	-0.238856f,	-0.864188f),	vec3f(-0.309017f,	-0.500000f,	-0.809017f),	vec3f(-0.162460f,	-0.262866f,	-0.951056f),
	vec3f(0.000000f,		-0.850651f,	-0.525731f),	vec3f(-0.147621f,	-0.716567f,	-0.681718f),	vec3f(0.147621f,		-0.716567f,	-0.681718f),
	vec3f(0.000000f,		-0.525731f,	-0.850651f),	vec3f(0.309017f,		-0.500000f,	-0.809017f),	vec3f(0.442863f,		-0.238856f,	-0.864188f),
	vec3f(0.162460f,		-0.262866f,	-0.951056f),	vec3f(0.238856f,		-0.864188f,	-0.442863f),	vec3f(0.500000f,		-0.809017f,	-0.309017f),
	vec3f(0.425325f,		-0.688191f,	-0.587785f),	vec3f(0.716567f,		-0.681718f,	-0.147621f),	vec3f(0.688191f,		-0.587785f,	-0.425325f),
	vec3f(0.587785f,		-0.425325f,	-0.688191f),	vec3f(0.000000f,		-0.955423f,	-0.295242f),	vec3f(0.000000f,		-1.000000f,	0.000000f),
	vec3f(0.262866f,		-0.951056f,	-0.162460f),	vec3f(0.000000f,		-0.850651f,	0.525731f),		vec3f(0.000000f,		-0.955423f,	0.295242f),
	vec3f(0.238856f,		-0.864188f,	0.442863f),		vec3f(0.262866f,		-0.951056f,	0.162460f),		vec3f(0.500000f,		-0.809017f,	0.309017f),
	vec3f(0.716567f,		-0.681718f,	0.147621f),		vec3f(0.525731f,		-0.850651f,	0.000000f),		vec3f(-0.238856f,	-0.864188f,	-0.442863f),
	vec3f(-0.500000f,	-0.809017f,	-0.309017f),	vec3f(-0.262866f,	-0.951056f,	-0.162460f),	vec3f(-0.850651f,	-0.525731f,	0.000000f),
	vec3f(-0.716567f,	-0.681718f,	-0.147621f),	vec3f(-0.716567f,	-0.681718f,	0.147621f),		vec3f(-0.525731f,	-0.850651f,	0.000000f),
	vec3f(-0.500000f,	-0.809017f,	0.309017f),		vec3f(-0.238856f,	-0.864188f,	0.442863f),		vec3f(-0.262866f,	-0.951056f,	0.162460f),
	vec3f(-0.864188f,	-0.442863f,	0.238856f),		vec3f(-0.809017f,	-0.309017f,	0.500000f),		vec3f(-0.688191f,	-0.587785f,	0.425325f),
	vec3f(-0.681718f,	-0.147621f,	0.716567f),		vec3f(-0.442863f,	-0.238856f,	0.864188f),		vec3f(-0.587785f,	-0.425325f,	0.688191f),
	vec3f(-0.309017f,	-0.500000f,	0.809017f),		vec3f(-0.147621f,	-0.716567f,	0.681718f),		vec3f(-0.425325f,	-0.688191f,	0.587785f),
	vec3f(-0.162460f,	-0.262866f,	0.951056f),		vec3f(0.442863f,		-0.238856f,	0.864188f),		vec3f(0.162460f,		-0.262866f,	0.951056f),
	vec3f(0.309017f,		-0.500000f,	0.809017f),		vec3f(0.147621f,		-0.716567f,	0.681718f),		vec3f(0.000000f,		-0.525731f,	0.850651f),
	vec3f(0.425325f,		-0.688191f,	0.587785f),		vec3f(0.587785f,		-0.425325f,	0.688191f),		vec3f(0.688191f,		-0.587785f,	0.425325f),
	vec3f(-0.955423f,	0.295242f,	0.000000f),		vec3f(-0.951056f,	0.162460f,	0.262866f),		vec3f(-1.000000f,	0.000000f,	0.000000f),
	vec3f(-0.850651f,	0.000000f,	0.525731f),		vec3f(-0.955423f,	-0.295242f,	0.000000f),		vec3f(-0.951056f,	-0.162460f,	0.262866f),
	vec3f(-0.864188f,	0.442863f,	-0.238856f),	vec3f(-0.951056f,	0.162460f,	-0.262866f),	vec3f(-0.809017f,	0.309017f,	-0.500000f),
	vec3f(-0.864188f,	-0.442863f,	-0.238856f),	vec3f(-0.951056f,	-0.162460f,	-0.262866f),	vec3f(-0.809017f,	-0.309017f,	-0.500000f),
	vec3f(-0.681718f,	0.147621f,	-0.716567f),	vec3f(-0.681718f,	-0.147621f,	-0.716567f),	vec3f(-0.850651f,	0.000000f,	-0.525731f),
	vec3f(-0.688191f,	0.587785f,	-0.425325f),	vec3f(-0.587785f,	0.425325f,	-0.688191f),	vec3f(-0.425325f,	0.688191f,	-0.587785f),
	vec3f(-0.425325f,	-0.688191f,	-0.587785f),	vec3f(-0.587785f,	-0.425325f,	-0.688191f),	vec3f(-0.688191f,	-0.587785f,	-0.425325f)
};

/*
=================
DirToByte

This isn't a real cheap function to call!
=================
*/
uint8 DirToByte(const vec3f &dirVec)
{
	if (!dirVec)
		return 0;

	uint8 best = 0;
	float bestDot = 0;
	for (uint8 i = 0; i < NUMVERTEXNORMALS; i++)
	{
		float dot = vec3f(m_byteDirs[i]) | dirVec;
		if (dot > bestDot)
		{
			bestDot = dot;
			best = i;
		}
	}

	return best;
}

/*
=================
ByteToDir
=================
*/
void ByteToDir(const uint8 dirByte, vec3f &dirVec)
{
	if (dirByte >= NUMVERTEXNORMALS)
	{
		dirVec.Clear();
		return;
	}

	dirVec = m_byteDirs[dirByte];
}

CC_DISABLE_DEPRECATION
void _WriteChar (sint8 val)
{
	gi.WriteChar (val);
}

void _WriteByte (uint8 val)
{
	gi.WriteByte (val);
}

void _WriteShort (sint16 val)
{
	gi.WriteShort (val);
}

void _WriteLong (long val)
{
	gi.WriteLong (val);
}

void _WriteFloat (float val)
{
	gi.WriteFloat (val);
}

void _WriteString (const char *val)
{
	gi.WriteString ((char*)val);
}

/**
\typedef	uint8 EWriteType

\brief	Defines an alias representing type of data to write.
**/
typedef uint8 EWriteType;

/**
\enum	

\brief	Values that represent the types of data that can be written. 
**/
enum
{
	WT_CHAR,
	WT_BYTE,
	WT_SHORT,
	WT_LONG,
	WT_FLOAT,
	WT_STRING,
};

class CWriteIndex
{
public:
	EWriteType	Type;

	CWriteIndex (EWriteType Type) :
	Type(Type)
	{
	};

	virtual ~CWriteIndex ()
	{
	};

	virtual void Write ()
	{
		CC_ASSERT_EXPR (0, "WriteIndex has invalid write ID");
	};
};

template <typename TType>
class CWritePrimIndex : public CWriteIndex
{
public:
	TType	Val;

	CWritePrimIndex (TType Val, EWriteType Type) :
	Val(Val),
	CWriteIndex(Type)
	{
	};

	void Write ()
	{
		switch (Type)
		{
		case WT_CHAR:
			_WriteChar (Val);
			break;
		case WT_BYTE:
			_WriteByte (Val);
			break;
		case WT_SHORT:
			_WriteShort (Val);
			break;
		case WT_LONG:
			_WriteLong (Val);
			break;
		case WT_FLOAT:
			_WriteFloat (Val);
			break;
		};
	};
};

class CWriteString : public CWriteIndex
{
public:
	String	Val;

	CWriteString (String Val) :
	  Val(Val),
	  CWriteIndex(WT_STRING)
	{
	};

	void Write ()
	{
		_WriteString (Val.CString());
	};
};

std::vector <CWriteIndex*> WriteQueue;

void SendQueue (CPlayerEntity *To, bool Reliable)
{
	for (size_t i = 0; i < WriteQueue.size(); i++)
		WriteQueue[i]->Write ();

	gi.unicast (To->GetGameEntity(), (Reliable) ? 1 : 0);
}

void Clear ()
{
	for (size_t i = 0; i < WriteQueue.size(); i++)
		QDelete WriteQueue[i];
	WriteQueue.clear();
}

// vec3f overloads
void Cast (ECastType castType, ECastFlags castFlags, vec3f &Origin, CPlayerEntity *Ent, bool SuppliedOrigin)
{
	// Sanity checks
	if (castType == CAST_MULTI && Ent)
		DebugPrint ("Multicast with an associated Ent\n");
	else if (castType == CAST_MULTI && !SuppliedOrigin)
	{
		DebugPrint ("Multicast with no associated Origin! Can't do!\n");
		Clear ();
		return;
	}
	else if (castType == CAST_UNI && !Ent)
	{
		DebugPrint ("Unicast with no associated Ent! Can't do!\n");
		Clear ();
		return;
	}

	// Sends to all entities
	switch (castType)
	{
	case CAST_MULTI:
		for (sint32 i = 1; i <= Game.MaxClients; i++)
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);

			if (!Player || !Player->GetInUse() || (Player->Client.Persistent.State != SVCS_SPAWNED))
				continue;

			if ((castFlags & CASTFLAG_PVS) && !InVisibleArea(Origin, Player->State.GetOrigin()))
				continue;
			if ((castFlags & CASTFLAG_PHS) && !InHearableArea(Origin, Player->State.GetOrigin()))
				continue;

			SendQueue (Player, (castFlags & CASTFLAG_RELIABLE) ? true : false);
		}
		break;
	// Send to one entity
	case CAST_UNI:
		if ((castFlags & CASTFLAG_PVS) && !InVisibleArea(Origin, Ent->State.GetOrigin()))
			break;
		if ((castFlags & CASTFLAG_PHS) && !InHearableArea(Origin, Ent->State.GetOrigin()))
			break;

		SendQueue (Ent, (castFlags & CASTFLAG_RELIABLE) ? true : false);
		break;
	}

	Clear ();
}
void Cast (ECastFlags castFlags, vec3f &Origin)
{
	Cast (CAST_MULTI, castFlags, Origin, NULL, true);
}
void Cast (ECastFlags castFlags, CPlayerEntity *Ent)
{
	Cast (CAST_UNI, castFlags, vec3fOrigin, Ent, false);
}

void WriteChar (sint8 val)
{
	if (CC_ASSERT_EXPR_MINOR(!(val < CHAR_MIN || val > CHAR_MAX), "Malformed char written!"))
		val = Clamp<char> (val, CHAR_MIN, CHAR_MAX);

	WriteQueue.push_back (QNew (TAG_GENERIC) CWritePrimIndex<sint8> (val, WT_CHAR));
}

void WriteByte (uint8 val)
{
	if (CC_ASSERT_EXPR_MINOR(!(val < 0 || val > UCHAR_MAX), "Malformed uint8 written!"))
		val = Clamp<uint8> (val, 0, UCHAR_MAX);

	WriteQueue.push_back (QNew (TAG_GENERIC) CWritePrimIndex<uint8> (val, WT_BYTE));
}

void WriteShort (sint16 val)
{
	if (CC_ASSERT_EXPR_MINOR(!(val < SHRT_MIN || val > SHRT_MAX), "Malformed sint16 written!"))
		val = Clamp<sint16> (val, SHRT_MIN, SHRT_MAX);

	WriteQueue.push_back (QNew (TAG_GENERIC) CWritePrimIndex<sint16> (val, WT_SHORT));
}

void WriteLong (long val)
{
	if (CC_ASSERT_EXPR_MINOR(!(val < LONG_MIN || val > LONG_MAX), "Malformed long written!"))
		val = Clamp<long> (val, LONG_MIN, LONG_MAX);

	WriteQueue.push_back (QNew (TAG_GENERIC) CWritePrimIndex<long> (val, WT_LONG));
}

void WriteFloat (float val)
{
	if (CC_ASSERT_EXPR_MINOR(!(val < FLT_MIN || val > FLT_MAX), "Malformed float written!"))
		val = Clamp<float> (val, FLT_MIN, FLT_MAX);

	WriteQueue.push_back (QNew (TAG_GENERIC) CWritePrimIndex<float> (val, WT_FLOAT));
}

void WriteAngle (float val)
{
	CC_ASSERT_EXPR_MINOR (!(val < 0 || val > 360), "Malformed angle may have been written!");

	WriteByte (ANGLE2BYTE (val));
}

void WriteAngle16 (float val)
{
	CC_ASSERT_EXPR_MINOR (!(val < 0 || val > 360), "Malformed angle may have been written!");

	WriteShort (ANGLE2SHORT (val));
}

void WriteString (const char *val)
{
	CC_ASSERT_EXPR_MINOR (!(!val || val == NULL || !val[0] || strlen(val) > 1400), "Malformed string written");

	WriteQueue.push_back (QNew (TAG_GENERIC) CWriteString (String(val)));
}

void WriteCoord (float f)
{
	WriteQueue.push_back (QNew (TAG_GENERIC) CWritePrimIndex<sint16> ((f * 8), WT_SHORT));
}

void WritePosition (vec3f val)
{
	if (!val)
	{
		for (sint32 i = 0; i < 3; i++)
			WriteCoord(vec3fOrigin[i]);
	}
	else
	{
		bool Printed = false;
		for (sint32 i = 0; i < 3; i++)
		{
			if (CC_ASSERT_EXPR_MINOR (!(!Printed && (val[i] > 4096 || val[i] < -4096)), "Malformed position may have been written!"))
				Printed = true;

			WriteCoord(val[i]);
		}
	}
}

void WriteDirection (vec3f val)
{
	WriteByte (DirToByte (val));
}

CC_ENABLE_DEPRECATION
