// TTypes.h

// define a few basic operations and constants

// null pointer
#ifndef NULL
# define NULL ((void *)0)
#endif

#ifdef USE_CPP0X
#define null nullptr
#define var auto
#else
//#define null NULL
#endif

#ifndef BIT
# define BIT(x) (1<<(x))
#endif

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef FALSE
# define FALSE 0
#endif

#ifndef TRUE
# define TRUE 1
#endif

// ===========================================================================

typedef unsigned char byte;

#ifdef _WIN64
typedef unsigned __int64	intptr;
#elif defined(__LP32__)
typedef unsigned int		intptr;
#else
typedef unsigned long		intptr;
#endif

// Types of arrays, for simple collections of data

typedef byte	bvec2_t[2];
typedef byte	bvec3_t[3];
typedef byte	bvec4_t[4];

typedef double	dvec2_t[2];
typedef double	dvec3_t[3];
typedef double	dvec4_t[4];

typedef sint32	index_t;
typedef sint32	ivec2_t[2];
typedef sint32	ivec3_t[3];
typedef sint32	ivec4_t[4];

typedef sint16	svec2_t[2];
typedef sint16	svec3_t[3];
typedef sint16	svec4_t[4];

typedef float	vec2_t[2];
typedef float	vec3_t[3];
typedef float	vec4_t[4];

typedef float	fvec2_t[2];
typedef float	fvec3_t[3];
typedef float	fvec4_t[4]; // why not fvec >_<

typedef	float	quat_t[4];
typedef float	mat3x3_t[3][3];
typedef float	mat4x4_t[16];

// Definitions of bit sizes

#ifndef WIN32 // These get declared in Platform.h in windows.
/*
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;
*/
#endif

/*
==============================================================================

	Template type definitions

	Defines some properties that are used in some template functions,
	just to optimize things.
	
==============================================================================
*/

/*
 * An _atomic_ class is a class that can be directly manipulated by memory, and does not need a constructor.
 */

template<typename TType>
class TypeInfo_Atomic
{
public:
	typedef TType InitType;
	enum { NeedsCtor = 0 };

	template<typename TRetType>
	inline TRetType TypeHash(const TType &Var) { return static_cast<TRetType>(Var); }

	static const size_t Size = sizeof(TType);
};

template<typename TType>
class TypeInfo_Constructed
{
public:
	typedef const TType &InitType;
	enum { NeedsCtor = 1 };

	static const size_t Size = sizeof(TType);
};

// Default
template<typename TType>
class TypeInfo : public TypeInfo_Constructed<TType> { };

template<typename TType>
class TypeInfo<TType*> : public TypeInfo_Atomic<TType*> { };

template<> class TypeInfo<char> : public TypeInfo_Atomic<char> {};
template<> class TypeInfo<bool> : public TypeInfo_Atomic<bool> {};
template<> class TypeInfo<byte> : public TypeInfo_Atomic<byte> {};
template<> class TypeInfo<double> : public TypeInfo_Atomic<double> {};
template<> class TypeInfo<float> : public TypeInfo_Atomic<float> {};
template<> class TypeInfo<int> : public TypeInfo_Atomic<int> {};
template<> class TypeInfo<short> : public TypeInfo_Atomic<short> {};
template<> class TypeInfo<uint32> : public TypeInfo_Atomic<uint32> {};
template<> class TypeInfo<uint16> : public TypeInfo_Atomic<uint16> {};

//=========================================================================================
template<typename Arg1>
struct Event1
{
	typedef void (*Action) (Arg1 arg1);
};

template<typename Arg1, typename Arg2>
struct Event2
{
	typedef void (*Action) (Arg1 arg1, Arg2 arg2);
	typedef Arg2 (*Func) (Arg1 arg1);
};

template<typename Arg1, typename Arg2, typename Arg3>
struct Event3
{
	typedef void (*Action) (Arg1 arg1, Arg2 arg2, Arg3 arg3);
	typedef Arg3 (*Func) (Arg1 arg1, Arg2 arg2);
};


#define typesafe_enum(name, type, ...) \
struct name \
{ \
	type		Value; \
	 \
	enum name_enum \
	{ \
		__VA_ARGS__ \
	}; \
	 \
	inline name() { } \
	 \
	inline name(const type &value) : \
		Value(value) { } \
		 \
	inline name(const name &value) : \
		Value(value.Value) { }; \
		 \
	inline operator type () const { return Value; } \
	inline type operator | (const name_enum &value) const { return Value | value; } \
	inline type operator & (const name_enum &value) const { return Value & value; } \
	inline type operator ^ (const name_enum &value) const { return Value ^ value; } \
	inline type operator ~ () const { return ~Value; } \
	inline type operator |= (const name_enum &value) { Value |= value; return Value; } \
	inline type operator &= (const name_enum &value) { Value &= value; return Value; } \
	inline type operator ^= (const name_enum &value) { Value ^= value; return Value; } \
	inline bool operator == (const name_enum &value) const { return value == Value; } \
	inline bool operator != (const name_enum &value) const { return value != Value; } \
	inline name &operator = (const name_enum &value) { Value = value; return *this; } \
	inline name &operator ++ () { ++Value; return *this; } \
	inline name &operator -- () { --Value; return *this; } \
	inline name &operator + () { return *this; } \
	inline type operator - () const { return -Value; } \
};
/*
template<typename T>
struct IntegralType
{
private:
	T _value;

public:
	IntegralType() { }
	IntegralType(const T &value) : _value(value) { }

	inline operator T() const { return _value; }
	inline T operator | (const T &value) const { return Value | value; }
	inline T operator & (const T &value) const { return Value & Value; }
	inline T operator ^ (const T &value) const { return Value ^ value; }
	inline T operator ~ () const { return ~Value; }
	inline T operator |= (const T &value) { Value |= value; return Value; }
	inline T operator &= (const T &value) { Value &= value; return Value; }
	inline T operator ^= (const T &value) { Value ^= value; return Value; }
	inline bool operator == (const T &value) const { return value == Value; }
	inline bool operator != (const T &value) const { return value != Value; }
	inline IntegralType &operator = (const T &value) { Value = value; return *this; }
	inline IntegralType &operator ++ () { ++Value; return *this; }
	inline IntegralType &operator -- () { --Value; return *this; }
	inline IntegralType &operator + () { return *this; }
	inline T operator - () const { return -Value; }
};

typedef IntegralType<signed __int32> Int32;
*/
