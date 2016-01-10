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
// cc_local.h
// Local header. Definitions local to the Game.
//

#if !defined(CC_GUARD_LOCAL_H) || !INCLUDE_GUARDS
#define CC_GUARD_LOCAL_H

#include "Options.h"
#include "../shared/Shared.h"

inline const char *BuildDate()
{
	return __DATE__;
};

inline const char *TimeStamp()
{
	return __TIMESTAMP__;
};


// Forward declarations
class IBaseEntity;
class CPlayerEntity;

typedef sint32	FrameNumber;

#define TO_STRING2(x) #x
#define TO_STRING(x) TO_STRING2(x)

// Stand-alone classes
#include "Utility/Enum.h"
#include "Utility/Hash.h"
#include "Player/Print.h"
#include "Utility/FileSystem.h"
#include "Platform/Platform.h"
#include "Player/Commands.h"
#include "Utility/Indexing.h"
#include "Utility/Media.h"
#include "Utility/DmFlags.h"
#include "Utility/Write.h"
#include "Utility/Cvar.h"
#include "Utility/Parse.h"
#define BAN_BASIC_INFO
#include "Player/Ban.h"
#undef BAN_BASIC_INFO
#include "Utility/Timer.h"
#include "Game/Save.h"
#include "Utility/IRC.h"
#include "Utility/Trace.h"
#include "Modules/Modules.h"

// Main class files
#include "Game/GameApi.h"
#include "Game/Game.h"
#include "Player/Statusbar.h"
#include "Utility/MapPrint.h"
#if CLEANCTF_ENABLED
#include "CTF/Ctf.h"
#endif
#include "Entities/BaseEntity.h"
#include "Monsters/Monsters.h"
#include "Utility/Utilities.h"

// Classes that require classes from the above includes
#include "Utility/Sound.h"
#include "Entities/EntityList.h"
#else
FILE_WARNING
#endif
