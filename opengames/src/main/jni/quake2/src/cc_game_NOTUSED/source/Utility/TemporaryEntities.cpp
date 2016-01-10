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
// cc_tent.cpp
// A huge class containing containers to spawn temp entities with a single function call.
//

#include "Local.h"
#include "Utility/TemporaryEntities.h"

const CTempEntFlags CTempEntFlags::DefaultTempEntFlags (CAST_MULTI, CASTFLAG_PVS, vec3fOrigin);

void CForEachPlayerMulticastCallback::Query (bool MustBeInUse)
{
	for (uint8 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);

		if (MustBeInUse && (!Player->GetInUse() || Player->Client.Persistent.State != SVCS_SPAWNED))
			continue;

		Index = i;
		if (Callback (Player))
			TempEnt->SendTo (Player);
	}
};

void CTempEnt::Send ()
{
	if (Flags.Type == CAST_UNI)
		SendTo (Player);
	else
	{
		SendData ();
		
		Cast (Flags.Type, Flags.Flags, Origin, NULL, true);
	}
};

void CTempEnt::SendTo (CPlayerEntity *Player)
{
	SendData ();

	Cast (CAST_UNI, Flags.Flags, (ToPlayerOrigin && Player) ? Player->State.GetOrigin() : Origin, Player, true);
};

void CTempEnt::SendHeader (ESVCType Enum)
{
	WriteByte (SVC_TEMP_ENTITY);
	WriteByte (Enum);
};

void CRocketExplosion::SendData ()
{
	SendHeader (Water ? TE_ROCKET_EXPLOSION_WATER : (Particles ? TE_ROCKET_EXPLOSION : TE_EXPLOSION1_NP));
	WritePosition (ExplosionOrigin);
};

void CGrenadeExplosion::SendData ()
{
	SendHeader (Water ? TE_GRENADE_EXPLOSION_WATER : TE_GRENADE_EXPLOSION);
	WritePosition (ExplosionOrigin);
};

void CBFGExplosion::SendData ()
{
	SendHeader (Big ? TE_BFG_BIGEXPLOSION : TE_BFG_EXPLOSION);
	WritePosition (ExplosionOrigin);
};

void CPlasmaExplosion::SendData ()
{
	SendHeader (TE_PLASMA_EXPLOSION);
	WritePosition (ExplosionOrigin);
};

void CTrackerExplosion::SendData ()
{
	SendHeader (TE_TRACKER_EXPLOSION);
	WritePosition (ExplosionOrigin);
};

void CNukeExplosion::SendData ()
{
	SendHeader (TE_NUKEBLAST);
	WritePosition (ExplosionOrigin);
};

void CGunshotRicochet::SendData ()
{
	SendHeader (TE_GUNSHOT);
	WritePosition (SplashOrigin);
	WriteDirection (SplashNormal ? SplashNormal : vec3fOrigin);
};

void CShotgunRicochet::SendData ()
{
	SendHeader (TE_SHOTGUN);
	WritePosition (SplashOrigin);
	WriteDirection (SplashNormal ? SplashNormal : vec3fOrigin);
};

void CBlood::SendData ()
{
	SendHeader (Type);
	WritePosition (SplashOrigin);
	WriteDirection (SplashNormal ? SplashNormal : vec3fOrigin);
};

void CBlasterSplash::SendData ()
{
	SendHeader (Type);
	WritePosition (SplashOrigin);

	// ...
	// ID, you impress me.
	if (Type == BL_BLUE_HYPERBLASTER)
		WritePosition (SplashNormal ? SplashNormal : vec3fOrigin);
	else
		WriteDirection (SplashNormal ? SplashNormal : vec3fOrigin);
};

void CSplash::SendData ()
{
	SendHeader (TE_SPLASH);
	WriteByte (Amount);
	WritePosition (SplashOrigin);
	WriteDirection (SplashNormal ? SplashNormal : vec3fOrigin);
	WriteByte (Color);
};

void CSparks::SendData ()
{
	SendHeader (Type);

	if (Type != ST_LASER_SPARKS && Type != ST_WELDING_SPARKS && Type != ST_TUNNEL_SPARKS)
	{
		WritePosition (SplashOrigin);
		WriteDirection (SplashNormal ? SplashNormal : vec3fOrigin);
	}
	else
	{
		WriteByte (Amount);
		WritePosition (SplashOrigin);
		WriteDirection (SplashNormal ? SplashNormal : vec3fOrigin);
		WriteByte (Color);
	}
};

void CShieldSparks::SendData ()
{
	SendHeader (Screen ? TE_SCREEN_SPARKS : TE_SHIELD_SPARKS);
	WritePosition (SplashOrigin);
	WriteDirection (SplashNormal ? SplashNormal : vec3fOrigin);
};

void CSteam::SendData ()
{
	SendHeader (TE_STEAM);

	WriteShort (ID);
	WriteByte (Count);
	WritePosition (SplashOrigin);
	WriteDirection (SplashNormal);
	WriteByte (Color);
	WriteShort (Magnitude);

	if (ID != -1)
		WriteLong (EndTime);
};

void CHeatSteam::SendData ()
{
	SendHeader (TE_HEATBEAM_STEAM);

	WritePosition (SplashOrigin);
	WriteDirection (SplashNormal ? SplashNormal : vec3fOrigin);
};

void CChainfistSmoke::SendData ()
{
	SendHeader (TE_CHAINFIST_SMOKE);

	WritePosition (ExplosionOrigin);
};

void CTrailBase::SendTrailBase ()
{
	if (EntityNumber != -1)
		WriteShort (EntityNumber);
	WritePosition (TrailStart);
	WritePosition (TrailEnd);
};

void CRailTrail::SendData ()
{
	SendHeader (TE_RAILTRAIL);
	SendTrailBase ();
};

void CBubbleTrail::SendData ()
{
	SendHeader (TE_BUBBLETRAIL);
	SendTrailBase ();
};

void CFleshCable::SendData ()
{
	SendHeader (TE_PARASITE_ATTACK);
	SendTrailBase ();
};

void CBFGLaser::SendData ()
{
	SendHeader (TE_BFG_LASER);
	SendTrailBase ();
};

void CGrappleCable::SendData ()
{
	SendHeader (TE_GRAPPLE_CABLE);
	SendTrailBase ();
	WritePosition (Offset);
};

void CLightning::SendData ()
{
	SendHeader (TE_LIGHTNING);

	WriteShort (EntityNumber);
	WriteShort (DestinationEntity);

	WritePosition (TrailStart);
	WritePosition (TrailEnd);
};

void CHeatBeamLaser::SendData ()
{
	SendHeader (Monster ? TE_MONSTER_HEATBEAM : TE_HEATBEAM);
	SendTrailBase ();
};

void CDebugTrail::SendData ()
{
	SendHeader (TE_DEBUGTRAIL);
	SendTrailBase ();
};

void CForceWall::SendData ()
{
	SendHeader (TE_FORCEWALL);
	SendTrailBase ();
	WriteByte (Color);
};

void CFlashLight::SendData ()
{
	SendHeader (TE_FLASHLIGHT);
	WritePosition (ExplosionOrigin);
	WriteShort (EntityNumber);
};

void CBossTeleport::SendData ()
{
	SendHeader (TE_BOSSTPORT);
	WritePosition (ExplosionOrigin);
};

void CTeleportEffect::SendData ()
{
	SendHeader (TE_TELEPORT_EFFECT);
	WritePosition (ExplosionOrigin);
};

void CWidowBeamOut::SendData ()
{
	SendHeader (TE_WIDOWBEAMOUT);
	WriteShort (ID);
	WritePosition(ExplosionOrigin);
};

void CWidowSplash::SendData ()
{
	SendHeader (TE_WIDOWSPLASH);
	WritePosition(ExplosionOrigin);
};

void CMuzzleFlash::SendData ()
{
	WriteByte ((Monster) ? SVC_MUZZLEFLASH2 : SVC_MUZZLEFLASH);
	WriteShort (EntityNumber);
	WriteByte (Flash);
};
