/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or v

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

// MathLib.h

#ifndef M_PI
# define M_PI			3.14159265358979323846f		// matches value in gcc v2 math.h
#endif

// angle indexes
enum
{
	PITCH,		// up / down
	YAW,		// left / right
	ROLL		// fall over
};

inline float DEG2RAD (const float v)
{
	return v * (M_PI / 180.0f);
}

inline float RAD2DEG (const float v)
{
	return v * (180.0f / M_PI);
}


inline sint16 ANGLE2SHORT (float x)
{
	return ((sint32)(x*65536/360) & 65535);
}

inline float SHORT2ANGLE (sint16 x)
{
	return (x*(360.0f/65536));
}


inline uint8 ANGLE2BYTE (float x)
{
	return ((sint32)(x*256/360) & 255);
}

inline float BYTE2ANGLE (uint8 x)
{
	return (x*(360.0f/256));
}

// ===========================================================================

#define LARGE_EPSILON 0.1
#define SMALL_EPSILON 0.01
#define TINY_EPSILON 0.001

#define TINY_NUMBER (1.e-8)
#define SMALL_NUMBER (1.e-4)

#ifndef ArrayCount
# define ArrayCount(a) (sizeof(a)/sizeof((a)[0]))
#endif

// ===========================================================================

inline uint8		FloatToByte(float x)
{
	union
	{
		float			f;
		uint32			i;
	} f2i;

	// Shift float to have 8bit fraction at base of number
	f2i.f = x + 32768.0f;
	f2i.i &= 0x7FFFFF;

	// Then read as integer and kill float bits...
	return (uint8)Min<sint32>(f2i.i, 255);
}

// ===========================================================================

#ifdef id386
inline __declspec_naked long Q_ftol(float f)
{
	static sint32	tmp;
	__asm {
		fld dword ptr [esp+4]
		fistp tmp
		mov eax, tmp
		ret
	}
}

inline float	Q_FastSqrt (float value)
{
	float result = 0;
	__asm {
		mov eax, value
		sub eax, 0x3f800000
		sar eax, 1
		add eax, 0x3f800000
		mov result, eax
	}
	return result;
}

#else // id386
inline long Q_ftol (float f) { return ((long)f); }
inline float Q_FastSqrt (float value) { return sqrt(value); }
#endif // id386

template <typename TType>
inline float Abs (TType val)
{
	return ((val < 0) ? -val : val);
}

// ===========================================================================

inline sint32 Q_rint (float x)
{
	return (x < 0 ? ((sint32)(x-0.5f)) : ((sint32)(x+0.5f)));
}

inline void Q_SinCosf(const float X, float *Sin, float *Cos)
{
#if defined(HAVE_SINCOSF)
	sincosf(X, Sin, Cos);
#elif defined(id386)
	__asm fld X
	__asm fsincos
	__asm mov ecx, Cos
	__asm mov edx, Sin
	__asm fstp dword ptr [ecx]
	__asm fstp dword ptr [edx]
#else
	*Sin = sinf(X);
	*Cos = cosf(X);
#endif
}

template <typename T>
inline T Q_RSqrt (T number)
{
	if (number == 0.0)
		return 0.0;

	T	y;
	*((sint32 *)&y) = 0x5f3759df - ((* (sint32 *) &number) >> 1);
	return y * (1.5 - (number * 0.5 * y * y));
}

inline sint32		Q_log2 (sint32 val)
{
	sint32 answer = 0;
	while(val >>= 1)
		answer++;
	return answer;
}

template<typename TType>
inline TType Q_NearestPow(const TType &Value, const bool bRoundDown)
{
	TType Result;
	for (Result = 1; Result < Value; Result <<= 1) ;

	if (bRoundDown && Result > Value)
		Result >>= 1;

	return Result;
}

// ===========================================================================

//
// m_angles.c
//
inline float		AngleModf (float a)
{
	return (360.0f/65536.0f) * ((sint32)(a*(65536.0f/360.0f)) & 65535);
}

inline float		LerpAngle (float a1, float a2, float frac)
{
	if (a1 - a2 > 180)
		a1 -= 360;
	if (a1 - a2 < -180)
		a1 += 360;

	return a2 + frac * (a1 - a2);
}

#include "Vector.h"

extern vec3f	vec3fOrigin,
				upOrigin;
// ===========================================================================

//
// m_bounds.c
//

class CBounds
{
	vec3f mins, maxs;
public:

	CBounds (vec3f mins, vec3f maxs) :
	  mins (mins),
	  maxs (maxs)
	  {
	  };

	CBounds ()
	{
		Clear ();
	};

	void AddPoint (vec3f v)
	{
		// X Vector
		if (v.X < mins.X)
			mins.X = v.X;
		if (v.X > maxs.X)
			maxs.X = v.X;

		// Y Vector
		if (v.Y < mins.Y)
			mins.Y = v.Y;
		if (v.Y > maxs.Y)
			maxs.Y = v.Y;

		// Z Vector
		if (v.Z < mins.Z)
			mins.Z = v.Z;
		if (v.Z > maxs.Z)
			maxs.Z = v.Z;
	};

	// Sphere intersect
	bool Intersects (const vec3f &centre, float radius)
	{
		return (mins <= (centre+radius) && maxs >= (centre+radius));
	};

	// Intersects other bounds
	bool Intersects (const CBounds &right)
	{
		return (mins <= right.mins && maxs >= right.maxs);
	};

	void Clear ()
	{
		mins = maxs = 999999;
	};

	float		RadiusFromBounds ()
	{
		return (mins.GetAbsolute() > maxs.GetAbsolute() ? mins.GetAbsolute() : maxs.GetAbsolute()).Length();
	};

	vec3f &GetMins() { return mins; };
	vec3f &GetMaxs() { return maxs; };
};
