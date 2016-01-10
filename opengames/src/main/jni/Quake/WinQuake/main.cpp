/* //device/apps/Quake/quake/src/QW/client/main.c
**
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
**
**     http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License.
*/

#include <stdio.h>
#include <assert.h>


#include <GLES/gl.h>


#include <quakedef.h>

// Timer utilities

#define USE_LOG

#ifdef ENABLE_PMP

static nsecs_t baseTime;
static nsecs_t lastTime;
static const unsigned int kStartTimeStackSize = 100;
static nsecs_t startTimes[kStartTimeStackSize];
static unsigned int startTimeStackPointer;

static
void PMP_Common(const char* fmt, va_list ap, char type)
{
	char buf[1024];
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	
	// Note: Timer acually has less than microsecond resolution, so track time in microseconds:
	
	nsecs_t time = systemTime(SYSTEM_TIME_THREAD) / 1000;
	if(baseTime == 0)
	{
		baseTime = time;
	}
	time -= baseTime;
	switch(type)
	{
	case '<':
		{
			if(startTimeStackPointer < kStartTimeStackSize)
			{
				startTimes[startTimeStackPointer] = time;
			}
#ifdef USE_LOG
			LOGI("< %lld [%d] %s\n", time, startTimeStackPointer, buf);
#else
			fprintf(stderr, "Quake < %lld %d %s\n", time, startTimeStackPointer, buf);
#endif
			startTimeStackPointer++;
		}
		break;
	case '>':
		{
			nsecs_t elapsed = 0;
			if(startTimeStackPointer > 0)
			{
				--startTimeStackPointer;
				if(startTimeStackPointer < kStartTimeStackSize)
				{
					elapsed = time - startTimes[startTimeStackPointer];
				}
			}
#ifdef USE_LOG
			LOGI("> %lld [%d] %lld %s\n", time, startTimeStackPointer, elapsed, buf);
#else
			fprintf(stderr, "Quake > %lld [%d] %lld %s\n", time, startTimeStackPointer, elapsed, buf);
#endif
		}
		break;
	default:
#ifdef USE_LOG
		LOGI("= %lld %lld %s\n", time, time - lastTime, buf);
#else
		fprintf(stderr, "Quake = %lld %s\n", time, buf);
#endif
		break;
	}
	lastTime = time;
}

void PMP_Begin(const char* fmt,...)
{
	va_list ap;
	va_start(ap, fmt);
	PMP_Common(fmt, ap, '<');
}

void PMP_Event(const char* fmt,...)
{
	va_list ap;
	va_start(ap, fmt);
	PMP_Common(fmt, ap, '=');
}

void PMP_End(const char* fmt,...)
{
	va_list ap;
	va_start(ap, fmt);
	PMP_Common(fmt, ap, '>');
}

#endif // ENABLE_PMP
