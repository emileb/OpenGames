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
// cc_berserk.h
// Berserker
//

#if !defined(CC_GUARD_BERSERK_H) || !INCLUDE_GUARDS
#define CC_GUARD_BERSERK_H

class CBerserker : public IMonster
{
public:
	enum
	{
		FRAME_stand1,
		FRAME_stand2,
		FRAME_stand3,
		FRAME_stand4,
		FRAME_stand5,
		FRAME_standb1,
		FRAME_standb2,
		FRAME_standb3,
		FRAME_standb4,
		FRAME_standb5,
		FRAME_standb6,
		FRAME_standb7,
		FRAME_standb8,
		FRAME_standb9,
		FRAME_standb10,
		FRAME_standb11,
		FRAME_standb12,
		FRAME_standb13,
		FRAME_standb14,
		FRAME_standb15,
		FRAME_standb16,
		FRAME_standb17,
		FRAME_standb18,
		FRAME_standb19,
		FRAME_standb20,
		FRAME_walkc1,
		FRAME_walkc2,
		FRAME_walkc3,
		FRAME_walkc4,
		FRAME_walkc5,
		FRAME_walkc6,
		FRAME_walkc7,
		FRAME_walkc8,
		FRAME_walkc9,
		FRAME_walkc10,
		FRAME_walkc11,
		FRAME_run1,
		FRAME_run2,
		FRAME_run3,
		FRAME_run4,
		FRAME_run5,
		FRAME_run6,
		FRAME_att_a1,
		FRAME_att_a2,
		FRAME_att_a3,
		FRAME_att_a4,
		FRAME_att_a5,
		FRAME_att_a6,
		FRAME_att_a7,
		FRAME_att_a8,
		FRAME_att_a9,
		FRAME_att_a10,
		FRAME_att_a11,
		FRAME_att_a12,
		FRAME_att_a13,
		FRAME_att_b1,
		FRAME_att_b2,
		FRAME_att_b3,
		FRAME_att_b4,
		FRAME_att_b5,
		FRAME_att_b6,
		FRAME_att_b7,
		FRAME_att_b8,
		FRAME_att_b9,
		FRAME_att_b10,
		FRAME_att_b11,
		FRAME_att_b12,
		FRAME_att_b13,
		FRAME_att_b14,
		FRAME_att_b15,
		FRAME_att_b16,
		FRAME_att_b17,
		FRAME_att_b18,
		FRAME_att_b19,
		FRAME_att_b20,
		FRAME_att_b21,
		FRAME_att_c1,
		FRAME_att_c2,
		FRAME_att_c3,
		FRAME_att_c4,
		FRAME_att_c5,
		FRAME_att_c6,
		FRAME_att_c7,
		FRAME_att_c8,
		FRAME_att_c9,
		FRAME_att_c10,
		FRAME_att_c11,
		FRAME_att_c12,
		FRAME_att_c13,
		FRAME_att_c14,
		FRAME_att_c15,
		FRAME_att_c16,
		FRAME_att_c17,
		FRAME_att_c18,
		FRAME_att_c19,
		FRAME_att_c20,
		FRAME_att_c21,
		FRAME_att_c22,
		FRAME_att_c23,
		FRAME_att_c24,
		FRAME_att_c25,
		FRAME_att_c26,
		FRAME_att_c27,
		FRAME_att_c28,
		FRAME_att_c29,
		FRAME_att_c30,
		FRAME_att_c31,
		FRAME_att_c32,
		FRAME_att_c33,
		FRAME_att_c34,
		FRAME_r_att1,
		FRAME_r_att2,
		FRAME_r_att3,
		FRAME_r_att4,
		FRAME_r_att5,
		FRAME_r_att6,
		FRAME_r_att7,
		FRAME_r_att8,
		FRAME_r_att9,
		FRAME_r_att10,
		FRAME_r_att11,
		FRAME_r_att12,
		FRAME_r_att13,
		FRAME_r_att14,
		FRAME_r_att15,
		FRAME_r_att16,
		FRAME_r_att17,
		FRAME_r_att18,
		FRAME_r_attb1,
		FRAME_r_attb2,
		FRAME_r_attb3,
		FRAME_r_attb4,
		FRAME_r_attb5,
		FRAME_r_attb6,
		FRAME_r_attb7,
		FRAME_r_attb8,
		FRAME_r_attb9,
		FRAME_r_attb10,
		FRAME_r_attb11,
		FRAME_r_attb12,
		FRAME_r_attb13,
		FRAME_r_attb14,
		FRAME_r_attb15,
		FRAME_r_attb16,
		FRAME_r_attb17,
		FRAME_r_attb18,
		FRAME_slam1,
		FRAME_slam2,
		FRAME_slam3,
		FRAME_slam4,
		FRAME_slam5,
		FRAME_slam6,
		FRAME_slam7,
		FRAME_slam8,
		FRAME_slam9,
		FRAME_slam10,
		FRAME_slam11,
		FRAME_slam12,
		FRAME_slam13,
		FRAME_slam14,
		FRAME_slam15,
		FRAME_slam16,
		FRAME_slam17,
		FRAME_slam18,
		FRAME_slam19,
		FRAME_slam20,
		FRAME_slam21,
		FRAME_slam22,
		FRAME_slam23,
		FRAME_duck1,
		FRAME_duck2,
		FRAME_duck3,
		FRAME_duck4,
		FRAME_duck5,
		FRAME_duck6,
		FRAME_duck7,
		FRAME_duck8,
		FRAME_duck9,
		FRAME_duck10,
		FRAME_fall1,
		FRAME_fall2,
		FRAME_fall3,
		FRAME_fall4,
		FRAME_fall5,
		FRAME_fall6,
		FRAME_fall7,
		FRAME_fall8,
		FRAME_fall9,
		FRAME_fall10,
		FRAME_fall11,
		FRAME_fall12,
		FRAME_fall13,
		FRAME_fall14,
		FRAME_fall15,
		FRAME_fall16,
		FRAME_fall17,
		FRAME_fall18,
		FRAME_fall19,
		FRAME_fall20,
		FRAME_painc1,
		FRAME_painc2,
		FRAME_painc3,
		FRAME_painc4,
		FRAME_painb1,
		FRAME_painb2,
		FRAME_painb3,
		FRAME_painb4,
		FRAME_painb5,
		FRAME_painb6,
		FRAME_painb7,
		FRAME_painb8,
		FRAME_painb9,
		FRAME_painb10,
		FRAME_painb11,
		FRAME_painb12,
		FRAME_painb13,
		FRAME_painb14,
		FRAME_painb15,
		FRAME_painb16,
		FRAME_painb17,
		FRAME_painb18,
		FRAME_painb19,
		FRAME_painb20,
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
		FRAME_deathc1,
		FRAME_deathc2,
		FRAME_deathc3,
		FRAME_deathc4,
		FRAME_deathc5,
		FRAME_deathc6,
		FRAME_deathc7,
		FRAME_deathc8,
	};

	inline const char	*GetMonsterName() { return "Berserker"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN,
		SOUND_DIE,
		SOUND_IDLE,
		SOUND_PUNCH,
		SOUND_SIGHT,
		SOUND_SEARCH,

		SOUND_MAX
	);

	CBerserker (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void Run ();
	void Search ();
	void Sight ();
	void Stand ();
	void Walk ();
	void Melee ();

	void Fidget ();
	void AttackSpike ();
	void AttackClub ();
	void Swing ();

#if ROGUE_FEATURES
	void SideStep ();
	void Dodge (IBaseEntity *Attacker, float eta, CTrace *tr) { MonsterDodge (Attacker, eta, tr); };
#endif

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
