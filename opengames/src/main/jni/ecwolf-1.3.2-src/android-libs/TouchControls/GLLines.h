#ifdef USE_GLES2
#include <GLES2/gl2.h>
#else
#include <GLES/gl.h>
#endif

#ifndef _GLLines_H_
#define _GLLines_H_

namespace touchcontrols
{

class GLLines
{
public:
	GLfloat *vertices;

	GLuint len;

	GLLines(int nbr);

};

}
#endif
