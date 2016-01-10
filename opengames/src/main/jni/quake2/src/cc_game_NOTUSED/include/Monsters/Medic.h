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
// cc_medic.h
// Medic
//

#if !defined(CC_GUARD_MEDIC_H) || !INCLUDE_GUARDS
#define CC_GUARD_MEDIC_H

class CMedic : public IMonster
{
public:
	enum
	{
		FRAME_walk1,
		FRAME_walk2,
		FRAME_walk3,
		FRAME_walk4,
		FRAME_walk5,
		FRAME_walk6,
		FRAME_walk7,
		FRAME_walk8,
		FRAME_walk9,
		FRAME_walk10,
		FRAME_walk11,
		FRAME_walk12,
		FRAME_wait1,
		FRAME_wait2,
		FRAME_wait3,
		FRAME_wait4,
		FRAME_wait5,
		FRAME_wait6,
		FRAME_wait7,
		FRAME_wait8,
		FRAME_wait9,
		FRAME_wait10,
		FRAME_wait11,
		FRAME_wait12,
		FRAME_wait13,
		FRAME_wait14,
		FRAME_wait15,
		FRAME_wait16,
		FRAME_wait17,
		FRAME_wait18,
		FRAME_wait19,
		FRAME_wait20,
		FRAME_wait21,
		FRAME_wait22,
		FRAME_wait23,
		FRAME_wait24,
		FRAME_wait25,
		FRAME_wait26,
		FRAME_wait27,
		FRAME_wait28,
		FRAME_wait29,
		FRAME_wait30,
		FRAME_wait31,
		FRAME_wait32,
		FRAME_wait33,
		FRAME_wait34,
		FRAME_wait35,
		FRAME_wait36,
		FRAME_wait37,
		FRAME_wait38,
		FRAME_wait39,
		FRAME_wait40,
		FRAME_wait41,
		FRAME_wait42,
		FRAME_wait43,
		FRAME_wait44,
		FRAME_wait45,
		FRAME_wait46,
		FRAME_wait47,
		FRAME_wait48,
		FRAME_wait49,
		FRAME_wait50,
		FRAME_wait51,
		FRAME_wait52,
		FRAME_wait53,
		FRAME_wait54,
		FRAME_wait55,
		FRAME_wait56,
		FRAME_wait57,
		FRAME_wait58,
		FRAME_wait59,
		FRAME_wait60,
		FRAME_wait61,
		FRAME_wait62,
		FRAME_wait63,
		FRAME_wait64,
		FRAME_wait65,
		FRAME_wait66,
		FRAME_wait67,
		FRAME_wait68,
		FRAME_wait69,
		FRAME_wait70,
		FRAME_wait71,
		FRAME_wait72,
		FRAME_wait73,
		FRAME_wait74,
		FRAME_wait75,
		FRAME_wait76,
		FRAME_wait77,
		FRAME_wait78,
		FRAME_wait79,
		FRAME_wait80,
		FRAME_wait81,
		FRAME_wait82,
		FRAME_wait83,
		FRAME_wait84,
		FRAME_wait85,
		FRAME_wait86,
		FRAME_wait87,
		FRAME_wait88,
		FRAME_wait89,
		FRAME_wait90,
		FRAME_run1,
		FRAME_run2,
		FRAME_run3,
		FRAME_run4,
		FRAME_run5,
		FRAME_run6,
		FRAME_paina1,
		FRAME_paina2,
		FRAME_paina3,
		FRAME_paina4,
		FRAME_paina5,
		FRAME_paina6,
		FRAME_paina7,
		FRAME_paina8,
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
		FRAME_duck11,
		FRAME_duck12,
		FRAME_duck13,
		FRAME_duck14,
		FRAME_duck15,
		FRAME_duck16,
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
		FRAME_death25,
		FRAME_death26,
		FRAME_death27,
		FRAME_death28,
		FRAME_death29,
		FRAME_death30,
		FRAME_attack1,
		FRAME_attack2,
		FRAME_attack3,
		FRAME_attack4,
		FRAME_attack5,
		FRAME_attack6,
		FRAME_attack7,
		FRAME_attack8,
		FRAME_attack9,
		FRAME_attack10,
		FRAME_attack11,
		FRAME_attack12,
		FRAME_attack13,
		FRAME_attack14,
		FRAME_attack15,
		FRAME_attack16,
		FRAME_attack17,
		FRAME_attack18,
		FRAME_attack19,
		FRAME_attack20,
		FRAME_attack21,
		FRAME_attack22,
		FRAME_attack23,
		FRAME_attack24,
		FRAME_attack25,
		FRAME_attack26,
		FRAME_attack27,
		FRAME_attack28,
		FRAME_attack29,
		FRAME_attack30,
		FRAME_attack31,
		FRAME_attack32,
		FRAME_attack33,
		FRAME_attack34,
		FRAME_attack35,
		FRAME_attack36,
		FRAME_attack37,
		FRAME_attack38,
		FRAME_attack39,
		FRAME_attack40,
		FRAME_attack41,
		FRAME_attack42,
		FRAME_attack43,
		FRAME_attack44,
		FRAME_attack45,
		FRAME_attack46,
		FRAME_attack47,
		FRAME_attack48,
		FRAME_attack49,
		FRAME_attack50,
		FRAME_attack51,
		FRAME_attack52,
		FRAME_attack53,
		FRAME_attack54,
		FRAME_attack55,
		FRAME_attack56,
		FRAME_attack57,
		FRAME_attack58,
		FRAME_attack59,
		FRAME_attack60,
	};

	FrameNumber		MedicTryTime;
	uint8			MedicTries;

	virtual inline const char	*GetMonsterName() { return "Medic"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_IDLE,
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_DIE,
		SOUND_SIGHT,
		SOUND_SEARCH,
		SOUND_HOOK_LAUNCH,
		SOUND_HOOK_HIT,
		SOUND_HOOK_HEAL,
		SOUND_HOOK_RETRACT,

		SOUND_MAX
	);

	CMedic (uint32 ID);

	ROGUE_VIRTUAL void SaveMonsterFields (CFile &File)
	{
		SAVE_MONSTER_SOUNDS
		File.Write<FrameNumber> (MedicTryTime);
		File.Write<uint8> (MedicTries);
	}
	ROGUE_VIRTUAL void LoadMonsterFields (CFile &File)
	{
		LOAD_MONSTER_SOUNDS
		MedicTryTime = File.Read<FrameNumber> ();
		MedicTries = File.Read<uint8> ();
	}

	ROGUE_VIRTUAL void Attack ();
	void Run ();
	void Search ();
	void Idle ();
	void Sight ();
	void Stand ();
	void Walk ();
	ROGUE_VIRTUAL bool CheckAttack ();
#if !ROGUE_FEATURES
	void Dodge (IBaseEntity *Attacker, float eta);
	void Duck_Down ();
	void Duck_Hold ();
	void Duck_Up ();
#else
	void Dodge (IBaseEntity *Attacker, float eta, CTrace *tr) { MonsterDodge (Attacker, eta, tr); };
	void Duck (float eta);
	void SideStep ();
#endif

	CMonsterEntity	*FindDeadMonster ();
	ROGUE_VIRTUAL void FireBlaster ();
	void ContinueFiring ();
	void HookLaunch ();
	void HookRetract();
	void CableAttack ();

#if ROGUE_FEATURES
	void CleanupHeal (bool ChangeFrame = false);
	void AbortHeal (bool Gib, bool Mark);
#endif

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	ROGUE_VIRTUAL void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
