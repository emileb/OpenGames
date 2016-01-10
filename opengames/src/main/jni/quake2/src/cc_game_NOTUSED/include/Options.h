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
// cc_options.h
// Compiler options.
// NOTE: NO INCLUDE GUARD
//

// Define this to quickly reset macros to certain game modes.
// Can be mixed in certain cases.
#define GAME_CUSTOM							0
#define GAME_ORIGINAL_QUAKE2				1
#define GAME_ROGUE							2
#define GAME_XATRIX							4

#ifndef CC_GAME_MODE
#define CC_GAME_MODE						(GAME_ROGUE | GAME_XATRIX)
#endif

// Enable IRC
//#ifndef CLEANCODE_IRC
//#define CLEANCODE_IRC						1
//#endif

// Deprecated imports (Windows)
#ifndef USE_DEPRECATED_GAME_IMPORTS
#define USE_DEPRECATED_GAME_IMPORTS			0
#endif

// Define this if you don't want old function calls to warn you about deprecation.
// THIS IS AT YOUR OWN RISK! CLEANCODE HAS PERFECTLY FUNCTIONAL ALTERNATIVES.
#ifndef NO_DEPRECATING_OLD_FUNCTIONS
#define NO_DEPRECATING_OLD_FUNCTIONS		0
#endif

#if !defined(WIN32) || NO_DEPRECATING_OLD_FUNCTIONS
	#define CC_INSECURE_DEPRECATE(_Replacement)
#else
	#define CC_INSECURE_DEPRECATE(_Replacement) _CRT_DEPRECATE_TEXT("CleanCode has a better replacement for this function. Consider using " #_Replacement " instead.\nTo disable deprecation, use NO_DEPRECATING_OLD_FUNCTIONS in cc_options.h.")
#endif

// This is a simple macro to disable deprecation for everything inside the macro.
// This is only used internally; using this in your code could cause big problems.
#if defined(WIN32) && !NO_DEPRECATING_OLD_FUNCTIONS
	#define CC_DISABLE_DEPRECATION		__pragma(warning(push)) \
										__pragma(warning(disable:4996))

	#define CC_ENABLE_DEPRECATION		__pragma(warning(pop))
#else
	#define CC_DISABLE_DEPRECATION
	#define CC_ENABLE_DEPRECATION
#endif

// Don't touch this
#if _WIN32 && (MSVS_VERSION >= VS_7)
#define _CRT_SECURE_NO_WARNINGS
#endif

// Define this to use the CleanCode exception handler
//#ifndef CC_USE_EXCEPTION_HANDLER
//#define CC_USE_EXCEPTION_HANDLER			1
//#endif

// Define this to allow GAME_CTF
#ifndef CLEANCTF_ENABLED
#define CLEANCTF_ENABLED					1
#endif

// Define this if you want monsters to not be able to go through monstersolids
#ifndef MONSTERS_HIT_MONSTERSOLID
#define MONSTERS_HIT_MONSTERSOLID			0
#endif

// Define this to enable some custom asserts that we place in some places
#ifndef ALLOW_ASSERTS
#define ALLOW_ASSERTS						1
#endif

// Define this if you want hand grenades to drop instead of explode
// if killed while it's primed
#ifndef DROP_DEATH_GRENADES
#define DROP_DEATH_GRENADES					1
#endif

// Notes regarding CC_ASSERT_EXPR:
// Msg must only be one string; the expr is shown and
// therefore not required to be repeated in msg. Arguments
// will cause this function to fail (obviously)
// see cc_utils.cpp
#ifdef _DEBUG
#define CC_ASSERT_EXPR(expr, msg) AssertExpression(!!(expr), (msg), __FILE__, __LINE__, true)

// "minor" asserts don't cause the window to pop up unless a debugger is attached.
// A windows convenience really.
#define CC_ASSERT_EXPR_MINOR(expr, msg) AssertExpression(!!(expr), (msg), __FILE__, __LINE__, false)
#else
#define CC_ASSERT_EXPR(expr, msg) false
#define CC_ASSERT_EXPR_MINOR(expr, msg) false
#endif

// Define this if you want to use include guards.
#ifndef INCLUDE_GUARDS
#define INCLUDE_GUARDS						1
#endif

// Generally keep this VC_NONE if it's not CleanCode base
#define VC_NONE		0
#define VC_WININET	1
#define VC_CURL		2

#ifndef VERSION_CHECKING
#define VERSION_CHECKING	VC_WININET
#endif

#ifdef INCLUDE_GUARDS
	#define CC_STR2(str) #str
	#define CC_STR1(str) CC_STR2(str)
	#define CC_LOC __FILE__ "("CC_STR1(__LINE__)") : warning: "

#if defined(_WIN32)
	#define FILE_WARNING __pragma(message(CC_LOC"file included more than once!"))
#else
	#define FILE_WARNING
#endif

#else
	#define FILE_WARNING
#endif

// Monster-related preprocessor flags
#define INFANTRY_DOES_REVERSE_GUN_ATTACK	1
#define FLYER_KNOWS_HOW_TO_DODGE			2
#define SUPERTANK_USES_GRENADES				4
#define MUTANT_JUMPS_UNSTUPIDLY				8

// Change this to change some global flags
#ifndef MONSTER_SPECIFIC_FLAGS
#define MONSTER_SPECIFIC_FLAGS (INFANTRY_DOES_REVERSE_GUN_ATTACK | FLYER_KNOWS_HOW_TO_DODGE | SUPERTANK_USES_GRENADES | MUTANT_JUMPS_UNSTUPIDLY)
#endif

// Enable the example Ammo Regen tech
#ifndef AMMO_REGEN_TECH
#define AMMO_REGEN_TECH						1
#endif

// Enable Rogue entities and weapons
#ifndef ROGUE_FEATURES
#define ROGUE_FEATURES						0
#endif

// just to reduce verbosity
#if ROGUE_FEATURES
#define ROGUE_VIRTUAL virtual
#else
#define ROGUE_VIRTUAL
#endif

// Enable Xatrix entities and weapons
#ifndef XATRIX_FEATURES
#define XATRIX_FEATURES						0
#endif

// just to reduce verbosity
#if XATRIX_FEATURES
#define XATRIX_VIRTUAL virtual
#else
#define XATRIX_VIRTUAL
#endif

// Enable fix for crashes in linux when closing the server
// Disabled by default. If the server's crashing at exit or map change
// enable it and recompile.
#ifndef STDCPP_LINUX_HACK
#define STDCPP_LINUX_HACK 0
#endif

#if (CC_GAME_MODE) == GAME_ORIGINAL_QUAKE2
#undef MONSTER_SPECIFIC_FLAGS
#define MONSTER_SPECIFIC_FLAGS 0
#undef AMMO_REGEN_TECH
#define AMMO_REGEN_TECH 0
#undef MONSTERS_HIT_MONSTERSOLID
#define MONSTERS_HIT_MONSTERSOLID 1
#undef DROP_DEATH_GRENADES
#define DROP_DEATH_GRENADES 0
#undef XATRIX_FEATURES
#define XATRIX_FEATURES 0
#undef ROGUE_FEATURES
#define ROGUE_FEATURES 0
#else
#if ((CC_GAME_MODE) & GAME_ROGUE) && ((CC_GAME_MODE) & GAME_XATRIX)
#undef MONSTER_SPECIFIC_FLAGS
#define MONSTER_SPECIFIC_FLAGS 0
#undef AMMO_REGEN_TECH
#define AMMO_REGEN_TECH 0
#undef MONSTERS_HIT_MONSTERSOLID
#define MONSTERS_HIT_MONSTERSOLID 1
#undef ROGUE_FEATURES
#define ROGUE_FEATURES 1
#undef XATRIX_FEATURES
#define XATRIX_FEATURES 1
#undef DROP_DEATH_GRENADES
#define DROP_DEATH_GRENADES 0
#elif ((CC_GAME_MODE) & GAME_ROGUE)
#undef MONSTER_SPECIFIC_FLAGS
#define MONSTER_SPECIFIC_FLAGS 0
#undef AMMO_REGEN_TECH
#define AMMO_REGEN_TECH 0
#undef MONSTERS_HIT_MONSTERSOLID
#define MONSTERS_HIT_MONSTERSOLID 1
#undef ROGUE_FEATURES
#define ROGUE_FEATURES 1
#undef DROP_DEATH_GRENADES
#define DROP_DEATH_GRENADES 0
#elif ((CC_GAME_MODE) & GAME_XATRIX)
#undef MONSTER_SPECIFIC_FLAGS
#define MONSTER_SPECIFIC_FLAGS 0
#undef AMMO_REGEN_TECH
#define AMMO_REGEN_TECH 0
#undef MONSTERS_HIT_MONSTERSOLID
#define MONSTERS_HIT_MONSTERSOLID 1
#undef XATRIX_FEATURES
#define XATRIX_FEATURES 1
#undef DROP_DEATH_GRENADES
#define DROP_DEATH_GRENADES 0
#endif
#endif

