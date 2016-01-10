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
// cc_xatrix_soldier_laser.cpp
// 
//

#include "Local.h"

#if XATRIX_FEATURES
#include "Monsters/SoldierBase.h"
#include "Xatrix/XatrixSoldierLaser.h"

CSoldierLaser::CSoldierLaser (uint32 ID) :
CSoldierBase (ID)
{
}

extern CAnim SoldierMoveAttack4;

void CSoldierLaser::Attack ()
{
#if ROGUE_FEATURES
	DoneDodge ();
#endif

	CurrentMove = &SoldierMoveAttack4;
}

static sint32 MachinegunFlash [] = {MZ2_SOLDIER_MACHINEGUN_1, MZ2_SOLDIER_MACHINEGUN_2, MZ2_SOLDIER_MACHINEGUN_3, MZ2_SOLDIER_MACHINEGUN_4, MZ2_SOLDIER_MACHINEGUN_5, MZ2_SOLDIER_MACHINEGUN_6, MZ2_SOLDIER_MACHINEGUN_7, MZ2_SOLDIER_MACHINEGUN_8};
void CSoldierLaser::FireGun (sint32 FlashNumber)
{
	if (!HasValidEnemy())
		return;

	sint32		flashIndex = MachinegunFlash[FlashNumber];

	if (!(AIFlags & AI_HOLD_FRAME))
		Wait = Level.Frame + (3 + irandom(8));

	CMonsterBeamLaser *Laser;

	// RAFAEL
	// this sound can't be called this frequent
	if (frand() > 0.8)
		Entity->PlaySound (CHAN_AUTO, SoundIndex("misc/lasfly.wav"), 255, ATTN_STATIC);

	vec3f start = Entity->State.GetOrigin();
	vec3f end = Entity->Enemy->State.GetOrigin();
	vec3f dir = end - start;
	vec3f tempvec = MonsterFlashOffsets[FlashNumber];
	
	Laser = QNewEntityOf CMonsterBeamLaser;
	
	anglef angles = dir.ToAngles().ToVectors ();
	Laser->State.GetAngles() = dir.ToAngles();

	if (flashIndex == MZ2_SOLDIER_MACHINEGUN_3)
		start = start.MultiplyAngles (tempvec[0]-14, angles.Right)
				.MultiplyAngles (tempvec[2]-32, angles.Up)
				.MultiplyAngles (tempvec[1], angles.Forward);
	else 
		start = start.MultiplyAngles (tempvec[0]+2, angles.Right)
				.MultiplyAngles (tempvec[2]-24, angles.Up)
				.MultiplyAngles (tempvec[1], angles.Forward);
			
	Laser->State.GetOrigin() = start;
	Laser->SetOwner(Entity);
	
	Laser->Damage = 1;

	MonsterFireBeam (Laser);

	if (Level.Frame >= Wait)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CSoldierLaser::SpawnSoldier ()
{
	Sounds[SOUND_PAIN] = SoundIndex ("soldier/solpain3.wav");
	Sounds[SOUND_DEATH] = SoundIndex ("soldier/soldeth3.wav");

	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/soldierh/tris.md2");
	Entity->State.GetSkinNum() = 4;
	SoldierAI = AI_MACHINEGUN;
	Entity->Health = 70;
	Entity->GibHealth = -30;
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_soldier_lasergun", CSoldierLaser);
#endif
