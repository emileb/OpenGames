// Templates.h

#include "PTypes.h"
#include "PTList.h"	

/*
==============================================================================

	Template functions
 
==============================================================================
*/

/*
 * List of functions that are available:
 * 
 * 
 * 
 */

template<typename TType>
inline TType Align(const TType Number, const TType Alignment)
{
	return (TType)(((intptr)Number + Alignment-1) & ~(Alignment-1));
}

template<typename TType>
inline TType Min(const TType A, const TType B)
{
	return (A<=B) ? A : B;
}
template<>
inline float Min(const float A, const float B)
{
	return (A<=B) ? A : B;
}
template<>
inline int Min(const int A, const int B)
{
	return (A<=B) ? A : B;
}

template<typename TType>
inline TType Max(const TType A, const TType B)
{
	return (A>=B) ? A : B;
}
template<>
inline float Max(const float A, const float B)
{
	return (A>=B) ? A : B;
}
template<>
inline int Max(const int A, const int B)
{
	return (A>=B) ? A : B;
}

template<typename TType>
inline TType Clamp(const TType V, const TType L, const TType H)
{
	return (V<L) ? L : (V>H) ? H : V;
}
template<>
inline float Clamp(const float V, const float L, const float H)
{
	return (V<L) ? L : (V>H) ? H : V;
}
template<>
inline int Clamp(const int V, const int L, const int H)
{
	return (V<L) ? L : (V>H) ? H : V;
}

template<typename TType>
inline bool IsPowOfTwo(const TType Value)
{
	return (bool)(Value > 0 && (Value & (Value-1)) == 0);
}
template<>
inline bool IsPowOfTwo(const int Value)
{
	return (bool)(Value > 0 && (Value & (Value-1)) == 0);
}
template<>
inline bool IsPowOfTwo(const uint32 Value)
{
	return (bool)(Value > 0 && (Value & (Value-1)) == 0);
}

#ifndef ArrayCount
# define ArrayCount(a) (sizeof(a)/sizeof((a)[0]))
#endif



template<typename T>
struct HashProvider
{
	int CalculateHash(const T &value)
	{
		return 0;
	}
};

template<typename T>
struct EqualityProvider
{
	bool Equals(const T &left, const T &right)
	{
		return left == right;
	}
};

template<typename T>
struct ComparingProvider
{
	int Compare(const T &left, const T &right)
	{
		return (left == right) ? 0 : (left < right) ? -1 : 1;
	}
};

template<typename T>
struct TypeSwitch
{
private:
	bool _fallThrough;
	const T &_check;
	const int _checkHash;
	typedef HashProvider<T> THashFunctor;
	THashFunctor _hashProvider;
	typedef EqualityProvider<T> TEqualityFunctor;
	TEqualityFunctor _equalityProvider;
	bool _broken, _caseMet;

public:
	const T &Value() const { return _check; }

	TypeSwitch (const T &check) :
		_check(check),
		_fallThrough(false),
		_broken(false),
		_caseMet(false),
		_checkHash(_hashProvider.CalculateHash(_check))
	{
	}
		
	template <typename TFunc>
	TypeSwitch &Case(const T &value, TFunc callback)
	{
		if (_broken)
			return *this;

		if (_fallThrough)
			callback();
		else
		{
			if (_checkHash == _hashProvider.CalculateHash(value) &&
				_equalityProvider.Equals(_check, value))
			{
				callback();
				_fallThrough = true;
				_caseMet = true;
			}
		}

		return *this;
	}

	TypeSwitch &Case(const T &value)
	{
		if (_broken)
			return *this;

		if (!_fallThrough)
		{
			if (_checkHash == _hashProvider.CalculateHash(value) &&
				_equalityProvider.Equals(_check, value))
			{
				_fallThrough = true;
				_caseMet = true;
			}
		}

		return *this;
	}

	TypeSwitch &Break()
	{
		if (_broken)
			return *this;

		_fallThrough = false;
		if (_caseMet)
			_broken = true;
		return *this;
	}
};


template<typename T>
struct MultiDimensionalArray
{
private:
	T *_array;
	int _rows, _columns, _count;

public:
	MultiDimensionalArray() :
		_array(null),
		_rows(0),
		_columns(0),
		_count(0)
	{
	}

	MultiDimensionalArray(int rows, int columns)
	{
		Resize(rows, columns);
	}

	void Resize(int rows, int columns)
	{
		if (_array != null)
			delete _array;

		_rows = rows;
		_columns = columns;
		_count = rows * columns;
		_array = new T[_count];
	}

	~MultiDimensionalArray()
	{
		delete _array;
	}

	T &ElementAt(int row, int column)
	{
		return _array[(column * _rows) + row];
	}

	/*T &operator[](int index)
	{
		return _array[index];
	}*/

	const T *Array() const
	{
		return _array;
	}
};

template<typename T1, typename T2>
struct TTuple
{
	T1 Value1;
	T2 Value2;

	TTuple() {}
	
	TTuple(const T1 &value1, const T2 &value2) :
		Value1(value1),
		Value2(value2)
	{
	};
};

template<typename T1, typename T2>
inline TTuple<T1, T2> MakeTuple(const T1 &value1, const T2 &value2)
{
	return TTuple<T1, T2>(value1, value2);
}
