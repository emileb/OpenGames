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
// cc_colors.cpp
// A container with enumerations for colors and classes
//

#include "Local.h"
#include "Utility/Colors.h"

using namespace NSColor;

// "palette to rgb" list
ColorPalette PalToRGB =
{
	colorb(0,		0,		0),
	colorb(15,		15,		15),
	colorb(31,		31,		31),
	colorb(47,		47,		47),
	colorb(63,		63,		63),
	colorb(75,		75,		75),
	colorb(91,		91,		91),
	colorb(107,		107,	107),
	colorb(123,		123,	123),
	colorb(139,		139,	139),
	colorb(155,		155,	155),
	colorb(171,		171,	171),
	colorb(187,		187,	187),
	colorb(203,		203,	203),
	colorb(219,		219,	219),
	colorb(235,		235,	235),
	colorb(99,		75,		35),
	colorb(91,		67,		31),
	colorb(83,		63,		31),
	colorb(79,		59,		27),
	colorb(71,		55,		27),
	colorb(63,		47,		23),
	colorb(59,		43,		23),
	colorb(51,		39,		19),
	colorb(47,		35,		19),
	colorb(43,		31,		19),
	colorb(39,		27,		15),
	colorb(35,		23,		15),
	colorb(27,		19,		11),
	colorb(23,		15,		11),
	colorb(19,		15,		7),
	colorb(15,		11,		7),
	colorb(95,		95,		111),
	colorb(91,		91,		103),
	colorb(91,		83,		95),
	colorb(87,		79,		91),
	colorb(83,		75,		83),
	colorb(79,		71,		75),
	colorb(71,		63,		67),
	colorb(63,		59,		59),
	colorb(59,		55,		55),
	colorb(51,		47,		47),
	colorb(47,		43,		43),
	colorb(39,		39,		39),
	colorb(35,		35,		35),
	colorb(27,		27,		27),
	colorb(23,		23,		23),
	colorb(19,		19,		19),
	colorb(143,		119,	83),
	colorb(123,		99,		67),
	colorb(115,		91,		59),
	colorb(103,		79,		47),
	colorb(207,		151,	75),
	colorb(167,		123,	59),
	colorb(139,		103,	47),
	colorb(111,		83,		39),
	colorb(235,		159,	39),
	colorb(203,		139,	35),
	colorb(175,		119,	31),
	colorb(147,		99,		27),
	colorb(119,		79,		23),
	colorb(91,		59,		15),
	colorb(63,		39,		11),
	colorb(35,		23,		7),
	colorb(167,		59,		43),
	colorb(159,		47,		35),
	colorb(151,		43,		27),
	colorb(139,		39,		19),
	colorb(127,		31,		15),
	colorb(115,		23,		11),
	colorb(103,		23,		7),
	colorb(87,		19,		0),
	colorb(75,		15,		0),
	colorb(67,		15,		0),
	colorb(59,		15,		0),
	colorb(51,		11,		0),
	colorb(43,		11,		0),
	colorb(35,		11,		0),
	colorb(27,		7,		0),
	colorb(19,		7,		0),
	colorb(123,		95,		75),
	colorb(115,		87,		67),
	colorb(107,		83,		63),
	colorb(103,		79,		59),
	colorb(95,		71,		55),
	colorb(87,		67,		51),
	colorb(83,		63,		47),
	colorb(75,		55,		43),
	colorb(67,		51,		39),
	colorb(63,		47,		35),
	colorb(55,		39,		27),
	colorb(47,		35,		23),
	colorb(39,		27,		19),
	colorb(31,		23,		15),
	colorb(23,		15,		11),
	colorb(15,		11,		7),
	colorb(111,		59,		23),
	colorb(95,		55,		23),
	colorb(83,		47,		23),
	colorb(67,		43,		23),
	colorb(55,		35,		19),
	colorb(39,		27,		15),
	colorb(27,		19,		11),
	colorb(15,		11,		7),
	colorb(179,		91,		79),
	colorb(191,		123,	111),
	colorb(203,		155,	147),
	colorb(215,		187,	183),
	colorb(203,		215,	223),
	colorb(179,		199,	211),
	colorb(159,		183,	195),
	colorb(135,		167,	183),
	colorb(115,		151,	167),
	colorb(91,		135,	155),
	colorb(71,		119,	139),
	colorb(47,		103,	127),
	colorb(23,		83,		111),
	colorb(19,		75,		103),
	colorb(15,		67,		91),
	colorb(11,		63,		83),
	colorb(7,		55,		75),
	colorb(7,		47,		63),
	colorb(7,		39,		51),
	colorb(0,		31,		43),
	colorb(0,		23,		31),
	colorb(0,		15,		19),
	colorb(0,		7,		11),
	colorb(0,		0,		0),
	colorb(139,		87,		87),
	colorb(131,		79,		79),
	colorb(123,		71,		71),
	colorb(115,		67,		67),
	colorb(107,		59,		59),
	colorb(99,		51,		51),
	colorb(91,		47,		47),
	colorb(87,		43,		43),
	colorb(75,		35,		35),
	colorb(63,		31,		31),
	colorb(51,		27,		27),
	colorb(43,		19,		19),
	colorb(31,		15,		15),
	colorb(19,		11,		11),
	colorb(11,		7,		7),
	colorb(0,		0,		0),
	colorb(151,		159,	123),
	colorb(143,		151,	115),
	colorb(135,		139,	107),
	colorb(127,		131,	99),
	colorb(119,		123,	95),
	colorb(115,		115,	87),
	colorb(107,		107,	79),
	colorb(99,		99,		71),
	colorb(91,		91,		67),
	colorb(79,		79,		59),
	colorb(67,		67,		51),
	colorb(55,		55,		43),
	colorb(47,		47,		35),
	colorb(35,		35,		27),
	colorb(23,		23,		19),
	colorb(15,		15,		11),
	colorb(159,		75,		63),
	colorb(147,		67,		55),
	colorb(139,		59,		47),
	colorb(127,		55,		39),
	colorb(119,		47,		35),
	colorb(107,		43,		27),
	colorb(99,		35,		23),
	colorb(87,		31,		19),
	colorb(79,		27,		15),
	colorb(67,		23,		11),
	colorb(55,		19,		11),
	colorb(43,		15,		7),
	colorb(31,		11,		7),
	colorb(23,		7,		0),
	colorb(11,		0,		0),
	colorb(0,		0,		0),
	colorb(119,		123,	207),
	colorb(111,		115,	195),
	colorb(103,		107,	183),
	colorb(99,		99,		167),
	colorb(91,		91,		155),
	colorb(83,		87,		143),
	colorb(75,		79,		127),
	colorb(71,		71,		115),
	colorb(63,		63,		103),
	colorb(55,		55,		87),
	colorb(47,		47,		75),
	colorb(39,		39,		63),
	colorb(35,		31,		47),
	colorb(27,		23,		35),
	colorb(19,		15,		23),
	colorb(11,		7,		7),
	colorb(155,		171,	123),
	colorb(143,		159,	111),
	colorb(135,		151,	99),
	colorb(123,		139,	87),
	colorb(115,		131,	75),
	colorb(103,		119,	67),
	colorb(95,		111,	59),
	colorb(87,		103,	51),
	colorb(75,		91,		39),
	colorb(63,		79,		27),
	colorb(55,		67,		19),
	colorb(47,		59,		11),
	colorb(35,		47,		7),
	colorb(27,		35,		0),
	colorb(19,		23,		0),
	colorb(11,		15,		0),
	colorb(0,		255,	0),
	colorb(35,		231,	15),
	colorb(63,		211,	27),
	colorb(83,		187,	39),
	colorb(95,		167,	47),
	colorb(95,		143,	51),
	colorb(95,		123,	51),
	colorb(255,		255,	255),
	colorb(255,		255,	211),
	colorb(255,		255,	167),
	colorb(255,		255,	127),
	colorb(255,		255,	83),
	colorb(255,		255,	39),
	colorb(255,		235,	31),
	colorb(255,		215,	23),
	colorb(255,		191,	15),
	colorb(255,		171,	7),
	colorb(255,		147,	0),
	colorb(239,		127,	0),
	colorb(227,		107,	0),
	colorb(211,		87,		0),
	colorb(199,		71,		0),
	colorb(183,		59,		0),
	colorb(171,		43,		0),
	colorb(155,		31,		0),
	colorb(143,		23,		0),
	colorb(127,		15,		0),
	colorb(115,		7,		0),
	colorb(95,		0,		0),
	colorb(71,		0,		0),
	colorb(47,		0,		0),
	colorb(27,		0,		0),
	colorb(239,		0,		0),
	colorb(55,		55,		255),
	colorb(255,		0,		0),
	colorb(0,		0,		255),
	colorb(43,		43,		35),
	colorb(27,		27,		23),
	colorb(19,		19,		15),
	colorb(235,		151,	127),
	colorb(195,		115,	83),
	colorb(159,		87,		51),
	colorb(123,		63,		27),
	colorb(235,		211,	199),
	colorb(199,		171,	155),
	colorb(167,		139,	119),
	colorb(135,		107,	87),
	colorb(159,		91,		83)
};

/**
\fn	EColors IndexFromRGB (colorb color)

\brief	Returns an EColors index from the RGB color 'color'

\author	Paril
\date	25/05/2010

\param	color	The color, in RGB (A is ignored). 

\return	Color index. 
**/
EColors IndexFromRGB (colorb color)
{
	const float maxDist = 255 * 255 * 255;
	float curDist = maxDist;
	EColors colorIndex = 0;

	for (uint16 i = 0; i < 256; i++)
	{
		float dist = color.DistanceFrom (PalToRGB[i]);

		if (dist < curDist)
		{
			curDist = dist;
			colorIndex = i;
		}
	}

	return colorIndex;
};
