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
// cc_sound.cpp
// Sound replacements
//

#include "Local.h"

/*
==================
SV_StartSound

Each entity can have eight independant sound sources, like voice,
weapon, feet, etc.

If channel & 8, the sound will be sent to everyone, not just
things in the PHS.

If entity isn't in the PHS, they must be forced to be sent or
have the origin explicitly sent.

Channel 0 is an auto-allocate channel, the others override anything
already running on that entity/channel pair.

An attenuation of 0 will play full volume everywhere in the Level.
Larger attenuations will drop off.  (max 4 attenuation)

timeOffset can range from 0.0 to 0.1 to cause sounds to be started
later in the frame than they normally would.

If origin is NULL, the origin is determined from the entity origin
or the midpoint of the entity box for bmodels.
==================
*/
const int 	SOUND_FULLVOLUME	= 80;

// sound communication
// a sound without an ent or pos will be a local only sound
enum
{
	SND_VOLUME			= BIT(0),	// a uint8
	SND_ATTENUATION		= BIT(1),	// a uint8
	SND_POS				= BIT(2),	// three coordinates
	SND_ENT				= BIT(3),	// a sint16 0-2: channel, 3-12: entity
	SND_OFFSET			= BIT(4)	// a uint8, msec offset from frame start
};

const uint8 DEFAULT_SOUND_PACKET_VOLUME			= 255;
const float DEFAULT_SOUND_PACKET_ATTENUATION	= 1.0;

static void SV_StartSound (vec3f origin, IBaseEntity *Entity, ESoundChannel channel, MediaIndex soundIndex, uint8 vol, EAttenuation attenuation, uint8 timeOffset, bool Positioned)
{
	if (!Positioned && !Entity)
	{
		DebugPrint ("CleanCode SV_StartSound: Not positioned and no entity!\n");
		return;
	}

	if (vol < 0 || vol > 255)
	{
		DebugPrintf ("CleanCode SV_StartSound: volume = %f", vol);
		return;
	}

	if (attenuation < 0 || attenuation > 4)
	{
		DebugPrintf ("CleanCode SV_StartSound: attenuation = %f", attenuation);
		return;
	}

	if (timeOffset < 0 || timeOffset > 65)
	{
		DebugPrintf ("CleanCode SV_StartSound: timeOffset = %f", timeOffset);
		return;
	}

	// No PHS flag
	bool usePHS = true;

	if ((channel & 8) || attenuation == ATTN_NONE)
	{
		// If the sound doesn't attenuate, send it to everyone (global radio chatter, voiceovers, etc)
		usePHS = false;
		channel &= 7;
	}

	sint32 sendChan = (Entity->State.GetNumber() << 3) | (channel & 7);

	sint32 flags = 0;
	if (vol != DEFAULT_SOUND_PACKET_VOLUME)
		flags |= SND_VOLUME;
	if (attenuation != DEFAULT_SOUND_PACKET_ATTENUATION)
		flags |= SND_ATTENUATION;

	/*
	** the client doesn't know that bmodels have weird origins
	** the origin can also be explicitly set
	*/
	if ((Entity->GetSvFlags() & SVF_NOCLIENT) || (Entity->GetSolid() == SOLID_BSP) || Positioned)
		flags |= SND_POS;

	// always send the entity number for channel overrides
	flags |= SND_ENT;

	if (timeOffset)
		flags |= SND_OFFSET;

	// use the entity origin/bmodel origin if the origin is specified
	if (!Positioned)
		origin = (Entity->GetSolid() == SOLID_BSP) ? Entity->State.GetOrigin() + 0.5f * (Entity->GetMins() + Entity->GetMaxs()) : Entity->State.GetOrigin();

	// Cycle through the different targets and do attenuation calculations
	for (sint32 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (Player->Client.Persistent.State == SVCS_FREE)
			continue;

		if (Player->Client.Persistent.State != SVCS_SPAWNED && !(channel & CHAN_RELIABLE))
			continue;

		if (usePHS)
		{
			// Not hearable from here
			if (!InHearableArea (Player->State.GetOrigin(), origin))
				continue;
		}

		vec3f sourceVec = origin - Player->State.GetOrigin();
		float distanceMult = (float)attenuation * ((attenuation == ATTN_STATIC) ? 0.001f : 0.0005f);

		float dist = sourceVec.Normalize () - SOUND_FULLVOLUME;
		if (dist < 0)
			dist = 0;			// close enough to be at full volume
		dist *= distanceMult;	// different attenuation levels

		anglef angles = Player->State.GetAngles().ToVectors ();
		float dot = angles.Right | sourceVec;

		// no attenuation = no spatialization
		float rightScale = 1.0f, leftScale = 1.0f;

		if (distanceMult)
		{
			rightScale = 0.5f * (1.0f + dot);
			leftScale = 0.5f * (1.0f - dot);
		}

		// add in distance effect
		float rightVol = ((static_cast<float>(vol) / 255) * ((1.0f - dist) * rightScale));
		float leftVol = ((static_cast<float>(vol) / 255) * ((1.0f - dist) * leftScale));

		if (rightVol <= 0 && leftVol <= 0)
			continue; // silent

		WriteByte (SVC_SOUND);
		WriteByte (flags);
		WriteByte (soundIndex);

		if (flags & SND_VOLUME)
			WriteByte (vol);

		if (flags & SND_ATTENUATION)
			WriteByte (attenuation*64);

		if (flags & SND_OFFSET)
			WriteByte (timeOffset * (1000 / 255));

		if (flags & SND_ENT)
			WriteShort (sendChan);

		if (flags & SND_POS)
			WritePosition (origin);

		Cast ((channel & CHAN_RELIABLE) ? CASTFLAG_RELIABLE : CASTFLAG_UNRELIABLE, Player);
	}
}

void PlaySoundFrom (IBaseEntity *Entity, ESoundChannel channel, MediaIndex soundIndex, uint8 volume, EAttenuation attenuation, uint8 timeOfs)
{
	SV_StartSound (vec3fOrigin, Entity, channel, soundIndex, volume, attenuation, timeOfs, false);
}

void PlaySoundAt (vec3f origin, IBaseEntity *Entity, ESoundChannel channel, MediaIndex soundIndex, uint8 volume, EAttenuation attenuation, uint8 timeOfs)
{
	SV_StartSound (origin, Entity, channel, soundIndex, volume, attenuation, timeOfs, true);
}
