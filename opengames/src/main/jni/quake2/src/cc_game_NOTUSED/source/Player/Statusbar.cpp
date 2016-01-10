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
// This can also be used for scoreboards (+ SendMsg)
//

#include "Local.h"

CStatusBar::CStatusBar() :
Bar ()
{
}

CStatusBar::~CStatusBar()
{
}

size_t CStatusBar::Length ()
{
	return Bar.Count();
}

void CStatusBar::Send ()
{
	ConfigString (CS_STATUSBAR, Bar.CString());
}

void CStatusBar::SendMsg (CPlayerEntity *Player, bool reliable)
{
	WriteByte (SVC_LAYOUT);
	WriteString (Bar.CString());
	Player->CastTo ((reliable) ? CASTFLAG_RELIABLE : CASTFLAG_UNRELIABLE);
}

void CStatusBar::AddToBarBuffer (const char *fmt, ...)
{
	va_list		argptr;
	CTempMemoryBlock text = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, fmt);
	vsnprintf (text.GetBuffer<char>(), text.GetSize() - 1, fmt, argptr);
	va_end (argptr);

	CC_ASSERT_EXPR (!(Bar.Count() + strlen(text.GetBuffer<char>()) > text.GetSize()), "Statusbar overflowed");

	Bar += text.GetBuffer<char>();
}

void CStatusBar::AddVirtualPoint_Y (sint32 y)
{
	AddToBarBuffer ("yv %i ", y);
}

void CStatusBar::AddVirtualPoint_X (sint32 x)
{
	AddToBarBuffer ("xv %i ", x);
}

void CStatusBar::AddPoint_X (sint32 x, bool inverted = false)
{
	AddToBarBuffer ("x%c %i ", inverted ? 'r' : 'l', x);
}

void CStatusBar::AddPoint_Y (sint32 y, bool inverted = false)
{
	AddToBarBuffer ("y%c %i ", inverted ? 'b' : 't', y);
}

void CStatusBar::AddString (const char *string, bool highBit, bool center)
{
	AddToBarBuffer ("%sstring%s \"%s\" ", center ? "c" : "", highBit ? "2" : "", string);
}

void CStatusBar::AddStatString (sint32 statIndex)
{
	AddToBarBuffer ("stat_string %i ", statIndex);
}

void CStatusBar::AddPic (const char *pic)
{
	AddToBarBuffer ("picn %s ", pic);
}

void CStatusBar::AddPicStat (sint32 statIndex)
{
	AddToBarBuffer ("pic %i ", statIndex);
}

void CStatusBar::AddNumStat (sint32 statIndex, sint32 width = 3)
{
	AddToBarBuffer ("num %i %i ", width, statIndex);
}

void CStatusBar::AddIf (sint32 statIndex)
{
	AddToBarBuffer ("if %i ", statIndex);
}

void CStatusBar::AddEndIf ()
{
	AddToBarBuffer ("endif ");
}

void CStatusBar::AddAmmoNum ()
{
	AddToBarBuffer ("anum ");
}

void CStatusBar::AddHealthNum ()
{
	AddToBarBuffer ("hnum ");
}

void CStatusBar::AddArmorNum ()
{
	AddToBarBuffer ("rnum ");
}

void CStatusBar::AddClientBlock (sint32 x, sint32 y, sint32 cNum, sint32 Score, sint32 ping, sint32 time)
{
	CC_ASSERT_EXPR (!(cNum >= Game.MaxClients || cNum < 0), "Client number out of bounds");
	AddToBarBuffer ("client %i %i %i %i %i %i ", x, y, cNum, Score, ping, time);
}

void CStatusBar::AddClientBlock (sint32 x, sint32 y, sint32 cNum, sint32 Score, sint32 ping)
{
	CC_ASSERT_EXPR (!(cNum >= Game.MaxClients || cNum < 0), "Client number out of bounds");
	AddToBarBuffer ("ctf %i %i %i %i %i %i ", x, y, cNum, Score, ping);
}

void CreateDMStatusbar ()
{
	CStatusBar DMBar;

	DMBar.AddPoint_Y (-24, true);

	DMBar.AddVirtualPoint_X (0);
	DMBar.AddHealthNum ();
	DMBar.AddVirtualPoint_X (50);
	DMBar.AddPicStat (STAT_HEALTH_ICON);

	DMBar.AddIf (STAT_AMMO_ICON);
		DMBar.AddVirtualPoint_X (100);
		DMBar.AddAmmoNum ();
		DMBar.AddVirtualPoint_X (150);
		DMBar.AddPicStat (STAT_AMMO_ICON);
	DMBar.AddEndIf ();

	DMBar.AddIf (STAT_ARMOR);
		DMBar.AddVirtualPoint_X (200);
		DMBar.AddArmorNum ();
		DMBar.AddVirtualPoint_X (250);
		DMBar.AddPicStat (STAT_ARMOR_ICON);
	DMBar.AddEndIf ();

	DMBar.AddIf (STAT_SELECTED_ICON);
		DMBar.AddVirtualPoint_X (296);
		DMBar.AddPicStat (STAT_SELECTED_ICON);
	DMBar.AddEndIf ();

	DMBar.AddPoint_Y (-50, true);

	DMBar.AddIf (STAT_PICKUP_ICON);
		DMBar.AddVirtualPoint_X (0);
		DMBar.AddPicStat (STAT_PICKUP_ICON);
		DMBar.AddVirtualPoint_X (26);
		DMBar.AddPoint_Y (-42, true);
		DMBar.AddStatString (STAT_PICKUP_STRING);
		DMBar.AddPoint_Y (-50, true);
	DMBar.AddEndIf ();

	DMBar.AddIf (STAT_TIMER_ICON);
		DMBar.AddVirtualPoint_X (246);
		DMBar.AddNumStat (STAT_TIMER, 2);
		DMBar.AddVirtualPoint_X (296);
		DMBar.AddPicStat (STAT_TIMER_ICON);
	DMBar.AddEndIf ();

	DMBar.AddIf (STAT_HELPICON);
		DMBar.AddVirtualPoint_X (148);
		DMBar.AddPicStat (STAT_HELPICON);
	DMBar.AddEndIf ();

	DMBar.AddPoint_X (-50, true);
	DMBar.AddPoint_Y (2);
	DMBar.AddNumStat (STAT_FRAGS);

	DMBar.AddPoint_Y (-129, true);

	DMBar.AddIf (STAT_TECH);
		DMBar.AddPoint_X (-26, true);
		DMBar.AddPicStat (STAT_TECH);
	DMBar.AddEndIf();

	DMBar.AddIf (STAT_CHASE);
		DMBar.AddVirtualPoint_X (0);
		DMBar.AddPoint_Y (-68, true);
		DMBar.AddString ("Chasing");
		DMBar.AddVirtualPoint_X (64);
		DMBar.AddStatString (STAT_CHASE);
	DMBar.AddEndIf ();

	DMBar.Send();
}

void CreateSPStatusbar ()
{
	CStatusBar SPBar;

	SPBar.AddPoint_Y (-24, true);

	SPBar.AddVirtualPoint_X (0);
	SPBar.AddHealthNum ();
	SPBar.AddVirtualPoint_X (50);
	SPBar.AddPicStat (STAT_HEALTH_ICON);

	SPBar.AddIf (STAT_AMMO_ICON);
		SPBar.AddVirtualPoint_X (100);
		SPBar.AddAmmoNum ();
		SPBar.AddVirtualPoint_X (150);
		SPBar.AddPicStat (STAT_AMMO_ICON);
	SPBar.AddEndIf ();

	SPBar.AddIf (STAT_ARMOR);
		SPBar.AddVirtualPoint_X (200);
		SPBar.AddArmorNum ();
		SPBar.AddVirtualPoint_X (250);
		SPBar.AddPicStat (STAT_ARMOR_ICON);
	SPBar.AddEndIf ();

	SPBar.AddIf (STAT_SELECTED_ICON);
		SPBar.AddVirtualPoint_X (296);
		SPBar.AddPicStat (STAT_SELECTED_ICON);
	SPBar.AddEndIf ();

	SPBar.AddPoint_Y (-50, true);

	SPBar.AddIf (STAT_PICKUP_ICON);
		SPBar.AddVirtualPoint_X (0);
		SPBar.AddPicStat (STAT_PICKUP_ICON);
		SPBar.AddVirtualPoint_X (26);
		SPBar.AddPoint_Y (-42, true);
		SPBar.AddStatString (STAT_PICKUP_STRING);
		SPBar.AddPoint_Y (-50, true);
	SPBar.AddEndIf ();

	SPBar.AddIf (STAT_TIMER_ICON);
		SPBar.AddVirtualPoint_X (246);
		SPBar.AddNumStat (STAT_TIMER, 2);
		SPBar.AddVirtualPoint_X (296);
		SPBar.AddPicStat (STAT_TIMER_ICON);
	SPBar.AddEndIf ();

	SPBar.AddIf (STAT_HELPICON);
		SPBar.AddVirtualPoint_X (148);
		SPBar.AddPicStat (STAT_HELPICON);
	SPBar.AddEndIf ();

	SPBar.Send();
}

void CreateMapDebugStatusbar ()
{
	CStatusBar DebugBar;

	DebugBar.AddPoint_Y (24, false);
	DebugBar.AddPoint_X (5, false);
	DebugBar.AddString ("Surface:");
	DebugBar.AddPoint_X (5 + (8 * 9), false);
	DebugBar.AddStatString (STAT_PICKUP_STRING);

	DebugBar.AddPoint_Y (24 + 8, false);
	DebugBar.AddPoint_X (5, false);
	DebugBar.AddString ("Entity:");
	DebugBar.AddPoint_X (5 + (8 * 9), false);
	DebugBar.AddStatString (STAT_TIMER_ICON);

	DebugBar.Send();
}

/*
==================
HelpComputer

Draw help computer.
==================
*/
void CPlayerEntity::HelpComputer ()
{
	CStatusBar Scoreboard;
	static const char	*sk = NULL;

	if (sk == NULL)
	{
		switch (CvarList[CV_SKILL].Integer())
		{
		case 0:
			sk = "easy";
			break;
		case 1:
			sk = "medium";
			break;
		case 2:
			sk = "hard";
			break;
		default:
			sk = "hard+";
			break;
		}
	}

	Scoreboard.AddVirtualPoint_X (32);
	Scoreboard.AddVirtualPoint_Y (8);
	Scoreboard.AddPic ("help");

	Scoreboard.AddVirtualPoint_X (202);
	Scoreboard.AddVirtualPoint_Y (12);
	Scoreboard.AddString (sk, true);

	Scoreboard.AddVirtualPoint_X (0);

	Scoreboard.AddVirtualPoint_Y (24);
	Scoreboard.AddString (Level.FullLevelName.CString(), true, true);

	Scoreboard.AddVirtualPoint_Y (54);
	Scoreboard.AddString (Game.HelpMessages[0].CString(), true, true);

	Scoreboard.AddVirtualPoint_Y (110);
	Scoreboard.AddString (Game.HelpMessages[1].CString(), true, true);

	Scoreboard.AddVirtualPoint_X (50);
	Scoreboard.AddVirtualPoint_Y (164);
	Scoreboard.AddString (" kills     goals    secrets", true);

	Scoreboard.AddVirtualPoint_Y (172);

	Scoreboard.AddString (String::Format("%3i/%3i     %i/%i       %i/%i", Level.Monsters.Killed, Level.Monsters.Total, 
		Level.Goals.Found, Level.Goals.Total,
		Level.Secrets.Found, Level.Secrets.Total), true, false);

	Scoreboard.SendMsg (this, true);
}

