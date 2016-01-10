#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"DP", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "DP", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"DP", __VA_ARGS__))

#include "TouchControlsContainer.h"
#include "JNITouchControlsUtils.h"

#include "in_android.h"

typedef enum keynum_e
{
	K_TEXT			= 1, // used only for unicode character input
	K_TAB			= 9,
	K_ENTER			= 13,
	K_ESCAPE		= 27,
	K_SPACE			= 32,

	// normal keys should be passed as lowercased ascii

	K_BACKSPACE		= 127,
	K_UPARROW,
	K_DOWNARROW,
	K_LEFTARROW,
	K_RIGHTARROW,

	K_ALT,
	K_CTRL,
	K_SHIFT,

	K_F1,
	K_F2,
	K_F3,
	K_F4,
	K_F5,
	K_F6,
	K_F7,
	K_F8,
	K_F9,
	K_F10,
	K_F11,
	K_F12,

	K_INS,
	K_DEL,
	K_PGDN,
	K_PGUP,
	K_HOME,
	K_END,

	K_PAUSE,

	K_NUMLOCK,
	K_CAPSLOCK,
	K_SCROLLOCK,

	K_KP_0,
	K_KP_INS = K_KP_0,
	K_KP_1,
	K_KP_END = K_KP_1,
	K_KP_2,
	K_KP_DOWNARROW = K_KP_2,
	K_KP_3,
	K_KP_PGDN = K_KP_3,
	K_KP_4,
	K_KP_LEFTARROW = K_KP_4,
	K_KP_5,
	K_KP_6,
	K_KP_RIGHTARROW = K_KP_6,
	K_KP_7,
	K_KP_HOME = K_KP_7,
	K_KP_8,
	K_KP_UPARROW = K_KP_8,
	K_KP_9,
	K_KP_PGUP = K_KP_9,
	K_KP_PERIOD,
	K_KP_DEL = K_KP_PERIOD,
	K_KP_DIVIDE,
	K_KP_SLASH = K_KP_DIVIDE,
	K_KP_MULTIPLY,
	K_KP_MINUS,
	K_KP_PLUS,
	K_KP_ENTER,
	K_KP_EQUALS,

	K_PRINTSCREEN,

	// mouse buttons generate virtual keys

	K_MOUSE1 = 512,
	K_OTHERDEVICESBEGIN = K_MOUSE1,
	K_MOUSE2,
	K_MOUSE3,
	K_MWHEELUP,
	K_MWHEELDOWN,
	K_MOUSE4,
	K_MOUSE5,
	K_MOUSE6,
	K_MOUSE7,
	K_MOUSE8,
	K_MOUSE9,
	K_MOUSE10,
	K_MOUSE11,
	K_MOUSE12,
	K_MOUSE13,
	K_MOUSE14,
	K_MOUSE15,
	K_MOUSE16,

	//
	// joystick buttons
	//
	K_JOY1 = 768,
	K_JOY2,
	K_JOY3,
	K_JOY4,
	K_JOY5,
	K_JOY6,
	K_JOY7,
	K_JOY8,
	K_JOY9,
	K_JOY10,
	K_JOY11,
	K_JOY12,
	K_JOY13,
	K_JOY14,
	K_JOY15,
	K_JOY16,

	//
	// aux keys are for multi-buttoned joysticks to generate so they can use
	// the normal binding process
	//
	K_AUX1,
	K_AUX2,
	K_AUX3,
	K_AUX4,
	K_AUX5,
	K_AUX6,
	K_AUX7,
	K_AUX8,
	K_AUX9,
	K_AUX10,
	K_AUX11,
	K_AUX12,
	K_AUX13,
	K_AUX14,
	K_AUX15,
	K_AUX16,
	K_AUX17,
	K_AUX18,
	K_AUX19,
	K_AUX20,
	K_AUX21,
	K_AUX22,
	K_AUX23,
	K_AUX24,
	K_AUX25,
	K_AUX26,
	K_AUX27,
	K_AUX28,
	K_AUX29,
	K_AUX30,
	K_AUX31,
	K_AUX32,

	// Microsoft Xbox 360 Controller For Windows
	K_X360_DPAD_UP,
	K_X360_DPAD_DOWN,
	K_X360_DPAD_LEFT,
	K_X360_DPAD_RIGHT,
	K_X360_START,
	K_X360_BACK,
	K_X360_LEFT_THUMB,
	K_X360_RIGHT_THUMB,
	K_X360_LEFT_SHOULDER,
	K_X360_RIGHT_SHOULDER,
	K_X360_A,
	K_X360_B,
	K_X360_X,
	K_X360_Y,
	K_X360_LEFT_TRIGGER,
	K_X360_RIGHT_TRIGGER,
	K_X360_LEFT_THUMB_UP,
	K_X360_LEFT_THUMB_DOWN,
	K_X360_LEFT_THUMB_LEFT,
	K_X360_LEFT_THUMB_RIGHT,
	K_X360_RIGHT_THUMB_UP,
	K_X360_RIGHT_THUMB_DOWN,
	K_X360_RIGHT_THUMB_LEFT,
	K_X360_RIGHT_THUMB_RIGHT,

	// generic joystick emulation for menu
	K_JOY_UP,
	K_JOY_DOWN,
	K_JOY_LEFT,
	K_JOY_RIGHT,

	K_MIDINOTE0 = 896, // to this, the note number is added
	K_MIDINOTE1,
	K_MIDINOTE2,
	K_MIDINOTE3,
	K_MIDINOTE4,
	K_MIDINOTE5,
	K_MIDINOTE6,
	K_MIDINOTE7,
	K_MIDINOTE8,
	K_MIDINOTE9,
	K_MIDINOTE10,
	K_MIDINOTE11,
	K_MIDINOTE12,
	K_MIDINOTE13,
	K_MIDINOTE14,
	K_MIDINOTE15,
	K_MIDINOTE16,
	K_MIDINOTE17,
	K_MIDINOTE18,
	K_MIDINOTE19,
	K_MIDINOTE20,
	K_MIDINOTE21,
	K_MIDINOTE22,
	K_MIDINOTE23,
	K_MIDINOTE24,
	K_MIDINOTE25,
	K_MIDINOTE26,
	K_MIDINOTE27,
	K_MIDINOTE28,
	K_MIDINOTE29,
	K_MIDINOTE30,
	K_MIDINOTE31,
	K_MIDINOTE32,
	K_MIDINOTE33,
	K_MIDINOTE34,
	K_MIDINOTE35,
	K_MIDINOTE36,
	K_MIDINOTE37,
	K_MIDINOTE38,
	K_MIDINOTE39,
	K_MIDINOTE40,
	K_MIDINOTE41,
	K_MIDINOTE42,
	K_MIDINOTE43,
	K_MIDINOTE44,
	K_MIDINOTE45,
	K_MIDINOTE46,
	K_MIDINOTE47,
	K_MIDINOTE48,
	K_MIDINOTE49,
	K_MIDINOTE50,
	K_MIDINOTE51,
	K_MIDINOTE52,
	K_MIDINOTE53,
	K_MIDINOTE54,
	K_MIDINOTE55,
	K_MIDINOTE56,
	K_MIDINOTE57,
	K_MIDINOTE58,
	K_MIDINOTE59,
	K_MIDINOTE60,
	K_MIDINOTE61,
	K_MIDINOTE62,
	K_MIDINOTE63,
	K_MIDINOTE64,
	K_MIDINOTE65,
	K_MIDINOTE66,
	K_MIDINOTE67,
	K_MIDINOTE68,
	K_MIDINOTE69,
	K_MIDINOTE70,
	K_MIDINOTE71,
	K_MIDINOTE72,
	K_MIDINOTE73,
	K_MIDINOTE74,
	K_MIDINOTE75,
	K_MIDINOTE76,
	K_MIDINOTE77,
	K_MIDINOTE78,
	K_MIDINOTE79,
	K_MIDINOTE80,
	K_MIDINOTE81,
	K_MIDINOTE82,
	K_MIDINOTE83,
	K_MIDINOTE84,
	K_MIDINOTE85,
	K_MIDINOTE86,
	K_MIDINOTE87,
	K_MIDINOTE88,
	K_MIDINOTE89,
	K_MIDINOTE90,
	K_MIDINOTE91,
	K_MIDINOTE92,
	K_MIDINOTE93,
	K_MIDINOTE94,
	K_MIDINOTE95,
	K_MIDINOTE96,
	K_MIDINOTE97,
	K_MIDINOTE98,
	K_MIDINOTE99,
	K_MIDINOTE100,
	K_MIDINOTE101,
	K_MIDINOTE102,
	K_MIDINOTE103,
	K_MIDINOTE104,
	K_MIDINOTE105,
	K_MIDINOTE106,
	K_MIDINOTE107,
	K_MIDINOTE108,
	K_MIDINOTE109,
	K_MIDINOTE110,
	K_MIDINOTE111,
	K_MIDINOTE112,
	K_MIDINOTE113,
	K_MIDINOTE114,
	K_MIDINOTE115,
	K_MIDINOTE116,
	K_MIDINOTE117,
	K_MIDINOTE118,
	K_MIDINOTE119,
	K_MIDINOTE120,
	K_MIDINOTE121,
	K_MIDINOTE122,
	K_MIDINOTE123,
	K_MIDINOTE124,
	K_MIDINOTE125,
	K_MIDINOTE126,
	K_MIDINOTE127,

	MAX_KEYS
}
keynum_t;


#define KEY_SHOW_WEAPONS 0x1000
#define KEY_SHOOT        0x1001
#define KEY_JUMP         0x1002
#define KEY_NEXT_WEAPONS 0x1003
#define KEY_PREV_WEAPONS 0x1004
#define KEY_QUICK_CMD    0x1005
#define KEY_MALICE_RELOAD   0x1006
#define KEY_MALICE_USE      0x1007
#define KEY_MALICE_CYCLE    0x1008
#define KEY_MALICE_CAM      0x1009



float gameControlsAlpha = 0.5;
bool showWeaponCycle = false;
bool turnMouseMode = true;
bool invertLook = false;
bool precisionShoot = false;
bool showSticks = true;
bool hideTouchControls = true;

bool shooting = false;


static int controlsCreated = 0;
touchcontrols::TouchControlsContainer controlsContainer;

touchcontrols::TouchControls *tcMenuMain=0;
touchcontrols::TouchControls *tcGameMain=0;
touchcontrols::TouchControls *tcGameWeapons=0;
touchcontrols::TouchControls *tcInventory=0;

//So can hide and show these buttons
touchcontrols::Button *nextWeapon=0;
touchcontrols::Button *prevWeapon=0;
touchcontrols::TouchJoy *touchJoyLeft;
touchcontrols::TouchJoy *touchJoyRight;
touchcontrols::Button *quickCmd=0;


extern JNIEnv* env_;

void openGLStart();
void openGLEnd();

void gameSettingsButton(int state)
{
	if (state == 1)
	{
		showTouchSettings();
	}
}

/*

void gameSettingsButton(int state)
{
	//LOGTOUCH("gameSettingsButton %d",state);
	if (state == 1)
	{
		jclass helloWorldClass;
		jmethodID mainMethod;

		//helloWorldClass = env_->FindClass("com/beloko/idtech/QuakeTouchControlsSettings");
		jclass tmp =  env_->FindClass("com/beloko/idtech/QuakeTouchControlsSettings");
		helloWorldClass = (jclass)env_->NewGlobalRef(tmp);

		mainMethod = env_->GetStaticMethodID(helloWorldClass, "showSettings", "()V");

		env_->CallStaticVoidMethod(helloWorldClass, mainMethod);
	}
}


void showCustomCommands()
{
	jclass helloWorldClass;
	jmethodID mainMethod;
	helloWorldClass = env_->FindClass("com/beloko/idtech/QuakeCustomCommands");
	mainMethod = env_->GetStaticMethodID(helloWorldClass, "showCommands", "()V");
	env_->CallStaticVoidMethod(helloWorldClass, mainMethod);
}
 */

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
			if (!tcGameWeapons->enabled)
				tcGameWeapons->animateIn(5);
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
	else if  (code ==KEY_MALICE_RELOAD){
		if (state == 1)
			PortableCommand("impulse 12");
	}
	else if  (code ==KEY_MALICE_USE){
		if (state == 1)
			PortableCommand("impulse 13");
	}
	else if  (code ==KEY_MALICE_CYCLE){
		if (state == 1)
			PortableCommand("impulse 14");
	}
	else if  (code ==KEY_MALICE_CAM){
		if (state == 1)
			PortableCommand("impulse 30");
	}
	else
		PortableKeyEvent(state, code);
}

void menuButton(int state,int code)
{
	if (code == K_TAB) //bit of a hack
		PortableKeyEvent(state, code);
	else
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

int inMenuLast = 1;
void frameControls()
{
	if (inMenuLast != PortableInMenu())
	{
		inMenuLast = PortableInMenu();
		if (!PortableInMenu())
		{
			tcGameMain->setEnabled(true);
			tcMenuMain->setEnabled(false);
		}
		else
		{
			tcGameMain->setEnabled(false);
			tcGameWeapons->setEnabled(false);
			tcMenuMain->setEnabled(true);
		}
	}
	weaponCycle(showWeaponCycle);
	setHideSticks(!showSticks);
	controlsContainer.draw();
}

void gamepadButtonPressed()
{
	if (hideTouchControls && tcGameMain)
		if (tcGameMain->isEnabled())
			tcGameMain->animateOut(30);
}

void initControls(int width, int height,const char * graphics_path,const char *settings_file,int game)
{
	touchcontrols::GLScaleWidth = (float)width;
	touchcontrols::GLScaleHeight = (float)height;


	if (!controlsCreated)
	{
		LOGI("creating controls");

		touchcontrols::setGraphicsBasePath(graphics_path);
		setControlsContainer(&controlsContainer);

		controlsContainer.openGL_start.connect( sigc::ptr_fun(&openGLStart));
		controlsContainer.openGL_end.connect( sigc::ptr_fun(&openGLEnd));


		tcMenuMain = new touchcontrols::TouchControls("menu",true,false);
		tcGameMain = new touchcontrols::TouchControls("game",false,true,1);
		tcGameWeapons = new touchcontrols::TouchControls("weapons",false,true,1);

		tcGameMain->signal_settingsButton.connect(  sigc::ptr_fun(&gameSettingsButton) );

		//Menu
		tcMenuMain->addControl(new touchcontrols::Button("down_arrow",touchcontrols::RectF(20,13,23,16),"arrow_down",PORT_ACT_MENU_DOWN));
		tcMenuMain->addControl(new touchcontrols::Button("up_arrow",touchcontrols::RectF(20,10,23,13),"arrow_up",PORT_ACT_MENU_UP));
		tcMenuMain->addControl(new touchcontrols::Button("left_arrow",touchcontrols::RectF(17,13,20,16),"arrow_left",PORT_ACT_MENU_LEFT));
		tcMenuMain->addControl(new touchcontrols::Button("right_arrow",touchcontrols::RectF(23,13,26,16),"arrow_right",PORT_ACT_MENU_RIGHT));
		tcMenuMain->addControl(new touchcontrols::Button("enter",touchcontrols::RectF(0,12,4,16),"enter",PORT_ACT_MENU_SELECT));
		tcMenuMain->addControl(new touchcontrols::Button("tab",touchcontrols::RectF(0,0,2,2),"tab",K_TAB));
		tcMenuMain->signal_button.connect(  sigc::ptr_fun(&menuButton) );

		//Game
		tcGameMain->setAlpha(gameControlsAlpha);
		tcGameMain->addControl(new touchcontrols::Button("jump",touchcontrols::RectF(23,5,26,8),"jump",KEY_JUMP));
		tcGameMain->addControl(new touchcontrols::Button("attack",touchcontrols::RectF(20,7,23,10),"shoot",KEY_SHOOT));
		tcGameMain->addControl(new touchcontrols::Button("quick_save",touchcontrols::RectF(24,0,26,2),"save",K_F6));
		tcGameMain->addControl(new touchcontrols::Button("quick_load",touchcontrols::RectF(20,0,22,2),"load",K_F9));
		tcGameMain->addControl(new touchcontrols::Button("show_weapons",touchcontrols::RectF(12,14,14,16),"show_weapons",KEY_SHOW_WEAPONS));
		nextWeapon = new touchcontrols::Button("next_weapon",touchcontrols::RectF(0,3,3,5),"next_weap",KEY_NEXT_WEAPONS);
		tcGameMain->addControl(nextWeapon);
		prevWeapon = new touchcontrols::Button("prev_weapon",touchcontrols::RectF(0,6,3,8),"prev_weap",KEY_PREV_WEAPONS);
		tcGameMain->addControl(prevWeapon);
		quickCmd = new touchcontrols::Button("quick_cmd",touchcontrols::RectF(24,3,26,5),"prompt",KEY_QUICK_CMD,false,true);
		tcGameMain->addControl(quickCmd);

		if (game == 1) //Malice
		{
			tcGameMain->addControl(new touchcontrols::Button("malice_reload",touchcontrols::RectF(0,5,3,6),"reload",KEY_MALICE_RELOAD));
			tcGameMain->addControl(new touchcontrols::Button("malice_use",touchcontrols::RectF(22,3,24,5),"use",KEY_MALICE_USE));
			tcGameMain->addControl(new touchcontrols::Button("malice_cycle",touchcontrols::RectF(15,0,17,2),"cycle",KEY_MALICE_CYCLE));
			tcGameMain->addControl(new touchcontrols::Button("malice_cam",touchcontrols::RectF(17,0,19,2),"camera",KEY_MALICE_CAM));

		}

		//tcGameMain->addControl(new touchcontrols::Button("show_inven",touchcontrols::RectF(0,4,2,6),"tab",K_TAB));
		//tcGameMain->addControl(new touchcontrols::Button("left_inven",touchcontrols::RectF(0,6,2,8),"tab",'['));
		//tcGameMain->addControl(new touchcontrols::Button("right_inven",touchcontrols::RectF(2,6,4,8),"tab",']'));
		//tcGameMain->addControl(new touchcontrols::Button("select_inven",touchcontrols::RectF(2,4,4,6),"enter",K_ENTER));

		touchJoyLeft = new touchcontrols::TouchJoy("stick",touchcontrols::RectF(0,7,8,16),"strafe_arrow");
		tcGameMain->addControl(touchJoyLeft);
		touchJoyLeft->signal_move.connect(sigc::ptr_fun(&left_stick) );
		touchJoyLeft->signal_double_tap.connect(sigc::ptr_fun(&left_double_tap) );

		touchJoyRight = new touchcontrols::TouchJoy("touch",touchcontrols::RectF(17,7,26,16),"look_arrow");
		tcGameMain->addControl(touchJoyRight);
		touchJoyRight->signal_move.connect(sigc::ptr_fun(&right_stick) );
		touchJoyRight->signal_double_tap.connect(sigc::ptr_fun(&right_double_tap) );

		tcGameMain->signal_button.connect(  sigc::ptr_fun(&gameButton) );

		//Weapons
		tcGameWeapons->addControl(new touchcontrols::Button("weapon1",touchcontrols::RectF(1,14,3,16),"key_1",'1'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon2",touchcontrols::RectF(4,14,6,16),"key_2",'2'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon3",touchcontrols::RectF(7,14,9,16),"key_3",'3'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon4",touchcontrols::RectF(10,14,12,16),"key_4",'4'));

		tcGameWeapons->addControl(new touchcontrols::Button("weapon5",touchcontrols::RectF(14,14,16,16),"key_5",'5'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon6",touchcontrols::RectF(17,14,19,16),"key_6",'6'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon7",touchcontrols::RectF(20,14,22,16),"key_7",'7'));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon8",touchcontrols::RectF(23,14,25,16),"key_8",'8'));
		tcGameWeapons->signal_button.connect(  sigc::ptr_fun(&selectWeaponButton) );
		tcGameWeapons->setAlpha(0.5);

		controlsContainer.addControlGroup(tcGameMain);
		controlsContainer.addControlGroup(tcGameWeapons);
		controlsContainer.addControlGroup(tcMenuMain);
		controlsCreated = 1;


		tcGameMain->setXMLFile((std::string)graphics_path +  "/darkplaces.xml");
		tcGameWeapons->setXMLFile((std::string)graphics_path +  "/weapons.xml");
	}
	else
		LOGI("NOT creating controls");

	controlsContainer.initGL();
}



void touchControls(int action, int pid, float x, float y)
{

	controlsContainer.processPointer(action,pid,x,y);
}


void setTouchSettings(float alpha,float strafe,float fwd,float pitch,float yaw,int other)
{

	gameControlsAlpha = alpha;
	if (tcGameMain)
		tcGameMain->setAlpha(gameControlsAlpha);

	showWeaponCycle = other & 0x1?true:false;
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

	strafe_sens = strafe;
	forward_sens = fwd;
	pitch_sens = pitch;
	yaw_sens = yaw;

}
