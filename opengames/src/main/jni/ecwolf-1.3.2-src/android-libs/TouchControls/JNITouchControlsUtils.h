
#ifdef __cplusplus
extern "C"
{
#endif
#include <jni.h>

//void setUIJNIEnv( JNIEnv* env);

void setTCJNIEnv( JavaVM* jvm);


void showCustomCommands();
void showTouchSettings();



void toggleKeyboard();
void showKeyboard(int val);
int hasHardwareKeyboard();

void ChangeDroidMusic(int action,int param1,int param2);

void setControlsContainer(touchcontrols::TouchControlsContainer * cc);


#ifdef __cplusplus
}
#endif
