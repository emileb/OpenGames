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
// cc_hyperblaster.cpp
// Hyperblaster
//

#include "Local.h"
#include "Weapons/WeaponMain.h"

CHyperBlaster::CHyperBlaster() :
IWeaponBase(8, 0, "models/weapons/v_hyperb/tris.md2", 0, 5, 6, 20,
		21, 49, 50, 53)
{
}

bool CHyperBlaster::CanFire (CPlayerEntity *Player)
{
	if (Player->Client.PlayerState.GetGunFrame() >= 6 && Player->Client.PlayerState.GetGunFrame() <= 11)
		return true;
	return false;
}

bool CHyperBlaster::CanStopFidgetting (CPlayerEntity *Player)
{
	return false;
}

vec3f hyperblasterOffsetTable[] =
{
	vec3f( -3.46f, 0, 2.0f	),
	vec3f( -3.46f, 0, -2.0f	),
	vec3f( 0, 0, -4.0f		),
	vec3f( 3.46f, 0, -2.0f	),
	vec3f( 3.46f, 0, 2.0f	),
	vec3f( 0, 0, 4.0f		)
};

void CHyperBlaster::Fire (CPlayerEntity *Player)
{
	Player->Client.WeaponSound = SoundIndex("weapons/hyprbl1a.wav");

	if (!(Player->Client.Buttons & BUTTON_ATTACK))
		Player->Client.PlayerState.GetGunFrame()++;
	else
	{
		if (!Player->Client.Persistent.Inventory.Has(Player->Client.Persistent.Weapon->Item->Ammo) )
		{
			OutOfAmmo(Player);
			NoAmmoWeaponChange (Player);
		}
		else
		{
			const sint32	effect = ((Player->Client.PlayerState.GetGunFrame() == 6) || (Player->Client.PlayerState.GetGunFrame() == 9)) ? FX_HYPERBLASTER : 0,
							damage = (Game.GameMode & GAME_DEATHMATCH) ?
					CalcQuadVal(15)
					:
					CalcQuadVal(20);

			vec3f	start;

			anglef angles = Player->Client.ViewAngle.ToVectors ();
			// I replaced this part with a table because they are constant.
			Player->P_ProjectSource (vec3f(24, 8, Player->ViewHeight-8)
				+
				hyperblasterOffsetTable[Player->Client.PlayerState.GetGunFrame() - 6], angles, start);

			Player->Client.KickOrigin = angles.Forward * -2;
			Player->Client.KickAngles.X = -1;

			CBlasterProjectile::Spawn (Player, start, angles.Forward, damage, 1000, effect, true);

			// send muzzle flash
			Muzzle (Player, MZ_HYPERBLASTER);
			AttackSound (Player);

			Player->PlayerNoiseAt (start, PNOISE_WEAPON);
			DepleteAmmo (Player, 1);

			FireAnimation (Player);
		}

		Player->Client.PlayerState.GetGunFrame()++;
		if (Player->Client.PlayerState.GetGunFrame() == 12 && Player->Client.Persistent.Inventory.Has(Player->Client.Persistent.Weapon->Item->Ammo))
			Player->Client.PlayerState.GetGunFrame() = 6;
	}

	if (Player->Client.PlayerState.GetGunFrame() == 12)
	{
		Player->PlaySound (CHAN_AUTO, SoundIndex("weapons/hyprbd1a.wav"));
		Player->Client.WeaponSound = 0;
	}
}

WEAPON_DEFS (CHyperBlaster);

LINK_ITEM_TO_CLASS (weapon_hyperblaster, CItemEntity);

void CHyperBlaster::CreateItem (CItemList *List)
{
	NItems::HyperBlaster = QNew (TAG_GENERIC) CWeaponItem
		("weapon_hyperblaster", "models/weapons/g_hyperb/tris.md2", FX_ROTATE, "misc/w_pkup.wav",
		"w_hyperblaster", "HyperBlaster", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE,
		&Weapon, NItems::Cells, 1, "#w_hyperblaster.md2");
};

