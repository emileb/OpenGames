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
// cc_jorg.h
// 
//

#if !defined(CC_GUARD_JORG_H) || !INCLUDE_GUARDS
#define CC_GUARD_JORG_H

class CJorg : public IMonster
{
public:
	enum
	{
		FRAME_attak101,
		FRAME_attak102,
		FRAME_attak103,
		FRAME_attak104,
		FRAME_attak105,
		FRAME_attak106,
		FRAME_attak107,
		FRAME_attak108,
		FRAME_attak109,
		FRAME_attak110,
		FRAME_attak111,
		FRAME_attak112,
		FRAME_attak113,
		FRAME_attak114,
		FRAME_attak115,
		FRAME_attak116,
		FRAME_attak117,
		FRAME_attak118,
		FRAME_attak201,
		FRAME_attak202,
		FRAME_attak203,
		FRAME_attak204,
		FRAME_attak205,
		FRAME_attak206,
		FRAME_attak207,
		FRAME_attak208,
		FRAME_attak209,
		FRAME_attak210,
		FRAME_attak211,
		FRAME_attak212,
		FRAME_attak213,
		FRAME_death01,
		FRAME_death02,
		FRAME_death03,
		FRAME_death04,
		FRAME_death05,
		FRAME_death06,
		FRAME_death07,
		FRAME_death08,
		FRAME_death09,
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
		FRAME_death33,
		FRAME_death24,
		FRAME_death25,
		FRAME_death26,
		FRAME_death27,
		FRAME_death28,
		FRAME_death29,
		FRAME_death30,
		FRAME_death31,
		FRAME_death32,
		FRAME_death33_,
		FRAME_death34,
		FRAME_death35,
		FRAME_death36,
		FRAME_death37,
		FRAME_death38,
		FRAME_death39,
		FRAME_death40,
		FRAME_death41,
		FRAME_death42,
		FRAME_death43,
		FRAME_death44,
		FRAME_death45,
		FRAME_death46,
		FRAME_death47,
		FRAME_death48,
		FRAME_death49,
		FRAME_death50,
		FRAME_pain101,
		FRAME_pain102,
		FRAME_pain103,
		FRAME_pain201,
		FRAME_pain202,
		FRAME_pain203,
		FRAME_pain301,
		FRAME_pain302,
		FRAME_pain303,
		FRAME_pain304,
		FRAME_pain305,
		FRAME_pain306,
		FRAME_pain307,
		FRAME_pain308,
		FRAME_pain309,
		FRAME_pain310,
		FRAME_pain311,
		FRAME_pain312,
		FRAME_pain313,
		FRAME_pain314,
		FRAME_pain315,
		FRAME_pain316,
		FRAME_pain317,
		FRAME_pain318,
		FRAME_pain319,
		FRAME_pain320,
		FRAME_pain321,
		FRAME_pain322,
		FRAME_pain323,
		FRAME_pain324,
		FRAME_pain325,
		FRAME_stand01,
		FRAME_stand02,
		FRAME_stand03,
		FRAME_stand04,
		FRAME_stand05,
		FRAME_stand06,
		FRAME_stand07,
		FRAME_stand08,
		FRAME_stand09,
		FRAME_stand10,
		FRAME_stand11,
		FRAME_stand12,
		FRAME_stand13,
		FRAME_stand14,
		FRAME_stand15,
		FRAME_stand16,
		FRAME_stand17,
		FRAME_stand18,
		FRAME_stand19,
		FRAME_stand20,
		FRAME_stand21,
		FRAME_stand22,
		FRAME_stand23,
		FRAME_stand24,
		FRAME_stand25,
		FRAME_stand26,
		FRAME_stand27,
		FRAME_stand28,
		FRAME_stand29,
		FRAME_stand30,
		FRAME_stand31,
		FRAME_stand32,
		FRAME_stand33,
		FRAME_stand34,
		FRAME_stand35,
		FRAME_stand36,
		FRAME_stand37,
		FRAME_stand38,
		FRAME_stand39,
		FRAME_stand40,
		FRAME_stand41,
		FRAME_stand42,
		FRAME_stand43,
		FRAME_stand44,
		FRAME_stand45,
		FRAME_stand46,
		FRAME_stand47,
		FRAME_stand48,
		FRAME_stand49,
		FRAME_stand50,
		FRAME_stand51,
		FRAME_walk01,
		FRAME_walk02,
		FRAME_walk03,
		FRAME_walk04,
		FRAME_walk05,
		FRAME_walk06,
		FRAME_walk07,
		FRAME_walk08,
		FRAME_walk09,
		FRAME_walk10,
		FRAME_walk11,
		FRAME_walk12,
		FRAME_walk13,
		FRAME_walk14,
		FRAME_walk15,
		FRAME_walk16,
		FRAME_walk17,
		FRAME_walk18,
		FRAME_walk19,
		FRAME_walk20,
		FRAME_walk21,
		FRAME_walk22,
		FRAME_walk23,
		FRAME_walk24,
		FRAME_walk25,
	};

	inline const char	*GetMonsterName() { return "JORG"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_PAIN3,
		SOUND_IDLE,
		SOUND_DEATH,
		SOUND_SEARCH1,
		SOUND_SEARCH2,
		SOUND_SEARCH3,
		SOUND_ATTACK1,
		SOUND_ATTACK2,
		SOUND_FIREGUN,
		SOUND_STEPLEFT,
		SOUND_STEPRIGHT,
		SOUND_DEATH_HIT,

		SOUND_MAX
	);

	CJorg (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void Run ();
	void Search ();
	void Stand ();
	void Walk ();
	void Attack();
	bool CheckAttack ();
	void Idle(); // Overloaded, but not an actual Idle..

	void StepLeft ();
	void StepRight ();
	void DeathHit ();
	void Explode ();
	void TossMakron ();
	void FireBFG ();
	void DoChainguns ();
	void ReAttack1 ();
	void FireBullet ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
