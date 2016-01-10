/*
Copyright (C) 1996-1997 Id Software, Inc.

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
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <sys/wait.h>
#include <linux/soundcard.h>
#include <stdio.h>
#include "quakedef.h"

//Updated by Emile Belanger for OpenAL

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <pthread.h>


int audio_fd;
int snd_inited;

static int tryrates[] = { 11025, 22051, 44100, 8000 };

static size_t gAvailableBytes=0;
static bool gSoundMixingStarted;

// Written by the callback function running in an audio thread.
// index in bytes of where we last read.

static volatile size_t gDMAByteIndex;

// The condition is "new data is now available"

static pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

const size_t BYTES_PER_SAMPLE = 2;
const size_t CHANNEL_COUNT = 2;
const size_t BITS_PER_SAMPLE = 8 * BYTES_PER_SAMPLE;

const size_t TOTAL_BUFFER_SIZE = 16 * 1024;

#define ETIMEDOUT 1000

// engine interfaces
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;

// output mix interfaces
static SLObjectItf outputMixObject = NULL;

// buffer queue player interfaces
static SLObjectItf bqPlayerObject = NULL;
static SLPlayItf bqPlayerPlay;
#ifdef ANDROID_NDK
static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
#else
static SLBufferQueueItf bqPlayerBufferQueue;
#endif

static SLEffectSendItf bqPlayerEffectSend;
static SLMuteSoloItf bqPlayerMuteSolo;
static SLVolumeItf bqPlayerVolume;

// pointer and size of the next player buffer to enqueue, and number of remaining buffers
static short *nextBuffer;
static unsigned nextSize;
static int nextCount;

static unsigned char play_buffer[1024];

static size_t min(size_t a, size_t b) {
	return a < b ? a : b;
}

static size_t mod(size_t value, size_t mod) {
	return value % mod;
}

static size_t next(size_t value, size_t mod) {
	value = value + 1;
	if ( value >= mod ) {
		value = 0;
	}
	return value;
}

static size_t prev(size_t value, size_t mod) {
	if ( value <= 0 ) {
		value = mod;
	}
	return value - 1;
}

void assert(bool v,const char * message)
{
	if (!v)
		LOGI("ASSERT: %s",message);
}
static bool gWaitingForMixerToRestart;

// Assumes the mutex is acquired.
// Waits until audio is available or a time period has elapsed.
static bool shouldMixSilence() {
	if (!gSoundMixingStarted) {
		return true;
	}
	while (gAvailableBytes == 0) {
		if (gWaitingForMixerToRestart) {
			return true;
		}
		timeval tp;
		if (gettimeofday(&tp, NULL)) {
			return true;
		}
		const long WAIT_NS = 40 * 1000 * 1000;
		const long NS_PER_SECOND = 1000 * 1000 * 1000;
		timespec ts;
		ts.tv_sec  = tp.tv_sec;
		ts.tv_nsec = tp.tv_usec * 1000 + WAIT_NS;
		if (ts.tv_nsec >= NS_PER_SECOND) {
			ts.tv_nsec -= NS_PER_SECOND;
			ts.tv_sec += 1;
		}
		if (ETIMEDOUT == pthread_cond_timedwait( &condition_cond,  &condition_mutex, &ts)) {
			gWaitingForMixerToRestart = true;
			return true;
		}
	}
	gWaitingForMixerToRestart = false;
	return false;
}

// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
	//LOGI("bqPlayerCallback");
	//assert(bq == bqPlayerBufferQueue,"bqPlayerCallback not equal");
	assert(NULL == context,"Context not null");


	size_t dmaByteIndex = gDMAByteIndex;
	size_t size = sizeof(play_buffer);
	unsigned char* pDestBuffer = play_buffer;

	if (size == 0) return;

	if ( ! shm ) {
		memset(pDestBuffer, 0, size);
		return;
	}

	const unsigned char* pSrcBuffer = shm->buffer;

	while(size > 0) {

		pthread_mutex_lock( &condition_mutex );

		if (shouldMixSilence()) {
			memset(pDestBuffer, 0, size);
			pthread_mutex_unlock( &condition_mutex );
			break;
		}

		size_t chunkSize = min(gAvailableBytes, min(TOTAL_BUFFER_SIZE-dmaByteIndex, size));
		gAvailableBytes -= chunkSize;

		pthread_mutex_unlock( &condition_mutex );

		memcpy(pDestBuffer, pSrcBuffer + dmaByteIndex, chunkSize);
		size -= chunkSize;
		pDestBuffer += chunkSize;
		dmaByteIndex += chunkSize;
		if (dmaByteIndex >= TOTAL_BUFFER_SIZE) {
			dmaByteIndex = 0;
		}
	}

	SLresult result;

	result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, play_buffer, sizeof(play_buffer));
	assert(SL_RESULT_SUCCESS == result,"Enqueue failed");

	gDMAByteIndex = dmaByteIndex;
	asm volatile ("":::"memory");

	/*
	SLresult result;

	for (int n=0;n<gAvailableBytes;n++)
		data[n] = shm->buffer[n];

	gAvailableBytes=0;

	result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, data, gAvailableBytes);
	assert(SL_RESULT_SUCCESS == result,"Enqueue failed");
	return;

	// for streaming playback, replace this test by logic to find and fill the next buffer
	if (--nextCount > 0 && NULL != nextBuffer && 0 != nextSize) {

		// enqueue another buffer
		result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, nextBuffer, nextSize);
		// the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
		// which for this code example would indicate a programming error
		assert(SL_RESULT_SUCCESS == result,"Enqueue failed");
	}
	 */
}



qboolean SNDDMA_Init(void)
{

	int rc;
	int fmt;
	int tmp;
	int i;
	char *s;
	struct audio_buf_info info;
	int caps;

	snd_inited = 0;

	gDMAByteIndex = 0;

	SLresult result;

	// create engine
	result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
	assert(SL_RESULT_SUCCESS == result,"slCreateEngine");

	// realize the engine
	result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result,"Realize");

	// get the engine interface, which is needed in order to create other objects
	result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
	assert(SL_RESULT_SUCCESS == result,"GetInterface");

	// create output mix
	result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, NULL, NULL);
	assert(SL_RESULT_SUCCESS == result,"CreateOutputMix");

	// realize the output mix
	result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result,"Realize output mix");

	//CREATE THE PLAYER

	// configure audio source
	SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 1};
	SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 2, SL_SAMPLINGRATE_11_025,
			SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
			 SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, SL_BYTEORDER_LITTLEENDIAN};
	SLDataSource audioSrc = {&loc_bufq, &format_pcm};

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
	SLDataSink audioSnk = {&loc_outmix, NULL};

	// create audio player
	LOGI("create audio player");
	const SLInterfaceID ids[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
	const SLboolean req[1] = {SL_BOOLEAN_TRUE};
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,
			1, ids, req);
	assert(SL_RESULT_SUCCESS == result,"CreateAudioPlayer");


	// realize the player
	result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result,"Realize AudioPlayer");

	// get the play interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
	assert(SL_RESULT_SUCCESS == result,"GetInterface AudioPlayer");

	// get the buffer queue interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
			&bqPlayerBufferQueue);
	assert(SL_RESULT_SUCCESS == result,"GetInterface buffer queue");

	// register callback on the buffer queue
	result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, NULL);
	assert(SL_RESULT_SUCCESS == result,"RegisterCallback");

	// set the player's state to playing
	result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result,"SetPlayState");


	shm = &sn;


	memset((void*)&sn, 0, sizeof(sn));

	shm->splitbuffer = false;	// Not used.
	shm->samplebits = 16;
	shm->speed = tryrates[0];
	shm->channels = CHANNEL_COUNT;
	shm->samples = TOTAL_BUFFER_SIZE / BYTES_PER_SAMPLE;
	shm->samplepos = 0; // Not used.
	shm->buffer = (unsigned char*) Hunk_AllocName(TOTAL_BUFFER_SIZE, (char*) "shmbuf");
	shm->submission_chunk = 1; // Not used.

	shm->soundalive = true;


	snd_inited = 1;

	result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, "\0", 1);
	assert(SL_RESULT_SUCCESS == result,"Enqueue first buffer");


	return 1;

}

int SNDDMA_GetDMAPos(void)
{
	//LOGI("SNDDMA_GetDMAPos");

	if (!snd_inited) return 0;

	int dmaPos = gDMAByteIndex / BYTES_PER_SAMPLE;
	 asm volatile ("":::"memory");
	return dmaPos;

}

void SNDDMA_ReportWrite(size_t lengthBytes) {
	//LOGI("SNDDMA_ReportWrite %d",lengthBytes);
	pthread_mutex_lock( &condition_mutex );
	gSoundMixingStarted = true;
	if (gAvailableBytes == 0) {
		pthread_cond_signal( &condition_cond );
	}
	gAvailableBytes += lengthBytes;
	pthread_mutex_unlock( &condition_mutex );
}

void SNDDMA_Shutdown(void)
{
	if (snd_inited)
	{
		close(audio_fd);
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
}

