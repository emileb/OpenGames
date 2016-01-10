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
// cc_xatrix_fixbot.h
// 
//

#if !defined(CC_GUARD_CC_XATRIX_FIXBOT_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_XATRIX_FIXBOT_H

class CFixbot : public IMonster
{
public:
	enum
	{
		FRAME_charging_01,
		FRAME_charging_02,
		FRAME_charging_03,
		FRAME_charging_04,
		FRAME_charging_05,
		FRAME_charging_06,
		FRAME_charging_07,
		FRAME_charging_08,
		FRAME_charging_09,
		FRAME_charging_10,
		FRAME_charging_11,
		FRAME_charging_12,
		FRAME_charging_13,
		FRAME_charging_14,
		FRAME_charging_15,
		FRAME_charging_16,
		FRAME_charging_17,
		FRAME_charging_18,
		FRAME_charging_19,
		FRAME_charging_20,
		FRAME_charging_21,
		FRAME_charging_22,
		FRAME_charging_23,
		FRAME_charging_24,
		FRAME_charging_25,
		FRAME_charging_26,
		FRAME_charging_27,
		FRAME_charging_28,
		FRAME_charging_29,
		FRAME_charging_30,
		FRAME_charging_31,
		FRAME_landing_01,
		FRAME_landing_02,
		FRAME_landing_03,
		FRAME_landing_04,
		FRAME_landing_05,
		FRAME_landing_06,
		FRAME_landing_07,
		FRAME_landing_08,
		FRAME_landing_09,
		FRAME_landing_10,
		FRAME_landing_11,
		FRAME_landing_12,
		FRAME_landing_13,
		FRAME_landing_14,
		FRAME_landing_15,
		FRAME_landing_16,
		FRAME_landing_17,
		FRAME_landing_18,
		FRAME_landing_19,
		FRAME_landing_20,
		FRAME_landing_21,
		FRAME_landing_22,
		FRAME_landing_23,
		FRAME_landing_24,
		FRAME_landing_25,
		FRAME_landing_26,
		FRAME_landing_27,
		FRAME_landing_28,
		FRAME_landing_29,
		FRAME_landing_30,
		FRAME_landing_31,
		FRAME_landing_32,
		FRAME_landing_33,
		FRAME_landing_34,
		FRAME_landing_35,
		FRAME_landing_36,
		FRAME_landing_37,
		FRAME_landing_38,
		FRAME_landing_39,
		FRAME_landing_40,
		FRAME_landing_41,
		FRAME_landing_42,
		FRAME_landing_43,
		FRAME_landing_44,
		FRAME_landing_45,
		FRAME_landing_46,
		FRAME_landing_47,
		FRAME_landing_48,
		FRAME_landing_49,
		FRAME_landing_50,
		FRAME_landing_51,
		FRAME_landing_52,
		FRAME_landing_53,
		FRAME_landing_54,
		FRAME_landing_55,
		FRAME_landing_56,
		FRAME_landing_57,
		FRAME_landing_58,
		FRAME_pushback_01,
		FRAME_pushback_02,
		FRAME_pushback_03,
		FRAME_pushback_04,
		FRAME_pushback_05,
		FRAME_pushback_06,
		FRAME_pushback_07,
		FRAME_pushback_08,
		FRAME_pushback_09,
		FRAME_pushback_10,
		FRAME_pushback_11,
		FRAME_pushback_12,
		FRAME_pushback_13,
		FRAME_pushback_14,
		FRAME_pushback_15,
		FRAME_pushback_16,
		FRAME_takeoff_01,
		FRAME_takeoff_02,
		FRAME_takeoff_03,
		FRAME_takeoff_04,
		FRAME_takeoff_05,
		FRAME_takeoff_06,
		FRAME_takeoff_07,
		FRAME_takeoff_08,
		FRAME_takeoff_09,
		FRAME_takeoff_10,
		FRAME_takeoff_11,
		FRAME_takeoff_12,
		FRAME_takeoff_13,
		FRAME_takeoff_14,
		FRAME_takeoff_15,
		FRAME_takeoff_16,
		FRAME_ambient_01,
		FRAME_ambient_02,
		FRAME_ambient_03,
		FRAME_ambient_04,
		FRAME_ambient_05,
		FRAME_ambient_06,
		FRAME_ambient_07,
		FRAME_ambient_08,
		FRAME_ambient_09,
		FRAME_ambient_10,
		FRAME_ambient_11,
		FRAME_ambient_12,
		FRAME_ambient_13,
		FRAME_ambient_14,
		FRAME_ambient_15,
		FRAME_ambient_16,
		FRAME_ambient_17,
		FRAME_ambient_18,
		FRAME_ambient_19,
		FRAME_paina_01,
		FRAME_paina_02,
		FRAME_paina_03,
		FRAME_paina_04,
		FRAME_paina_05,
		FRAME_paina_06,
		FRAME_painb_01,
		FRAME_painb_02,
		FRAME_painb_03,
		FRAME_painb_04,
		FRAME_painb_05,
		FRAME_painb_06,
		FRAME_painb_07,
		FRAME_painb_08,
		FRAME_pickup_01,
		FRAME_pickup_02,
		FRAME_pickup_03,
		FRAME_pickup_04,
		FRAME_pickup_05,
		FRAME_pickup_06,
		FRAME_pickup_07,
		FRAME_pickup_08,
		FRAME_pickup_09,
		FRAME_pickup_10,
		FRAME_pickup_11,
		FRAME_pickup_12,
		FRAME_pickup_13,
		FRAME_pickup_14,
		FRAME_pickup_15,
		FRAME_pickup_16,
		FRAME_pickup_17,
		FRAME_pickup_18,
		FRAME_pickup_19,
		FRAME_pickup_20,
		FRAME_pickup_21,
		FRAME_pickup_22,
		FRAME_pickup_23,
		FRAME_pickup_24,
		FRAME_pickup_25,
		FRAME_pickup_26,
		FRAME_pickup_27,
		FRAME_freeze_01,
		FRAME_shoot_01,
		FRAME_shoot_02,
		FRAME_shoot_03,
		FRAME_shoot_04,
		FRAME_shoot_05,
		FRAME_shoot_06,
		FRAME_weldstart_01,
		FRAME_weldstart_02,
		FRAME_weldstart_03,
		FRAME_weldstart_04,
		FRAME_weldstart_05,
		FRAME_weldstart_06,
		FRAME_weldstart_07,
		FRAME_weldstart_08,
		FRAME_weldstart_09,
		FRAME_weldstart_10,
		FRAME_weldmiddle_01,
		FRAME_weldmiddle_02,
		FRAME_weldmiddle_03,
		FRAME_weldmiddle_04,
		FRAME_weldmiddle_05,
		FRAME_weldmiddle_06,
		FRAME_weldmiddle_07,
		FRAME_weldend_01,
		FRAME_weldend_02,
		FRAME_weldend_03,
		FRAME_weldend_04,
		FRAME_weldend_05,
		FRAME_weldend_06,
		FRAME_weldend_07,
	};

	inline const char	*GetMonsterName() { return "Fixbot"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN1,
		SOUND_DIE,
		SOUND_WELD1,
		SOUND_WELD2,
		SOUND_WELD3,
		SOUND_MAX
	);

	CFixbot (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void Attack ();
	void Run ();
	void Stand ();
	void Walk ();

	void AI_Facing (float dist);
	void AI_Move2 (float dist);
	void AI_MoveToGoal (float dist);

	CMonsterEntity *FindDeadMonster ();
	bool SearchForMonsters ();
	void LandingGoal ();
	void TakeOffGoal ();
	void ChangeToRoam ();
	void RoamGoal ();
	void UseScanner ();
	void BlastOff (vec3f &start, vec3f &aimdir, int damage, int kick, int te_impact, int hspread, int vspread);
	void FlyVertical ();
	void FlyVertical2 ();
	void GoRoam ();
	bool CheckTelefrag ();
	void FireLaser ();
	void WeldState ();
	void FireWelder ();
	void FireBlaster ();

	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif