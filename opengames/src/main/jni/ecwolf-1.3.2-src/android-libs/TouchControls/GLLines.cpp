#include "TouchControlsConfig.h"

#include "GLLines.h"
#include <string.h>

using namespace touchcontrols;



GLLines::GLLines(int nbr)
{
	len = nbr*2;
	vertices = new GLfloat[len * 3];
	memset(vertices,0,len * 3 * sizeof(GLfloat));
	//int * p1 = new int[5];
}


