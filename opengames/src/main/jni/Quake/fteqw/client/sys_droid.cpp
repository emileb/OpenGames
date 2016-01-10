#include <jni.h>
#include <errno.h>

#include <android/log.h>

#include "quakedef.h"
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <dirent.h>

#include "TouchControlsContainer.h"
#include "JNITouchControlsUtils.h"

#include "in_android.h"


#ifdef ANTI_HACK
#include "beloko_common/tools.h"
#endif

#ifndef isDedicated
#ifdef SERVERONLY
qboolean isDedicated = true;
#else
qboolean isDedicated = qfalse;
#endif
#endif
void *sys_window; /*public so the renderer can attach to the correct place*/
static int sys_running = false;
int sys_glesversion;
int sys_soundflags;	/*1 means active. 2 means reset (so claim that its not active for one frame to force a reset)*/
static void *sys_memheap;
static unsigned int sys_lastframe;
static unsigned int vibrateduration;
static char errormessage[256];
extern  jmp_buf 	host_abort;

bool QUAKE_QUIT;

#define GAME_QUAKE = 0;
#define GAME_H2 = 1;
static int game;

float gameControlsAlpha = 0.5;
bool showSticks = false;
bool turnMouseMode = false;
bool invertLook = false;
bool precisionShoot = false;
bool hideTouchControls = true;

bool shooting = false;


cvar_t sys_vibrate = CVARD("sys_vibrate", "1", "Enables the system vibrator for damage events and such things. The value provided is a duration scaler.");
cvar_t sys_osk = CVAR("sys_osk", "0");	//to be toggled
cvar_t sys_keepscreenon = CVARD("sys_keepscreenon", "1", "If set, the screen will never darken. This might cost some extra battery power, but then so will running a 3d engine.");	//to be toggled
cvar_t sys_orientation = CVARD("sys_orientation", "landscape", "Specifies what angle to render quake at.\nValid values are: sensor (autodetect), landscape, portrait, reverselandscape, reverseportrait");
cvar_t sys_glesversion_cvar = CVARD("sys_glesversion", "1", "Specifies which version of gles to use. 1 or 2 are valid values.");
extern cvar_t vid_conautoscale;


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, DISTRIBUTION"Droid", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, DISTRIBUTION"Droid", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, DISTRIBUTION"Droid", __VA_ARGS__))

static int controlsCreated = 0;
touchcontrols::TouchControlsContainer controlsContainer;

touchcontrols::TouchControls *tcMenuMain=0;
touchcontrols::TouchControls *tcGameMain=0;
touchcontrols::TouchControls *tcGameWeapons=0;

//So can hide and show these buttons
touchcontrols::TouchJoy *touchJoyLeft;
touchcontrols::TouchJoy *touchJoyRight;

#define KEY_SHOW_WEAPONS 0x1000
#define KEY_SHOOT        0x1001
#define KEY_JUMP         0x1002
#define KEY_NEXT_WEAPONS 0x1003
#define KEY_PREV_WEAPONS 0x1004
#define KEY_QUICK_CMD    0x1005

extern "C"
{
void CDDroid_SetJni(JNIEnv* e);

int PortableKeyEvent(int state, int code);
void PortableAction(int state, int action);

void PortableMove(float fwd, float strafe);
void PortableMoveFwd(float fwd);
void PortableMoveSide(float strafe);
void PortableLookPitch(int mode, float pitch);
void PortableLookYaw(int mode, float pitch);

void PortableCommand(char * cmd);

void openGLStart()
{
	glDisable(GL_ALPHA_TEST);
	glDisableClientState(GL_COLOR_ARRAY);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void openGLEnd()
{
	glEnableClientState(GL_COLOR_ARRAY);
	glColor4f (1,1,1,1);
	glDisable (GL_BLEND);
}
JNIEnv* env_;

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



void setHideSticks(bool v)
{
	if (touchJoyLeft) touchJoyLeft->setHideGraphics(v);
	if (touchJoyRight) touchJoyRight->setHideGraphics(v);
}

void Sys_Vibrate(int count)
{
	vibrateduration = count*10*sys_vibrate.value;
}
__attribute__((visibility("default"))) jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	LOGI("JNI_OnLoad");

	setTCJNIEnv(vm);
	return JNI_VERSION_1_4;
}


JNIEXPORT jint JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_getvibrateduration(JNIEnv *env, jobject obj)
{
	unsigned int dur = vibrateduration;
	vibrateduration = 0;
	return dur;
}

JNIEXPORT jstring JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_geterrormessage(JNIEnv *env, jobject obj)
{
	return env->NewStringUTF( errormessage);
}
JNIEXPORT jstring JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_getpreferedorientation(JNIEnv *env, jobject obj)
{
	sys_orientation.modified = false;
	sys_glesversion_cvar.modified = false;
	return env->NewStringUTF(sys_orientation.string);
}

JNIEXPORT jint JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_getpreferedglesversion(JNIEnv *env, jobject obj)
{
	return sys_glesversion_cvar.ival;
}

/*the java passes in all input directly via a 'UI' thread. we don't need to poll it at all*/
void INS_Move(float *movements, int pnum)
{
}
void INS_Commands(void)
{
}
void INS_Init(void)
{
}
void INS_ReInit(void)
{
}
void INS_Shutdown(void)
{
}

std::string quickCommandString;
JNIEXPORT void JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_quickCommand(JNIEnv *env, jobject obj,
		jstring command)
{
	const char * p = env->GetStringUTFChars(command,NULL);
	quickCommandString =  std::string(p) + "\n";
	env->ReleaseStringUTFChars(command, p);
	PortableCommand(quickCommandString.c_str());
}

JNIEXPORT void JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_keypress(JNIEnv *env, jobject obj,
		jint down, jint keycode, jint unicode)
{
	IN_KeyEvent(0, down, keycode, unicode);
	//PortableKeyEvent(down,keycode);
}


JNIEXPORT void JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_touchEvent(JNIEnv *env, jobject obj,
		jint action, jint pid, jfloat x, jfloat y)
{
#ifdef ANTI_HACK
    static std::string libpath;
    static const char * libpath_c = NULL;


	if (!libpath_c)
	{
		libpath = getLibPath(env);
		libpath += "/libdarkplaces.so";
		libpath_c = libpath.c_str();
	}

  	if (crcCheck(libpath_c))
    	return;
#endif
	controlsContainer.processPointer(action,pid,x,y);
}

JNIEXPORT void JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_doAction(JNIEnv *env, jobject obj,
		jint state, jint action)
{
	if (hideTouchControls)
		if (tcGameMain->isEnabled())
			tcGameMain->animateOut(30);

	PortableAction(state,action);
}

JNIEXPORT void JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_analogFwd(JNIEnv *env, jobject obj,
		jfloat v)
{
	PortableMoveFwd(v);
}

JNIEXPORT void JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_analogSide(JNIEnv *env, jobject obj,
		jfloat v)
{
	PortableMoveSide(v);
}

JNIEXPORT void JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_analogPitch(JNIEnv *env, jobject obj,
		jint mode,jfloat v)
{
	PortableLookPitch(mode, v);
}

JNIEXPORT void JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_analogYaw(JNIEnv *env, jobject obj,
		jint mode,jfloat v)
{
	PortableLookYaw(mode, v);
}

JNIEXPORT void JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_setTouchSettings(JNIEnv *env, jobject obj,
		jfloat alpha,jfloat strafe,jfloat fwd,jfloat pitch,jfloat yaw,int other)
{
	gameControlsAlpha = alpha;
	if (tcGameMain)
		tcGameMain->setAlpha(gameControlsAlpha);

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

//Called Once at the beginning
JNIEXPORT jint JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_preInit(JNIEnv *env, jobject obj)
{
	sys_running = false;
	QUAKE_QUIT = false;

}

int key_dest_last = -1;
JNIEXPORT jint JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_frame(JNIEnv *env, jobject obj,
		jfloat ax, jfloat ay, jfloat az)
{
	int ret;
	static vec3_t oac;


	if (QUAKE_QUIT)
	{
#ifndef SERVERONLY
		Host_Shutdown ();
#else
		SV_Shutdown();
#endif
		if (sys_memheap)
			free(sys_memheap);
		sys_memheap = 0;
		return 128;
	}

	//if we had an error, don't even run a frame any more.
	if (*errormessage || !sys_running)
	{
		Sys_Printf("Crashed or quit\n");
		return 8;
	}

#ifdef SERVERONLY
	SV_Frame();
#else
	unsigned int now = Sys_Milliseconds();
	double tdelta = (now - sys_lastframe) * 0.001;
	if (oac[0] != ax || oac[1] != ay || oac[2] != az)
	{
		//down: x= +9.8
		//left: y= -9.8
		//up:   z= +9.8
		CSQC_Accelerometer(ax, ay, az);
		oac[0] = ax;
		oac[1] = ay;
		oac[2] = az;
	}
	Host_Frame(tdelta);
	sys_lastframe = now;
#endif

	ret = 0;
	if (key_dest == key_console || key_dest == key_message || (key_dest == key_game && cls.state == ca_disconnected) || sys_osk.ival)
		ret |= 1;
	if (vibrateduration)
		ret |= 2;
	if (sys_keepscreenon.ival)
		ret |= 4;
	if (*errormessage)
		ret |= 8;
	if (sys_orientation.modified || sys_glesversion_cvar.modified)
		ret |= 16;
	if (sys_soundflags)
	{
		if (sys_soundflags & 2)
			sys_soundflags &= ~2;
		else
			ret |= 32;
	}

	if (key_dest != key_dest_last)
	{
		if (key_dest == key_game)
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
		key_dest_last = key_dest;
	}

	setHideSticks(!showSticks);
	controlsContainer.draw();

	#ifdef ANTI_HACK
    	if (apkCheck(env))
    	{
    		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    		glClear(GL_COLOR_BUFFER_BIT);
    	}
    #endif

	return ret;
}

//tells us that our old gl context got completely obliterated
JNIEXPORT void JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_newglcontext(JNIEnv *env, jobject obj)
{
	if (sys_running)
		sys_running = 2;

	//fixme: wipe image handles
}

std::string graphicpath;
int argc=1;
const char * argv[32];
//called for init or resizes
JNIEXPORT void JNICALL Java_com_beloko_opengames_fteqw_FTEDroidEngine_init(JNIEnv *env, jobject obj,
		jint width, jint height, jint glesversion,jstring graphics_dir,jint game,jint mem_mb,jobjectArray argsArray)
{

	env_ = env;

	const char *tmp;

	if (*errormessage)
		return;

	CDDroid_SetJni(env);

	vid.pixelwidth = width;
	vid.pixelheight = height;
	sys_glesversion = glesversion;
	if (sys_running)
	{
		Sys_Printf("vid size changed\n");
		if (1)//FFS sys_running == 2)
		{
			//if our textures got destroyed, we need to reload them all
			Cmd_ExecuteString("vid_restart\n", RESTRICT_LOCAL);
		}
		else
		{
			//otherwise we just need to set the size properly again.
			Cvar_ForceCallback(&vid_conautoscale);
		}
	}
	else
	{

		quakeparms_t parms;
		Sys_Printf("reinit\n");
		if (sys_memheap)
			free(sys_memheap);
		memset(&parms, 0, sizeof(parms));
		parms.basedir = NULL;
		parms.memsize = mem_mb*1024*1024;
		parms.membase = sys_memheap = malloc(parms.memsize);
		if (!parms.membase)
		{
			Sys_Printf("Unable to alloc heap\n");
			Q_strncpyz(errormessage, "Unable to alloc heap\n", sizeof(errormessage));
			return;
		}
		//Create args
		argv[0] = "quake";
		int argCount = (env)->GetArrayLength( argsArray);
		Sys_Printf("argCount = %d",argCount);
		for (int i=0; i<argCount; i++) {
			jstring string = (jstring) (env)->GetObjectArrayElement( argsArray, i);
			argv[argc] = (char *)(env)->GetStringUTFChars( string, 0);
			Sys_Printf("arg = %s",argv[argc]);
			argc++;
		}

		parms.argc = argc;
		parms.argv = argv;
		Sys_Printf("Calling COM_InitArgv...");
		COM_InitArgv(parms.argc, parms.argv);
		Sys_Printf("...done COM_InitArgv");
		TL_InitLanguages();
#ifdef SERVERONLY
		SV_Init(&parms);
#else
		Host_Init(&parms);
#endif
		//Restart to apply settings (eg gamma)
		R_RestartRenderer_f();
		//Show main menu to get rid of console
		M_Menu_Main_f ();

		sys_running = true;
		sys_lastframe = Sys_Milliseconds();
		sys_orientation.modified = true;

		Sys_Printf("Done init");
		/*
		for (int i=0; i<argCount; i++) {
			jstring string = (jstring) (env)->GetObjectArrayElement( argsArray, i);
			(env)->ReleaseStringUTFChars(string, argv[i+1]);
		}
		 */
	}

	touchcontrols::GLScaleWidth = (float)vid.width;
	touchcontrols::GLScaleHeight = -(float)vid.height;


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
		tcGameMain = new touchcontrols::TouchControls("game",false,true);
		tcGameWeapons = new touchcontrols::TouchControls("weapons",false,false);

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
		tcGameMain->addControl(new touchcontrols::Button("next_weapon",touchcontrols::RectF(0,3,3,5),"next_weap",KEY_NEXT_WEAPONS));
		tcGameMain->addControl(new touchcontrols::Button("prev_weapon",touchcontrols::RectF(0,6,3,8),"prev_weap",KEY_PREV_WEAPONS));
		tcGameMain->addControl(new touchcontrols::Button("quick_cmd",touchcontrols::RectF(24,3,26,5),"prompt",KEY_QUICK_CMD,false,true));


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

		//tcGameMain->edit();
		//tcGameMain->saveXML("/sdcard/t.xml");
		//tcGameMain->loadXML("/sdcard/t.xml");
		tcGameMain->setXMLFile(graphicpath + "/fte_ctrl.xml");
	}
	else
		LOGI("NOT creating controls");

	Sys_Printf("w = %f, h = %f",touchcontrols::GLScaleWidth,touchcontrols::GLScaleHeight);

	controlsContainer.initGL();
}

static int secbase;

#ifdef _POSIX_TIMERS
double Sys_DoubleTime(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);

	if (!secbase)
	{
		secbase = ts.tv_sec;
		return ts.tv_nsec/1000000000.0;
	}
	return (ts.tv_sec - secbase) + ts.tv_nsec/1000000000.0;
}
unsigned int Sys_Milliseconds(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);

	if (!secbase)
	{
		secbase = ts.tv_sec;
		return ts.tv_nsec/1000000;
	}
	return (ts.tv_sec - secbase)*1000 + ts.tv_nsec/1000000;
}
#else
double Sys_DoubleTime(void)
{
	struct timeval tp;
	struct timezone tzp;

	gettimeofday(&tp, &tzp);

	if (!secbase)
	{
		secbase = tp.tv_sec;
		return tp.tv_usec/1000000.0;
	}

	return (tp.tv_sec - secbase) + tp.tv_usec/1000000.0;
}
unsigned int Sys_Milliseconds(void)
{
	struct timeval tp;
	struct timezone tzp;

	gettimeofday(&tp, &tzp);

	if (!secbase)
	{
		secbase = tp.tv_sec;
		return tp.tv_usec/1000;
	}

	return (tp.tv_sec - secbase)*1000 + tp.tv_usec/1000;
}
#endif

void Sys_Shutdown(void)
{
	free(sys_memheap);
	sys_memheap = 0;
}


void Sys_Quit(void)
{

	QUAKE_QUIT = true;
	/*
#ifndef SERVERONLY
	Host_Shutdown ();
#else
	SV_Shutdown();
#endif


	longjmp(host_abort, 1);

	//exit(0);

	 */
}
void Sys_Error (const char *error, ...)
{
	va_list         argptr;
	char             string[1024];

	va_start (argptr, error);
	vsnprintf (string,sizeof(string)-1, error,argptr);
	va_end (argptr);
	if (!*string)
		strcpy(string, "no error");

	Q_strncpyz(errormessage, string, sizeof(errormessage));

	LOGE("%s", string);

	longjmp(host_abort, 1);
	exit(1);
}
void Sys_Printf (char *fmt, ...)
{
	va_list         argptr;
	char             string[1024];

	va_start (argptr, fmt);
	vsnprintf (string,sizeof(string)-1, fmt,argptr);
	va_end (argptr);

	LOGI("%s", string);
}

void Sys_Warn (char *fmt, ...)
{
	va_list         argptr;
	char             string[1024];

	va_start (argptr, fmt);
	vsnprintf (string,sizeof(string)-1, fmt,argptr);
	va_end (argptr);

	LOGW("%s", string);
}

void Sys_CloseLibrary(dllhandle_t *lib)
{
	dlclose(lib);
}
dllhandle_t *Sys_LoadLibrary(const char *name, dllfunction_t *funcs)
{
	dllhandle_t *h;
	h = (void**)dlopen(name, RTLD_LAZY);
	return h;
}
void *Sys_GetAddressForName(dllhandle_t *module, const char *exportname)
{
	return dlsym(module, exportname);
}

static void *game_library;

void Sys_UnloadGame(void)
{
	if (game_library)
	{
		dlclose(game_library);
		game_library = 0;
	}
}

void *Sys_GetGameAPI (void *parms)
{
	LOGI("Sys_GetGameAPI CALLED!");

	void *(*GetGameAPI)(void *);

	char name[MAX_OSPATH];
	char curpath[MAX_OSPATH];
	char *searchpath;
	const char *agamename = "libq2game.so";
	char *result;

	void *ret;

	result = getcwd(curpath, sizeof(curpath)); // do something with result?


	searchpath = 0;
	while((searchpath = COM_NextPath(searchpath)))
	{
		if (searchpath[0] == '/')
			snprintf(name, sizeof(name), "%s/%s", searchpath, agamename);
		else
			snprintf(name, sizeof(name), "%s/%s/%s", curpath, searchpath, agamename);



		//game_library = dlopen (name, RTLD_NOW | RTLD_LOCAL);
		game_library = dlopen ("/data/data/com.beloko.quake2touch/lib/libq2gamealt.so", RTLD_NOW | RTLD_LOCAL);
		if (game_library)
		{
			GetGameAPI = (void *)dlsym (game_library, "GetGameAPI");
			if (GetGameAPI && (ret = GetGameAPI(parms)))
			{
				LOGI("Sys_GetGameAPI FOUND");

				return ret;
			}

			dlclose(game_library);
			game_library = 0;
		}
		else
		{
			LOGI("Sys_GetGameAPI game_library bad");

		}
	}
	LOGI("Sys_GetGameAPI NOTFOUND");
	return 0;
}

char *Sys_ConsoleInput (void)
{
	return NULL;
}
void Sys_mkdir (char *path)    //not all pre-unix systems have directories (including dos 1)
{
	mkdir(path, 0777);
}
qboolean Sys_remove (char *path)
{
	return (!unlink(path)) ? qtrue : qfalse;
}
qboolean Sys_Rename (char *oldfname, char *newfname)
{
	return !(rename(oldfname, newfname))? qtrue : qfalse;
}
void Sys_SendKeyEvents(void)
{
}
void Sys_Init(void)
{
	Cvar_Register(&sys_vibrate, "android stuff");
	Cvar_Register(&sys_osk, "android stuff");
	Cvar_Register(&sys_keepscreenon, "android stuff");
	Cvar_Register(&sys_orientation, "android stuff");
	Cvar_Register(&sys_glesversion_cvar, "android stuff");
}

qboolean Sys_GetDesktopParameters(int *width, int *height, int *bpp, int *refreshrate)
{
	*width = 320;
	*height = 240;
	*bpp = 16;
	*refreshrate = 60;
	return qfalse;
}
qboolean Sys_RandomBytes(qbyte *string, int len)
{
	qboolean res = qfalse;
	int fd = open("/dev/urandom", 0);
	if (fd >= 0)
	{
		res = (read(fd, string, len) == len)? qtrue : qfalse;
		close(fd);
	}

	return res;
}

void Sys_ServerActivity(void)
{
	/*FIXME: flash window*/
}

qboolean Sys_InitTerminal(void)
{
	/*switching to dedicated mode, show text window*/
	return qfalse;
}
void Sys_CloseTerminal(void)
{
}

char *Sys_GetClipboard(void)
{
	return NULL;
}
void Sys_CloseClipboard(char *buf)
{
}
void Sys_SaveClipboard(char *text)
{
}

int Sys_EnumerateFiles (const char *gpath, const char *match, int (*func)(const char *, int, void *, void *), void *parm, void *spath)
{
	DIR *dir;
	char apath[MAX_OSPATH];
	char file[MAX_OSPATH];
	char truepath[MAX_OSPATH];
	char *s;
	struct dirent *ent;
	struct stat st;

	//printf("path = %s\n", gpath);
	//printf("match = %s\n", match);

	if (!gpath)
		gpath = "";
	*apath = '\0';

	Q_strncpyz(apath, match, sizeof(apath));
	for (s = apath+strlen(apath)-1; s >= apath; s--)
	{
		if (*s == '/')
		{
			s[1] = '\0';
			match += s - apath+1;
			break;
		}
	}
	if (s < apath)  //didn't find a '/' 
		*apath = '\0'; 

	Q_snprintfz(truepath, sizeof(truepath), "%s/%s", gpath, apath); 


	//printf("truepath = %s\n", truepath); 
	//printf("gamepath = %s\n", gpath); 
	//printf("apppath = %s\n", apath); 
	//printf("match = %s\n", match); 
	dir = opendir(truepath); 
	if (!dir) 
	{ 
		Con_DPrintf("Failed to open dir %s\n", truepath); 
		return true; 
	} 
	do 
	{ 
		ent = readdir(dir); 
		if (!ent) 
			break; 
		if (*ent->d_name != '.') 
		{ 
			if (wildcmp(match, ent->d_name)) 
			{ 
				Q_snprintfz(file, sizeof(file), "%s/%s", truepath, ent->d_name); 

				if (stat(file, &st) == 0) 
				{ 
					Q_snprintfz(file, sizeof(file), "%s%s%s", apath, ent->d_name, S_ISDIR(st.st_mode)?"/":""); 

					if (!func(file, st.st_size, parm, spath)) 
					{ 
						closedir(dir); 
						return false; 
					} 
				} 
				else 
					printf("Stat failed for \"%s\"\n", file); 
			} 
		} 
	} while(1); 
	closedir(dir); 

	return true; 
}


}

