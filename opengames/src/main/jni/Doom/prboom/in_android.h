#include "beloko_common/port_act_defs.h"


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
void PortableCommand(const char * cmd);

void PortableInit(int argc,const char ** argv);
void PortableFrame();

int PortableInMenu();
int PortableShowKeyboard();
int PortableInAutomap();


#ifdef __cplusplus
}
#endif
