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
// cc_coonchars.h
// An enumeration for each character in conchars
//

#if !defined(CC_GUARD_CONCHARS_H) || !INCLUDE_GUARDS
#define CC_GUARD_CONCHARS_H

const int	CFLAG_SECONDARY		= 128;	// Flag if the char is green/secondary

/**
\typedef	uint8 EConChars

\brief	Defines an alias representing the ASCII console character representation in Quake 2.
**/
typedef uint8 EConChars;

/**
\enum	

\brief	Values that represent secret ASCII characters in the Q2 charset. 
**/
enum
{
	CCHAR_DOT1,
	CCHAR_CONTAINER1_UPPERLEFT,
	CCHAR_CONTAINER1_UPPERCENTER,
	CCHAR_CONTAINER1_UPPERRIGHT,
	CCHAR_CONTAINER1_MIDDLELEFT,
	CCHAR_CONTAINER1_MIDDLECENTER,
	CCHAR_CONTAINER1_MIDDLERIGHT,
	CCHAR_CONTAINER1_LOWERLEFT,
	CCHAR_CONTAINER1_LOWERCENTER,
	CCHAR_CONTAINER1_LOWERRIGHT,

	CCHAR_BLANK1,

	CCHAR_BIGSQUARE,
	
	CCHAR_BLANK2,

	CCHAR_ARROW,
	CCHAR_DOT2,
	CCHAR_DOT3,

	CCHAR_LEFT_BIGBRACKET,
	CCHAR_RIGHT_BIGBRACKET,

	CCHAR_CONTAINER2_UPPERLEFT,
	CCHAR_CONTAINER2_UPPERCENTER,
	CCHAR_CONTAINER2_UPPERRIGHT,
	CCHAR_CONTAINER2_MIDDLELEFT,
	CCHAR_CONTAINER2_MIDDLECENTER,
	CCHAR_CONTAINER2_MIDDLERIGHT,
	CCHAR_CONTAINER2_LOWERLEFT,
	CCHAR_CONTAINER2_LOWERCENTER,
	CCHAR_CONTAINER2_LOWERRIGHT,

	CCHAR_UPPERSCORE,
	CCHAR_DOT4,
	CCHAR_BAR1_LEFT,
	CCHAR_BAR1_CENTER,
	CCHAR_BAR1_RIGHT,

	CCHAR_SPACE,

	CCHAR_DOUBLEQUOTE	=	126,
	CCHAR_LEFTARROW		=	127,

	// Green/secondary stuff
	CCHAR_DOWNLOADBAR_LEFT = (CCHAR_DOT1 | CFLAG_SECONDARY),
	CCHAR_DOWNLOADBAR_CENTER,
	CCHAR_DOWNLOADBAR_RIGHT,
	CCHAR_DOWNLOADBAR_THUMB
};

#else
FILE_WARNING
#endif
