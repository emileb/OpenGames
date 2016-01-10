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
// cc_ctfadmin.cpp
// 
//

#include "Local.h"
#include "Menu/Menu.h"

#if CLEANCTF_ENABLED
/**
\fn	bool CTFBeginElection(CPlayerEntity *Player, EElectState Type, String Message)

\brief	Begin an election. 

\author	Paril
\date	29/05/2010

\param [in,out]	Player	If non-null, the player. 
\param	Type			The type. 
\param	Message			The message. 

\return	true if it succeeds, false if it fails. 
**/
bool CTFBeginElection(CPlayerEntity *Player, EElectState Type, String Message);

class CCTFSettingsMenu : public CMenu
{
public:
	CMenu_Spin			*WeaponsStaySpin,
						*InstantItemsSpin,
						*QuadDropSpin,
						*InstantWeaponsSpin;

	class CCloseLabel : public CMenu_Label
	{
	public:
		CCloseLabel(CCTFSettingsMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *Player)
		{
			return true;
		};
	};
	
	class CApplyLabel : public CMenu_Label
	{
	public:
		CCTFSettingsMenu *MyMenu;

		CApplyLabel(CCTFSettingsMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y),
		MyMenu(Menu)
		{
		};

		bool Select (CPlayerEntity *Player)
		{
			sint32 i = CvarList[CV_DMFLAGS].Integer();
			if (!!MyMenu->WeaponsStaySpin->Index != DeathmatchFlags.dfWeaponsStay.IsEnabled())
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s weapons stay.\n",
					Player->Client.Persistent.Name.CString(), MyMenu->WeaponsStaySpin->Index ? "on" : "off");

				if (MyMenu->WeaponsStaySpin->Index)
					i |= DF_WEAPONS_STAY;
				else
					i &= ~DF_WEAPONS_STAY;
			}

			if (!!MyMenu->InstantItemsSpin->Index != DeathmatchFlags.dfInstantItems.IsEnabled())
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s instant items.\n",
					Player->Client.Persistent.Name.CString(), MyMenu->InstantItemsSpin->Index ? "on" : "off");

				if (MyMenu->InstantItemsSpin->Index)
					i |= DF_INSTANT_ITEMS;
				else
					i &= ~DF_INSTANT_ITEMS;
			}

			if (!!MyMenu->QuadDropSpin->Index != DeathmatchFlags.dfQuadDrop.IsEnabled())
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s quad drop.\n",
					Player->Client.Persistent.Name.CString(), MyMenu->QuadDropSpin->Index ? "on" : "off");

				if (MyMenu->QuadDropSpin->Index)
					i |= DF_QUAD_DROP;
				else
					i &= ~DF_QUAD_DROP;
			}

			CvarList[CV_DMFLAGS].Set (i);

			if (!!MyMenu->InstantWeaponsSpin->Index != CvarList[CV_INSTANT_WEAPONS].Boolean())
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s instant weapons.\n",
					Player->Client.Persistent.Name.CString(), MyMenu->InstantWeaponsSpin->Index ? "on" : "off");

				CvarList[CV_INSTANT_WEAPONS].Set (MyMenu->InstantWeaponsSpin->Index);
			}

			return true;
		};
	};

	CCTFSettingsMenu	(CPlayerEntity *Player) :
	  CMenu (Player),
	  WeaponsStaySpin (NULL),
	  InstantItemsSpin (NULL),
	  QuadDropSpin (NULL),
	  InstantWeaponsSpin (NULL)
	  {
	  };

	bool Open ()
	{
		sint32 x = 0, y = 0;
		static SSpinControlIndex YesNoValues[] =
		{
			{ "Off", "0" },
			{ "On", "1" },
			{ NULL, NULL},
		};

		CMenu_Image *Background = QNew (TAG_LEVEL) CMenu_Image (this, x, y);
		Background->ImageString = "inventory";
		Background->Width = 256;
		Background->Height = 192;
		Background->Enabled = false;

		y = -76; // Top
		CMenu_Label *TopName = QNew (TAG_LEVEL) CMenu_Label(this, x, y);
		TopName->Enabled = false;
		TopName->Align = LA_CENTER;
		TopName->Flags = LF_GREEN;
		TopName->LabelString = "Settings Menu";

		x = -98;
		y += 8 * 3;

		x += (8 * 16);
		WeaponsStaySpin = QNew (TAG_LEVEL) CMenu_Spin (this, x, y, &YesNoValues[0]);
		WeaponsStaySpin->Index = DeathmatchFlags.dfWeaponsStay.IsEnabled();
		WeaponsStaySpin->x += (8 * 4);
		WeaponsStaySpin->Align = LA_LEFT;

		y += 8;
		InstantItemsSpin = QNew (TAG_LEVEL) CMenu_Spin (this, x, y, &YesNoValues[0]);
		InstantItemsSpin->Index = DeathmatchFlags.dfInstantItems.IsEnabled();
		InstantItemsSpin->x += (8 * 4);
		InstantItemsSpin->Align = LA_LEFT;

		y += 8;
		QuadDropSpin = QNew (TAG_LEVEL) CMenu_Spin (this, x, y, &YesNoValues[0]);
		QuadDropSpin->Index = DeathmatchFlags.dfQuadDrop.IsEnabled();
		QuadDropSpin->x += (8 * 4);
		QuadDropSpin->Align = LA_LEFT;

		y += 8;
		InstantWeaponsSpin = QNew (TAG_LEVEL) CMenu_Spin (this, x, y, &YesNoValues[0]);
		InstantWeaponsSpin->Index = CvarList[CV_INSTANT_WEAPONS].Boolean();
		InstantWeaponsSpin->x += (8 * 4);
		InstantWeaponsSpin->Align = LA_LEFT;

		x = -98;
		y = -76 + 8 * 3; // Top
		CMenu_Label *WeaponsStayLabel = QNew (TAG_LEVEL) CMenu_Label (this, x, y);
		WeaponsStayLabel->Enabled = false;
		WeaponsStayLabel->LabelString = "Weapons Stay";
		WeaponsStayLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *InstantItemsLabel = QNew (TAG_LEVEL) CMenu_Label (this, x, y);
		InstantItemsLabel->Enabled = false;
		InstantItemsLabel->LabelString = "Instant Items";
		InstantItemsLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *QuadDropLabel = QNew (TAG_LEVEL) CMenu_Label (this, x, y);
		QuadDropLabel->Enabled = false;
		QuadDropLabel->LabelString = "Quad Drop";
		QuadDropLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *InstantWeaponsLabel = QNew (TAG_LEVEL) CMenu_Label (this, x, y);
		InstantWeaponsLabel->Enabled = false;
		InstantWeaponsLabel->LabelString = "Instant Weapons";
		InstantWeaponsLabel->Flags = LF_GREEN;

		y += 16;
		CApplyLabel *ApplyLabel = QNew (TAG_LEVEL) CApplyLabel (this, x, y);
		ApplyLabel->Enabled = true;
		ApplyLabel->LabelString = "Apply";
		ApplyLabel->Flags = LF_GREEN;

		y += 8;
		CCloseLabel *CancelLabel = QNew (TAG_LEVEL) CCloseLabel (this, x, y);
		CancelLabel->Enabled = true;
		CancelLabel->LabelString = "Cancel";
		CancelLabel->Flags = LF_GREEN;

		return true;
	};

	void				Close ()
	{
	};

	void				Draw (bool reliable)
	{
		CStatusBar Bar;

		DrawItems(&Bar);
		Bar.SendMsg(Player, reliable);
	};
};

void CTFOpenAdminSettings (CPlayerEntity *Player)
{
	if (Player->Client.Respawn.MenuState.InMenu)
		return;

	Player->Client.Respawn.MenuState.CurrentMenu = QNew (TAG_LEVEL) CCTFSettingsMenu(Player);
	Player->Client.Respawn.MenuState.OpenMenu ();

	Player->Client.Respawn.MenuState.CurrentMenu->Draw (true);
}

class CCTFAdminMenu : public CMenu
{
public:
	CCTFAdminMenu		(CPlayerEntity *Player) :
	  CMenu (Player)
	  {
	  };

	class CCloseLabel : public CMenu_Label
	{
	public:
		CCloseLabel(CCTFAdminMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *Player)
		{
			return true;
		};
	};

	class COpenSettingsLabel : public CMenu_Label
	{
	public:
		COpenSettingsLabel(CCTFAdminMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *Player)
		{
			Player->Client.Respawn.MenuState.CloseMenu();
			CTFOpenAdminSettings (Player);

			return false;
		};
	};

	bool Open ()
	{
		sint32 x = 0, y = 0;

		CMenu_Image *Background = QNew (TAG_LEVEL) CMenu_Image (this, x, y);
		Background->ImageString = "inventory";
		Background->Width = 256;
		Background->Height = 192;
		Background->Enabled = false;

		y = -76; // Top
		CMenu_Label *TopName = QNew (TAG_LEVEL) CMenu_Label(this, x, y);
		TopName->Enabled = false;
		TopName->Align = LA_CENTER;
		TopName->Flags = LF_GREEN;
		TopName->LabelString = "Quake II\nCleanCode Capture the Flag\nAdministration Menu";

		x = -98;
		y += 8 * 5;
		COpenSettingsLabel *SettingsLabel = QNew (TAG_LEVEL) COpenSettingsLabel(this, x, y);
		SettingsLabel->Enabled = true;
		SettingsLabel->Align = LA_LEFT;
		SettingsLabel->LabelString = "Settings";

		y += 8 * 2;
		CCloseLabel *CloseLabel = QNew (TAG_LEVEL) CCloseLabel(this, x, y);
		CloseLabel->Enabled = true;
		CloseLabel->Align = LA_LEFT;
		CloseLabel->LabelString = "Close";

		return true;
	};

	void				Close ()
	{
	};

	void				Draw (bool reliable)
	{
		CStatusBar Bar;

		DrawItems(&Bar);
		Bar.SendMsg(Player, reliable);
	};
};

void CCTFAdminCommand::Execute ()
{
	if (ArgCount() > 1 && CvarList[CV_ADMIN_PASSWORD].StringValue() && *CvarList[CV_ADMIN_PASSWORD].StringValue() &&
		!Player->Client.Respawn.CTF.Admin && strcmp(CvarList[CV_ADMIN_PASSWORD].StringValue(), ArgGets(1).CString()) == 0)
	{
		Player->Client.Respawn.CTF.Admin = true;
		BroadcastPrintf(PRINT_HIGH, "%s has become an admin.\n", Player->Client.Persistent.Name.CString());
		Player->PrintToClient (PRINT_HIGH, "Type 'admin' to access the adminstration menu.\n");

		return;
	}

	if (!Player->Client.Respawn.CTF.Admin)
	{
		CTFBeginElection(Player, ELECT_ADMIN, Player->Client.Persistent.Name + " has requested admin rights.");
		return;
	}

	if (Player->Client.Respawn.MenuState.InMenu)
		return;

	Player->Client.Respawn.MenuState.CurrentMenu = QNew (TAG_LEVEL) CCTFAdminMenu(Player);
	Player->Client.Respawn.MenuState.OpenMenu ();

	Player->Client.Respawn.MenuState.CurrentMenu->Draw (true);
}
#endif
