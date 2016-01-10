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
// cc_func_entities.cpp
// Func_ entities that aren't brush models
//

#include "Local.h"
#include "Entities/BrushModels.h"
#include "Entities/FuncEntities.h"
#include <ctime>

/*QUAKED func_timer (0.3 0.1 0.6) (-8 -8 -8) (8 8 8) START_ON
"wait"			base time between triggering all targets, default is 1
"random"		wait variance, default is 0

so, the basic time between firing is a random time between
(wait - random) and (wait + random)

"delay"			delay before first firing when turned on, default is 0

"pausetime"		additional delay used only the very first time
				and only if spawned with START_ON

These can used but not touched.
*/

/**
\enum	

\brief	Values that represent spawnflags pertaining to CFuncTimer. 
**/
enum
{
	TIMER_START_ON		= BIT(0)
};

CFuncTimer::CFuncTimer () :
	IBaseEntity (),
	IMapEntity (),
	IThinkableEntity (),
	IUsableEntity (),
	Wait (0),
	Random (0),
	PauseTime (0)
	{
	};

CFuncTimer::CFuncTimer (sint32 Index) :
	IBaseEntity (Index),
	IMapEntity (Index),
	IThinkableEntity (Index),
	IUsableEntity (Index),
	Wait (0),
	Random (0),
	PauseTime (0)
	{
	};

bool ValidateWait (IBaseEntity *Entity, uint8 *ClassOffset, const char *Value, void *ExtraData)
{
	if (atof(Value) == -1)
	{
		MapPrint (MAPPRINT_ERROR, Entity, Entity->State.GetOrigin(), "A wait of -1 is invalid\n");
		return false;
	}

	return true;
}

ENTITYFIELDS_BEGIN(CFuncTimer)
{
	CEntityField ("random",		EntityMemberOffset(CFuncTimer,Random),		FT_FRAMENUMBER | FT_SAVABLE),
	CEntityField ("pausetime",	EntityMemberOffset(CFuncTimer,PauseTime),	FT_FRAMENUMBER | FT_SAVABLE),
	CEntityField ("wait",		EntityMemberOffset(CFuncTimer,Wait),		FT_FRAMENUMBER | FT_SAVABLE,	ValidateWait),
};
ENTITYFIELDS_END(CFuncTimer)

bool			CFuncTimer::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CFuncTimer> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
};

void			CFuncTimer::SaveFields (CFile &File)
{
	SaveEntityFields <CFuncTimer> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
	IThinkableEntity::SaveFields (File);
}

void			CFuncTimer::LoadFields (CFile &File)
{
	LoadEntityFields <CFuncTimer> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);
}

void CFuncTimer::Think ()
{
	UseTargets (*User, Message);
	NextThink = Level.Frame + (Wait + (irandom(Random)));
}

bool CFuncTimer::Run ()
{
	return IBaseEntity::Run ();
};

void CFuncTimer::Use (IBaseEntity *Other, IBaseEntity *Activator)
{
	User = Activator;

	// if on, turn it off
	if (NextThink)
	{
		NextThink = 0;
		return;
	}

	// turn it on
	if (Delay)
		NextThink = Level.Frame + Delay;
	else
		Think ();
}

void CFuncTimer::Spawn ()
{
	if (!Wait)
		Wait = 10;

	if (Random >= Wait)
	{
		Random = Wait - ServerFramesPerSecond;
		// Paril FIXME
		// This to me seems like a very silly warning.
		MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Random is greater than or equal to wait\n");
	}

	if (SpawnFlags & TIMER_START_ON)
	{
		NextThink = Level.Frame + 10 + (PauseTime + Delay + Wait + irandom(Random));
		User = this;
	}

	GetSvFlags() = SVF_NOCLIENT;
}

LINK_CLASSNAME_TO_CLASS ("func_timer", CFuncTimer);

// These two are required for func_clock, so they're here.

/*QUAKED target_character (0 0 1) ?
used with target_string (must be on same "team")
"count" is position in the string (starts at 1)
*/

CTargetCharacter::CTargetCharacter () :
	IBaseEntity (),
	IMapEntity (),
	IBrushModel ()
	{
	};

CTargetCharacter::CTargetCharacter (sint32 Index) :
	IBaseEntity (Index),
	IMapEntity (Index),
	IBrushModel (Index)
	{
	};

bool CTargetCharacter::Run ()
{
	return IBrushModel::Run ();
};

void CTargetCharacter::Spawn ()
{
	PhysicsType = PHYSICS_PUSH;
	SetBrushModel ();
	GetSolid() = SOLID_BSP;
	State.GetFrame() = 12;
	Link ();
};

ENTITYFIELDS_BEGIN(CTargetCharacter)
{
	CEntityField ("count", EntityMemberOffset(CTargetCharacter,Character), FT_BYTE | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetCharacter)

bool CTargetCharacter::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetCharacter> (this, Key, Value))
		return true;

	return (IBrushModel::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
}

void			CTargetCharacter::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetCharacter> (this, File);
	IMapEntity::SaveFields (File);
	IBrushModel::SaveFields (File);
}

void			CTargetCharacter::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetCharacter> (this, File);
	IMapEntity::LoadFields (File);
	IBrushModel::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_character", CTargetCharacter);

/*QUAKED target_string (0 0 1) (-8 -8 -8) (8 8 8)
*/

CTargetString::CTargetString () :
	IBaseEntity (),
	IMapEntity ()
	{
	};

CTargetString::CTargetString (sint32 Index) :
	IBaseEntity (Index),
	IMapEntity (Index)
	{
	};

class CTargetStringForEachCallback : public CForEachTeamChainCallback
{
public:
	String		Message;
	IBaseEntity *Me;

	CTargetStringForEachCallback (IBaseEntity *Me, String &Message) :
	  Me(Me),
	  Message(Message)
	{
	};

	void Callback (IBaseEntity *e)
	{
		if (e == Me)
			return;

		CTargetCharacter *Entity = entity_cast<CTargetCharacter>(e);

		if (!Entity->Character)
			return;
		size_t n = Entity->Character - 1;
		if (n > Message.Count())
		{
			e->State.GetFrame() = 12;
			return;
		}

		switch (Message[n])
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			e->State.GetFrame() = (Message[n] - '0');
			break;
		case '-':
			e->State.GetFrame() = 10;
			break;
		case ':':
			e->State.GetFrame() = 11;
			break;
		default:
			e->State.GetFrame() = 12;
			break;
		}
	};
};

void CTargetString::Use (IBaseEntity *Other, IBaseEntity *Activator)
{
	CTargetStringForEachCallback (this, Message).Query (this);
}

void CTargetString::Spawn ()
{
}

LINK_CLASSNAME_TO_CLASS ("target_string", CTargetString);

/*QUAKED func_clock (0 0 1) (-8 -8 -8) (8 8 8) TIMER_UP TIMER_DOWN START_OFF MULTI_USE
target a target_string with this

The default is to be a time of day clock

TIMER_UP and TIMER_DOWN run for "count" seconds and the fire "pathtarget"
If START_OFF, this entity must be used before it starts

"style"		0 "xx"
			1 "xx:xx"
			2 "xx:xx:xx"
*/

/**
\enum	

\brief	Values that represent spawnflags pertaining to CFuncClock. 
**/
enum
{
	CLOCK_TIMER_UP		= BIT(0),
	CLOCK_TIMER_DOWN	= BIT(1),
	CLOCK_START_OFF		= BIT(2),
	CLOCK_MULTI_USE		= BIT(3)
};

CFuncClock::CFuncClock () :
	IBaseEntity (),
	IMapEntity (),
	IThinkableEntity (),
	IUsableEntity (),
	Seconds(0),
	TargetString(NULL),
	Wait(0),
	Style(0),
	Count(0),
	CountTarget(NULL)
	{
	};

CFuncClock::CFuncClock (sint32 Index) :
	IBaseEntity (Index),
	IMapEntity (Index),
	IThinkableEntity (Index),
	IUsableEntity (Index),
	Seconds(0),
	TargetString(NULL),
	Wait(0),
	Style(0),
	Count(0),
	CountTarget(NULL)
	{
	};

ENTITYFIELDS_BEGIN(CFuncClock)
{
	CEntityField ("style", EntityMemberOffset(CFuncClock,Style), FT_BYTE | FT_SAVABLE),
	CEntityField ("count", EntityMemberOffset(CFuncClock,Count), FT_INT | FT_SAVABLE),
	CEntityField ("pathtarget", EntityMemberOffset(CFuncClock,CountTarget), FT_LEVEL_STRING | FT_SAVABLE),

	CEntityField ("Wait", EntityMemberOffset(CFuncClock,Wait), FT_FRAMENUMBER | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("Seconds", EntityMemberOffset(CFuncClock,Seconds), FT_INT | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END (CFuncClock);

bool CFuncClock::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CFuncClock> (this, Key, Value))
		return true;

	return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
}

bool CFuncClock::Run ()
{
	return IBaseEntity::Run ();
};

void		CFuncClock::SaveFields (CFile &File)
{
	File.Write<sint32> ((TargetString) ? TargetString->State.GetNumber() : -1);

	SaveEntityFields <CFuncClock> (this, File);
	IMapEntity::SaveFields (File);
	IUsableEntity::SaveFields (File);
	IThinkableEntity::SaveFields (File);
}

void		CFuncClock::LoadFields (CFile &File)
{
	sint32 Index = File.Read<sint32> ();

	if (Index != -1)
		TargetString = entity_cast<CTargetString>(Game.Entities[Index].Entity);

	LoadEntityFields <CFuncClock> (this, File);
	IMapEntity::LoadFields (File);
	IUsableEntity::LoadFields (File);
	IThinkableEntity::LoadFields (File);
}

// don't let field width of any clock messages change, or it
// could cause an overwrite after a game load

void CFuncClock::Reset ()
{
	User = nullentity;
	if (SpawnFlags & CLOCK_TIMER_UP)
	{
		Seconds = 0;
		Wait = Count;
	}
	else if (SpawnFlags & CLOCK_TIMER_DOWN)
	{
		Seconds = Count;
		Wait = 0;
	}
}

void CFuncClock::FormatCountdown ()
{
	switch (Style)
	{
	case 0:
	default:
		Message = String::Format ("%2i", Seconds);
		break;
	case 1:
		Message = String::Format("%2i:%2i", Seconds / 60, Seconds % 60);
		if (Message[3] == ' ')
			Message[3] = '0';
		break;
	case 2:
		Message = String::Format ("%2i:%2i:%2i", Seconds / 3600, (Seconds - (Seconds / 3600) * 3600) / 60, Seconds % 60);
		if (Message[3] == ' ')
			Message[3] = '0';
		if (Message[6] == ' ')
			Message[6] = '0';
		break;
	};
}

void CFuncClock::Think ()
{
	if (!TargetString)
	{
		TargetString = entity_cast<CTargetString>(CC_Find<IMapEntity, EF_MAP, EntityMemberOffset(IMapEntity,TargetName)> (NULL, Target.CString()));
		if (!TargetString)
			return;
	}

	if (SpawnFlags & CLOCK_TIMER_UP)
	{
		FormatCountdown ();
		Seconds++;
	}
	else if (SpawnFlags & CLOCK_TIMER_DOWN)
	{
		FormatCountdown ();
		Seconds--;
	}
	else
	{
		struct tm	*ltime;
		time_t		gmtime;

		time(&gmtime);
		ltime = localtime (&gmtime);
		Message = String::Format ("%2i:%2i:%2i", ltime->tm_hour, ltime->tm_min, ltime->tm_sec);
		if (Message[3] == ' ')
			Message[3] = '0';
		if (Message[6] == ' ')
			Message[6] = '0';
	}

	TargetString->Message = Message;
	TargetString->Use (this, this);

	if (((SpawnFlags & CLOCK_TIMER_UP) && (Seconds > Wait)) ||
		((SpawnFlags & CLOCK_TIMER_DOWN) && (Seconds < Wait)))
	{
		if (CountTarget)
		{
			String savetarget = Target;
			String savemessage = Message;
			Target = CountTarget;
			Message.Clear();
			UseTargets (*User, Message);
			Target = savetarget;
			Message = savemessage;
		}

		if (!(SpawnFlags & CLOCK_MULTI_USE))
			return;

		Reset ();

		if (SpawnFlags & CLOCK_START_OFF)
			return;
	}

	NextThink = Level.Frame + 10;
}

void CFuncClock::Use (IBaseEntity *Other, IBaseEntity *Activator)
{
	if (!Usable)
		return;

	if (!(SpawnFlags & CLOCK_MULTI_USE))
		Usable = false;
	
	if (User)
		return;

	User = Activator;
	Think ();
}

void CFuncClock::Spawn ()
{
	if (Target.IsNullOrEmpty())
	{
		MapPrint (MAPPRINT_ERROR, this, GetAbsMin(), "No target\n");
		Free ();
		return;
	}

	if ((SpawnFlags & CLOCK_TIMER_DOWN) && (!Count))
	{
		MapPrint (MAPPRINT_ERROR, this, GetAbsMin(), "No count\n");
		Free ();
		return;
	}

	if ((SpawnFlags & CLOCK_TIMER_UP) && (!Count))
		Count = 3600;

	Reset ();

	if (SpawnFlags & CLOCK_START_OFF)
		Usable = true;
	else
		NextThink = Level.Frame + 10;
}

LINK_CLASSNAME_TO_CLASS ("func_clock", CFuncClock);

