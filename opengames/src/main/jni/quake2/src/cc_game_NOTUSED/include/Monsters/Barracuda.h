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
// cc_barracuda.h
// Sharky!
//

#if !defined(CC_GUARD_BARRACUDA_H) || !INCLUDE_GUARDS
#define CC_GUARD_BARRACUDA_H

class CBarracudaShark : public IMonster
{
public:
	enum
	{
		FRAME_flpbit01,
		FRAME_flpbit02,
		FRAME_flpbit03,
		FRAME_flpbit04,
		FRAME_flpbit05,
		FRAME_flpbit06,
		FRAME_flpbit07,
		FRAME_flpbit08,
		FRAME_flpbit09,
		FRAME_flpbit10,
		FRAME_flpbit11,
		FRAME_flpbit12,
		FRAME_flpbit13,
		FRAME_flpbit14,
		FRAME_flpbit15,
		FRAME_flpbit16,
		FRAME_flpbit17,
		FRAME_flpbit18,
		FRAME_flpbit19,
		FRAME_flpbit20,
		FRAME_flptal01,
		FRAME_flptal02,
		FRAME_flptal03,
		FRAME_flptal04,
		FRAME_flptal05,
		FRAME_flptal06,
		FRAME_flptal07,
		FRAME_flptal08,
		FRAME_flptal09,
		FRAME_flptal10,
		FRAME_flptal11,
		FRAME_flptal12,
		FRAME_flptal13,
		FRAME_flptal14,
		FRAME_flptal15,
		FRAME_flptal16,
		FRAME_flptal17,
		FRAME_flptal18,
		FRAME_flptal19,
		FRAME_flptal20,
		FRAME_flptal21,
		FRAME_flphor01,
		FRAME_flphor02,
		FRAME_flphor03,
		FRAME_flphor04,
		FRAME_flphor05,
		FRAME_flphor06,
		FRAME_flphor07,
		FRAME_flphor08,
		FRAME_flphor09,
		FRAME_flphor10,
		FRAME_flphor11,
		FRAME_flphor12,
		FRAME_flphor13,
		FRAME_flphor14,
		FRAME_flphor15,
		FRAME_flphor16,
		FRAME_flphor17,
		FRAME_flphor18,
		FRAME_flphor19,
		FRAME_flphor20,
		FRAME_flphor21,
		FRAME_flphor22,
		FRAME_flphor23,
		FRAME_flphor24,
		FRAME_flpver01,
		FRAME_flpver02,
		FRAME_flpver03,
		FRAME_flpver04,
		FRAME_flpver05,
		FRAME_flpver06,
		FRAME_flpver07,
		FRAME_flpver08,
		FRAME_flpver09,
		FRAME_flpver10,
		FRAME_flpver11,
		FRAME_flpver12,
		FRAME_flpver13,
		FRAME_flpver14,
		FRAME_flpver15,
		FRAME_flpver16,
		FRAME_flpver17,
		FRAME_flpver18,
		FRAME_flpver19,
		FRAME_flpver20,
		FRAME_flpver21,
		FRAME_flpver22,
		FRAME_flpver23,
		FRAME_flpver24,
		FRAME_flpver25,
		FRAME_flpver26,
		FRAME_flpver27,
		FRAME_flpver28,
		FRAME_flpver29,
		FRAME_flppn101,
		FRAME_flppn102,
		FRAME_flppn103,
		FRAME_flppn104,
		FRAME_flppn105,
		FRAME_flppn201,
		FRAME_flppn202,
		FRAME_flppn203,
		FRAME_flppn204,
		FRAME_flppn205,
		FRAME_flpdth01,
		FRAME_flpdth02,
		FRAME_flpdth03,
		FRAME_flpdth04,
		FRAME_flpdth05,
		FRAME_flpdth06,
		FRAME_flpdth07,
		FRAME_flpdth08,
		FRAME_flpdth09,
		FRAME_flpdth10,
		FRAME_flpdth11,
		FRAME_flpdth12,
		FRAME_flpdth13,
		FRAME_flpdth14,
		FRAME_flpdth15,
		FRAME_flpdth16,
		FRAME_flpdth17,
		FRAME_flpdth18,
		FRAME_flpdth19,
		FRAME_flpdth20,
		FRAME_flpdth21,
		FRAME_flpdth22,
		FRAME_flpdth23,
		FRAME_flpdth24,
		FRAME_flpdth25,
		FRAME_flpdth26,
		FRAME_flpdth27,
		FRAME_flpdth28,
		FRAME_flpdth29,
		FRAME_flpdth30,
		FRAME_flpdth31,
		FRAME_flpdth32,
		FRAME_flpdth33,
		FRAME_flpdth34,
		FRAME_flpdth35,
		FRAME_flpdth36,
		FRAME_flpdth37,
		FRAME_flpdth38,
		FRAME_flpdth39,
		FRAME_flpdth40,
		FRAME_flpdth41,
		FRAME_flpdth42,
		FRAME_flpdth43,
		FRAME_flpdth44,
		FRAME_flpdth45,
		FRAME_flpdth46,
		FRAME_flpdth47,
		FRAME_flpdth48,
		FRAME_flpdth49,
		FRAME_flpdth50,
		FRAME_flpdth51,
		FRAME_flpdth52,
		FRAME_flpdth53,
		FRAME_flpdth54,
		FRAME_flpdth55,
		FRAME_flpdth56,
	};

	inline const char	*GetMonsterName() { return "Barracuda Shark"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_CHOMP,
		SOUND_ATTACK,
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_DEATH,
		SOUND_IDLE,
		SOUND_SEARCH,
		SOUND_SIGHT,

		SOUND_MAX
	);

	CBarracudaShark (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void Run ();
	void Sight ();
	void Stand ();
	void Walk ();
	void Melee ();

	void PreAttack ();
	void Bite ();
	void DoRun ();
	void RunLoop ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();

	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
