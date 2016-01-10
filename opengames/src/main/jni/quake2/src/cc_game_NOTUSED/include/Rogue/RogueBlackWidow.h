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
// cc_rogue_black_widow.h
// 
//

#if !defined(CC_GUARD_CC_ROGUE_BLACK_WIDOW_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ROGUE_BLACK_WIDOW_H

class CWidowGib : public CGibEntity, public ITouchableEntity
{
public:
	MediaIndex		HitSound;

	CWidowGib () :
	  CGibEntity (),
	  ITouchableEntity ()
	  {
	  };

	CWidowGib (sint32 Index) :
	  IBaseEntity (Index),
	  CGibEntity (Index),
	  ITouchableEntity (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER (CWidowGib);

	bool Run () { return CGibEntity::Run (); };

	void SaveFields (CFile &File)
	{
		File.Write<MediaIndex> (HitSound);
		CGibEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		HitSound = File.Read<MediaIndex> ();
		CGibEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
	}

	void	Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	static void Spawn (IBaseEntity *self, MediaIndex GibIndex, int damage, EGibType type, vec3f startpos, bool sized, MediaIndex hitsound, bool fade);
};

void ThrowWidowGib (IBaseEntity *self, MediaIndex GibIndex, int damage, int type);
void ThrowWidowGibLoc (IBaseEntity *self, MediaIndex GibIndex, int damage, int type, vec3f startpos, bool fade);
void ThrowWidowGibSized (IBaseEntity *self, MediaIndex GibIndex, int damage, int type, vec3f startpos, MediaIndex hitsound, bool fade);
void ThrowSmallStuff (IBaseEntity *Entity, vec3f point);
void ThrowMoreStuff (IBaseEntity *Entity, vec3f point);

class CBlackWidow : public CWidowStand
{
public:
	enum
	{
		FRAME_blackwidow3,
		FRAME_walk01,
		FRAME_walk02,
		FRAME_walk03,
		FRAME_walk04,
		FRAME_walk05,
		FRAME_walk06,
		FRAME_walk07,
		FRAME_walk08,
		FRAME_walk09,
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
		FRAME_firea01,
		FRAME_firea02,
		FRAME_firea03,
		FRAME_firea04,
		FRAME_firea05,
		FRAME_firea06,
		FRAME_firea07,
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
		FRAME_tongs01,
		FRAME_tongs02,
		FRAME_tongs03,
		FRAME_tongs04,
		FRAME_tongs05,
		FRAME_tongs06,
		FRAME_tongs07,
		FRAME_tongs08,
		FRAME_pain01,
		FRAME_pain02,
		FRAME_pain03,
		FRAME_pain04,
		FRAME_pain05,
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
		FRAME_death31,
		FRAME_death32,
		FRAME_death33,
		FRAME_death34,
		FRAME_death35,
		FRAME_death36,
		FRAME_death37,
		FRAME_death38,
		FRAME_death39,
		FRAME_death40,
		FRAME_death41,
		FRAME_death42,
		FRAME_death43,
		FRAME_death44,
		FRAME_dthsrh01,
		FRAME_dthsrh02,
		FRAME_dthsrh03,
		FRAME_dthsrh04,
		FRAME_dthsrh05,
		FRAME_dthsrh06,
		FRAME_dthsrh07,
		FRAME_dthsrh08,
		FRAME_dthsrh09,
		FRAME_dthsrh10,
		FRAME_dthsrh11,
		FRAME_dthsrh12,
		FRAME_dthsrh13,
		FRAME_dthsrh14,
		FRAME_dthsrh15,
		FRAME_dthsrh16,
		FRAME_dthsrh17,
		FRAME_dthsrh18,
		FRAME_dthsrh19,
		FRAME_dthsrh20,
		FRAME_dthsrh21,
		FRAME_dthsrh22,
	};

	inline float	GetScale() { return 2.0f; }
	inline const char	*GetMonsterName() { return "Black Widow"; }

	FrameNumber		MeleeDebounceTime;
	vec3f			BeamPos[2];

	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_PAIN3,
		SOUND_DEATH,
		SOUND_SEARCH1,
		SOUND_TENTACLES_RETRACT,

		SOUND_MAX
	);

	CBlackWidow (uint32 ID);

	void SaveMonsterFields (CFile &File)
	{
		File.Write<FrameNumber> (MeleeDebounceTime);
		SAVE_MONSTER_SOUNDS
	}

	void LoadMonsterFields (CFile &File)
	{
		MeleeDebounceTime = File.Read<FrameNumber>();
		LOAD_MONSTER_SOUNDS
	}

	void Run ();
	void Stand ();
	void Walk ();
	void Melee ();
	void Attack ();
	bool CheckAttack ();
	void Search ();
	bool Blocked (float Dist);

	void WidowExplode ();
	static bool TongueAttackOK (vec3f &spot1, vec3f &spot2, float dist);
	void KillChildren ();
	void ReAttackBeam ();
	void AttackBeam ();
	void KeepSearching ();
	void StartSearching ();
	void WidowExplosionLeg ();
	void WidowExplosion7 ();
	void WidowExplosion6 ();
	void WidowExplosion5 ();
	void WidowExplosion4 ();
	void WidowExplosion3 ();
	void WidowExplosion2 ();
	void WidowExplosion1 ();
	void Tongue ();
	void TonguePull ();
	void Crunch ();
	void Toss ();
	void FireBeam ();
	void SaveBeamTarget ();
	void SaveDisruptLocation ();
	void FireDisrupt ();
	void DisruptReAttack ();
	void ReadySpawn ();
	void SpawnCheck ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif