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
// cc_media.h
// Storage for constant media
//

#if !defined(CC_GUARD_MEDIA_H) || !INCLUDE_GUARDS
#define CC_GUARD_MEDIA_H

#define PUTTOGETHER(name) name##_index
#define MEDIA_INDEXING_FUNCTION(name,string,registerer) \
private:\
	MediaIndex PUTTOGETHER(name); \
public:\
	inline MediaIndex name () \
	{ \
		if (!PUTTOGETHER(name)) \
			PUTTOGETHER(name) = registerer (string); \
		return PUTTOGETHER(name); \
	}

struct SPlayerMedia
{
	MediaIndex		Death[4];
	MediaIndex		Fall[2];
	MediaIndex		Gurp[2];
	MediaIndex		Jump;
	MediaIndex		Pain[4][2];
};

struct SHudMedia
{
	MediaIndex	HealthPic;
	MediaIndex	HelpPic;
};

class CGameMedia
{
public:
	// Player media
	SPlayerMedia	Player;
	// HUD Media
	SHudMedia		Hud;

	// Gibs
	MediaIndex		Gib_SmallMeat;
	MEDIA_INDEXING_FUNCTION (Gib_SmallMetal, "models/objects/gibs/sm_metal/tris.md2", ModelIndex);
	MEDIA_INDEXING_FUNCTION (Gib_Arm, "models/objects/gibs/arm/tris.md2", ModelIndex);
	MEDIA_INDEXING_FUNCTION (Gib_Leg, "models/objects/gibs/leg/tris.md2", ModelIndex);
	MEDIA_INDEXING_FUNCTION (Gib_Gear, "models/objects/gibs/gear/tris.md2", ModelIndex);
	MediaIndex		Gib_Bone[2];
	MediaIndex		Gib_Chest;
	MediaIndex		Gib_Skull;
	MediaIndex		Gib_Head[2];

	// World stuff
	MEDIA_INDEXING_FUNCTION (FrySound, "player/fry.wav", SoundIndex);
	MEDIA_INDEXING_FUNCTION (FlySound, "misc/windfly.wav", SoundIndex);
};

extern CGameMedia GameMedia;

void InitGameMedia ();

#else
FILE_WARNING
#endif

