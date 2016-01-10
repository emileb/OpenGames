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
// cc_gladiator.h
// WACHOOEYCHOY
//

#if !defined(CC_GUARD_GLADIATOR_H) || !INCLUDE_GUARDS
#define CC_GUARD_GLADIATOR_H

class CGladiator : public IMonster
{
public:
	enum
	{
		FRAME_stand1,
		FRAME_stand2,
		FRAME_stand3,
		FRAME_stand4,
		FRAME_stand5,
		FRAME_stand6,
		FRAME_stand7,
		FRAME_walk1,
		FRAME_walk2,
		FRAME_walk3,
		FRAME_walk4,
		FRAME_walk5,
		FRAME_walk6,
		FRAME_walk7,
		FRAME_walk8,
		FRAME_walk9,
		FRAME_walk10,
		FRAME_walk11,
		FRAME_walk12,
		FRAME_walk13,
		FRAME_walk14,
		FRAME_walk15,
		FRAME_walk16,
		FRAME_run1,
		FRAME_run2,
		FRAME_run3,
		FRAME_run4,
		FRAME_run5,
		FRAME_run6,
		FRAME_melee1,
		FRAME_melee2,
		FRAME_melee3,
		FRAME_melee4,
		FRAME_melee5,
		FRAME_melee6,
		FRAME_melee7,
		FRAME_melee8,
		FRAME_melee9,
		FRAME_melee10,
		FRAME_melee11,
		FRAME_melee12,
		FRAME_melee13,
		FRAME_melee14,
		FRAME_melee15,
		FRAME_melee16,
		FRAME_melee17,
		FRAME_attack1,
		FRAME_attack2,
		FRAME_attack3,
		FRAME_attack4,
		FRAME_attack5,
		FRAME_attack6,
		FRAME_attack7,
		FRAME_attack8,
		FRAME_attack9,
		FRAME_pain1,
		FRAME_pain2,
		FRAME_pain3,
		FRAME_pain4,
		FRAME_pain5,
		FRAME_pain6,
		FRAME_death1,
		FRAME_death2,
		FRAME_death3,
		FRAME_death4,
		FRAME_death5,
		FRAME_death6,
		FRAME_death7,
		FRAME_death8,
		FRAME_death9,
		FRAME_death10,
		FRAME_death11,
		FRAME_death12,
		FRAME_death13,
		FRAME_death14,
		FRAME_death15,
		FRAME_death16,
		FRAME_death17,
		FRAME_death18,
		FRAME_death19,
		FRAME_death20,
		FRAME_death21,
		FRAME_death22,
		FRAME_painup1,
		FRAME_painup2,
		FRAME_painup3,
		FRAME_painup4,
		FRAME_painup5,
		FRAME_painup6,
		FRAME_painup7,
	};

	vec3f		SavedFirePosition;

	virtual inline const char	*GetMonsterName() { return "Gladiator"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_DIE,
		SOUND_GUN,
		SOUND_CLEAVER_SWING,
		SOUND_CLEAVER_HIT,
		SOUND_CLEAVER_MISS,
		SOUND_IDLE,
		SOUND_SEARCH,
		SOUND_SIGHT,

		SOUND_MAX
	);

	CGladiator (uint32 ID);

	void SaveMonsterFields (CFile &File)
	{
		SAVE_MONSTER_SOUNDS
		File.Write<vec3f> (SavedFirePosition);
	}
	void LoadMonsterFields (CFile &File)
	{
		LOAD_MONSTER_SOUNDS
		SavedFirePosition = File.Read<vec3f> ();
	}

#if XATRIX_FEATURES
	virtual
#endif
	void Attack ();
	void Idle ();
	void Run ();
	void Sight ();
	void Stand ();
	void Walk ();
	void Melee ();
	void Search ();

	void FireRail ();
	void StorePosition ();
	void SwingCleaver ();
	void MeleeAttack ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

#if XATRIX_FEATURES
	virtual
#endif
	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
