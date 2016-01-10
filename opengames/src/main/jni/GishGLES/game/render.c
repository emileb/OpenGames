/*
Copyright (C) 2005, 2010 - Cryptic Sea

This file is part of Gish.

Gish is free software; you can redistribute it and/or
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

#include "../config.h"

#include "../video/opengl.h"

#include <math.h>

#include "../game/render.h"
#include "../game/block.h"
#include "../game/editor.h"
#include "../game/game.h"
#include "../game/level.h"
#include "../game/lighting.h"
#include "../game/gameobject.h"
#include "../game/physics.h"
#include "../game/prerender.h"
#include "../game/sprite.h"
#include "../audio/audio.h"
#include "../math/vector.h"
#include "../physics/bond.h"
#include "../physics/particle.h"
#include "../sdl/event.h"
#include "../video/text.h"
#include "../video/texture.h"

_frame frame;

void renderlevelback(void)
  {
  int count,count2;
  int lightcount;
  int blocknum;
  int lightrange;
  float vec[3];
  //float texcoord[2];

  updateogg();

  for (count=view.position[1]-view.zoomy;count<=view.position[1]+view.zoomy;count++)
  if (count>=0 && count<256)
  for (count2=view.position[0]-view.zoomx;count2<=view.position[0]+view.zoomx;count2++)
  if (count2>=0 && count2<256)
    {
    blocknum=level.backgrid[count][count2];

    if (block[blocknum].animation!=0)
    if (block[blocknum].animationspeed!=0)
      blocknum+=(game.framenum/block[blocknum].animationspeed)%block[blocknum].animation;

    if (!game.godmode)
    if (!editor.active || !editor.showgrid)
    if (level.grid[count][count2]!=0 && level.gridmod[count][count2]==0)
    if (!texture[level.grid[count][count2]].isalpha)
      blocknum=0;

    if (!game.godmode)
    if (!editor.active || !editor.showgrid)
    if (level.foregrid[count][count2]!=0)
    if (!texture[level.foregrid[count][count2]].isalpha)
      blocknum=0;

    if (blocknum!=0)
      {
      glBindTexture(GL_TEXTURE_2D,texture[blocknum].glname);
#if defined(USE_GLES)
    glColor4f(level.ambient[0][0],level.ambient[0][1],level.ambient[0][2],1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        GLfloat quad[] = {  (float)count2,(float)count+1.0f,
                            (float)count2+1.0f,(float)count+1.0f,
                            (float)count2+1.0f,(float)count,
                            (float)count2,(float)count};
        GLfloat tx[] = {    0,0,
                            1,0,
                            1,1,
                            0,1};
        glVertexPointer(2, GL_FLOAT, 0, quad);
        glTexCoordPointer(2, GL_FLOAT, 0, tx);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
      glBegin(GL_QUADS);
  
      glColor3fv(level.ambient[0]);
  
      vec[0]=(float)count2;
      vec[1]=(float)count+1.0f;
      glTexCoord2f(0.0f,0.0f);
      glVertex3f(vec[0],vec[1],0.0f);
  
      vec[0]=(float)count2+1.0f;
      vec[1]=(float)count+1.0f;
      glTexCoord2f(1.0f,0.0f);
      glVertex3f(vec[0],vec[1],0.0f);
  
      vec[0]=(float)count2+1.0f;
      vec[1]=(float)count;
      glTexCoord2f(1.0f,1.0f);
      glVertex3f(vec[0],vec[1],0.0f);
  
      vec[0]=(float)count2;
      vec[1]=(float)count;
      glTexCoord2f(0.0f,1.0f);
      glVertex3f(vec[0],vec[1],0.0f);
  
      glEnd();
#endif
      }
    }

  glBlendFunc(GL_SRC_ALPHA,GL_ONE);

  glEnable(GL_STENCIL_TEST);
  glActiveTextureARB(GL_TEXTURE1_ARB);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[332].glname);
  glDisable(GL_TEXTURE_2D);
  glActiveTextureARB(GL_TEXTURE0_ARB);

  for (lightcount=0;lightcount<frame.numoflights;lightcount++)
    {
    glStencilMask((1<<lightcount));
    glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
    glStencilFunc(GL_NOTEQUAL,(1<<lightcount),(1<<lightcount));

    for (count=view.position[1]-view.zoomy;count<=view.position[1]+view.zoomy;count++)
    if (count>=0 && count<256)
    for (count2=view.position[0]-view.zoomx;count2<=view.position[0]+view.zoomx;count2++)
    if (count2>=0 && count2<256)
      {
      blocknum=level.backgrid[count][count2];

      if (block[blocknum].animation!=0)
      if (block[blocknum].animationspeed!=0)
        blocknum+=(game.framenum/block[blocknum].animationspeed)%block[blocknum].animation;

      if (!editor.active || !editor.showgrid)
      if (level.grid[count][count2]!=0 && level.gridmod[count][count2]==0)
      if (!texture[level.grid[count][count2]].isalpha)
        blocknum=0;

      if (!editor.active || !editor.showgrid)
      if (level.foregrid[count][count2]!=0)
      if (!texture[level.foregrid[count][count2]].isalpha)
        blocknum=0;

      lightrange=frame.light[lightcount].intensity*0.5f;
      if ((count2-frame.light[lightcount].position[0])>lightrange+1)
        blocknum=0;
      if ((count-frame.light[lightcount].position[1])>lightrange+1)
        blocknum=0;

      if (blocknum!=0)
        {
        glBindTexture(GL_TEXTURE_2D,texture[blocknum].glname);
    
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glEnable(GL_TEXTURE_2D);
    
#if defined(USE_GLES)
        glColor4f(frame.light[lightcount].color[0],frame.light[lightcount].color[1],frame.light[lightcount].color[2],1.0f);
        GLfloat quad[] = {  (float)count2,(float)count+1.0f,
                            (float)count2+1.0f,(float)count+1.0f,
                            (float)count2+1.0f,(float)count,
                            (float)count2,(float)count};
        glVertexPointer(2, GL_FLOAT, 0,quad);
        GLfloat tx[] = {    0,0,
                            1,0,
                            1,1,
                            0,1};

        GLfloat lightTx[]={ (quad[0]-frame.light[lightcount].position[0])/frame.light[lightcount].intensity+0.5f,
                                (quad[1]-frame.light[lightcount].position[1])/frame.light[lightcount].intensity+0.5f,
                            (quad[2]-frame.light[lightcount].position[0])/frame.light[lightcount].intensity+0.5f,
                                (quad[3]-frame.light[lightcount].position[1])/frame.light[lightcount].intensity+0.5f,
                            (quad[4]-frame.light[lightcount].position[0])/frame.light[lightcount].intensity+0.5f,
                                (quad[5]-frame.light[lightcount].position[1])/frame.light[lightcount].intensity+0.5f,
                            (quad[6]-frame.light[lightcount].position[0])/frame.light[lightcount].intensity+0.5f,
                                (quad[7]-frame.light[lightcount].position[1])/frame.light[lightcount].intensity+0.5f};
    glEnable(GL_TEXTURE_2D);
    glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, tx);

    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, lightTx);

    glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glClientActiveTexture(GL_TEXTURE0);
#else
        glBegin(GL_QUADS);
    
        glColor3fv(frame.light[lightcount].color);
    
        vec[0]=(float)count2;
        vec[1]=(float)count+1.0f;
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0.0f,0.0f);
        setuplighttexcoord(lightcount,vec);
        glVertex3f(vec[0],vec[1],0.0f);
    
        vec[0]=(float)count2+1.0f;
        vec[1]=(float)count+1.0f;
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1.0f,0.0f);
        setuplighttexcoord(lightcount,vec);
        glVertex3f(vec[0],vec[1],0.0f);
    
        vec[0]=(float)count2+1.0f;
        vec[1]=(float)count;
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1.0f,1.0f);
        setuplighttexcoord(lightcount,vec);
        glVertex3f(vec[0],vec[1],0.0f);
    
        vec[0]=(float)count2;
        vec[1]=(float)count;
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0.0f,1.0f);
        setuplighttexcoord(lightcount,vec);
        glVertex3f(vec[0],vec[1],0.0f);
    
        glEnd();
#endif
        glDisable(GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        }
      }
    }

  glDisable(GL_STENCIL_TEST);

  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  }

void renderlevel(void)
  {
  int count,count2,count3;
  int lightcount;
  int blocknum;
  int lightrange;
  float vec[3],vec2[3],vec3[3];
  float normal[3];
  float scale;

  updateogg();

  for (count=view.position[1]-view.zoomy;count<=view.position[1]+view.zoomy;count++)
  if (count>=0 && count<256)
  for (count2=view.position[0]-view.zoomx;count2<=view.position[0]+view.zoomx;count2++)
  if (count2>=0 && count2<256)
    {
    blocknum=level.grid[count][count2];

    if (block[blocknum].animation!=0)
    if (block[blocknum].animationspeed!=0)
      blocknum+=(game.framenum/block[blocknum].animationspeed)%block[blocknum].animation;

    if (level.gridmod[count][count2]==1)
      blocknum=0;

    if (blocknum>=240 && blocknum<248)
    if (level.gridmod[count][count2]==2)
      blocknum+=8;

    if (!editor.active || !editor.showgrid)
    if (level.foregrid[count][count2]!=0)
    if (!texture[level.foregrid[count][count2]].isalpha)
      blocknum=0;

    if (blocknum!=0)
      {
      glBindTexture(GL_TEXTURE_2D,texture[blocknum].glname);

#if defined(USE_GLES)
    glColor4f(level.ambient[1][0],level.ambient[1][1],level.ambient[1][2],1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        GLfloat quad[] = {  (float)count2,(float)count+1.0f,
                            (float)count2+1.0f,(float)count+1.0f,
                            (float)count2+1.0f,(float)count,
                            (float)count2,(float)count};
        GLfloat tx[] = {    0,0,
                            1,0,
                            1,1,
                            0,1};
        glVertexPointer(2, GL_FLOAT, 0, quad);
        glTexCoordPointer(2, GL_FLOAT, 0, tx);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
      glBegin(GL_QUADS);

      glColor3fv(level.ambient[1]);

      vec[0]=(float)count2;
      vec[1]=(float)count+1.0f;
      glTexCoord2f(0.0f,0.0f);
      glVertex3f(vec[0],vec[1],0.0f);
  
      vec[0]=(float)count2+1.0f;
      vec[1]=(float)count+1.0f;
      glTexCoord2f(1.0f,0.0f);
      glVertex3f(vec[0],vec[1],0.0f);
  
      vec[0]=(float)count2+1.0f;
      vec[1]=(float)count;
      glTexCoord2f(1.0f,1.0f);
      glVertex3f(vec[0],vec[1],0.0f);
  
      vec[0]=(float)count2;
      vec[1]=(float)count;
      glTexCoord2f(0.0f,1.0f);
      glVertex3f(vec[0],vec[1],0.0f);
  
      glEnd();
#endif
      /*
      if (block[blocknum].friction<0.1f)
        {
        glDisable(GL_TEXTURE_2D);

        for (count3=0;count3<block[blocknum].numoflines;count3++)
        if (((level.gridflags[count][count2]>>count3)&1)==0)
          {
          normal[0]=-(block[blocknum].line[count3][3]-block[blocknum].line[count3][1]);
          normal[1]=(block[blocknum].line[count3][2]-block[blocknum].line[count3][0]);
          normal[2]=0.0f;
          normalizevector(normal,normal);

          glBegin(GL_QUADS);

          glColor4f(0.25f,0.25f,0.25f,1.0f);

          vec[0]=(float)count2+block[blocknum].line[count3][0];
          vec[1]=(float)count+block[blocknum].line[count3][1];
          vec[2]=0.0f;
          glVertex3fv(vec);

          vec[0]=(float)count2+block[blocknum].line[count3][2];
          vec[1]=(float)count+block[blocknum].line[count3][3];
          vec[2]=0.0f;
          glVertex3fv(vec);

          glColor4f(0.25f,0.25f,0.25f,0.0f);

          vec[0]=(float)count2+block[blocknum].line[count3][2]-normal[0]*0.125f*0.5f;
          vec[1]=(float)count+block[blocknum].line[count3][3]-normal[1]*0.125f*0.5f;
          vec[2]=0.0f;
          glVertex3fv(vec);
  
          vec[0]=(float)count2+block[blocknum].line[count3][0]-normal[0]*0.125f*0.5f;
          vec[1]=(float)count+block[blocknum].line[count3][1]-normal[1]*0.125f*0.5f;
          vec[2]=0.0f;
          glVertex3fv(vec);

          glEnd();
          }

        glEnable(GL_TEXTURE_2D);
        }
      */
      }
    }

  glBlendFunc(GL_ONE,GL_ONE);

  glEnable(GL_STENCIL_TEST);

  for (lightcount=0;lightcount<frame.numoflights;lightcount++)
    {
    glStencilMask((1<<lightcount));
    glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
    glStencilFunc(GL_NOTEQUAL,(1<<lightcount),(1<<lightcount));

    for (count=view.position[1]-view.zoomy;count<=view.position[1]+view.zoomy;count++)
    if (count>=0 && count<256)
    for (count2=view.position[0]-view.zoomx;count2<=view.position[0]+view.zoomx;count2++)
    if (count2>=0 && count2<256)
      {
      blocknum=level.grid[count][count2];

      if (block[blocknum].animation!=0)
      if (block[blocknum].animationspeed!=0)
        blocknum+=(game.framenum/block[blocknum].animationspeed)%block[blocknum].animation;

      if (level.gridmod[count][count2]==1)
        blocknum=0;

      if (blocknum>=240 && blocknum<248)
      if (level.gridmod[count][count2]==2)
        blocknum+=8;

      if (!editor.active || !editor.showgrid)
      if (level.foregrid[count][count2]!=0)
      if (!texture[level.foregrid[count][count2]].isalpha)
        blocknum=0;

      lightrange=frame.light[lightcount].intensity*0.5f;

      if ((count2-frame.light[lightcount].position[0])>lightrange+1)
        blocknum=0;
      if ((count-frame.light[lightcount].position[1])>lightrange+1)
        blocknum=0;

      if (blocknum!=0)
        {
        glBindTexture(GL_TEXTURE_2D,texture[blocknum].glname);
  
        for (count3=0;count3<block[blocknum].numoflines;count3++)
        if (((level.gridflags[count][count2]>>count3)&1)==0)
          {
          normal[0]=-(block[blocknum].line[count3][3]-block[blocknum].line[count3][1]);
          normal[1]=(block[blocknum].line[count3][2]-block[blocknum].line[count3][0]);
          normal[2]=0.0f;
          normalizevector(normal,normal);
  
#if defined(USE_GLES)
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
        GLfloat color[] = { 0,0,0,1,
                            0,0,0,1,
                            0,0,0,1,
                            0,0,0,1 };
        GLfloat quad[] = {  (float)count2+block[blocknum].line[count3][0],(float)count+block[blocknum].line[count3][1],
                            (float)count2+block[blocknum].line[count3][2],(float)count+block[blocknum].line[count3][3],
                            (float)count2+block[blocknum].line[count3][2]-normal[0]*0.125f,(float)count+block[blocknum].line[count3][3]-normal[1]*0.125f,
                            (float)count2+block[blocknum].line[count3][0]-normal[0]*0.125f,(float)count+block[blocknum].line[count3][1]-normal[1]*0.125f };
        GLfloat tx[] = {    block[blocknum].line[count3][0],1.0f-block[blocknum].line[count3][1],
                            block[blocknum].line[count3][2],1.0f-block[blocknum].line[count3][3],
                            (block[blocknum].line[count3][2]-normal[0]*0.125f),1.0f-(block[blocknum].line[count3][3]-normal[1]*0.125f),
                            (block[blocknum].line[count3][0]-normal[0]*0.125f),1.0f-(block[blocknum].line[count3][1]-normal[1]*0.125f)};
#else
          glBegin(GL_QUADS);
#endif
  
          vec[0]=(float)count2+block[blocknum].line[count3][0];
          vec[1]=(float)count+block[blocknum].line[count3][1];
          vec[2]=0.0f;
          subtractvectors(vec2,frame.light[lightcount].position,vec);
          normalizevector(vec3,vec2);
          scale=frame.light[lightcount].intensity*dotproduct(vec3,normal)/(vec2[0]*vec2[0]+vec2[1]*vec2[1]);
          scalevector(vec2,frame.light[lightcount].color,scale);
#if defined(USE_GLES)
        color[0] = vec2[0]; color[1] = vec2[1]; color[2] = vec2[2];
#else
          glColor3fv(vec2);
          glTexCoord2f(block[blocknum].line[count3][0],1.0f-block[blocknum].line[count3][1]);
          glVertex3f(vec[0],vec[1],0.0f);
#endif
          vec[0]=(float)count2+block[blocknum].line[count3][2];
          vec[1]=(float)count+block[blocknum].line[count3][3];
          vec[2]=0.0f;
          subtractvectors(vec2,frame.light[lightcount].position,vec);
          normalizevector(vec3,vec2);
          scale=frame.light[lightcount].intensity*dotproduct(vec3,normal)/(vec2[0]*vec2[0]+vec2[1]*vec2[1]);
          scalevector(vec2,frame.light[lightcount].color,scale);
#if defined(USE_GLES)
        color[4] = vec2[0]; color[5] = vec2[1]; color[6] = vec2[2];
#else
          glColor3fv(vec2);
          glTexCoord2f(block[blocknum].line[count3][2],1.0f-block[blocknum].line[count3][3]);
          glVertex3f(vec[0],vec[1],0.0f);
#endif
  
#if !defined(USE_GLES)
          vec[0]=(float)count2+block[blocknum].line[count3][2]-normal[0]*0.125f;
          vec[1]=(float)count+block[blocknum].line[count3][3]-normal[1]*0.125f;
          glColor3f(0.0f,0.0f,0.0f);
          glTexCoord2f((block[blocknum].line[count3][2]-normal[0]*0.125f),1.0f-(block[blocknum].line[count3][3]-normal[1]*0.125f));
          glVertex3f(vec[0],vec[1],0.0f);
  
          vec[0]=(float)count2+block[blocknum].line[count3][0]-normal[0]*0.125f;
          vec[1]=(float)count+block[blocknum].line[count3][1]-normal[1]*0.125f;
          glColor3f(0.0f,0.0f,0.0f);
          glTexCoord2f((block[blocknum].line[count3][0]-normal[0]*0.125f),1.0f-(block[blocknum].line[count3][1]-normal[1]*0.125f));
          glVertex3f(vec[0],vec[1],0.0f);
#endif
  
#if defined(USE_GLES)
        glVertexPointer(2, GL_FLOAT, 0, quad);
        glTexCoordPointer(2, GL_FLOAT, 0, tx);
        glColorPointer(4, GL_FLOAT, 0, color);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
          glEnd();
#endif
          }
        }
      }
    }

  glDisable(GL_STENCIL_TEST);

  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  }

void renderlevelfore(void)
  {
  int count,count2;
  int blocknum;
  float vec[3];

  updateogg();

  for (count=view.position[1]-view.zoomy;count<=view.position[1]+view.zoomy;count++)
  if (count>=0 && count<256)
  for (count2=view.position[0]-view.zoomx;count2<=view.position[0]+view.zoomx;count2++)
  if (count2>=0 && count2<256)
    {
    blocknum=level.foregrid[count][count2];

    if (block[blocknum].animation!=0)
    if (block[blocknum].animationspeed!=0)
      blocknum+=(game.framenum/block[blocknum].animationspeed)%block[blocknum].animation;

    if (blocknum!=0)
      {
      glBindTexture(GL_TEXTURE_2D,texture[blocknum].glname);

#if defined(USE_GLES)
    glColor4f(level.ambient[2][0],level.ambient[2][1],level.ambient[2][2],1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        GLfloat quad[] = {  (float)count2,(float)count+1.0f,
                            (float)count2+1.0f,(float)count+1.0f,
                            (float)count2+1.0f,(float)count,
                            (float)count2,(float)count};
        GLfloat tx[] = {    0,0,
                            1,0,
                            1,1,
                            0,1};
        glVertexPointer(2, GL_FLOAT, 0, quad);
        glTexCoordPointer(2, GL_FLOAT, 0, tx);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
      glBegin(GL_QUADS);

      glColor3fv(level.ambient[2]);

      vec[0]=(float)count2;
      vec[1]=(float)count+1.0f;
      glTexCoord2f(0.0f,0.0f);
      glVertex3f(vec[0],vec[1],0.0f);
  
      vec[0]=(float)count2+1.0f;
      vec[1]=(float)count+1.0f;
      glTexCoord2f(1.0f,0.0f);
      glVertex3f(vec[0],vec[1],0.0f);
  
      vec[0]=(float)count2+1.0f;
      vec[1]=(float)count;
      glTexCoord2f(1.0f,1.0f);
      glVertex3f(vec[0],vec[1],0.0f);
  
      vec[0]=(float)count2;
      vec[1]=(float)count;
      glTexCoord2f(0.0f,1.0f);
      glVertex3f(vec[0],vec[1],0.0f);
  
      glEnd();
#endif
      }
    }
  }

void renderbonds(void)
  {
  int count;
  float vec[3];

  glDisable(GL_TEXTURE_2D);

#if defined(USE_GLES)
    int index = -1;
    GLfloat vtxA[numofbonds*6];

    glEnableClientState(GL_VERTEX_ARRAY);
#else
  glBegin(GL_LINES);
#endif

  glColor4f(0.0f,1.0f,0.0f,1.0f);

  for (count=0;count<numofbonds;count++)
    {
#if defined(USE_GLES)
    vtxA[++index] = particle[bond[count].part1].position[0];
    vtxA[++index] = particle[bond[count].part1].position[1];
    vtxA[++index] = particle[bond[count].part1].position[2];

    vtxA[++index] = particle[bond[count].part2].position[0];
    vtxA[++index] = particle[bond[count].part2].position[1];
    vtxA[++index] = particle[bond[count].part2].position[2];
#else
    vec[0]=particle[bond[count].part1].position[0];
    vec[1]=particle[bond[count].part1].position[1];
    vec[2]=particle[bond[count].part1].position[2];
    glVertex3fv(vec);

    vec[0]=particle[bond[count].part2].position[0];
    vec[1]=particle[bond[count].part2].position[1];
    vec[2]=particle[bond[count].part2].position[2];
    glVertex3fv(vec);
#endif
    }

#if defined(USE_GLES)
        glVertexPointer(3, GL_FLOAT, 0, vtxA);
        glDrawArrays(GL_LINES,0,numofbonds*2);
    glDisableClientState(GL_VERTEX_ARRAY);
#endif


  glColor4f(1.0f,1.0f,0.0f,1.0f);

#if defined(USE_GLES)
    index = -1;
    GLfloat vtxB[physicstemp.numofbonds*6];

    glEnableClientState(GL_VERTEX_ARRAY);
#endif
  for (count=0;count<physicstemp.numofbonds;count++)
    {
    if (physicstemp.bond[count].type==0)
      {
#if defined(USE_GLES)
    vtxB[++index] = particle[physicstemp.bond[count].part1].position[0];
    vtxB[++index] = particle[physicstemp.bond[count].part1].position[1];
    vtxB[++index] = particle[physicstemp.bond[count].part1].position[2];

    vtxB[++index] = physicstemp.bond[count].point[0];
    vtxB[++index] = physicstemp.bond[count].point[1];
    vtxB[++index] = physicstemp.bond[count].point[2];
#else
      glVertex3fv(particle[physicstemp.bond[count].part1].position);
      glVertex3fv(physicstemp.bond[count].point);
#endif
      }
    if (physicstemp.bond[count].type==1)
      {
#if defined(USE_GLES)
    vtxB[++index] = particle[physicstemp.bond[count].part1].position[0];
    vtxB[++index] = particle[physicstemp.bond[count].part1].position[1];
    vtxB[++index] = particle[physicstemp.bond[count].part1].position[2];

    vtxB[++index] = physicstemp.bond[count].point[0];
    vtxB[++index] = physicstemp.bond[count].point[1];
    vtxB[++index] = physicstemp.bond[count].point[2];
#else
      glVertex3fv(particle[physicstemp.bond[count].part1].position);
      glVertex3fv(physicstemp.bond[count].point);
#endif
      }
    if (physicstemp.bond[count].type==5)
      {
      scalevector(vec,particle[physicstemp.bond[count].part1].position,physicstemp.bond[count].force[0]);
      scaleaddvectors(vec,vec,particle[physicstemp.bond[count].part2].position,physicstemp.bond[count].force[1]);
#if defined(USE_GLES)
    vtxB[++index] = vec[0];
    vtxB[++index] = vec[1];
    vtxB[++index] = vec[2];

    vtxB[++index] = particle[physicstemp.bond[count].part3].position[0];
    vtxB[++index] = particle[physicstemp.bond[count].part3].position[1];
    vtxB[++index] = particle[physicstemp.bond[count].part3].position[2];
#else
      glVertex3fv(vec);
      glVertex3fv(particle[physicstemp.bond[count].part3].position);
#endif
      }
    }
#if defined(USE_GLES)
        glVertexPointer(3, GL_FLOAT, 0, vtxB);
        glDrawArrays(GL_LINES,0,physicstemp.numofbonds*2);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
  glEnd();
#endif

  glEnable(GL_TEXTURE_2D);
  }

void renderobjectinvisible(int objectnum)
  {
  int count,count2;
  float vec[3],vec2[3];
  float windowsize;

  windowsize=2.0f*640.0f/800.0f;

  vec[0]=object[objectrender[objectnum].objectnum].position[0]-view.position[0];
  vec[1]=object[objectrender[objectnum].objectnum].position[1]-view.position[1];
  vec[0]/=10.0f;
  vec[1]/=10.0f;
  count2=(windowinfo.resolutionx>>1)+vec[0]*(windowinfo.resolutionx>>1);
  count=(windowinfo.resolutiony>>1)+vec[1]*(windowinfo.resolutionx>>1);
  count2-=64;
  count-=64;
  if (count2<0)
    count2=0;
  if (count<0)
    count=0;
  if (count2>windowinfo.resolutionx-128)
    count2=windowinfo.resolutionx-128;
  if (count>windowinfo.resolutiony-128)
    count=windowinfo.resolutiony-128;

  glBindTexture(GL_TEXTURE_2D,texture[331].glname);
  glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,count2,count,128,128,0);

  glBlendFunc(GL_ONE,GL_ZERO);

  //glDisable(GL_ALPHA_TEST);

  glBindTexture(GL_TEXTURE_2D,texture[331].glname);

#if defined(USE_GLES)
    int index_vtx = -1;
    int index_tex = -1;
    GLfloat vtx[32*9];
    GLfloat tex[32*6];

    glColor4f(0.75f,0.75f,0.75f,1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#else
  glBegin(GL_TRIANGLES);

  glColor3f(0.75f,0.75f,0.75f);
#endif

  for (count2=0;count2<32;count2++)
    {
    subtractvectors(vec2,objectrender[objectnum].vertex[count2],object[objectrender[objectnum].objectnum].position);
    normalizevector(vec2,vec2);
#if defined(USE_GLES)
    tex[++index_tex] = 0.5f+vec2[0]*0.3f;
    tex[++index_tex] = 0.5f+vec2[1]*0.3f;
    vtx[++index_vtx] = objectrender[objectnum].vertex[count2][0];
    vtx[++index_vtx] = objectrender[objectnum].vertex[count2][1];
    vtx[++index_vtx] = objectrender[objectnum].vertex[count2][2];
#else
    glTexCoord2f(0.5f+vec2[0]*0.3f,0.5f+vec2[1]*0.3f);
    glVertex3fv(objectrender[objectnum].vertex[count2]);
#endif

    subtractvectors(vec2,objectrender[objectnum].vertex[((count2+1)&31)],object[objectrender[objectnum].objectnum].position);
    normalizevector(vec2,vec2);
#if defined(USE_GLES)
    tex[++index_tex] = 0.5f+vec2[0]*0.3f;
    tex[++index_tex] = 0.5f+vec2[1]*0.3f;
    vtx[++index_vtx] = objectrender[objectnum].vertex[((count2+1)&31)][0];
    vtx[++index_vtx] = objectrender[objectnum].vertex[((count2+1)&31)][1];
    vtx[++index_vtx] = objectrender[objectnum].vertex[((count2+1)&31)][2];

    tex[++index_tex] = 0.5f;
    tex[++index_tex] = 0.5f;
    vtx[++index_vtx] = objectrender[objectnum].vertex[32][0];
    vtx[++index_vtx] = objectrender[objectnum].vertex[32][1];
    vtx[++index_vtx] = objectrender[objectnum].vertex[32][2];
#else
    glTexCoord2f(0.5f+vec2[0]*0.3f,0.5f+vec2[1]*0.3f);
    glVertex3fv(objectrender[objectnum].vertex[((count2+1)&31)]);

    glTexCoord2f(0.5f,0.5f);
    glVertex3fv(objectrender[objectnum].vertex[32]);
#endif
    }

#if defined(USE_GLES)
        glVertexPointer(3, GL_FLOAT, 0, vtx);
        glTexCoordPointer(2, GL_FLOAT, 0, tex);
        glDrawArrays(GL_TRIANGLES,0,32*3);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
  glEnd();
#endif

  /*
  for (count=0;count<16;count++)
    {
    glBegin(GL_TRIANGLES);

    glColor3f(0.5f,0.5f,0.5f);

    subtractvectors(vec2,particle[object[objectnum].particle[count]].position,object[objectnum].position);
    normalizevector(vec2,vec2);
    glTexCoord2f(0.5f+vec2[0]*0.3f,0.5f+vec2[1]*0.3f);
    glVertex3fv(particle[object[objectnum].particle[count]].position);

    subtractvectors(vec2,particle[object[objectnum].particle[((count+1)&15)]].position,object[objectnum].position);
    normalizevector(vec2,vec2);
    glTexCoord2f(0.5f+vec2[0]*0.3f,0.5f+vec2[1]*0.3f);
    glVertex3fv(particle[object[objectnum].particle[((count+1)&15)]].position);

    glTexCoord2f(0.5f,0.5f);
    glVertex3fv(object[objectnum].position);

    glEnd();
    }
  */
  //glEnable(GL_ALPHA_TEST);

  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  }

void setuplighttexcoord(int lightcount,float position[3])
  {
  float texcoord[2];

  texcoord[0]=(position[0]-frame.light[lightcount].position[0])/frame.light[lightcount].intensity+0.5f;
  texcoord[1]=(position[1]-frame.light[lightcount].position[1])/frame.light[lightcount].intensity+0.5f;

  glMultiTexCoord2fARB(GL_TEXTURE1_ARB,texcoord[0],texcoord[1]);
  }

float calclight(int lightcount,float position[3],float normal[3])
  {
  float vec[3],vec2[3];
  float scale;

  subtractvectors(vec,frame.light[lightcount].position,position);
  normalizevector(vec2,vec);
  scale=frame.light[lightcount].intensity*dotproduct(vec2,normal)/(vec[0]*vec[0]+vec[1]*vec[1]);

  return(scale);
  }

void renderparticles(void)
  {
  int count;
  float vec[3];
  float alpha;

  for (count=0;count<numofparticles;count++)
  if (particle[count].type==5)
  if (particle[count].texturenum!=366)
    {
    alpha=1.0f;
    if (particle[count].timetolive<50)
      alpha=(float)particle[count].timetolive/50.0f;

    glBindTexture(GL_TEXTURE_2D,texture[particle[count].texturenum].glname);

#if defined(USE_GLES)
    glColor4f(1.0f,1.0f,1.0f,alpha);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        GLfloat quad[] = {  particle[count].position[0]-particle[count].rendersize*0.5f,particle[count].position[1]+particle[count].rendersize*0.5f,
                            particle[count].position[0]+particle[count].rendersize*0.5f,particle[count].position[1]+particle[count].rendersize*0.5f,
                            particle[count].position[0]+particle[count].rendersize*0.5f,particle[count].position[1]-particle[count].rendersize*0.5f,
                            particle[count].position[0]-particle[count].rendersize*0.5f,particle[count].position[1]-particle[count].rendersize*0.5f };
        GLfloat tx[] = {    0,0,
                            1,0,
                            1,1,
                            0,1};
        glVertexPointer(2, GL_FLOAT, 0, quad);
        glTexCoordPointer(2, GL_FLOAT, 0, tx);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
    glBegin(GL_QUADS);

    glColor4f(1.0f,1.0f,1.0f,alpha);

    glTexCoord2f(0.0f,0.0f);
    vec[0]=particle[count].position[0]-particle[count].rendersize*0.5f;
    vec[1]=particle[count].position[1]+particle[count].rendersize*0.5f;
    glVertex3f(vec[0],vec[1],0.0f);

    glTexCoord2f(1.0f,0.0f);
    vec[0]=particle[count].position[0]+particle[count].rendersize*0.5f;
    vec[1]=particle[count].position[1]+particle[count].rendersize*0.5f;
    glVertex3f(vec[0],vec[1],0.0f);

    glTexCoord2f(1.0f,1.0f);
    vec[0]=particle[count].position[0]+particle[count].rendersize*0.5f;
    vec[1]=particle[count].position[1]-particle[count].rendersize*0.5f;
    glVertex3f(vec[0],vec[1],0.0f);

    glTexCoord2f(0.0f,1.0f);
    vec[0]=particle[count].position[0]-particle[count].rendersize*0.5f;
    vec[1]=particle[count].position[1]-particle[count].rendersize*0.5f;
    glVertex3f(vec[0],vec[1],0.0f);

    glEnd();
#endif
    }
  }

void renderparticles2(void)
  {
  int count;
  float vec[3];
  float alpha;

  for (count=0;count<numofparticles;count++)
  if (particle[count].type==5)
  if (particle[count].texturenum==366)
    {
    alpha=1.0f;
    if (particle[count].timetolive<50)
      alpha=(float)particle[count].timetolive/50.0f;

    glBindTexture(GL_TEXTURE_2D,texture[particle[count].texturenum].glname);

#if defined(USE_GLES)
    glColor4f(1.0f,1.0f,1.0f,alpha);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        GLfloat quad[] = {  particle[count].position[0]-particle[count].rendersize*0.5f,particle[count].position[1]+particle[count].rendersize*0.5f,
                            particle[count].position[0]+particle[count].rendersize*0.5f,particle[count].position[1]+particle[count].rendersize*0.5f,
                            particle[count].position[0]+particle[count].rendersize*0.5f,particle[count].position[1]-particle[count].rendersize*0.5f,
                            particle[count].position[0]-particle[count].rendersize*0.5f,particle[count].position[1]-particle[count].rendersize*0.5f };
        GLfloat tx[] = {    0,0,
                            1,0,
                            1,1,
                            0,1};
        glVertexPointer(2, GL_FLOAT, 0, quad);
        glTexCoordPointer(2, GL_FLOAT, 0, tx);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
    glBegin(GL_QUADS);

    glColor4f(1.0f,1.0f,1.0f,alpha);

    glTexCoord2f(0.0f,0.0f);
    vec[0]=particle[count].position[0]-particle[count].rendersize*0.5f;
    vec[1]=particle[count].position[1]+particle[count].rendersize*0.5f;
    glVertex3f(vec[0],vec[1],0.0f);

    glTexCoord2f(1.0f,0.0f);
    vec[0]=particle[count].position[0]+particle[count].rendersize*0.5f;
    vec[1]=particle[count].position[1]+particle[count].rendersize*0.5f;
    glVertex3f(vec[0],vec[1],0.0f);

    glTexCoord2f(1.0f,1.0f);
    vec[0]=particle[count].position[0]+particle[count].rendersize*0.5f;
    vec[1]=particle[count].position[1]-particle[count].rendersize*0.5f;
    glVertex3f(vec[0],vec[1],0.0f);

    glTexCoord2f(0.0f,1.0f);
    vec[0]=particle[count].position[0]-particle[count].rendersize*0.5f;
    vec[1]=particle[count].position[1]-particle[count].rendersize*0.5f;
    glVertex3f(vec[0],vec[1],0.0f);

    glEnd();
#endif
    }
  }

void renderobjects(void)
  {
#if defined(USE_GLES)
    int index = -1;
    int index_vtxA = -1;
    int index_vtxB = -1;
    int index_colorB = -1;
    int index_vtxC = -1;
    int index_texC = -1;
    int index_vtxD = -1;
    int index_texD = -1;
    int index_vtxE = -1;
    int index_texE = -1;
    int index_colorE = -1;
#endif
  int count,count2;
  int lightcount;
  float vec[3],vec2[3];
  float normal[3];
  float color[4];
  float size;

  updateogg();

  for (count=0;count<numofobjectrenders;count++)
    {
    if (objectrender[count].type==1)
      {
      if (game.godmode && game.oldschool==0)
        renderobjectinvisible(count);
      else
        {
        glDisable(GL_TEXTURE_2D);
  
#if defined(USE_GLES)
    index = -1;
    GLfloat vtx[32*9];
    glEnableClientState(GL_VERTEX_ARRAY);
#else
        glBegin(GL_TRIANGLES);
#endif
  
        glColor4f(0.0f,0.0f,0.0f,1.0f);
        if (objectrender[count].objectnum==1)
          glColor4f(0.5f,0.5f,0.5f,1.0f);
        if (objectrender[count].objectnum==2)
          glColor4f(0.355f,0.31f,0.15f,1.0f);
        if (objectrender[count].objectnum==3)
          glColor4f(0.453f,0.3f,0.28f,1.0f);
  
        if (object[objectrender[count].objectnum].hitpoints<object[objectrender[count].objectnum].prevhitpoints)
          glColor4f(0.25f,0.0f,0.0f,1.0f);
  
        for (count2=0;count2<32;count2++)
          {
#if defined(USE_GLES)
            vtx[++index] = objectrender[count].vertex[count2][0];
            vtx[++index] = objectrender[count].vertex[count2][1];
            vtx[++index] = objectrender[count].vertex[count2][2];
            vtx[++index] = objectrender[count].vertex[((count2+1)&31)][0];
            vtx[++index] = objectrender[count].vertex[((count2+1)&31)][1];
            vtx[++index] = objectrender[count].vertex[((count2+1)&31)][2];
            vtx[++index] = objectrender[count].vertex[32][0];
            vtx[++index] = objectrender[count].vertex[32][1];
            vtx[++index] = objectrender[count].vertex[32][2];
#else
          glVertex3fv(objectrender[count].vertex[count2]);
          glVertex3fv(objectrender[count].vertex[((count2+1)&31)]);
          glVertex3fv(objectrender[count].vertex[32]);
#endif
          }
  
#if defined(USE_GLES)
        glVertexPointer(3, GL_FLOAT, 0, vtx);
        glDrawArrays(GL_TRIANGLES,0,32*3);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
        glEnd();
#endif

        glEnable(GL_TEXTURE_2D);
        }

      renderobjectspecular(count);

      size=0.75f;

      glBindTexture(GL_TEXTURE_2D,texture[objectrender[count].texturenum].glname);

#if defined(USE_GLES)
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        GLfloat quad[] = {  objectrender[count].vertex[32][0]+size*cos(objectrender[count].angle+pi/4.0f),objectrender[count].vertex[32][1]+size*sin(objectrender[count].angle+pi/4.0f),
                            objectrender[count].vertex[32][0]+size*cos(objectrender[count].angle+3.0f*pi/4.0f),objectrender[count].vertex[32][1]+size*sin(objectrender[count].angle+3.0f*pi/4.0f),
                            objectrender[count].vertex[32][0]+size*cos(objectrender[count].angle+5.0f*pi/4.0f),objectrender[count].vertex[32][1]+size*sin(objectrender[count].angle+5.0f*pi/4.0f),
                            objectrender[count].vertex[32][0]+size*cos(objectrender[count].angle+7.0f*pi/4.0f),objectrender[count].vertex[32][1]+size*sin(objectrender[count].angle+7.0f*pi/4.0f) };
        GLfloat txA[] = {    0,0,
                             1,0,
                             1,1,
                             0,1};
        GLfloat txB[] = {    1,0,
                             0,0,
                             0,1,
                             1,1};
        glVertexPointer(2, GL_FLOAT, 0, quad);
        if (object[objectrender[count].objectnum].direction==0)
            glTexCoordPointer(2, GL_FLOAT, 0, txA);
        else
            glTexCoordPointer(2, GL_FLOAT, 0, txB);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
      glBegin(GL_QUADS);
    
      glColor4f(1.0f,1.0f,1.0f,1.0f);
    
      vec[0]=objectrender[count].vertex[32][0]+size*cos(objectrender[count].angle+pi/4.0f);
      vec[1]=objectrender[count].vertex[32][1]+size*sin(objectrender[count].angle+pi/4.0f);
      vec[2]=0.0f;
      if (object[objectrender[count].objectnum].direction==0)
        glTexCoord2f(0.0f,0.0f);
      else
        glTexCoord2f(1.0f,0.0f);
      glVertex3fv(vec);
    
      vec[0]=objectrender[count].vertex[32][0]+size*cos(objectrender[count].angle+3.0f*pi/4.0f);
      vec[1]=objectrender[count].vertex[32][1]+size*sin(objectrender[count].angle+3.0f*pi/4.0f);
      vec[2]=0.0f;
      if (object[objectrender[count].objectnum].direction==0)
        glTexCoord2f(1.0f,0.0f);
      else
        glTexCoord2f(0.0f,0.0f);
      glVertex3fv(vec);
    
      vec[0]=objectrender[count].vertex[32][0]+size*cos(objectrender[count].angle+5.0f*pi/4.0f);
      vec[1]=objectrender[count].vertex[32][1]+size*sin(objectrender[count].angle+5.0f*pi/4.0f);
      vec[2]=0.0f;
      if (object[objectrender[count].objectnum].direction==0)
        glTexCoord2f(1.0f,1.0f);
      else
        glTexCoord2f(0.0f,1.0f);   
      glVertex3fv(vec);
    
      vec[0]=objectrender[count].vertex[32][0]+size*cos(objectrender[count].angle+7.0f*pi/4.0f);
      vec[1]=objectrender[count].vertex[32][1]+size*sin(objectrender[count].angle+7.0f*pi/4.0f);
      vec[2]=0.0f;
      if (object[objectrender[count].objectnum].direction==0)
        glTexCoord2f(0.0f,1.0f);
      else
        glTexCoord2f(1.0f,1.0f);
      glVertex3fv(vec);
    
      glEnd();
#endif
      if (!object[objectrender[count].objectnum].idata[0])
        {
        if (game.oldschool==0)
          glEnable(GL_LINE_SMOOTH);

        glDisable(GL_TEXTURE_2D);
  
        for (count2=0;count2<32;count2++)
          {
#if defined(USE_GLES)
        glEnableClientState(GL_VERTEX_ARRAY);
        GLfloat vtx[] = {   objectrender[count].vertex[count2][0],objectrender[count].vertex[count2][1],objectrender[count].vertex[count2][2],
                            objectrender[count].vertex[((count2+1)&31)][0],objectrender[count].vertex[((count2+1)&31)][1],objectrender[count].vertex[((count2+1)&31)][2] };
#else
          glBegin(GL_LINES);
#endif
  
          if (objectrender[count].objectnum==0)
            glColor4f(0.5f,0.5f,0.5f,0.5f);
          else
            glColor4f(0.25f,0.25f,0.25f,0.5f);
  
#if defined(USE_GLES)
        glVertexPointer(3, GL_FLOAT, 0, vtx);
        glDrawArrays(GL_LINES,0,2);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
          glVertex3fv(objectrender[count].vertex[count2]);
          glVertex3fv(objectrender[count].vertex[((count2+1)&31)]);
  
          glEnd();
#endif
          }
        if ((object[objectrender[count].objectnum].button&4)==4)
        for (count2=0;count2<32;count2++)
          {
#if defined(USE_GLES)
        index_vtxA = -1;
        GLfloat vtxA[6];
        glEnableClientState(GL_VERTEX_ARRAY);
#else
          glBegin(GL_LINES);
#endif
          if (objectrender[count].objectnum==0)
            glColor4f(0.5f,0.5f,0.5f,0.5f);
          else
            glColor4f(0.25f,0.25f,0.25f,0.5f);
  
          normal[0]=objectrender[count].vertex[count2][1]-objectrender[count].vertex[((count2+1)&31)][1];
          normal[1]=objectrender[count].vertex[((count2+1)&31)][0]-objectrender[count].vertex[count2][0];
          normal[2]=0.0f;
  
          copyvector(vec,objectrender[count].vertex[count2]);
          scaleaddvectors(vec,vec,normal,0.1f);
#if defined(USE_GLES)
            vtxA[++index_vtxA] = vec[0]; vtxA[++index_vtxA] = vec[1]; vtxA[++index_vtxA] = vec[2];
#else
          glVertex3fv(vec);
#endif
  
          normal[0]=objectrender[count].vertex[((count2+1)&31)][1]-objectrender[count].vertex[((count2+2)&31)][1];
          normal[1]=objectrender[count].vertex[((count2+2)&31)][0]-objectrender[count].vertex[((count2+1)&31)][0];
          normal[2]=0.0f;
  
          copyvector(vec,objectrender[count].vertex[((count2+1)&31)]);
          scaleaddvectors(vec,vec,normal,0.1f);
#if defined(USE_GLES)
            vtxA[++index_vtxA] = vec[0]; vtxA[++index_vtxA] = vec[1]; vtxA[++index_vtxA] = vec[2];
#else
          glVertex3fv(vec);
#endif
  
#if defined(USE_GLES)
        glVertexPointer(3, GL_FLOAT, 0, vtxA);
        glDrawArrays(GL_LINES,0,2);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
          glEnd();
#endif
          }
        if ((object[objectrender[count].objectnum].button&1)==1)
        for (count2=0;count2<32;count2++)
          {
#if defined(USE_GLES)
        index_vtxB = -1;
        index_colorB = -1;
        GLfloat vtxB[12];
        GLfloat colorB[16];
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
#else
          glBegin(GL_LINES);
#endif
          if (objectrender[count].objectnum==0)
            {
            color[0]=0.5f;
            color[1]=0.5f;
            color[2]=0.5f;
            }
          else
            {
            color[0]=0.25f;
            color[1]=0.25f;
            color[2]=0.25f;
            }
  
          normal[0]=objectrender[count].vertex[count2][1]-objectrender[count].vertex[((count2+1)&31)][1];
          normal[1]=objectrender[count].vertex[((count2+1)&31)][0]-objectrender[count].vertex[count2][0];
          normal[2]=0.0f;
          scalevector(normal,normal,0.8f);
  
          copyvector(vec,objectrender[count].vertex[count2]);
          color[3]=1.0f;
#if defined(USE_GLES)
            vtxB[++index_vtxB] = vec[0]; vtxB[++index_vtxB] = vec[1]; vtxB[++index_vtxB] = vec[2];
            colorB[++index_colorB] = color[0]; colorB[++index_colorB] = color[1]; colorB[++index_colorB] = color[2]; colorB[++index_colorB] = color[3];
#else
          glColor4fv(color);
          glVertex3fv(vec);
#endif
          addvectors(vec,vec,normal);
          color[3]=0.0f;
#if defined(USE_GLES)
            vtxB[++index_vtxB] = vec[0]; vtxB[++index_vtxB] = vec[1]; vtxB[++index_vtxB] = vec[2];
            colorB[++index_colorB] = color[0]; colorB[++index_colorB] = color[1]; colorB[++index_colorB] = color[2]; colorB[++index_colorB] = color[3];
#else
          glColor4fv(color);
          glVertex3fv(vec);
#endif
          scalevector(vec,objectrender[count].vertex[count2],0.5f);
          scaleaddvectors(vec,vec,objectrender[count].vertex[((count2+1)&31)],0.5f);
          color[3]=1.0f;
#if defined(USE_GLES)
            vtxB[++index_vtxB] = vec[0]; vtxB[++index_vtxB] = vec[1]; vtxB[++index_vtxB] = vec[2];
            colorB[++index_colorB] = color[0]; colorB[++index_colorB] = color[1]; colorB[++index_colorB] = color[2]; colorB[++index_colorB] = color[3];
#else
          glColor4fv(color);
          glVertex3fv(vec);
#endif
          addvectors(vec,vec,normal);
          color[3]=0.0f;
#if defined(USE_GLES)
            vtxB[++index_vtxB] = vec[0]; vtxB[++index_vtxB] = vec[1]; vtxB[++index_vtxB] = vec[2];
            colorB[++index_colorB] = color[0]; colorB[++index_colorB] = color[1]; colorB[++index_colorB] = color[2]; colorB[++index_colorB] = color[3];
#else
          glColor4fv(color);
          glVertex3fv(vec);
#endif
#if defined(USE_GLES)
        glVertexPointer(3, GL_FLOAT, 0, vtxB);
        glColorPointer(4, GL_FLOAT, 0, colorB);
        glDrawArrays(GL_LINES,0,4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
#else
          glEnd();
#endif
          }
  
        glEnable(GL_TEXTURE_2D);

        glDisable(GL_LINE_SMOOTH);
        }
      }
    else
      {
      glBindTexture(GL_TEXTURE_2D,texture[objectrender[count].texturenum].glname);

#if defined(USE_GLES)
        index_vtxC = -1;
        index_texC = -1;
        GLfloat vtxC[objectrender[count].numoftris*9];
        GLfloat texC[objectrender[count].numoftris*6];
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#else
      glBegin(GL_TRIANGLES);
#endif
      glColor4f(level.ambient[1][0],level.ambient[1][1],level.ambient[1][2],objectrender[count].alpha);

      if (objectrender[count].type==8)
      if (object[objectrender[count].objectnum].lighttype==1) {
#if defined(USE_GLES)
        glColor4f(object[objectrender[count].objectnum].lightcolor[0], object[objectrender[count].objectnum].lightcolor[1], object[objectrender[count].objectnum].lightcolor[2], 1.0f);
#else
        glColor3fv(object[objectrender[count].objectnum].lightcolor);
#endif
      }

      for (count2=0;count2<objectrender[count].numoftris;count2++)
        {
#if defined(USE_GLES)
        texC[++index_texC] = objectrender[count].texcoord[objectrender[count].tri[count2][0]][0]; texC[++index_texC] = objectrender[count].texcoord[objectrender[count].tri[count2][0]][1];
        vtxC[++index_vtxC] = objectrender[count].vertex[objectrender[count].tri[count2][0]][0]; vtxC[++index_vtxC] = objectrender[count].vertex[objectrender[count].tri[count2][0]][1]; vtxC[++index_vtxC] = objectrender[count].vertex[objectrender[count].tri[count2][0]][2];
        texC[++index_texC] = objectrender[count].texcoord[objectrender[count].tri[count2][1]][0]; texC[++index_texC] = objectrender[count].texcoord[objectrender[count].tri[count2][1]][1];
        vtxC[++index_vtxC] = objectrender[count].vertex[objectrender[count].tri[count2][1]][0]; vtxC[++index_vtxC] = objectrender[count].vertex[objectrender[count].tri[count2][1]][1]; vtxC[++index_vtxC] = objectrender[count].vertex[objectrender[count].tri[count2][1]][2];
        texC[++index_texC] = objectrender[count].texcoord[objectrender[count].tri[count2][2]][0]; texC[++index_texC] = objectrender[count].texcoord[objectrender[count].tri[count2][2]][1];
        vtxC[++index_vtxC] = objectrender[count].vertex[objectrender[count].tri[count2][2]][0]; vtxC[++index_vtxC] = objectrender[count].vertex[objectrender[count].tri[count2][2]][1]; vtxC[++index_vtxC] = objectrender[count].vertex[objectrender[count].tri[count2][2]][2];
#else
        glTexCoord2fv(objectrender[count].texcoord[objectrender[count].tri[count2][0]]);
        glVertex3fv(objectrender[count].vertex[objectrender[count].tri[count2][0]]);
        glTexCoord2fv(objectrender[count].texcoord[objectrender[count].tri[count2][1]]);
        glVertex3fv(objectrender[count].vertex[objectrender[count].tri[count2][1]]);
        glTexCoord2fv(objectrender[count].texcoord[objectrender[count].tri[count2][2]]);
        glVertex3fv(objectrender[count].vertex[objectrender[count].tri[count2][2]]);
#endif
        }
#if defined(USE_GLES)
        glVertexPointer(3, GL_FLOAT, 0, vtxC);
        glTexCoordPointer(2, GL_FLOAT, 0, texC);
        glDrawArrays(GL_TRIANGLES,0,objectrender[count].numoftris*3);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
      glEnd();
#endif

      glBlendFunc(GL_SRC_ALPHA,GL_ONE);
      glEnable(GL_STENCIL_TEST);

      for (lightcount=0;lightcount<frame.numoflights;lightcount++)
      if (((objectrender[count].lightflags>>lightcount)&1)==1)
        {
        glStencilMask((1<<lightcount));
        glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
        glStencilFunc(GL_NOTEQUAL,(1<<lightcount),(1<<lightcount));

        glActiveTextureARB(GL_TEXTURE1_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,texture[332].glname);

        if (objectrender[count].type!=2 && objectrender[count].type!=10)
          {
#if defined(USE_GLES)
        index_vtxD = -1;
        index_texD = -1;
        GLfloat vtxD[objectrender[count].numoftris*9];
        GLfloat texD[objectrender[count].numoftris*6];
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#else
          glBegin(GL_TRIANGLES);
#endif
          glColor4f(level.ambient[1][0],level.ambient[1][1],level.ambient[1][2],objectrender[count].alpha);
  
          for (count2=0;count2<objectrender[count].numoftris;count2++)
            {
            setuplighttexcoord(lightcount,objectrender[count].vertex[objectrender[count].tri[count2][0]]);
#if defined(USE_GLES)
        texD[++index_texD] = objectrender[count].texcoord[objectrender[count].tri[count2][0]][0]; texD[++index_texD] = objectrender[count].texcoord[objectrender[count].tri[count2][0]][1];
        vtxD[++index_vtxD] = objectrender[count].vertex[objectrender[count].tri[count2][0]][0]; vtxD[++index_vtxD] = objectrender[count].vertex[objectrender[count].tri[count2][0]][1]; vtxD[++index_vtxD] = objectrender[count].vertex[objectrender[count].tri[count2][0]][2];
#else
            glTexCoord2fv(objectrender[count].texcoord[objectrender[count].tri[count2][0]]);
            glVertex3fv(objectrender[count].vertex[objectrender[count].tri[count2][0]]);
#endif
            setuplighttexcoord(lightcount,objectrender[count].vertex[objectrender[count].tri[count2][1]]);
#if defined(USE_GLES)
        texD[++index_texD] = objectrender[count].texcoord[objectrender[count].tri[count2][1]][0]; texD[++index_texD] = objectrender[count].texcoord[objectrender[count].tri[count2][1]][1];
        vtxD[++index_vtxD] = objectrender[count].vertex[objectrender[count].tri[count2][1]][0]; vtxD[++index_vtxD] = objectrender[count].vertex[objectrender[count].tri[count2][1]][1]; vtxD[++index_vtxD] = objectrender[count].vertex[objectrender[count].tri[count2][1]][2];
#else
            glTexCoord2fv(objectrender[count].texcoord[objectrender[count].tri[count2][1]]);
            glVertex3fv(objectrender[count].vertex[objectrender[count].tri[count2][1]]);
#endif
            setuplighttexcoord(lightcount,objectrender[count].vertex[objectrender[count].tri[count2][2]]);
#if defined(USE_GLES)
        texD[++index_texD] = objectrender[count].texcoord[objectrender[count].tri[count2][2]][0]; texD[++index_texD] = objectrender[count].texcoord[objectrender[count].tri[count2][2]][1];
        vtxD[++index_vtxD] = objectrender[count].vertex[objectrender[count].tri[count2][2]][0]; vtxD[++index_vtxD] = objectrender[count].vertex[objectrender[count].tri[count2][2]][1]; vtxD[++index_vtxD] = objectrender[count].vertex[objectrender[count].tri[count2][2]][2];
#else
            glTexCoord2fv(objectrender[count].texcoord[objectrender[count].tri[count2][2]]);
            setuplighttexcoord(lightcount,objectrender[count].vertex[objectrender[count].tri[count2][2]]);
            glVertex3fv(objectrender[count].vertex[objectrender[count].tri[count2][2]]);
#endif
            }
  
#if defined(USE_GLES)
        glVertexPointer(3, GL_FLOAT, 0, vtxD);
        glTexCoordPointer(2, GL_FLOAT, 0, texD);
        glDrawArrays(GL_TRIANGLES,0,objectrender[count].numoftris*3);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
          glEnd();
#endif
          }
        else
          {
#if defined(USE_GLES)
        index_vtxE = -1;
        index_texE = -1;
        index_colorE = -1;
        GLfloat vtxE[objectrender[count].numoftris*12];
        GLfloat texE[objectrender[count].numoftris*8];
        GLfloat colorE[objectrender[count].numoftris*16];
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
#else
          glBegin(GL_QUADS);
#endif
          for (count2=0;count2<objectrender[count].numofedges;count2++)
            {
            subtractvectors(vec2,frame.light[lightcount].position,objectrender[count].edgevertex[count2][0]);
            scalevector(vec,frame.light[lightcount].color,dotproduct(vec2,objectrender[count].edgenormal[count2]));
#if defined(USE_GLES)
            colorE[++index_colorE]=vec[0]; colorE[++index_colorE]=vec[1]; colorE[++index_colorE]=vec[2]; colorE[++index_colorE]=objectrender[count].alpha;
#else
            glColor4f(vec[0],vec[1],vec[2],objectrender[count].alpha);
#endif

            setuplighttexcoord(lightcount,objectrender[count].edgevertex[count2][0]);
#if defined(USE_GLES)
            texE[++index_texE] = objectrender[count].edgetexcoord[count2][0][0]; texE[++index_texE] = objectrender[count].edgetexcoord[count2][0][1];
            vtxE[++index_vtxE] = objectrender[count].edgevertex[count2][0][0]; vtxE[++index_vtxE] = objectrender[count].edgevertex[count2][0][1]; vtxE[++index_vtxE] = objectrender[count].edgevertex[count2][0][2];
#else
            glTexCoord2fv(objectrender[count].edgetexcoord[count2][0]);
            glVertex3fv(objectrender[count].edgevertex[count2][0]);
#endif

            subtractvectors(vec2,frame.light[lightcount].position,objectrender[count].edgevertex[count2][1]);
            scalevector(vec,frame.light[lightcount].color,dotproduct(vec2,objectrender[count].edgenormal[count2]));
#if defined(USE_GLES)
            colorE[++index_colorE]=vec[0]; colorE[++index_colorE]=vec[1]; colorE[++index_colorE]=vec[2]; colorE[++index_colorE]=objectrender[count].alpha;
#else
            glColor4f(vec[0],vec[1],vec[2],objectrender[count].alpha);
#endif

            setuplighttexcoord(lightcount,objectrender[count].edgevertex[count2][1]);
#if defined(USE_GLES)
            texE[++index_texE] = objectrender[count].edgetexcoord[count2][1][0]; texE[++index_texE] = objectrender[count].edgetexcoord[count2][1][1];
            vtxE[++index_vtxE] = objectrender[count].edgevertex[count2][1][0]; vtxE[++index_vtxE] = objectrender[count].edgevertex[count2][1][1]; vtxE[++index_vtxE] = objectrender[count].edgevertex[count2][1][2];
#else
            glTexCoord2fv(objectrender[count].edgetexcoord[count2][1]);
            glVertex3fv(objectrender[count].edgevertex[count2][1]);
#endif
            zerovector(vec);
#if defined(USE_GLES)
            colorE[++index_colorE]=vec[0]; colorE[++index_colorE]=vec[1]; colorE[++index_colorE]=vec[2]; colorE[++index_colorE]=objectrender[count].alpha;
#else
            glColor4f(vec[0],vec[1],vec[2],objectrender[count].alpha);
#endif

            setuplighttexcoord(lightcount,objectrender[count].edgevertex[count2][2]);
#if defined(USE_GLES)
            texE[++index_texE] = objectrender[count].edgetexcoord[count2][2][0]; texE[++index_texE] = objectrender[count].edgetexcoord[count2][2][1];
            vtxE[++index_vtxE] = objectrender[count].edgevertex[count2][2][0]; vtxE[++index_vtxE] = objectrender[count].edgevertex[count2][2][1]; vtxE[++index_vtxE] = objectrender[count].edgevertex[count2][2][2];
#else
            glTexCoord2fv(objectrender[count].edgetexcoord[count2][2]);
            glVertex3fv(objectrender[count].edgevertex[count2][2]);
#endif
            zerovector(vec);
#if defined(USE_GLES)
            colorE[++index_colorE]=vec[0]; colorE[++index_colorE]=vec[1]; colorE[++index_colorE]=vec[2]; colorE[++index_colorE]=objectrender[count].alpha;
#else
            glColor4f(vec[0],vec[1],vec[2],objectrender[count].alpha);
#endif

            setuplighttexcoord(lightcount,objectrender[count].edgevertex[count2][3]);
#if defined(USE_GLES)
            texE[++index_texE] = objectrender[count].edgetexcoord[count2][3][0]; texE[++index_texE] = objectrender[count].edgetexcoord[count2][3][1];
            vtxE[++index_vtxE] = objectrender[count].edgevertex[count2][3][0]; vtxE[++index_vtxE] = objectrender[count].edgevertex[count2][3][1]; vtxE[++index_vtxE] = objectrender[count].edgevertex[count2][3][2];
#else
            glTexCoord2fv(objectrender[count].edgetexcoord[count2][3]);
            glVertex3fv(objectrender[count].edgevertex[count2][3]);
#endif
            }
#if defined(USE_GLES)
        glVertexPointer(3, GL_FLOAT, 0, vtxE);
        glTexCoordPointer(2, GL_FLOAT, 0, texE);
        glColorPointer(4, GL_FLOAT, 0, colorE);
        glDrawArrays(GL_TRIANGLE_FAN,0,objectrender[count].numofedges*4);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
          glEnd();
#endif
          }

        glDisable(GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        }

      glDisable(GL_STENCIL_TEST);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

      if (game.oldschool==2)
      if (objectrender[count].type==32)
        {
        glDisable(GL_TEXTURE_2D);
#if defined(USE_GLES)
    GLfloat vtxF[] = {  particle[rope[objectrender[count].objectnum].part1].position[0], particle[rope[objectrender[count].objectnum].part1].position[1], particle[rope[objectrender[count].objectnum].part1].position[2],
                        particle[rope[objectrender[count].objectnum].part2].position[0], particle[rope[objectrender[count].objectnum].part2].position[1], particle[rope[objectrender[count].objectnum].part2].position[2] };

    glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vtxF);
        glDrawArrays(GL_LINES,0,2);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
        glBegin(GL_LINES);

        glColor4f(0.0f,0.0f,0.0f,1.0f);

        glVertex3fv(particle[rope[objectrender[count].objectnum].part1].position);
        glVertex3fv(particle[rope[objectrender[count].objectnum].part2].position);

        glEnd();
#endif
        glEnable(GL_TEXTURE_2D);
        }
      }
    }
  }

void rendersprites(void)
  {
  int count;
  int x,y;
  float vec[3];

  for (count=0;count<numofsprites;count++)
    {
    subtractvectors(vec,sprite[count].position,view.position);
    vec[0]/=view.zoom;
    vec[1]/=view.zoom;
    vec[0]*=(float)(640>>1);
    vec[0]+=(float)(640>>1);
    vec[1]*=(float)(640>>1);
    vec[1]=(float)(480>>1)-vec[1];
    x=vec[0];
    y=vec[1];

    drawtext(sprite[count].text,(x|TEXT_CENTER),(y|TEXT_CENTER),sprite[count].size,sprite[count].red,sprite[count].green,sprite[count].blue,sprite[count].alpha);
    }
  }

