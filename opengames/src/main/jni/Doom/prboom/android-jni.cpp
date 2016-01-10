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
#include <unistd.h>

#ifdef ANTI_HACK
#include "s-setup/s-setup.h"
#endif
#include "SDL_android_extra.h"

#include "TouchControlsContainer.h"
#include "JNITouchControlsUtils.h"


extern "C"
{

#include "doomdef.h"

#include "in_android.h"

#include "../GL/gl.h"
#include "../GL/nano_gl.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"PRBOOM", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "PRBOOM", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"PRBOOM", __VA_ARGS__))


#define REND_SOFT  0
#define REND_GL    1

static int curRenderer;

int android_screen_width;
int android_screen_height;


#define KEY_SHOW_WEAPONS 0x1000
#define KEY_SHOOT        0x1001
#define KEY_JUMP         0x1002
#define KEY_NEXT_WEAPONS 0x1003
#define KEY_PREV_WEAPONS 0x1004
#define KEY_QUICK_CMD    0x1005
#define KEY_USE          0x1006
#define KEY_MAP          0x1007
#define KEY_SHOW_KBRD    0x1008


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
touchcontrols::TouchControls *tcAutomap=0;
touchcontrols::TouchControls *tcWeaponWheel=0;

//So can hide and show these buttons
touchcontrols::Button *nextWeapon=0;
touchcontrols::Button *prevWeapon=0;
touchcontrols::TouchJoy *touchJoyLeft;
touchcontrols::TouchJoy *touchJoyRight;
touchcontrols::WheelSelect *weaponWheel;

extern JNIEnv* env_;
JavaVM* jvm_;

void openGLStart()
{
	if (curRenderer == REND_GL)
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
		//glEnable (GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();

		nanoPushState();
	}
	else //software mode
	{

		glDisable(GL_ALPHA_TEST);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY );
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable (GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		glMatrixMode(GL_MODELVIEW);

	}

}

void openGLEnd()
{
	if (curRenderer == REND_GL)
	{
		glPopMatrix();
		nanoPopState();
		glPopMatrix();
	}
	else// Software mode
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
}

void gameSettingsButton(int state)
{
	if (state == 1)
	{
		showTouchSettings();
	}
}



int showKeyboardInt = 0;
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
	else if  (code == KEY_MAP)
	{
		PortableAction(state,PORT_ACT_MAP);
	}
	else if  (code == KEY_SHOW_KBRD)
	{
		if (state = 1)
			showKeyboardInt = 1;
	}
	else if  (code == KEY_QUICK_CMD){
		if (state == 1)
			showCustomCommands();
	}
	else
	{
		PortableKeyEvent(state, code);
	}
}

void automapButton(int state,int code)
{
	PortableAction(state,code);
}


void menuButton(int state,int code)
{
	if  (code == KEY_SHOW_KBRD)
	{
		if (state = 1)
			showKeyboardInt = 1;
	}
	else
		PortableKeyEvent(state, code);
}

//Weapon wheel callbacks
void weaponWheelSelected(int enabled)
{
	//int weapons = cg.snap->ps.stats[ STAT_WEAPONS ];
	int weapons = 0xFFFF;

	for (int n=0;n<8;n++)
	{
		if (weapons & 1<<n)
			weaponWheel->setSegmentEnabled(n,true);
		else
			weaponWheel->setSegmentEnabled(n,false);
	}

	if (enabled)
		tcWeaponWheel->fade(touchcontrols::FADE_IN,5); //fade in

}

void weaponWheelChosen(int segment)
{
	LOGI("weaponWheelChosen %d",segment);
	//PortableKeyEvent(1, '1' + segment);
	//PortableKeyEvent(0, '1' + segment);
	PortableAction(1,PORT_ACT_WEAP1 + segment);
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
		tcAutomap  = new touchcontrols::TouchControls("automap",false,true,1);
		tcWeaponWheel = new touchcontrols::TouchControls("weapon_wheel",false,true,1);

		tcGameMain->signal_settingsButton.connect(  sigc::ptr_fun(&gameSettingsButton) );

		//Weapon wheel
		weaponWheel = new touchcontrols::WheelSelect("weapon_wheel",touchcontrols::RectF(7,2,19,14),"weapon_wheel",8);
		weaponWheel->signal_selected.connect(sigc::ptr_fun(&weaponWheelChosen) );
		weaponWheel->signal_enabled.connect(sigc::ptr_fun(&weaponWheelSelected));

		tcWeaponWheel->addControl(weaponWheel);
		tcWeaponWheel->setAlpha(0.8);

		//Menu
		tcMenuMain->addControl(new touchcontrols::Button("down_arrow",touchcontrols::RectF(20,13,23,16),"arrow_down",KEYD_DOWNARROW));
		tcMenuMain->addControl(new touchcontrols::Button("up_arrow",touchcontrols::RectF(20,10,23,13),"arrow_up",KEYD_UPARROW));
		tcMenuMain->addControl(new touchcontrols::Button("left_arrow",touchcontrols::RectF(17,13,20,16),"arrow_left",KEYD_LEFTARROW));
		tcMenuMain->addControl(new touchcontrols::Button("right_arrow",touchcontrols::RectF(23,13,26,16),"arrow_right",KEYD_RIGHTARROW));
		tcMenuMain->addControl(new touchcontrols::Button("enter",touchcontrols::RectF(0,12,4,16),"enter",KEYD_ENTER));
		tcMenuMain->addControl(new touchcontrols::Button("keyboard",touchcontrols::RectF(0,0,3,3),"keyboard",KEY_SHOW_KBRD));


		tcMenuMain->signal_button.connect(  sigc::ptr_fun(&menuButton) );
		tcMenuMain->setAlpha(0.8);

		//Automap
		tcAutomap->addControl(new touchcontrols::Button("down_arrow",touchcontrols::RectF(2,14,4,16),"arrow_down",PORT_ACT_MAP_DOWN));
		tcAutomap->addControl(new touchcontrols::Button("up_arrow",touchcontrols::RectF(2,10,4,12),"arrow_up",PORT_ACT_MAP_UP));
		tcAutomap->addControl(new touchcontrols::Button("left_arrow",touchcontrols::RectF(0,12,2,14),"arrow_left",PORT_ACT_MAP_LEFT));
		tcAutomap->addControl(new touchcontrols::Button("right_arrow",touchcontrols::RectF(4,12,6,14),"arrow_right",PORT_ACT_MAP_RIGHT));
		tcAutomap->addControl(new touchcontrols::Button("zoom_in",touchcontrols::RectF(4,10,6,12),"key_+",PORT_ACT_MAP_ZOOM_IN));
		tcAutomap->addControl(new touchcontrols::Button("zoom_out",touchcontrols::RectF(4,14,6,16),"key_-",PORT_ACT_MAP_ZOOM_OUT));

		tcAutomap->signal_button.connect(  sigc::ptr_fun(&automapButton) );
		tcAutomap->setAlpha(0.8);

		//Game
		tcGameMain->setAlpha(gameControlsAlpha);
		tcGameMain->addControl(new touchcontrols::Button("use",touchcontrols::RectF(23,5,26,8),"use",KEY_USE));
		tcGameMain->addControl(new touchcontrols::Button("attack",touchcontrols::RectF(20,7,23,10),"shoot",KEY_SHOOT));
		tcGameMain->addControl(new touchcontrols::Button("quick_save",touchcontrols::RectF(24,0,26,2),"save",KEYD_F6));
		tcGameMain->addControl(new touchcontrols::Button("quick_load",touchcontrols::RectF(20,0,22,2),"load",KEYD_F9));
		tcGameMain->addControl(new touchcontrols::Button("gamma",touchcontrols::RectF(16,0,18,2),"gamma",KEYD_F11));
		tcGameMain->addControl(new touchcontrols::Button("map",touchcontrols::RectF(4,0,6,2),"map",KEY_MAP));
		tcGameMain->addControl(new touchcontrols::Button("keyboard",touchcontrols::RectF(8,0,10,2),"keyboard",KEY_SHOW_KBRD));

		tcGameMain->addControl(new touchcontrols::Button("show_weapons",touchcontrols::RectF(12,14,14,16),"show_weapons",KEY_SHOW_WEAPONS));
		nextWeapon = new touchcontrols::Button("next_weapon",touchcontrols::RectF(0,3,3,5),"next_weap",KEY_NEXT_WEAPONS);
		tcGameMain->addControl(nextWeapon);
		prevWeapon = new touchcontrols::Button("prev_weapon",touchcontrols::RectF(0,6,3,8),"prev_weap",KEY_PREV_WEAPONS);
		tcGameMain->addControl(prevWeapon);

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
		tcGameWeapons->setAlpha(0.8);

		controlsContainer.addControlGroup(tcGameMain);
		controlsContainer.addControlGroup(tcGameWeapons);
		controlsContainer.addControlGroup(tcMenuMain);
		controlsContainer.addControlGroup(tcAutomap);
		controlsContainer.addControlGroup(tcWeaponWheel);
		controlsCreated = 1;

		tcGameMain->setXMLFile((std::string)graphics_path +  "/prdoom.xml");
		tcGameWeapons->setXMLFile((std::string)graphics_path +  "/weapons.xml");
		tcAutomap->setXMLFile((std::string)graphics_path +  "/automap.xml");
		tcWeaponWheel->setXMLFile((std::string)graphics_path +  "/weaponwheel.xml");

	}
	else
		LOGI("NOT creating controls");

	controlsContainer.initGL();
}

int inMenuLast = 1;
int inAutomapLast = 0;
void frameControls()
{
	int inMenuNew = PortableInMenu();
	if (inMenuLast != inMenuNew)
	{
		inMenuLast = inMenuNew;
		if (!inMenuNew)
		{
			tcGameMain->setEnabled(true);
			tcMenuMain->setEnabled(false);
			if (enableWeaponWheel)
				tcWeaponWheel->setEnabled(true);
		}
		else
		{
			tcGameMain->setEnabled(false);
			tcGameWeapons->setEnabled(false);
			tcWeaponWheel->setEnabled(false);
			tcMenuMain->setEnabled(true);
		}
	}

	int inAutomapNew =  PortableInAutomap() && !inMenuLast; //Dont show if menu comes up
	if (inAutomapLast != inAutomapNew)
	{
		inAutomapLast = inAutomapNew;
		if (inAutomapNew)
		{
			tcAutomap->animateIn(5);
		}
		else
		{
			tcAutomap->animateOut(5);
		}
	}

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

int quit_now = 0;

#define EXPORT_ME __attribute__ ((visibility("default")))

JNIEnv* env_;

int argc=1;
const char * argv[32];
std::string graphicpath;


std::string doom_path;

const char * getDoomPath()
{
	return doom_path.c_str();
}

#ifdef PRBOOM_SERVER
extern int server_main(int argc,const char** argv);

jint EXPORT_ME
Java_com_beloko_opengames_prboom_NativeLib_startserver( JNIEnv* env,
		jobject thiz,jobjectArray argsArray,jstring doom_path_ )
{

	argv[0] = "quake";
	int argCount = (env)->GetArrayLength( argsArray);
	LOGI("argCount = %d",argCount);
	for (int i=0; i<argCount; i++) {
		jstring string = (jstring) (env)->GetObjectArrayElement( argsArray, i);
		argv[argc] = (char *)(env)->GetStringUTFChars( string, 0);
		LOGI("arg = %s",argv[argc]);
		argc++;
	}

	doom_path = (char *)(env)->GetStringUTFChars( doom_path_, 0);

	LOGI("doom_path = %s",getDoomPath());

	chdir(getDoomPath());
	putenv("TIMIDITY_CFG=../timidity.cfg");

	server_main(argc,argv);
}
#endif
//in_android.c
extern int ANDROID_SAMPLECOUNT;
extern void Android_SetGameResolution(int width, int height);

jint EXPORT_ME
Java_com_beloko_opengames_prboom_NativeLib_init( JNIEnv* env,
		jobject thiz,jstring graphics_dir,jint mem_mb,jobjectArray argsArray,jint renderer,jstring doom_path_,jint snd_buffer_size )
{
	env_ = env;
	curRenderer = renderer;

	argv[0] = "quake";
	int argCount = (env)->GetArrayLength( argsArray);
	LOGI("argCount = %d",argCount);
	for (int i=0; i<argCount; i++) {
		jstring string = (jstring) (env)->GetObjectArrayElement( argsArray, i);
		argv[argc] = (char *)(env)->GetStringUTFChars( string, 0);
		LOGI("arg = %s",argv[argc]);
		argc++;
	}

	Android_SetGameResolution(320,240);

	ANDROID_SAMPLECOUNT = snd_buffer_size;

	doom_path = (char *)(env)->GetStringUTFChars( doom_path_, 0);

	LOGI("doom_path = %s",getDoomPath());

	chdir(getDoomPath());
	putenv("TIMIDITY_CFG=../timidity.cfg");

	PortableInit(argc,argv);

	const char * p = env->GetStringUTFChars(graphics_dir,NULL);
	graphicpath =  std::string(p);



	initControls(android_screen_width,-android_screen_height,graphicpath.c_str(),(graphicpath + "/prdoom.xml").c_str());

#ifdef ANTI_HACK
	getGlobalClasses(env_);
#endif

	if (renderer != REND_SOFT)
		SDL_SetSwapBufferCallBack(frameControls);

	if (renderer == REND_SOFT)// In soft mode SDL calls swap buffer, disable so it does not flicker
		SDL_SwapBufferPerformsSwap(false);

	return 0;
}


jint EXPORT_ME
Java_com_beloko_opengames_prboom_NativeLib_frame( JNIEnv* env,	jobject thiz )
{

	//LOGI("Java_com_beloko_opengames_prboom_NativeLib_frame");

	PortableFrame();

	if (quit_now)
	{
		LOGI("frame QUIT");
		return 128;
	}

	frameControls();

#ifdef ANTI_HACK
	if (rsa_key_fail || setup_not_run_fail || hash_fail || ufile_fail)
	{
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
#endif
	int ret = 0;

	if (showKeyboardInt)
	{
		ret |= 1;
		showKeyboardInt = 0;
	}

	return ret;
}

__attribute__((visibility("default"))) jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	LOGI("JNI_OnLoad");
	jvm_ = vm;
	setTCJNIEnv(jvm_);

	return JNI_VERSION_1_4;
}


void EXPORT_ME
Java_com_beloko_opengames_prboom_NativeLib_keypress(JNIEnv *env, jobject obj,
		jint down, jint keycode, jint unicode)
{
	LOGI("keypress %d",keycode);
	if (controlsContainer.isEditing())
	{
		if (down && (keycode == KEYD_ESCAPE ))
			controlsContainer.finishEditing();
		return;
	}
	PortableKeyEvent(down,keycode);

}


void EXPORT_ME
Java_com_beloko_opengames_prboom_NativeLib_touchEvent(JNIEnv *env, jobject obj,
		jint action, jint pid, jfloat x, jfloat y)
{
	//LOGI("TOUCHED");
	controlsContainer.processPointer(action,pid,x,y);
}


void EXPORT_ME Java_com_beloko_opengames_prboom_NativeLib_doAction(JNIEnv *env, jobject obj,
		jint state, jint action)
{
	LOGI("doAction %d %d",state,action);

	if (hideTouchControls)
		if (tcGameMain)
		{
			if (tcGameMain->isEnabled())
				tcGameMain->animateOut(30);

			tcWeaponWheel->animateOut(1);
		}

	PortableAction(state,action);
}

void EXPORT_ME Java_com_beloko_opengames_prboom_NativeLib_analogFwd(JNIEnv *env, jobject obj,
		jfloat v)
{
	PortableMoveFwd(v);
}

void EXPORT_ME Java_com_beloko_opengames_prboom_NativeLib_analogSide(JNIEnv *env, jobject obj,
		jfloat v)
{
	PortableMoveSide(v);
}

void EXPORT_ME Java_com_beloko_opengames_prboom_NativeLib_analogPitch(JNIEnv *env, jobject obj,
		jint mode,jfloat v)
{
	PortableLookPitch(mode, v);
}

void EXPORT_ME Java_com_beloko_opengames_prboom_NativeLib_analogYaw(JNIEnv *env, jobject obj,
		jint mode,jfloat v)
{
	PortableLookYaw(mode, v);
}

void EXPORT_ME Java_com_beloko_opengames_prboom_NativeLib_setTouchSettings(JNIEnv *env, jobject obj,
		jfloat alpha,jfloat strafe,jfloat fwd,jfloat pitch,jfloat yaw,int other)
{
	setTouchSettings(alpha,strafe,fwd,pitch,yaw,other);
}

std::string quickCommandString;
jint EXPORT_ME
Java_com_beloko_opengames_prboom_NativeLib_quickCommand(JNIEnv *env, jobject obj,
		jstring command)
{
	const char * p = env->GetStringUTFChars(command,NULL);
	quickCommandString =  std::string(p) + "\n";
	env->ReleaseStringUTFChars(command, p);
	PortableCommand(quickCommandString.c_str());
}




void EXPORT_ME
Java_com_beloko_opengames_prboom_NativeLib_setScreenSize( JNIEnv* env,
		jobject thiz, jint width, jint height)
{
	android_screen_width = width;
	android_screen_height = height;
}
#ifdef ANTI_HACK
//CPP
void launchSSetup()
{
	//LOGI("JNI running setup");

	JNIEnv* pJNIEnv = 0;

	bool isAttached = false;
	int status = jvm_->GetEnv((void **) &pJNIEnv, JNI_VERSION_1_4);
	if(status < 0) {
		//LOGI("Attaching...");

		status = jvm_->AttachCurrentThread(&pJNIEnv, NULL);

		if(status < 0) {
			LOGI("callback_handler: failed to attach current thread");
		}
		isAttached = true;
	}
	//LOGI("JNI run_ssetup");
	run_ssetup(pJNIEnv);

	jvm_->DetachCurrentThread();
}
#endif
}
