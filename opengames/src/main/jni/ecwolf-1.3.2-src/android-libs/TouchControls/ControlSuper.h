#include <string>

#include "RectF.h"
#define TIXML_USE_STL
#include "tinyxml.h"

#ifndef _ControlSuper_H_
#define _ControlSuper_H_

#define TC_TYPE_BUTTON   1
#define TC_TYPE_TOUCHJOY 2
#define TC_TYPE_MOUSE    3
#define TC_TYPE_WHEELSEL 4
#define TC_TYPE_MULTITOUCHMOUSE 5



namespace touchcontrols
{
class ControlSuper
{
protected:
	bool enabled;

	bool hidden; //Can hide control from view, but still enabled

public:
	RectF controlPos;

	void setEnabled(bool v);
	void setHidden(bool v);

	bool isEnabled();
	bool isHidden();

	std::string description;

	std::string tag;

	int type;

	ControlSuper(int type,std::string t,RectF pos);

	virtual void resetOutput() = 0; //This outputs a reset

	virtual bool processPointer(int action, int pid, float x, float y) = 0;

	virtual bool drawGL(bool forEditor = false) = 0;

	virtual bool initGL() = 0;

	virtual void updateSize() = 0;

	virtual void saveXML(TiXmlDocument &doc) = 0;

	virtual void loadXML(TiXmlDocument &doc) = 0;

	void saveXML(TiXmlElement &root);

	void loadXML(TiXmlElement &root);

};

}

#endif
