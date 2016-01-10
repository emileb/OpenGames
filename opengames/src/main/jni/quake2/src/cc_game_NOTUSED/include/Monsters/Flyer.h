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
// cc_flyer.h
// Flyer Monster
//

#if !defined(CC_GUARD_FLYER_H) || !INCLUDE_GUARDS
#define CC_GUARD_FLYER_H

class CFlyer : public IMonster
{
public:
	enum
	{
		FRAME_start01,
		FRAME_start02,
		FRAME_start03,
		FRAME_start04,
		FRAME_start05,
		FRAME_start06,
		FRAME_stop01,
		FRAME_stop02,
		FRAME_stop03,
		FRAME_stop04,
		FRAME_stop05,
		FRAME_stop06,
		FRAME_stop07,
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
		FRAME_bankl01,
		FRAME_bankl02,
		FRAME_bankl03,
		FRAME_bankl04,
		FRAME_bankl05,
		FRAME_bankl06,
		FRAME_bankl07,
		FRAME_bankr01,
		FRAME_bankr02,
		FRAME_bankr03,
		FRAME_bankr04,
		FRAME_bankr05,
		FRAME_bankr06,
		FRAME_bankr07,
		FRAME_rollf01,
		FRAME_rollf02,
		FRAME_rollf03,
		FRAME_rollf04,
		FRAME_rollf05,
		FRAME_rollf06,
		FRAME_rollf07,
		FRAME_rollf08,
		FRAME_rollf09,
		FRAME_rollr01,
		FRAME_rollr02,
		FRAME_rollr03,
		FRAME_rollr04,
		FRAME_rollr05,
		FRAME_rollr06,
		FRAME_rollr07,
		FRAME_rollr08,
		FRAME_rollr09,
		FRAME_defens01,
		FRAME_defens02,
		FRAME_defens03,
		FRAME_defens04,
		FRAME_defens05,
		FRAME_defens06,
		FRAME_pain101,
		FRAME_pain102,
		FRAME_pain103,
		FRAME_pain104,
		FRAME_pain105,
		FRAME_pain106,
		FRAME_pain107,
		FRAME_pain108,
		FRAME_pain109,
		FRAME_pain201,
		FRAME_pain202,
		FRAME_pain203,
		FRAME_pain204,
		FRAME_pain301,
		FRAME_pain302,
		FRAME_pain303,
		FRAME_pain304,
	};

	virtual inline const char	*GetMonsterName() { return "Flyer"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_SIGHT,
		SOUND_IDLE,
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_SLASH,
		SOUND_SPROING,
		SOUND_DIE,

		SOUND_MAX
	);

	CFlyer (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	ROGUE_VIRTUAL void Attack ();
#if (MONSTER_SPECIFIC_FLAGS & FLYER_KNOWS_HOW_TO_DODGE)
#if ROGUE_FEATURES
	void Duck (float eta);
	void SideStep ();
#else
	void Dodge (IBaseEntity *Attacker, float eta);
#endif

	void ChooseAfterDodge ();
	void AI_Roll (float Dist);
#endif
	void Idle ();
	ROGUE_VIRTUAL void Run ();
	void Sight ();
	ROGUE_VIRTUAL void Stand ();
	ROGUE_VIRTUAL void Walk ();
	ROGUE_VIRTUAL void Melee ();

	void CheckMelee ();
	void LoopMelee ();
	void SlashLeft ();
	void SlashRight ();
	void PopBlades ();
	void FireLeft ();
	void FireRight ();
	void Fire (sint32 FlashNumber);

	void Dead ();
	ROGUE_VIRTUAL void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	ROGUE_VIRTUAL void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
