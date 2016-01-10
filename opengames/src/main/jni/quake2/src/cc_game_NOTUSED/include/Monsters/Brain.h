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
// cc_brain.h
// 
//

#if !defined(CC_GUARD_BRAIN_H) || !INCLUDE_GUARDS
#define CC_GUARD_BRAIN_H

class CBrain : public IMonster
{
public:
	enum
	{
		FRAME_walk101,
		FRAME_walk102,
		FRAME_walk103,
		FRAME_walk104,
		FRAME_walk105,
		FRAME_walk106,
		FRAME_walk107,
		FRAME_walk108,
		FRAME_walk109,
		FRAME_walk110,
		FRAME_walk111,
		FRAME_walk112,
		FRAME_walk113,
		FRAME_walk201,
		FRAME_walk202,
		FRAME_walk203,
		FRAME_walk204,
		FRAME_walk205,
		FRAME_walk206,
		FRAME_walk207,
		FRAME_walk208,
		FRAME_walk209,
		FRAME_walk210,
		FRAME_walk211,
		FRAME_walk212,
		FRAME_walk213,
		FRAME_walk214,
		FRAME_walk215,
		FRAME_walk216,
		FRAME_walk217,
		FRAME_walk218,
		FRAME_walk219,
		FRAME_walk220,
		FRAME_walk221,
		FRAME_walk222,
		FRAME_walk223,
		FRAME_walk224,
		FRAME_walk225,
		FRAME_walk226,
		FRAME_walk227,
		FRAME_walk228,
		FRAME_walk229,
		FRAME_walk230,
		FRAME_walk231,
		FRAME_walk232,
		FRAME_walk233,
		FRAME_walk234,
		FRAME_walk235,
		FRAME_walk236,
		FRAME_walk237,
		FRAME_walk238,
		FRAME_walk239,
		FRAME_walk240,
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
		FRAME_pain119,
		FRAME_pain120,
		FRAME_pain121,
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
		FRAME_death113,
		FRAME_death114,
		FRAME_death115,
		FRAME_death116,
		FRAME_death117,
		FRAME_death118,
		FRAME_death201,
		FRAME_death202,
		FRAME_death203,
		FRAME_death204,
		FRAME_death205,
		FRAME_duck01,
		FRAME_duck02,
		FRAME_duck03,
		FRAME_duck04,
		FRAME_duck05,
		FRAME_duck06,
		FRAME_duck07,
		FRAME_duck08,
		FRAME_defens01,
		FRAME_defens02,
		FRAME_defens03,
		FRAME_defens04,
		FRAME_defens05,
		FRAME_defens06,
		FRAME_defens07,
		FRAME_defens08,
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
	};

	bool		Refire;

	inline const char	*GetMonsterName() { return "Brain"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_CHEST_OPEN,
		SOUND_TENTACLES_EXTEND,
		SOUND_TENTACLES_RETRACT,
		SOUND_DEATH,
		SOUND_IDLE1,
		SOUND_IDLE2,
		SOUND_IDLE3,
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_SIGHT,
		SOUND_SEARCH,
		SOUND_MELEE1,
		SOUND_MELEE2,
		SOUND_MELEE3,

		SOUND_MAX
	);

	CBrain (uint32 ID);

	void SaveMonsterFields (CFile &File)
	{
		SAVE_MONSTER_SOUNDS
		File.Write<bool> (Refire);
	}
	void LoadMonsterFields (CFile &File)
	{
		LOAD_MONSTER_SOUNDS
		Refire = File.Read<bool> ();
	}

	void Run ();
	void Search ();
	void Sight ();
	void Stand ();
	void Walk ();
	void Idle ();
	void Melee ();

	void SwingRight ();
	void SwingLeft ();
	void HitRight ();
	void HitLeft ();
	void ChestOpen ();
	void TentacleAttack ();
	void ChestClosed ();

#if XATRIX_FEATURES
	void Attack ();

	void TongueAttack ();
	void LaserBeamFire ();
	void LaserBeamRefire ();
#endif

#if !ROGUE_FEATURES
	void Duck_Down ();
	void Duck_Hold ();
	void Duck_Up ();

	void Dodge (IBaseEntity *Attacker, float eta);
#else
	void Dodge (IBaseEntity *Attacker, float eta, CTrace *tr) { MonsterDodge (Attacker, eta, tr); };
	void Duck (float eta);
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
