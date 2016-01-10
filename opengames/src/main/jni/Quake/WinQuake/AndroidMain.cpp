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
#include <jni.h>

#include <stdio.h>
#include <assert.h>
#include <dlfcn.h>

#include <string>
#include "TouchControlsContainer.h"

#include "JNITouchControlsUtils.h"


#ifdef ANTI_HACK
#include "beloko_common/tools.h"
#endif

void CDDroid_SetJni(JNIEnv* e);

void portablePreInit();
int portableInit(int argc,const char** argv);
int portableTouchEvent(int action, int pid, float x, float y) ;
int portableKeyEvent(int type, int value);
int portableStep(int width,int height);
void portableQuit();
int portableSetGraphicsBasePath(const char * path);
void portableSetCacheBasePath(const char *path);
void portableSetBasePath(const char *path);
void setTouchSettings(float alpha,float strafe,float fwd,float pitch,float yaw,int other);
int quickCommand(char *command);


void PortableAction(int state, int action);
void PortableMove(float fwd, float strafe);
void PortableMoveFwd(float fwd);
void PortableMoveSide(float strafe);
void PortableLookPitch(int mode, float pitch);
void PortableLookYaw(int mode, float pitch);


//Ensure c string does not disapear!
std::string graphicpath;
std::string basepath;
std::string cachepath;

unsigned long ipAddr;

JNIEnv* env_;


void
qpreInit()
{
	LOGI("qpreInit");

	portablePreInit();
}

int argc=1;
const char * argv[32];

jboolean
qinit(JNIEnv *env, jobject thiz,jobjectArray argsArray) {
	LOGI("qinit");


	CDDroid_SetJni(env);

	env_ = env;
	argv[0] = "quake";
	int argCount = (env)->GetArrayLength( argsArray);
	Sys_Printf("argCount = %d",argCount);
	for (int i=0; i<argCount; i++) {
		jstring string = (jstring) (env)->GetObjectArrayElement( argsArray, i);
		argv[argc] = (char *)(env)->GetStringUTFChars( string, 0);
		Sys_Printf("arg = %s",argv[argc]);
		argc++;
	}

	return portableInit(argc, argv) ? JNI_TRUE : JNI_FALSE;
}

jboolean
qsetBasePath(JNIEnv *env, jobject thiz, jstring path) {
	const char * p = env->GetStringUTFChars(path,NULL);

	basepath = std::string(p);
	portableSetBasePath(basepath.c_str());
}

jboolean
qsetCachePath(JNIEnv *env, jobject thiz, jstring path) {
	const char * p = env->GetStringUTFChars(path,NULL);

	cachepath =  std::string(p);
	portableSetCacheBasePath(cachepath.c_str());
}

jboolean
qsetGraphicsBase(JNIEnv *env, jobject thiz, jstring path) {
	const char * p = env->GetStringUTFChars(path,NULL);

	graphicpath =  std::string(p);
	portableSetGraphicsBasePath(graphicpath.c_str());
}

jboolean
qtouchevent(JNIEnv *env, jobject thiz, jint action,
		jint pid, jfloat x, jfloat y) {

#ifdef ANTI_HACK
	static std::string libpath;
	static const char * libpath_c = NULL;


	if (!libpath_c)
	{
		libpath = getLibPath(env);
		libpath += "/libquake.so";
		libpath_c = libpath.c_str();
	}

	if (crcCheck(libpath_c))
		return 0;
#endif

	return portableTouchEvent(action,pid,x,y);
}

jboolean
qportabelmove(JNIEnv *env, jobject thiz,jfloat fwd, jfloat strafe) {
	PortableMove(fwd,strafe);
}
void doAction(JNIEnv *env, jobject obj,
		jint state, jint action)
{
	PortableAction(state,action);
}

void analogFwd(JNIEnv *env, jobject obj,
		jfloat v)
{
	PortableMoveFwd(v);
}

void analogSide(JNIEnv *env, jobject obj,
		jfloat v)
{
	PortableMoveSide(v);
}

void analogPitch(JNIEnv *env, jobject obj,
		jint mode,jfloat v)
{
	PortableLookPitch(mode, v);
}

void analogYaw(JNIEnv *env, jobject obj,
		jint mode,jfloat v)
{
	PortableLookYaw(mode, v);
}

void qsetTouchSettings(JNIEnv *env, jobject obj,jfloat alpha,jfloat strafe,jfloat fwd,jfloat pitch,jfloat yaw,int other)
{
	setTouchSettings(alpha,strafe,fwd,pitch,yaw,other);

}
jboolean
qevent(JNIEnv *env, jobject thiz, jint type, jint value) {
	return portableKeyEvent(type, value) ? JNI_TRUE : JNI_FALSE;
}



jint
qstep(JNIEnv *env, jobject thiz, jint width, jint height) {

	int ret =  portableStep(width,height);


#ifdef ANTI_HACK
	if (apkCheck(env))
	{
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
#endif

	return ret;
}

void
qipaddr(JNIEnv *env, jobject thiz, jint addr) {
	ipAddr = addr;
}

void
qquit(JNIEnv *env, jobject thiz) {
	portableQuit();
}

jboolean
qquickCommand(JNIEnv *env, jobject thiz, jstring command) {
	const char * p = env->GetStringUTFChars(command,NULL);
	quickCommand((char*)p);
	env->ReleaseStringUTFChars(command, p);
}

//Quake calls this at some point
unsigned long  PortableGetIpAddress()
{
	return ipAddr;
}

static const char *classPathName = "com/beloko/opengames/quakegl/QuakeLib";

static JNINativeMethod methods[] = {
		{"preInit", "()V", (void*)qpreInit },
		{"init", "([Ljava/lang/String;)Z", (void*)qinit },
		{"setGraphicsBase", "(Ljava/lang/String;)V", (void*)qsetGraphicsBase },
		{"setBasePath", "(Ljava/lang/String;)V", (void*)qsetBasePath },
		{"setCachePath", "(Ljava/lang/String;)V", (void*)qsetCachePath },
		{"touchEvent", "(IIFF)Z", (void*) qtouchevent },
		{"doAction", "(II)V", (void*)doAction},
		{"analogFwd", "(F)V", (void*)analogFwd},
		{"analogSide", "(F)V", (void*)analogSide},
		{"analogPitch", "(IF)V", (void*)analogPitch},
		{"analogYaw", "(IF)V", (void*)analogYaw},
		{"setTouchSettings", "(FFFFFI)V", (void*)qsetTouchSettings},
		{"quickCommand", "(Ljava/lang/String;)V", (void*)qquickCommand },

		{"ipAddr", "(J)V", (void*)qipaddr },
		{"event", "(II)Z", (void*)qevent },
		{"step", "(II)I", (void*)qstep },
		{"quit", "()V", (void*)qquit },
};

/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
		JNINativeMethod* gMethods, int numMethods)
{
	jclass clazz;

	clazz = env->FindClass(className);
	if (clazz == NULL) {
		fprintf(stderr,
				"Native registration unable to find class '%s'\n", className);
		return JNI_FALSE;
	}
	if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
		fprintf(stderr, "RegisterNatives failed for '%s'\n", className);
		return JNI_FALSE;
	}

	return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 */
static int registerNatives(JNIEnv* env)
{
	if (!registerNativeMethods(env, classPathName,
			methods, sizeof(methods) / sizeof(methods[0]))) {
		return JNI_FALSE;
	}

	return JNI_TRUE;
}

/*
 * Set some test stuff up.
 *
 * Returns the JNI version on success, -1 on failure.
 */

typedef union {
	JNIEnv* env;
	void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	setTCJNIEnv(vm);

	UnionJNIEnvToVoid uenv;
	uenv.venv = NULL;
	jint result = -1;
	JNIEnv* env = NULL;

	if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
		fprintf(stderr, "ERROR: GetEnv failed\n");
		goto bail;
	}
	env = uenv.env;

	assert(env != NULL);

	printf("In mgmain JNI_OnLoad\n");

	if (!registerNatives(env)) {
		fprintf(stderr, "ERROR: quakemaster native registration failed\n");
		goto bail;
	}

	/* success -- return valid version number */
	result = JNI_VERSION_1_4;

	bail:
	return result;
}
