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
// cc_sbar.h
// Contains a class to create statusbars easily, without the need to directly edit a string.
// This can also be used for like, scoreboards (+ SendMsg)
//

#if !defined(CC_GUARD_SBAR_H) || !INCLUDE_GUARDS
#define CC_GUARD_SBAR_H

class CStatusBar
{
	String			Bar;

public:
	CStatusBar();
	~CStatusBar();

	void Send ();
	void SendMsg (CPlayerEntity *Player, bool reliable = true);

	void AddToBarBuffer (const char *fmt, ...);
	void AddVirtualPoint_Y (sint32 y);
	void AddVirtualPoint_X (sint32 x);
	void AddPoint_X (sint32 x, bool inverted);
	void AddPoint_Y (sint32 y, bool inverted);
	void AddString (const char *string, bool highBit = false, bool center = false);

	inline void AddString (const String &str, bool highBit = false, bool center = false)
	{
		AddString (str.CString(), highBit, center);
	}

	void AddStatString (sint32 statIndex);
	void AddPic (const char *pic);
	void AddPicStat (sint32 statIndex);
	void AddNumStat (sint32 statIndex, sint32 width);
	void AddIf (sint32 statIndex);
	void AddEndIf ();
	void AddAmmoNum ();
	void AddHealthNum ();
	void AddArmorNum ();
	void AddClientBlock (sint32 x, sint32 y, sint32 cNum, sint32 Score, sint32 ping, sint32 time);
	void AddClientBlock (sint32 x, sint32 y, sint32 cNum, sint32 Score, sint32 ping);

	size_t	Length ();
};

#else
FILE_WARNING
#endif
