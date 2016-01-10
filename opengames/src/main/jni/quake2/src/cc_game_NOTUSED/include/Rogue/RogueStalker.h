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
// cc_rogue_stalker.h
// 
//

#if !defined(CC_GUARD_CC_ROGUE_STALKER_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ROGUE_STALKER_H

#if ROGUE_FEATURES
class CStalker : public IMonster
{
public:
	enum
	{
		FRAME_idle01,
		FRAME_idle02,
		FRAME_idle03,
		FRAME_idle04,
		FRAME_idle05,
		FRAME_idle06,
		FRAME_idle07,
		FRAME_idle08,
		FRAME_idle09,
		FRAME_idle10,
		FRAME_idle11,
		FRAME_idle12,
		FRAME_idle13,
		FRAME_idle14,
		FRAME_idle15,
		FRAME_idle16,
		FRAME_idle17,
		FRAME_idle18,
		FRAME_idle19,
		FRAME_idle20,
		FRAME_idle21,
		FRAME_idle201,
		FRAME_idle202,
		FRAME_idle203,
		FRAME_idle204,
		FRAME_idle205,
		FRAME_idle206,
		FRAME_idle207,
		FRAME_idle208,
		FRAME_idle209,
		FRAME_idle210,
		FRAME_idle211,
		FRAME_idle212,
		FRAME_idle213,
		FRAME_walk01,
		FRAME_walk02,
		FRAME_walk03,
		FRAME_walk04,
		FRAME_walk05,
		FRAME_walk06,
		FRAME_walk07,
		FRAME_walk08,
		FRAME_jump01,
		FRAME_jump02,
		FRAME_jump03,
		FRAME_jump04,
		FRAME_jump05,
		FRAME_jump06,
		FRAME_jump07,
		FRAME_run01,
		FRAME_run02,
		FRAME_run03,
		FRAME_run04,
		FRAME_attack01,
		FRAME_attack02,
		FRAME_attack03,
		FRAME_attack04,
		FRAME_attack05,
		FRAME_attack06,
		FRAME_attack07,
		FRAME_attack08,
		FRAME_attack11,
		FRAME_attack12,
		FRAME_attack13,
		FRAME_attack14,
		FRAME_attack15,
		FRAME_pain01,
		FRAME_pain02,
		FRAME_pain03,
		FRAME_pain04,
		FRAME_death01,
		FRAME_death02,
		FRAME_death03,
		FRAME_death04,
		FRAME_death05,
		FRAME_death06,
		FRAME_death07,
		FRAME_death08,
		FRAME_death09,
		FRAME_twitch01,
		FRAME_twitch02,
		FRAME_twitch03,
		FRAME_twitch04,
		FRAME_twitch05,
		FRAME_twitch06,
		FRAME_twitch07,
		FRAME_twitch08,
		FRAME_twitch09,
		FRAME_twitch10,
		FRAME_reactive01,
		FRAME_reactive02,
		FRAME_reactive03,
		FRAME_reactive04,
	};

	inline const char	*GetMonsterName() { return "Stalker"; }

	FrameNumber		TimeStamp;
	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN,
		SOUND_DIE,
		SOUND_SIGHT,
		SOUND_PUNCH_HIT1,
		SOUND_PUNCH_HIT2,
		SOUND_IDLE,

		SOUND_MAX
	);

	CStalker (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void Run ();
	void Walk ();
	void Sight ();
	void Stand ();
	void Idle ();
	void Attack ();
	void Melee ();

	bool DoPounce (vec3f Dest);
	void IdleNoise ();
	void DodgeJump ();
	void SwingCheckL ();
	void SwingCheckR ();
	void SwingAttack ();
	void JumpStraightUp ();
	void JumpWaitLand ();
	void FalseDeath ();
	void FalseDeathStart ();
	bool OKToTransition ();
	void Reactivate ();
	void Heal ();
	void ShootAttack ();
	void ShootAttack2 ();
	bool CheckLZ (IBaseEntity *target, vec3f dest);
		
	void Dodge (IBaseEntity *Attacker, float eta
#if ROGUE_FEATURES
		, CTrace *tr
#endif
		);

	inline bool OnCeiling ()
	{
		return Entity->GravityVector[2] > 0 ? 1 : 0;
	}

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};
#endif

#else
FILE_WARNING
#endif