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
// Events.h
// A C#-style delegate event class
//

#if !defined(CC_GUARD_EVENTS_H) || !INCLUDE_GUARDS
#define CC_GUARD_EVENTS_H

// A C#-style event class.
// Notes:
//   The max supported arguments is two. This is because this follows the C# standard
//   of 'sender, eventargs'. Optionally, you may omit the event arguments, or even the sender.
//   I found this to be a really nice method though, so this is CleanCode's event standard.

/*abstract*/ class EventArgs
{
	static EventArgs Empty;
};

template <typename T,
	typename Arg1 = void*,
	typename Arg2 = EventArgs>
class Event
{
protected:
	typedef std::list<T> listType;
	typedef typename listType::iterator iteratorType;
	std::list<T> functions;

public:
	void operator += (T functor) { functions.push_front(functor); }
	void operator -= (T functor) { functions.remove (functor); }

	void operator() ()
	{
		for (iteratorType it = functions.begin(); it != functions.end(); ++it)
			(*it)();
	}

	void operator() (Arg1 arg1)
	{
		for (iteratorType it = functions.begin(); it != functions.end(); ++it)
			(*it)(arg1);
	}
	
	void operator() (Arg1 arg1, Arg2 arg2)
	{
		for (iteratorType it = functions.begin(); it != functions.end(); ++it)
			(*it)(arg1, arg2);
	}
};

typedef void (*EventHandlerType) (void *sender, EventArgs args);
typedef Event<EventHandlerType, void*, EventArgs> EventHandler;

#else
FILE_WARNING
#endif