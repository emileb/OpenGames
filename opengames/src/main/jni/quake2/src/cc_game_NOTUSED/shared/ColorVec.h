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

// ColorVec.h

/**
\class	colorBase

\brief	Color base class. 

\author	Paril
\date	27/05/2010
**/
template <typename TType>
class colorBase
{
public:
	/**
	 * Data
	 */
	TType R, G, B, A;

	/**
	 * Constructors
	 */

	/**
	\fn	colorBase()
	
	\brief	Default constructor. 
	
	\author	Paril
	\date	27/05/2010
	**/
	colorBase() :
	  R(0),
	  G(0),
	  B(0),
	  A(0)
	  {
	  }

	/**
	\fn	inline colorBase(const TType InR, const TType InG, const TType InB, const TType InA)
	
	\brief	Constructor. Initializes a color with four separate color values.
	
	\author	Paril
	\date	27/05/2010
	
	\param	InR	The in r. 
	\param	InG	The in g. 
	\param	InB	The in b. 
	\param	InA	The in a. 
	**/
	inline colorBase(const TType InR, const TType InG, const TType InB, const TType InA) :
	  R(InR),
	  G(InG),
	  B(InB),
	  A(InA)
	{
	}

	/**
	 * Operators
	 */

	/**
	\fn	inline bool operator!= (const colorBase &Other)
	
	\brief	Inequality operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Other	The other. 
	
	\return	true if the parameters are not considered equivalent. 
	**/
	inline bool operator != (const colorBase &Other)
	{
		return (R != Other.R || G != Other.G || B != Other.B || A != Other.A);
	}

	/**
	\fn	inline bool operator== (const colorBase &Other)
	
	\brief	Equality operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Other	The other. 
	
	\return	true if the parameters are considered equivalent. 
	**/
	inline bool operator == (const colorBase &Other)
	{
		return (R == Other.R && G == Other.G && B == Other.B && A == Other.A);
	}

	/**
	\fn	inline TType &operator[] (const sint32 Index)
	
	\brief	[] casting operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Index	Zero-based index of color. 
	
	\return	R if Index is 0, G if 1, B if 2, A if 3. 
	**/
	inline TType &operator [] (const sint32 Index)
	{
		return (&R)[Index];
	}

	/**
	\fn	inline operator TType * ()
	
	\brief	Pointer operator. 
	
	\author	Paril
	\date	27/05/2010

	\return	Pointer to R.
	**/
	inline operator TType * ()
	{
		return (&R);
	}

	/**
	\fn	inline colorBase &operator= (const colorBase &Other)
	
	\brief	Copy operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Other	The other color. 
	
	\return	A reference to this color. 
	**/
	inline colorBase &operator = (const colorBase &Other)
	{
		R = Other.R;
		G = Other.G;
		B = Other.B;
		A = Other.A;

		return *this;
	}

	/**
	 * Functions
	 */

	/**
	\fn	inline void Set (const colorBase &Other)
	
	\brief	Sets. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Other	The other. 
	**/
	inline void Set (const colorBase &Other)
	{
		R = Other.R;
		G = Other.G;
		B = Other.B;
		A = Other.A;
	}

	/**
	\fn	inline void Set (const TType InR, const TType InG, const TType InB, const TType InA)
	
	\brief	Sets this color to InR, InG, InB, InA
	
	\author	Paril
	\date	27/05/2010
	
	\param	InR	The in r. 
	\param	InG	The in g. 
	\param	InB	The in b. 
	\param	InA	The in a. 
	**/
	inline void Set (const TType InR, const TType InG, const TType InB, const TType InA)
	{
		R = InR;
		G = InG;
		B = InB;
		A = InA;
	}

	/**
	\fn	inline float DistanceFrom (colorBase &Other)
	
	\brief	The color's distance to another color.
	
	\author	Paril
	\date	27/05/2010
	
	\param [in,out]	Other	The other. 
	
	\return	. 
	**/
	inline float DistanceFrom (colorBase &Other)
	{
		return 
			((R - Other.R) * (R - Other.R)) +
			((R - Other.G) * (G - Other.G)) +
			((R - Other.B) * (B - Other.B)) +
			((R - Other.A) * (A - Other.A));
	}
	
	/**
	\fn	colorBase GetNormalized (float &norm)
	
	\brief	Normalizes the color and returns the normalized value.
	
	\author	Paril
	\date	27/05/2010
	
	\param [in,out]	norm	The normalize value to be stored. 

	\note	The alpha value is not modified.
	
	\return	The normalized. 
	**/
	colorBase GetNormalized (float &norm)
	{
		colorBase out;
		float f = Max<>(Max<>(R, G), B);

		if (f > 1.0f)
		{
			f = 1.0f / f;
			out.R = ((float)R) * f;
			out.G = ((float)G) * f;
			out.B = ((float)B) * f;
			out.A = A;
		}
		else
			out = *this;

		norm = f;
		return out;
	};

	/**
	\fn	inline colorBase GetNormalized ()
	
	\brief	Gets the normalized color.
	
	\return	The normalized. 
	**/
	inline colorBase GetNormalized ()
	{
		float t;
		return GetNormalized(t);
	};

	/**
	\fn	inline void Normalize ()
	
	\brief	Normalizes this color.
	**/
	inline void Normalize ()
	{
		*this = GetNormalized();
	};
};

/*
==============================================================================

	colorf
 
==============================================================================
*/

/**
\class	colorf

\brief	Float color. 

\author	Paril
\date	27/05/2010
**/
class colorf : public colorBase<float>
{
public:
	/**
	 * Constructors
	 */

	/**
	\fn	colorf()
	
	\brief	Default constructor. 
	
	\author	Paril
	\date	27/05/2010
	**/
	colorf() : 
	  colorBase<float>()
	  {
	  }

	/**
	\fn	inline colorf(const colorf &Other)
	
	\brief	Copy constructor. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Other	The other. 
	**/
	inline colorf(const colorf &Other) : 
	  colorBase<float>(Other.R, Other.G, Other.B, Other.A)
	  {
	  }

	/**
	\fn	inline colorf(const colorb &Other)
  
	\brief	Constructor. 
			Construct a colorf from a colorb.
  
	\author	Paril
	\date	27/05/2010
  
	\param	Other	The other. 
	**/
	inline colorf(const class colorb &Other);

	/**
	\fn	inline colorf(const float InR, const float InG, const float InB, const float InA = 1.0f)
  
	\brief	Constructor. 
			Construct a colorf from four color values.
  
	\author	Paril
	\date	27/05/2010  
	
	\param	InR	The in R value. 
	\param	InG	The in G value. 
	\param	InB	The in B value. 
	\param	InA	The in A value. 
	**/
	inline colorf(const float InR, const float InG, const float InB, const float InA = 1.0f) :
	  colorBase<float>(InR, InG, InB, InA)
	  {
	  }

	/**
	\fn	inline colorf &operator= (const colorf &Other)
	
	\brief	Copy operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Other	The other. 
	
	\return	Reference to this color.
	**/
	inline colorf &operator = (const colorf &Other)
	{
		R = Other.R;
		G = Other.G;
		B = Other.B;
		A = Other.A;

		return *this;
	}
};

/*
==============================================================================

	colorb
 
==============================================================================
*/

/**
\class	colorb

\brief	Byte color. 

\author	Paril
\date	27/05/2010
**/
class colorb : public colorBase<uint8>
{
public:
	/**
	 * Constructors
	 */

	/**
	\fn	colorb()
	
	\brief	Default constructor. 
	
	\author	Paril
	\date	27/05/2010
	**/
	colorb() :
	  colorBase<uint8>()
	  {
	  }

	/**
	\fn	inline colorb(const colorb &Other)
	
	\brief	Copy constructor. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Other	The other. 
	**/
	inline colorb(const colorb &Other) :
	  colorBase<uint8>(Other.R, Other.G, Other.B, Other.A)
	  {
	  }

	/**
	\fn	inline colorb(const uint8 InR, const uint8 InG, const uint8 InB, const uint8 InA = 255)
	
	\brief	Constructor.
			Construct a colorb from four color values.
	
	\author	Paril
	\date	27/05/2010
	
	\param	InR	The in R value. 
	\param	InG	The in G value. 
	\param	InB	The in B value. 
	\param	InA	The in A value. 
	**/
	inline colorb(const uint8 InR, const uint8 InG, const uint8 InB, const uint8 InA = 255) :
	  colorBase<uint8>(InR, InG, InB, InA)
	  {
	  }

	/**
	\fn	inline colorb(const colorf &Other)
	
	\brief	Constructor. 
			Construct a colorb from a colorf.
	
	\author	Paril
	\date	27/05/2010
	
	\param	Other	The other color. 
	**/
	inline colorb(const colorf &Other) : 
	  colorBase<uint8>(FloatToByte(Other.R), FloatToByte(Other.G), FloatToByte(Other.B), FloatToByte(Other.A))
	  {
	  }

	/**
	\fn	inline colorb &operator= (const colorb &Other)
	
	\brief	Copy operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Other	The other. 
	
	\return	Reference to this color.
	**/
	inline colorb &operator = (const colorb &Other)
	{
		R = Other.R;
		G = Other.G;
		B = Other.B;
		A = Other.A;

		return *this;
	}
};

/**
\fn	inline colorf::colorf(const colorb &Other)
  
\brief	Constructor. 
		Construct a colorf from a colorb.
  
\author	Paril
\date	27/05/2010
  
\param	Other	The other. 
**/
inline colorf::colorf(const colorb &Other) :
  colorBase<float>(((float)Other.R) / 255, ((float)Other.G) / 255, ((float)Other.B) / 255, ((float)Other.A) / 255)
  {
  }

inline uint8 HexColor_GetR (sint32 rgba)
{
	return (rgba & 0xFF);
}

inline uint8 HexColor_GetG (sint32 rgba)
{
	return ((rgba >> 8) & 0xFF);
}

inline uint8 HexColor_GetB (sint32 rgba)
{
	return ((rgba >> 16) & 0xFF);
}

inline uint8 HexColor_GetA (sint32 rgba)
{
	return ((rgba >> 24) & 0xFF);
}

inline sint32 Color_RGBToHex (uint8 r, uint8 g, uint8 b)
{
	return ((r << 0) | (g << 8) | (b << 16));
}

inline sint32 Color_RGBAToHex (uint8 r, uint8 g, uint8 b, uint8 a)
{
	return (r | (g << 8) | (b << 16) | (a << 24));
}

