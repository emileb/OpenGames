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
// cc_debugweapons.cpp
// 
//

#include "Local.h"
#include "Weapons/WeaponMain.h"

CDebugWeapon::CDebugWeapon () :
IWeaponBase (11, 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL)
{
};

void CDebugWeapon::Think (CPlayerEntity *Player)
{
	// Update pointing surface
	//if (!(Level.Frame & 2))
	{
		vec3f end = Player->State.GetOrigin().MultiplyAngles(8192, Player->Client.ViewAngle.ToVectors ().Forward);
		CTrace tr (Player->State.GetOrigin(), end, Player, CONTENTS_MASK_SOLID|CONTENTS_MASK_WATER);

		if (tr.Fraction < 1 && tr.Surface)
			ConfigString (CS_POINTING_SURFACE, tr.Surface->Name, Player);

		tr (Player->State.GetOrigin(), end, Player, CONTENTS_MASK_SHOT|CONTENTS_MASK_WATER);

		if (tr.Fraction < 1 && tr.Entity && !tr.Entity->ClassName.IsNullOrEmpty())
		{
			if (!tr.Surface)
				ConfigString (CS_POINTING_SURFACE-1, const_cast<char*>(tr.Entity->ClassName.CString()), Player);
			else
				ConfigString (CS_POINTING_SURFACE-1, (tr.Entity->ClassName + " (" + tr.Surface->Name + ")").CString(), Player);
		}
	}

	if (Player->Client.NewWeapon)
		ChangeWeapon (Player);
	else if ((Player->Client.Buttons|Player->Client.LatchedButtons) & BUTTON_ATTACK)
	{
		Player->Client.LatchedButtons &= ~BUTTON_ATTACK;

		// This here is ugly, but necessary so that machinegun/chaingun/hyperblaster
		// get the right acceptance on first-frame-firing
		Player->Client.Buttons |= BUTTON_ATTACK;

		Fire(Player);
	}
}

CSurfacePicker::CSurfacePicker () :
CDebugWeapon ()
{
};

CSurfacePicker CSurfacePicker::Weapon;

void CSurfacePicker::Fire (CPlayerEntity *Player)
{
};

void CSurfacePicker::AddWeaponToItemList (CItemList *List)
{
	if (CvarList[CV_MAP_DEBUG].Boolean())
		CreateItem (List);
};

void CSurfacePicker::InitWeaponVwepModel (sint32 TakeAway)
{
};

void CSurfacePicker::CreateItem (CItemList *List)
{
	if (CvarList[CV_MAP_DEBUG].Boolean())
		QNew (TAG_GENERIC) CWeaponItem (NULL, NULL, 0, NULL, NULL, "Surface Picker", ITEMFLAG_WEAPON|ITEMFLAG_USABLE, &Weapon, NULL, 0, NULL);
};

