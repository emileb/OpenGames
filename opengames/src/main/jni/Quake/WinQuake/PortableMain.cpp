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
#define LOG_TAG "QuakeMaster"

#include "quakedef.h"


#include <stdio.h>
#include <assert.h>
#include <dlfcn.h>

#include "TouchControlsContainer.h"
#include "JNITouchControlsUtils.h"

#include "in_android.h"



bool QUAKE_QUIT;

extern  qboolean gDoubleInitializeGuard;
extern  qboolean gInitialized;
extern  qboolean	host_initialized;
int controlsCreated = 0;
touchcontrols::TouchControlsContainer controlsContainer;

touchcontrols::TouchControls tcMenuMain("menu",true,false);
touchcontrols::TouchControls tcGameMain("game",false,true);
touchcontrols::TouchControls tcGameWeapons("weapons",false,false);

//So can hide and show these buttons

touchcontrols::TouchJoy *touchJoyLeft;
touchcontrols::TouchJoy *touchJoyRight;


#define KEY_SHOW_WEAPONS 0x1000
#define KEY_SHOOT        0x1001
#define KEY_JUMP         0x1002
#define KEY_KEYBOARD     0x1003
#define KEY_NEXT_WEAPONS 0x1003
#define KEY_PREV_WEAPONS 0x1004
#define KEY_QUICK_CMD    0x1005

static int showKeyboardNow = 0;
float gameControlsAlpha = 0.5;
bool showSticks = false;
bool turnMouseMode = false;
bool invertLook = false;
bool precisionShoot = false;
bool hideTouchControls = true;

bool shooting = false;

void PortableInitArgs(int argc,const char** argv) ;
int  PortableInit();
int  PortableStep(int width, int height);
void PortableQuit();

int PortableKeyEvent(int type, int value);
void PortableAction(int state, int action);

void PortableMove(float fwd, float strafe);
void PortableMoveFwd(float fwd);
void PortableMoveSide(float strafe);
void PortableLookPitch(int mode, float pitch);
void PortableLookYaw(int mode, float pitch);
void PortableCommand(char * cmd);

void PortableSetBasePath(const char *path);
void PortableSetCacheBasePath(const char *path);

void showTouchSettings();
void showCustomCommands();

void gameSettingsButton(int state)
{
	//LOGTOUCH("gameSettingsButton %d",state);
	if (state == 1)
	{
		showTouchSettings();
	}
}

void openGLStart()
{
	glDisable(GL_ALPHA_TEST);
	glEnable (GL_BLEND);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

}
void openGLEnd()
{
	glColor4f (1,1,1,1);
	glEnable(GL_ALPHA_TEST);
	glDisable (GL_BLEND);

}

//Game movment callbacks
void gameButton(int state,int code)
{
	if (code == KEY_SHOOT)
	{
		shooting = state;
		PortableAction(state,PORT_ACT_ATTACK);
	}
	else if  (code == KEY_JUMP)
	{
		//PortableAction(state,PORT_ACT_NEXT_WEP);
		PortableAction(state,PORT_ACT_JUMP);
	}
	else if (code == KEY_SHOW_WEAPONS)
	{
		if (state == 1)
			if (!tcGameWeapons.enabled)
				tcGameWeapons.animateIn(5);
	}
	else if  (code == KEY_NEXT_WEAPONS)
	{
		PortableAction(state,PORT_ACT_NEXT_WEP);
	}
	else if  (code == KEY_PREV_WEAPONS)
	{
		PortableAction(state,PORT_ACT_PREV_WEP);
	}
	else if  (code ==KEY_QUICK_CMD){
		if (state == 1)
			showCustomCommands();
	}
	else
		PortableKeyEvent(state, code);
}

void menuButton(int state,int code)
{
	if (code == KEY_KEYBOARD)
		showKeyboardNow = 1;
	else
		PortableAction(state, code);
}

int left_double_action;
int right_double_action;

void left_double_tap(int state)
{
	LOGTOUCH("L double %d",state);
	if (left_double_action)
		PortableAction(state,left_double_action);
}

void right_double_tap(int state)
{
	LOGTOUCH("R double %d",state);
	if (right_double_action)
		PortableAction(state,right_double_action);
}

//To be set by android
float strafe_sens,forward_sens;
float pitch_sens,yaw_sens;

void left_stick(float joy_x, float joy_y,float mouse_x, float mouse_y)
{
	joy_x *=10;
	float strafe = joy_x*joy_x;
	if (joy_x < 0)
		strafe *= -1;

	PortableMove(joy_y * 10 * forward_sens,-strafe * strafe_sens);
}
void right_stick(float joy_x, float joy_y,float mouse_x, float mouse_y)
{
	int invert = invertLook?-1:1;

	float scale = (shooting && precisionShoot)?0.3:1;

	PortableLookPitch(LOOK_MODE_MOUSE,-mouse_y  * pitch_sens * invert * scale);

	if (turnMouseMode)
		PortableLookYaw(LOOK_MODE_MOUSE,mouse_x*2*yaw_sens * scale);
	else
		PortableLookYaw(LOOK_MODE_JOYSTICK,joy_x*6*yaw_sens * scale);
}

void setTouchSettings(float alpha,float strafe,float fwd,float pitch,float yaw,int other)
{
	turnMouseMode   = other & 0x2?true:false;
	invertLook      = other & 0x4?true:false;
	precisionShoot  = other & 0x8?true:false;
	showSticks      = other & 0x1000?true:false;

	hideTouchControls = other & 0x80000000?true:false;

	switch ((other>>4) & 0xF)
	{
	case 1:
		left_double_action = PORT_ACT_ATTACK;
		break;
	case 2:
		left_double_action = PORT_ACT_JUMP;
		break;
	default:
		left_double_action = 0;
	}

	switch ((other>>8) & 0xF)
	{
	case 1:
		right_double_action = PORT_ACT_ATTACK;
		break;
	case 2:
		right_double_action = PORT_ACT_JUMP;
		break;
	default:
		right_double_action = 0;
	}

	gameControlsAlpha = alpha;
	tcGameMain.setAlpha(gameControlsAlpha);
	strafe_sens = strafe;
	forward_sens = fwd;
	pitch_sens = pitch;
	yaw_sens = yaw;

}

void setHideSticks(bool v)
{
	if (touchJoyLeft) touchJoyLeft->setHideGraphics(v);
	if (touchJoyRight) touchJoyRight->setHideGraphics(v);
}


//Weapon select callbacks
void selectWeaponButton(int state, int code)
{
	PortableKeyEvent(state, code);
	if (state == 0)
		tcGameWeapons.animateOut(5);
}



void portableSetBasePath(const char *path)
{
	LOGI("base = %s",path);
	PortableSetBasePath(path);
}

void portableSetCacheBasePath(const char *path)
{
	LOGI("cache = %s",path);
	PortableSetCacheBasePath(path);
}

std::string graphicspath;
//extern std::string graphicsBasePath;
int portableSetGraphicsBasePath(const char * path)
{
	LOGI("graphics = %s",path);
	touchcontrols::setGraphicsBasePath(path);
	graphicspath = path;

}

void portablePreInit()
{
	QUAKE_QUIT = false;
	gInitialized = false;
	gDoubleInitializeGuard = false;
	host_initialized = false;
	touchcontrols::GLScaleWidth = 400;
	touchcontrols::GLScaleHeight = -300;

	LOGI("portablePreInit %d",gInitialized);
}

int portableInit(int argc,const char** argv) {
	LOGI("portableInit");

	PortableInitArgs( argc, argv);

	if (!controlsCreated)
	{
		LOGI("creating controls");

		controlsContainer.openGL_start.connect( sigc::ptr_fun(&openGLStart));
		controlsContainer.openGL_end.connect( sigc::ptr_fun(&openGLEnd));
		setControlsContainer(&controlsContainer);


		//Menu

		tcMenuMain.addControl(new touchcontrols::Button("down_arrow",touchcontrols::RectF(20,13,23,16),"arrow_down",PORT_ACT_MENU_DOWN));
		tcMenuMain.addControl(new touchcontrols::Button("up_arrow",touchcontrols::RectF(20,10,23,13),"arrow_up",PORT_ACT_MENU_UP));
		tcMenuMain.addControl(new touchcontrols::Button("left_arrow",touchcontrols::RectF(17,13,20,16),"arrow_left",PORT_ACT_MENU_LEFT));
		tcMenuMain.addControl(new touchcontrols::Button("right_arrow",touchcontrols::RectF(23,13,26,16),"arrow_right",PORT_ACT_MENU_RIGHT));
		tcMenuMain.addControl(new touchcontrols::Button("enter",touchcontrols::RectF(0,12,4,16),"enter",PORT_ACT_MENU_SELECT));
		tcMenuMain.addControl(new touchcontrols::Button("keyboard",touchcontrols::RectF(0,0,2,2),"keyboard",KEY_KEYBOARD));
		tcMenuMain.signal_button.connect(  sigc::ptr_fun(&menuButton) );

		//Game
		tcGameMain.addControl(new touchcontrols::Button("jump",touchcontrols::RectF(23,5,26,8),"jump",KEY_JUMP));
		tcGameMain.addControl(new touchcontrols::Button("attack",touchcontrols::RectF(20,7,23,10),"shoot",KEY_SHOOT));
		tcGameMain.addControl(new touchcontrols::Button("quick_save",touchcontrols::RectF(24,0,26,2),"save",K_F6));
		tcGameMain.addControl(new touchcontrols::Button("quick_load",touchcontrols::RectF(20,0,22,2),"load",K_F9));
		tcGameMain.addControl(new touchcontrols::Button("show_weapons",touchcontrols::RectF(12,14,14,16),"show_weapons",KEY_SHOW_WEAPONS));


		tcGameMain.addControl( new touchcontrols::Button("next_weapon",touchcontrols::RectF(0,3,3,5),"next_weap",KEY_NEXT_WEAPONS));
		tcGameMain.addControl(new touchcontrols::Button("prev_weapon",touchcontrols::RectF(0,6,3,8),"prev_weap",KEY_PREV_WEAPONS));
		tcGameMain.addControl(new touchcontrols::Button("quick_cmd",touchcontrols::RectF(24,3,26,5),"prompt",KEY_QUICK_CMD,false,true));

		touchJoyLeft = new touchcontrols::TouchJoy("stick",touchcontrols::RectF(0,7,8,16),"strafe_arrow");
		tcGameMain.addControl(touchJoyLeft);
		touchJoyLeft->signal_move.connect(sigc::ptr_fun(&left_stick) );
		touchJoyLeft->signal_double_tap.connect(sigc::ptr_fun(&left_double_tap) );

		touchJoyRight = new touchcontrols::TouchJoy("touch",touchcontrols::RectF(17,7,26,16),"look_arrow");
		tcGameMain.addControl(touchJoyRight);
		touchJoyRight->signal_move.connect(sigc::ptr_fun(&right_stick) );
		touchJoyRight->signal_double_tap.connect(sigc::ptr_fun(&right_double_tap) );

		tcGameMain.signal_button.connect(  sigc::ptr_fun(&gameButton) );
		tcGameMain.signal_settingsButton.connect(  sigc::ptr_fun(&gameSettingsButton) );
		//Weapons
		tcGameWeapons.addControl(new touchcontrols::Button("weapon1",touchcontrols::RectF(1,14,3,16),"key_1",'1'));
		tcGameWeapons.addControl(new touchcontrols::Button("weapon2",touchcontrols::RectF(4,14,6,16),"key_2",'2'));
		tcGameWeapons.addControl(new touchcontrols::Button("weapon3",touchcontrols::RectF(7,14,9,16),"key_3",'3'));
		tcGameWeapons.addControl(new touchcontrols::Button("weapon4",touchcontrols::RectF(10,14,12,16),"key_4",'4'));

		tcGameWeapons.addControl(new touchcontrols::Button("weapon5",touchcontrols::RectF(14,14,16,16),"key_5",'5'));
		tcGameWeapons.addControl(new touchcontrols::Button("weapon6",touchcontrols::RectF(17,14,19,16),"key_6",'6'));
		tcGameWeapons.addControl(new touchcontrols::Button("weapon7",touchcontrols::RectF(20,14,22,16),"key_7",'7'));
		tcGameWeapons.addControl(new touchcontrols::Button("weapon8",touchcontrols::RectF(23,14,25,16),"key_8",'8'));
		tcGameWeapons.signal_button.connect(  sigc::ptr_fun(&selectWeaponButton) );
		tcGameWeapons.alpha = 0.5;

		controlsContainer.addControlGroup(&tcGameMain);
		controlsContainer.addControlGroup(&tcGameWeapons);
		controlsContainer.addControlGroup(&tcMenuMain);
		controlsCreated = 1;


		tcGameMain.setXMLFile(graphicspath + "/gl_ctrl.xml");
	}
	else
		LOGI("NOT creating controls");

	controlsContainer.initGL();

	LOGI("PortableInit");
	return PortableInit();
}

int portableTouchEvent(int action, int pid, float x, float y) {
	//LOGTOUCH("TOUCHED %d %d %f %f",action,pid,x,y);

	controlsContainer.processPointer(action,pid,x,y);
}

int portableKeyEvent(int type, int value) {
	//LOGTOUCH("portableKeyEvent %d   %d",type,value);
	return PortableKeyEvent(type, value);
}


std::string quickCommandString;
int quickCommand(char *command)
{
	quickCommandString =  std::string(command) + "\n";
	PortableCommand((char*)quickCommandString.c_str());
}

int old_r=-1;
int portableStep(int width,int height) {
	int r;

	if (QUAKE_QUIT)
	{
		Host_Shutdown();
		return 128;
	}

	r = PortableStep(width, height);
	if (r!=old_r)
	{
		if (r)
		{
			tcGameMain.enabled = true;
			tcMenuMain.enabled = false;
		}
		else
		{
			tcGameMain.enabled = false;
			tcGameWeapons.enabled = false;
			tcMenuMain.enabled = true;
		}
	}

	setHideSticks(!showSticks);
	controlsContainer.draw();


	if (showKeyboardNow)
	{
		showKeyboardNow = false;
		return 1;
	}
	else
		return 0;
}

void portableQuit() {
	LOGI("portableQuit");
	PortableQuit();
}
