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
// cc_floater.h
// Floater Monster
//

#if !defined(CC_GUARD_FLOATER_H) || !INCLUDE_GUARDS
#define CC_GUARD_FLOATER_H

class CFloater : public IMonster
{
public:
	enum
	{
		FRAME_actvat01,
		FRAME_actvat02,
		FRAME_actvat03,
		FRAME_actvat04,
		FRAME_actvat05,
		FRAME_actvat06,
		FRAME_actvat07,
		FRAME_actvat08,
		FRAME_actvat09,
		FRAME_actvat10,
		FRAME_actvat11,
		FRAME_actvat12,
		FRAME_actvat13,
		FRAME_actvat14,
		FRAME_actvat15,
		FRAME_actvat16,
		FRAME_actvat17,
		FRAME_actvat18,
		FRAME_actvat19,
		FRAME_actvat20,
		FRAME_actvat21,
		FRAME_actvat22,
		FRAME_actvat23,
		FRAME_actvat24,
		FRAME_actvat25,
		FRAME_actvat26,
		FRAME_actvat27,
		FRAME_actvat28,
		FRAME_actvat29,
		FRAME_actvat30,
		FRAME_actvat31,
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
		FRAME_attak214,
		FRAME_attak215,
		FRAME_attak216,
		FRAME_attak217,
		FRAME_attak218,
		FRAME_attak219,
		FRAME_attak220,
		FRAME_attak221,
		FRAME_attak222,
		FRAME_attak223,
		FRAME_attak224,
		FRAME_attak225,
		FRAME_attak301,
		FRAME_attak302,
		FRAME_attak303,
		FRAME_attak304,
		FRAME_attak305,
		FRAME_attak306,
		FRAME_attak307,
		FRAME_attak308,
		FRAME_attak309,
		FRAME_attak310,
		FRAME_attak311,
		FRAME_attak312,
		FRAME_attak313,
		FRAME_attak314,
		FRAME_attak315,
		FRAME_attak316,
		FRAME_attak317,
		FRAME_attak318,
		FRAME_attak319,
		FRAME_attak320,
		FRAME_attak321,
		FRAME_attak322,
		FRAME_attak323,
		FRAME_attak324,
		FRAME_attak325,
		FRAME_attak326,
		FRAME_attak327,
		FRAME_attak328,
		FRAME_attak329,
		FRAME_attak330,
		FRAME_attak331,
		FRAME_attak332,
		FRAME_attak333,
		FRAME_attak334,
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
		FRAME_pain101,
		FRAME_pain102,
		FRAME_pain103,
		FRAME_pain104,
		FRAME_pain105,
		FRAME_pain106,
		FRAME_pain107,
		FRAME_pain201,
		FRAME_pain202,
		FRAME_pain203,
		FRAME_pain204,
		FRAME_pain205,
		FRAME_pain206,
		FRAME_pain207,
		FRAME_pain208,
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
		FRAME_stand101,
		FRAME_stand102,
		FRAME_stand103,
		FRAME_stand104,
		FRAME_stand105,
		FRAME_stand106,
		FRAME_stand107,
		FRAME_stand108,
		FRAME_stand109,
		FRAME_stand110,
		FRAME_stand111,
		FRAME_stand112,
		FRAME_stand113,
		FRAME_stand114,
		FRAME_stand115,
		FRAME_stand116,
		FRAME_stand117,
		FRAME_stand118,
		FRAME_stand119,
		FRAME_stand120,
		FRAME_stand121,
		FRAME_stand122,
		FRAME_stand123,
		FRAME_stand124,
		FRAME_stand125,
		FRAME_stand126,
		FRAME_stand127,
		FRAME_stand128,
		FRAME_stand129,
		FRAME_stand130,
		FRAME_stand131,
		FRAME_stand132,
		FRAME_stand133,
		FRAME_stand134,
		FRAME_stand135,
		FRAME_stand136,
		FRAME_stand137,
		FRAME_stand138,
		FRAME_stand139,
		FRAME_stand140,
		FRAME_stand141,
		FRAME_stand142,
		FRAME_stand143,
		FRAME_stand144,
		FRAME_stand145,
		FRAME_stand146,
		FRAME_stand147,
		FRAME_stand148,
		FRAME_stand149,
		FRAME_stand150,
		FRAME_stand151,
		FRAME_stand152,
		FRAME_stand201,
		FRAME_stand202,
		FRAME_stand203,
		FRAME_stand204,
		FRAME_stand205,
		FRAME_stand206,
		FRAME_stand207,
		FRAME_stand208,
		FRAME_stand209,
		FRAME_stand210,
		FRAME_stand211,
		FRAME_stand212,
		FRAME_stand213,
		FRAME_stand214,
		FRAME_stand215,
		FRAME_stand216,
		FRAME_stand217,
		FRAME_stand218,
		FRAME_stand219,
		FRAME_stand220,
		FRAME_stand221,
		FRAME_stand222,
		FRAME_stand223,
		FRAME_stand224,
		FRAME_stand225,
		FRAME_stand226,
		FRAME_stand227,
		FRAME_stand228,
		FRAME_stand229,
		FRAME_stand230,
		FRAME_stand231,
		FRAME_stand232,
		FRAME_stand233,
		FRAME_stand234,
		FRAME_stand235,
		FRAME_stand236,
		FRAME_stand237,
		FRAME_stand238,
		FRAME_stand239,
		FRAME_stand240,
		FRAME_stand241,
		FRAME_stand242,
		FRAME_stand243,
		FRAME_stand244,
		FRAME_stand245,
		FRAME_stand246,
		FRAME_stand247,
		FRAME_stand248,
		FRAME_stand249,
		FRAME_stand250,
		FRAME_stand251,
		FRAME_stand252,
	};

	inline const char	*GetMonsterName() { return "Technician"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_ATTACK2,
		SOUND_ATTACK3,
		SOUND_DEATH1,
		SOUND_IDLE,
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_SIGHT,

		SOUND_MAX
	);

	CFloater (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void Attack ();
	void Idle ();
	void Run ();
	void Sight ();
	void Stand ();
	void Walk ();
	void Melee ();

	void Zap ();
	void Wham ();
	void FireBlaster ();

	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

#if ROGUE_FEATURES
	void Dodge (IBaseEntity *Attacker, float eta, CTrace *tr) { MonsterDodge (Attacker, eta, tr); };
#endif

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
