

#include <string>
#include <stdbool.h>
#include <vector>

#include "TouchControlsConfig.h"
#include "TouchControls.h"

#ifndef _TouchControlsContainer_H_
#define _TouchControlsContainer_H_

//#define ScaleX
namespace touchcontrols
{


class TouchControlsContainer
{

	std::vector<TouchControls *> controls;

	TouchControls * editingControls;

	//For when we have an edit group
	Button *editorButton;
	void editorButtonPress(int state,int code);
	int drawEditButton;

public:

	int dukeHack;
	float editButtonAlpha;

	sigc::signal<void> openGL_start;
	sigc::signal<void> openGL_end;

	sigc::signal<void,int> signal_settings;


	TouchControlsContainer();

	void addControlGroup(TouchControls *cntrl);

	void editControls(TouchControls * ctrl);

	int draw ();

	void initGL ();

	bool processPointer(int action, int pid, float x, float y);

	bool isEditing();
	void finishEditing();

	void resetDefaults();

	TouchControls* getEditingControls();

};

}


#endif
