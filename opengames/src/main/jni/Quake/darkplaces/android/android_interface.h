
//game 0 = Quake
//game 1 = Malice

void initControls(int width, int height,const char * graphics_path,const char *settings_file,int game=0);

void frameControls();

void touchControls(int action, int pid, float x, float y);

void setTouchSettings(float alpha,float strafe,float fwd,float pitch,float yaw,int other);


void gamepadButtonPressed();
