/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <time.h>
#include <unistd.h>

#ifdef ANTI_HACK
#include "beloko_common/tools.h"
#endif

#include "TouchControlsContainer.h"
#include "in_android.h"


#include "JNITouchControlsUtils.h"


#include "../client/vid.h"

#include "gl.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"QUAKE2", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "QUAKE2", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"QUAKE2", __VA_ARGS__))


enum QKEYS {
	K_TAB			= 9,
	K_ENTER			= 13,
	K_ESCAPE		= 27,
	K_SPACE			= 32,

	// normal keys should be passed as lowercased ascii

	K_BACKSPACE		= 127,
	K_UPARROW		= 128,
	K_DOWNARROW		= 129,
	K_LEFTARROW		= 130,
	K_RIGHTARROW		= 131,

	K_ALT			= 132,
	K_CTRL			= 133,
	K_SHIFT			= 134,
	K_F1			= 135,
	K_F2			= 136,
	K_F3			= 137,
	K_F4			= 138,
	K_F5			= 139,
	K_F6			= 140,
	K_F7			= 141,
	K_F8			= 142,
	K_F9			= 143,
	K_F10			= 144,
	K_F11			= 145,
	K_F12			= 146,
	K_INS			= 147,
	K_DEL			= 148,
	K_PGDN			= 149,
	K_PGUP			= 150,
	K_HOME			= 151,
	K_END			= 152,

	K_KP_HOME		= 160,
	K_KP_UPARROW		= 161,
	K_KP_PGUP		= 162,
	K_KP_LEFTARROW		= 163,
	K_KP_5			= 164,
	K_KP_RIGHTARROW		= 165,
	K_KP_END		= 166,
	K_KP_DOWNARROW		= 167,
	K_KP_PGDN		= 168,
	K_KP_ENTER		= 169,
	K_KP_INS		= 170,
	K_KP_DEL		= 171,
	K_KP_SLASH		= 172,
	K_KP_MINUS		= 173,
	K_KP_PLUS		= 174,

	//
	// mouse buttons generate virtual keys
	//
	K_MOUSE1		= 200,
	K_MOUSE2		= 201,
	K_MOUSE3		= 202,
	K_MOUSE4		= 241,
	K_MOUSE5		= 242,

	//
	// joystick buttons
	//
	K_JOY1			= 203,
	K_JOY2			= 204,
	K_JOY3			= 205,
	K_JOY4			= 206,

	//
	// aux keys are for multi-buttoned joysticks to generate so they can use
	// the normal binding process
	//
	K_AUX1			= 207,
	K_AUX2			= 208,
	K_AUX3			= 209,
	K_AUX4			= 210,
	K_AUX5			= 211,
	K_AUX6			= 212,
	K_AUX7			= 213,
	K_AUX8			= 214,
	K_AUX9			= 215,
	K_AUX10			= 216,
	K_AUX11			= 217,
	K_AUX12			= 218,
	K_AUX13			= 219,
	K_AUX14			= 220,
	K_AUX15			= 221,
	K_AUX16			= 222,
	K_AUX17			= 223,
	K_AUX18			= 224,
	K_AUX19			= 225,
	K_AUX20			= 226,
	K_AUX21			= 227,
	K_AUX22			= 228,
	K_AUX23			= 229,
	K_AUX24			= 230,
	K_AUX25			= 231,
	K_AUX26			= 232,
	K_AUX27			= 233,
	K_AUX28			= 234,
	K_AUX29			= 235,
	K_AUX30			= 236,
	K_AUX31			= 237,
	K_AUX32			= 238,

	K_MWHEELDOWN		= 239,
	K_MWHEELUP		= 240,

	K_PAUSE			= 255,

	K_LAST
};


extern "C"
{
void CDDroid_SetJni(JNIEnv* e);


//Used in gl_rmain.c
int android_screen_width;
int android_screen_height;

typedef enum {key_game, key_console, key_message, key_menu} keydest_t;

keydest_t getKeyDest();



std::string library_path_string;
char const * library_path;

int PortableKeyEvent(int state, int code);
void PortableAction(int state, int action);

void PortableMove(float fwd, float strafe);
void PortableMoveFwd(float fwd);
void PortableMoveSide(float strafe);
void PortableLookPitch(int mode, float pitch);
void PortableLookYaw(int mode, float pitch);
void PortableCommand(char * cmd);

//#include "../ref_soft/r_local.h"
//#include "rw_android.h"
#include "quake2-jni.h"

//#include "gl.h"

#define QUAKE2_JNI_VERSION "1.8"


#define EXPORT_ME __attribute__ ((visibility("default")))

jstring EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2GetVersion( JNIEnv* env,
		jobject thiz )
{
	return env->NewStringUTF( QUAKE2_JNI_VERSION);
}

static int 	oldtime ;

extern int Sys_Milliseconds(void);
extern void Qcommon_Init(int argc, char **argv);
extern void Qcommon_Frame(int msec);
extern void CL_Shutdown ();
extern void  Qcommon_Shutdown ();


float gameControlsAlpha = 0.5;
bool showWeaponCycle = false;
bool turnMouseMode = true;
bool invertLook = false;
bool precisionShoot = false;
bool showSticks = true;
bool hideTouchControls = true;
bool enableWeaponWheel = true;

bool shooting = false;


static int controlsCreated = 0;
touchcontrols::TouchControlsContainer controlsContainer;

touchcontrols::TouchControls *tcMenuMain=0;
touchcontrols::TouchControls *tcGameMain=0;
touchcontrols::TouchControls *tcGameWeapons=0;
touchcontrols::TouchControls *tcInventory=0;
touchcontrols::TouchControls *tcWeaponWheel=0;

//So can hide and show these buttons
touchcontrols::Button *nextWeapon=0;
touchcontrols::Button *prevWeapon=0;
touchcontrols::TouchJoy *touchJoyLeft;
touchcontrols::TouchJoy *touchJoyRight;
touchcontrols::Button *quickCmd=0;



#define KEY_SHOW_WEAPONS 0x1000
#define KEY_SHOOT        0x1001
#define KEY_JUMP         0x1002
#define KEY_NEXT_WEAPONS 0x1003
#define KEY_PREV_WEAPONS 0x1004
#define KEY_CROUCH       0x1005
#define KEY_SHOW_INV     0x1006
#define KEY_QUICK_CMD    0x1007

void openGLStart()
{
	nanoPushState();

	glDisableClientState(GL_COLOR_ARRAY);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
}
void openGLEnd()
{

	nanoPopState();
	glPopMatrix();
}

JNIEnv* env_;
JavaVM* jvm_;

void gameSettingsButton(int state)
{
	if (state == 1)
	{
		showTouchSettings();
	}
}

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
	else if  (code == KEY_NEXT_WEAPONS)
	{
		PortableAction(state,PORT_ACT_NEXT_WEP);
	}
	else if  (code == KEY_PREV_WEAPONS)
	{
		PortableAction(state,PORT_ACT_PREV_WEP);
	}
	else if  (code == KEY_CROUCH)
	{
		PortableAction(state,PORT_ACT_DOWN);
	}
	else if (code == KEY_SHOW_WEAPONS)
	{
		if (state == 1)
			if (!tcGameWeapons->enabled)
			{
				tcInventory->animateOut(5);
				tcGameWeapons->animateIn(5);
			}
	}
	else if (code == KEY_SHOW_INV)
	{
		if (state == 1)
		{
			if (!tcInventory->enabled)
			{
				tcGameWeapons->animateOut(5);
				tcInventory->animateIn(5);
			}
			else
				tcInventory->animateOut(5);
		}
	}
	else if  (code == KEY_QUICK_CMD){
		if (state == 1)
			showCustomCommands();
	}
	else
		PortableKeyEvent(state, code);
}


void menuButton(int state,int code)
{
	if (code == K_TAB) //bit of a hack
		quake2_jni_key_event(state, code);
	else
		PortableAction(state,code);
}

void inventoryButton(int state,int code)
{
	PortableAction(state,code);
}

int left_double_action;
int right_double_action;

void left_double_tap(int state)
{
	//LOGTOUCH("L double %d",state);
	if (left_double_action)
		PortableAction(state,left_double_action);
}

void right_double_tap(int state)
{
	//LOGTOUCH("R double %d",state);
	if (right_double_action)
		PortableAction(state,right_double_action);
}

//To be set by android
float strafe_sens=1,forward_sens=1;
float pitch_sens=1,yaw_sens=1;

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
	//LOGTOUCH(" mouse x = %f",mouse_x);
	int invert = invertLook?-1:1;

	float scale = (shooting && precisionShoot)?0.3:1;

	PortableLookPitch(LOOK_MODE_MOUSE,-mouse_y  * pitch_sens * invert * scale);

	if (turnMouseMode)
		PortableLookYaw(LOOK_MODE_MOUSE,mouse_x*2*yaw_sens * scale);
	else
		PortableLookYaw(LOOK_MODE_JOYSTICK,joy_x*6*yaw_sens * scale);
}

//Weapon select callbacks
void selectWeaponButton(int state, int code)
{
	PortableKeyEvent(state, code);
	if (state == 0)
		tcGameWeapons->animateOut(5);
}

//Weapon wheel callbacks
void weaponWheelSelected(int enabled)
{
	if (enabled)
		tcWeaponWheel->fade(touchcontrols::FADE_IN,5); //fade in
}

void weaponWheel(int segment)
{
	LOGI("weaponWheel %d",segment);
	int code;
	if (segment == 9)
		code = '0';
	else
		code = '1' + segment;

	PortableKeyEvent(1,code);
	PortableKeyEvent(0, code);
}


void weaponCycle(bool v)
{
	if (v)
	{
		if (nextWeapon) nextWeapon->setEnabled(true);
		if (prevWeapon) prevWeapon->setEnabled(true);
	}
	else
	{
		if (nextWeapon) nextWeapon->setEnabled(false);
		if (prevWeapon) prevWeapon->setEnabled(false);
	}
}

void setHideSticks(bool v)
{
	if (touchJoyLeft) touchJoyLeft->setHideGraphics(v);
	if (touchJoyRight) touchJoyRight->setHideGraphics(v);
}

__attribute__((visibility("default"))) jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	LOGI("JNI_OnLoad");
	jvm_ = vm;
	setTCJNIEnv(jvm_);

	return JNI_VERSION_1_2;
}

int argc=1;
const char * argv[32];
std::string graphicpath;

extern int quake2_game_dll;
jint EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2Init( JNIEnv* env,
		jobject thiz,jstring graphics_dir,jint mem_mb,jobjectArray argsArray,jint game )
{
	LOGI("Java_com_beloko_opengames_quake2_Quake2Lib_Quake2Init");

	env_ = env;

	quake2_game_dll = game;

	CDDroid_SetJni(env);

	/// check for static init problem (see below)
	static int init_state = 0;
	if (init_state!=0)
		return 1;
	init_state = 1;

	//Cvar_Set("developer","1");

	argv[0] = "quake";
	int argCount = (env)->GetArrayLength( argsArray);
	LOGI("argCount = %d",argCount);
	for (int i=0; i<argCount; i++) {
		jstring string = (jstring) (env)->GetObjectArrayElement( argsArray, i);
		argv[argc] = (char *)(env)->GetStringUTFChars( string, 0);
		LOGI("arg = %s",argv[argc]);
		argc++;
	}

	Qcommon_Init(argc, (char**)argv);


	oldtime = Sys_Milliseconds ();

	__android_log_print(ANDROID_LOG_DEBUG, "quake2-jni.c", "native code : Quake2Init exit\n");



	touchcontrols::GLScaleWidth = (float)viddef.width;
	touchcontrols::GLScaleHeight = -(float)viddef.height;


	if (!controlsCreated)
	{
		LOGI("creating controls");

		const char * p = env->GetStringUTFChars(graphics_dir,NULL);
		graphicpath =  std::string(p);
		touchcontrols::setGraphicsBasePath(graphicpath.c_str());
		setControlsContainer(&controlsContainer);

		controlsContainer.openGL_start.connect( sigc::ptr_fun(&openGLStart));
		controlsContainer.openGL_end.connect( sigc::ptr_fun(&openGLEnd));


		tcMenuMain = new touchcontrols::TouchControls("menu",true,false);
		tcGameMain = new touchcontrols::TouchControls("game",false,true,1);
		tcGameWeapons = new touchcontrols::TouchControls("weapons",false,true,1);
		tcInventory  = new touchcontrols::TouchControls("inventory",false,true,1);
		tcWeaponWheel = new touchcontrols::TouchControls("weapon_wheel",false,true,1);


		//Menu
		tcMenuMain->addControl(new touchcontrols::Button("down_arrow",touchcontrols::RectF(20,13,23,16),"arrow_down",PORT_ACT_MENU_DOWN));
		tcMenuMain->addControl(new touchcontrols::Button("up_arrow",touchcontrols::RectF(20,10,23,13),"arrow_up",PORT_ACT_MENU_UP));
		tcMenuMain->addControl(new touchcontrols::Button("left_arrow",touchcontrols::RectF(17,13,20,16),"arrow_left",PORT_ACT_MENU_LEFT));
		tcMenuMain->addControl(new touchcontrols::Button("right_arrow",touchcontrols::RectF(23,13,26,16),"arrow_right",PORT_ACT_MENU_RIGHT));
		tcMenuMain->addControl(new touchcontrols::Button("enter",touchcontrols::RectF(0,12,4,16),"enter",PORT_ACT_MENU_SELECT));
		//tcMenuMain->addControl(new touchcontrols::Button("tab",touchcontrols::RectF(0,0,2,2),"tab",K_TAB));
		tcMenuMain->signal_button.connect(  sigc::ptr_fun(&menuButton) );

 		//Game
		tcGameMain->setAlpha(gameControlsAlpha);
		tcGameMain->addControl(new touchcontrols::Button("jump",touchcontrols::RectF(23,5,26,8),"jump",KEY_JUMP));
		tcGameMain->addControl(new touchcontrols::Button("crouch",touchcontrols::RectF(24,14,26,16),"crouch",KEY_CROUCH));
		tcGameMain->addControl(new touchcontrols::Button("attack",touchcontrols::RectF(20,7,23,10),"shoot",KEY_SHOOT));
		tcGameMain->addControl(new touchcontrols::Button("quick_save",touchcontrols::RectF(24,0,26,2),"save",K_F6));
		tcGameMain->addControl(new touchcontrols::Button("quick_load",touchcontrols::RectF(20,0,22,2),"load",K_F9));
		tcGameMain->addControl(new touchcontrols::Button("help_comp",touchcontrols::RectF(9,0,11,2),"gamma",K_F1));
		tcGameMain->addControl(new touchcontrols::Button("show_weapons",touchcontrols::RectF(12,14,14,16),"show_weapons",KEY_SHOW_WEAPONS));
		tcGameMain->addControl(new touchcontrols::Button("show_inventory",touchcontrols::RectF(17,0,19,2),"inv",KEY_SHOW_INV));

		nextWeapon = new touchcontrols::Button("next_weapon",touchcontrols::RectF(0,3,3,5),"next_weap",KEY_NEXT_WEAPONS);
		tcGameMain->addControl(nextWeapon);
		prevWeapon = new touchcontrols::Button("prev_weapon",touchcontrols::RectF(0,6,3,8),"prev_weap",KEY_PREV_WEAPONS);
		tcGameMain->addControl(prevWeapon);
		quickCmd = new touchcontrols::Button("quick_cmd",touchcontrols::RectF(24,3,26,5),"keyboard",KEY_QUICK_CMD,false,true);
		tcGameMain->addControl(quickCmd);

		touchJoyLeft = new touchcontrols::TouchJoy("stick",touchcontrols::RectF(0,7,8,16),"strafe_arrow");
		tcGameMain->addControl(touchJoyLeft);
		touchJoyLeft->signal_move.connect(sigc::ptr_fun(&left_stick) );
		touchJoyLeft->signal_double_tap.connect(sigc::ptr_fun(&left_double_tap) );

		touchJoyRight = new touchcontrols::TouchJoy("touch",touchcontrols::RectF(17,7,26,16),"look_arrow");
		tcGameMain->addControl(touchJoyRight);
		touchJoyRight->signal_move.connect(sigc::ptr_fun(&right_stick) );
		touchJoyRight->signal_double_tap.connect(sigc::ptr_fun(&right_double_tap) );

		tcGameMain->signal_button.connect(  sigc::ptr_fun(&gameButton) );
		tcGameMain->signal_settingsButton.connect(  sigc::ptr_fun(&gameSettingsButton) );

		//Weapons
		tcGameWeapons->addControl(new touchcontrols::Button("weapon1",touchcontrols::RectF(1,14,3,16),"key_1",'1'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon2",touchcontrols::RectF(3,14,5,16),"key_2",'2'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon3",touchcontrols::RectF(5,14,7,16),"key_3",'3'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon4",touchcontrols::RectF(7,14,9,16),"key_4",'4'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon5",touchcontrols::RectF(9,14,11,16),"key_5",'5'));

		tcGameWeapons->addControl(new touchcontrols::Button("weapon6",touchcontrols::RectF(15,14,17,16),"key_6",'6'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon7",touchcontrols::RectF(17,14,19,16),"key_7",'7'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon8",touchcontrols::RectF(19,14,21,16),"key_8",'8'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon9",touchcontrols::RectF(21,14,23,16),"key_9",'9'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon0",touchcontrols::RectF(23,14,25,16),"key_0",'0'));

		tcGameWeapons->signal_button.connect(  sigc::ptr_fun(&selectWeaponButton) );
		tcGameWeapons->setAlpha(0.5);

		//Weapon wheel
		touchcontrols::WheelSelect *wheel = new touchcontrols::WheelSelect("weapon_wheel",touchcontrols::RectF(7,2,19,14),"weapon_wheel",10);
		wheel->signal_selected.connect(sigc::ptr_fun(&weaponWheel) );
		wheel->signal_enabled.connect(sigc::ptr_fun(&weaponWheelSelected));
		tcWeaponWheel->addControl(wheel);
		tcWeaponWheel->setAlpha(0.5);

		//Inventory
		tcInventory->addControl(new touchcontrols::Button("inven",touchcontrols::RectF(1,14,3,16),"key_tab",PORT_ACT_INVEN));
		tcInventory->addControl(new touchcontrols::Button("invuse",touchcontrols::RectF(3,14,5,16),"enter",PORT_ACT_INVUSE));
		tcInventory->addControl(new touchcontrols::Button("invprev",touchcontrols::RectF(6,14,8,16),"arrow_left",PORT_ACT_INVPREV));
		tcInventory->addControl(new touchcontrols::Button("invnext",touchcontrols::RectF(8,14,10,16),"arrow_right",PORT_ACT_INVNEXT));

		tcInventory->signal_button.connect(  sigc::ptr_fun(&inventoryButton) );
		tcInventory->setAlpha(0.5);


		controlsContainer.addControlGroup(tcGameMain);
		controlsContainer.addControlGroup(tcGameWeapons);
		controlsContainer.addControlGroup(tcMenuMain);
		controlsContainer.addControlGroup(tcInventory);
		controlsContainer.addControlGroup(tcWeaponWheel);
		controlsCreated = 1;


		tcGameMain->setXMLFile(graphicpath + "/quake2_ctrl.xml");
		tcGameWeapons->setXMLFile(graphicpath + "/weapons.xml");
		tcInventory->setXMLFile(graphicpath + "/inventory.xml");
		tcWeaponWheel->setXMLFile(graphicpath + "/weaponwheel.xml");
	}
	controlsContainer.initGL();


	return 0;
}


int key_dest_last = -1;
jint EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2Frame( JNIEnv* env,
		jobject thiz )
{

	int 	time, newtime;

	quake2_jni_reset_framecount();

	// find time spent rendering last frame

	bool repeat = false;
	while (!quake2_jni_get_framecount())
	{
		do {
			newtime = Sys_Milliseconds ();
			time = newtime - oldtime;
		} while (time < 1);
		Qcommon_Frame (time);

		if (repeat)
			usleep(2000);

		repeat = true;
		//Qcommon_Frame (20);
	}

	oldtime = newtime;

	if (getKeyDest() != key_dest_last)
	{
		if (getKeyDest() == key_game)
		{
			tcGameMain->setEnabled(true);
			if (enableWeaponWheel)
				tcWeaponWheel->setEnabled(true);
			tcMenuMain->setEnabled(false);
		}
		else
		{
			tcGameMain->setEnabled(false);
			tcWeaponWheel->setEnabled(false);
			tcGameWeapons->setEnabled(false);
			tcMenuMain->setEnabled(true);
		}
		key_dest_last = getKeyDest();
	}

	weaponCycle(showWeaponCycle);
	setHideSticks(!showSticks);

	int ret;
	ret = 0;
	if (getKeyDest() == key_console || getKeyDest() == key_message )
		ret |= 1;

	//controlsContainer.draw();

#ifdef ANTI_HACK
	if (apkCheck(env))
	{
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
#endif
	//__android_log_print(ANDROID_LOG_DEBUG, "quake2-jni.c", "native code : Quake2Frame exit\n");

	return ret;
}

void drawTouchControls()
{
	controlsContainer.draw();
}

void EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_keypress(JNIEnv *env, jobject obj,
		jint down, jint keycode, jint unicode)
{
	if (controlsContainer.isEditing())
	{
		if (down && (keycode == K_ESCAPE ))
			controlsContainer.finishEditing();
		return;
	}

	PortableKeyEvent(down,keycode);
}


void EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_touchEvent(JNIEnv *env, jobject obj,
		jint action, jint pid, jfloat x, jfloat y)
{
	//LOGI("TOUCHED");
#ifdef ANTI_HACK
    if (crcCheck((library_path_string + "/libq2game.so").c_str()))
        return;
#endif
	controlsContainer.processPointer(action,pid,x,y);
}


void EXPORT_ME Java_com_beloko_opengames_quake2_Quake2Lib_doAction(JNIEnv *env, jobject obj,
		jint state, jint action)
{
	if (hideTouchControls)
	{
		if (tcGameMain->isEnabled())
			tcGameMain->animateOut(30);

		tcWeaponWheel->animateOut(1);
	}

	PortableAction(state,action);
}

void EXPORT_ME Java_com_beloko_opengames_quake2_Quake2Lib_analogFwd(JNIEnv *env, jobject obj,
		jfloat v)
{
	PortableMoveFwd(v);
}

void EXPORT_ME Java_com_beloko_opengames_quake2_Quake2Lib_analogSide(JNIEnv *env, jobject obj,
		jfloat v)
{
	PortableMoveSide(v);
}

void EXPORT_ME Java_com_beloko_opengames_quake2_Quake2Lib_analogPitch(JNIEnv *env, jobject obj,
		jint mode,jfloat v)
{
	PortableLookPitch(mode, v);
}

void EXPORT_ME Java_com_beloko_opengames_quake2_Quake2Lib_analogYaw(JNIEnv *env, jobject obj,
		jint mode,jfloat v)
{
	PortableLookYaw(mode, v);
}

void EXPORT_ME Java_com_beloko_opengames_quake2_Quake2Lib_setTouchSettings(JNIEnv *env, jobject obj,
		jfloat alpha,jfloat strafe,jfloat fwd,jfloat pitch,jfloat yaw,int other)
{
	gameControlsAlpha = alpha;
	if (tcGameMain)
		tcGameMain->setAlpha(gameControlsAlpha);

	showWeaponCycle = other & 0x1?true:false;
	turnMouseMode   = other & 0x2?true:false;
	invertLook      = other & 0x4?true:false;
	precisionShoot  = other & 0x8?true:false;
	showSticks      = other & 0x1000?true:false;
	enableWeaponWheel  = other & 0x2000?true:false;

	if (tcWeaponWheel)
		tcWeaponWheel->setEnabled(enableWeaponWheel);



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

	strafe_sens = strafe;
	forward_sens = fwd;
	pitch_sens = pitch;
	yaw_sens = yaw;

}

std::string quickCommandString;
jint EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_quickCommand(JNIEnv *env, jobject obj,
		jstring command)
{
	const char * p = env->GetStringUTFChars(command,NULL);
	quickCommandString =  std::string(p) + "\n";
	PortableCommand((char*)quickCommandString.c_str());
	env->ReleaseStringUTFChars(command, p);
}


jint EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2Quit( JNIEnv* env,
		jobject thiz )
{
	//Sys_Quit();
	CL_Shutdown ();
	Qcommon_Shutdown ();
	//fcntl (0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY);
	// 	_exit(0);
	return 0;
}


jint EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2Test( JNIEnv* env,
		jobject thiz )
{
	return 0;
}



jint EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2GetDisableScreen( JNIEnv* env,
		jobject thiz )
{
	extern int quake2_jni_disable_screen();
	return quake2_jni_disable_screen();
}


jint EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2GetVibration( JNIEnv* env,
		jobject thiz )
{
	extern int quake2_jni_get_vibration();
	return quake2_jni_get_vibration();
}

///////////////////////////

void EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2SetScreenSize( JNIEnv* env,
		jobject thiz, jint width, jint height)
{
	android_screen_width = width;
	android_screen_height = height;
}



static int width;
static int height;


void EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2SetWidth( JNIEnv* env,
		jobject thiz, jint value )
{
	width = value;
}

void EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2SetHeight( JNIEnv* env,
		jobject thiz, jint value )
{
	height = value;
}

int quake2_jni_get_width()
{
	return width;
}

int quake2_jni_get_height()
{
	return height;
}


void EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2SetOverlay( JNIEnv* env,
		jobject thiz, jint value )
{
	extern void quake2_jni_set_overlay( int value );
	quake2_jni_set_overlay( value );
}

void EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_setLibraryPath( JNIEnv* env,
		jobject thiz, jstring path )
{
	const char * p = env->GetStringUTFChars(path,NULL);
	library_path_string =  std::string(p);
	library_path = library_path_string.c_str();
}
/*************************
 * I/O
 *************************/


void EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2KeyEvent(JNIEnv* env, jobject obj,
		jint key, jint down) {


	quake2_jni_key_event( key, down );
}


void EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2MoveEvent(JNIEnv* env, jobject obj,
		jint mode,
		jint forwardmove, jint sidemove, jint upmove,
		jfloat pitch, jfloat yaw, jfloat roll ) {

	/*
	__android_log_print(ANDROID_LOG_DEBUG, "quake2-jni.c",
			"Quake2MoveEvent forwardmove= %d yaw= %.0f\n", forwardmove, yaw);
	 */

	quake2_jni_move_event( mode,
			forwardmove,  sidemove,  upmove,
			pitch, yaw, roll
	);
}



/*************************
 * Audio stuff
 *************************/

jint EXPORT_ME
Java_com_beloko_opengames_quake2_Quake2Lib_Quake2PaintAudio( JNIEnv* env,
		jobject thiz, jobject buf )
{
	extern int paint_audio (void *unused, void * stream, int len);

	void *stream;
	int len;


	stream = env->GetDirectBufferAddress(buf);
	len = env->GetDirectBufferCapacity(buf);

	//__android_log_print(ANDROID_LOG_DEBUG, "quake2-jni.c", "paint_audio %p %d\n", stream, len );

	return paint_audio ( NULL, stream, len );
}

static JNIEnv *my_getJNIEnv ( )
{
	JNIEnv *pJNIEnv ;

	if ( jvm_ && ( jvm_->GetEnv ( (void**) &pJNIEnv, JNI_VERSION_1_4 ) >= 0 ) )
	{
		return pJNIEnv ;
	}
	return NULL ;
}

}
