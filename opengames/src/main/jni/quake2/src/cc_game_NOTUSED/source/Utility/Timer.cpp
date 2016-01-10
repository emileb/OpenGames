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
// cc_timer.cpp
// Classes and functions to make global timers
//

#include "Local.h"

#if 0
void CGameTimer::Run ()
{
	if (EndTime <= Level.Frame)
		OnTimeUp ();
}

typedef std::multimap <sint32, CGameTimer *> TTimerListType;

TTimerListType TimerList;

void ClearTimers ()
{
	TimerList.clear ();
}

void RunTimers ()
{
	for (TTimerListType::iterator it = TimerList.begin(); it != TimerList.end(); )
	{
		CGameTimer *Timer = (*it).second;

		Timer->Run ();

		// Done
		if (Timer->EndTime <= Level.Frame)
		{
			it = TimerList.erase (it);

			Timer->Destroy ();
			QDelete Timer;

			continue;
		}
		++it;
	}
}

void AddTimer (CGameTimer *Timer, sint32 Key)
{
	TimerList.insert (std::pair <const sint32, CGameTimer*> (Key, Timer));
	Timer->Create ();
}

void KillTimer (sint32 Key)
{
	if (Key < 0)
		return;

	std::pair<TTimerListType::iterator,TTimerListType::iterator> ret = TimerList.equal_range(Key);
	for (TTimerListType::iterator it = ret.first; it != ret.second; )
	{
		// Kill the timers
		CGameTimer *Timer = (*it).second;
		it = TimerList.erase (it);

		Timer->Destroy ();
		QDelete Timer;
	}
}
#endif
