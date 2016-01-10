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
// cc_mutant.cpp
// Mutant
//

#if !defined(CC_GUARD_MUTANT_H) || !INCLUDE_GUARDS
#define CC_GUARD_MUTANT_H

class CMutant : public IMonster
{
public:
	enum
	{
		FRAME_attack01,
		FRAME_attack02,
		FRAME_attack03,
		FRAME_attack04,
		FRAME_attack05,
		FRAME_attack06,
		FRAME_attack07,
		FRAME_attack08,
		FRAME_attack09,
		FRAME_attack10,
		FRAME_attack11,
		FRAME_attack12,
		FRAME_attack13,
		FRAME_attack14,
		FRAME_attack15,
		FRAME_death101,
		FRAME_death102,
		FRAME_death103,
		FRAME_death104,
		FRAME_death105,
		FRAME_death106,
		FRAME_death107,
		FRAME_death108,
		FRAME_death109,
		FRAME_death201,
		FRAME_death202,
		FRAME_death203,
		FRAME_death204,
		FRAME_death205,
		FRAME_death206,
		FRAME_death207,
		FRAME_death208,
		FRAME_death209,
		FRAME_death210,
		FRAME_pain101,
		FRAME_pain102,
		FRAME_pain103,
		FRAME_pain104,
		FRAME_pain105,
		FRAME_pain201,
		FRAME_pain202,
		FRAME_pain203,
		FRAME_pain204,
		FRAME_pain205,
		FRAME_pain206,
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
		FRAME_run03,
		FRAME_run04,
		FRAME_run05,
		FRAME_run06,
		FRAME_run07,
		FRAME_run08,
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
		FRAME_stand153,
		FRAME_stand154,
		FRAME_stand155,
		FRAME_stand156,
		FRAME_stand157,
		FRAME_stand158,
		FRAME_stand159,
		FRAME_stand160,
		FRAME_stand161,
		FRAME_stand162,
		FRAME_stand163,
		FRAME_stand164,
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
	};

	bool		AttemptJumpToLastSight;
	bool		Jumping;
	inline const char	*GetMonsterName() { return "Mutant"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_SWING,
		SOUND_HIT1,
		SOUND_HIT2,
		SOUND_DEATH,
		SOUND_IDLE,
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_SIGHT,
		SOUND_SEARCH,
		SOUND_STEP1,
		SOUND_STEP2,
		SOUND_STEP3,
		SOUND_THUD,

		SOUND_MAX
	);

	CMutant (uint32 ID);

	void SaveMonsterFields (CFile &File)
	{
		SAVE_MONSTER_SOUNDS
		File.Write<bool> (AttemptJumpToLastSight);
		File.Write<bool> (Jumping);
	}
	void LoadMonsterFields (CFile &File)
	{
		LOAD_MONSTER_SOUNDS
		AttemptJumpToLastSight = File.Read<bool> ();
		Jumping = File.Read<bool> ();
	}

	void Step ();
	void Swing ();
	void IdleLoop ();
	void WalkLoop ();
	void HitLeft ();
	void HitRight ();
	void CheckRefire ();
	void JumpTakeOff ();
	void CheckLanding ();
	bool CheckMelee ();
	bool CheckJump ();
	
	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	void Attack ();
	void Melee ();
	void Run ();
	void Sight ();
	void Stand ();
	void Walk ();
	void Search ();
	void Idle ();

	bool CheckAttack ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
