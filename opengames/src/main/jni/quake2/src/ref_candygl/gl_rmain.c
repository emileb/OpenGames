/*
Copyright (C) 1997-2001 Id Software, Inc.

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
// r_main.c
#include "gl_local.h"

void R_Clear (void);

#define random()	((rand () & 0x7fff) / ((float)0x7fff))
#define crandom()	(2.0 * (random() - 0.5))

viddef_t	vid;

refimport_t	ri;

int QGL_TEXTURE0, QGL_TEXTURE1;

model_t		*r_worldmodel;

float		gldepthmin, gldepthmax;

glconfig_t gl_config;
glstate_t  gl_state;

image_t		*r_particletextures[PARTICLE_TYPES]; //list for particles
image_t		*r_particlebeam;//used for beam ents
image_t		*r_notexture;		// use for bad textures

entity_t	*currententity;
model_t		*currentmodel;

cplane_t	frustum[4];

int			r_visframecount;	// bumped when going to a new PVS
int			r_framecount;		// used for dlight push checking

int			c_brush_polys, c_alias_polys;

float		v_blend[4];			// final blending color

void GL_Strings_f( void );

//
// view origin
//
vec3_t	vup;
vec3_t	vpn;
vec3_t	vright;
vec3_t	r_origin;

float	r_world_matrix[16];
float	r_base_world_matrix[16];

//
// screen size info
//
refdef_t	r_newrefdef;

int		r_viewcluster, r_viewcluster2, r_oldviewcluster, r_oldviewcluster2;

cvar_t *skydistance; // DMP - skybox size change

cvar_t *gl_transrendersort;
cvar_t *gl_particlelighting;
cvar_t *gl_particledistance;

cvar_t	*r_norefresh;
cvar_t	*r_drawentities;
cvar_t	*r_drawworld;
cvar_t	*r_speeds;
cvar_t	*r_fullbright;
cvar_t	*r_novis;
cvar_t	*r_nocull;
cvar_t	*r_lerpmodels;
cvar_t	*r_lefthand;
cvar_t	*gl_partscale;

cvar_t *r_stainmap;

cvar_t	*r_lightlevel;	// FIXME: This is a HACK to get the client's light level

cvar_t	*gl_nosubimage;
cvar_t	*gl_allow_software;

cvar_t	*gl_vertex_arrays;

cvar_t	*r_cellshading;
cvar_t	*r_cellshading_width;

cvar_t	*con_font;
cvar_t	*con_font_size;

cvar_t	*cl_3dcam;
cvar_t	*cl_3dcam_angle;
cvar_t	*cl_3dcam_chase;
cvar_t	*cl_3dcam_dist;
cvar_t	*cl_3dcam_alpha;
cvar_t	*cl_3dcam_adjust;

cvar_t	*gl_ext_swapinterval;
cvar_t	*gl_ext_multitexture;
cvar_t	*gl_ext_pointparameters;
cvar_t	*gl_ext_compiled_vertex_array;
cvar_t	*gl_ext_palettedtexture;

cvar_t	*gl_log;
cvar_t	*gl_bitdepth;
cvar_t	*gl_drawbuffer;
cvar_t  *gl_driver;
cvar_t	*gl_lightmap;
cvar_t	*gl_shadows;
cvar_t	*gl_shellstencil;
cvar_t	*gl_mode;
cvar_t	*gl_dynamic;
cvar_t  *gl_monolightmap;
cvar_t	*gl_modulate;
cvar_t	*gl_nobind;
cvar_t	*gl_round_down;
cvar_t	*gl_picmip;
cvar_t	*gl_skymip;
cvar_t	*gl_showtris;
cvar_t	*gl_ztrick;
cvar_t	*gl_finish;
cvar_t	*gl_clear;
cvar_t	*gl_cull;
cvar_t	*gl_polyblend;
cvar_t	*gl_flashblend;
cvar_t	*gl_playermip;
cvar_t  *gl_saturatelighting;
cvar_t	*gl_swapinterval;
cvar_t	*gl_texturemode;
cvar_t	*gl_texturealphamode;
cvar_t	*gl_texturesolidmode;
cvar_t	*gl_lockpvs;

cvar_t	*gl_3dlabs_broken;

cvar_t	*vid_fullscreen;
cvar_t	*vid_gamma;
cvar_t	*vid_ref;

/*
=================
GL_Stencil

setting stencil buffer
=================
*/
extern qboolean have_stencil;
void GL_Stencil (qboolean enable)
{
	if (!have_stencil) 
		return;

	if (enable)
	{
		qglEnable(GL_STENCIL_TEST);
		qglStencilFunc(GL_EQUAL, 1, 2);
		qglStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
	}
	else
	{
		qglDisable(GL_STENCIL_TEST);
	}
}

/*
=================
R_CullBox

Returns true if the box is completely outside the frustom
=================
*/
qboolean R_CullBox (vec3_t mins, vec3_t maxs)
{
	int		i;

	if (r_nocull->value)
		return false;

	for (i=0 ; i<4 ; i++)
		if ( BOX_ON_PLANE_SIDE(mins, maxs, &frustum[i]) == 2)
			return true;
	return false;
}


void R_RotateForEntity (entity_t *e, qboolean full)
{
    qglTranslatef (e->origin[0],  e->origin[1],  e->origin[2]);

    qglRotatef (e->angles[1],  0, 0, 1);
	if (full==true)
	{
		qglRotatef (-e->angles[0],  0, 1, 0);
		qglRotatef (-e->angles[2],  1, 0, 0);
	}
}

/*
=============================================================

  SPRITE MODELS

=============================================================
*/


/*
=================
R_DrawSpriteModel

=================
*/
void R_DrawSpriteModel (entity_t *e)
{
	float alpha = 1.0F;
	vec3_t	point;
	dsprframe_t	*frame;
	float		*up, *right;
	dsprite_t		*psprite;

	// don't even bother culling, because it's just a single
	// polygon without a surface cache

	psprite = (dsprite_t *)currentmodel->extradata;

#if 0
	if (e->frame < 0 || e->frame >= psprite->numframes)
	{
		ri.Con_Printf (PRINT_ALL, "no such sprite frame %i\n", e->frame);
		e->frame = 0;
	}
#endif
	e->frame %= psprite->numframes;

	frame = &psprite->frames[e->frame];

#if 0
	if (psprite->type == SPR_ORIENTED)
	{	// bullet marks on walls
	vec3_t		v_forward, v_right, v_up;

	AngleVectors (currententity->angles, v_forward, v_right, v_up);
		up = v_up;
		right = v_right;
	}
	else
#endif
	{	// normal sprite
		up = vup;
		right = vright;
	}

	if ( e->flags & RF_TRANSLUCENT )
		alpha = e->alpha;

	if ((currententity->flags&RF_TRANS_ADDITIVE) && (alpha != 1.0F))
	{ 
		GL_TexEnv( GL_MODULATE );
		qglDepthMask   (false);
		qglBlendFunc   (GL_SRC_ALPHA, GL_ONE);	
		qglEnable( GL_BLEND );

		GL_Bind(currentmodel->skins[e->frame]->texnum);

		qglColor4ub(255, 255, 255, alpha*254);
		
	}
	else
	{
		if ( alpha != 1.0F )
			qglEnable( GL_BLEND );

		GL_TexEnv( GL_MODULATE );
		
		qglColor4f( 1, 1, 1, alpha );
		
		if ( alpha == 1.0 )
			qglEnable (GL_ALPHA_TEST);
		else
			qglDisable( GL_ALPHA_TEST );
		
	 GL_Bind(currentmodel->skins[e->frame]->texnum);
	}


	qglBegin (GL_QUADS);

	qglTexCoord2f (0, 1);
	VectorMA (e->origin, -frame->origin_y, up, point);
	VectorMA (point, -frame->origin_x, right, point);
	qglVertex3fv (point);

	qglTexCoord2f (0, 0);
	VectorMA (e->origin, frame->height - frame->origin_y, up, point);
	VectorMA (point, -frame->origin_x, right, point);
	qglVertex3fv (point);

	qglTexCoord2f (1, 0);
	VectorMA (e->origin, frame->height - frame->origin_y, up, point);
	VectorMA (point, frame->width - frame->origin_x, right, point);
	qglVertex3fv (point);

	qglTexCoord2f (1, 1);
	VectorMA (e->origin, -frame->origin_y, up, point);
	VectorMA (point, frame->width - frame->origin_x, right, point);
	qglVertex3fv (point);
	
	qglEnd ();



	if (currententity->flags&RF_TRANS_ADDITIVE)
	{
		qglBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		qglDepthMask (true);
	}
	else
		qglDisable (GL_ALPHA_TEST);

	GL_TexEnv( GL_REPLACE );

	if ( alpha != 1.0F )
		qglDisable( GL_BLEND );

	qglColor4f( 1, 1, 1, 1 );
}

//==================================================================================

/*
=============
R_DrawNullModel
=============
*/
void R_DrawNullModel (void)
{
	vec3_t	shadelight;
	int		i;

	if ( currententity->flags & RF_FULLBRIGHT )
		shadelight[0] = shadelight[1] = shadelight[2] = 1.0F;
	else
		R_LightPoint (currententity->origin, shadelight, true);

    qglPushMatrix ();
	R_RotateForEntity (currententity, true);

	qglDisable (GL_TEXTURE_2D);
	qglColor3fv (shadelight);

	qglBegin (GL_TRIANGLE_FAN);
	qglVertex3f (0, 0, -16);
	for (i=0 ; i<=4 ; i++)
		qglVertex3f (16*cos(i*M_PI/2), 16*sin(i*M_PI/2), 0);
	qglEnd ();

	qglBegin (GL_TRIANGLE_FAN);
	qglVertex3f (0, 0, 16);
	for (i=4 ; i>=0 ; i--)
		qglVertex3f (16*cos(i*M_PI/2), 16*sin(i*M_PI/2), 0);
	qglEnd ();

	qglColor3f (1,1,1);
	qglPopMatrix ();
	qglEnable (GL_TEXTURE_2D);
}
/*
=============
R_DrawEntitiesOnList
=============
*/
typedef struct sortedent_s 
{
	entity_t *ent;
	vec_t len;
	qboolean inwater;
} sortedent_t;

sortedent_t theents[MAX_ENTITIES];

int transCompare(const void *arg1, const void *arg2 ) 
{
	const sortedent_t *a1, *a2;
	a1 = (sortedent_t *) arg1;
	a2 = (sortedent_t *) arg2;
	return (a2->len - a1->len);
}

sortedent_t NewSortEnt (entity_t *ent, qboolean waterstart)
{
	vec3_t result;
	sortedent_t s_ent;

	s_ent.ent=ent;

	VectorSubtract(ent->origin, r_origin, result);
	s_ent.len = (result[0] * result[0]) + (result[1] * result[1]) + (result[2] * result[2]);

	s_ent.inwater = (Mod_PointInLeaf(ent->origin, r_worldmodel)->contents & MASK_WATER);
	if ( ent->flags & RF_WEAPONMODEL || ent->flags & RF_VIEWERMODEL )
		s_ent.inwater = waterstart;

	return s_ent;
}
void R_SortEntitiesOnList (qboolean inwater)
{
	int i;

	for (i=0 ; i<r_newrefdef.num_entities ; i++)
		theents[i] = NewSortEnt(&r_newrefdef.entities[i], inwater);

	qsort((void *) theents, r_newrefdef.num_entities, sizeof(sortedent_t), transCompare);
}

void R_DrawEntitiesOnList (qboolean inWater, qboolean solids)
{
	vec3_t origin;
	int		i, j;

	if (!r_drawentities->value)
		return;

	// draw non-transparent first
	if (solids)
		for (i=0;i<r_newrefdef.num_entities; i++)
		{
			currententity = &r_newrefdef.entities[i];

			if (currententity->flags&RF_TRANSLUCENT || currententity->flags&RF_VIEWERMODEL)
				continue;

			for (j=0;j<3;j++)
				origin[j]=currententity->origin[j];

			if ( currententity->flags & RF_BEAM )
			{
				R_DrawBeam( currententity );
			}
			else
			{
				currentmodel = currententity->model;
				if (!currentmodel)
				{
					R_DrawNullModel ();
					continue;
				}
				switch (currentmodel->type)
				{
				case mod_alias:
					R_DrawAliasModel (currententity);
					break;
				case mod_brush:
					R_DrawBrushModel (currententity);
					break;
				case mod_sprite:
					R_DrawSpriteModel (currententity);
					break;
				default:
					ri.Sys_Error (ERR_DROP, "Bad modeltype");
					break;
				}
			}
		}

	qglDepthMask (0);
	for (i=0;i<r_newrefdef.num_entities; i++)
	{
		if (gl_transrendersort->value && !(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
		{
			currententity = theents[i].ent;

			if (!(currententity->flags&RF_TRANSLUCENT || currententity->flags&RF_VIEWERMODEL) ||
				(inWater && !theents[i].inwater) || (!inWater && theents[i].inwater))
				continue;	// solid
		}
		else if (inWater)
		{
			currententity = &r_newrefdef.entities[i];

			if (!(currententity->flags&RF_TRANSLUCENT || currententity->flags&RF_VIEWERMODEL))
				continue;	// solid
		}
		else
			continue;

		if ( currententity->flags & RF_BEAM )
		{
			R_DrawBeam( currententity );
		}
		else
		{
			currentmodel = currententity->model;

			if (!currentmodel)
			{
				R_DrawNullModel ();
				continue;
			}
			switch (currentmodel->type)
			{
			case mod_alias:
				R_DrawAliasModel (currententity);
				break;
			case mod_brush:
				R_DrawBrushModel (currententity);
				break;
			case mod_sprite:
				R_DrawSpriteModel (currententity);
				break;
			default:
				ri.Sys_Error (ERR_DROP, "Bad modeltype");
				break;
			}
		}
	}
	qglDepthMask (1);		// back to writing

}

/*
** GL_DrawParticles
**
*/

typedef struct sortedpart_s 
{
	particle_t *p;
	vec_t len;
	qboolean inwater;
} sortedpart_t;

sortedpart_t theparts[MAX_PARTICLES];

sortedpart_t NewSortPart (particle_t *p)
{
	vec3_t result;
	sortedpart_t s_part;

	s_part.p = p;
	VectorSubtract(p->origin, r_origin, result);
	s_part.len = (result[0] * result[0]) + (result[1] * result[1]) + (result[2] * result[2]);
	s_part.inwater = (Mod_PointInLeaf(p->origin, r_worldmodel)->contents & MASK_WATER);

	return s_part;
}

void R_SortParticlesOnList (int num_particles, const particle_t particles[])
{
	int i;

	for (i=0 ; i<num_particles ; i++)
		theparts[i] = NewSortPart((particle_t *)&particles[i]);

	qsort((void *) theparts, num_particles, sizeof(sortedpart_t), transCompare);
}

int texParticle (int type)
{
	image_t		*part_img;

	part_img = r_particletextures [type];

	return part_img->texnum;
}

void vectoanglerolled (vec3_t value1, float angleyaw, vec3_t angles)
{
	float	forward, yaw, pitch;

	yaw = (int) (atan2(value1[1], value1[0]) * 180 / M_PI);
	forward = sqrt (value1[0]*value1[0] + value1[1]*value1[1]);
	pitch = (int) (atan2(value1[2], forward) * 180 / M_PI);

	if (pitch < 0)
		pitch += 360;

	angles[PITCH] = -pitch;
	angles[YAW] =  yaw;
	angles[ROLL] = - angleyaw;
}

float AngleFind(float input)
{
	return 180.0/input;
}
void GL_DrawParticles( int num_particles, qboolean inWater )
{
	int		i;
	float	size, lighting = gl_particlelighting->value;
	int		oldrender=0, rendertype = 0;
	
	byte		alpha;
	
	vec3_t		up		= {vup[0]    * 0.75f, vup[1]    * 0.75f, vup[2]    * 0.75f};
	vec3_t		right	= {vright[0] * 0.75f, vright[1] * 0.75f, vright[2] * 0.75f};
	vec3_t		coord[4], shadelight;

	particle_t	*p;

	VectorAdd      (up, right, coord[0]);
	VectorSubtract (right, up, coord[1]);
	VectorNegate   (coord[0], coord[2]);
	VectorNegate   (coord[1], coord[3]);
	
	qglEnable( GL_TEXTURE_2D );
	GL_TexEnv( GL_MODULATE );
	qglDepthMask   (false);
	qglBlendFunc   (GL_SRC_ALPHA, GL_ONE);
	qglEnable( GL_BLEND );
	qglShadeModel (GL_SMOOTH);


	
	  for ( i=0 ; i < num_particles ; i++)
	{
		if (!(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
		{
			if (gl_transrendersort->value==2)//sorted with ents etc
			{
				p = theparts[i].p;

				if ( (inWater && !theparts[i].inwater) || (!inWater && theparts[i].inwater) )
					continue;

				if ( gl_particledistance->value > 0 && theparts[i].len>(100.0*gl_particledistance->value))
					continue;
			}
			else if (gl_transrendersort->value)//sorted by itself
			{
				p = theparts[i].p;

				if ( (inWater && !theparts[i].inwater) || (!inWater && theparts[i].inwater) )
					continue;

				if ( gl_particledistance->value > 0 && theparts[i].len>(100.0*gl_particledistance->value))
					continue;
			}
			else if (inWater)
				p=&r_newrefdef.particles[i];
			else
				continue;
		}
		else if (inWater)
			p=&r_newrefdef.particles[i];
		else
			continue;

		oldrender=rendertype; rendertype = (p->flags&PART_TRANS)? 1: 0;
		if (rendertype!=oldrender)
		{
			if (rendertype==1)
				qglBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			else
				qglBlendFunc   (GL_SRC_ALPHA, GL_ONE);
		} 


		if (!(p->flags&PART_SPARK))
			GL_Bind(texParticle(p->image));

		alpha	= p->alpha * 254.0;
		size = (p->size>0.1) ? p->size : 0.1;

		if (p->flags&PART_SHADED)
		{
			int j;
			float lightest = 0;

			R_LightPoint (p->origin, shadelight, false);

			shadelight[0]= (lighting*shadelight[0]+(1-lighting)) * p->red;
			shadelight[1]= (lighting*shadelight[1]+(1-lighting)) * p->green;
			shadelight[2]= (lighting*shadelight[2]+(1-lighting)) * p->blue;

			//this cleans up the lighting
			{
				for (j=0;j<3;j++)
					if (shadelight[j]>lightest)
						lightest= shadelight[j];
				if (lightest>255)
					for (j=0;j<3;j++)
						shadelight[j]*= 255/lightest;
			}

			qglColor4ub( shadelight[0], shadelight[1], shadelight[2], alpha);
		}
		else
			qglColor4ub(p->red, p->green, p->blue, alpha);

//id move this somewhere less dirty, but im too damn lazy - psychospaz
#define DEPTHHACK_RANGE_SHORT	0.9985f
#define DEPTHHACK_RANGE_MID		0.975f
#define DEPTHHACK_RANGE_LONG	0.95f

		if (p->flags&PART_DEPTHHACK_SHORT) //nice little poly-peeking - psychospaz
			qglDepthRange (gldepthmin, gldepthmin + DEPTHHACK_RANGE_SHORT*(gldepthmax-gldepthmin));
		if (p->flags&PART_DEPTHHACK_MID)
			qglDepthRange (gldepthmin, gldepthmin + DEPTHHACK_RANGE_MID*(gldepthmax-gldepthmin));
		if (p->flags&PART_DEPTHHACK_LONG)
			qglDepthRange (gldepthmin, gldepthmin + DEPTHHACK_RANGE_LONG*(gldepthmax-gldepthmin));

		if (p->flags&PART_SPARK) //QMB style particles
		{
			int angle;
			vec3_t v;

			qglDisable (GL_TEXTURE_2D);

			qglPushMatrix();
			{
				qglBegin (GL_TRIANGLE_FAN);
				{
#define MAGICNUMBER 32
					float magicnumber = AngleFind(MAGICNUMBER);


					qglVertex3fv (p->origin);
					qglColor4ub( 0, 0, 0, 0);

					for (angle=magicnumber*fabs(1-MAGICNUMBER) ; angle>=0 ; angle-=magicnumber)
					{
						v[0]=p->origin[0] - p->angle[0]*size + (right[0]*cos(angle) + up[0]*sin(angle));
						v[1]=p->origin[1] - p->angle[1]*size + (right[1]*cos(angle) + up[1]*sin(angle));
						v[2]=p->origin[2] - p->angle[2]*size + (right[2]*cos(angle) + up[2]*sin(angle));
						qglVertex3fv (v);
					}
				}
				qglEnd ();
			}
			qglPopMatrix ();

			qglEnable (GL_TEXTURE_2D);
		}
		else if (p->flags&PART_BEAM) //given start and end positions, will have fun here :)
		{	//p->angle = start
			//p->origin= end
			vec3_t	angl_coord[4];
			vec3_t	ang_up, ang_right, vdelta;

			VectorSubtract(p->origin, p->angle, vdelta);
			VectorNormalize(vdelta);
		
			VectorCopy(vdelta, ang_up);
			VectorSubtract(r_newrefdef.vieworg, p->angle, vdelta);
			CrossProduct(ang_up, vdelta, ang_right);
			if(!VectorCompare(ang_right, vec3_origin))
				VectorNormalize(ang_right);

			VectorScale(ang_right, size, ang_right);

			VectorAdd(p->origin, ang_right, angl_coord[0]);
			VectorAdd(p->angle, ang_right, angl_coord[1]);
			VectorSubtract(p->angle, ang_right, angl_coord[2]);
			VectorSubtract(p->origin, ang_right, angl_coord[3]);

			qglPushMatrix();
			{
				qglBegin (GL_QUADS);
				{
					qglTexCoord2f (0, 1);
					qglVertex3fv  (angl_coord[0]);
					qglTexCoord2f (0, 0);
					qglVertex3fv  (angl_coord[1]);
					qglTexCoord2f (1, 0); 
					qglVertex3fv  (angl_coord[2]);
					qglTexCoord2f (1, 1);
					qglVertex3fv  (angl_coord[3]);
				}
				qglEnd ();

			}
			qglPopMatrix ();
		}
		else if (p->flags&PART_LIGHTNING) //given start and end positions, will have fun here :)
		{	//p->angle = start
			//p->origin= end
		  float	tlen, halflen, len, dec = size, warpfactor, warpadd, oldwarpadd, warpsize, time, i=0, factor;
			vec3_t	move, lastvec, thisvec, tempvec;
			vec3_t	angl_coord[4];
			vec3_t	ang_up, ang_right, vdelta;
			
			oldwarpadd=0;
			warpsize = dec * 0.01;
			warpfactor = M_PI*1.0;
			time = -r_newrefdef.time*10.0;

			VectorSubtract(p->origin, p->angle, move);
			len = VectorNormalize(move);
		
			VectorCopy(move, ang_up);
			VectorSubtract(r_newrefdef.vieworg, p->angle, vdelta);
			CrossProduct(ang_up, vdelta, ang_right);
			if(!VectorCompare(ang_right, vec3_origin))
				VectorNormalize(ang_right);

			VectorScale (ang_right, dec, ang_right);
			VectorScale (ang_up, dec, ang_up);

			VectorScale(move, dec, move);
			VectorCopy(p->angle, lastvec);
			VectorAdd(lastvec, move, thisvec);
			VectorCopy(thisvec, tempvec);

			//lightning starts at point and then flares out
#define LIGHTNINGWARPFUNCTION 0.25*sin(time+i+warpfactor)*(dec/len)

			warpadd = LIGHTNINGWARPFUNCTION;
			factor = 1;
			halflen = len/2.0;

			
			thisvec[0]= (thisvec[0]*2 + crandom()*5)/2;
			thisvec[1]= (thisvec[1]*2 + crandom()*5)/2;
			thisvec[2]= (thisvec[2]*2 + crandom()*5)/2;
			/*
			  thisvec[0]= (thisvec[0]*2 + 5)/2;
			  thisvec[1]= (thisvec[1]*2 + 5)/2;
			  thisvec[2]= (thisvec[2]*2 + 5)/2;
			*/
			while (len>dec)
			{	i+=warpsize;

				VectorAdd(thisvec, ang_right, angl_coord[0]);
				VectorAdd(lastvec, ang_right, angl_coord[1]);
				VectorSubtract(lastvec, ang_right, angl_coord[2]);
				VectorSubtract(thisvec, ang_right, angl_coord[3]);

				qglPushMatrix();
				{
					qglBegin (GL_QUADS);
					{
						qglTexCoord2f (0+warpadd, 1);
						qglVertex3fv  (angl_coord[0]);
						qglTexCoord2f (0+oldwarpadd, 0);
						qglVertex3fv  (angl_coord[1]);
						qglTexCoord2f (1+oldwarpadd, 0); 
						qglVertex3fv  (angl_coord[2]);
						qglTexCoord2f (1+warpadd, 1);
						qglVertex3fv  (angl_coord[3]);
					}
					qglEnd ();

				}
				qglPopMatrix ();

				tlen = (len<halflen)? fabs(len-halflen): fabs(len-halflen);
				factor = tlen/(size*size);

				if (factor > 4)
					factor = 4;
				else if (factor < 1)
					factor = 1;

				oldwarpadd = warpadd;
				warpadd = LIGHTNINGWARPFUNCTION;

				len-=dec;
				VectorCopy(thisvec, lastvec);
				VectorAdd(tempvec, move, tempvec);
				VectorAdd(lastvec, move, thisvec);
				
				
				/*
				  thisvec[0]= ((thisvec[0] + (crandom()*size)) + tempvec[0]*factor)/(factor+1);
				  thisvec[1]= ((thisvec[1] + (crandom()*size)) + tempvec[1]*factor)/(factor+1);
				  thisvec[2]= ((thisvec[2] + (crandom()*size)) + tempvec[2]*factor)/(factor+1);
				*/
				thisvec[0]= ((thisvec[0] + (size)) + tempvec[0]*factor)/(factor+1);
				thisvec[1]= ((thisvec[1] + (size)) + tempvec[1]*factor)/(factor+1);
				thisvec[2]= ((thisvec[2] + (size)) + tempvec[2]*factor)/(factor+1);
				
			}

			i+=warpsize;

			//one more time
			if (len>0)
			{
				VectorAdd(p->origin, ang_right, angl_coord[0]);
				VectorAdd(lastvec, ang_right, angl_coord[1]);
				VectorSubtract(lastvec, ang_right, angl_coord[2]);
				VectorSubtract(p->origin, ang_right, angl_coord[3]);

				qglPushMatrix();
				{
					qglBegin (GL_QUADS);
					{
						qglTexCoord2f (0+warpadd, 1);
						qglVertex3fv  (angl_coord[0]);
						qglTexCoord2f (0+oldwarpadd, 0);
						qglVertex3fv  (angl_coord[1]);
						qglTexCoord2f (1+oldwarpadd, 0); 
						qglVertex3fv  (angl_coord[2]);
						qglTexCoord2f (1+warpadd, 1);
						qglVertex3fv  (angl_coord[3]);
					}
					qglEnd ();

				}
				qglPopMatrix ();
			}
		}
		else if (p->flags&PART_DIRECTION) //streched out in direction...tracers etc...
			//never dissapears because of angle like other engines :D
		{
			float len, dot;
			vec3_t angl_coord[4];
			vec3_t ang_up, ang_right, vdelta;

			VectorCopy(p->angle, ang_up);
			len = VectorNormalize(ang_up);
			VectorSubtract(r_newrefdef.vieworg, p->origin, vdelta);
			CrossProduct(ang_up, vdelta, ang_right);
			if(!VectorCompare(ang_right, vec3_origin))
				VectorNormalize(ang_right);

			
			//the more you look into ang_up, the more it looks like normal sprite (recalc ang_up)
			if (false)
			{
			  vec3_t temp_up[3], tempfor;
			  
			  AngleVectors(r_newrefdef.viewangles, tempfor, NULL, NULL); 
			  dot = 1-fabs(DotProduct (ang_up, tempfor));
			  
			  //	if (DotProduct (vdelta, ang_up)<0 || DotProduct (vdelta, ang_right)<0)
			  //		VectorScale(tempfor, -1.0, tempfor);
			  
				VectorSubtract(p->origin, r_newrefdef.vieworg, vdelta);
				Com_Printf("up:%f right:%f\n", DotProduct (vdelta, ang_up), DotProduct (vdelta, ang_right));

				ProjectPointOnPlane( temp_up[2], ang_up, tempfor );
				VectorNormalize(temp_up[2]);

				VectorScale(ang_up, dot, temp_up[0]);
				VectorScale(temp_up[2], 1-dot, temp_up[1]);
				VectorAdd(temp_up[1], temp_up[0], ang_up);

				len = 1+dot*(len-1);

				//dont mess with stuff harder than needed
				if (dot<0.015)
				{
					len = 1;
					VectorCopy(up, ang_up);
					VectorCopy(right, ang_right);
				}
			}

			VectorScale (ang_right, 0.75f, ang_right);
			VectorScale (ang_up, 0.75f * len, ang_up);

			VectorAdd      (ang_up, ang_right, angl_coord[0]);
			VectorSubtract (ang_right, ang_up, angl_coord[1]);
			VectorNegate   (angl_coord[0], angl_coord[2]);
			VectorNegate   (angl_coord[1], angl_coord[3]);

			qglPushMatrix();
			{
				qglTranslatef( p->origin[0], p->origin[1], p->origin[2] );
				qglScalef(size, size, size );

				qglBegin (GL_QUADS);
				{
					qglTexCoord2f (0, 1);
					qglVertex3fv  (angl_coord[0]);
					qglTexCoord2f (0, 0);
					qglVertex3fv  (angl_coord[1]);
					qglTexCoord2f (1, 0); 
					qglVertex3fv  (angl_coord[2]);
					qglTexCoord2f (1, 1);
					qglVertex3fv  (angl_coord[3]);
				}
				qglEnd ();
			}
			qglPopMatrix ();
		}
		else if (p->flags&PART_ANGLED) //facing direction... (decal wannabes)
		{
			vec3_t angl_coord[4];
			vec3_t ang_up, ang_right, ang_forward;

			AngleVectors(p->angle, ang_forward, ang_right, ang_up); 

			VectorScale (ang_right, 0.75f, ang_right);
			VectorScale (ang_up, 0.75f, ang_up);

			VectorAdd      (ang_up, ang_right, angl_coord[0]);
			VectorSubtract (ang_right, ang_up, angl_coord[1]);
			VectorNegate   (angl_coord[0], angl_coord[2]);
			VectorNegate   (angl_coord[1], angl_coord[3]);

			if (p->flags&PART_DEPTHHACK)
			{
				qglDisable (GL_CULL_FACE);
				qglPushMatrix();
				{
					if (p->flags&(PART_DEPTHHACK_SHORT|PART_DEPTHHACK_MID|PART_DEPTHHACK_LONG))
					{
						VectorMA(p->origin, size, angl_coord[0], angl_coord[0]);
						VectorMA(p->origin, size, angl_coord[1], angl_coord[1]);
						VectorMA(p->origin, size, angl_coord[2], angl_coord[2]);
						VectorMA(p->origin, size, angl_coord[3], angl_coord[3]);
					}
					else
					{
						qglTranslatef( p->origin[0], p->origin[1], p->origin[2] );
						qglScalef(size, size, size );
					}

					qglBegin (GL_QUADS);
					{
						qglTexCoord2f (0, 1);
						qglVertex3fv  (angl_coord[0]);
						qglTexCoord2f (0, 0);
						qglVertex3fv  (angl_coord[1]);
						qglTexCoord2f (1, 0); 
						qglVertex3fv  (angl_coord[2]);
						qglTexCoord2f (1, 1);
						qglVertex3fv  (angl_coord[3]);
					}
					qglEnd ();
				}
				qglPopMatrix ();
				qglEnable (GL_CULL_FACE);
			}
		}
		else //normal sprites
		{
			qglPushMatrix();
			{
				if (!(p->flags&PART_DEPTHHACK))
				{
					qglTranslatef( p->origin[0], p->origin[1], p->origin[2] );
					qglScalef(size, size, size );
				}

				qglBegin (GL_QUADS);
				{
					if (p->angle[2]) //if we have roll, we do calcs
					{
						vec3_t angl_coord[4];
						vec3_t ang_up, ang_right, ang_forward;

						VectorSubtract(p->origin, r_newrefdef.vieworg, angl_coord[0]);

						vectoanglerolled(angl_coord[0], p->angle[2], angl_coord[1]);
						AngleVectors(angl_coord[1], ang_forward, ang_right, ang_up); 

						VectorScale (ang_forward, 0.75f, ang_forward);
						VectorScale (ang_right, 0.75f, ang_right);
						VectorScale (ang_up, 0.75f, ang_up);

						VectorAdd      (ang_up, ang_right, angl_coord[0]);
						VectorSubtract (ang_right, ang_up, angl_coord[1]);
						VectorNegate   (angl_coord[0], angl_coord[2]);
						VectorNegate   (angl_coord[1], angl_coord[3]);

						if (p->flags&PART_DEPTHHACK)
						{
							VectorMA(p->origin, size, angl_coord[0], angl_coord[0]);
							VectorMA(p->origin, size, angl_coord[1], angl_coord[1]);
							VectorMA(p->origin, size, angl_coord[2], angl_coord[2]);
							VectorMA(p->origin, size, angl_coord[3], angl_coord[3]);
						}

						qglTexCoord2f (0, 1);
						qglVertex3fv  (angl_coord[0]);
						qglTexCoord2f (0, 0);
						qglVertex3fv  (angl_coord[1]);
						qglTexCoord2f (1, 0); 
						qglVertex3fv  (angl_coord[2]);
						qglTexCoord2f (1, 1);
						qglVertex3fv  (angl_coord[3]);
					}
					else if (p->flags&PART_DEPTHHACK)
					{
						vec3_t angl_coord[4];

						VectorMA(p->origin, size, coord[0], angl_coord[0]);
						VectorMA(p->origin, size, coord[1], angl_coord[1]);
						VectorMA(p->origin, size, coord[2], angl_coord[2]);
						VectorMA(p->origin, size, coord[3], angl_coord[3]);

						qglTexCoord2f (0, 1);
						qglVertex3fv  (angl_coord[0]);
						qglTexCoord2f (0, 0);
						qglVertex3fv  (angl_coord[1]);
						qglTexCoord2f (1, 0); 
						qglVertex3fv  (angl_coord[2]);
						qglTexCoord2f (1, 1);
						qglVertex3fv  (angl_coord[3]);
					}
					else
					{
						qglTexCoord2f (0, 1);
						qglVertex3fv  (coord[0]);
						qglTexCoord2f (0, 0);
						qglVertex3fv  (coord[1]);
						qglTexCoord2f (1, 0); 
						qglVertex3fv  (coord[2]);
						qglTexCoord2f (1, 1);
						qglVertex3fv  (coord[3]);

					}
				}
				qglEnd ();
			}
			qglPopMatrix ();
		}

		if (p->flags&PART_DEPTHHACK)
			qglDepthRange (gldepthmin, gldepthmax);
	}

	qglDepthRange (gldepthmin, gldepthmax);
	qglBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GL_TexEnv( GL_MODULATE );
	qglDepthMask (true);
	qglDisable ( GL_BLEND );
	qglColor4f   (1,1,1,1);
}

/*
===============
R_DrawParticles
===============
*/
void R_DrawParticles (qboolean inWater)
{
	qglDisable(GL_FOG); //no fogging my precious

	GL_DrawParticles( r_newrefdef.num_particles, inWater );
	
	//Dont know if it was on...
	//qglEnable(GL_FOG);
}

/*
============
R_PolyBlend
============
*/
void R_PolyBlend (void)
{
	if (!gl_polyblend->value)
		return;
	if (!v_blend[3])
		return;

	qglDisable (GL_ALPHA_TEST);
	qglEnable (GL_BLEND);
	qglDisable (GL_DEPTH_TEST);
	qglDisable (GL_TEXTURE_2D);

    qglLoadIdentity ();

	// FIXME: get rid of these
    qglRotatef (-90,  1, 0, 0);	    // put Z going up
    qglRotatef (90,  0, 0, 1);	    // put Z going up

	qglColor4fv (v_blend);

	qglBegin (GL_QUADS);

	qglVertex3f (10, 100, 100);
	qglVertex3f (10, -100, 100);
	qglVertex3f (10, -100, -100);
	qglVertex3f (10, 100, -100);
	qglEnd ();

	qglDisable (GL_BLEND);
	qglEnable (GL_TEXTURE_2D);
	qglEnable (GL_ALPHA_TEST);

	qglColor4f(1,1,1,1);
}

//=======================================================================

int SignbitsForPlane (cplane_t *out)
{
	int	bits, j;

	// for fast box on planeside test

	bits = 0;
	for (j=0 ; j<3 ; j++)
	{
		if (out->normal[j] < 0)
			bits |= 1<<j;
	}
	return bits;
}


void R_SetFrustum (void)
{
	int		i;

#if 0
	/*
	** this code is wrong, since it presume a 90 degree FOV both in the
	** horizontal and vertical plane
	*/
	// front side is visible
	VectorAdd (vpn, vright, frustum[0].normal);
	VectorSubtract (vpn, vright, frustum[1].normal);
	VectorAdd (vpn, vup, frustum[2].normal);
	VectorSubtract (vpn, vup, frustum[3].normal);

	// we theoretically don't need to normalize these vectors, but I do it
	// anyway so that debugging is a little easier
	VectorNormalize( frustum[0].normal );
	VectorNormalize( frustum[1].normal );
	VectorNormalize( frustum[2].normal );
	VectorNormalize( frustum[3].normal );
#else
	// rotate VPN right by FOV_X/2 degrees
	RotatePointAroundVector( frustum[0].normal, vup, vpn, -(90-r_newrefdef.fov_x / 2 ) );
	// rotate VPN left by FOV_X/2 degrees
	RotatePointAroundVector( frustum[1].normal, vup, vpn, 90-r_newrefdef.fov_x / 2 );
	// rotate VPN up by FOV_X/2 degrees
	RotatePointAroundVector( frustum[2].normal, vright, vpn, 90-r_newrefdef.fov_y / 2 );
	// rotate VPN down by FOV_X/2 degrees
	RotatePointAroundVector( frustum[3].normal, vright, vpn, -( 90 - r_newrefdef.fov_y / 2 ) );
#endif

	for (i=0 ; i<4 ; i++)
	{
		frustum[i].type = PLANE_ANYZ;
		frustum[i].dist = DotProduct (r_origin, frustum[i].normal);
		frustum[i].signbits = SignbitsForPlane (&frustum[i]);
	}
}

//=======================================================================

/*
===============
R_SetupFrame
===============
*/
void R_SetupFrame (void)
{
	int i;
	mleaf_t	*leaf;

	r_framecount++;

// build the transformation matrix for the given view angles
	VectorCopy (r_newrefdef.vieworg, r_origin);

	AngleVectors (r_newrefdef.viewangles, vpn, vright, vup);

// current viewcluster
	if ( !( r_newrefdef.rdflags & RDF_NOWORLDMODEL ) )
	{
		r_oldviewcluster = r_viewcluster;
		r_oldviewcluster2 = r_viewcluster2;
		leaf = Mod_PointInLeaf (r_origin, r_worldmodel);
		r_viewcluster = r_viewcluster2 = leaf->cluster;

		// check above and below so crossing solid water doesn't draw wrong
		if (!leaf->contents)
		{	// look down a bit
			vec3_t	temp;

			VectorCopy (r_origin, temp);
			temp[2] -= 16;
			leaf = Mod_PointInLeaf (temp, r_worldmodel);
			if ( !(leaf->contents & CONTENTS_SOLID) &&
				(leaf->cluster != r_viewcluster2) )
				r_viewcluster2 = leaf->cluster;
		}
		else
		{	// look up a bit
			vec3_t	temp;

			VectorCopy (r_origin, temp);
			temp[2] += 16;
			leaf = Mod_PointInLeaf (temp, r_worldmodel);
			if ( !(leaf->contents & CONTENTS_SOLID) &&
				(leaf->cluster != r_viewcluster2) )
				r_viewcluster2 = leaf->cluster;
		}
	}

	for (i=0 ; i<4 ; i++)
		v_blend[i] = r_newrefdef.blend[i];

	c_brush_polys = 0;
	c_alias_polys = 0;

	// clear out the portion of the screen that the NOWORLDMODEL defines
	if ( r_newrefdef.rdflags & RDF_NOWORLDMODEL )
	{
		qglEnable( GL_SCISSOR_TEST );
		qglClearColor( 0.3, 0.3, 0.3, 1 );
		qglScissor( r_newrefdef.x, vid.height - r_newrefdef.height - r_newrefdef.y, r_newrefdef.width, r_newrefdef.height );
		qglClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		qglClearColor( 1, 0, 0.5, 0.5 );
		qglDisable( GL_SCISSOR_TEST );
	}
}


void MYgluPerspective( GLdouble fovy, GLdouble aspect,
		     GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan( fovy * M_PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   xmin += -( 2 * gl_state.camera_separation ) / zNear;
   xmax += -( 2 * gl_state.camera_separation ) / zNear;

   qglFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}


/*
=============
R_SetupGL
=============
*/
void R_SetupGL (void)
{
	static GLdouble farz; // DMP skybox size change
	GLdouble boxsize;  // DMP skybox size change

	float	screenaspect;
//	float	yfov;
	int		x, x2, y2, y, w, h;

	//
	// set up viewport
	//
	x = floor(r_newrefdef.x * vid.width / vid.width);
	x2 = ceil((r_newrefdef.x + r_newrefdef.width) * vid.width / vid.width);
	y = floor(vid.height - r_newrefdef.y * vid.height / vid.height);
	y2 = ceil(vid.height - (r_newrefdef.y + r_newrefdef.height) * vid.height / vid.height);

	w = x2 - x;
	h = y - y2;

	qglViewport (x, y2, w, h);

	if (skydistance->modified)
	{
		skydistance->modified = false;
		boxsize = skydistance->value;
		boxsize -= 252 * ceil(boxsize / 2300);
		farz = 1.0;
		while (farz < boxsize)  // DMP: make this value a power-of-2
		{
			farz *= 2.0;
			if (farz >= 65536.0)  // DMP: don't make it larger than this
				break;
  		}
		farz *= 2.0;	// DMP: double since boxsize is distance from camera to
				// edge of skybox - not total size of skybox
		ri.Con_Printf(PRINT_DEVELOPER, "farz now set to %g\n", farz);
	}


	//
	// set up projection matrix
	//
    screenaspect = (float)r_newrefdef.width/r_newrefdef.height;
//	yfov = 2*atan((float)r_newrefdef.height/r_newrefdef.width)*180/M_PI;
	qglMatrixMode(GL_PROJECTION);
    qglLoadIdentity ();

//    MYgluPerspective (r_newrefdef.fov_y,  screenaspect,  4,  4096);
	MYgluPerspective (r_newrefdef.fov_y,  screenaspect, 4, farz); // DMP skybox


	qglCullFace(GL_FRONT);

	qglMatrixMode(GL_MODELVIEW);
    qglLoadIdentity ();

    qglRotatef (-90,  1, 0, 0);	    // put Z going up
    qglRotatef (90,  0, 0, 1);	    // put Z going up
    qglRotatef (-r_newrefdef.viewangles[2],  1, 0, 0);
    qglRotatef (-r_newrefdef.viewangles[0],  0, 1, 0);
    qglRotatef (-r_newrefdef.viewangles[1],  0, 0, 1);
    qglTranslatef (-r_newrefdef.vieworg[0],  -r_newrefdef.vieworg[1],  -r_newrefdef.vieworg[2]);

//	if ( gl_state.camera_separation != 0 && gl_state.stereo_enabled )
//		qglTranslatef ( gl_state.camera_separation, 0, 0 );

	qglGetFloatv (GL_MODELVIEW_MATRIX, r_world_matrix);

	//
	// set drawing parms
	//
	if (gl_cull->value)
		qglEnable(GL_CULL_FACE);
	else
		qglDisable(GL_CULL_FACE);

	qglDisable(GL_BLEND);
	qglDisable(GL_ALPHA_TEST);
	qglEnable(GL_DEPTH_TEST);
}

/*
=============
R_Clear
=============
*/
void R_Clear (void)
{
	if (gl_ztrick->value)
	{
		static int trickframe;

		if (gl_clear->value)
			qglClear (GL_COLOR_BUFFER_BIT);

		trickframe++;
		if (trickframe & 1)
		{
			gldepthmin = 0;
			gldepthmax = 0.49999;
			qglDepthFunc (GL_LEQUAL);
		}
		else
		{
			gldepthmin = 1;
			gldepthmax = 0.5;
			qglDepthFunc (GL_GEQUAL);
		}
	}
	else
	{
		if (gl_clear->value)
			qglClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			qglClear (GL_DEPTH_BUFFER_BIT);
		gldepthmin = 0;
		gldepthmax = 1;
		qglDepthFunc (GL_LEQUAL);
	}

	if (have_stencil)
	{
		qglClearStencil(1);
		qglClear(GL_STENCIL_BUFFER_BIT);
	}



	qglDepthRange (gldepthmin, gldepthmax);

}

void R_Flash( void )
{
	R_PolyBlend ();
}

/*
================
R_RenderView

r_newrefdef must be set before the first call
================
*/
void R_RenderView (refdef_t *fd)
{
	qboolean inWater;

	if (r_norefresh->value)
		return;

	r_newrefdef = *fd;

	if (!r_worldmodel && !( r_newrefdef.rdflags & RDF_NOWORLDMODEL ) )
		ri.Sys_Error (ERR_DROP, "R_RenderView: NULL worldmodel");

	if (r_speeds->value)
	{
		c_brush_polys = 0;
		c_alias_polys = 0;
	}

	R_PushDlights ();

	if (gl_finish->value)
		qglFinish ();

	R_SetupFrame ();

	R_SetFrustum ();

	R_SetupGL ();

	R_MarkLeaves ();	// done here so we know if we're in water

	R_DrawWorld ();

	if ( r_newrefdef.rdflags & RDF_NOWORLDMODEL || !gl_transrendersort->value)
		inWater = false;
	else
		inWater = Mod_PointInLeaf (fd->vieworg, r_worldmodel)->contents;

	if (!(r_newrefdef.rdflags & RDF_NOWORLDMODEL) && gl_transrendersort->value)
	{
		R_SortParticlesOnList(r_newrefdef.num_particles, r_newrefdef.particles);
		R_SortEntitiesOnList(inWater);
	}

	R_DrawEntitiesOnList ((inWater)?false:true, true);

	R_RenderDlights ();

	R_DrawParticles ((inWater)?false:true);

	R_DrawAlphaSurfaces ();

	R_DrawEntitiesOnList ((inWater)?true:false, false);
	
	R_DrawParticles ((inWater)?true:false);

	R_Flash();

	if (r_speeds->value)
	{
		ri.Con_Printf (PRINT_ALL, "%4i wpoly %4i epoly %i tex %i lmaps\n",
			c_brush_polys, 
			c_alias_polys, 
			c_visible_textures, 
			c_visible_lightmaps); 
	}
}


void	R_SetGL2D (void)
{
	// set 2D virtual screen size
	qglViewport (0,0, vid.width, vid.height);
	qglMatrixMode(GL_PROJECTION);
    qglLoadIdentity ();
	qglOrtho  (0, vid.width, vid.height, 0, -99999, 99999);
	qglMatrixMode(GL_MODELVIEW);
    qglLoadIdentity ();
	qglDisable (GL_DEPTH_TEST);
	qglDisable (GL_CULL_FACE);
	qglDisable (GL_BLEND);
	qglEnable (GL_ALPHA_TEST);
	qglColor4f (1,1,1,1);
}

static void GL_DrawColoredStereoLinePair( float r, float g, float b, float y )
{
	qglColor3f( r, g, b );
	qglVertex2f( 0, y );
	qglVertex2f( vid.width, y );
	qglColor3f( 0, 0, 0 );
	qglVertex2f( 0, y + 1 );
	qglVertex2f( vid.width, y + 1 );
}

static void GL_DrawStereoPattern( void )
{
	int i;

	if ( !( gl_config.renderer & GL_RENDERER_INTERGRAPH ) )
		return;

	if ( !gl_state.stereo_enabled )
		return;

	R_SetGL2D();

	qglDrawBuffer( GL_BACK_LEFT );

	for ( i = 0; i < 20; i++ )
	{
		qglBegin( GL_LINES );
			GL_DrawColoredStereoLinePair( 1, 0, 0, 0 );
			GL_DrawColoredStereoLinePair( 1, 0, 0, 2 );
			GL_DrawColoredStereoLinePair( 1, 0, 0, 4 );
			GL_DrawColoredStereoLinePair( 1, 0, 0, 6 );
			GL_DrawColoredStereoLinePair( 0, 1, 0, 8 );
			GL_DrawColoredStereoLinePair( 1, 1, 0, 10);
			GL_DrawColoredStereoLinePair( 1, 1, 0, 12);
			GL_DrawColoredStereoLinePair( 0, 1, 0, 14);
		qglEnd();
		
		GLimp_EndFrame();
	}
}


/*
====================
R_SetLightLevel

====================
*/
void R_SetLightLevel (void)
{
	vec3_t		shadelight;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	// save off light value for server to look at (BIG HACK!)

	R_LightPoint (r_newrefdef.vieworg, shadelight, true);

	// pick the greatest component, which should be the same
	// as the mono value returned by software
	if (shadelight[0] > shadelight[1])
	{
		if (shadelight[0] > shadelight[2])
			r_lightlevel->value = 150*shadelight[0];
		else
			r_lightlevel->value = 150*shadelight[2];
	}
	else
	{
		if (shadelight[1] > shadelight[2])
			r_lightlevel->value = 150*shadelight[1];
		else
			r_lightlevel->value = 150*shadelight[2];
	}

}

/*
@@@@@@@@@@@@@@@@@@@@@
R_RenderFrame

@@@@@@@@@@@@@@@@@@@@@
*/
void R_RenderFrame (refdef_t *fd)
{
	R_RenderView( fd );
	R_SetLightLevel ();
	R_SetGL2D ();
}


void R_Register( void )
{
	r_stainmap = ri.Cvar_Get ("r_stainmap", "1", CVAR_ARCHIVE);

	con_font = ri.Cvar_Get ("con_font", "default", CVAR_ARCHIVE);
	con_font_size = ri.Cvar_Get ("con_font_size", "8", CVAR_ARCHIVE);

	skydistance = ri.Cvar_Get("r_skydistance", "10000", CVAR_ARCHIVE); // DMP - skybox size change
	gl_shellstencil = ri.Cvar_Get ("gl_shellstencil", "1", CVAR_ARCHIVE );
	gl_partscale = ri.Cvar_Get ("gl_partscale", "100", CVAR_ARCHIVE );
	gl_transrendersort = ri.Cvar_Get ("gl_transrendersort", "1", CVAR_ARCHIVE );
	gl_particlelighting = ri.Cvar_Get ("gl_particlelighting", "0.75", CVAR_ARCHIVE );
	gl_particledistance = ri.Cvar_Get ("gl_particledistance", "0", CVAR_ARCHIVE );

	r_lefthand = ri.Cvar_Get( "hand", "0", CVAR_USERINFO | CVAR_ARCHIVE );
	r_norefresh = ri.Cvar_Get ("r_norefresh", "0", 0);
	r_fullbright = ri.Cvar_Get ("r_fullbright", "0", 0);
	r_drawentities = ri.Cvar_Get ("r_drawentities", "1", 0);
	r_drawworld = ri.Cvar_Get ("r_drawworld", "1", 0);
	r_novis = ri.Cvar_Get ("r_novis", "0", 0);
	r_nocull = ri.Cvar_Get ("r_nocull", "0", 0);
	r_lerpmodels = ri.Cvar_Get ("r_lerpmodels", "1", 0);
	r_speeds = ri.Cvar_Get ("r_speeds", "0", 0);

	r_lightlevel = ri.Cvar_Get ("r_lightlevel", "0", 0);

	gl_nosubimage = ri.Cvar_Get( "gl_nosubimage", "0", 0 );
	gl_allow_software = ri.Cvar_Get( "gl_allow_software", "0", 0 );

	gl_modulate = ri.Cvar_Get ("gl_modulate", "1", CVAR_ARCHIVE );
	gl_log = ri.Cvar_Get( "gl_log", "0", 0 );
	gl_bitdepth = ri.Cvar_Get( "gl_bitdepth", "0", 0 );
	gl_mode = ri.Cvar_Get( "gl_mode", "3", CVAR_ARCHIVE );
	gl_lightmap = ri.Cvar_Get ("gl_lightmap", "0", 0);
	gl_shadows = ri.Cvar_Get ("gl_shadows", "0", CVAR_ARCHIVE );
	gl_dynamic = ri.Cvar_Get ("gl_dynamic", "1", 0);
	gl_nobind = ri.Cvar_Get ("gl_nobind", "0", 0);
	gl_round_down = ri.Cvar_Get ("gl_round_down", "1", 0);
	gl_picmip = ri.Cvar_Get ("gl_picmip", "0", 0);
	gl_skymip = ri.Cvar_Get ("gl_skymip", "0", 0);
	gl_showtris = ri.Cvar_Get ("gl_showtris", "0", 0);
	gl_ztrick = ri.Cvar_Get ("gl_ztrick", "0", 0);
	gl_finish = ri.Cvar_Get ("gl_finish", "0", CVAR_ARCHIVE);
	gl_clear = ri.Cvar_Get ("gl_clear", "0", 0);
	gl_cull = ri.Cvar_Get ("gl_cull", "1", 0);
	gl_polyblend = ri.Cvar_Get ("gl_polyblend", "1", 0);
	gl_flashblend = ri.Cvar_Get ("gl_flashblend", "0", 0);
	gl_playermip = ri.Cvar_Get ("gl_playermip", "0", 0);
	gl_monolightmap = ri.Cvar_Get( "gl_monolightmap", "0", 0 );
#ifdef _WIN32
	gl_driver = ri.Cvar_Get( "gl_driver", "opengl32", CVAR_ARCHIVE );
#else
	gl_driver = ri.Cvar_Get( "gl_driver", "libGL.so", CVAR_ARCHIVE );
#endif	
	gl_texturemode = ri.Cvar_Get( "gl_texturemode", "GL_LINEAR_MIPMAP_NEAREST", CVAR_ARCHIVE );
	gl_texturealphamode = ri.Cvar_Get( "gl_texturealphamode", "default", CVAR_ARCHIVE );
	gl_texturesolidmode = ri.Cvar_Get( "gl_texturesolidmode", "default", CVAR_ARCHIVE );
	gl_lockpvs = ri.Cvar_Get( "gl_lockpvs", "0", 0 );

	ri.Cvar_Set( "gl_texturemode", "GL_LINEAR_MIPMAP_NEAREST");

	gl_vertex_arrays = ri.Cvar_Get( "gl_vertex_arrays", "0", CVAR_ARCHIVE );

	r_cellshading = ri.Cvar_Get( "r_cellshading", "0", CVAR_ARCHIVE );
	r_cellshading_width = ri.Cvar_Get( "r_cellshading_width", "4", CVAR_ARCHIVE );

	cl_3dcam = ri.Cvar_Get ("cl_3dcam", "0", CVAR_ARCHIVE);
	cl_3dcam_angle = ri.Cvar_Get ("cl_3dcam_angle", "0", CVAR_ARCHIVE);
	cl_3dcam_dist = ri.Cvar_Get ("cl_3dcam_dist", "50", CVAR_ARCHIVE);
	cl_3dcam_alpha = ri.Cvar_Get ("cl_3dcam_alpha", "1", CVAR_ARCHIVE);
	cl_3dcam_chase = ri.Cvar_Get ("cl_3dcam_chase", "1", CVAR_ARCHIVE);
	cl_3dcam_adjust = ri.Cvar_Get ("cl_3dcam_adjust", "1", CVAR_ARCHIVE);

	gl_ext_swapinterval = ri.Cvar_Get( "gl_ext_swapinterval", "1", CVAR_ARCHIVE );
	gl_ext_multitexture = ri.Cvar_Get( "gl_ext_multitexture", "1", CVAR_ARCHIVE );
	gl_ext_pointparameters = ri.Cvar_Get( "gl_ext_pointparameters", "1", CVAR_ARCHIVE );
	gl_ext_compiled_vertex_array = ri.Cvar_Get( "gl_ext_compiled_vertex_array", "1", CVAR_ARCHIVE );
	gl_ext_palettedtexture = ri.Cvar_Get( "gl_ext_palettedtexture", "1", CVAR_ARCHIVE );

	gl_drawbuffer = ri.Cvar_Get( "gl_drawbuffer", "GL_BACK", 0 );
	gl_swapinterval = ri.Cvar_Get( "gl_swapinterval", "1", CVAR_ARCHIVE );

	gl_saturatelighting = ri.Cvar_Get( "gl_saturatelighting", "0", 0 );

	gl_3dlabs_broken = ri.Cvar_Get( "gl_3dlabs_broken", "1", CVAR_ARCHIVE );

	vid_fullscreen = ri.Cvar_Get( "vid_fullscreen", "0", CVAR_ARCHIVE );
	vid_gamma = ri.Cvar_Get( "vid_gamma", "1.0", CVAR_ARCHIVE );
	vid_ref = ri.Cvar_Get( "vid_ref", "gl", CVAR_ARCHIVE );

	ri.Cmd_AddCommand( "imagelist", GL_ImageList_f );
	ri.Cmd_AddCommand( "screenshot", GL_ScreenShot_f );
	ri.Cmd_AddCommand( "modellist", Mod_Modellist_f );
	ri.Cmd_AddCommand( "gl_strings", GL_Strings_f );
}

/*
==================
R_SetMode
==================
*/
qboolean R_SetMode (void)
{
	rserr_t err;
	qboolean fullscreen;

#ifdef _WIN32
	if ( vid_fullscreen->modified && !gl_config.allow_cds )
	{
		ri.Con_Printf( PRINT_ALL, "R_SetMode() - CDS not allowed with this driver\n" );
		ri.Cvar_SetValue( "vid_fullscreen", !vid_fullscreen->value );
		vid_fullscreen->modified = false;
	}
#endif
	fullscreen = vid_fullscreen->value;

	skydistance->modified = true; // DMP skybox size change
	vid_fullscreen->modified = false;
	gl_mode->modified = false;

	if ( ( err = GLimp_SetMode( &vid.width, &vid.height, gl_mode->value, fullscreen ) ) == rserr_ok )
	{
		gl_state.prev_mode = gl_mode->value;
	}
	else
	{
		if ( err == rserr_invalid_fullscreen )
		{
			ri.Cvar_SetValue( "vid_fullscreen", 0);
			vid_fullscreen->modified = false;
			ri.Con_Printf( PRINT_ALL, "rfx_gl::R_SetMode() - fullscreen unavailable in this mode\n" );
			if ( ( err = GLimp_SetMode( &vid.width, &vid.height, gl_mode->value, false ) ) == rserr_ok )
				return true;
		}
		else if ( err == rserr_invalid_mode )
		{
			ri.Cvar_SetValue( "gl_mode", gl_state.prev_mode );
			gl_mode->modified = false;
			ri.Con_Printf( PRINT_ALL, "rfx_gl::R_SetMode() - invalid mode\n" );
		}

		// try setting it back to something safe
		if ( ( err = GLimp_SetMode( &vid.width, &vid.height, gl_state.prev_mode, false ) ) != rserr_ok )
		{
			ri.Con_Printf( PRINT_ALL, "rfx_gl::R_SetMode() - could not revert to safe mode\n" );
			return false;
		}
	}
	return true;
}

/*
===============
R_Init
===============
*/
int R_Init( void *hinstance, void *hWnd )
{	
	char renderer_buffer[1000];
	char vendor_buffer[1000];
	int		err;
	int		j;
	extern float r_turbsin[256];

	for ( j = 0; j < 256; j++ )
	{
		r_turbsin[j] *= 0.5;
	}

	ri.Con_Printf (PRINT_ALL, "rfx_gl version: "REF_VERSION"\n");

	Draw_GetPalette ();

	R_Register();

	// initialize our QGL dynamic bindings
	if ( !QGL_Init( gl_driver->string ) )
	{
		QGL_Shutdown();
        ri.Con_Printf (PRINT_ALL, "rfx_gl::R_Init() - could not load \"%s\"\n", gl_driver->string );
		return -1;
	}

	// initialize OS-specific parts of OpenGL
	if ( !GLimp_Init( hinstance, hWnd ) )
	{
		QGL_Shutdown();
		return -1;
	}

	// set our "safe" modes
	gl_state.prev_mode = 3;

	// create the window and set up the context
	if ( !R_SetMode () )
	{
		QGL_Shutdown();
        ri.Con_Printf (PRINT_ALL, "rfx_gl::R_Init() - could not R_SetMode()\n" );
		return -1;
	}

	ri.Vid_MenuInit();

	/*
	** get our various GL strings
	*/
	gl_config.vendor_string = qglGetString (GL_VENDOR);
	ri.Con_Printf (PRINT_ALL, "GL_VENDOR: %s\n", gl_config.vendor_string );
	gl_config.renderer_string = qglGetString (GL_RENDERER);
	ri.Con_Printf (PRINT_ALL, "GL_RENDERER: %s\n", gl_config.renderer_string );
	gl_config.version_string = qglGetString (GL_VERSION);
	ri.Con_Printf (PRINT_ALL, "GL_VERSION: %s\n", gl_config.version_string );
	gl_config.extensions_string = qglGetString (GL_EXTENSIONS);
	ri.Con_Printf (PRINT_ALL, "GL_EXTENSIONS: %s\n", gl_config.extensions_string );

	strcpy( renderer_buffer, gl_config.renderer_string );
	//strlwr( renderer_buffer );

	strcpy( vendor_buffer, gl_config.vendor_string );
	//strlwr( vendor_buffer );

	if ( strcasecmp( renderer_buffer, "voodoo" ) )
	{
		if ( !strcasecmp( renderer_buffer, "rush" ) )
			gl_config.renderer = GL_RENDERER_VOODOO;
		else
			gl_config.renderer = GL_RENDERER_VOODOO_RUSH;
	}
	else if ( strcasecmp( vendor_buffer, "sgi" ) )
		gl_config.renderer = GL_RENDERER_SGI;
	else if ( strcasecmp( renderer_buffer, "permedia" ) )
		gl_config.renderer = GL_RENDERER_PERMEDIA2;
	else if ( strcasecmp( renderer_buffer, "glint" ) )
		gl_config.renderer = GL_RENDERER_GLINT_MX;
	else if ( strcasecmp( renderer_buffer, "glzicd" ) )
		gl_config.renderer = GL_RENDERER_REALIZM;
	else if ( strcasecmp( renderer_buffer, "gdi" ) )
		gl_config.renderer = GL_RENDERER_MCD;
	else if ( strcasecmp( renderer_buffer, "pcx2" ) )
		gl_config.renderer = GL_RENDERER_PCX2;
	else if ( strcasecmp( renderer_buffer, "verite" ) )
		gl_config.renderer = GL_RENDERER_RENDITION;
	else
		gl_config.renderer = GL_RENDERER_OTHER;

	if ( !(gl_monolightmap->string[1] == 'F' ||
	       gl_monolightmap->string[1] == 'f'))
	{
		if ( gl_config.renderer == GL_RENDERER_PERMEDIA2 )
		{
			ri.Cvar_Set( "gl_monolightmap", "A" );
			ri.Con_Printf( PRINT_ALL, "...using gl_monolightmap 'a'\n" );
		}
		else if ( gl_config.renderer & GL_RENDERER_POWERVR ) 
		{
			ri.Cvar_Set( "gl_monolightmap", "0" );
		}
		else
		{
			ri.Cvar_Set( "gl_monolightmap", "0" );
		}
	}

	// power vr can't have anything stay in the framebuffer, so
	// the screen needs to redraw the tiled background every frame
	if ( gl_config.renderer & GL_RENDERER_POWERVR ) 
	{
		ri.Cvar_Set( "scr_drawall", "1" );
	}
	else
	{
		ri.Cvar_Set( "scr_drawall", "0" );
	}

#ifdef __linux__
	ri.Cvar_SetValue( "gl_finish", 1 );
#endif

	// MCD has buffering issues
	if ( gl_config.renderer == GL_RENDERER_MCD )
	{
		ri.Cvar_SetValue( "gl_finish", 1 );
	}

	if ( gl_config.renderer & GL_RENDERER_3DLABS )
	{
		if ( gl_3dlabs_broken->value )
			gl_config.allow_cds = false;
		else
			gl_config.allow_cds = true;
	}
	else
	{
		gl_config.allow_cds = true;
	}

	if ( gl_config.allow_cds )
		ri.Con_Printf( PRINT_ALL, "...allowing CDS\n" );
	else
		ri.Con_Printf( PRINT_ALL, "...disabling CDS\n" );

	/*
	** grab extensions
	*/
	if ( strstr( gl_config.extensions_string, "GL_EXT_compiled_vertex_array" ) || 
		 strstr( gl_config.extensions_string, "GL_SGI_compiled_vertex_array" ) )
	{
		ri.Con_Printf( PRINT_ALL, "...enabling GL_EXT_compiled_vertex_array\n" );
		qglLockArraysEXT = ( void * ) qwglGetProcAddress( "glLockArraysEXT" );
		qglUnlockArraysEXT = ( void * ) qwglGetProcAddress( "glUnlockArraysEXT" );
	}
	else
	{
		ri.Con_Printf( PRINT_ALL, "...GL_EXT_compiled_vertex_array not found\n" );
	}

#ifdef _WIN32
	if ( strstr( gl_config.extensions_string, "WGL_EXT_swap_control" ) )
	{
		qwglSwapIntervalEXT = ( BOOL (WINAPI *)(int)) qwglGetProcAddress( "wglSwapIntervalEXT" );
		ri.Con_Printf( PRINT_ALL, "...enabling WGL_EXT_swap_control\n" );
	}
	else
	{
		ri.Con_Printf( PRINT_ALL, "...WGL_EXT_swap_control not found\n" );
	}
#endif

	if ( strstr( gl_config.extensions_string, "GL_EXT_point_parameters" ) )
	{
		if ( gl_ext_pointparameters->value )
		{
			qglPointParameterfEXT = ( void (APIENTRY *)( GLenum, GLfloat ) ) qwglGetProcAddress( "glPointParameterfEXT" );
			qglPointParameterfvEXT = ( void (APIENTRY *)( GLenum, const GLfloat * ) ) qwglGetProcAddress( "glPointParameterfvEXT" );
			ri.Con_Printf( PRINT_ALL, "...using GL_EXT_point_parameters\n" );
		}
		else
		{
			ri.Con_Printf( PRINT_ALL, "...ignoring GL_EXT_point_parameters\n" );
		}
	}
	else
	{
		ri.Con_Printf( PRINT_ALL, "...GL_EXT_point_parameters not found\n" );
	}

	if ( strstr( gl_config.extensions_string, "GL_ARB_multitexture" ) )
	{
		if ( gl_ext_multitexture->value )
		{
			ri.Con_Printf( PRINT_ALL, "...using GL_ARB_multitexture\n" );
			qglMTexCoord2fSGIS = ( void * ) qwglGetProcAddress( "glMultiTexCoord2fARB" );
			qglActiveTextureARB = ( void * ) qwglGetProcAddress( "glActiveTextureARB" );
			qglClientActiveTextureARB = ( void * ) qwglGetProcAddress( "glClientActiveTextureARB" );
			QGL_TEXTURE0 = GL_TEXTURE0_ARB;
			QGL_TEXTURE1 = GL_TEXTURE1_ARB;
		}
		else
		{
			ri.Con_Printf( PRINT_ALL, "...ignoring GL_ARB_multitexture\n" );
		}
	}
	else
	{
		ri.Con_Printf( PRINT_ALL, "...GL_ARB_multitexture not found\n" );
	}

	if ( strstr( gl_config.extensions_string, "GL_SGIS_multitexture" ) )
	{
		if ( qglActiveTextureARB )
		{
			ri.Con_Printf( PRINT_ALL, "...GL_SGIS_multitexture deprecated in favor of ARB_multitexture\n" );
		}
		else if ( gl_ext_multitexture->value )
		{
			ri.Con_Printf( PRINT_ALL, "...using GL_SGIS_multitexture\n" );
			qglMTexCoord2fSGIS = ( void * ) qwglGetProcAddress( "glMTexCoord2fSGIS" );
			qglSelectTextureSGIS = ( void * ) qwglGetProcAddress( "glSelectTextureSGIS" );
			QGL_TEXTURE0 = GL_TEXTURE0_SGIS;
			QGL_TEXTURE1 = GL_TEXTURE1_SGIS;
		}
		else
		{
			ri.Con_Printf( PRINT_ALL, "...ignoring GL_SGIS_multitexture\n" );
		}
	}
	else
	{
		ri.Con_Printf( PRINT_ALL, "...GL_SGIS_multitexture not found\n" );
	}

	GL_SetDefaultState();
	
	/*
	** draw our stereo patterns
	*/
#if 0 // commented out until H3D pays us the money they owe us
	GL_DrawStereoPattern();
#endif

	GL_InitImages ();
	Mod_Init ();
	R_InitParticleTexture ();
	Draw_InitLocal ();
	
	err = qglGetError();
	if ( err != GL_NO_ERROR )
		ri.Con_Printf (PRINT_ALL, "glGetError() = 0x%x\n", err);
	return 0;
}

/*
===============
R_Shutdown
===============
*/
void R_Shutdown (void)
{	
	ri.Cmd_RemoveCommand ("modellist");
	ri.Cmd_RemoveCommand ("screenshot");
	ri.Cmd_RemoveCommand ("imagelist");
	ri.Cmd_RemoveCommand ("gl_strings");

	Mod_FreeAll ();

	GL_ShutdownImages ();

	/*
	** shut down OS specific OpenGL stuff like contexts, etc.
	*/
	GLimp_Shutdown();

	/*
	** shutdown our QGL subsystem
	*/
	QGL_Shutdown();
}



/*
@@@@@@@@@@@@@@@@@@@@@
R_BeginFrame
@@@@@@@@@@@@@@@@@@@@@
*/
void R_BeginFrame( float camera_separation )
{

	gl_state.camera_separation = camera_separation;

	/*
	** change modes if necessary
	*/
	if ( gl_mode->modified || vid_fullscreen->modified )
	{	// FIXME: only restart if CDS is required
		cvar_t	*ref;

		ref = ri.Cvar_Get ("vid_ref", "gl", 0);
		ref->modified = true;
	}

	if ( gl_log->modified )
	{
		GLimp_EnableLogging( gl_log->value );
		gl_log->modified = false;
	}

	if ( gl_log->value )
	{
		GLimp_LogNewFrame();
	}

	/*
	** update 3Dfx gamma -- it is expected that a user will do a vid_restart
	** after tweaking this value
	*/
	if ( vid_gamma->modified )
	{
		vid_gamma->modified = false;

		if ( gl_config.renderer & ( GL_RENDERER_VOODOO ) )
		{
			char envbuffer[1024];
			float g;

			g = 2.00 * ( 0.8 - ( vid_gamma->value - 0.5 ) ) + 1.0F;
			Com_sprintf( envbuffer, sizeof(envbuffer), "SSTV2_GAMMA=%f", g );
			putenv( envbuffer );
			Com_sprintf( envbuffer, sizeof(envbuffer), "SST_GAMMA=%f", g );
			putenv( envbuffer );
		}
	}

	if (gl_particlelighting->modified)
	{	
		gl_particlelighting->modified = false;
		if (gl_particlelighting->value<0)
			gl_particlelighting->value=0;
		if (gl_particlelighting->value>1)
			gl_particlelighting->value=1;
	}

	GLimp_BeginFrame( camera_separation );

	/*
	** go into 2D mode
	*/
	qglViewport (0,0, vid.width, vid.height);
	qglMatrixMode(GL_PROJECTION);
    qglLoadIdentity ();
	qglOrtho  (0, vid.width, vid.height, 0, -99999, 99999);
	qglMatrixMode(GL_MODELVIEW);
    qglLoadIdentity ();
	qglDisable (GL_DEPTH_TEST);
	qglDisable (GL_CULL_FACE);
	qglDisable (GL_BLEND);
	qglEnable (GL_ALPHA_TEST);
	qglColor4f (1,1,1,1);

	/*
	** draw buffer stuff
	*/
	if ( gl_drawbuffer->modified )
	{
		gl_drawbuffer->modified = false;

		if ( gl_state.camera_separation == 0 || !gl_state.stereo_enabled )
		{
			if ( Q_stricmp( gl_drawbuffer->string, "GL_FRONT" ) == 0 )
				qglDrawBuffer( GL_FRONT );
			else
				qglDrawBuffer( GL_BACK );
		}
	}

	/*
	** texturemode stuff
	*/
	if ( gl_texturemode->modified )
	{
		GL_TextureMode( gl_texturemode->string );
		gl_texturemode->modified = false;
	}

	if ( gl_texturealphamode->modified )
	{
		GL_TextureAlphaMode( gl_texturealphamode->string );
		gl_texturealphamode->modified = false;
	}

	if ( gl_texturesolidmode->modified )
	{
		GL_TextureSolidMode( gl_texturesolidmode->string );
		gl_texturesolidmode->modified = false;
	}

	/*
	** swapinterval stuff
	*/
	GL_UpdateSwapInterval();

	//
	// clear screen if desired
	//
	R_Clear ();
}

/*
=============
R_SetPalette
=============
*/
unsigned r_rawpalette[256];

void R_SetPalette ( const unsigned char *palette)
{
	int		i;

	byte *rp = ( byte * ) r_rawpalette;

	if ( palette )
	{
		for ( i = 0; i < 256; i++ )
		{
			rp[i*4+0] = palette[i*3+0];
			rp[i*4+1] = palette[i*3+1];
			rp[i*4+2] = palette[i*3+2];
			rp[i*4+3] = 0xff;
		}
	}
	else
	{
		for ( i = 0; i < 256; i++ )
		{
			rp[i*4+0] = d_8to24table[i] & 0xff;
			rp[i*4+1] = ( d_8to24table[i] >> 8 ) & 0xff;
			rp[i*4+2] = ( d_8to24table[i] >> 16 ) & 0xff;
			rp[i*4+3] = 0xff;
		}
	}
//	GL_SetTexturePalette( r_rawpalette );

	qglClearColor (0,0,0,0);
	qglClear (GL_COLOR_BUFFER_BIT);
	qglClearColor (1,0, 0.5 , 0.5);
	
}

/*
** R_DrawBeam
*/
void R_RenderBeam( vec3_t start, vec3_t end, float size, float red, float green, float blue, float alpha );

void R_DrawBeam( entity_t *e )
{

	R_RenderBeam( e->origin, e->oldorigin, e->frame, 
		( d_8to24table[e->skinnum & 0xFF] ) & 0xFF,
		( d_8to24table[e->skinnum & 0xFF] >> 8 ) & 0xFF,
		( d_8to24table[e->skinnum & 0xFF] >> 16 ) & 0xFF,
		e->alpha*254 );

	//OLD BEAM RENDERING CODE ...
	/*
#define NUM_BEAM_SEGS 12

	int	i;
	float r, g, b;

	vec3_t perpvec;
	vec3_t direction, normalized_direction;
	vec3_t	start_points[NUM_BEAM_SEGS], end_points[NUM_BEAM_SEGS];
	vec3_t oldorigin, origin;

	oldorigin[0] = e->oldorigin[0];
	oldorigin[1] = e->oldorigin[1];
	oldorigin[2] = e->oldorigin[2];

	origin[0] = e->origin[0];
	origin[1] = e->origin[1];
	origin[2] = e->origin[2];

	normalized_direction[0] = direction[0] = oldorigin[0] - origin[0];
	normalized_direction[1] = direction[1] = oldorigin[1] - origin[1];
	normalized_direction[2] = direction[2] = oldorigin[2] - origin[2];

	if ( VectorNormalize( normalized_direction ) == 0 )
		return;

	PerpendicularVector( perpvec, normalized_direction );
	VectorScale( perpvec, e->frame / 2, perpvec );

	for ( i = 0; i < NUM_BEAM_SEGS; i++ )
	{
		RotatePointAroundVector( start_points[i], normalized_direction, perpvec, (360.0/NUM_BEAM_SEGS)*i );
		VectorAdd( start_points[i], origin, start_points[i] );
		VectorAdd( start_points[i], direction, end_points[i] );
	}

	qglDisable( GL_TEXTURE_2D );
	qglEnable( GL_BLEND );
	qglDepthMask( GL_FALSE );

	r = ( d_8to24table[e->skinnum & 0xFF] ) & 0xFF;
	g = ( d_8to24table[e->skinnum & 0xFF] >> 8 ) & 0xFF;
	b = ( d_8to24table[e->skinnum & 0xFF] >> 16 ) & 0xFF;

	r *= DIV255;
	g *= DIV255;
	b *= DIV255;

	qglColor4f( r, g, b, e->alpha );

	qglBegin( GL_TRIANGLE_STRIP );
	for ( i = 0; i < NUM_BEAM_SEGS; i++ )
	{
		qglVertex3fv( start_points[i] );
		qglVertex3fv( end_points[i] );
		qglVertex3fv( start_points[(i+1)%NUM_BEAM_SEGS] );
		qglVertex3fv( end_points[(i+1)%NUM_BEAM_SEGS] );
	}
	qglEnd();

	qglEnable( GL_TEXTURE_2D );
	qglDisable( GL_BLEND );
	qglDepthMask( GL_TRUE );
	*/
}

void R_RenderBeam( vec3_t start, vec3_t end, float size, float red, float green, float blue, float alpha )
{
	float	len;	
	vec3_t coord[4], ang_up, ang_right, vdelta;

	GL_TexEnv( GL_MODULATE );
	qglDepthMask   (false);
	qglBlendFunc   (GL_SRC_ALPHA, GL_ONE);
	qglEnable( GL_BLEND );
	qglShadeModel (GL_SMOOTH);
	GL_Bind(r_particlebeam->texnum);
	qglColor4ub(red, green, blue, alpha);

	VectorSubtract(start, end, ang_up);
	len = VectorNormalize(ang_up);

	VectorSubtract(r_newrefdef.vieworg, start, vdelta);
	CrossProduct(ang_up, vdelta, ang_right);
	if(!VectorCompare(ang_right, vec3_origin))
		VectorNormalize(ang_right);

	VectorScale (ang_right, size*3.0, ang_right);

	VectorAdd(start, ang_right, coord[0]);
	VectorAdd(end, ang_right, coord[1]);
	VectorSubtract(end, ang_right, coord[2]);
	VectorSubtract(start, ang_right, coord[3]);

	qglPushMatrix();
	{
		qglBegin (GL_QUADS);
		{
			qglTexCoord2f (0, 1);
			qglVertex3fv  (coord[0]);
			qglTexCoord2f (0, 0);
			qglVertex3fv  (coord[1]);
			qglTexCoord2f (1, 0); 
			qglVertex3fv  (coord[2]);
			qglTexCoord2f (1, 1);
			qglVertex3fv  (coord[3]);
		}
		qglEnd ();
	}
	qglPopMatrix ();
	
	qglBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GL_TexEnv( GL_MODULATE );
	qglDepthMask (true);
	qglDisable ( GL_BLEND );
	qglColor4f   (1,1,1,1);
}

//===================================================================


void	R_BeginRegistration (char *map);
struct model_s	*R_RegisterModel (char *name);
struct image_s	*R_RegisterSkin (char *name);
void R_SetSky (char *name, float rotate, vec3_t axis);
void	R_EndRegistration (void);
void	R_AddStain (vec3_t org, float intensity, float r, float g, float b);

void	R_RenderFrame (refdef_t *fd);

struct image_s	*Draw_FindPic (char *name);

void	Draw_ScaledPic (int x, int y, float scale, float alpha, char *pic);
void	Draw_Pic (int x, int y, char *name);
void	Draw_Char (int x, int y, int c, int alpha);
void	Draw_TileClear (int x, int y, int w, int h, char *name);
void	Draw_Fill (int x, int y, int w, int h, int c);
void	Draw_FadeScreen (void);

float	CharMapScale (void);
void	SetParticlePicture (int num, char *name);

/*
@@@@@@@@@@@@@@@@@@@@@
GetRefAPI

@@@@@@@@@@@@@@@@@@@@@
*/
refexport_t GetRefAPI (refimport_t rimp )
{
	refexport_t	re;

	ri = rimp;

	re.api_version = API_VERSION;

	re.BeginRegistration = R_BeginRegistration;
	re.RegisterModel = R_RegisterModel;
	re.RegisterSkin = R_RegisterSkin;
	re.RegisterPic = Draw_FindPic;
	re.SetSky = R_SetSky;
	re.EndRegistration = R_EndRegistration;

	re.AddStain = R_AddStain;

	re.RenderFrame = R_RenderFrame;

	re.SetParticlePicture = SetParticlePicture;

	re.DrawGetPicSize = Draw_GetPicSize;
	re.DrawScaledPic = Draw_ScaledPic;
	re.DrawPic = Draw_Pic;
	re.DrawStretchPic = Draw_StretchPic;
	re.DrawChar = Draw_Char;
	re.DrawTileClear = Draw_TileClear;
	re.DrawFill = Draw_Fill;
	re.DrawFadeScreen= Draw_FadeScreen;
	re.CharMap_Scale = CharMapScale;

	re.DrawStretchRaw = Draw_StretchRaw;

	re.Init = R_Init;
	re.Shutdown = R_Shutdown;

	re.CinematicSetPalette = R_SetPalette;
	re.BeginFrame = R_BeginFrame;
	re.EndFrame = GLimp_EndFrame;

	re.AppActivate = GLimp_AppActivate;

	Swap_Init ();

	return re;
}


#ifndef REF_HARD_LINKED
// this is only here so the functions in q_shared.c and q_shwin.c can link
void Sys_Error (char *error, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	vsprintf (text, error, argptr);
	va_end (argptr);

	ri.Sys_Error (ERR_FATAL, "%s", text);
}

void Com_Printf (char *fmt, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	vsprintf (text, fmt, argptr);
	va_end (argptr);

	ri.Con_Printf (PRINT_ALL, "%s", text);
}

#endif
