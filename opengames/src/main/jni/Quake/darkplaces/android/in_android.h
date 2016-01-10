#include "port_act_defs.h"



#define LOOK_MODE_MOUSE    0
#define LOOK_MODE_ABSOLUTE 1
#define LOOK_MODE_JOYSTICK 2



#ifdef __cplusplus
extern "C"
{
#endif
int PortableKeyEvent(int state, int code);
void PortableAction(int state, int action);

void PortableMove(float fwd, float strafe);
void PortableMoveFwd(float fwd);
void PortableMoveSide(float strafe);
void PortableLookPitch(int mode, float pitch);
void PortableLookYaw(int mode, float pitch);
void PortableCommand(char * cmd);

void PortableInit(int argc,const char ** argv);
void PortableFrame();

int PortableInMenu();
int PortableShowKeyboard();

#ifdef __cplusplus
}
#endif
