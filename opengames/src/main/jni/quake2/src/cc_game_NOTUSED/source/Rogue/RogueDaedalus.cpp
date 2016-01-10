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
// cc_rogue_deadalus.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES
#include "Monsters/Icarus.h"
#include "Rogue/RogueDaedalus.h"

CDaedalus::CDaedalus (uint32 ID) :
CIcarus (ID)
{
}

void CDaedalus::FireBlaster ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start, end, dir;

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_HOVER_BLASTER_1], angles, start);

	end = Entity->Enemy->State.GetOrigin();
	end.Z += Entity->Enemy->ViewHeight;
	dir = end - start;

	MonsterFireBlaster2 (start, dir, 1, 1000, MZ2_DAEDALUS_BLASTER, FX_BLASTER);
}

/*QUAKED monster_daedalus (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
extern CAnim HoverMoveStand;
void CDaedalus::Spawn ()
{
	Sounds[SOUND_PAIN1] = SoundIndex ("daedalus/daedpain1.wav");	
	Sounds[SOUND_PAIN2] = SoundIndex ("daedalus/daedpain2.wav");	
	Sounds[SOUND_DEATH1] = SoundIndex ("daedalus/daeddeth1.wav");	
	Sounds[SOUND_DEATH2] = SoundIndex ("daedalus/daeddeth2.wav");	
	Sounds[SOUND_SIGHT] = SoundIndex ("daedalus/daedsght1.wav");	
	Sounds[SOUND_SEARCH1] = SoundIndex ("daedalus/daedsrch1.wav");	
	Sounds[SOUND_SEARCH2] = SoundIndex ("daedalus/daedsrch2.wav");	

	SoundIndex ("tank/tnkatck3.wav");	

	Entity->State.GetSound() = SoundIndex ("daedalus/daedidle1.wav");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex("models/monsters/hover/tris.md2");
	Entity->GetMins().Set (-24, -24, -24);
	Entity->GetMaxs().Set (24, 24, 32);
	Entity->State.GetSkinNum() = 2;

	YawSpeed = 25;
	PowerArmorType = POWER_ARMOR_SCREEN;
	PowerArmorPower = 100;
	Entity->Health = 450;
	Entity->GibHealth = -100;
	Entity->Mass = 150;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_SIGHT | MF_HAS_SEARCH);

	Entity->Link ();

	CurrentMove = &HoverMoveStand;	

	FlyMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_daedalus", CDaedalus);

#endif