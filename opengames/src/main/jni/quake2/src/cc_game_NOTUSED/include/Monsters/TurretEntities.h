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
// cc_turret_entities.h
// Turret entities
//

class CTurretEntityBase : public IMapEntity, public IBrushModel, public IBlockableEntity
{
public:
	CTurretEntityBase ();
	CTurretEntityBase (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CTurretEntityBase)

	void Blocked (IBaseEntity *Other);

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (IBrushModel::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	};

	virtual void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IBrushModel::SaveFields (File);
	};

	virtual void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IBrushModel::LoadFields (File);
	};

	bool Run ();
	virtual void Spawn () = 0;
};

class CTurretBreach : public CTurretEntityBase
{
public:
	bool			FinishInit, ShouldFire;
	float			PitchOptions[4];
	String			Target;

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTurretBreach)

	CTurretBreach ();
	CTurretBreach (sint32 Index);

	void Fire ();
	void Think ();

	void Spawn ();
};

class CTurretBase : public CTurretEntityBase
{
public:
	CTurretBase ();
	CTurretBase (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CTurretBase)

	void Spawn ();
};

class CTurretDriver : public CInfantry
{
public:
	vec3f				MoveOrigin;
	CTurretBreach		*TargetedBreach;

	CTurretDriver (uint32 ID);

	void Attack () {};
	void Idle () {};
	void Run () {};
	void Sight () {};
	void Walk () {};
	void Melee () {};

	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void TurretThink ();
	void TurretLink ();
	void Spawn ();

	MONSTER_ID_HEADER
};

void AnglesNormalize(vec3f &vec);
