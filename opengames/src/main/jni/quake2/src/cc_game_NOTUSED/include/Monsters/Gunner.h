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
// cc_gunner.h
// Gunner Monster
//

#if !defined(CC_GUARD_GUNNER_H) || !INCLUDE_GUARDS
#define CC_GUARD_GUNNER_H

class CGunner : public IMonster
{
public:
	enum
	{
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
		FRAME_stand61,
		FRAME_stand62,
		FRAME_stand63,
		FRAME_stand64,
		FRAME_stand65,
		FRAME_stand66,
		FRAME_stand67,
		FRAME_stand68,
		FRAME_stand69,
		FRAME_stand70,
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
		FRAME_walk24,
		FRAME_run01,
		FRAME_run02,
		FRAME_run03,
		FRAME_run04,
		FRAME_run05,
		FRAME_run06,
		FRAME_run07,
		FRAME_run08,
		FRAME_runs01,
		FRAME_runs02,
		FRAME_runs03,
		FRAME_runs04,
		FRAME_runs05,
		FRAME_runs06,
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
		FRAME_attak115,
		FRAME_attak116,
		FRAME_attak117,
		FRAME_attak118,
		FRAME_attak119,
		FRAME_attak120,
		FRAME_attak121,
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
		FRAME_attak226,
		FRAME_attak227,
		FRAME_attak228,
		FRAME_attak229,
		FRAME_attak230,
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
		FRAME_pain112,
		FRAME_pain113,
		FRAME_pain114,
		FRAME_pain115,
		FRAME_pain116,
		FRAME_pain117,
		FRAME_pain118,
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
		FRAME_duck01,
		FRAME_duck02,
		FRAME_duck03,
		FRAME_duck04,
		FRAME_duck05,
		FRAME_duck06,
		FRAME_duck07,
		FRAME_duck08,
	};

	inline float GetScale() { return 1.15f; }
	inline const char	*GetMonsterName() { return "Gunner"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_DEATH,
		SOUND_IDLE,
		SOUND_OPEN,
		SOUND_SEARCH,
		SOUND_SIGHT,
	
		SOUND_MAX
	);

	CGunner (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void ReFireChain ();
	void FireChain ();
	void Grenade ();
	void OpenGun ();
	void Fire ();

#if !ROGUE_FEATURES
	void DuckUp ();
	void DuckHold ();
#else
	bool GrenadeCheck ();
	void BlindCheck ();
	void Duck (float eta);
	void SideStep ();
#endif

	void DuckDown ();
	void RunAndShoot ();
	void Fidget ();

	void Attack ();
#if !ROGUE_FEATURES
	void Dodge (IBaseEntity *Attacker, float eta);
#else
	void Dodge (IBaseEntity *Attacker, float eta, CTrace *tr) { MonsterDodge (Attacker, eta, tr); };
#endif
	void Idle ();
	void Search ();
	void Run ();
	void Sight ();
	void Stand ();
	void Walk ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
