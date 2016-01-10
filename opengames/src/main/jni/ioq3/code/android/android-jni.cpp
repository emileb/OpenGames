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

#ifdef ANTI_HACK
#include "s-setup/s-setup.h"
#endif

#include "SDL_android_extra.h"

#include "TouchControlsContainer.h"
#include "JNITouchControlsUtils.h"

extern "C"
{

#include "code/client/keycodes.h"

#include "in_android.h"

#include "../GL/gl.h"
#include "../GL/nano_gl.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"JNI", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "JNI", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"JNI", __VA_ARGS__))

#define JAVA_FUNC(x) Java_com_beloko_idtech_quake3_NativeLib_##x

int android_screen_width;
int android_screen_height;


#define KEY_SHOW_WEAPONS 0x1000
#define KEY_SHOOT        0x1001
#define KEY_JUMP         0x1002
#define KEY_NEXT_WEAPONS 0x1003
#define KEY_PREV_WEAPONS 0x1004
#define KEY_CROUCH       0x1005
#define KEY_SHOW_INV     0x1006
#define KEY_QUICK_CMD    0x1007
#define KEY_USE          0x1008
#define KEY_SHOW_KEYB    0x1009

#define KEY_QUICK_KEY1    0x100B
#define KEY_QUICK_KEY2    0x100C
#define KEY_QUICK_KEY3    0x100D
#define KEY_QUICK_KEY4    0x100E


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
touchcontrols::TouchControls *tcWeaponWheel=0;

//So can hide and show these buttons
touchcontrols::Button *nextWeapon=0;
touchcontrols::Button *prevWeapon=0;
touchcontrols::TouchJoy *touchJoyLeft;
touchcontrols::TouchJoy *touchJoyRight;

extern JNIEnv* env_;

void openGLStart()
{

	glPushMatrix();
	//glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
	//LOGI("openGLStart");
	glDisable(GL_ALPHA_TEST);
	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY );

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable (GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();


}

void openGLEnd()
{

	glPopMatrix();

	glPopMatrix();

}


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
	else if  (code == KEY_USE)
	{
		PortableAction(state,PORT_ACT_USE);
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
	else if  (code == KEY_JUMP)
	{
		PortableAction(state,PORT_ACT_JUMP);
	}
	else if  (code == KEY_CROUCH)
	{
		PortableAction(state,PORT_ACT_DOWN);
	}
	else if  (code == KEY_QUICK_CMD){
		//if (state == 1)
		//	showCustomCommands();
		PortableKeyEvent(state, K_CONSOLE, 0);
		if (state)
			toggleKeyboard();
	}
	else if  (code == PORT_ACT_ZOOM_IN)
	{
		PortableAction(state,PORT_ACT_ZOOM_IN);
	}
	else if  (code == KEY_QUICK_KEY1)
		PortableKeyEvent(state, K_F1, 0);
	else if  (code == KEY_QUICK_KEY2)
		PortableKeyEvent(state, K_F2, 0);
	else if  (code == KEY_QUICK_KEY3)
		PortableKeyEvent(state, K_F3, 0);
	else if  (code == KEY_QUICK_KEY4)
		PortableKeyEvent(state, K_F4, 0);
	else
	{
		PortableKeyEvent(state, code, 0);
	}
}

void automapButton(int state,int code)
{
	PortableAction(state,code);
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

	PortableKeyEvent(1, '1' + segment, 0);
	PortableKeyEvent(0, '1' + segment, 0);

}

void menuButton(int state,int code)
{
	if (code == KEY_SHOW_KEYB)
	{
		if (state)
			toggleKeyboard();
		return;
	}
	PortableKeyEvent(state, code, 0);
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

void mouse_move(int action,float x, float y,float mouse_x, float mouse_y)
{

	PortableMouse(mouse_x,mouse_y);
	//PortableMouseAbs(x * 640,y * 480);
	/*
	if (action == TOUCHMOUSE_TAP)
	{
		PortableKeyEvent(1, K_MOUSE1, 0);
		PortableKeyEvent(0, K_MOUSE1, 0);
	}
	 */
}

//To be set by android
float strafe_sens,forward_sens;
float pitch_sens,yaw_sens;

void left_stick(float joy_x, float joy_y,float mouse_x, float mouse_y)
{
	joy_x *=10;
	//float strafe = joy_x*joy_x;
	float strafe = joy_x;
	//if (joy_x < 0)
	//	strafe *= -1;

	PortableMove(joy_y * 15 * forward_sens,-strafe * strafe_sens);
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
	PortableKeyEvent(state, code, 0);
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


void initControls(int width, int height,const char * graphics_path,const char *settings_file)
{
	touchcontrols::GLScaleWidth = (float)width;
	touchcontrols::GLScaleHeight = (float)height;

	LOGI("initControls %d x %d,x path = %s, settings = %s",width,height,graphics_path,settings_file);

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
		tcWeaponWheel = new touchcontrols::TouchControls("weapon_wheel",false,true,1);

		tcGameMain->signal_settingsButton.connect(  sigc::ptr_fun(&gameSettingsButton) );

		//Menu
		tcMenuMain->addControl(new touchcontrols::Button("down_arrow",touchcontrols::RectF(1,9,3,11),"arrow_down",K_DOWNARROW));
		tcMenuMain->addControl(new touchcontrols::Button("up_arrow",touchcontrols::RectF(1,5,3,7),"arrow_up",K_UPARROW));
		tcMenuMain->addControl(new touchcontrols::Button("left_arrow",touchcontrols::RectF(0,7,2,9),"arrow_left",K_LEFTARROW));
		tcMenuMain->addControl(new touchcontrols::Button("right_arrow",touchcontrols::RectF(2,7,4,9),"arrow_right",K_RIGHTARROW));
		tcMenuMain->addControl(new touchcontrols::Button("enter",touchcontrols::RectF(0,13,3,16),"enter",K_ENTER));
		tcMenuMain->addControl(new touchcontrols::Button("mouse",touchcontrols::RectF(23,13,26,16),"mouse",K_MOUSE1));
		tcMenuMain->addControl(new touchcontrols::Button("esc",touchcontrols::RectF(0,0,3,3),"esc",K_ESCAPE));
		tcMenuMain->addControl(new touchcontrols::Button("keyboard",touchcontrols::RectF(23,0,26,3),"keyboard",KEY_SHOW_KEYB));
		//tcMenuMain->addControl(new touchcontrols::Button("tab",touchcontrols::RectF(0,0,2,2),"tab",K_TAB));
		tcMenuMain->signal_button.connect(  sigc::ptr_fun(&menuButton) );


		touchcontrols::Mouse *mouse = new touchcontrols::Mouse("mouse",touchcontrols::RectF(0,0,26,16),"");
		mouse->setHideGraphics(true);
		tcMenuMain->addControl(mouse);
		mouse->signal_action.connect(sigc::ptr_fun(&mouse_move) );

		tcMenuMain->setAlpha(0.8);


		//Game
		tcGameMain->setAlpha(gameControlsAlpha);
		tcGameMain->addControl(new touchcontrols::Button("jump",touchcontrols::RectF(23,5,26,8),"jump",KEY_JUMP));
		tcGameMain->addControl(new touchcontrols::Button("crouch",touchcontrols::RectF(24,14,26,16),"crouch",KEY_CROUCH));
		tcGameMain->addControl(new touchcontrols::Button("attack",touchcontrols::RectF(20,7,23,10),"shoot",KEY_SHOOT));
		tcGameMain->addControl(new touchcontrols::Button("show_weapons",touchcontrols::RectF(12,14,14,16),"show_weapons",KEY_SHOW_WEAPONS));
		tcGameMain->addControl(new touchcontrols::Button("use_inventory",touchcontrols::RectF(24,3,26,5),"inv",KEY_USE));
		tcGameMain->addControl( new touchcontrols::Button("quick_cmd",touchcontrols::RectF(24,0,26,2),"keyboard",KEY_QUICK_CMD));
		tcGameMain->addControl( new touchcontrols::Button("zoom",touchcontrols::RectF(22,3,24,5),"binocular",PORT_ACT_ZOOM_IN));

		nextWeapon = new touchcontrols::Button("next_weapon",touchcontrols::RectF(0,3,3,5),"next_weap",KEY_NEXT_WEAPONS);
		tcGameMain->addControl(nextWeapon);
		prevWeapon = new touchcontrols::Button("prev_weapon",touchcontrols::RectF(0,6,3,8),"prev_weap",KEY_PREV_WEAPONS);
		tcGameMain->addControl(prevWeapon);

		tcGameMain->addControl(new touchcontrols::Button("quick_key_1",touchcontrols::RectF(4,3,6,5),"quick_key_1",KEY_QUICK_KEY1,false,true));
		tcGameMain->addControl(new touchcontrols::Button("quick_key_2",touchcontrols::RectF(6,3,8,5),"quick_key_2",KEY_QUICK_KEY2,false,true));
		tcGameMain->addControl(new touchcontrols::Button("quick_key_3",touchcontrols::RectF(8,3,10,5),"quick_key_3",KEY_QUICK_KEY3,false,true));
		tcGameMain->addControl(new touchcontrols::Button("quick_key_4",touchcontrols::RectF(10,3,12,5),"quick_key_4",KEY_QUICK_KEY4,false,true));


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
		tcGameWeapons->setAlpha(0.8);

		//Weapon wheel
		touchcontrols::WheelSelect *wheel = new touchcontrols::WheelSelect("weapon_wheel",touchcontrols::RectF(7,2,19,14),"weapon_wheel",9);
		wheel->signal_selected.connect(sigc::ptr_fun(&weaponWheel) );
		wheel->signal_enabled.connect(sigc::ptr_fun(&weaponWheelSelected));
		tcWeaponWheel->addControl(wheel);
		tcWeaponWheel->setAlpha(0.5);

		controlsContainer.addControlGroup(tcGameMain);
		controlsContainer.addControlGroup(tcGameWeapons);
		controlsContainer.addControlGroup(tcMenuMain);
		controlsContainer.addControlGroup(tcWeaponWheel);
		controlsCreated = 1;

		tcGameMain->setXMLFile(settings_file);
		tcWeaponWheel->setXMLFile((std::string)graphics_path +  "/weaponwheel.xml");
		tcGameWeapons->setXMLFile((std::string)graphics_path +  "/weapons.xml");
	}
	else
		LOGI("NOT creating controls");

	controlsContainer.initGL();
}

int inMenuLast = 1;
int inAutomapLast = 0;
void frameControls()
{
#ifdef ANTI_HACK
	if ((rand() % 100) == 50)
		check_rsa_key();

#endif

	int inMenuNew = PortableInMenu();
	if (inMenuLast != inMenuNew)
	{
		inMenuLast = inMenuNew;
		if (!inMenuNew)
		{
			tcGameMain->setEnabled(true);
			tcWeaponWheel->setEnabled(true);
			tcMenuMain->setEnabled(false);
		}
		else
		{
			tcGameMain->setEnabled(false);
			tcGameWeapons->setEnabled(false);
			tcWeaponWheel->setEnabled(false);
			tcMenuMain->setEnabled(true);
		}
	}

#ifdef ANTI_HACK
	if (rsa_key_fail)
	{
		//LOGI("%d %d %d %d",ufile_fail,rsa_key_fail,setup_not_run_fail,hash_fail);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
#endif
	weaponCycle(showWeaponCycle);
	setHideSticks(!showSticks);
	controlsContainer.draw();
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

int quit_now = 0;

#define EXPORT_ME __attribute__ ((visibility("default")))

JNIEnv* env_;

int argc=1;
const char * argv[32];
std::string graphicpath;


std::string game_path;

const char * getGamePath()
{
	return game_path.c_str();
}

jint EXPORT_ME
JAVA_FUNC(init) ( JNIEnv* env,	jobject thiz,jstring graphics_dir,jint mem_mb,jobjectArray argsArray,jint renderer,jstring game_path_ )
{
	env_ = env;
#ifdef ANTI_HACK
	getGlobalClasses(env);
#endif
	argv[0] = "quake";
	int argCount = (env)->GetArrayLength( argsArray);
	LOGI("argCount = %d",argCount);
	for (int i=0; i<argCount; i++) {
		jstring string = (jstring) (env)->GetObjectArrayElement( argsArray, i);
		argv[argc] = (char *)(env)->GetStringUTFChars( string, 0);
		LOGI("arg = %s",argv[argc]);
		argc++;
	}


	game_path = (char *)(env)->GetStringUTFChars( game_path_, 0);

	LOGI("game_path = %s",getGamePath());


	PortableInit(argc,argv);

	const char * p = env->GetStringUTFChars(graphics_dir,NULL);
	graphicpath =  std::string(p);



	initControls(android_screen_width,-android_screen_height,graphicpath.c_str(),(graphicpath + "/game_controls.xml").c_str());

	SDL_SwapBufferPerformsSwap(false);

	return 0;
}

extern int androidSwapped;

jint EXPORT_ME
JAVA_FUNC(frame) ( JNIEnv* env,	jobject thiz )
{

	//LOGI("frame");

	androidSwapped = 1;

	PortableFrame();

	if (quit_now)
	{
		LOGI("frame QUIT");
		return 128;
	}

	frameControls();

	int ret = 0;

	return ret;
}

__attribute__((visibility("default"))) jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	LOGI("JNI_OnLoad");
	setTCJNIEnv(vm);

	return JNI_VERSION_1_4;
}


void EXPORT_ME
JAVA_FUNC(keypress) (JNIEnv *env, jobject obj,	jint down, jint keycode, jint unicode)
{
	LOGI("keypress %d",keycode);
	if (controlsContainer.isEditing())
	{
		if (down && (keycode == K_ESCAPE ))
			controlsContainer.finishEditing();
		return;
	}

	PortableKeyEvent(down,keycode,unicode);
}


void EXPORT_ME
JAVA_FUNC(touchEvent) (JNIEnv *env, jobject obj,jint action, jint pid, jfloat x, jfloat y)
{
	//LOGI("TOUCHED");
	controlsContainer.processPointer(action,pid,x,y);
}


void EXPORT_ME
JAVA_FUNC(doAction) (JNIEnv *env, jobject obj,	jint state, jint action)
{
	//gamepadButtonPressed();
	if (hideTouchControls)
		if (tcGameMain->isEnabled())
			tcGameMain->animateOut(30);
	LOGI("doAction %d %d",state,action);
	PortableAction(state,action);
}

void EXPORT_ME
JAVA_FUNC(analogFwd) (JNIEnv *env, jobject obj,	jfloat v)
{
	PortableMoveFwd(v);
}

void EXPORT_ME
JAVA_FUNC(analogSide) (JNIEnv *env, jobject obj,	jfloat v)
{
	PortableMoveSide(v);
}

void EXPORT_ME
JAVA_FUNC(analogPitch) (JNIEnv *env, jobject obj,	jint mode,jfloat v)
{
	PortableLookPitch(mode, v);
}

void EXPORT_ME
JAVA_FUNC(analogYaw) (JNIEnv *env, jobject obj,	jint mode,jfloat v)
{
	PortableLookYaw(mode, v);
}

void EXPORT_ME
JAVA_FUNC(setTouchSettings) (JNIEnv *env, jobject obj,	jfloat alpha,jfloat strafe,jfloat fwd,jfloat pitch,jfloat yaw,int other)
{
	setTouchSettings(alpha,strafe,fwd,pitch,yaw,other);
}

std::string quickCommandString;
jint EXPORT_ME
JAVA_FUNC(quickCommand) (JNIEnv *env, jobject obj,	jstring command)
{
	const char * p = env->GetStringUTFChars(command,NULL);
	quickCommandString =  std::string(p) + "\n";
	env->ReleaseStringUTFChars(command, p);
	PortableCommand(quickCommandString.c_str());
	return 0;
}




void EXPORT_ME
JAVA_FUNC(setScreenSize) ( JNIEnv* env,	jobject thiz, jint width, jint height)
{
	android_screen_width = width;
	android_screen_height = height;
}


}
