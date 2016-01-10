#ifdef USE_GLES2
#include <GLES2/gl2.h>
#else
#include <GLES/gl.h>
#endif

#ifndef _GLRect_H_
#define _GLRect_H_

namespace touchcontrols
{

class GLRect
{
public:
	float width,height;

	GLfloat vertices[12];

	GLfloat texture[8];

	GLRect();

	void resize(float width, float height);
};

}
#endif
