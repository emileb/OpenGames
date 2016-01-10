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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stropts.h>
#include <sys/types.h>
#include <sys/audioio.h>

#include "../client/client.h"
#include "../client/snd_loc.h"

#define	SND_DEBUG	0

#if	SND_DEBUG
#define	DPRINTF(...)	printf(__VA_ARGS__)
#else
#define	DPRINTF(...)	/**/
#endif

static int audio_fd = -1;
static int snd_inited;

static cvar_t *sndbits;
static cvar_t *sndspeed;
static cvar_t *sndchannels;
static cvar_t *snddevice;


static int tryrates[] = { 11025, 22051, 44100, 8000 };

#define	QSND_NUM_CHUNKS	2

/*
==================
SNDDMA_Init

Try to find a sound device to mix for.
Returns false if nothing is found.
Returns true and fills in the "dma" structure with information for the mixer.
==================
*/
qboolean SNDDMA_Init(void)
{
    int i;
    int samples;
    audio_info_t au_info;

    if (snd_inited)
	return 1;

    if (!snddevice) {
	sndbits = Cvar_Get("sndbits", "16", CVAR_ARCHIVE);
	sndspeed = Cvar_Get("sndspeed", "0", CVAR_ARCHIVE);
	sndchannels = Cvar_Get("sndchannels", "2", CVAR_ARCHIVE);
	snddevice = Cvar_Get("snddevice", "/dev/audio", CVAR_ARCHIVE);
    }

// open /dev/audio

    if (audio_fd < 0) {

	audio_fd = open(snddevice->string, O_WRONLY);

	if (audio_fd < 0) {
	    Com_Printf("Could not open %s: %s\n", snddevice->string, strerror(errno));
	    return 0;
		}
	}

// set sample bits & speed

    if ((int)sndspeed->value > 0) {
	AUDIO_INITINFO(&au_info);

	au_info.play.precision = (int)sndbits->value;
	au_info.play.encoding =
	    ( au_info.play.precision == 8
	      ? AUDIO_ENCODING_LINEAR8
	      : AUDIO_ENCODING_LINEAR ); 
	au_info.play.sample_rate = (int)sndspeed->value;
	au_info.play.channels = (int)sndchannels->value;

	if (ioctl(audio_fd, AUDIO_SETINFO, &au_info) == -1) {
	    Com_Printf("AUDIO_SETINFO failed: %s\n", strerror(errno));
		return 0;
	}
    } else {
	for (i=0 ; i<sizeof(tryrates)/sizeof(tryrates[0]) ; i++) {
	    AUDIO_INITINFO(&au_info);

	    au_info.play.precision = (int)sndbits->value;
	    au_info.play.encoding =
		( au_info.play.precision == 8
		  ? AUDIO_ENCODING_LINEAR8
		  : AUDIO_ENCODING_LINEAR ); 
	    au_info.play.sample_rate = tryrates[i];
	    au_info.play.channels = (int)sndchannels->value;

	    if (ioctl(audio_fd, AUDIO_SETINFO, &au_info) == 0)
		break;

	    Com_Printf("AUDIO_SETINFO failed: %s\n", strerror(errno));
	}
	if (i >= sizeof(tryrates)/sizeof(tryrates[0]))
		return 0;
	}
    dma.samplebits = au_info.play.precision;
    dma.channels = au_info.play.channels;
    dma.speed = au_info.play.sample_rate;
	
    /*
     * submit some sound data every ~ 0.1 seconds, and try to buffer 2*0.1 
     * seconds in sound driver
     */
    samples = dma.channels * dma.speed / 10;
    for (i = 0; (1 << i) < samples; i++)
	;
    dma.submission_chunk = 1 << (i-1);
    DPRINTF("channels %d, speed %d, log2(samples) %d, submission chunk %d\n",
	    dma.channels, dma.speed, i-1,
	    dma.submission_chunk);

    dma.samples = QSND_NUM_CHUNKS * dma.submission_chunk;
    dma.buffer = calloc(dma.samples, dma.samplebits/8);
    if (dma.buffer == NULL) {
	Com_Printf("Could not alloc sound buffer\n");
	return 0;
    }
	
    AUDIO_INITINFO(&au_info);
    au_info.play.eof = 0;
    au_info.play.samples = 0;
    ioctl(audio_fd, AUDIO_SETINFO, &au_info);

    dma.samplepos = 0;

	snd_inited = 1;
	
	return 1;
}

/*
==============
SNDDMA_GetDMAPos

return the current sample position (in mono samples, not stereo)
inside the recirculating dma buffer, so the mixing code will know
how many sample are required to fill it up.
===============
*/
int SNDDMA_GetDMAPos(void)
{
    int s_pos;
    audio_info_t au_info;

    if (!snd_inited) 
	return 0;

    if (ioctl(audio_fd, AUDIO_GETINFO, &au_info) == -1) {
	Com_Printf("AUDIO_GETINFO failed: %s\n", strerror(errno));
	return 0;
 	}

    s_pos = au_info.play.samples * dma.channels;
    return s_pos & (dma.samples - 1);
}

/*
==============
SNDDMA_Shutdown

Reset the sound device for exiting
===============
*/
void SNDDMA_Shutdown(void)
{
    if (snd_inited) {
	if (audio_fd >= 0) {
	    ioctl(audio_fd, I_FLUSH, FLUSHW);
	    close(audio_fd);
	    audio_fd = -1;
		}
	snd_inited = 0;
	}
}

/*
==============
SNDDMA_Submit
	
Send sound to device if buffer isn't really the dma buffer
===============
*/
void SNDDMA_Submit(void)
{
    int samplebytes = dma.samplebits/8;
    audio_info_t au_info;
    int s_pos;
    int chunk_idx;
    static int last_chunk_idx = -1;

	if (!snd_inited)
	return;
    
    if (last_chunk_idx == -1) {
	if (write(audio_fd, dma.buffer, dma.samples * samplebytes) != dma.samples * samplebytes)
	    Com_Printf("initial write on audio device failed\n");
	last_chunk_idx = 0;
	dma.samplepos = 0;
	return;
    }

    if (ioctl(audio_fd, AUDIO_GETINFO, &au_info) == -1) {
	Com_Printf("AUDIO_GETINFO failed: %s\n", strerror(errno));
	return;
    }

    if (au_info.play.error) {

	/*
	 * underflow? clear the error flag and reset the HW sample counter
	 * and send the whole dma_buffer, to get sound output working again
	 */

	DPRINTF("audio data underflow\n");

	AUDIO_INITINFO(&au_info);
	au_info.play.error = 0;
	au_info.play.samples = 0;
	ioctl(audio_fd, AUDIO_SETINFO, &au_info);

	if (write(audio_fd, dma.buffer, dma.samples * samplebytes) != dma.samples * samplebytes)
	    Com_Printf("refill sound driver after underflow failed\n");
	last_chunk_idx = 0;
	dma.samplepos = 0;
	return;
    }

    s_pos = au_info.play.samples * dma.channels;
    chunk_idx = (s_pos % dma.samples) / dma.submission_chunk;
 
    DPRINTF("HW DMA Pos=%u (%u), dma.samplepos=%u, play in=%d, last=%d\n",
	    au_info.play.samples, s_pos, dma.samplepos,
	    chunk_idx, last_chunk_idx);

    while (chunk_idx != last_chunk_idx) {
	
	if (write(audio_fd,
		  dma.buffer + dma.samplepos * samplebytes,
		  dma.submission_chunk * samplebytes) != dma.submission_chunk * samplebytes) {
	    Com_Printf("write error on audio device\n");
	}

	if ((dma.samplepos += dma.submission_chunk) >= dma.samples)
	    dma.samplepos = 0;

	if (++last_chunk_idx >= QSND_NUM_CHUNKS)
	    last_chunk_idx = 0;
    }
}


void SNDDMA_BeginPainting (void)
{
}

