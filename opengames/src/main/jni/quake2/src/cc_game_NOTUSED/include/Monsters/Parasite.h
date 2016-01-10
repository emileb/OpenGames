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
// cc_parasite.h
// Parasite
//

#if !defined(CC_GUARD_PARASITE_H) || !INCLUDE_GUARDS
#define CC_GUARD_PARASITE_H

class CParasite : public IMonster
{
public:
	enum
	{
		FRAME_break01,
		FRAME_break02,
		FRAME_break03,
		FRAME_break04,
		FRAME_break05,
		FRAME_break06,
		FRAME_break07,
		FRAME_break08,
		FRAME_break09,
		FRAME_break10,
		FRAME_break11,
		FRAME_break12,
		FRAME_break13,
		FRAME_break14,
		FRAME_break15,
		FRAME_break16,
		FRAME_break17,
		FRAME_break18,
		FRAME_break19,
		FRAME_break20,
		FRAME_break21,
		FRAME_break22,
		FRAME_break23,
		FRAME_break24,
		FRAME_break25,
		FRAME_break26,
		FRAME_break27,
		FRAME_break28,
		FRAME_break29,
		FRAME_break30,
		FRAME_break31,
		FRAME_break32,
		FRAME_death101,
		FRAME_death102,
		FRAME_death103,
		FRAME_death104,
		FRAME_death105,
		FRAME_death106,
		FRAME_death107,
		FRAME_drain01,
		FRAME_drain02,
		FRAME_drain03,
		FRAME_drain04,
		FRAME_drain05,
		FRAME_drain06,
		FRAME_drain07,
		FRAME_drain08,
		FRAME_drain09,
		FRAME_drain10,
		FRAME_drain11,
		FRAME_drain12,
		FRAME_drain13,
		FRAME_drain14,
		FRAME_drain15,
		FRAME_drain16,
		FRAME_drain17,
		FRAME_drain18,
		FRAME_pain101,
		FRAME_pain102,
		FRAME_pain103,
		FRAME_pain104,
		FRAME_pain105,
		FRAME_pain106,
		FRAME_pain107,
		FRAME_pain108,
		FRAME_pain109,
		FRAME_pain110,
		FRAME_pain111,
		FRAME_run01,
		FRAME_run02,
		FRAME_run03,
		FRAME_run04,
		FRAME_run05,
		FRAME_run06,
		FRAME_run07,
		FRAME_run08,
		FRAME_run09,
		FRAME_run10,
		FRAME_run11,
		FRAME_run12,
		FRAME_run13,
		FRAME_run14,
		FRAME_run15,
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
	};

	inline const char	*GetMonsterName() { return "Parasite"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_DIE,
		SOUND_LAUNCH,
		SOUND_IMPACT,
		SOUND_SUCK,
		SOUND_REELIN,
		SOUND_SIGHT,
		SOUND_TAP,
		SOUND_SCRATCH,
		SOUND_SEARCH,

		SOUND_MAX
	);

	CParasite (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void Run ();
	void Walk ();
	void Sight ();
	void Stand ();
	void Idle ();
	void Attack ();
	bool CheckAttack ();

	void Launch ();
	void ReelIn ();
	void Tap ();
	void Scratch ();
	void DoFidget();
	void ReFidget();
	void DoWalk ();
	void DoRun ();
	void StartRun ();
	bool DrainAttackOK (vec3f &start, vec3f &end);
	void DrainAttack ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
