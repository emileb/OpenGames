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
#include "unistd.h"

#include "SDL.h"

#include "SDL_android_extra.h"


#include "TouchControlsContainer.h"
#include "JNITouchControlsUtils.h"


extern "C"
{


#include "in_android.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"JNI", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "JNI", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"JNI", __VA_ARGS__))

void exit (int x)
{
	LOGI("EXIT");
	*((int *)0) = 0;
}

int printf ( const char * format, ... )
{
  	va_list argptr;
   	va_start(argptr, format);

	char string[512];
    vsprintf(string, format, argptr);
    LOGI("%s",string);
	return 0;
}

#define JAVA_FUNC(x) Java_com_beloko_opengames_abuse_NativeLib_##x

int android_screen_width;
int android_screen_height;


#define KEY_SHOW_KBRD    0x1000


bool hideTouchControls;

float gameControlsAlpha = 0.5;


static int controlsCreated = 0;
touchcontrols::TouchControlsContainer controlsContainer;

touchcontrols::TouchControls *tcMenuMain=0;
touchcontrols::TouchControls *tcGameMain=0;


extern JNIEnv* env_;

void openGLStart()
{

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glViewport(0, 0, android_screen_width, android_screen_height);
	glOrthof(0.0f, android_screen_width, android_screen_height, 0.0f, -1.0f, 1.0f);


	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY );

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	//glDisable(GL_CULL_FACE);
	glMatrixMode(GL_MODELVIEW);

	return;
}

void openGLEnd()
{

	glDisable (GL_BLEND);
	glColor4f(1,1,1,1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

}

void gameSettingsButton(int state)
{
	if (state == 1)
	{
		showTouchSettings();
	}
}







static jclass NativeLibClass = 0;
static jmethodID swapBuffersMethod = 0;
void swapBuffers()
{
	if (NativeLibClass == 0)
	{
		NativeLibClass = env_->FindClass("com/beloko/opengames/abuse/NativeLib");
		swapBuffersMethod = env_->GetStaticMethodID(NativeLibClass, "swapBuffers", "()V");
	}
	env_->CallStaticVoidMethod(NativeLibClass, swapBuffersMethod);
}

extern unsigned int Sys_Milliseconds(void);

static unsigned int reload_time_down;
void gameButton(int state,int code)
{

	if  (code == KEY_SHOW_KBRD)
	{
		if (state)
			toggleKeyboard();
	}
	else
	{
		PortableKeyEvent(state, code, 0);
	}
}


void menuButton(int state,int code)
{
	if (code == KEY_SHOW_KBRD)
	{
		if (state)
			toggleKeyboard();
	}
	else
		PortableKeyEvent(state, code, 0);
}

void mouse_move(int action,float x, float y,float dx, float dy)
{
	//PortableMouse(dx,dy);
	PortableMouseAbs(x * 640,y * 400);
	if (action == TOUCHMOUSE_TAP)
	{
		PortableMouseButton(SDL_BUTTON(1));
		//PortableKeyEvent(1, A_MOUSE1, 0);
		//PortableKeyEvent(0, A_MOUSE1, 0);
	}
}


int left_double_action;
int right_double_action;





void initControls(int width, int height,const char * graphics_path)
{
	touchcontrols::GLScaleWidth = (float)width;
	touchcontrols::GLScaleHeight = (float)height;

	LOGI("initControls %d x %d,x path = %s",width,height,graphics_path);

	if (!controlsCreated)
	{
		LOGI("creating controls");

		touchcontrols::setGraphicsBasePath(graphics_path);

		controlsContainer.openGL_start.connect( sigc::ptr_fun(&openGLStart));
		controlsContainer.openGL_end.connect( sigc::ptr_fun(&openGLEnd));


		tcMenuMain = new touchcontrols::TouchControls("menu",true,false);
		tcGameMain = new touchcontrols::TouchControls("game",false,true,1);

		tcGameMain->signal_settingsButton.connect(  sigc::ptr_fun(&gameSettingsButton) );

		//Menu
		//tcMenuMain->addControl(new touchcontrols::Button("down_arrow",touchcontrols::RectF(20,13,23,16),"arrow_down",SCAN_DOWN));
		//tcMenuMain->addControl(new touchcontrols::Button("up_arrow",touchcontrols::RectF(20,10,23,13),"arrow_up",SCAN_A));
		//tcMenuMain->addControl(new touchcontrols::Button("left_arrow",touchcontrols::RectF(17,13,20,16),"arrow_left",SCAN_LEFT));
		//tcMenuMain->addControl(new touchcontrols::Button("right_arrow",touchcontrols::RectF(23,13,26,16),"arrow_right",SCAN_RIGHT));
		//tcMenuMain->addControl(new touchcontrols::Button("enter",touchcontrols::RectF(0,12,4,16),"enter",SCAN_ENTER));
		tcMenuMain->addControl(new touchcontrols::Button("keyboard",touchcontrols::RectF(5,0,8,3),"keyboard",KEY_SHOW_KBRD));

		touchcontrols::Mouse *mouse = new touchcontrols::Mouse("mouse",touchcontrols::RectF(0,0,26,16),"");
		mouse->setHideGraphics(true);
		tcMenuMain->addControl(mouse);
		mouse->signal_action.connect(sigc::ptr_fun(&mouse_move) );


		tcMenuMain->signal_button.connect(  sigc::ptr_fun(&menuButton) );

		tcMenuMain->setAlpha(0.5);



		/*
		//Weapons
		tcGameWeapons->addControl(new touchcontrols::Button("weapon1",touchcontrols::RectF(1,14,3,16),"1",SDL_SCANCODE_1));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon2",touchcontrols::RectF(4,14,6,16),"2",SDL_SCANCODE_2));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon3",touchcontrols::RectF(7,14,9,16),"3",SDL_SCANCODE_3));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon4",touchcontrols::RectF(10,14,12,16),"4",SDL_SCANCODE_4));

		tcGameWeapons->addControl(new touchcontrols::Button("weapon5",touchcontrols::RectF(14,14,16,16),"5",SDL_SCANCODE_5));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon6",touchcontrols::RectF(17,14,19,16),"6",SDL_SCANCODE_6));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon7",touchcontrols::RectF(20,14,22,16),"7",SDL_SCANCODE_7));
		tcGameWeapons->addControl(new touchcontrols::Button("weapon8",touchcontrols::RectF(23,14,25,16),"8",SDL_SCANCODE_8));
		tcGameWeapons->signal_button.connect(  sigc::ptr_fun(&selectWeaponButton) );
		tcGameWeapons->setAlpha(0.8);
		 */


		controlsContainer.addControlGroup(tcGameMain);

		controlsContainer.addControlGroup(tcMenuMain);

		controlsCreated = 1;

		tcGameMain->setXMLFile((std::string)graphics_path +  "/game.xml");

	}
	else
		LOGI("NOT creating controls");

	controlsContainer.initGL();
}


int inMenuLast = 1;
int inAutomapLast = 0;
void frameControls()
{

	//LOGI("frameControls");

	int inMenuNew = PortableInMenu();
	if (inMenuLast != inMenuNew)
	{
		inMenuLast = inMenuNew;
		if (!inMenuNew)
		{
			tcGameMain->setEnabled(true);
			tcMenuMain->setEnabled(false);
		}
		else
		{
			tcGameMain->setEnabled(false);
			tcMenuMain->setEnabled(true);
		}
	}



	//controlsContainer.draw();

	swapBuffers();

	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
}


void setTouchSettings(float alpha,float strafe,float fwd,float pitch,float yaw,int other)
{

	gameControlsAlpha = alpha;
	if (tcGameMain)
		tcGameMain->setAlpha(gameControlsAlpha);

	hideTouchControls = other & 0x80000000?true:false;

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

std::string home_env;


extern void Android_SetGameResolution(int width, int height);
extern int SDLDisableAlphaFix;
jint EXPORT_ME
JAVA_FUNC(init) ( JNIEnv* env,	jobject thiz,jstring graphics_dir,jint mem_mb,jobjectArray argsArray,jint lowRes,jstring game_path_ )
{
	env_ = env;
	/*
	if (lowRes)
		Android_SetGameResolution(320,240);
	else
		Android_SetGameResolution(640,400);
	 */
	//Android_SetGameResolution(320,200);
	//Android_SetGameResolution(320,240);
	Android_SetGameResolution(640,400);

	SDLDisableAlphaFix = 1;

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


	game_path += "/";


	LOGI("game_path = %s",getGamePath());

	//Needed for ecwolf to run
	//home_env = "HOME=/" + game_path;
	//putenv(home_env.c_str());
	setenv("HOME", getGamePath(),1);

	putenv("TIMIDITY_CFG=../timidity.cfg");

	chdir(getGamePath());


	const char * p = env->GetStringUTFChars(graphics_dir,NULL);
	graphicpath =  std::string(p);



	initControls(android_screen_width,-android_screen_height,graphicpath.c_str());

	SDL_SetSwapBufferCallBack(frameControls);

	//Now done in java to keep context etc
	SDL_SwapBufferPerformsSwap(false);

	PortableInit(argc,argv); //Never returns!!

	return 0;
}


jint EXPORT_ME
JAVA_FUNC(frame) ( JNIEnv* env,	jobject thiz )
{

	//NOT CALLED

}

__attribute__((visibility("default"))) jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	LOGI("JNI_OnLoad");
	setTCJNIEnv(vm);

	return JNI_VERSION_1_4;
}


void EXPORT_ME
JAVA_FUNC(keypress) (JNIEnv *env, jobject obj,jint down, jint keycode, jint unicode)
{
	LOGI("keypress %d",keycode);
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
	LOGI("doAction %d %d",state,action);
	if (hideTouchControls)
		if (tcGameMain)
		{
			if (tcGameMain->isEnabled())
				tcGameMain->animateOut(30);


		}
	PortableAction(state,action);
}

void EXPORT_ME
JAVA_FUNC(analogFwd) (JNIEnv *env, jobject obj,	jfloat v)
{
	PortableMoveFwd(v);
}

void EXPORT_ME
JAVA_FUNC(analogSide) (JNIEnv *env, jobject obj,jfloat v)
{
	PortableMoveSide(v);
}

void EXPORT_ME
JAVA_FUNC(analogPitch) (JNIEnv *env, jobject obj,
		jint mode,jfloat v)
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

	return 0;
}




void EXPORT_ME
JAVA_FUNC(setScreenSize) ( JNIEnv* env,	jobject thiz, jint width, jint height)
{
	android_screen_width = width;
	android_screen_height = height;
}


}
