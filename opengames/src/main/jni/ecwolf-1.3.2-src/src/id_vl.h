// ID_VL.H

#ifndef __ID_VL_H__
#define __ID_VL_H__

// wolf compatability

void Quit (const char *error,...);

//===========================================================================

extern SDL_Surface *curSurface;

extern  bool	fullscreen, usedoublebuffering;
extern  unsigned screenWidth, screenHeight, screenBits, curPitch;
extern  unsigned scaleFactorX, scaleFactorY;
extern	float	screenGamma;

extern	bool  screenfaded;

extern SDL_Color gamepal[256];

//===========================================================================

//
// VGA hardware routines
//

#define VL_WaitVBL(a) SDL_Delay((a)*8)

void VL_ReadPalette(const char* lump);

void VL_SetVGAPlaneMode (bool forSignon=false);
void VL_SetTextMode (void);

void VL_FadeOut     (int start, int end, int red, int green, int blue, int steps);
void VL_FadeIn      (int start, int end, int steps);

byte *VL_LockSurface();
void VL_UnlockSurface();

#include "v_video.h"
#define VL_ClearScreen(color) VWB_Clear(color, 0, 0, SCREENWIDTH, SCREENHEIGHT)

#endif
