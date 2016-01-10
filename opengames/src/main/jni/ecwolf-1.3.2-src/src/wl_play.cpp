// WL_PLAY.C

#include "c_cvars.h"
#include "wl_def.h"
#include "wl_menu.h"
#include "id_ca.h"
#include "id_sd.h"
#include "id_vl.h"
#include "id_vh.h"
#include "id_us.h"

#include "wl_cloudsky.h"
#include "wl_shade.h"
#include "language.h"
#include "lumpremap.h"
#include "thinker.h"
#include "actor.h"
#include "textures/textures.h"
#include "wl_agent.h"
#include "wl_debug.h"
#include "wl_draw.h"
#include "wl_game.h"
#include "wl_inter.h"
#include "wl_play.h"
#include "g_mapinfo.h"
#include "a_inventory.h"
#include "am_map.h"

/*
=============================================================================

												LOCAL CONSTANTS

=============================================================================
*/

#define sc_Question     0x35

/*
=============================================================================

												GLOBAL VARIABLES

=============================================================================
*/

bool madenoise;              // true when shooting or screaming

exit_t playstate;

static int DebugOk;
#ifdef __ANDROID__
extern bool ShadowingEnabled;
#endif

bool noclip, ammocheat, mouselook = false;
int godmode, singlestep;
bool notargetmode = false;
unsigned int extravbls = 0; // to remove flicker (gray stuff at the bottom)

//
// replacing refresh manager
//
bool noadaptive = false;
unsigned tics;

//
// control info
//
#define JoyAx(x) (32+(x<<1))
ControlScheme controlScheme[] =
{
	{ bt_moveforward,		"Forward",		JoyAx(1),	sc_UpArrow,		-1, &controly, 1 },
	{ bt_movebackward,		"Backward",		JoyAx(1)+1,	sc_DownArrow,	-1, &controly, 0 },
	{ bt_strafeleft,		"Strafe Left",	JoyAx(0),	sc_Comma,		-1, &controlstrafe, 1 },
	{ bt_straferight,		"Strafe Right",	JoyAx(0)+1,	sc_Peroid,		-1, &controlstrafe, 0 },
	{ bt_turnleft,			"Turn Left",	JoyAx(3),	sc_LeftArrow,	-1, &controlx, 1 },
	{ bt_turnright,			"Turn Right",	JoyAx(3)+1,	sc_RightArrow,	-1, &controlx, 0 },
	{ bt_attack,			"Attack",		0,			sc_Control,		0,  NULL, 0},
	{ bt_strafe,			"Strafe",		3,			sc_Alt,			-1, NULL, 0 },
	{ bt_run,				"Run",			2,			sc_LShift,		-1, NULL, 0 },
	{ bt_use,				"Use",			1,			sc_Space,		-1, NULL, 0 },
	{ bt_slot1,				"Slot 1",		-1,			sc_1,			-1, NULL, 0 },
	{ bt_slot2,				"Slot 2", 		-1,			sc_2,			-1, NULL, 0 },
	{ bt_slot3,				"Slot 3",		-1,			sc_3,			-1, NULL, 0 },
	{ bt_slot4,				"Slot 4",		-1,			sc_4,			-1, NULL, 0 },
	{ bt_slot5,				"Slot 5",		-1,			sc_5,			-1, NULL, 0 },
	{ bt_slot6,				"Slot 6",		-1,			sc_6,			-1, NULL, 0 },
	{ bt_slot7,				"Slot 7",		-1,			sc_7,			-1, NULL, 0 },
	{ bt_slot8,				"Slot 8",		-1,			sc_8,			-1, NULL, 0 },
	{ bt_slot9,				"Slot 9",		-1,			sc_9,			-1, NULL, 0 },
	{ bt_slot0,				"Slot 0",		-1,			sc_0,			-1, NULL, 0 },
	{ bt_nextweapon,		"Next Weapon",	4,			-1,				-1, NULL, 0 },
	{ bt_prevweapon,		"Prev Weapon",	5, 			-1,				-1, NULL, 0 },
	{ bt_altattack,			"Alt Attack",	-1,			-1,				-1, NULL, 0 },
	{ bt_reload,			"Reload",		-1,			-1,				-1, NULL, 0 },
	{ bt_zoom,				"Zoom",			-1,			-1,				-1, NULL, 0 },
	{ bt_automap,			"Automap",		-1,			-1,				-1, NULL, 0 },
	{ bt_showstatusbar,		"Show Status",	-1,			sc_Tab,			-1,	NULL, 0 },

	// End of List
	{ bt_nobutton,			NULL, -1, -1, -1, NULL, 0 }
};
ControlScheme &schemeAutomapKey = controlScheme[25]; // When the input system is redone, hopefully we don't need this kind of thing

ControlScheme amControlScheme[] =
{
	{ bt_zoomin,			"Zoom In",		JoyAx(2),	sc_Equals,		-1, NULL, 0 },
	{ bt_zoomout,			"Zoom Out",		JoyAx(2)+1,	sc_Minus,		-1, NULL, 0 },
	{ bt_panup,				"Pan Up",		JoyAx(1),	sc_UpArrow,		-1, &controlpany, 0 },
	{ bt_pandown,			"Pan Down",		JoyAx(1)+1,	sc_DownArrow,	-1, &controlpany, 1 },
	{ bt_panleft,			"Pan Left",		JoyAx(0),	sc_LeftArrow,	-1, &controlpanx, 0 },
	{ bt_panright,			"Pan Right",	JoyAx(0)+1,	sc_RightArrow,	-1, &controlpanx, 1 },

	{ bt_nobutton,			NULL, -1, -1, -1, NULL, 0 }
};

void ControlScheme::setKeyboard(ControlScheme* scheme, Button button, int value)
{
	for(int i = 0;scheme[i].button != bt_nobutton;i++)
	{
		if(scheme[i].keyboard == value)
			scheme[i].keyboard = -1;
		if(scheme[i].button == button)
			scheme[i].keyboard = value;
	}
}

void ControlScheme::setJoystick(ControlScheme* scheme, Button button, int value)
{
	for(int i = 0;scheme[i].button != bt_nobutton;i++)
	{
		if(scheme[i].joystick == value)
			scheme[i].joystick = -1;
		if(scheme[i].button == button)
			scheme[i].joystick = value;
	}
}

void ControlScheme::setMouse(ControlScheme* scheme, Button button, int value)
{
	for(int i = 0;scheme[i].button != bt_nobutton;i++)
	{
		if(scheme[i].mouse == value)
			scheme[i].mouse = -1;
		if(scheme[i].button == button)
			scheme[i].mouse = value;
	}
}

int viewsize;

bool buttonheld[NUMBUTTONS], ambuttonheld[NUMAMBUTTONS];

bool demorecord, demoplayback;
int8_t *demoptr, *lastdemoptr;
memptr demobuffer;

//
// current user input
//
int controlx, controly, controlstrafe;         // range from -100 to 100 per tic
int controlpanx, controlpany;
bool buttonstate[NUMBUTTONS], ambuttonstate[NUMAMBUTTONS];

//===========================================================================


void CenterWindow (word w, word h);
int StopMusic (void);
void StartMusic (void);
void ContinueMusic (int offs);
void PlayLoop (void);

/*
=============================================================================

							USER CONTROL

=============================================================================
*/

/*
===================
=
= PollKeyboardButtons
=
===================
*/

void PollKeyboardButtons (void)
{
	if(automap == AMA_Normal)
	{
		// HACK
		bool jam[512] = {false};
		bool jamall = !!(Paused & 2); // Paused for automap

		for(int i = 0;jamall ? amControlScheme[i].button != bt_nobutton : amControlScheme[i].button <= bt_zoomout;i++)
		{
			if(amControlScheme[i].keyboard != -1 && Keyboard[amControlScheme[i].keyboard])
			{
				ambuttonstate[amControlScheme[i].button] = true;
				jam[amControlScheme[i].keyboard] = true;
			}
		}
		for(int i = 0;controlScheme[i].button != bt_nobutton;i++)
		{
			if(controlScheme[i].keyboard != -1 && Keyboard[controlScheme[i].keyboard] && !jam[controlScheme[i].keyboard])
				buttonstate[controlScheme[i].button] = true;
		}
	}
	else
	{
		for(int i = 0;controlScheme[i].button != bt_nobutton;i++)
		{
			if(controlScheme[i].keyboard != -1 && Keyboard[controlScheme[i].keyboard])
				buttonstate[controlScheme[i].button] = true;
		}
	}
}


/*
===================
=
= PollMouseButtons
=
===================
*/

void PollMouseButtons (void)
{
	int buttons = IN_MouseButtons();
	for (int i = 0; controlScheme[i].button != bt_nobutton; i++)
	{
		if (controlScheme[i].mouse != -1 && (buttons & (1 << controlScheme[i].mouse)))
			buttonstate[controlScheme[i].button] = true;
	}
}



/*
===================
=
= PollJoystickButtons
=
===================
*/

void PollJoystickButtons (void)
{
	if(automap == AMA_Normal)
	{
		// HACK
		bool jam[64] = {false};
		bool jamall = !!(Paused & 2); // Paused for automap

		int buttons = IN_JoyButtons();
		int axes = IN_JoyAxes();
		for(int i = 0;jamall ? amControlScheme[i].button != bt_nobutton : amControlScheme[i].button <= bt_zoomout;i++)
		{
			if(amControlScheme[i].joystick != -1)
			{
				if(amControlScheme[i].joystick < 32 && (buttons & (1<<amControlScheme[i].joystick)))
				{
					ambuttonstate[amControlScheme[i].button] = true;
					jam[amControlScheme[i].joystick] = true;
				}
				else if(amControlScheme[i].axis == NULL && amControlScheme[i].joystick >= 32 && (axes & (1<<(amControlScheme[i].joystick-32))))
				{
					ambuttonstate[amControlScheme[i].button] = true;
					jam[amControlScheme[i].joystick] = true;
				}
			}
		}
		for(int i = 0;controlScheme[i].button != bt_nobutton;i++)
		{
			if(controlScheme[i].joystick != -1 && !jam[controlScheme[i].joystick])
			{
				if(controlScheme[i].joystick < 32 && (buttons & (1<<controlScheme[i].joystick)))
					buttonstate[controlScheme[i].button] = true;
				else if(controlScheme[i].axis == NULL && controlScheme[i].joystick >= 32 && (axes & (1<<(controlScheme[i].joystick-32))))
					buttonstate[controlScheme[i].button] = true;
			}
		}
	}
	else
	{
		int buttons = IN_JoyButtons();
		int axes = IN_JoyAxes();
		for(int i = 0;controlScheme[i].button != bt_nobutton;i++)
		{
			if(controlScheme[i].joystick != -1)
			{
				if(controlScheme[i].joystick < 32 && (buttons & (1<<controlScheme[i].joystick)))
					buttonstate[controlScheme[i].button] = true;
				else if(controlScheme[i].axis == NULL && controlScheme[i].joystick >= 32 && (axes & (1<<(controlScheme[i].joystick-32))))
					buttonstate[controlScheme[i].button] = true;
			}
		}
	}
}


/*
===================
=
= PollKeyboardMove
=
===================
*/

void PollKeyboardMove (void)
{
	int delta = (!alwaysrun && buttonstate[bt_run]) || (alwaysrun && !buttonstate[bt_run]) ? RUNMOVE : BASEMOVE;

	if(buttonstate[bt_moveforward])
		controly -= delta;
	if(buttonstate[bt_movebackward])
		controly += delta;
	if(buttonstate[bt_turnleft])
		controlx -= delta;
	if(buttonstate[bt_turnright])
		controlx += delta;
	if(buttonstate[bt_strafeleft])
		controlstrafe -= delta;
	if(buttonstate[bt_straferight])
		controlstrafe += delta;
}


/*
===================
=
= PollMouseMove
=
===================
*/

void PollMouseMove (void)
{
	SDL_GetRelativeMouseState(&controlpanx, &controlpany);

	controlx += controlpanx * 20 / (21 - mousexadjustment);
	if(mouselook)
	{
		int mousey = controlpany;

		if(players[0].ReadyWeapon && players[0].ReadyWeapon->fovscale > 0)
			mousey = xs_ToInt(controlpany*fabs(players[0].ReadyWeapon->fovscale));

		players[0].mo->pitch += mousey * (ANGLE_1 / (21 - mouseyadjustment));
		if(players[0].mo->pitch+ANGLE_180 > ANGLE_180+56*ANGLE_1)
			players[0].mo->pitch = 56*ANGLE_1;
		else if(players[0].mo->pitch+ANGLE_180 < ANGLE_180-56*ANGLE_1)
			players[0].mo->pitch = ANGLE_NEG(56*ANGLE_1);
	}
	else if(!mouseyaxisdisabled)
		controly += controlpany * 40 / (21 - mouseyadjustment);
}


/*
===================
=
= PollJoystickMove
=
===================
*/

void PollJoystickMove (void)
{
	const bool useam = automap == AMA_Normal && Paused;
	const ControlScheme *scheme = useam ? amControlScheme+2 : controlScheme;
	do
	{
		if(scheme->joystick >= 32)
		{
			int axisnum = (scheme->joystick-32)>>1;
			bool positive = (scheme->joystick&1) != 0;
			// Scale to -100 - 100
			const int rawaxis = clamp(IN_GetJoyAxis(axisnum), -0x7FFF, 0x7FFF);
			const int dzfactor = clamp(JoySensitivity[axisnum].deadzone*0x8000/20, 0, 0x7FFF);
			int axis = clamp(abs(rawaxis)+1-dzfactor, 0, 0x8000)*5*JoySensitivity[axisnum].sensitivity/(0x8000-dzfactor);
			if(useam)
				axis >>= 2;
			else if(buttonstate[bt_run])
				axis <<= 1;
			if(positive ^ (rawaxis < 0))
				*scheme->axis += scheme->negative ? -axis : axis;
		}
	}
	while((++scheme)->axis);
}

/*
===================
=
= PollControls
=
= Gets user or demo input
= Enable absolute positioning once per frame. This prevents absolute devices
= from being carried over to adaptive tics.
=
= controlx              set between -100 and 100 per tic
= controly
= buttonheld[]  the state of the buttons LAST frame
= buttonstate[] the state of the buttons THIS frame
=
===================
*/

void PollControls (bool absolutes)
{
	int i;
	byte buttonbits;

	controlx = 0;
	controly = 0;
	controlpanx = 0;
	controlpany = 0;
	controlstrafe = 0;
	memcpy (buttonheld, buttonstate, sizeof (buttonstate));
	memset (buttonstate, 0, sizeof (buttonstate));
	if (automap)
	{
		memcpy (ambuttonheld, ambuttonstate, sizeof (ambuttonstate));
		memset (ambuttonstate, 0, sizeof (ambuttonstate));
	}

	if (demoplayback)
	{
		//
		// read commands from demo buffer
		//
		buttonbits = *demoptr++;
		for (i = 0; i < NUMBUTTONS; i++)
		{
			buttonstate[i] = buttonbits & 1;
			buttonbits >>= 1;
		}

		controlx = *demoptr++;
		controly = *demoptr++;

		if (demoptr == lastdemoptr)
			playstate = ex_completed;   // demo is done

		return;
	}


//
// get button states
//
	PollKeyboardButtons ();
	
	if (mouseenabled && IN_IsInputGrabbed())
		PollMouseButtons ();

	if (joystickenabled && IN_JoyPresent())
		PollJoystickButtons ();

//
// get movements
//
	PollKeyboardMove ();

	if (absolutes && mouseenabled && IN_IsInputGrabbed())
		PollMouseMove ();

	if (joystickenabled && IN_JoyPresent())
		PollJoystickMove ();

#ifdef __ANDROID__
	extern void pollAndroidControls();
	pollAndroidControls();
#endif

	if (demorecord)
	{
		//
		// save info out to demo buffer
		//
		buttonbits = 0;

		// TODO: Support 32-bit buttonbits
		for (i = NUMBUTTONS - 1; i >= 0; i--)
		{
			buttonbits <<= 1;
			if (buttonstate[i])
				buttonbits |= 1;
		}

		*demoptr++ = buttonbits;
		*demoptr++ = controlx;
		*demoptr++ = controly;

		if (demoptr >= lastdemoptr - 8)
			playstate = ex_completed;
	}

	// Check automap toggle before we set any buttons as held
	if (buttonstate[bt_automap] && !buttonheld[bt_automap])
	{
		AM_Toggle();
	}
	if (automap)
	{
		AM_CheckKeys();
	}
}

// This should be called once per frame
void ProcessEvents()
{
	IN_ProcessEvents();

//
// get timing info for last frame
//
	if (demoplayback || demorecord)   // demo recording and playback needs to be constant
	{
		// wait up to DEMOTICS Wolf tics
		uint32_t curtime = SDL_GetTicks();
		lasttimecount += DEMOTICS;
		int32_t timediff = (lasttimecount * 100) / 7 - curtime;
		if(timediff > 0)
			SDL_Delay(timediff);

		if(timediff < -2 * DEMOTICS)       // more than 2-times DEMOTICS behind?
			lasttimecount = (curtime * 7) / 100;    // yes, set to current timecount

		tics = DEMOTICS;
	}
	else
		CalcTics ();
}

//===========================================================================


void BumpGamma()
{
	screenGamma += 0.1f;
	if(screenGamma > 3.0f)
		screenGamma = 1.0f;
	screen->SetGamma(screenGamma);
	US_CenterWindow (10,2);
	FString msg;
	msg.Format("Gamma: %g", screenGamma);
	US_PrintCentered (msg);
	VW_UpdateScreen();
	VW_UpdateScreen();
	IN_Ack();
}

/*
=====================
=
= CheckKeys
=
=====================
*/

bool changeSize = true;
void CheckKeys (void)
{
	ScanCode scan;


	if (screenfaded || demoplayback)    // don't do anything with a faded screen
		return;

	scan = LastScan;

	// [BL] Allow changing the screen size with the -/= keys a la Doom.
	if(automap != AMA_Normal && changeSize)
	{
		if(Keyboard[sc_Equals] && !Keyboard[sc_Minus])
			NewViewSize(viewsize+1);
		else if(!Keyboard[sc_Equals] && Keyboard[sc_Minus])
			NewViewSize(viewsize-1);
		if(Keyboard[sc_Equals] || Keyboard[sc_Minus])
		{
			SD_PlaySound("world/hitwall");
			DrawPlayScreen();
			changeSize = false;
		}
	}
	else if(!Keyboard[sc_Equals] && !Keyboard[sc_Minus])
		changeSize = true;

	//
	// SECRET CHEAT CODE: 'MLI'
	//
	if (Keyboard[sc_M] && Keyboard[sc_L] && Keyboard[sc_I])
		DebugMLI();

	//
	// OPEN UP DEBUG KEYS
	//
	if (Keyboard[sc_BackSpace] && Keyboard[sc_LShift] && Keyboard[sc_Alt])
	{
		ClearMemory ();
		ClearSplitVWB ();

		Message ("Debugging keys are\nnow available!");
		IN_ClearKeysDown ();
		IN_Ack ();

		DrawPlayBorderSides ();
		DebugOk = 1;
	}

	//
	// TRYING THE KEEN CHEAT CODE!
	//
	if (Keyboard[sc_B] && Keyboard[sc_A] && Keyboard[sc_T])
	{
		ClearMemory ();
		ClearSplitVWB ();

		Message ("Commander Keen is also\n"
				"available from Apogee, but\n"
				"then, you already know\n" "that - right, Cheatmeister?!");

		IN_ClearKeysDown ();
		IN_Ack ();

		if (viewsize < 18)
			StatusBar->RefreshBackground ();
	}

//
// pause key weirdness can't be checked as a scan code
//
	if(buttonstate[bt_pause]) Paused |= 1;
	if(Paused & 1)
	{
		int lastoffs = StopMusic();
		IN_ReleaseMouse();
		VWB_DrawGraphic(TexMan("PAUSED"), (20 - 4)*8, 80 - 2*8);
		VH_UpdateScreen();
		IN_Ack ();
		IN_GrabMouse();
		Paused &= ~1;
		ContinueMusic(lastoffs);
		if (MousePresent && IN_IsInputGrabbed())
			IN_CenterMouse();     // Clear accumulated mouse movement
		lasttimecount = GetTimeCount();
		return;
	}

//
// F1-F7/ESC to enter control panel
//
	if (scan == sc_F10 ||
		scan == sc_F9 || scan == sc_F7 || scan == sc_F8)     // pop up quit dialog
	{
		ClearMemory ();
		ClearSplitVWB ();
		US_ControlPanel (scan);

		DrawPlayBorderSides ();

		IN_ClearKeysDown ();
		return;
	}

	if ((scan >= sc_F1 && scan <= sc_F9) || scan == sc_Escape || buttonstate[bt_esc])
	{
		int lastoffs = StopMusic ();
		ClearMemory ();

		US_ControlPanel (buttonstate[bt_esc] ? sc_Escape : scan);

		if(screenfaded)
		{
			IN_ClearKeysDown ();
			if (!startgame && !loadedgame)
			{
				VW_FadeOut();
				ContinueMusic (lastoffs);
				if(viewsize != 21)
					DrawPlayScreen ();
			}
			if (loadedgame)
				playstate = ex_abort;
			lasttimecount = GetTimeCount();
			if (MousePresent && IN_IsInputGrabbed())
				IN_CenterMouse();     // Clear accumulated mouse movement
		}
		else
		{
			IN_ClearKeysDown();
			ContinueMusic (lastoffs);
		}
		return;
	}

	if(scan == sc_F11)
	{
		BumpGamma();
		return;
	}

//
// TAB-? debug keys
//
	if (DebugOk)
	{
		// Jam debug sequence if we're trying to open the automap
		// We really only need to check for the automap control since it's
		// likely to be put in the Tab space and be tapped while using other controls
		bool keyDown = Keyboard[sc_Tab] || Keyboard[sc_BackSpace] || Keyboard[sc_Grave];
		if ((schemeAutomapKey.keyboard == sc_Tab || schemeAutomapKey.keyboard == sc_BackSpace || schemeAutomapKey.keyboard == sc_Grave)
			&& (buttonstate[bt_automap] || buttonheld[bt_automap]))
			keyDown = false;

#ifdef __ANDROID__
		// Soft keyboard
		if (ShadowingEnabled)
			keyDown = true;
#endif

		if (keyDown)
		{
			if (DebugKeys ())
			{
				if (viewsize < 20)
					StatusBar->RefreshBackground ();       // dont let the blue borders flash

				if (MousePresent && IN_IsInputGrabbed())
					IN_CenterMouse();     // Clear accumulated mouse movement

				lasttimecount = GetTimeCount();
			}
		}
	}
}


//===========================================================================

/*
=============================================================================

												MUSIC STUFF

=============================================================================
*/


/*
=================
=
= StopMusic
=
=================
*/
int StopMusic (void)
{
	int lastoffs = 0;

    if (music == NULL)
    {
        lastoffs = SD_MusicOff ();
    }
    else
    {
        lastoffs = SD_PauseMusic ();
    }

	return lastoffs;
}

//==========================================================================


/*
=================
=
= StartMusic
=
=================
*/

void StartMusic ()
{
	SD_MusicOff ();
	SD_StartMusic(levelInfo->Music);
}

void ContinueMusic (int offs)
{
	SD_MusicOff ();
	SD_ContinueMusic(levelInfo->Music, offs);
}

/*
=============================================================================

										PALETTE SHIFTING STUFF

=============================================================================
*/

#define NUMREDSHIFTS    6
#define REDSTEPS        8

#define NUMWHITESHIFTS  3
#define WHITESTEPS      20
#define WHITETICS       6

int damagecount, bonuscount;
bool palshifted;

/*
=====================
=
= ClearPaletteShifts
=
=====================
*/

void ClearPaletteShifts (void)
{
	bonuscount = damagecount = 0;
	palshifted = false;
}


/*
=====================
=
= StartBonusFlash
=
=====================
*/

void StartBonusFlash (void)
{
	bonuscount = NUMWHITESHIFTS * WHITETICS;    // white shift palette
}


/*
=====================
=
= StartDamageFlash
=
=====================
*/

void StartDamageFlash (int damage)
{
	damagecount += damage;
}


/*
=====================
=
= UpdatePaletteShifts
=
=====================
*/

void UpdatePaletteShifts (void)
{
	int red, white;

	if (bonuscount)
	{
		white = bonuscount / WHITETICS + 1;
		if (white > NUMWHITESHIFTS)
			white = NUMWHITESHIFTS;
		bonuscount -= tics;
		if (bonuscount < 0)
			bonuscount = 0;
	}
	else
		white = 0;


	if (damagecount)
	{
		red = damagecount / 10 + 1;
		if (red > NUMREDSHIFTS)
			red = NUMREDSHIFTS;

		damagecount -= tics;
		if (damagecount < 0)
			damagecount = 0;
	}
	else
		red = 0;

	if (red)
	{
		V_SetBlend(RPART(players[0].mo->damagecolor),
                             GPART(players[0].mo->damagecolor),
                             BPART(players[0].mo->damagecolor), red*(174/NUMREDSHIFTS));
		palshifted = true;
	}
	else if (white)
	{
		// [BL] More of a yellow if you ask me.
		V_SetBlend(0xFF, 0xF8, 0x00, white*(38/NUMWHITESHIFTS));
		palshifted = true;
	}
	else if (palshifted)
	{
		V_SetBlend(0, 0, 0, 0);
		palshifted = false;
	}
}


/*
=====================
=
= FinishPaletteShifts
=
= Resets palette to normal if needed
=
=====================
*/

void FinishPaletteShifts (void)
{
	if (palshifted)
	{
		V_SetBlend(0, 0, 0, 0);
		VH_UpdateScreen();
		palshifted = false;
	}
}


/*
=============================================================================

												CORE PLAYLOOP

=============================================================================
*/

/*
===================
=
= PlayLoop
=
===================
*/
int32_t funnyticount;


void PlayLoop (void)
{
#if defined(USE_FEATUREFLAGS) && defined(USE_CLOUDSKY)
	if(GetFeatureFlags() & FF_CLOUDSKY)
		InitSky();
#endif

#ifdef __ANDROID__
	if (ShadowingEnabled)
		DebugOk = 1;
#endif

	playstate = ex_stillplaying;
	lasttimecount = GetTimeCount();
	frameon = 0;
	funnyticount = 0;
	memset (buttonstate, 0, sizeof (buttonstate));
	ClearPaletteShifts ();

	if(automap != AMA_Off)
	{
			// Force the automap to off if it were previously on, unpause the game if am_pause
		automap = AMA_Off;

		if(am_pause) Paused &= ~2;
	}


	if (MousePresent && IN_IsInputGrabbed())
		IN_CenterMouse();         // Clear accumulated mouse movement

	if (demoplayback)
		IN_StartAck ();

	StatusBar->NewGame();

	do
	{
		ProcessEvents();

//
// actor thinking
//
		madenoise = false;

		// Run tics
		if(Paused & 2)
		{
			static bool absolutes = false;

			// If paused due to the automap, continue polling controls but don't tick anything.
			PollControls(absolutes);

			absolutes = !absolutes;
		}
		else
		{
			for (unsigned int i = 0;i < tics;++i)
			{
				PollControls(!i);

				++gamestate.TimeCount;
				thinkerList->Tick();
				AActor::FinishSpawningActors();
			}
		}

		UpdatePaletteShifts ();

		ThreeDRefresh ();

		if(automap && !gamestate.victoryflag)
			BasicOverhead();

		//
		// MAKE FUNNY FACE IF BJ DOESN'T MOVE FOR AWHILE
		//
		funnyticount += tics;

		TexMan.UpdateAnimations(gamestate.TimeCount*14);
		GC::CheckGC();

		UpdateSoundLoc ();      // JAB
		if (screenfaded)
			VW_FadeIn ();

		CheckKeys ();
		if((gamestate.TimeCount & 1) || !(tics & 1))
			StatusBar->DrawStatusBar();

		VH_UpdateScreen();
//
// debug aids
//
		if (singlestep)
		{
			VW_WaitVBL (singlestep);
			lasttimecount = GetTimeCount();
		}
		if (extravbls)
			VW_WaitVBL (extravbls);

		if (demoplayback)
		{
			if (IN_CheckAck ())
			{
				IN_ClearKeysDown ();
				playstate = ex_abort;
			}
		}
	}
	while (!playstate && !startgame);

	if (playstate != ex_died)
		FinishPaletteShifts ();
}
