#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>


#include "TouchControlsContainer.h"

#include "JNITouchControlsUtils.h"


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"DP", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "DP", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"DP", __VA_ARGS__))


//Used in gl_rmain.c
int android_screen_width;
int android_screen_height;



void openGLStart()
{

}

//These are in gl_backend
extern int android_reset_vertex;
extern int android_reset_color;

void openGLEnd()
{
	//Need to make sure DP resets these because touch controls changed
	android_reset_vertex = 1;
	android_reset_color = 1;
}

#include "android_interface.h"

extern "C"
{
void CDDroid_SetJni(JNIEnv* e);


int quit_now = 0;

#include "in_android.h"

#define EXPORT_ME __attribute__ ((visibility("default")))

JNIEnv* env_;

int argc=1;
const char * argv[32];
std::string graphicpath;



jint EXPORT_ME
Java_com_beloko_opengames_darkplaces_DPLib_init( JNIEnv* env,
		jobject thiz,jstring graphics_dir,jint mem_mb,jobjectArray argsArray,jint game )
{
	env_ = env;

	CDDroid_SetJni(env);

	argv[0] = "quake";
	int argCount = (env)->GetArrayLength( argsArray);
	LOGI("argCount = %d",argCount);
	for (int i=0; i<argCount; i++) {
		jstring string = (jstring) (env)->GetObjectArrayElement( argsArray, i);
		argv[argc] = (char *)(env)->GetStringUTFChars( string, 0);
		LOGI("arg = %s",argv[argc]);
		argc++;
	}



	//argv[0] = "vavoom";
	//argv[1] = "-basedir";
	//argv[2] = "/sdcard/Beloko/Quake/FULL";
	//args[3] = "-doom";

	PortableInit(argc,argv);

	const char * p = env->GetStringUTFChars(graphics_dir,NULL);
	graphicpath =  std::string(p);



	initControls(android_screen_width,android_screen_height,graphicpath.c_str(),(graphicpath + "/darkplaces.xml").c_str(),game);

	return 0;
}


jint EXPORT_ME
Java_com_beloko_opengames_darkplaces_DPLib_frame( JNIEnv* env,
		jobject thiz )
{

	PortableFrame();

	if (quit_now)
	{
		LOGI("frame QUIT");
		return 128;
	}

	frameControls();

	int ret = 0;

	if (PortableShowKeyboard())
	{
		ret |= 1;
	}

	return ret;
}

__attribute__((visibility("default"))) jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	LOGI("JNI_OnLoad");
	setTCJNIEnv(vm);

	return JNI_VERSION_1_2;
}


void EXPORT_ME
Java_com_beloko_opengames_darkplaces_DPLib_keypress(JNIEnv *env, jobject obj,
		jint down, jint keycode, jint unicode)
{
	LOGI("keypress %d",keycode);
	PortableKeyEvent(down,keycode);
}


void EXPORT_ME
Java_com_beloko_opengames_darkplaces_DPLib_touchEvent(JNIEnv *env, jobject obj,
		jint action, jint pid, jfloat x, jfloat y)
{
	//LOGI("TOUCHED");
	touchControls(action,pid,x,y);
}


void EXPORT_ME Java_com_beloko_opengames_darkplaces_DPLib_doAction(JNIEnv *env, jobject obj,
		jint state, jint action)
{

	gamepadButtonPressed();

	PortableAction(state,action);
}

void EXPORT_ME Java_com_beloko_opengames_darkplaces_DPLib_analogFwd(JNIEnv *env, jobject obj,
		jfloat v)
{
	PortableMoveFwd(v);
}

void EXPORT_ME Java_com_beloko_opengames_darkplaces_DPLib_analogSide(JNIEnv *env, jobject obj,
		jfloat v)
{
	PortableMoveSide(v);
}

void EXPORT_ME Java_com_beloko_opengames_darkplaces_DPLib_analogPitch(JNIEnv *env, jobject obj,
		jint mode,jfloat v)
{
	PortableLookPitch(mode, v);
}

void EXPORT_ME Java_com_beloko_opengames_darkplaces_DPLib_analogYaw(JNIEnv *env, jobject obj,
		jint mode,jfloat v)
{
	PortableLookYaw(mode, v);
}

void EXPORT_ME Java_com_beloko_opengames_darkplaces_DPLib_setTouchSettings(JNIEnv *env, jobject obj,
		jfloat alpha,jfloat strafe,jfloat fwd,jfloat pitch,jfloat yaw,int other)
{
	setTouchSettings(alpha,strafe,fwd,pitch,yaw,other);
}

std::string quickCommandString;
jint EXPORT_ME
Java_com_beloko_opengames_darkplaces_DPLib_quickCommand(JNIEnv *env, jobject obj,
		jstring command)
{
	const char * p = env->GetStringUTFChars(command,NULL);
	quickCommandString =  std::string(p) + "\n";
	env->ReleaseStringUTFChars(command, p);
	PortableCommand(quickCommandString.c_str());
}




void EXPORT_ME
Java_com_beloko_opengames_darkplaces_DPLib_setScreenSize( JNIEnv* env,
		jobject thiz, jint width, jint height)
{
	android_screen_width = width;
	android_screen_height = height;
}


}
