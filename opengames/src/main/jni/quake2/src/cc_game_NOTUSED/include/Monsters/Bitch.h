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
// cc_bitch.h
// Iron Maiden
//

#if !defined(CC_GUARD_BITCH_H) || !INCLUDE_GUARDS
#define CC_GUARD_BITCH_H

class CMaiden : public IMonster
{
public:
	enum
	{
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
		FRAME_attak122,
		FRAME_attak123,
		FRAME_attak124,
		FRAME_attak125,
		FRAME_attak126,
		FRAME_attak127,
		FRAME_attak128,
		FRAME_attak129,
		FRAME_attak130,
		FRAME_attak131,
		FRAME_attak132,
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
		FRAME_death101,
		FRAME_death102,
		FRAME_death103,
		FRAME_death104,
		FRAME_death105,
		FRAME_death106,
		FRAME_death107,
		FRAME_death108,
		FRAME_death109,
		FRAME_death110,
		FRAME_death111,
		FRAME_death112,
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
		FRAME_death211,
		FRAME_death212,
		FRAME_death213,
		FRAME_death214,
		FRAME_death215,
		FRAME_death216,
		FRAME_death217,
		FRAME_death218,
		FRAME_death219,
		FRAME_death220,
		FRAME_death221,
		FRAME_death222,
		FRAME_death223,
		FRAME_duck01,
		FRAME_duck02,
		FRAME_duck03,
		FRAME_duck04,
		FRAME_duck05,
		FRAME_duck06,
		FRAME_duck07,
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
		FRAME_pain313,
		FRAME_pain314,
		FRAME_pain315,
		FRAME_pain316,
		FRAME_pain317,
		FRAME_pain318,
		FRAME_pain319,
		FRAME_pain320,
		FRAME_pain321,
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
		FRAME_walk25,
		FRAME_walk26,
		FRAME_walk27,
		FRAME_recln201,
		FRAME_recln202,
		FRAME_recln203,
		FRAME_recln204,
		FRAME_recln205,
		FRAME_recln206,
		FRAME_recln207,
		FRAME_recln208,
		FRAME_recln209,
		FRAME_recln210,
		FRAME_recln211,
		FRAME_recln212,
		FRAME_recln213,
		FRAME_recln214,
		FRAME_recln215,
		FRAME_recln216,
		FRAME_recln217,
		FRAME_recln218,
		FRAME_recln219,
		FRAME_recln220,
		FRAME_recln221,
		FRAME_recln222,
		FRAME_recln223,
		FRAME_recln224,
		FRAME_recln225,
		FRAME_recln226,
		FRAME_recln227,
		FRAME_recln228,
		FRAME_recln229,
		FRAME_recln230,
		FRAME_recln231,
		FRAME_recln232,
		FRAME_recln233,
		FRAME_recln234,
		FRAME_recln235,
		FRAME_recln236,
		FRAME_recln237,
		FRAME_recln238,
		FRAME_recln239,
		FRAME_recln240,
		FRAME_recln101,
		FRAME_recln102,
		FRAME_recln103,
		FRAME_recln104,
		FRAME_recln105,
		FRAME_recln106,
		FRAME_recln107,
		FRAME_recln108,
		FRAME_recln109,
		FRAME_recln110,
		FRAME_recln111,
		FRAME_recln112,
		FRAME_recln113,
		FRAME_recln114,
		FRAME_recln115,
		FRAME_recln116,
		FRAME_recln117,
		FRAME_recln118,
		FRAME_recln119,
		FRAME_recln120,
		FRAME_recln121,
		FRAME_recln122,
		FRAME_recln123,
		FRAME_recln124,
		FRAME_recln125,
		FRAME_recln126,
		FRAME_recln127,
		FRAME_recln128,
		FRAME_recln129,
		FRAME_recln130,
		FRAME_recln131,
		FRAME_recln132,
		FRAME_recln133,
		FRAME_recln134,
		FRAME_recln135,
		FRAME_recln136,
		FRAME_recln137,
		FRAME_recln138,
		FRAME_recln139,
		FRAME_recln140,
	};

	virtual inline const char	*GetMonsterName() { return "Iron Maiden"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_MISSILE_PRELAUNCH,
		SOUND_MISSILE_LAUNCH,
		SOUND_MISSILE_RELOAD,
		SOUND_MELEE_SWING,
		SOUND_MELEE_HIT,
		SOUND_FALL_DOWN,
		SOUND_IDLE1,
		SOUND_IDLE2,
		SOUND_DEATH1,
		SOUND_DEATH2,
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_PAIN3,
		SOUND_SIGHT,
		SOUND_SEARCH,

		SOUND_MAX
	)

	CMaiden (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void Attack ();
	void Idle ();
	void Run ();
	void Sight ();
	void Stand ();
	void Walk ();
	void Melee ();

#if ROGUE_FEATURES
	void Duck (float eta);
	void SideStep ();
	void Dodge (IBaseEntity *Attacker, float eta, CTrace *tr) { MonsterDodge (Attacker, eta, tr); };
#else
	void DuckDown ();
	void DuckHold ();
	void DuckUp ();
	void Dodge (IBaseEntity *Attacker, float eta);
#endif

	void Moan ();
	void Reload ();
	void PreAttack ();
#if XATRIX_FEATURES
	virtual
#endif
	void Rocket ();
	void ReRocket ();
	void Slash ();
	void DoSlash ();
	void ReSlash ();
	void Attack1 ();

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
