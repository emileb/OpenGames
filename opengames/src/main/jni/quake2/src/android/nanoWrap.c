/*
Copyright (C) 2007-2008 Olli Hinkka

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

 */

//#include <e32def.h>
//#include <e32std.h>
#include <stdio.h>
#include <string.h>


#if ANDROID
#define USE_GLES 1
#else
#define USE_GLES 0
#endif

#define COLOR_BYTE 1

#define CHECK_OVERFLOW 1

// give error : E/GLLogger(  857): called unimplemented OpenGL ES API
//#include "glesinterface.h"

#define __MULTITEXTURE_SUPPORT__

#if USE_GLES
#include "gl.h"
#include "glesinterface.h"
#else
#include <GL/gl.h>
#include "nanogl.h"
#endif

#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1

typedef struct
{
	GLboolean alpha_test;
	GLboolean blend;
	GLboolean clip_planei;
	GLboolean color_logic_op;
	GLboolean color_material;
	GLboolean cull_face;
	GLboolean depth_test;
	GLboolean dither;
	GLboolean fog;
	GLboolean lighti;
	GLboolean lighting;
	GLboolean line_smooth;
	GLboolean matrix_palette_oes;
	GLboolean multisample;
	GLboolean normalize;
	GLboolean point_smooth;
	GLboolean point_sprite_oes;
	GLboolean polygon_offset_fill;
	GLboolean rescale_normal;
	GLboolean sample_alpha_to_coverage;
	GLboolean sample_alpha_to_one;
	GLboolean sample_coverage;
	GLboolean scissor_test;
	GLboolean stencil_test;
	GLboolean depthmask;
	GLclampf depth_range_near;
	GLclampf depth_range_far;
	GLenum depth_func;
	GLenum cullface;
	GLenum shademodel;
	GLenum sfactor;
	GLenum dfactor;
	GLenum matrixmode;
} nanoState;

static nanoState nanoglState;

static nanoState nanoglInitState =
{
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_TRUE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_TRUE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_TRUE,
		0.0f,
		1.0f,
		GL_LESS,
		GL_BACK,
		GL_SMOOTH,
		GL_ONE,
		GL_ZERO,
		GL_MODELVIEW,
};

struct booleanstate
{
	GLboolean value;
	GLboolean changed;
};

struct floatstate
{
	GLfloat value;
	GLboolean changed;
};

struct uintstate
{
	GLuint value;
	GLboolean changed;
};

struct ptrstate
{
	GLint size;
	GLenum type;
	GLsizei stride;
	GLvoid* ptr;
	GLboolean changed;
	GLboolean enabled;
};


typedef struct
{
	struct booleanstate texture_2d;
	struct floatstate texture_env_mode;
	struct uintstate boundtexture;
	struct ptrstate vertex_array;
	struct ptrstate color_array;
	struct ptrstate texture_coord_array;
} nanotmuState;

static nanotmuState tmuState0;
static nanotmuState tmuState1;

static nanotmuState tmuInitState =
{
		{GL_FALSE, GL_FALSE},
		{GL_MODULATE,GL_FALSE},
		{0x7fffffff,GL_FALSE},
		{4,GL_FLOAT,0, NULL, GL_FALSE, GL_FALSE},
		{4,GL_FLOAT,0, NULL, GL_FALSE, GL_FALSE},
		{4,GL_FLOAT,0, NULL, GL_FALSE, GL_FALSE},
};

static nanotmuState* activetmuState = &tmuState0;

//extern "C++" GlESInterface* GLESIMPL ;
//extern GlESInterface* GLESIMPL ;
#define GLESIMPL

static GLenum wrapperPrimitiveMode = GL_QUADS;
GLboolean useTexCoordArray = GL_FALSE;
static GLenum activetmu = GL_TEXTURE0;
static GLenum clientactivetmu = GL_TEXTURE0;

#if defined(__MULTITEXTURE_SUPPORT__)
GLboolean useMultiTexCoordArray = GL_FALSE;
#endif


#if !defined (__WINS__)
#define __FORCEINLINE __forceinline
#else
#define __FORCEINLINE
#endif

static GLboolean delayedttmuchange = GL_FALSE;
static GLenum delayedtmutarget = GL_TEXTURE0;


typedef struct
{
	float x;
	float y;
	float z;
#if !defined(__MULTITEXTURE_SUPPORT__)
	float padding;
#endif

#if COLOR_BYTE
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
#else
	float red;
	float green;
	float blue;
	float alpha;	
#endif

	float s;
	float t;
#if defined(__MULTITEXTURE_SUPPORT__)
	float s_multi;
	float t_multi;
#endif
} VertexAttrib;




#define SIZE_VERTEXATTRIBS 16000
static VertexAttrib vertexattribs[SIZE_VERTEXATTRIBS];

#define SIZE_INDEXARRAY 60000
static GLushort indexArray[SIZE_INDEXARRAY];

static GLuint vertexCount = 0;
static GLuint indexCount = 0;
static GLuint vertexMark = 0;
static int indexbase = 0;

static VertexAttrib* ptrVertexAttribArray = NULL;
static VertexAttrib* ptrVertexAttribArrayMark = NULL;

static VertexAttrib currentVertexAttrib;
/*
#if defined(__MULTITEXTURE_SUPPORT__)
static VertexAttrib currentVertexAttribInit = {0.0f,0.0f,0.0f,
                                    255,255,255,255,
                                    0.0f,0.0f,
                                    0.0f,0.0f
                                    };
#else
static VertexAttrib currentVertexAttribInit = {0.0f,0.0f,0.0f,0.0f,
                                    255,255,255,255,
                                    0.0f,0.0f,
                                    };
#endif
 */

static GLushort* ptrIndexArray = NULL;

static GLboolean arraysValid = GL_FALSE;

void InitGLStructs()
{
	ptrVertexAttribArray = vertexattribs;
	ptrVertexAttribArrayMark = ptrVertexAttribArray;
	ptrIndexArray = indexArray;

	memcpy(&nanoglState, &nanoglInitState, sizeof( nanoState));
	memcpy(&tmuState0,&tmuInitState,sizeof( nanotmuState));
	memcpy(&tmuState1,&tmuInitState,sizeof( nanotmuState));

	//memcpy(&currentVertexAttrib,&currentVertexAttribInit,sizeof( VertexAttrib));
	currentVertexAttrib.x = 0.0f;
	currentVertexAttrib.y = 0.0f;
	currentVertexAttrib.z = 0.0f;
	currentVertexAttrib.red 	=0;
	currentVertexAttrib.green	=0;
	currentVertexAttrib.blue	=0;
	currentVertexAttrib.alpha	=0;
	currentVertexAttrib.s	 = 0.0f;
	currentVertexAttrib.t	 = 0.0f;
	currentVertexAttrib.s_multi	 = 0.0f;
	currentVertexAttrib.t_multi	 = 0.0f;


	activetmuState = &tmuState0;
	wrapperPrimitiveMode = GL_QUADS;
	useTexCoordArray = GL_FALSE;
	activetmu = GL_TEXTURE0;
	clientactivetmu = GL_TEXTURE0;
	delayedttmuchange = GL_FALSE;
	delayedtmutarget = GL_TEXTURE0;
	vertexCount = 0;
	indexCount = 0;
	vertexMark = 0;
	indexbase = 0;
	arraysValid = GL_FALSE;
}


void FlushOnStateChange()
{
	if (delayedttmuchange)
	{
		delayedttmuchange = GL_FALSE;
		GLESIMPL glActiveTexture(delayedtmutarget);
	}

	if (!vertexCount)
		return;

	if (!arraysValid)
	{
		GLESIMPL glClientActiveTexture(GL_TEXTURE0);
		GLESIMPL glVertexPointer(3, GL_FLOAT, sizeof(VertexAttrib), &vertexattribs[0].x);
#if COLOR_BYTE
GLESIMPL glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(VertexAttrib), &vertexattribs[0].red);
#else
GLESIMPL glColorPointer(4, GL_FLOAT, sizeof(VertexAttrib), &vertexattribs[0].red);
#endif
GLESIMPL glTexCoordPointer(2, GL_FLOAT, sizeof(VertexAttrib), &vertexattribs[0].s);
GLESIMPL glEnableClientState(GL_VERTEX_ARRAY);
GLESIMPL glEnableClientState(GL_TEXTURE_COORD_ARRAY);
GLESIMPL glEnableClientState(GL_COLOR_ARRAY);
#if defined(__MULTITEXTURE_SUPPORT__)
GLESIMPL glClientActiveTexture(GL_TEXTURE1);
GLESIMPL glTexCoordPointer(2, GL_FLOAT, sizeof(VertexAttrib), &vertexattribs[0].s_multi);
GLESIMPL glEnableClientState(GL_TEXTURE_COORD_ARRAY);
GLESIMPL glClientActiveTexture(GL_TEXTURE0);
#endif
arraysValid = GL_TRUE;
	}


#if CHECK_OVERFLOW
	if ( ptrVertexAttribArray >= vertexattribs + SIZE_VERTEXATTRIBS){
		extern void Sys_Error (char *error, ...);
		Sys_Error("vertexattribs overflow\n");
	}
	if ( ptrIndexArray >= indexArray + SIZE_INDEXARRAY){
		extern void Sys_Error (char *error, ...);
		Sys_Error("indexArray overflow\n");
	}
#endif

	GLESIMPL glDrawElements( GL_TRIANGLES,vertexCount,GL_UNSIGNED_SHORT, indexArray );


#if defined(__MULTITEXTURE_SUPPORT__)
	useMultiTexCoordArray = GL_FALSE;
#endif
	vertexCount = 0;
	indexCount = 0;
	ptrVertexAttribArray = vertexattribs;
	ptrVertexAttribArrayMark = ptrVertexAttribArray;
	ptrIndexArray = indexArray;
	useTexCoordArray = GL_FALSE;
}

int stackTextureState;
void nanoPushState()
{
	stackTextureState = activetmuState->boundtexture.value;

}

void nanoPopState()
{

	GLESIMPL glVertexPointer(tmuState0.vertex_array.size,
					tmuState0.vertex_array.type,
					tmuState0.vertex_array.stride,
					tmuState0.vertex_array.ptr);


	GLESIMPL glBindTexture(GL_TEXTURE_2D, stackTextureState);

	arraysValid = GL_FALSE;
}


void nanoglBegin(GLenum mode)
{
	wrapperPrimitiveMode = mode;
	vertexMark = vertexCount;
	ptrVertexAttribArrayMark = ptrVertexAttribArray;
	indexbase = indexCount;
}

void nanoglEnd(void)
{
	int count ;

	vertexCount+=((unsigned char*)ptrVertexAttribArray-(unsigned char*)ptrVertexAttribArrayMark)/sizeof(VertexAttrib);
	if (vertexCount < 3)
	{
		return;
	}
	switch (wrapperPrimitiveMode)
	{
	case GL_QUADS:
	{
		*ptrIndexArray++ = indexCount;
		*ptrIndexArray++ = indexCount+1;
		*ptrIndexArray++ = indexCount+2;
		*ptrIndexArray++ = indexCount;
		*ptrIndexArray++ = indexCount+2;
		*ptrIndexArray++ = indexCount+3;
		indexCount+=4;
		vertexCount+=2;
	}
	break;
	case GL_TRIANGLES:
	{
		int  vcount = (vertexCount-vertexMark)/3;
		for ( count = 0; count < vcount; count++)
		{
			*ptrIndexArray++ = indexCount;
			*ptrIndexArray++ = indexCount+1;
			*ptrIndexArray++ = indexCount+2;
			indexCount+=3;
		}
	}
	break;
	case GL_TRIANGLE_STRIP:
	{
		*ptrIndexArray++ = indexCount;
		*ptrIndexArray++ = indexCount+1;
		*ptrIndexArray++ = indexCount+2;
		indexCount+=3;
		int vcount = ((vertexCount-vertexMark)-3);



		if (vcount && ((long)ptrIndexArray & 0x02))
		{
			*ptrIndexArray++ = indexCount-1; // 2
			*ptrIndexArray++ = indexCount-2; // 1
			*ptrIndexArray++ = indexCount;   // 3
			indexCount++;
			vcount-=1;

			/*
                for ( count = 0; count < vcount; count++)
					{
			 *ptrIndexArray++ = indexCount-2; // 2
			 *ptrIndexArray++ = indexCount-1; // 1
			 *ptrIndexArray++ = indexCount;   // 3
				   indexCount++;
				   }
			 */

			 // bullshit optimization ....

			 int odd = vcount&1;
			 vcount/=2;
			 unsigned int* longptr = (unsigned int*) ptrIndexArray;

			 for ( count = 0; count < vcount; count++)
			 {
				 *(longptr++) = (indexCount-2) | ((indexCount-1)<<16);
				 *(longptr++) = (indexCount) | ((indexCount)<<16);
				 *(longptr++) = (indexCount-1) | ((indexCount+1)<<16);
				 indexCount+=2;
			 }
			 ptrIndexArray = (unsigned short*)(longptr);


			 if (odd)
			 {
				 *ptrIndexArray++ = indexCount-2; // 2
				 *ptrIndexArray++ = indexCount-1; // 1
				 *ptrIndexArray++ = indexCount;   // 3
				 indexCount++;
			 }

		}
		else
		{


			//already aligned
			int odd = vcount&1;
			vcount/=2;
			unsigned int* longptr = (unsigned int*) ptrIndexArray;

			for ( count = 0; count < vcount; count++)
			{
				*(longptr++) = (indexCount-1) | ((indexCount-2)<<16);
				*(longptr++) = (indexCount) | ((indexCount-1)<<16);
				*(longptr++) = (indexCount) | ((indexCount+1)<<16);
				indexCount+=2;

			}
			ptrIndexArray = (unsigned short*)(longptr);
			if (odd)
			{

				*ptrIndexArray++ = indexCount-1; // 2
				*ptrIndexArray++ = indexCount-2; // 1
				*ptrIndexArray++ = indexCount;   // 3
				indexCount++;
			}


		}
		vertexCount+=(vertexCount-vertexMark-3)*2;
	}


	break;
	case GL_POLYGON:
	case GL_TRIANGLE_FAN:
	{
		*ptrIndexArray++ = indexCount++;
		*ptrIndexArray++ = indexCount++;
		*ptrIndexArray++ = indexCount++;
		int vcount = ((vertexCount-vertexMark)-3);
		for ( count = 0; count < vcount; count++)
		{
			*ptrIndexArray++ = indexbase;
			*ptrIndexArray++ = indexCount-1;
			*ptrIndexArray++ = indexCount++;
			vertexCount+=2;
		}
	}
	break;

	default:
		break;
	}

	// flush after glEnd()
	//FlushOnStateChange();

}

void nanoglEnable (GLenum cap)
{
	GLboolean statechanged = GL_FALSE;
	switch(cap)
	{
	case GL_ALPHA_TEST:
	{
		if (!nanoglState.alpha_test)
		{
			nanoglState.alpha_test = GL_TRUE;
			statechanged = GL_TRUE;
		}
		break;
	}
	case GL_BLEND:
	{
		if (!nanoglState.blend)
		{
			nanoglState.blend = GL_TRUE;
			statechanged = GL_TRUE;
		}
		break;
	}
	//case GL_CLIP_PLANEi
	case GL_COLOR_LOGIC_OP:
	{
		if (!nanoglState.color_logic_op)
		{
			nanoglState.color_logic_op = GL_TRUE;
			statechanged = GL_TRUE;
		}
		break;
	}
	case GL_COLOR_MATERIAL:
	{
		if (!nanoglState.color_material)
		{
			nanoglState.color_material = GL_TRUE;
			statechanged = GL_TRUE;
		}
		break;
	}
	case GL_CULL_FACE:
	{
		if (!nanoglState.cull_face)
		{
			nanoglState.cull_face = GL_TRUE;
			statechanged = GL_TRUE;
		}
		break;
	}
	case GL_DEPTH_TEST:
	{
		if (!nanoglState.depth_test)
		{
			nanoglState.depth_test = GL_TRUE;
			statechanged = GL_TRUE;
		}
		break;
	}
	case GL_DITHER:
	{
		if (!nanoglState.dither)
		{
			nanoglState.dither = GL_TRUE;
			statechanged = GL_TRUE;
		}
		break;
	}
	case GL_FOG:
		//case GL_LIGHTi
		{
			if (!nanoglState.fog)
			{
				nanoglState.fog = GL_TRUE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_LIGHTING:
		{
			if (!nanoglState.lighting)
			{
				nanoglState.lighting = GL_TRUE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_LINE_SMOOTH:
		{
			if (!nanoglState.line_smooth)
			{
				nanoglState.line_smooth = GL_TRUE;
				statechanged = GL_TRUE;
			}
			break;
		}
		/*        case GL_MATRIX_PALETTE_OES:
            {
            if (!nanoglState.matrix_palette_oes)
                {
                nanoglState.matrix_palette_oes = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }*/
		case GL_MULTISAMPLE:
		{
			if (!nanoglState.multisample)
			{
				nanoglState.multisample = GL_TRUE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_NORMALIZE:
		{
			if (!nanoglState.normalize)
			{
				nanoglState.normalize = GL_TRUE;
				statechanged = GL_TRUE;
			}
			break;
		}
		/*        case GL_POINT_SPRITE_OES:
            {
            if (!nanoglState.point_sprite_oes)
                {
                nanoglState.point_sprite_oes = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }*/
		case GL_POLYGON_OFFSET_FILL:
		{
			if (!nanoglState.polygon_offset_fill)
			{
				nanoglState.polygon_offset_fill = GL_TRUE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_RESCALE_NORMAL:
		{
			if (!nanoglState.rescale_normal)
			{
				nanoglState.rescale_normal = GL_TRUE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_SAMPLE_ALPHA_TO_COVERAGE:
		{
			if (!nanoglState.sample_alpha_to_coverage)
			{
				nanoglState.sample_alpha_to_coverage = GL_TRUE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_SAMPLE_ALPHA_TO_ONE:
		{
			if (!nanoglState.sample_alpha_to_one)
			{
				nanoglState.sample_alpha_to_one = GL_TRUE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_SAMPLE_COVERAGE:
		{
			if (!nanoglState.sample_coverage)
			{
				nanoglState.sample_coverage = GL_TRUE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_SCISSOR_TEST:
		{
			if (!nanoglState.scissor_test)
			{
				nanoglState.scissor_test = GL_TRUE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_STENCIL_TEST:
		{
			return;
			/*            if (!nanoglState.stencil_test)
                {
                nanoglState.stencil_test = GL_TRUE;
                statechanged = GL_TRUE;
                }*/
                break;
		}
		case GL_TEXTURE_2D:
		{
			if (!activetmuState->texture_2d.value)
			{
				FlushOnStateChange();
				GLESIMPL glEnable(cap);
				activetmuState->texture_2d.value = GL_TRUE;
				return;
			}
			break;
		}
		default:
			break;
	}

	if (statechanged)
	{
		FlushOnStateChange();
		GLESIMPL glEnable(cap);
	}
}

void nanoglDisable (GLenum cap)
{
	GLboolean statechanged = GL_FALSE;
	switch(cap)
	{
	case GL_ALPHA_TEST:
	{
		if (nanoglState.alpha_test)
		{
			nanoglState.alpha_test = GL_FALSE;
			statechanged = GL_TRUE;
		}
		break;
	}
	case GL_BLEND:
	{
		if (nanoglState.blend)
		{
			nanoglState.blend = GL_FALSE;
			statechanged = GL_TRUE;
		}
		break;
	}
	//case GL_CLIP_PLANEi
	case GL_COLOR_LOGIC_OP:
	{
		if (nanoglState.color_logic_op)
		{
			nanoglState.color_logic_op = GL_FALSE;
			statechanged = GL_TRUE;
		}
		break;
	}
	case GL_COLOR_MATERIAL:
	{
		if (nanoglState.color_material)
		{
			nanoglState.color_material = GL_FALSE;
			statechanged = GL_TRUE;
		}
		break;
	}
	case GL_CULL_FACE:
	{
		if (nanoglState.cull_face)
		{
			nanoglState.cull_face = GL_FALSE;
			statechanged = GL_TRUE;
		}
		break;
	}
	case GL_DEPTH_TEST:
	{
		if (nanoglState.depth_test)
		{
			nanoglState.depth_test = GL_FALSE;
			statechanged = GL_TRUE;
		}
		break;
	}
	case GL_DITHER:
	{
		if (nanoglState.dither)
		{
			nanoglState.dither = GL_FALSE;
			statechanged = GL_TRUE;
		}
		break;
	}
	case GL_FOG:
		//case GL_LIGHTi
		{
			if (nanoglState.fog)
			{
				nanoglState.fog = GL_FALSE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_LIGHTING:
		{
			if (nanoglState.lighting)
			{
				nanoglState.lighting = GL_FALSE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_LINE_SMOOTH:
		{
			if (nanoglState.line_smooth)
			{
				nanoglState.line_smooth = GL_FALSE;
				statechanged = GL_TRUE;
			}
			break;
		}
		/*        case GL_MATRIX_PALETTE_OES:
            {
            if (nanoglState.matrix_palette_oes)
                {
                nanoglState.matrix_palette_oes = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }*/
		case GL_MULTISAMPLE:
		{
			if (nanoglState.multisample)
			{
				nanoglState.multisample = GL_FALSE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_NORMALIZE:
		{
			if (nanoglState.normalize)
			{
				nanoglState.normalize = GL_FALSE;
				statechanged = GL_TRUE;
			}
			break;
		}
		/*        case GL_POINT_SPRITE_OES:
            {
            if (nanoglState.point_sprite_oes)
                {
                nanoglState.point_sprite_oes = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }*/
		case GL_POLYGON_OFFSET_FILL:
		{
			if (nanoglState.polygon_offset_fill)
			{
				nanoglState.polygon_offset_fill = GL_FALSE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_RESCALE_NORMAL:
		{
			if (nanoglState.rescale_normal)
			{
				nanoglState.rescale_normal = GL_FALSE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_SAMPLE_ALPHA_TO_COVERAGE:
		{
			if (nanoglState.sample_alpha_to_coverage)
			{
				nanoglState.sample_alpha_to_coverage = GL_FALSE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_SAMPLE_ALPHA_TO_ONE:
		{
			if (nanoglState.sample_alpha_to_one)
			{
				nanoglState.sample_alpha_to_one = GL_FALSE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_SAMPLE_COVERAGE:
		{
			if (nanoglState.sample_coverage)
			{
				nanoglState.sample_coverage = GL_FALSE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_SCISSOR_TEST:
		{
			if (nanoglState.scissor_test)
			{
				nanoglState.scissor_test = GL_FALSE;
				statechanged = GL_TRUE;
			}
			break;
		}
		case GL_STENCIL_TEST:
		{
			return;
			/*            if (nanoglState.stencil_test)
                {
                nanoglState.stencil_test = GL_FALSE;
                statechanged = GL_TRUE;
                }*/
                break;
		}
		case GL_TEXTURE_2D:
		{
			if (activetmuState->texture_2d.value)
			{
				FlushOnStateChange();
				GLESIMPL glDisable(cap);
				activetmuState->texture_2d.value = GL_FALSE;
				return;


			}
			break;
		}
		default:
			break;
	}

	if (statechanged)
	{
		FlushOnStateChange();
		GLESIMPL glDisable(cap);
	}
}

void nanoglVertex2f(GLfloat x, GLfloat y)
{
	nanoglVertex3f(x,y,0.0f);
}


static inline unsigned char to_byte(float value)
{
	int retval = ( int)(value *255.0f);
	if (retval > 255)
	{
		retval = 255;
	}
	return (unsigned char)retval;
}


void nanoglColor3f(	GLfloat red, GLfloat green, GLfloat blue)
{
	nanoglColor4f( red, green, blue, 1.0f );
}

void nanoglTexCoord2fv( const GLfloat *v )
{
	nanoglTexCoord2f( v[0], v[1] );
}


void nanoglTexCoord2f(GLfloat s, GLfloat t)
{
	currentVertexAttrib.s = s;
	currentVertexAttrib.t = t;
}

void nanoglViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
	FlushOnStateChange();
	GLESIMPL glViewport(x,y,width,height);
}

void nanoglLoadIdentity (void)
{
	FlushOnStateChange();
	GLESIMPL glLoadIdentity();
}

void nanoglColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
#if	COLOR_BYTE
currentVertexAttrib.red = to_byte(red);
currentVertexAttrib.green = to_byte(green);
currentVertexAttrib.blue = to_byte(blue);
currentVertexAttrib.alpha = to_byte(alpha);
#else
currentVertexAttrib.red = (red);
currentVertexAttrib.green = (green);
currentVertexAttrib.blue = (blue);
currentVertexAttrib.alpha = (alpha);
#endif
}

void nanoglOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	FlushOnStateChange();
#if USE_GLES
	GLESIMPL glOrthof(left,right,bottom,top, zNear,zFar);
#else
	GLESIMPL glOrtho(left,right,bottom,top, zNear,zFar);
#endif
}



void nanoglMatrixMode (GLenum mode)
{
	if (nanoglState.matrixmode == mode)
	{
		return;
	}
	nanoglState.matrixmode = mode;
	FlushOnStateChange();
	GLESIMPL glMatrixMode(mode);
}

void nanoglTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
	if (pname == GL_TEXTURE_BORDER_COLOR)
	{
		return; // not supported by opengl es
	}
	if (    (pname == GL_TEXTURE_WRAP_S ||
			pname == GL_TEXTURE_WRAP_T) &&
			param == GL_CLAMP)
	{
		param = 0x812F;
	}

	FlushOnStateChange();
	GLESIMPL glTexParameterf(target, pname,param);
}

void nanoglTexParameterfv(	GLenum target, GLenum pname, const GLfloat *params)
{
	nanoglTexParameterf(target, pname, params[0]);
}

void nanoglTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
	FlushOnStateChange();
	internalformat = format;
	GLESIMPL glTexImage2D(target, level, internalformat, width, height,border,format,type,pixels);
}

void nanoglDrawBuffer(GLenum mode)
{
}

void nanoglTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
	FlushOnStateChange();
	GLESIMPL glTranslatef(x,y,z);
}

void nanoglRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	FlushOnStateChange();
	GLESIMPL glRotatef(angle, x, y, z);
}

void nanoglScalef (GLfloat x, GLfloat y, GLfloat z)
{
	FlushOnStateChange();
	GLESIMPL glScalef(x,y,z);
}

//void nanoglDepthRange(GLclampf zNear, GLclampf zFar)
void nanoglDepthRange(GLclampd zNear, GLclampd zFar)
{
	if ((nanoglState.depth_range_near == zNear) &&(nanoglState.depth_range_far == zFar))
	{
		return;
	}
	else
	{
		nanoglState.depth_range_near = zNear;
		nanoglState.depth_range_far = zFar;
	}
	FlushOnStateChange();
#if USE_GLES
GLESIMPL glDepthRangef(zNear, zFar);
#else
GLESIMPL glDepthRange(zNear, zFar);
#endif
}

void nanoglDepthFunc (GLenum func)
{
	if (nanoglState.depth_func == func)
	{
		return;
	}
	else
	{
		nanoglState.depth_func = func;
	}
	FlushOnStateChange();
	GLESIMPL glDepthFunc(func);
}

void nanoglFinish (void)
{
	FlushOnStateChange();
	GLESIMPL glFinish();
}

/* GL_OES_matrix_get */
#define GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES               0x898D
#define GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES              0x898E
#define GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES                 0x898F


void nanoglGetFloatv (GLenum pname, GLfloat *params)
{
	FlushOnStateChange();


#if USE_GLES
	// glGetFloatv not implemented in 1.0
	// causes this warning in logcat
	// E/GLLogger( 3891): called unimplemented OpenGL ES API
	if (pname==GL_MODELVIEW_MATRIX){
		GLESIMPL glGetIntegerv (GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES, (GLint*)params);
	}
#else
	GLESIMPL glGetFloatv(pname, params);
#endif
}

void nanoglCullFace (GLenum mode)
{
	if (nanoglState.cullface == mode)
	{
		return;
	}
	else
	{
		nanoglState.cullface = mode;
	}
	FlushOnStateChange();
	GLESIMPL glCullFace(mode);
}

void nanoglFrustum ( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
//void nanoglFrustum (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
	FlushOnStateChange();
#if USE_GLES
	GLESIMPL glFrustumf(left,right,bottom,top,zNear,zFar);
#else
	GLESIMPL glFrustum(left,right,bottom,top,zNear,zFar);
#endif
}

void nanoglClear (GLbitfield mask)
{
	FlushOnStateChange();
	GLESIMPL glClear(mask);
}


void nanoglVertex3f( GLfloat x, GLfloat y, GLfloat z )
{


	currentVertexAttrib.x = x;
	currentVertexAttrib.y = y;
	currentVertexAttrib.z = z;

	ptrVertexAttribArray->x = currentVertexAttrib.x;
	ptrVertexAttribArray->y = currentVertexAttrib.y;
	ptrVertexAttribArray->z = currentVertexAttrib.z;

	ptrVertexAttribArray->red = currentVertexAttrib.red;
	ptrVertexAttribArray->green = currentVertexAttrib.green;
	ptrVertexAttribArray->blue = currentVertexAttrib.blue;
	ptrVertexAttribArray->alpha = currentVertexAttrib.alpha;

	ptrVertexAttribArray->s = currentVertexAttrib.s;
	ptrVertexAttribArray->t = currentVertexAttrib.t;

	ptrVertexAttribArray->s_multi = currentVertexAttrib.s_multi;
	ptrVertexAttribArray->t_multi = currentVertexAttrib.t_multi;

	ptrVertexAttribArray ++;





}

void nanoglColor4fv( const GLfloat *v )
{
	nanoglColor4f(  v[0], v[1], v[2], v[3] );
}



void nanoglColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha )
{
#if	COLOR_BYTE
	currentVertexAttrib.red = red;
	currentVertexAttrib.green = green;
	currentVertexAttrib.blue = blue;
	currentVertexAttrib.alpha = alpha;
#else
	/*static*/ float inv_255 = 1.0f/ 255.0f;
	currentVertexAttrib.red = red * inv_255;
	currentVertexAttrib.green = green * inv_255;
	currentVertexAttrib.blue = blue * inv_255;
	currentVertexAttrib.alpha = alpha * inv_255;
#endif

}

void nanoglColor3ubv( const GLubyte* v)
{
	nanoglColor4ub( v[0], v[1], v[2], 255 );
}

void nanoglColor4ubv( const GLubyte *v )
{
	nanoglColor4ub( v[0], v[1], v[2], v[3] );
}

void nanoglColor3fv( const GLfloat *v )
{
	nanoglColor3f( v[0], v[1], v[2] );
}



void nanoglHint (GLenum target, GLenum mode)
{
	FlushOnStateChange();
	GLESIMPL glHint(target, mode);
}

void nanoglBlendFunc (GLenum sfactor, GLenum dfactor)
{
	if ((nanoglState.sfactor == sfactor) && (nanoglState.dfactor == dfactor))
	{
		return;
	}
	nanoglState.sfactor = sfactor;
	nanoglState.dfactor = dfactor;
	FlushOnStateChange();
	GLESIMPL glBlendFunc(sfactor, dfactor);
}

void nanoglPopMatrix (void)
{
	FlushOnStateChange();
	GLESIMPL glPopMatrix();
}

void nanoglShadeModel (GLenum mode)
{
	if (nanoglState.shademodel == mode)
	{
		return;
	}
	nanoglState.shademodel = mode;
	FlushOnStateChange();
	GLESIMPL glShadeModel(mode);
}

void nanoglPushMatrix (void)
{
	FlushOnStateChange();
	GLESIMPL glPushMatrix();
}

void nanoglTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
	if (target == GL_TEXTURE_ENV)
	{
		if (pname == GL_TEXTURE_ENV_MODE)
		{
			if (param == activetmuState->texture_env_mode.value)
			{
				return;
			}
			else
			{
				FlushOnStateChange();
				GLESIMPL glTexEnvf(target, pname, param);
				activetmuState->texture_env_mode.value = param;
				return;
			}
		}
	}
	FlushOnStateChange();
	GLESIMPL glTexEnvf(target, pname, param);
}

void nanoglVertex3fv( const GLfloat *v )
{
	nanoglVertex3f( v[0], v[1], v[2] );
}


void nanoglDepthMask (GLboolean flag)
{
	if (nanoglState.depthmask == flag)
	{
		return;
	}
	nanoglState.depthmask = flag;
	FlushOnStateChange();
	GLESIMPL glDepthMask(flag);
}

void nanoglBindTexture (GLenum target, GLuint texture)
{
	if (activetmuState->boundtexture.value == texture)
	{
		return;
	}
	FlushOnStateChange();
	activetmuState->boundtexture.value = texture;
	GLESIMPL glBindTexture(target, texture);
}


void nanoglGetIntegerv (GLenum pname, GLint *params)
{
	FlushOnStateChange();
	GLESIMPL glGetIntegerv(pname, params);
}

GLubyte nano_extensions_string[4096];
const GLubyte* nanoglGetString (GLenum name)
{

	if (name == GL_EXTENSIONS)
	{
#if defined(__MULTITEXTURE_SUPPORT__)
        		sprintf((char*)nano_extensions_string,"%s %s",GLESIMPL glGetString(name),"GL_ARB_multitexture EXT_texture_env_add");
#else
	sprintf((char*)nano_extensions_string,"%s %s",GLESIMPL glGetString(name),"EXT_texture_env_add");
#endif
return nano_extensions_string;
	}
	return GLESIMPL glGetString(name);
}

void nanoglAlphaFunc (GLenum func, GLclampf ref)
{
	FlushOnStateChange();
	GLESIMPL glAlphaFunc(func,ref);
}

void nanoglFlush (void)
{
	FlushOnStateChange();
	GLESIMPL glFlush();
}

void nanoglReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
{
	if (format == GL_DEPTH_COMPONENT)
	{
		// OpenglEs 1.1 does not support reading depth buffer without an extension
		memset(pixels, 0xff,4);
		return;
	}
	FlushOnStateChange();
	GLESIMPL glReadPixels(x,y,width,height,format,type,pixels);
}

void nanoglReadBuffer( GLenum mode )
{

}

void nanoglLoadMatrixf (const GLfloat *m)
{
	FlushOnStateChange();
	GLESIMPL glLoadMatrixf(m);
}

void nanoglTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
	FlushOnStateChange();
	GLESIMPL glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void nanoglClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	FlushOnStateChange();
	GLESIMPL glClearColor(red,green,blue,alpha);
}

GLenum nanoglGetError (void)
{
	FlushOnStateChange();
	return GL_NO_ERROR;//GLESIMPL glGetError();
}

void nanoglActiveTexture (GLenum texture)
{
	if (activetmu == texture)
	{
		return;
	}
	if (delayedttmuchange)
	{
		delayedttmuchange = GL_FALSE;
	}
	else
	{
		delayedttmuchange = GL_TRUE;
		delayedtmutarget = texture;
	}
	if (texture == GL_TEXTURE0)
	{
		activetmuState = &tmuState0;
	}
	else
	{
		activetmuState = &tmuState1;
	}
	activetmu = texture;
}

void nanoglClientActiveTexture (GLenum texture)
{
	clientactivetmu = texture;
}

void nanoglPolygonMode( GLenum face, GLenum mode )
{
}

void nanoglDeleteTextures( GLsizei n, const GLuint *textures )
{
	FlushOnStateChange();
	GLESIMPL glDeleteTextures(n,textures);
}

//void nanoglClearDepth( GLclampf depth )
void nanoglClearDepth( GLclampd depth )
{
	FlushOnStateChange();
#if USE_GLES
	GLESIMPL glClearDepthf( depth );
#else
	GLESIMPL glClearDepth( depth );
#endif
}

// glClipPlanef is only available in OpenGL ES 1.1
// not needed by Quake2
void nanoglClipPlane( GLenum plane, const GLdouble *equation )
{
	/*
    FlushOnStateChange();
#if USE_GLES
    float array[4];
    array[0] = (GLfloat)(equation[0]);
    array[1] = (GLfloat)(equation[1]);
    array[2] = (GLfloat)(equation[2]);
    array[3] = (GLfloat)(equation[3]);
    GLESIMPL glClipPlanef( plane, array );
#else
    GLESIMPL glClipPlane( plane, equation );
#endif
	 */
}

void nanoglScissor( GLint x, GLint	y, GLsizei width, GLsizei height )
{
	FlushOnStateChange();
	GLESIMPL glScissor( x, y, width,height);
}

void nanoglPointSize( GLfloat size )
{
	FlushOnStateChange();
	GLESIMPL glPointSize( size );
}

void nanoglArrayElement(GLint i) {}
void nanoglLineWidth(GLfloat width) {}
void nanoglCallList( GLuint list ) {}
void nanoglColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha ) {}
void nanoglStencilFunc( GLenum func, GLint ref, GLuint mask ) {}
void nanoglStencilOp( GLenum fail, GLenum zfail, GLenum zpass ) {}

struct ptrstate vertex_array;
struct ptrstate color_array;
struct ptrstate texture_coord_array;

void nanoglDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )
{
	// ensure that all primitives specified between glBegin/glEnd pairs
	// are rendered first, and that we have correct tmu in use..
	FlushOnStateChange();
	// setup correct vertex/color/texcoord pointers
	if (arraysValid ||
			tmuState0.vertex_array.changed ||
			tmuState0.color_array.changed ||
			tmuState0.texture_coord_array.changed)
	{
		GLESIMPL glClientActiveTexture(GL_TEXTURE0);
	}
	if (arraysValid || tmuState0.vertex_array.changed)
	{
		if (tmuState0.vertex_array.enabled)
		{
			GLESIMPL glEnableClientState(GL_VERTEX_ARRAY);
		}
		else
		{
			GLESIMPL glDisableClientState(GL_VERTEX_ARRAY);
		}
		GLESIMPL glVertexPointer(tmuState0.vertex_array.size,
				tmuState0.vertex_array.type,
				tmuState0.vertex_array.stride,
				tmuState0.vertex_array.ptr);
		tmuState0.vertex_array.changed = GL_FALSE;
	}
	if (arraysValid || tmuState0.color_array.changed)
	{
		if (tmuState0.color_array.enabled)
		{
			GLESIMPL glEnableClientState(GL_COLOR_ARRAY);
		}
		else
		{
			GLESIMPL glDisableClientState(GL_COLOR_ARRAY);
		}
		GLESIMPL glColorPointer(tmuState0.color_array.size,
				tmuState0.color_array.type,
				tmuState0.color_array.stride,
				tmuState0.color_array.ptr);
		tmuState0.color_array.changed = GL_FALSE;
	}
	if (arraysValid || tmuState0.texture_coord_array.changed)
	{
		tmuState0.texture_coord_array.changed = GL_FALSE;
		if (tmuState0.texture_coord_array.enabled)
		{
			GLESIMPL glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		else
		{
			GLESIMPL glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		GLESIMPL glTexCoordPointer(tmuState0.texture_coord_array.size,
				tmuState0.texture_coord_array.type,
				tmuState0.texture_coord_array.stride,
				tmuState0.texture_coord_array.ptr);
	}

	if (arraysValid || tmuState1.texture_coord_array.changed)
	{
		tmuState1.texture_coord_array.changed = GL_FALSE;
		GLESIMPL glClientActiveTexture(GL_TEXTURE1);
		if (tmuState1.texture_coord_array.enabled)
		{
			GLESIMPL glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		else
		{
			GLESIMPL glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		GLESIMPL glTexCoordPointer(tmuState1.texture_coord_array.size,
				tmuState1.texture_coord_array.type,
				tmuState1.texture_coord_array.stride,
				tmuState1.texture_coord_array.ptr);
	}

	arraysValid = GL_FALSE;
	GLESIMPL glDrawElements(mode, count, type, indices);
}

void nanoglEnableClientState(GLenum  array)
{
	nanotmuState* clientstate = NULL;
	if (clientactivetmu == GL_TEXTURE0)
	{
		clientstate = &tmuState0;
	}
	else if (clientactivetmu == GL_TEXTURE1)
	{
		clientstate = &tmuState1;
	}
	else
	{
		return;
	}
	switch (array)
	{
	case GL_VERTEX_ARRAY:
		if (clientstate->vertex_array.enabled)
		{
			return;
		}
		clientstate->vertex_array.enabled = GL_TRUE;
		clientstate->vertex_array.changed = GL_TRUE;
		break;
	case GL_COLOR_ARRAY:
		if (clientstate->color_array.enabled)
		{
			return;
		}
		clientstate->color_array.enabled = GL_TRUE;
		clientstate->color_array.changed = GL_TRUE;

		break;
	case GL_TEXTURE_COORD_ARRAY:
		if (clientstate->texture_coord_array.enabled)
		{
			return;
		}
		clientstate->texture_coord_array.enabled = GL_TRUE;
		clientstate->texture_coord_array.changed = GL_TRUE;
		break;
	default:
		break;
	}
}
void nanoglDisableClientState(GLenum  array)
{
	nanotmuState* clientstate = NULL;
	if (clientactivetmu == GL_TEXTURE0)
	{
		clientstate = &tmuState0;
	}
	else if (clientactivetmu == GL_TEXTURE1)
	{
		clientstate = &tmuState1;
	}
	else
	{
		return;
	}
	switch (array)
	{
	case GL_VERTEX_ARRAY:
		if (!clientstate->vertex_array.enabled)
		{
			return;
		}
		clientstate->vertex_array.enabled = GL_FALSE;
		clientstate->vertex_array.changed = GL_TRUE;
		break;
	case GL_COLOR_ARRAY:
		if (!clientstate->color_array.enabled)
		{
			return;
		}
		clientstate->color_array.enabled = GL_FALSE;
		clientstate->color_array.changed = GL_TRUE;

		break;
	case GL_TEXTURE_COORD_ARRAY:
		if (!clientstate->texture_coord_array.enabled)
		{
			return;
		}
		clientstate->texture_coord_array.enabled = GL_FALSE;
		clientstate->texture_coord_array.changed = GL_TRUE;
		break;
	default:
		break;
	}
}
void nanoglVertexPointer(	GLint size,	GLenum type,GLsizei	stride,	const GLvoid *pointer )
{
	if (tmuState0.vertex_array.size == size &&
			tmuState0.vertex_array.stride == stride &&
			tmuState0.vertex_array.type == type &&
			tmuState0.vertex_array.ptr == pointer)
	{
		return;
	}
	tmuState0.vertex_array.size = size;
	tmuState0.vertex_array.stride = stride;
	tmuState0.vertex_array.type = type;
	tmuState0.vertex_array.ptr  = (GLvoid*)pointer;
	tmuState0.vertex_array.changed = GL_TRUE;
}
void nanoglTexCoordPointer( GLint	size,  GLenum type,  GLsizei stride,  const	GLvoid *pointer	)
{
	nanotmuState* clientstate = NULL;
	if (clientactivetmu == GL_TEXTURE0)
	{
		clientstate = &tmuState0;
	}
	else if (clientactivetmu == GL_TEXTURE1)
	{
		clientstate = &tmuState1;
	}
	if (clientstate->texture_coord_array.size == size &&
			clientstate->texture_coord_array.stride == stride &&
			clientstate->texture_coord_array.type == type &&
			clientstate->texture_coord_array.ptr == pointer)
	{
		return;
	}
	clientstate->texture_coord_array.size = size;
	clientstate->texture_coord_array.stride = stride;
	clientstate->texture_coord_array.type = type;
	clientstate->texture_coord_array.ptr  = (GLvoid*)pointer;
	clientstate->texture_coord_array.changed = GL_TRUE;
}
void nanoglColorPointer( GLint size,  GLenum type,  GLsizei stride,  const GLvoid *pointer )
{
	if (tmuState0.color_array.size == size &&
			tmuState0.color_array.stride == stride &&
			tmuState0.color_array.type == type &&
			tmuState0.color_array.ptr == pointer)
	{
		return;
	}
	tmuState0.color_array.size = size;
	tmuState0.color_array.stride = stride;
	tmuState0.color_array.type = type;
	tmuState0.color_array.ptr  = (GLvoid*)pointer;
	tmuState0.color_array.changed = GL_TRUE;
}
void nanoglPolygonOffset( GLfloat factor, GLfloat units )
{
	FlushOnStateChange();
	GLESIMPL glPolygonOffset(factor, units);
}
void nanoglStencilMask( GLuint mask ) {}
void nanoglClearStencil( GLint s ) {}


#if defined(__MULTITEXTURE_SUPPORT__)


		void nanoglMultiTexCoord2fARB( GLenum target, GLfloat s, GLfloat t )
{
	if (target == GL_TEXTURE0)
	{
		nanoglTexCoord2f(s,t);
	}
	else
	{
		currentVertexAttrib.s_multi = s;
		currentVertexAttrib.t_multi = t;
	}
}
#endif


