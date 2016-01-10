
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <unistd.h>

#include "TouchControlsContainer.h"
#include "JNITouchControlsUtils.h"


#include "../game/g_headers.h"

#include "../game/b_local.h"
#include "../game/q_shared.h"
#include "../client/keycodes.h"
#include "../qcommon/qcommon.h"
#include "in_android.h"
#include "../cgame/cg_headers.h"
#include "../cgame/cg_media.h"

#ifdef ANTI_HACK
#include "beloko_common/tools.h"
#endif
//extern unsigned int Sys_Milliseconds(void);

extern "C"
{
//#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"JNI", __VA_ARGS__))
//#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "JNI", __VA_ARGS__))
//#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"JNI", __VA_ARGS__))

#define JAVA_FUNC(x) Java_com_beloko_opengames_jk3_NativeLib_##x

int android_screen_width;
int android_screen_height;

#define KEY_SHOW_WEAPONS 0x1000
#define KEY_SHOOT        0x1001

#define KEY_SHOW_INV     0x1006
#define KEY_QUICK_CMD    0x1007
#define KEY_SHOW_KEYB      0x1009
#define KEY_QUICK_FORCE    0x100A
#define KEY_QUICK_KEY1    0x100B
#define KEY_QUICK_KEY2    0x100C
#define KEY_QUICK_KEY3    0x100D
#define KEY_QUICK_KEY4    0x100E

float gameControlsAlpha = 0.5;
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
touchcontrols::TouchControls *tcInventory=0;
touchcontrols::TouchControls *tcWeaponWheel=0;
touchcontrols::TouchControls *tcForceSelect=0;

//So can hide and show these buttons
touchcontrols::TouchJoy *touchJoyLeft;
touchcontrols::TouchJoy *touchJoyRight;
touchcontrols::WheelSelect *weaponWheel;

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

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glBlendFunc(GL_DST_COLOR, GL_ZERO);

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


static unsigned int reload_time_down;
void gameButton(int state,int code)
{
	if (code == KEY_SHOOT)
	{
		shooting = state;
		PortableAction(state,PORT_ACT_ATTACK);
	}
	else if (code == KEY_SHOW_INV)
	{
		if (state == 1)
		{
			if (!tcInventory->enabled)
			{
				tcInventory->animateIn(5);
			}
			else
				tcInventory->animateOut(5);
		}
	}
	else if  (code == KEY_QUICK_CMD){
		//if (state == 1)
		//	showCustomCommands();
		PortableKeyEvent(state, '~', 0);
		if (state)
			toggleKeyboard();
	}
	else if  (code == KEY_QUICK_FORCE)
	{
		if (state)
		{
			if (!tcForceSelect->isEnabled())
			{
				tcWeaponWheel->setEnabled(false); //disable WW while showing

				tcForceSelect->setEnabled(true);
				tcForceSelect->fade(0,5);
			}
			else
			{
				tcWeaponWheel->setEnabled(true);
				tcForceSelect->setEnabled(false);
			}
		}
	}
	else if  (code == KEY_QUICK_KEY1)
		PortableKeyEvent(state, A_F1, 0);
	else if  (code == KEY_QUICK_KEY2)
		PortableKeyEvent(state, A_F2, 0);
	else if  (code == KEY_QUICK_KEY3)
		PortableKeyEvent(state, A_F3, 0);
	else if  (code == KEY_QUICK_KEY4)
		PortableKeyEvent(state, A_F4, 0);
	else
	{
		PortableAction(state, code);
	}
}


//Weapon wheel callbacks
void weaponWheelSelected(int enabled)
{
	//int weapons = cg.snap->ps.stats[ STAT_WEAPONS ];
	int weapons = 0xFFFF;

	for (int n=0;n<10;n++)
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
	PortableAction(1, PORT_ACT_WEAPON_SELECT,segment+1);
}

int lastWeapon = -1;
void weaponWheelScroll(int segment)
{
	if (segment != lastWeapon)
	{
		PortableAction(1, PORT_ACT_WEAPON_SELECT,segment+1);
		lastWeapon = segment;
	}
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

void inventoryButton(int state,int code)
{
	PortableAction(state,code);
}

void forceSelectButton(int state,int code)
{
	PortableAction(state,code);

	if (!state)
	{
		tcWeaponWheel->setEnabled(true);
		tcForceSelect->setEnabled(false);
	}
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

void mouse_move(int action,float x, float y,float dx, float dy)
{
	//PortableMouse(dx,dy);
	PortableMouseAbs(x * 640,y * 480);
	if (action == TOUCHMOUSE_TAP)
	{
		PortableKeyEvent(1, A_MOUSE1, 0);
		PortableKeyEvent(0, A_MOUSE1, 0);
	}
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
	//LOGI(" mouse x = %f",mouse_x);
	int invert = invertLook?-1:1;

	float scale;

	scale = (shooting && precisionShoot)?0.3:1;

	scale /= (float)PortableLookScale();

	if (1)
	{
		PortableLookPitch(LOOK_MODE_MOUSE,-mouse_y  * pitch_sens * invert * scale);

		if (turnMouseMode)
			PortableLookYaw(LOOK_MODE_MOUSE,mouse_x*2*yaw_sens * scale);
		else
			PortableLookYaw(LOOK_MODE_JOYSTICK,joy_x*6*yaw_sens * scale);
	}
	else
	{
		float y = mouse_y * mouse_y;
		y *= 50;
		if (mouse_y < 0)
			y *= -1;

		PortableLookPitch(LOOK_MODE_MOUSE,-y  * pitch_sens * invert * scale);

		float x = mouse_x * mouse_x;
		x *= 100;
		if (mouse_x < 0)
			x *= -1;

		if (turnMouseMode)
			PortableLookYaw(LOOK_MODE_MOUSE,x*2*yaw_sens * scale);
		else
			PortableLookYaw(LOOK_MODE_JOYSTICK,joy_x*6*yaw_sens * scale);

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
		tcInventory  = new touchcontrols::TouchControls("inventory",false,true,1);
		tcWeaponWheel = new touchcontrols::TouchControls("weapon_wheel",false,true,1);
		tcForceSelect = new touchcontrols::TouchControls("force_select",false,true,1);
		//tcForceSelect = new touchcontrols::TouchControls("force_select",false,false); //disabled for now


		tcGameMain->signal_settingsButton.connect(  sigc::ptr_fun(&gameSettingsButton) );
		tcMenuMain->signal_button.connect(  sigc::ptr_fun(&menuButton) );

		touchcontrols::Mouse *mouse = new touchcontrols::Mouse("mouse",touchcontrols::RectF(0,0,26,16),"");
		mouse->setHideGraphics(true);
		tcMenuMain->addControl(mouse);
		mouse->signal_action.connect(sigc::ptr_fun(&mouse_move) );

		tcMenuMain->setAlpha(0.3);


		//Game
		tcGameMain->setAlpha(gameControlsAlpha);
		tcGameMain->addControl(new touchcontrols::Button("jump",touchcontrols::RectF(24,3,26,5),"jump",PORT_ACT_JUMP));
		tcGameMain->addControl(new touchcontrols::Button("crouch",touchcontrols::RectF(24,14,26,16),"crouch",PORT_ACT_DOWN));
		tcGameMain->addControl(new touchcontrols::Button("attack",touchcontrols::RectF(23,7,26,10),"shoot",KEY_SHOOT));
		tcGameMain->addControl(new touchcontrols::Button("alt_attack",touchcontrols::RectF(20,7,23,10),"shoot_alt",PORT_ACT_ALT_ATTACK));
		tcGameMain->addControl(new touchcontrols::Button("use_force",touchcontrols::RectF(23,5,25,7),"shoot_force",PORT_ACT_FORCE_USE));

		tcGameMain->addControl(new touchcontrols::Button("saber_style",touchcontrols::RectF(4,0,6,2),"saber_style",PORT_ACT_SABER_STYLE));

		tcGameMain->addControl(new touchcontrols::Button("use",touchcontrols::RectF(21,5,23,7),"use",PORT_ACT_USE));

		tcGameMain->addControl(new touchcontrols::Button("datapad",touchcontrols::RectF(14,0,16,2),"gamma",PORT_ACT_DATAPAD));
		tcGameMain->addControl(new touchcontrols::Button("use_inventory",touchcontrols::RectF(16,0,18,2),"inv",KEY_SHOW_INV));
		tcGameMain->addControl(new touchcontrols::Button("quick_save",touchcontrols::RectF(24,0,26,2),"save",PORT_ACT_QUICKSAVE));
		tcGameMain->addControl(new touchcontrols::Button("quick_load",touchcontrols::RectF(20,0,22,2),"load",PORT_ACT_QUICKLOAD));
		tcGameMain->addControl( new touchcontrols::Button("prompt",touchcontrols::RectF(9,0,11,2),"keyboard",KEY_QUICK_CMD));
		tcGameMain->addControl( new touchcontrols::Button("next_weapon",touchcontrols::RectF(0,3,3,5),"next_weap",PORT_ACT_NEXT_WEP));
		tcGameMain->addControl( new touchcontrols::Button("prev_weapon",touchcontrols::RectF(0,7,3,9),"prev_weap",PORT_ACT_PREV_WEP));

		//force contols
		tcGameMain->addControl(new touchcontrols::Button("prev_force",touchcontrols::RectF(8,14,10,16),"prev_force",PORT_ACT_PREV_FORCE));
		tcGameMain->addControl(new touchcontrols::Button("next_force",touchcontrols::RectF(16,14,18,16),"next_force",PORT_ACT_NEXT_FORCE));
		tcGameMain->addControl(new touchcontrols::Button("quick_force",touchcontrols::RectF(12,14,14,16),"quick_force",KEY_QUICK_FORCE));

		//quick actions binds
		tcGameMain->addControl(new touchcontrols::Button("quick_key_1",touchcontrols::RectF(4,3,6,5),"quick_key_1",KEY_QUICK_KEY1,false,true));
		tcGameMain->addControl(new touchcontrols::Button("quick_key_2",touchcontrols::RectF(6,3,8,5),"quick_key_2",KEY_QUICK_KEY2,false,true));
		tcGameMain->addControl(new touchcontrols::Button("quick_key_3",touchcontrols::RectF(8,3,10,5),"quick_key_3",KEY_QUICK_KEY3,false,true));
		tcGameMain->addControl(new touchcontrols::Button("quick_key_4",touchcontrols::RectF(10,3,12,5),"quick_key_4",KEY_QUICK_KEY4,false,true));


		touchJoyLeft = new touchcontrols::TouchJoy("stick",touchcontrols::RectF(0,7,8,16),"strafe_arrow");
		tcGameMain->addControl(touchJoyLeft);
		touchJoyLeft->signal_move.connect(sigc::ptr_fun(&left_stick) );
		touchJoyLeft->signal_double_tap.connect(sigc::ptr_fun(&left_double_tap) );

		touchJoyRight = new touchcontrols::TouchJoy("touch",touchcontrols::RectF(17,4,26,16),"look_arrow");
		tcGameMain->addControl(touchJoyRight);
		touchJoyRight->signal_move.connect(sigc::ptr_fun(&right_stick) );
		touchJoyRight->signal_double_tap.connect(sigc::ptr_fun(&right_double_tap) );

		tcGameMain->signal_button.connect(  sigc::ptr_fun(&gameButton) );

		//Weapon wheel
		weaponWheel = new touchcontrols::WheelSelect("weapon_wheel",touchcontrols::RectF(7,2,19,14),"weapon_wheel",10);
		weaponWheel->signal_selected.connect(sigc::ptr_fun(&weaponWheelChosen) );
		weaponWheel->signal_enabled.connect(sigc::ptr_fun(&weaponWheelSelected));
		weaponWheel->signal_scroll.connect(sigc::ptr_fun(&weaponWheelScroll));

		tcWeaponWheel->addControl(weaponWheel);
		tcWeaponWheel->setAlpha(0.8);

		//Inventory
		tcInventory->addControl(new touchcontrols::Button("thirdperson",touchcontrols::RectF(0,14,2,16),"camera",PORT_ACT_THIRD_PERSON));
		tcInventory->addControl(new touchcontrols::Button("invuse",touchcontrols::RectF(3,14,5,16),"enter",PORT_ACT_INVUSE));
		tcInventory->addControl(new touchcontrols::Button("invprev",touchcontrols::RectF(6,14,8,16),"arrow_left",PORT_ACT_INVPREV));
		tcInventory->addControl(new touchcontrols::Button("invnext",touchcontrols::RectF(8,14,10,16),"arrow_right",PORT_ACT_INVNEXT));

		tcInventory->signal_button.connect(  sigc::ptr_fun(&inventoryButton) );
		tcInventory->setAlpha(0.5);

		//Force Select "_new" added so reset to new postions
		tcForceSelect->addControl(new touchcontrols::Button("force_absorb_new",touchcontrols::RectF(4,2,7,5),"f_lt_absorb",PORT_ACT_FORCE_ABSORB));
		tcForceSelect->addControl(new touchcontrols::Button("force_heal_new",touchcontrols::RectF(4,5,7,8),"f_lt_heal",PORT_ACT_FORCE_HEAL));
		tcForceSelect->addControl(new touchcontrols::Button("force_mind_new",touchcontrols::RectF(4,8,7,11),"f_lt_mind",PORT_ACT_FORCE_MIND));
		tcForceSelect->addControl(new touchcontrols::Button("force_protect_new",touchcontrols::RectF(4,11,7,14),"f_lt_protect",PORT_ACT_FORCE_PROTECT));

		tcForceSelect->addControl(new touchcontrols::Button("force_pull_new",touchcontrols::RectF(7,2,10,5),"force_pull",PORT_ACT_FORCE_PULL));
		tcForceSelect->addControl(new touchcontrols::Button("force_speed_new",touchcontrols::RectF(7,5,10,8),"force_speed",PORT_ACT_FORCE_SPEED));
		tcForceSelect->addControl(new touchcontrols::Button("force_push_new",touchcontrols::RectF(7,8,10,11),"force_push",PORT_ACT_FORCE_PUSH));
		tcForceSelect->addControl(new touchcontrols::Button("force_sence_new",touchcontrols::RectF(7,11,10,14),"force_sence",PORT_ACT_FORCE_SIGHT));

		tcForceSelect->addControl(new touchcontrols::Button("force_drain_new",touchcontrols::RectF(10,2,13,5),"f_dk_drain",PORT_ACT_FORCE_DRAIN));
		tcForceSelect->addControl(new touchcontrols::Button("force_grip_new",touchcontrols::RectF(10,5,13,8),"f_dk_grip",PORT_ACT_FORCE_GRIP));
		tcForceSelect->addControl(new touchcontrols::Button("force_lightning_new",touchcontrols::RectF(10,8,13,11),"f_dk_lightning",PORT_ACT_FORCE_LIGHT));
		tcForceSelect->addControl(new touchcontrols::Button("force_rage_new",touchcontrols::RectF(10,11,13,14),"f_dk_rage",PORT_ACT_FORCE_RAGE));


		tcForceSelect->signal_button.connect(  sigc::ptr_fun(&forceSelectButton) );

		tcForceSelect->setAlpha(0.8);

		controlsContainer.addControlGroup(tcGameMain);
		//controlsContainer.addControlGroup(tcGameWeapons);
		controlsContainer.addControlGroup(tcMenuMain);
		controlsContainer.addControlGroup(tcInventory);
		controlsContainer.addControlGroup(tcWeaponWheel);
		controlsContainer.addControlGroup(tcForceSelect);
		controlsCreated = 1;

		tcGameMain->setXMLFile(settings_file);
		tcInventory->setXMLFile((std::string)graphics_path +  "/inventory.xml");
		tcWeaponWheel->setXMLFile((std::string)graphics_path +  "/weaponwheel.xml");
		tcForceSelect->setXMLFile((std::string)graphics_path +  "/forceselect.xml");
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
			if (enableWeaponWheel)
				tcWeaponWheel->setEnabled(true);
			tcMenuMain->setEnabled(false);

			tcForceSelect->setEnabled(false);
		}
		else
		{
			tcGameMain->setEnabled(false);
			tcWeaponWheel->setEnabled(false);
			tcForceSelect->setEnabled(false);
			tcMenuMain->setEnabled(true);
		}
	}

#ifdef ANTI_HACK
	if (apkCheck(env_))
	{
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
#endif

	setHideSticks(!showSticks);
	controlsContainer.draw();
}


void setTouchSettings(float alpha,float strafe,float fwd,float pitch,float yaw,int other)
{

	gameControlsAlpha = alpha;
	if (tcGameMain)
		tcGameMain->setAlpha(gameControlsAlpha);

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
	case 3:
		left_double_action = PORT_ACT_USE;
		break;
	case 4:
		left_double_action = PORT_ACT_FORCE_USE;
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
	case 3:
		right_double_action = PORT_ACT_USE;
		break;
	case 4:
		right_double_action = PORT_ACT_FORCE_USE;
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
JavaVM* jvm_;

int argc=1;
const char * argv[32];
std::string graphicpath;


std::string game_path;
const char * getGamePath()
{
	return game_path.c_str();
}

std::string lib_path;
const char * getLibPath()
{
	return lib_path.c_str();
}

jobject *activity_;
jint EXPORT_ME
JAVA_FUNC(init) ( JNIEnv* env,	jobject thiz,jobject act,jstring graphics_dir,jint mem_mb,jobjectArray argsArray,jint renderer,jstring game_path_,jstring lib_path_  )
{
	activity_ = &act;
	env_ = env;


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
	lib_path = (char *)(env)->GetStringUTFChars( lib_path_, 0);

	LOGI("game_path = %s",getGamePath());
	LOGI("lib_path = %s",getLibPath());

	chdir(getGamePath());

	//self_crc_check((string(getLibPath()) + "/libjk3.so").c_str());

	PortableInit(argc,argv);

	const char * p = env->GetStringUTFChars(graphics_dir,NULL);
	graphicpath =  std::string(p);



	initControls(640,-480,graphicpath.c_str(),(graphicpath + "/game_controls.xml").c_str());


	return 0;
}

extern int androidSwapped;

jint EXPORT_ME
JAVA_FUNC(frame) ( JNIEnv* env,	jobject thiz )
{
	androidSwapped = 1;

	PortableFrame();

	if (quit_now)
	{
		LOGI("frame QUIT");
		return 128;
	}

	frameControls();

	//glFlush();
	//glFinish();
	int ret = 0;

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
JAVA_FUNC(keypress) (JNIEnv *env, jobject obj,jint down, jint keycode, jint unicode)
{
	if (controlsContainer.isEditing())
	{
		if (down && (keycode == A_ESCAPE ))
			controlsContainer.finishEditing();
		return;
	}
	PortableKeyEvent(down,keycode,unicode);
}


void EXPORT_ME
JAVA_FUNC(touchEvent) (JNIEnv *env, jobject obj,jint action, jint pid, jfloat x, jfloat y)
{
#ifdef ANTI_HACK
	if (crcCheck((lib_path + "/libjk3.so").c_str()))
        return;
#endif

	controlsContainer.processPointer(action,pid,x,y);
}


void EXPORT_ME
JAVA_FUNC(doAction) (JNIEnv *env, jobject obj,	jint state, jint action)
{
	//gamepadButtonPressed();
	if (hideTouchControls)
		if (tcGameMain)
		{
			if (tcGameMain->isEnabled())
				tcGameMain->animateOut(30);

			tcWeaponWheel->animateOut(30);
		}
	//LOGI("doAction %d %d",state,action);
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
JAVA_FUNC(analogPitch) (JNIEnv *env, jobject obj,jint mode,jfloat v)
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
JAVA_FUNC(quickCommand) (JNIEnv *env, jobject obj,		jstring command)
{
	const char * p = env->GetStringUTFChars(command,NULL);
	quickCommandString =  std::string(p) + "\n";
	env->ReleaseStringUTFChars(command, p);
	PortableCommand(quickCommandString.c_str());
	return 0;
}


void EXPORT_ME
JAVA_FUNC(setScreenSize) ( JNIEnv* env,		jobject thiz, jint width, jint height)
{
	android_screen_width = width;
	android_screen_height = height;
}

}

