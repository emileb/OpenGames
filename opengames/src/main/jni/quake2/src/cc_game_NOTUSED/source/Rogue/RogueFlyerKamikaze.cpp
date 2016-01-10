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
// cc_rogue_flyer_kamikaze.cpp
// 
//

#include "Local.h"

#if ROGUE_FEATURES
#include "Monsters/Flyer.h"
#include "Rogue/RogueFlyerKamikaze.h"
#include "Utility/TemporaryEntities.h"

CFlyerKamikaze::CFlyerKamikaze (uint32 ID) :
CFlyer(ID)
{
};

void CFlyerKamikaze::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point)
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DIE]);
	Entity->BecomeExplosion (false);
};

void CFlyerKamikaze::Pain (IBaseEntity *Other, sint32 Damage)
{
	// We don't feel pain
};

#include "Rogue/RogueCarrier.h"

bool CFlyerKamikaze::Blocked (float Dist)
{
	KamikazeCheck();

	// if the above didn't blow us up (i.e. I got blocked by the player)
	if (Entity->GetInUse())
	{
		if (Commander && Commander->GetInUse() && 
			Commander->Monster->MonsterID == CCarrier::ID)
			Commander->Monster->MonsterSlots++;

		CRocketExplosion(CTempEntFlags (CAST_MULTI, CASTFLAG_PHS, Entity->State.GetOrigin()), Entity->State.GetOrigin().MultiplyAngles (0.02f, Entity->Velocity)).Send();
		Entity->Free ();
	}

	return true;
};

void CFlyerKamikaze::KamikazeExplode ()
{
	if (Commander && Commander->GetInUse() && 
		Commander->Monster->MonsterID == CCarrier::ID)
		Commander->Monster->MonsterSlots++;

	if (Entity->Enemy && (Entity->Enemy->EntityFlags & EF_HURTABLE))
		entity_cast<IHurtableEntity>(*Entity->Enemy)->TakeDamage (	Entity, Entity, Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin(),
																	Entity->State.GetOrigin(), vec3fOrigin, 50, 50, DAMAGE_RADIUS, MOD_UNKNOWN);

	Die (NULL, NULL, 0, vec3fOrigin);
}

void CFlyerKamikaze::KamikazeCheck ()
{
	// PMM - this needed because we could have gone away before we get here (blocked code)
	if (!Entity->GetInUse())
		return;

	if (!Entity->Enemy)
	{
		KamikazeExplode ();
		return;
	}

	Entity->GoalEntity = Entity->Enemy;
	if ((RangeFrom(Entity->State.GetOrigin(), Entity->Enemy->State.GetOrigin()) < 90))
		KamikazeExplode ();
}

CFrame FlyerFramesKamikaze [] =
{
	CFrame (&IMonster::AI_Charge, 40,	ConvertDerivedFunction(&CFlyerKamikaze::KamikazeCheck)),
	CFrame (&IMonster::AI_Charge, 40,	ConvertDerivedFunction(&CFlyerKamikaze::KamikazeCheck)),
	CFrame (&IMonster::AI_Charge, 40,	ConvertDerivedFunction(&CFlyerKamikaze::KamikazeCheck)),
	CFrame (&IMonster::AI_Charge, 40,	ConvertDerivedFunction(&CFlyerKamikaze::KamikazeCheck)),
	CFrame (&IMonster::AI_Charge, 40,	ConvertDerivedFunction(&CFlyerKamikaze::KamikazeCheck))
};
CAnim FlyerMoveKamikaze (CFlyer::FRAME_rollr02, CFlyer::FRAME_rollr06, FlyerFramesKamikaze, &IMonster::Run);

void CFlyerKamikaze::Run ()
{
	AIFlags |= AI_CHARGING;
	CurrentMove = &FlyerMoveKamikaze;
};

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_kamikaze", CFlyerKamikaze);

#endif