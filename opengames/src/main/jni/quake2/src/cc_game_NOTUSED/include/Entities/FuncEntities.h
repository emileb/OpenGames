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
// cc_func_entities.h
// Func_ entities that aren't brush models
//

class CFuncTimer : public IMapEntity, public IThinkableEntity, public IUsableEntity
{
public:
	FrameNumber		Wait, Random, PauseTime;

	CFuncTimer ();
	CFuncTimer (sint32 Index);

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CFuncTimer)

	void Think ();
	void Use (IBaseEntity *Other, IBaseEntity *Activator);

	bool Run ();
	void Spawn ();
};

class CTargetCharacter : public IMapEntity, public IBrushModel
{
public:
	uint8		Character;

	CTargetCharacter ();
	CTargetCharacter (sint32 Index);

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetCharacter)

	bool Run ();
	void Spawn ();
};

class CTargetString : public IMapEntity, public IUsableEntity
{
public:
	CTargetString ();
	CTargetString (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CTargetString)

	bool ParseField (const char *Key, const char *Value)
	{
		return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator);
	void Spawn ();
};

class CFuncClock : public IMapEntity, public IUsableEntity, public IThinkableEntity
{
public:
	char			*CountTarget;
	FrameNumber		Wait;
	uint8			Style;
	sint32			Seconds;
	sint32			Count;
	CTargetString	*TargetString;
	
	CFuncClock ();
	CFuncClock (sint32 Index);

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CFuncClock)

	void Think ();
	void Use (IBaseEntity *Other, IBaseEntity *Activator);

	void FormatCountdown ();
	void Reset ();

	bool Run ();
	void Spawn ();
};
