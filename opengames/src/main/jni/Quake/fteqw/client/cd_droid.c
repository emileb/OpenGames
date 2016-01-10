#include "quakedef.h"

#include "JNITouchControlsUtils.h"

#ifdef _WIN32
//not really needed, but nice none-the-less.
#include "winquake.h"
LONG CDAudio_MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 1;
}
#endif

#define DROID_STOP   0
#define DROID_PLAY   1
#define DROID_PAUSE  2
#define DROID_RESUME 3

JNIEnv* cd_droid_env;

void CDDroid_SetJni(JNIEnv* e)
{
	cd_droid_env = e;
}

void CDAudio_Shutdown(void)
{
}

void CDAudio_Update(void)
{
}

int CDAudio_Init(void)
{

	return 0;
}

void CDAudio_Play(int track, qboolean looping)
{
	Sys_Printf("CDAudio_Play %d %d",track,looping);
	ChangeDroidMusic(DROID_PLAY,track,looping);
}

void CDAudio_Stop(void)
{
	ChangeDroidMusic(DROID_STOP,0,0);
}

void CDAudio_Pause(void)
{
	ChangeDroidMusic(DROID_PAUSE,0,0);
}

void CDAudio_Resume(void)
{
	ChangeDroidMusic(DROID_RESUME,0,0);
}
