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
// cc_supertank.h
// Supertank (Boss1)
//

#if !defined(CC_GUARD_SUPERTANK_H) || !INCLUDE_GUARDS
#define CC_GUARD_SUPERTANK_H

class CSuperTank : public IMonster
{
public:
	enum
	{
		FRAME_attaka1,
		FRAME_attaka2,
		FRAME_attaka3,
		FRAME_attaka4,
		FRAME_attaka5,
		FRAME_attaka6,
		FRAME_attaka7,
		FRAME_attaka8,
		FRAME_attaka9,
		FRAME_attaka10,
		FRAME_attaka11,
		FRAME_attaka12,
		FRAME_attaka13,
		FRAME_attaka14,
		FRAME_attaka15,
		FRAME_attaka16,
		FRAME_attaka17,
		FRAME_attaka18,
		FRAME_attaka19,
		FRAME_attaka20,
		FRAME_attakb1,
		FRAME_attakb2,
		FRAME_attakb3,
		FRAME_attakb4,
		FRAME_attakb5,
		FRAME_attakb6,
		FRAME_attakb7,
		FRAME_attakb8,
		FRAME_attakb9,
		FRAME_attakb10,
		FRAME_attakb11,
		FRAME_attakb12,
		FRAME_attakb13,
		FRAME_attakb14,
		FRAME_attakb15,
		FRAME_attakb16,
		FRAME_attakb17,
		FRAME_attakb18,
		FRAME_attakb19,
		FRAME_attakb20,
		FRAME_attakb21,
		FRAME_attakb22,
		FRAME_attakb23,
		FRAME_attakb24,
		FRAME_attakb25,
		FRAME_attakb26,
		FRAME_attakb27,
		FRAME_attakc1,
		FRAME_attakc2,
		FRAME_attakc3,
		FRAME_attakc4,
		FRAME_attakc5,
		FRAME_attakc6,
		FRAME_attakc7,
		FRAME_attakc8,
		FRAME_attakc9,
		FRAME_attakc10,
		FRAME_attakc11,
		FRAME_attakc12,
		FRAME_attakc13,
		FRAME_attakc14,
		FRAME_attakc15,
		FRAME_attakc16,
		FRAME_attakc17,
		FRAME_attakc18,
		FRAME_attakc19,
		FRAME_attakc20,
		FRAME_attakc21,
		FRAME_attakc22,
		FRAME_attakc23,
		FRAME_attakc24,
		FRAME_attakc25,
		FRAME_attakc26,
		FRAME_attakc27,
		FRAME_attakd1,
		FRAME_attakd2,
		FRAME_attakd3,
		FRAME_attakd4,
		FRAME_attakd5,
		FRAME_attakd6,
		FRAME_backwd1,
		FRAME_backwd2,
		FRAME_backwd3,
		FRAME_backwd4,
		FRAME_backwd5,
		FRAME_backwd6,
		FRAME_backwd7,
		FRAME_backwd8,
		FRAME_backwd9,
		FRAME_backwd10,
		FRAME_backwd11,
		FRAME_backwd12,
		FRAME_backwd13,
		FRAME_backwd14,
		FRAME_backwd15,
		FRAME_backwd16,
		FRAME_backwd17,
		FRAME_backwd18,
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
		FRAME_death23,
		FRAME_death24,
		FRAME_death31,
		FRAME_death32,
		FRAME_death33,
		FRAME_death45,
		FRAME_death46,
		FRAME_death47,
		FRAME_forwrd1,
		FRAME_forwrd2,
		FRAME_forwrd3,
		FRAME_forwrd4,
		FRAME_forwrd5,
		FRAME_forwrd6,
		FRAME_forwrd7,
		FRAME_forwrd8,
		FRAME_forwrd9,
		FRAME_forwrd10,
		FRAME_forwrd11,
		FRAME_forwrd12,
		FRAME_forwrd13,
		FRAME_forwrd14,
		FRAME_forwrd15,
		FRAME_forwrd16,
		FRAME_forwrd17,
		FRAME_forwrd18,
		FRAME_left1,
		FRAME_left2,
		FRAME_left3,
		FRAME_left4,
		FRAME_left5,
		FRAME_left6,
		FRAME_left7,
		FRAME_left8,
		FRAME_left9,
		FRAME_left10,
		FRAME_left11,
		FRAME_left12,
		FRAME_left13,
		FRAME_left14,
		FRAME_left15,
		FRAME_left16,
		FRAME_left17,
		FRAME_left18,
		FRAME_paina1,
		FRAME_paina2,
		FRAME_paina3,
		FRAME_paina4,
		FRAME_painb5,
		FRAME_painb6,
		FRAME_painb7,
		FRAME_painb8,
		FRAME_painc9,
		FRAME_painc10,
		FRAME_painc11,
		FRAME_painc12,
		FRAME_right1,
		FRAME_right2,
		FRAME_right3,
		FRAME_right4,
		FRAME_right5,
		FRAME_right6,
		FRAME_right7,
		FRAME_right8,
		FRAME_right9,
		FRAME_right10,
		FRAME_right11,
		FRAME_right12,
		FRAME_right13,
		FRAME_right14,
		FRAME_right15,
		FRAME_right16,
		FRAME_right17,
		FRAME_right18,
		FRAME_stand1,
		FRAME_stand2,
		FRAME_stand3,
		FRAME_stand4,
		FRAME_stand5,
		FRAME_stand6,
		FRAME_stand7,
		FRAME_stand8,
		FRAME_stand9,
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
		FRAME_stand52,
		FRAME_stand53,
		FRAME_stand54,
		FRAME_stand55,
		FRAME_stand56,
		FRAME_stand57,
		FRAME_stand58,
		FRAME_stand59,
		FRAME_stand60,
	};

	virtual inline const char	*GetMonsterName() { return "Supertank"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_PAIN3,
		SOUND_DEATH,
		SOUND_SEARCH1,
		SOUND_SEARCH2,
		SOUND_TREAD,

		SOUND_MAX
	);

	CSuperTank (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void PlayTreadSound ();
	void Explode ();
	void MachineGun ();
	void Rocket ();
	void ReAttack1 ();
#if (MONSTER_SPECIFIC_FLAGS & SUPERTANK_USES_GRENADES)
	void Grenade ();
#endif

	void Attack ();
	void Run ();
	void Stand ();
	void Walk ();
	void Search ();

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
