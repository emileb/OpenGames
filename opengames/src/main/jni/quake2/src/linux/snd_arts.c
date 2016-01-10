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

// snd_arts.c
// arts support for quake2

#include <artsc.h>

#include "../client/client.h"
#include "../client/snd_loc.h"
#define snd_buf (dma.samples * 2)

static arts_stream_t stream;
static int snd_inited;
static char *buffer;

qboolean SNDDMA_Init(void)
{
	int errorcode;
	int frag_spec;

	if(snd_inited)
		return 1;

	errorcode=arts_init();
	
	if(errorcode)
		Com_Printf ("aRts: %s\n", arts_error_text(errorcode));

	dma.samplebits=(Cvar_Get("sndbits", "16", CVAR_ARCHIVE))->value;

	dma.speed=(Cvar_Get("s_khz", "0", CVAR_ARCHIVE))->value;

	if (dma.speed == 44)
		dma.speed = 44100;
	else if (dma.speed == 22)
		dma.speed = 22050;
	else
		dma.speed = 11025;

	dma.channels=(Cvar_Get("sndchannels", "2", CVAR_ARCHIVE))->value;

	if (dma.speed == 44100)
		dma.samples = (2048 * dma.channels);
	else if (dma.speed == 22050)
		dma.samples = (1024 * dma.channels);
	else
		dma.samples = (512 * dma.channels);

	buffer=malloc(snd_buf);
	memset(buffer, 0, snd_buf);

	for ( frag_spec = 0; (0x01<<frag_spec) < snd_buf; ++frag_spec )
		;
	frag_spec |= 0x00020000;
	stream=arts_play_stream(dma.speed, dma.samplebits, dma.channels, "Q2Stream");
	arts_stream_set(stream, ARTS_P_PACKET_SETTINGS, frag_spec);
	arts_stream_set(stream, ARTS_P_BLOCKING, 0);

	dma.samplepos = 0;
	dma.submission_chunk = 1;
	dma.buffer = buffer;

	snd_inited=1;

	return 1;
}

int SNDDMA_GetDMAPos(void)
{
	if(snd_inited)
		return dma.samplepos;
	else
		Com_Printf ("Sound not inizialized\n");
	return 0;
}

void SNDDMA_Shutdown(void)
{
	if(!snd_inited) {
		Com_Printf ("Sound not inizialized\n");
		return;
	}
	arts_close_stream(stream);
	arts_free();
	snd_inited=0;
	free(dma.buffer);
}

void SNDDMA_BeginPainting (void)
{
}

void SNDDMA_Submit(void)
{
	int written;

	if(!snd_inited)
		return;

	written = arts_write(stream, dma.buffer, snd_buf);
	dma.samplepos+=(written / (dma.samplebits / 8));
}
