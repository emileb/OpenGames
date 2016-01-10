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
// cc_xatrix_gekk.h
// 
//

#if !defined(CC_GUARD_CC_XATRIX_GEKK_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_XATRIX_GEKK_H

class CGekk : public IMonster
{
public:
	enum
	{
		FRAME_stand_01,
		FRAME_stand_02,
		FRAME_stand_03,
		FRAME_stand_04,
		FRAME_stand_05,
		FRAME_stand_06,
		FRAME_stand_07,
		FRAME_stand_08,
		FRAME_stand_09,
		FRAME_stand_10,
		FRAME_stand_11,
		FRAME_stand_12,
		FRAME_stand_13,
		FRAME_stand_14,
		FRAME_stand_15,
		FRAME_stand_16,
		FRAME_stand_17,
		FRAME_stand_18,
		FRAME_stand_19,
		FRAME_stand_20,
		FRAME_stand_21,
		FRAME_stand_22,
		FRAME_stand_23,
		FRAME_stand_24,
		FRAME_stand_25,
		FRAME_stand_26,
		FRAME_stand_27,
		FRAME_stand_28,
		FRAME_stand_29,
		FRAME_stand_30,
		FRAME_stand_31,
		FRAME_stand_32,
		FRAME_stand_33,
		FRAME_stand_34,
		FRAME_stand_35,
		FRAME_stand_36,
		FRAME_stand_37,
		FRAME_stand_38,
		FRAME_stand_39,
		FRAME_run_01,
		FRAME_run_02,
		FRAME_run_03,
		FRAME_run_04,
		FRAME_run_05,
		FRAME_run_06,
		FRAME_clawatk3_01,
		FRAME_clawatk3_02,
		FRAME_clawatk3_03,
		FRAME_clawatk3_04,
		FRAME_clawatk3_05,
		FRAME_clawatk3_06,
		FRAME_clawatk3_07,
		FRAME_clawatk3_08,
		FRAME_clawatk3_09,
		FRAME_clawatk4_01,
		FRAME_clawatk4_02,
		FRAME_clawatk4_03,
		FRAME_clawatk4_04,
		FRAME_clawatk4_05,
		FRAME_clawatk4_06,
		FRAME_clawatk4_07,
		FRAME_clawatk4_08,
		FRAME_clawatk5_01,
		FRAME_clawatk5_02,
		FRAME_clawatk5_03,
		FRAME_clawatk5_04,
		FRAME_clawatk5_05,
		FRAME_clawatk5_06,
		FRAME_clawatk5_07,
		FRAME_clawatk5_08,
		FRAME_clawatk5_09,
		FRAME_leapatk_01,
		FRAME_leapatk_02,
		FRAME_leapatk_03,
		FRAME_leapatk_04,
		FRAME_leapatk_05,
		FRAME_leapatk_06,
		FRAME_leapatk_07,
		FRAME_leapatk_08,
		FRAME_leapatk_09,
		FRAME_leapatk_10,
		FRAME_leapatk_11,
		FRAME_leapatk_12,
		FRAME_leapatk_13,
		FRAME_leapatk_14,
		FRAME_leapatk_15,
		FRAME_leapatk_16,
		FRAME_leapatk_17,
		FRAME_leapatk_18,
		FRAME_leapatk_19,
		FRAME_pain3_01,
		FRAME_pain3_02,
		FRAME_pain3_03,
		FRAME_pain3_04,
		FRAME_pain3_05,
		FRAME_pain3_06,
		FRAME_pain3_07,
		FRAME_pain3_08,
		FRAME_pain3_09,
		FRAME_pain3_10,
		FRAME_pain3_11,
		FRAME_pain4_01,
		FRAME_pain4_02,
		FRAME_pain4_03,
		FRAME_pain4_04,
		FRAME_pain4_05,
		FRAME_pain4_06,
		FRAME_pain4_07,
		FRAME_pain4_08,
		FRAME_pain4_09,
		FRAME_pain4_10,
		FRAME_pain4_11,
		FRAME_pain4_12,
		FRAME_pain4_13,
		FRAME_death1_01,
		FRAME_death1_02,
		FRAME_death1_03,
		FRAME_death1_04,
		FRAME_death1_05,
		FRAME_death1_06,
		FRAME_death1_07,
		FRAME_death1_08,
		FRAME_death1_09,
		FRAME_death1_10,
		FRAME_death2_01,
		FRAME_death2_02,
		FRAME_death2_03,
		FRAME_death2_04,
		FRAME_death2_05,
		FRAME_death2_06,
		FRAME_death2_07,
		FRAME_death2_08,
		FRAME_death2_09,
		FRAME_death2_10,
		FRAME_death2_11,
		FRAME_death3_01,
		FRAME_death3_02,
		FRAME_death3_03,
		FRAME_death3_04,
		FRAME_death3_05,
		FRAME_death3_06,
		FRAME_death3_07,
		FRAME_death4_01,
		FRAME_death4_02,
		FRAME_death4_03,
		FRAME_death4_04,
		FRAME_death4_05,
		FRAME_death4_06,
		FRAME_death4_07,
		FRAME_death4_08,
		FRAME_death4_09,
		FRAME_death4_10,
		FRAME_death4_11,
		FRAME_death4_12,
		FRAME_death4_13,
		FRAME_death4_14,
		FRAME_death4_15,
		FRAME_death4_16,
		FRAME_death4_17,
		FRAME_death4_18,
		FRAME_death4_19,
		FRAME_death4_20,
		FRAME_death4_21,
		FRAME_death4_22,
		FRAME_death4_23,
		FRAME_death4_24,
		FRAME_death4_25,
		FRAME_death4_26,
		FRAME_death4_27,
		FRAME_death4_28,
		FRAME_death4_29,
		FRAME_death4_30,
		FRAME_death4_31,
		FRAME_death4_32,
		FRAME_death4_33,
		FRAME_death4_34,
		FRAME_death4_35,
		FRAME_rduck_01,
		FRAME_rduck_02,
		FRAME_rduck_03,
		FRAME_rduck_04,
		FRAME_rduck_05,
		FRAME_rduck_06,
		FRAME_rduck_07,
		FRAME_rduck_08,
		FRAME_rduck_09,
		FRAME_rduck_10,
		FRAME_rduck_11,
		FRAME_rduck_12,
		FRAME_rduck_13,
		FRAME_lduck_01,
		FRAME_lduck_02,
		FRAME_lduck_03,
		FRAME_lduck_04,
		FRAME_lduck_05,
		FRAME_lduck_06,
		FRAME_lduck_07,
		FRAME_lduck_08,
		FRAME_lduck_09,
		FRAME_lduck_10,
		FRAME_lduck_11,
		FRAME_lduck_12,
		FRAME_lduck_13,
		FRAME_idle_01,
		FRAME_idle_02,
		FRAME_idle_03,
		FRAME_idle_04,
		FRAME_idle_05,
		FRAME_idle_06,
		FRAME_idle_07,
		FRAME_idle_08,
		FRAME_idle_09,
		FRAME_idle_10,
		FRAME_idle_11,
		FRAME_idle_12,
		FRAME_idle_13,
		FRAME_idle_14,
		FRAME_idle_15,
		FRAME_idle_16,
		FRAME_idle_17,
		FRAME_idle_18,
		FRAME_idle_19,
		FRAME_idle_20,
		FRAME_idle_21,
		FRAME_idle_22,
		FRAME_idle_23,
		FRAME_idle_24,
		FRAME_idle_25,
		FRAME_idle_26,
		FRAME_idle_27,
		FRAME_idle_28,
		FRAME_idle_29,
		FRAME_idle_30,
		FRAME_idle_31,
		FRAME_idle_32,
		FRAME_spit_01,
		FRAME_spit_02,
		FRAME_spit_03,
		FRAME_spit_04,
		FRAME_spit_05,
		FRAME_spit_06,
		FRAME_spit_07,
		FRAME_amb_01,
		FRAME_amb_02,
		FRAME_amb_03,
		FRAME_amb_04,
		FRAME_wdeath_01,
		FRAME_wdeath_02,
		FRAME_wdeath_03,
		FRAME_wdeath_04,
		FRAME_wdeath_05,
		FRAME_wdeath_06,
		FRAME_wdeath_07,
		FRAME_wdeath_08,
		FRAME_wdeath_09,
		FRAME_wdeath_10,
		FRAME_wdeath_11,
		FRAME_wdeath_12,
		FRAME_wdeath_13,
		FRAME_wdeath_14,
		FRAME_wdeath_15,
		FRAME_wdeath_16,
		FRAME_wdeath_17,
		FRAME_wdeath_18,
		FRAME_wdeath_19,
		FRAME_wdeath_20,
		FRAME_wdeath_21,
		FRAME_wdeath_22,
		FRAME_wdeath_23,
		FRAME_wdeath_24,
		FRAME_wdeath_25,
		FRAME_wdeath_26,
		FRAME_wdeath_27,
		FRAME_wdeath_28,
		FRAME_wdeath_29,
		FRAME_wdeath_30,
		FRAME_wdeath_31,
		FRAME_wdeath_32,
		FRAME_wdeath_33,
		FRAME_wdeath_34,
		FRAME_wdeath_35,
		FRAME_wdeath_36,
		FRAME_wdeath_37,
		FRAME_wdeath_38,
		FRAME_wdeath_39,
		FRAME_wdeath_40,
		FRAME_wdeath_41,
		FRAME_wdeath_42,
		FRAME_wdeath_43,
		FRAME_wdeath_44,
		FRAME_wdeath_45,
		FRAME_swim_01,
		FRAME_swim_02,
		FRAME_swim_03,
		FRAME_swim_04,
		FRAME_swim_05,
		FRAME_swim_06,
		FRAME_swim_07,
		FRAME_swim_08,
		FRAME_swim_09,
		FRAME_swim_10,
		FRAME_swim_11,
		FRAME_swim_12,
		FRAME_swim_13,
		FRAME_swim_14,
		FRAME_swim_15,
		FRAME_swim_16,
		FRAME_swim_17,
		FRAME_swim_18,
		FRAME_swim_19,
		FRAME_swim_20,
		FRAME_swim_21,
		FRAME_swim_22,
		FRAME_swim_23,
		FRAME_swim_24,
		FRAME_swim_25,
		FRAME_swim_26,
		FRAME_swim_27,
		FRAME_swim_28,
		FRAME_swim_29,
		FRAME_swim_30,
		FRAME_swim_31,
		FRAME_swim_32,
		FRAME_attack_01,
		FRAME_attack_02,
		FRAME_attack_03,
		FRAME_attack_04,
		FRAME_attack_05,
		FRAME_attack_06,
		FRAME_attack_07,
		FRAME_attack_08,
		FRAME_attack_09,
		FRAME_attack_10,
		FRAME_attack_11,
		FRAME_attack_12,
		FRAME_attack_13,
		FRAME_attack_14,
		FRAME_attack_15,
		FRAME_attack_16,
		FRAME_attack_17,
		FRAME_attack_18,
		FRAME_attack_19,
		FRAME_attack_20,
		FRAME_attack_21,
		FRAME_pain_01,
		FRAME_pain_02,
		FRAME_pain_03,
		FRAME_pain_04,
		FRAME_pain_05,
		FRAME_pain_06,
	};

	inline const char	*GetMonsterName() { return "Gekk"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_SWING,
		SOUND_HIT,
		SOUND_HIT2,
		SOUND_DEATH,
		SOUND_PAIN1,
		SOUND_SIGHT,
		SOUND_SEARCH,
		SOUND_STEP1,
		SOUND_STEP2,
		SOUND_STEP3,
		SOUND_THUD,
		SOUND_CHANT_LOW,
		SOUND_CHANT_MID,
		SOUND_CHANT_HIGH,

		SOUND_MAX
	);

	bool Jumping;

	void SaveMonsterFields (CFile &File)
	{
		SAVE_MONSTER_SOUNDS
		File.Write<bool> (Jumping);
	}
	void LoadMonsterFields (CFile &File)
	{
		LOAD_MONSTER_SOUNDS
		Jumping = File.Read <bool> ();
	}

	CGekk (uint32 ID);

	bool CheckMelee ();
	bool CheckJump ();
	bool CheckJumpClose ();

	void Gekk_AI_Stand (float Dist);

	void LandToWater ();
	void WaterToLand ();
	void GibFest ();
	void IsGibFest ();
	void Jump ();
	void CheckLanding ();
	void StopSkid ();
	void JumpTakeoff2 ();
	void JumpTakeoff ();
	void Bite ();
	void CheckUnderwater ();
	void ReFireLoogie ();
	void Loogie ();
	void CheckMeleeRefire ();
	void HitLeft ();
	void HitRight ();
	void DoRun ();
	void IdleLoop ();
	void Chant ();
	void SwimLoop ();
	void Face ();
	void Swing ();
	void Step ();
	void Swim ();

	void Attack ();
	void Run (); // run start
	void Stand ();
	void Walk ();
	void Search ();
	void Idle ();
	void Sight ();
	void Melee ();
#if !ROGUE_FEATURES
	void Dodge (IBaseEntity *Attacker, float eta);
	void Duck_Down ();
	void Duck_Hold ();
	void Duck_Up ();
#else
	void Duck (float eta);
	void SideStep ();
#endif

	bool CheckAttack ();

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);
	void DamageEffect (vec3f &Dir, vec3f &Point, vec3f &Normal, sint32 &Damage, EDamageFlags &DamageFlags, EMeansOfDeath &MeansOfDeath);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

class CLoogie : public IFlyMissileProjectile, public IThinkableEntity, public ITouchableEntity
{
public:
	sint32		Damage;

	CLoogie ();
	CLoogie (sint32 Index);

	IMPLEMENT_SAVE_HEADER (CLoogie)

	void SaveFields (CFile &File)
	{
		File.Write<sint32> (Damage);

		IThinkableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
		IFlyMissileProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Damage = File.Read <sint32>();

		IThinkableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
		IFlyMissileProjectile::LoadFields (File);
	}

	void Think ();
	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	static void Spawn	(IBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed);

	bool Run ();
};

#else
FILE_WARNING
#endif