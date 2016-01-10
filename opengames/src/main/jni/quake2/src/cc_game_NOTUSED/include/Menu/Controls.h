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
// cc_controls.h
// Common controls
//

#if !defined(CC_GUARD_CONTROLS_H) || !INCLUDE_GUARDS
#define CC_GUARD_CONTROLS_H

#include <sstream>
#include "Utility/Conchars.h"

/**
\typedef	uint8 ELabelFlags

\brief	Defines an alias representing the label flags.
**/
typedef uint8 ELabelFlags;

/**
\enum	

\brief	Values that represent label flags. 
**/
enum
{
	LF_GREEN			=	1,
};

/**
\typedef	uint8 ELabelAlign

\brief	Defines an alias representing label text alignment.
**/
typedef uint8 ELabelAlign;

/**
\enum	

\brief	Values that represent label text alignment. 
**/
enum
{
	LA_LEFT,
	LA_CENTER,
	LA_RIGHT
};

class CMenu_Label : public CMenuItem
{
public:
	String					LabelString;
	ELabelFlags				Flags;
	ELabelAlign				Align;

	CMenu_Label				(CMenu *Menu, sint32 x, sint32 y);
	virtual void Draw		(CPlayerEntity *Player, CStatusBar *DrawState);

	virtual bool	CanSelect (CPlayerEntity *Player)
	{
		return Enabled;
	}
	virtual bool	Select (CPlayerEntity *Player)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *Player)
	{
	}
};

class CMenu_Image : public CMenuItem
{
public:
	String					ImageString;
	sint32					Width, Height;

	CMenu_Image				(CMenu *Menu, sint32 x, sint32 y);
	virtual void Draw		(CPlayerEntity *Player, CStatusBar *DrawState);

	virtual bool	CanSelect (CPlayerEntity *Player)
	{
		return Enabled;
	}
	virtual bool	Select (CPlayerEntity *Player)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *Player)
	{
	}
};

struct SSpinControlIndex
{
	const char		*Text;
	const char		*Value;
};

class CMenu_Spin : public CMenuItem
{
public:
	ELabelFlags				Flags;
	ELabelAlign				Align;
	sint32						Index; // Where we are in the spin control
	sint32						NumIndices;
	SSpinControlIndex		*Indices;

	CMenu_Spin				(CMenu *Menu, sint32 x, sint32 y, SSpinControlIndex *Indices);
	virtual void Draw		(CPlayerEntity *Player, CStatusBar *DrawState);

	virtual bool	CanSelect (CPlayerEntity *Player)
	{
		return Enabled;
	}
	virtual bool	Select (CPlayerEntity *Player)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *Player);
};

/**
\typedef	uint8 ESliderTextPosition

\brief	Defines an alias representing the slider text position.
**/
typedef uint8 ESliderTextPosition;

/**
\enum	

\brief	Values that represent the slider text position. 
**/
enum
{
	STP_RIGHT,
	STP_TOP,
	STP_LEFT,
	STP_BOTTOM,

	STP_CUSTOM // Allows programmer to shove the text where ever he wants
};

const int MAX_SLIDER_WIDTH = 64;

template <typename TType>
class CMenu_Slider : public CMenuItem
{
public:
	ELabelAlign				Align;
	ESliderTextPosition		TextAlign;
	sint32					TextX, TextY;
	uint8					Width;

	TType					Min;
	TType					Max;
	TType					Step;
	TType					Value;
	String					AppendText;

	CMenu_Slider			(CMenu *Menu, sint32 x, sint32 y) :
	  CMenuItem(Menu, x, y)
	{
	};

	virtual void Draw		(CPlayerEntity *Player, CStatusBar *DrawState)
	{
		if (Width > MAX_SLIDER_WIDTH-3)
			Width = MAX_SLIDER_WIDTH-3;

		sint32 drawX = x;

		switch (Align)
		{
		case LA_LEFT:
			drawX += 160;
			break;
		case LA_RIGHT:
			drawX += 160 - (Width * 8);
			break;
		case LA_CENTER:
			break;
		}
		DrawState->AddVirtualPoint_X (drawX);
		DrawState->AddVirtualPoint_Y (y + 120);

		char Buffer[MAX_SLIDER_WIDTH];
		Buffer[0] = CCHAR_DOWNLOADBAR_LEFT;

		// Which number is closest to the value?
		float Percent = (((!Value) ? 0.1 : ((float)Value / (float)Max)));
		sint32 BestValue = ((Width-1) * Percent);

		if (BestValue > Width)
			BestValue = Width;

		for (sint32 i = (sint32)Min; i <= (sint32)Width; i++)
		{
			Buffer[((i-(sint32)Min)+1)] = (i == BestValue) ? CCHAR_DOWNLOADBAR_THUMB : CCHAR_DOWNLOADBAR_CENTER;
		}

		Buffer[Width+1] = CCHAR_DOWNLOADBAR_RIGHT;
		Buffer[Width+2] = '\0';

		DrawState->AddString (Buffer, false, (Align == LA_CENTER));

		// Draw the value if desired
		switch (Align)
		{
		case LA_LEFT:
			drawX = x + 190 + (Width * 8);
			break;
		case LA_RIGHT:
			drawX = x + 190;
			break;
		case LA_CENTER:
			drawX = x + 145 + (Width * 8);
			break;
		}

		DrawState->AddVirtualPoint_X (drawX);

		DrawState->AddString (String::Format("%d%s", (int)Value, AppendText.CString()), Selected, false);
	};

	virtual bool	CanSelect (CPlayerEntity *Player)
	{
		return Enabled;
	}
	virtual bool	Select (CPlayerEntity *Player)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *Player)
	{
		switch (Player->Client.Respawn.MenuState.Key)
		{
		case CMenuState::KEY_RIGHT:
			if (Value == Max)
				return; // Can't do that, Dave

			Value += Step;
			if (Value > Max)
				Value = Max;
			break;
		case CMenuState::KEY_LEFT:
			if (Value == Min)
				return;

			Value -= Step;
			if (Value < Min)
				Value = Min;
			break;
		}
	};
};

class CMenu_Box : public CMenuItem
{
private:
	bool					Enabled;

public:
	ELabelAlign				Align;

	sint32						Width;
	sint32						Height;
	sint32						Type;

	CMenu_Box			(CMenu *Menu, sint32 x, sint32 y);
	virtual void Draw		(CPlayerEntity *Player, CStatusBar *DrawState);

	// Can't select
	bool	CanSelect (CPlayerEntity *Player)
	{
		return false;
	}
	bool	Select (CPlayerEntity *Player)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *Player)
	{
	};
};

#else
FILE_WARNING
#endif
