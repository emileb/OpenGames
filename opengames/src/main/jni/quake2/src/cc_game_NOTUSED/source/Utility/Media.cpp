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
// cc_media.cpp
// Storage for constant media
//

#include "Local.h"

CGameMedia GameMedia;

void InitItemMedia ();

void InitGameMedia ()
{
	Mem_Zero (&GameMedia, sizeof(GameMedia));

	for (sint32 i = 0; i < 4; i++)
		GameMedia.Player.Death[i] 	= SoundIndexf("*death%i.wav", i+1);

	GameMedia.Player.Fall[0] 		= SoundIndex("*fall1.wav");
	GameMedia.Player.Fall[1] 		= SoundIndex("*fall2.wav");
	
	GameMedia.Player.Gurp[0] 		= SoundIndex("*gurp1.wav");
	GameMedia.Player.Gurp[1] 		= SoundIndex("*gurp2.wav");

	GameMedia.Player.Jump 			= SoundIndex("*jump1.wav");

	for (sint32 i = 25, l = 0; i <= 100; i += 25, l++)
	{
		GameMedia.Player.Pain[l][0] = SoundIndexf("*pain%i_1.wav", i);
		GameMedia.Player.Pain[l][1] = SoundIndexf("*pain%i_2.wav", i);
	}

	GameMedia.Gib_SmallMeat			=	ModelIndex ("models/objects/gibs/sm_meat/tris.md2");
	GameMedia.Gib_Bone[0]			=	ModelIndex ("models/objects/gibs/bone/tris.md2");
	GameMedia.Gib_Bone[1]			=	ModelIndex ("models/objects/gibs/bone2/tris.md2");
	GameMedia.Gib_Chest				=	ModelIndex ("models/objects/gibs/chest/tris.md2");
	GameMedia.Gib_Skull				=	ModelIndex ("models/objects/gibs/skull/tris.md2");
	GameMedia.Gib_Head[0]			=	ModelIndex ("models/objects/gibs/head/tris.md2");
	GameMedia.Gib_Head[1]			=	ModelIndex ("models/objects/gibs/head2/tris.md2");

	// Hud stuff
	GameMedia.Hud.HealthPic			=	ImageIndex ("i_health");
	GameMedia.Hud.HelpPic			=	ImageIndex ("i_help");

	InitItemMedia ();
}
