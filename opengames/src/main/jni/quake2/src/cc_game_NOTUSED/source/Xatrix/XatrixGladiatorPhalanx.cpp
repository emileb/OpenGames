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
// cc_xatrix_gladiator_phalanx.cpp
// 
//

#include "Local.h"

#if XATRIX_FEATURES
#include "Monsters/Gladiator.h"
#include "Xatrix/XatrixGladiatorPhalanx.h"
#include "Weapons/WeaponMain.h"
#include "Xatrix/XatrixPhalanx.h"

CPhalanxGladiator::CPhalanxGladiator (uint32 ID) :
CGladiator (ID)
{
};

void CPhalanxGladiator::FirePhalanx ()
{
	if (Entity->State.GetFrame() == FRAME_attack9 && CvarList[CV_SKILL].Integer() != 3)
		return;

	vec3f	start;

	anglef angles = Entity->State.GetAngles().ToVectors ();
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_GLADIATOR_RAILGUN_1], angles, start);

	// calc direction to where we targeted
	CPhalanxPlasma::Spawn (Entity, start, (SavedFirePosition - start).GetNormalized(), 100, 725, 60, 60);
};

CFrame GladiatorPhalanxFramesAttackGun [] =
{
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CPhalanxGladiator::FirePhalanx)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CPhalanxGladiator::FirePhalanx)),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0),
	CFrame (&IMonster::AI_Charge, 0, ConvertDerivedFunction(&CPhalanxGladiator::FirePhalanx))
};
CAnim GladiatorPhalanxMoveAttackGun (CGladiator::FRAME_attack1, CGladiator::FRAME_attack9, GladiatorPhalanxFramesAttackGun, &IMonster::Run);

void CPhalanxGladiator::Attack ()
{
	// a small safe zone
	if ((Entity->State.GetOrigin() - Entity->Enemy->State.GetOrigin()).Length() <= (MELEE_DISTANCE + 32))
		return;

	// charge up the railgun
	StorePosition ();
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_GUN]);
	SavedFirePosition = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight);
	CurrentMove = &GladiatorPhalanxMoveAttackGun;
}

/*QUAKED monster_gladb (1 .5 0) (-32 -32 -24) (32 32 64) Ambush Trigger_Spawn Sight
*/
void CPhalanxGladiator::Spawn ()
{
	CGladiator::Spawn ();

	Sounds[SOUND_GUN] = SoundIndex ("weapons/plasshot.wav");
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/gladb/tris.md2");

	Entity->Health = 800;
	Entity->Mass = 350;

	PowerArmorType = POWER_ARMOR_SHIELD;
	PowerArmorPower = 400;
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_gladb", CPhalanxGladiator);
#endif
