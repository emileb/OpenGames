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
// cc_rogue_carrier.h
// 
//

#if !defined(CC_GUARD_CC_ROGUE_CARRIER_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ROGUE_CARRIER_H

class CCarrier : public IMonster
{
public:
	enum
	{
		FRAME_search01,
		FRAME_search02,
		FRAME_search03,
		FRAME_search04,
		FRAME_search05,
		FRAME_search06,
		FRAME_search07,
		FRAME_search08,
		FRAME_search09,
		FRAME_search10,
		FRAME_search11,
		FRAME_search12,
		FRAME_search13,
		FRAME_firea01,
		FRAME_firea02,
		FRAME_firea03,
		FRAME_firea04,
		FRAME_firea05,
		FRAME_firea06,
		FRAME_firea07,
		FRAME_firea08,
		FRAME_firea09,
		FRAME_firea10,
		FRAME_firea11,
		FRAME_firea12,
		FRAME_firea13,
		FRAME_firea14,
		FRAME_firea15,
		FRAME_fireb01,
		FRAME_fireb02,
		FRAME_fireb03,
		FRAME_fireb04,
		FRAME_fireb05,
		FRAME_fireb06,
		FRAME_fireb07,
		FRAME_fireb08,
		FRAME_fireb09,
		FRAME_fireb10,
		FRAME_fireb11,
		FRAME_fireb12,
		FRAME_fireb13,
		FRAME_fireb14,
		FRAME_fireb15,
		FRAME_fireb16,
		FRAME_spawn01,
		FRAME_spawn02,
		FRAME_spawn03,
		FRAME_spawn04,
		FRAME_spawn05,
		FRAME_spawn06,
		FRAME_spawn07,
		FRAME_spawn08,
		FRAME_spawn09,
		FRAME_spawn10,
		FRAME_spawn11,
		FRAME_spawn12,
		FRAME_spawn13,
		FRAME_spawn14,
		FRAME_spawn15,
		FRAME_spawn16,
		FRAME_spawn17,
		FRAME_spawn18,
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
	};

	inline const char	*GetMonsterName() { return "Carrier"; }

	float			OriginalYawSpeed;
	FrameNumber		RefireWait, FrameCalc;
	vec3f			RailFireSpot;

	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_PAIN3,
		SOUND_DEATH,
		SOUND_SIGHT,
		SOUND_RAIL,
		SOUND_SPAWN,

		SOUND_MAX
	);

	CCarrier (uint32 ID);

	void SaveMonsterFields (CFile &File)
	{
		File.Write<float> (OriginalYawSpeed);
		SAVE_MONSTER_SOUNDS
	}

	void LoadMonsterFields (CFile &File)
	{
		OriginalYawSpeed = File.Read<float> ();
		LOAD_MONSTER_SOUNDS
	}

	void Attack ();
	void Run ();
	void Sight ();
	void Stand ();
	void Walk ();
	bool CheckAttack ();

	void ReAttackGrenade ();
	void AttackGrenade ();
	void ReAttackMachinegun ();
	void AttackMachinegun ();
	void Rocket ();
	void PredictiveRocket ();
	void Grenade ();
	void MachineGun ();
	void FireBulletRight ();
	void FireBulletLeft ();
	void SpawnMonsters ();
	void PrepSpawn ();
	void SpawnCheck ();
	void ReadySpawn ();
	void StartSpawn ();
	void CoopCheck ();
	void Rail ();
	void SaveLoc ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);
	void TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker,
					vec3f Dir, vec3f Point, vec3f Normal, sint32 Damage,
					sint32 Knockback, EDamageFlags DamageFlags, EMeansOfDeath MeansOfDeath);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif