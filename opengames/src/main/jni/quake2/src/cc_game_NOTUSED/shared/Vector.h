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

// Vector.h

/*
==============================================================================

	vec3Base
 
==============================================================================
*/

template<class vecType>
class angleBase
{
public:
	vecType			Forward,
					Right,
					Up;

	angleBase () :
	  Forward(),
	  Right(),
	  Up()
	{
	}

	angleBase (vecType angle)
	{
		*this = FromVector(angle);
	}

	angleBase (vecType forward, vecType right, vecType up) :
	  Forward(forward),
	  Right(right),
	  Up(up)
	{
	}

	static angleBase<vecType> FromVector (vecType vec)
	{
		angleBase<vecType> angles;

		float sr, sp, sy, cr, cp, cy;
		Q_SinCosf(DEG2RAD (vec.X), &sp, &cp);
		Q_SinCosf(DEG2RAD (vec.Y), &sy, &cy);
		Q_SinCosf(DEG2RAD (vec.Z), &sr, &cr);

		angles.Forward = vec3f(cp* cy, cp*sy, -sp);
		angles.Right = vec3f(-1 * sr * sp * cy + -1 * cr * -sy,
					-1 * sr * sp * sy + -1 * cr * cy,
					-1 * sr * cp);
		angles.Up = vec3f(cr * sp * cy + -sr * -sy,
					cr * sp * sy + -sr * cy,
					cr * cp);

		return angles;
	}
};

/**
\class	vec3Base

\brief	3d vector base class.
		TType is the type stored as values of X, Y and Z.

\author	Paril
\date	27/05/2010
**/
template<typename TType>
class vec3Base
{
public:
	/**
	 * Data
	 */
	TType			X, Y, Z;

	/**
	 * Constructors
	 */
	vec3Base () :
	  X(0), 
	  Y(0),
	  Z(0)
	  {
	  };

	/**
	\fn	inline vec3Base (const vec3Base &Vec)
	
	\brief	Copy constructor.
			Construct this vector from another vector.
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The source vector. 
	**/
	inline vec3Base (const vec3Base &Vec) :
	  X(Vec[0]),
	  Y(Vec[1]),
	  Z(Vec[2])
	  {
	  };

	/**
	\fn	template <typename TNewType> inline vec3Base (const vec3Base<TNewType> &Vec)
	
	\brief	Copy constructor for copying from a different base type.
	
	\author	Paril
	\date	13/10/2010
	
	\param	Vec	The vector. 
	**/
	template <typename TNewType>
	inline vec3Base (const vec3Base<TNewType> &Vec) :
	  X((TType)Vec[0]),
	  Y((TType)Vec[1]),
	  Z((TType)Vec[2])
	  {
	  };

	/**
	\fn	inline vec3Base(const TType Value)
	
	\brief	Constructor.
			Initializes this vector with all one value.
	
	\author	Paril
	\date	27/05/2010
	
	\param	Value	The value to set X, Y and Z to. 
	**/
	inline vec3Base(const TType Value) :
	  X(Value),
	  Y(Value),
	  Z(Value)
	  {
	  };

	/**
	\fn	inline vec3Base(const TType Values[3])
	
	\brief	Constructor.
			Initializes this vector with an array of three TTypes.
			Also accepts pointers.
	
	\author	Paril
	\date	27/05/2010
	
	\param	Values	The value array.
	**/
	inline vec3Base(const TType Values[3]) :
	  X(Values[0]),
	  Y(Values[1]),
	  Z(Values[2])
	  {
	  };

	/**
	\fn	inline vec3Base(const TType InX, const TType InY, const TType InZ)
	
	\brief	Constructor.
			Sets the X, Y and Z values to 'InX', 'InY' and 'InZ'.
	
	\author	Paril
	\date	27/05/2010
	
	\param	InX	The wanted x coordinate. 
	\param	InY	The wanted y coordinate. 
	\param	InZ	The wanted z coordinate. 
	**/
	inline vec3Base(const TType InX, const TType InY, const TType InZ) :
	  X(InX),
	  Y(InY),
	  Z(InZ)
	  {
	  };

	/**
	 * Operators
	 */
	/**
	\fn	inline bool operator!= (const vec3Base &Vec) const
	
	\brief	Inequality operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector. 
	
	\return	true if the parameters are not considered equivalent. 
	**/
	inline bool operator != (const vec3Base &Vec) const
	{
		return (X != Vec[0] || Y != Vec[1] || Z != Vec[2]);
	}

	/**
	\fn	inline bool operator== (const vec3Base &Vec) const
	
	\brief	Equality operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector. 
	
	\return	true if the parameters are considered equivalent. 
	**/
	inline bool operator == (const vec3Base &Vec) const
	{
		return (X == Vec[0] && Y == Vec[1] && Z == Vec[2]);
	}

	/**
	\fn	inline TType &operator[] (const sint32 Index) const
	
	\brief	[] casting operator. 
			For old-style operations and looping.
			0 is X, 1 is Y and 2 is Z.
	
	\author	Paril
	\date	27/05/2010
	
	\param	Index	Zero-based index of the vector. 
	
	\return	TType of the index wanted. 
	**/
	inline TType &operator [] (const sint32 Index)
	{
		return (&X)[Index];
	}

	/**
	\fn	inline operator TType *() const
	
	\brief	Pointer conversion. 
	
	\author	Paril
	\date	27/05/2010
	**/
	inline operator TType *()
	{
		return (&X);
	}

	/**
	\fn	inline operator const TType *() const
	
	\brief	Const pointer conversion.
	
	\return	. 
	**/
	inline operator const TType *() const
	{
		return (&X);
	}

	/**
	 * Operators
	 */
	/**
	\fn	inline vec3Base operator* (const vec3Base &Vec) const
	
	\brief	Muliplication operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector to multiply this vector by. 
	
	\return	The new vector. 
	**/
	inline vec3Base operator * (const vec3Base &Vec) const
	{
		return vec3Base(X * Vec.X, Y * Vec.Y, Z * Vec.Z);
	}

	/**
	\fn	inline vec3Base operator* (const float Scale) const
	
	\brief	Muliplication operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Scale	The scale to scale this vector by. 
	
	\return	This vector, scaled by 'Scale'
	**/
	inline vec3Base operator * (const float Scale) const
	{
		return vec3Base(X * Scale, Y * Scale, Z * Scale);
	}

	/**
	\fn	inline vec3Base &operator*= (const vec3Base &Vec)
	
	\brief	Assignment by muliplication operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector to multiply by. 
	
	\return	A reference of this object. 
	**/
	inline vec3Base &operator *= (const vec3Base &Vec)
	{
		Set (X * Vec.X, Y * Vec.Y, Z * Vec.Z);
		return *this;
	}

	/**
	\fn	inline vec3Base &operator*= (const float Scale)
	
	\brief	Assignment by muliplication operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Scale	The scale to multiply by. 
	
	\return	A reference of this object. 
	**/
	inline vec3Base &operator *= (const float Scale)
	{
		Set (X * Scale, Y * Scale, Z * Scale);
		return *this;
	}

	/**
	\fn	inline vec3Base operator+ (const vec3Base &Vec) const
	
	\brief	Addition operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector to add to this vector. 
	
	\return	The result of the operation. 
	**/
	inline vec3Base operator + (const vec3Base &Vec) const
	{
		return vec3Base(X + Vec.X, Y + Vec.Y, Z + Vec.Z);
	}

	/**
	\fn	inline vec3Base &operator+= (const vec3Base &Vec)
	
	\brief	Assignment by addition operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector to add to this vector. 
	
	\return	A reference of this object. 
	**/
	inline vec3Base &operator += (const vec3Base &Vec)
	{
		Set (X + Vec.X, Y + Vec.Y, Z + Vec.Z);
		return *this;
	}

	/**
	\fn	inline vec3Base operator- (const vec3Base &Vec) const
	
	\brief	Negation operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector to subtract from this vector. 
	
	\return	The result of the operation. 
	**/
	inline vec3Base operator - (const vec3Base &Vec) const
	{
		return vec3Base(X - Vec.X, Y - Vec.Y, Z - Vec.Z);
	}

	/**
	\fn	inline vec3Base &operator-= (const vec3Base &Vec)
	
	\brief	Assignment by subtraction operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector to subtract from this vector. 
	
	\return	A reference of this object. 
	**/
	inline vec3Base &operator -= (const vec3Base &Vec)
	{
		Set (X - Vec.X, Y - Vec.Y, Z - Vec.Z);
		return *this;
	}

	/**
	\fn	inline vec3Base operator/ (const vec3Base &Vec) const
	
	\brief	Division operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector to multiply this vector by. 
	
	\return	The result of the operation. 
	**/
	inline vec3Base operator / (const vec3Base &Vec) const
	{
		return vec3Base(X / Vec.X, Y / Vec.Y, Z / Vec.Z);
	}

	/**
	\fn	inline vec3Base operator/ (const TType Number) const
	
	\brief	Division operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Number	Value to divide by. 
	
	\return	The result of the operation. 
	**/
	inline vec3Base operator / (const TType Number) const
	{
		const TType InvNumber = 1.0f / (float)Number;
		return vec3Base(X * InvNumber, Y * InvNumber, Z * InvNumber);
	}

	/**
	\fn	inline vec3Base &operator/= (const vec3Base &Vec)
	
	\brief	Assignment by division operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector to divide this vector by. 
	
	\return	A reference of this object. 
	**/
	inline vec3Base &operator /= (const vec3Base &Vec)
	{
		Set (X / Vec.X, Y / Vec.Y, Z / Vec.Z);
		return *this;
	}

	/**
	\fn	inline vec3Base &operator/= (const float Number)
	
	\brief	Assignment by division operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Number	Number to divide this vector by. 
	
	\return	A reference of this object. 
	**/
	inline vec3Base &operator /= (const float Number)
	{
		const float InvNumber = 1.0f / (float)Number;
		Set (X * InvNumber, Y * InvNumber, Z * InvNumber);
		return *this;
	}

	/**
	\fn	inline bool operator < (const vec3Base &Vec) const
	
	\brief	Less than operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	Vector to compare this vector to 
	
	\return	True if this vector's values are less than 'Vec'. 
	**/
	inline bool operator < (const vec3Base &Vec) const
	{
		return (X < Vec.X && Y < Vec.Y && Z < Vec.Z);
	}

	/**
	\fn	inline bool operator < (const TType Number) const
	
	\brief	Less than operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Number	Number to compare this vector to 
	
	\return	True if this vector's values are less than 'Number'. 
	**/
	inline bool operator < (const TType Number) const
	{
		return (X < Number && Y < Number && Z < Number);
	}

	/**
	\fn	inline vec3Base &operator= (const vec3Base &Vec)
	
	\brief	Copy operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector that this vector will be set to. 
	
	\return	A reference of this object. 
	**/
	inline vec3Base &operator = (const vec3Base &Vec)
	{
		Set (Vec);
		return *this;
	}

	/**
	\fn	template <typename TNewType> inline vec3Base &operator= (const vec3Base<TNewType> &Vec)
	
	\brief	Copy operator for a new type.
	
	\author	Paril
	\date	13/10/2010
	
	\param	Vec	The vector. 
	
	\return	A shallow copy of this object. 
	**/
	template <typename TNewType>
	inline vec3Base &operator= (const vec3Base<TNewType> &Vec)
	{
		Set (Vec);
		return *this;
	}

	/**
	\fn	inline bool operator > (const vec3Base &Vec) const
	
	\brief	Less than operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	Vector to compare this vector to 
	
	\return	True if this vector's values are greater than than 'Vec'. 
	**/
	inline bool operator > (const vec3Base &Vec) const
	{
		return (X > Vec.X && Y > Vec.Y && Z > Vec.Z);
	}
	
	/**
	\fn	inline bool operator > (const TType Number) const
	
	\brief	Less than operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Number	Number to compare this vector to 
	
	\return	True if this vector's values are less than 'Number'. 
	**/
	inline bool operator > (const TType Number) const
	{
		return (X > Number && Y > Number && Z > Number);
	}

	/**
	\fn	inline bool operator <= (const vec3Base &Vec) const
	
	\brief	Less than or equals operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	Vector to compare this vector to 
	
	\return	True if this vector's values are less than or equal to 'Vec'. 
	**/
	inline bool operator <= (const vec3Base &Vec) const
	{
		return (operator< (Vec) || operator== (Vec));
	}

	/**
	\fn	inline bool operator >= (const vec3Base &Vec) const
	
	\brief	Greater than or equals operator. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	Vector to compare this vector to 
	
	\return	True if this vector's values are greater than or equal to 'Vec'. 
	**/
	inline bool operator >= (const vec3Base &Vec) const
	{
		return (operator> (Vec) || operator== (Vec));
	}

	/**
	\fn	inline vec3Base operator^ (const vec3Base &Vec) const
	
	\brief	Bitwise 'exclusive or' operator.
			Used for cross-product.
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector to calculate cross product with. 
	
	\return	The result of the operation. 
	**/
	inline vec3Base operator ^ (const vec3Base &Vec) const
	{
		return Cross (Vec);
	}

	/**
	\fn	inline float operator| (const vec3Base &Vec) const
	
	\brief	Bitwise 'or' operator.
			Used for dot-product.
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The second vector in dot product operation. 
	
	\return	The result of the operation. 
	**/
	inline float operator | (const vec3Base &Vec) const
	{
		return Dot (Vec);
	}

	/**
	\fn	inline vec3Base operator- () const
	
	\brief	Inversion operator.
	
	\author	Paril
	\date	27/05/2010
	
	\return	The negative of this vector.. 
	**/
	inline vec3Base operator - () const
	{
		return vec3Base(-X, -Y, -Z);
	}

	/**
	 * Functions
	 */

	/**
	\fn	inline void Clear()
	
	\brief	Clears this object to its blank/initial state. 
	**/
	inline void Clear()
	{
		X = Y = Z = 0;
	}

	/**
	\fn	inline bool Compare (const vec3Base &Vec) const
	
	\brief	Compares this const vec3Base & object to another.
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The constant vector to compare to this object. 
	
	\return	True if they are equal, false if they are not. 
	**/
	inline bool Compare (const vec3Base &Vec) const
	{
		return (X == Vec.X && Y == Vec.Y && Z == Vec.Z);
	}

	/**
	\fn	inline void Copy (const vec3Base &Vec)
	
	\brief	Copies the given Vec to this one.
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector. 
	**/
	inline void Copy (const vec3Base &Vec)
	{
		Set (Vec);
	}

	/**
	\fn	inline void Invert ()
	
	\brief	Inverts this object. 
	**/
	inline void Invert ()
	{
		Set (-X, -Y, -Z);
	}

	/**
	\fn	inline vec3Base GetInverted () const
	
	\brief	Gets the inverted vector.
	
	\return	The inverted vector. 
	**/
	inline vec3Base GetInverted () const
	{
		return vec3Base(-X, -Y, -Z);
	}

	/**
	\fn	inline bool IsZero () const
	
	\brief	Query if this object is zero. 
	
	\return	true if zero, false if not. 
	**/
	inline bool IsZero () const
	{
		return (X == 0 && Y == 0 && Z == 0);
	}

	/**
	\fn	inline void Set (const TType InX, const TType InY, const TType InZ)
	
	\brief	Sets the vector's values to 'InX', 'InY', and 'InZ'
	
	\author	Paril
	\date	27/05/2010
	
	\param	InX	The x coordinate. 
	\param	InY	The y coordinate. 
	\param	InZ	The z coordinate. 
	**/
	inline void Set (const TType InX, const TType InY, const TType InZ)
	{
		X = InX;
		Y = InY;
		Z = InZ;
	}

	/**
	\fn	inline void Set (const TType Number)
	
	\brief	Sets the vector's values to 'Number'
	
	\author	Paril
	\date	27/05/2010
	
	\param	Number	Number to set vectors values to. 
	**/
	inline void Set (const TType Number)
	{
		Set (Number, Number, Number);
	}

	/**
	\fn	inline void Set (const vec3Base &Vec)
	
	\brief	Sets this vector to the values of 'Vec'. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The source vector. 
	**/
	inline void Set (const vec3Base &Vec)
	{
		Set (Vec[0], Vec[1], Vec[2]);
	}

	/**
	\fn	template <typename TNewType> inline void Set (const vec3Base<TNewType> &Vec)
	
	\brief	Sets this vector to the converted values of 'Vec'.
	
	\author	Paril
	\date	13/10/2010
	
	\param	Vec	The source vector. 
	**/
	template <typename TNewType>
	inline void Set (const vec3Base<TNewType> &Vec)
	{
		Set ((TType)Vec[0], (TType)Vec[1], (TType)Vec[2]);
	}

	/**
	\fn	bool Compare(const vec3Base &Vec, const TType Epsilon) const
	
	\brief	Compares two vectors using an Epsilon, for floating point values.
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec		Constant vector 3 base & to be compared. 
	\param	Epsilon	Constant t type to be compared. 
	
	\return	True if the vectors are the same (using an epsilon), otherwise false.
	**/
	bool Compare(const vec3Base &Vec, const TType Epsilon) const
	{
		if (Abs(X - Vec[0]) > Epsilon ||
			Abs(Y - Vec[1]) > Epsilon ||
			Abs(Z - Vec[2]) > Epsilon)
			return false;
		return true;
	}

	/**
	\fn	bool Inside (const vec3Base Min, const vec3Base Max) const
	
	\brief	Queries if this vector is inside the bounds of 'Min' and 'Max'
	
	\author	Paril
	\date	27/05/2010
	
	\param	Min	The min bounds. 
	\param	Max	The max bounds. 
	
	\return	true if they are inside, false if it fails. 
	**/
	bool Inside (const vec3Base Min, const vec3Base Max) const
	{
		return (operator> (Min)) && (operator< (Max));
	}

	/**
	\fn	inline float Dot (const vec3Base &Vec) const
	
	\brief	Calculates the dot product of this vector and 'Vec'
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The second vector. 
	
	\return	The dot product. 
	**/
	inline float Dot (const vec3Base &Vec) const
	{
		return X * Vec.X + Y * Vec.Y + Z * Vec.Z;
	}

	/**
	\fn	inline vec3Base Cross (const vec3Base &Vec) const
	
	\brief	Calculates the cross product of this vector and Vec. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The second vector. 
	
	\return	The cross product. 
	**/
	inline vec3Base Cross (const vec3Base &Vec) const
	{
		return vec3Base(Y * Vec.Z - Z * Vec.Y, Z * Vec.X - X * Vec.Z, X * Vec.Y - Y * Vec.X);
	}

	/**
	\fn	inline float Dist (const vec3Base &Vec) const
	
	\brief	Gets the distance between this vector and 'Vec'.
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector to calculate the distance to. 
	
	\return	The distance between this vector and 'Vec'. 
	**/
	inline float Dist (const vec3Base &Vec) const
	{
		return sqrtf ((X - Vec.X) * (X - Vec.X) + (Y - Vec.Y) * (Y - Vec.Y) + (Z - Vec.Z) * (Z - Vec.Z));
	}

	/**
	\fn	inline float DistFast (const vec3Base &Vec) const
	
	\brief	Gets the distance (fast) between this vector and 'Vec'.
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector to calculate the distance to. 
	
	\return	The distance between this vector and 'Vec'. 
	**/
	inline float DistFast (const vec3Base &Vec) const
	{
		return Q_FastSqrt ((X - Vec.X) * (X - Vec.X) + (Y - Vec.Y) * (Y - Vec.Y) + (Z - Vec.Z) * (Z - Vec.Z));
	}

	/**
	\fn	inline float DistSq (const vec3Base &Vec) const
	
	\brief	Gets the squared distance between this vector and 'Vec'.
	
	\author	Paril
	\date	27/05/2010
	
	\param	Vec	The vector to calculate the distance to. 
	
	\return	The squared distance between this vector and 'Vec'. 
	**/
	inline float DistSq (const vec3Base &Vec) const
	{
		return (X - Vec.X) * (X - Vec.X) + (Y - Vec.Y) * (Y - Vec.Y) + (Z - Vec.Z) * (Z - Vec.Z);
	}

	/**
	\fn	inline float Length() const
	
	\brief	Gets the length of this vector.
	
	\return	The length of this vector. 
	**/
	inline float Length() const
	{
		return sqrtf (X * X + Y * Y + Z * Z);
	}
	
	/**
	\fn	inline float LengthFast() const
	
	\brief	Gets the length (fast) of this vector.
	
	\return	The length (fast) of this vector. 
	**/
	inline float LengthFast() const
	{
		return Q_FastSqrt (X * X + Y * Y + Z * Z);
	}
	
	/**
	\fn	inline float LengthSq() const
	
	\brief	Gets the squared length of this vector.
	
	\return	The squared length of this vector. 
	**/
	inline float LengthSq() const
	{
		return X * X + Y * Y + Z * Z;
	}

	/**
	\fn	inline void Absolute()
	
	\brief	Sets this vector to it's absolute value. 
	**/
	inline void Absolute()
	{
		Set (Abs(X), Abs(Y), Abs(Z));
	}
	
	/**
	\fn	inline vec3Base GetAbsolute() const
	
	\brief	Returns the absolute value of this vector.
	
	\return	The abs. 
	**/
	inline vec3Base GetAbsolute() const
	{
		return vec3Base(Abs(X), Abs(Y), Abs(Z));
	}

	/**
	\fn	bool IsNearlyZero(const float Epsilon = SMALL_NUMBER) const
	
	\brief	Query if this vector is nearly zero (using 'Epsilon).
	
	\author	Paril
	\date	27/05/2010
	
	\param	Epsilon	The epsilon. 
	
	\return	true if nearly zero, false if not. 
	**/
	bool IsNearlyZero(const float Epsilon = SMALL_NUMBER) const
	{
		return (Abs(X) <= Epsilon && Abs(Y) <= Epsilon && Abs(Z) <= Epsilon);
	}

	/**
	\fn	float Normalize()
	
	\brief	Normalizes this vector.
	
	\return	The length of this vector. 
	**/
	float Normalize()
	{
		float Len = Length();

		if (Len)
			operator*= (1.0f / Len);
		else
			Clear();

		return Len;
	}

	/**
	\fn	float NormalizeFast()
	
	\brief	Normalize this vector (fast). 
	
	\return	The length of this vector. 
	**/
	float NormalizeFast()
	{
		float Len = Q_RSqrt<float>(Dot(*this));
		operator*= (Len);
		return (Len) ? (1.0f / Len) : 0;
	}

	/**
	\fn	inline vec3Base const GetNormalized ()
	
	\brief	Gets the normalized vector. 
	
	\return	The normalized vector. 
	**/
	inline vec3Base const GetNormalized ()
	{
		vec3Base val (*this);
		val.Normalize ();
		return val;
	}

	/**
	\fn	inline vec3Base const GetNormalized (float &length)
	
	\brief	Gets the normalized vector and fills 'length' with the length
			of the vector. 
	
	\author	Paril
	\date	27/05/2010
	
	\param [in,out]	length	The length. 
	
	\return	The normalized vector. 
	**/
	inline vec3Base const GetNormalized (float &length)
	{
		vec3Base val (*this);
		length = val.Normalize ();
		return val;
	}

	/**
	\fn	inline vec3Base const GetNormalizedFast ()
	
	\brief	Gets the normalized vector (fast). 
	
	\return	The normalized vector. 
	**/
	inline vec3Base const GetNormalizedFast ()
	{
		vec3Base val (*this);
		val.NormalizeFast ();
		return val;
	}

	/**
	\fn	inline vec3Base const GetNormalizedFast (float &length)
	
	\brief	Gets a normalized vector (fast) with length storage. 
	
	\author	Paril
	\date	27/05/2010
	
	\param [in,out]	length	The length. 
	
	\return	The normalized vector. 
	**/
	inline vec3Base const GetNormalizedFast (float &length)
	{
		vec3Base val (*this);
		length = val.NormalizeFast ();
		return val;
	}

	/**
	\fn	float ToYaw () const
	
	\brief	Converts this vector to yaw. 
	
	\return	This vector expressed as yaw. 
	**/
	float ToYaw () const
	{
		float	yaw = 0;

		if (X == 0)
		{
			if (Y > 0)
				yaw = 90;
			else if (Y < 0)
				yaw = -90;
		}
		else
		{
			yaw = atan2f(Y, X) * 180 / M_PI;
			if (yaw < 0)
				yaw += 360;
		}

		return yaw;
	}

	/**
	\fn	void ToVectors (vec3Base *forward, vec3Base *right, vec3Base *up) const
	
	\brief	Converts this angle to three separate vectors. 
	
	\author	Paril
	\date	27/05/2010
	
	\return The angle in three vectors.
	**/
	angleBase<vec3Base<TType>> ToVectors () const
	{
		return angleBase<vec3Base<TType>>(*this);
	}

	/**
	\fn	inline vec3Base MultiplyAngles (const float scale, const vec3Base &b) const
	
	\brief	Projects this vector in 'b' direction by 'scale' inits. 
	
	\author	Paril
	\date	27/05/2010
	
	\param	scale	The scale. 
	\param	b		The direction. 
	
	\return	. 
	**/
	inline vec3Base MultiplyAngles (const float scale, const vec3Base &b) const
	{
		return vec3Base(	X + b.X * scale,
						Y + b.Y * scale,
						Z + b.Z * scale);
	}

	/**
	\fn	vec3Base ToAngles () const
	
	\brief	Converts this vector to angles. 
	
	\return	This vector as euler angles. 
	**/
	vec3Base ToAngles () const
	{
		float	yaw = 0, pitch = 270;
		
		if (Y == 0 && X == 0)
		{
			if (Z > 0)
				pitch = 90;
		}
		else
		{
			if (X)
				yaw = atan2f (Y, X) * (180.0f / M_PI);
			else if (Y > 0)
				yaw = 90;
			else
				yaw = 270;

			if (yaw < 0)
				yaw += 360;

			pitch = atan2f (Z, sqrtf (X * X + Y * Y)) * (180.0f / M_PI);
			if (pitch < 0)
				pitch += 360;
		}

		return vec3Base (-pitch, yaw, 0);
	}

	/**
	\fn	void MakeNormalVectors(vec3Base &right, vec3Base &up) const
	
	\brief	Makes normal vectors.
	
	\author	Paril
	\date	05/06/2010
	
	\param [in,out]	right	The right. 
	\param [in,out]	up		The up. 
	**/
	void MakeNormalVectors(vec3Base &right, vec3Base &up) const
	{
		// This rotate and negate guarantees a vector not colinear with the original
		right.Set (Z, -X, Y);

		float d = right | *this;
		right = right.MultiplyAngles(-d, *this).GetNormalized();
		up = right ^ *this;
	}

	/**
	\fn	vec3Base RotateAroundVector(const vec3Base &Dir, const float Degrees) const
	
	\brief	Rotate this vector in direction 'Dir' 'Degrees' degrees.
	
	\author	Paril
	\date	05/06/2010
	
	\param	Dir		The dir. 
	\param	Degrees	The degrees. 
	**/
	vec3Base RotateAroundVector(const vec3Base &Dir, const float Degrees) const
	{
		vec3Base dest;
		
		float c, s;
		Q_SinCosf(DEG2RAD(Degrees), &s, &c);

		vec3Base vr, vu;
		Dir.MakeNormalVectors (vr, vu);

		float t0, t1;
		t0 = vr.X * c + vu.X * -s;
		t1 = vr.X * s + vu.X *  c;
		dest.X = (t0 * vr.X + t1 * vu.X + Dir.X * Dir.X) * X
						+ (t0 * vr.Y + t1 * vu.Y + Dir.X * Dir.Y) * Y
						+ (t0 * vr.Z + t1 * vu.Z + Dir.X * Dir.Z) * Z;

		t0 = vr.Y * c + vu.Y * -s;
		t1 = vr.Y * s + vu.Y *  c;
		dest.Y = (t0 * vr.X + t1 * vu.X + Dir.Y * Dir.X) * X
						+ (t0 * vr.Y + t1 * vu.Y + Dir.Y * Dir.Y) * Y
						+ (t0 * vr.Z + t1 * vu.Z + Dir.Y * Dir.Z) * Z;

		t0 = vr.Z * c + vu.Z * -s;
		t1 = vr.Z * s + vu.Z *  c;
		dest.Z = (t0 * vr.X + t1 * vu.X + Dir.Z * Dir.X) * X
						+ (t0 * vr.Y + t1 * vu.Y + Dir.Z * Dir.Y) * Y
						+ (t0 * vr.Z + t1 * vu.Z + Dir.Z * Dir.Z) * Z;

		return dest;
	}
};

/**
\typedef	vec3Base<float> vec3f

\brief	Defines an alias representing a 3d float vector.
**/
typedef vec3Base<float> vec3f;

typedef angleBase<vec3f> anglef;

/**
\fn	inline vec3f operator* (const float &l, const vec3f &r)

\brief	Muliplication operator. 

\author	Paril
\date	27/05/2010

\param	l	The scale value. 
\param	r	The vector to be scaled. 

\return	The result of the operation. 
**/
inline vec3f operator * (const float &l, const vec3f &r)
{
	return (r * l);
};

/**
\page Utilities
\section Vectors

CleanCode includes several classes to replace
the original methods in Quake II. One of them is vectors.
The vec3f class is included as a replacement for the original
C-style method of using vectors.

vec3f is much easier to use than original vectors; operators
such as +, / and * work on them, and there are functions included
all to replace the originals.
**/