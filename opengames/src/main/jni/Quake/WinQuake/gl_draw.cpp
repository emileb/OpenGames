/*
Copyright (C) 1996-1997 Id Software, Inc.

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

// draw.c -- this is the only file outside the refresh that touches the
// vid buffer

#include "quakedef.h"

#define GL_COLOR_INDEX8_EXT     0x80E5


cvar_t		gl_nobind = CVAR2("gl_nobind", "0");
cvar_t		gl_max_size = CVAR2("gl_max_size", "1024");
cvar_t		gl_picmip = CVAR2("gl_picmip", "0");

byte		*draw_chars;				// 8*8 graphic characters
qpic_t		*draw_disc;
qpic_t		*draw_backtile;

int			translate_texture;
int			char_texture;

typedef struct
{
  int		texnum;
  float	sl, tl, sh, th;
} glpic_t;

typedef union
{
    qpic_t qpic;
    struct {
        // First part is from qpic
        int width;
        int height;

        glpic_t glpic;
    } g;
} packedGlpic_t;

typedef union
{
    byte buffer[sizeof(qpic_t) + sizeof(glpic_t)];
    packedGlpic_t pics;
} conback_t;

conback_t conbackUnion;

#define		conback_buffer (conbackUnion.buffer)
packedGlpic_t *conback = &conbackUnion.pics;

int		gl_lightmap_format = 4;
int		gl_solid_format = 3;
int		gl_alpha_format = 4;

#if 1 // Standard defaults
int		gl_filter_min = GL_LINEAR_MIPMAP_NEAREST;
int		gl_filter_max = GL_LINEAR;
#else
int		gl_filter_min = GL_NEAREST_MIPMAP_NEAREST;
int		gl_filter_max = GL_NEAREST;
#endif

int		texels;

typedef struct
{
  int		texnum;
  char	identifier[64];
  int		width, height;
  qboolean	mipmap;
} gltexture_t;

#define	MAX_GLTEXTURES	1024
gltexture_t	gltextures[MAX_GLTEXTURES];
int			numgltextures;

// GlQuake creates textures, but never deletes them. This approach works fine on
// computers with lots of RAM and/or swap, but not so well on our swapless
// RAM-constrained system.
//
// We work around this problem by adding a level of indirection. We
// hook GL_LoadTexture to store enough information to recreate a texture.
// Then we hook GL_BindTexture to consult a table to see whether a texture
// is currently in memory or not. If it isn't, we throw out some other
// texture and bring the required texture back into memory. In this way
// we can limit the working set of textures.
//
// The texture data is stored in a memory-mapped file that is backed by
// a file on the sd card. It is recreated each time the game is run. We
// don't bother deleting it.

#define USE_TEXTURE_STORE

#ifdef USE_TEXTURE_STORE

#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

// Allow named textures to be evicted from memory.

#define TEXTURE_STORE_NAME "glquake/texture.store"

class textureStore {

private:
    static const GLuint UNUSED = (GLuint) -2;
    static const GLuint PAGED_OUT = (GLuint) -1;

    struct entry
    {
        entry* next;
        entry* prev;
        GLuint real_texnum;    // UNUSED, PAGED_OUT
        byte* pData; // 0 ==> not created by us.
        size_t size;
        qboolean alpha;
        int width;
        int height;
        qboolean mipmap;

        entry() {
            next = 0;
            prev = 0;
            real_texnum = UNUSED;
            pData = 0;
        }


        void unlink() {
            if (next) {
                next->prev = prev;
            }
            if (prev) {
                prev->next = next;
            }
            next = 0;
            prev = 0;
        }

        void insertBefore(entry* e){
            if (e) {
                prev = e->prev;
                if ( prev ) {
                    prev->next = this;
                }
                next = e;
                e->prev = this;
            }
            else {
                prev = 0;
                next = 0;
            }
        }
    };

public:

    static textureStore* get() {
        if (g_pTextureCache == 0) {
            g_pTextureCache = new textureStore();
        }
        return g_pTextureCache;
    }

    // Equivalent of glBindTexture, but uses the virtual texture table

    void bind(int virtTexNum) {
        if ( (unsigned int) virtTexNum >= TEXTURE_STORE_NUM_TEXTURES) {
            Sys_Error("not in the range we're managing");
        }
        mBoundTextureID = virtTexNum;
        entry* e = &mTextures[virtTexNum];

        //LOGI("virtTexNum = %d e->real_texnum  = %d",virtTexNum,e->real_texnum);
        if ( e->real_texnum == UNUSED) {
            glGenTextures( 1, &e->real_texnum);
        }

        if ( e->pData == 0) {
            glBindTexture(GL_TEXTURE_2D, e->real_texnum);
            return;
        }

        update(e);
    }

    void update(entry* e)
    {
        // Update the "LRU" part of the cache
        unlink(e);
        e->insertBefore(mFirst);
        mFirst = e;
        if (! mLast) {
            mLast = e;
        }

        if (e->real_texnum == PAGED_OUT ) {
            // Create a real texture
            // Make sure there is enough room for this texture
            ensure(e->size);

            glGenTextures( 1, &e->real_texnum);

            glBindTexture(GL_TEXTURE_2D, e->real_texnum);
            GL_Upload8 (e->pData, e->width, e->height, e->mipmap,
                    e->alpha);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, e->real_texnum);
        }
    }

    // Create a texture, and remember the data so we can create
    // it again later.

    void create(int width, int height, byte* data, qboolean mipmap,
            qboolean alpha) {
        int size = width * height;
        if (size + mLength > mCapacity) {
            Sys_Error("Ran out of virtual texture space. %d", size);
        };
        entry* e = &mTextures[mBoundTextureID];

        // Call evict in case the currently bound texture id is already
        // in use. (Shouldn't happen in Quake.)
        // To Do: reclaim the old texture memory from the virtual memory.

        evict(e);

        e->alpha = alpha;
        e->pData = mBase + mLength;
        memcpy(e->pData, data, size);
        e->size = size;
        e->width = width;
        e->height = height;
        e->mipmap = mipmap;
        e->real_texnum = PAGED_OUT;
        mLength += size;

        update(e);
    }

    // Re-upload the current textures because we've been reset.
    void rebindAll() {
        grabMagicTextureIds();
        for (entry* e = mFirst; e; e = e->next ) {
            if (! (e->real_texnum == UNUSED || e->real_texnum == PAGED_OUT)) {
                glBindTexture(GL_TEXTURE_2D, e->real_texnum);
                if (e->pData) {
                    GL_Upload8 (e->pData, e->width, e->height, e->mipmap,
                        e->alpha);
                }
            }
        }
    }

private:

    textureStore() {
        grabMagicTextureIds();
        mFirst = 0;
        mLast = 0;
        mTextureCount = 0;

        char fullpath[MAX_OSPATH];
        sprintf(fullpath, "%s/%s", com_gamedir, TEXTURE_STORE_NAME);

        mFileId = open(fullpath, O_RDWR | O_CREAT, 0666);
        if ( mFileId == -1 ) {
            Sys_Error("Could not open texture store file %s: %d", fullpath,
                    errno);
        }

        if (-1 == lseek(mFileId, TEXTURE_STORE_SIZE-1, SEEK_SET)) {
            Sys_Error("Could not extend the texture store file size. %d",
                    errno);
        }
        char end;
        end = 0;
        if (-1 == write(mFileId, &end, 1)) {
            Sys_Error("Could not write last byte of the texture store file. %d",
                    errno);
        }

        mBase = (byte*) mmap((caddr_t)0, TEXTURE_STORE_SIZE,
                PROT_READ | PROT_WRITE, MAP_PRIVATE, mFileId, 0);

        if (mBase == (byte*) -1) {
            Sys_Error("Could not mmap file %s: %d", fullpath, errno);
        }
        mLength = 0;
        mCapacity = TEXTURE_STORE_SIZE;
        mRamUsed = 0;
        mRamSize = LIVE_TEXTURE_LIMIT;
    }

    ~textureStore() {
        munmap(mBase, mCapacity);
        COM_CloseFile(mFileId);
    }

    void grabMagicTextureIds() {
        // reserve these two texture ids.
        glBindTexture(GL_TEXTURE_2D, UNUSED);
        glBindTexture(GL_TEXTURE_2D, PAGED_OUT);
    }

    void unlink(entry* e) {
        if (e == mFirst) {
            mFirst = e->next;
        }
        if (e == mLast) {
            mLast = e->prev;
        }
        e->unlink();
    }

    void ensure(int size) {
        while ( mRamSize - mRamUsed < (unsigned int) size) {
            entry* e = mLast;
            if(! e) {
                Sys_Error("Ran out of entries");
                return;
            }
            evict(e);
        }
        mRamUsed += size;
    }

    void evict(entry* e) {
        unlink(e);
        if ( e->pData ) {
            glDeleteTextures(1, &e->real_texnum);
            e->real_texnum = PAGED_OUT;
            mRamUsed -= e->size;
        }
    }

    //static const size_t TEXTURE_STORE_SIZE = 16 * 1024 * 1024;
    //static const size_t LIVE_TEXTURE_LIMIT = 1 * 1024 * 1024;
    //Emile
    static const size_t TEXTURE_STORE_SIZE = 32 * 1024 * 1024;
    static const size_t LIVE_TEXTURE_LIMIT = 16 * 1024 * 1024;
    static const size_t TEXTURE_STORE_NUM_TEXTURES = 512;

    int mFileId;
    byte* mBase;    // Base address of the memory mapped file
    size_t mLength; // How much of the mm file we are currently using
    size_t mCapacity; // Total size of the memory mapped file

    // Keep track of texture RAM.
    size_t mRamUsed;
    size_t mRamSize;

    // The virtual textures


    entry mTextures[MAX_GLTEXTURES];
    entry* mFirst; // LRU queue
    entry* mLast;
    size_t mTextureCount; // How many virtual textures have been allocated

    static textureStore* g_pTextureCache;

    int mBoundTextureID;
};

textureStore* textureStore::g_pTextureCache;

#endif


void GL_Bind (int texnum)
{
  if (gl_nobind.value)
    texnum = char_texture;
  if (currenttexture == texnum)
    return;
  currenttexture = texnum;
#ifdef _WIN32
  bindTexFunc (GL_TEXTURE_2D, texnum);
#else

#ifdef USE_TEXTURE_STORE
     textureStore::get()->bind(texnum);
#else
  glBindTexture(GL_TEXTURE_2D, texnum);
#endif

#endif
}


/*
=============================================================================

  scrap allocation

  Allocate all the little status bar obejcts into a single texture
  to crutch up stupid hardware / drivers

=============================================================================
*/

#define	MAX_SCRAPS		2
#define	BLOCK_WIDTH		256
#define	BLOCK_HEIGHT	256

int			scrap_allocated[MAX_SCRAPS][BLOCK_WIDTH];
byte		scrap_texels[MAX_SCRAPS][BLOCK_WIDTH*BLOCK_HEIGHT*4];
qboolean	scrap_dirty;
int			scrap_texnum;

// returns a texture number and the position inside it
int Scrap_AllocBlock (int w, int h, int *x, int *y)
{
  int		i, j;
  int		best, best2;
  int		bestx;
  int		texnum;

  for (texnum=0 ; texnum<MAX_SCRAPS ; texnum++)
  {
    best = BLOCK_HEIGHT;

    for (i=0 ; i<BLOCK_WIDTH-w ; i++)
    {
      best2 = 0;

      for (j=0 ; j<w ; j++)
      {
        if (scrap_allocated[texnum][i+j] >= best)
          break;
        if (scrap_allocated[texnum][i+j] > best2)
          best2 = scrap_allocated[texnum][i+j];
      }
      if (j == w)
      {	// this is a valid spot
        *x = i;
        *y = best = best2;
      }
    }

    if (best + h > BLOCK_HEIGHT)
      continue;

    for (i=0 ; i<w ; i++)
      scrap_allocated[texnum][*x + i] = best + h;

    return texnum;
  }

  Sys_Error ("Scrap_AllocBlock: full");
  return 0;
}

int	scrap_uploads;

void Scrap_Upload (void)
{
  int		texnum;

  scrap_uploads++;

  for (texnum=0 ; texnum<MAX_SCRAPS ; texnum++) {
    GL_Bind(scrap_texnum + texnum);
    GL_Upload8 (scrap_texels[texnum], BLOCK_WIDTH, BLOCK_HEIGHT, false, true);
  }
  scrap_dirty = false;
}

//=============================================================================
/* Support Routines */

typedef struct cachepic_s
{
  char		name[MAX_QPATH];
  qpic_t		pic;
  byte		padding[32];	// for appended glpic
} cachepic_t;

#define	MAX_CACHED_PICS		128
cachepic_t	menu_cachepics[MAX_CACHED_PICS];
int			menu_numcachepics;

byte		menuplyr_pixels[4096];

int		pic_texels;
int		pic_count;


/*
================
GL_LoadPicTexture
================
*/
int GL_LoadPicTexture (qpic_t *pic)
{
  return GL_LoadTexture ("", pic->width, pic->height, pic->data, false, true);
}


qpic_t *Draw_PicFromWad (const char *name)
{
    packedGlpic_t	*pp;

  pp = (packedGlpic_t*) W_GetLumpName (name);

  qpic_t* p = & pp->qpic;
  glpic_t* gl = & pp->g.glpic;

  // load little ones into the scrap
  if (p->width < 64 && p->height < 64)
  {
    int		x, y;
    int		i, j, k;
    int		texnum;

    texnum = Scrap_AllocBlock (p->width, p->height, &x, &y);
    scrap_dirty = true;
    k = 0;
    for (i=0 ; i<p->height ; i++)
      for (j=0 ; j<p->width ; j++, k++)
        scrap_texels[texnum][(y+i)*BLOCK_WIDTH + x + j] = p->data[k];
    texnum += scrap_texnum;
    gl->texnum = texnum;
    gl->sl = (x+0.01)/(float)BLOCK_WIDTH;
    gl->sh = (x+p->width-0.01)/(float)BLOCK_WIDTH;
    gl->tl = (y+0.01)/(float)BLOCK_WIDTH;
    gl->th = (y+p->height-0.01)/(float)BLOCK_WIDTH;

    pic_count++;
    pic_texels += p->width*p->height;
  }
  else
  {
    gl->texnum = GL_LoadPicTexture (p);
    gl->sl = 0;
    gl->sh = 1;
    gl->tl = 0;
    gl->th = 1;
  }
  return p;
}


/*
================
Draw_CachePic
================
*/
qpic_t	*Draw_CachePic (const char *path)
{
  cachepic_t	*pic;
  int			i;
  qpic_t		*dat;
  glpic_t		*gl;

  for (pic=menu_cachepics, i=0 ; i<menu_numcachepics ; pic++, i++)
    if (!strcmp (path, pic->name))
      return &pic->pic;

  if (menu_numcachepics == MAX_CACHED_PICS)
    Sys_Error ("menu_numcachepics == MAX_CACHED_PICS");
  menu_numcachepics++;
  strcpy (pic->name, path);

//
// load the pic from disk
//
  dat = (qpic_t *)COM_LoadTempFile (path);
  if (!dat)
    Sys_Error ("Draw_CachePic: failed to load %s", path);
  SwapPic (dat);

  // HACK HACK HACK --- we need to keep the bytes for
  // the translatable player picture just for the menu
  // configuration dialog
  if (!strcmp (path, "gfx/menuplyr.lmp"))
    memcpy (menuplyr_pixels, dat->data, dat->width*dat->height);

  pic->pic.width = dat->width;
  pic->pic.height = dat->height;

  glpic_t temp;
  gl = &temp;
  gl->texnum = GL_LoadPicTexture (dat);
  gl->sl = 0;
  gl->sh = 1;
  gl->tl = 0;
  gl->th = 1;

  memcpy(pic->pic.data, &temp, sizeof(temp));

  return &pic->pic;
}


void Draw_CharToConback (int num, byte *dest)
{
  int		row, col;
  byte	*source;
  int		drawline;
  int		x;

  row = num>>4;
  col = num&15;
  source = draw_chars + (row<<10) + (col<<3);

  drawline = 8;

  while (drawline--)
  {
    for (x=0 ; x<8 ; x++)
      if (source[x] != 255)
        dest[x] = 0x60 + source[x];
    source += 128;
    dest += 320;
  }

}

typedef struct
{
  const char *name;
  int	minimize, maximize;
} glmode_t;

glmode_t modes[] = {
  {"GL_NEAREST", GL_NEAREST, GL_NEAREST},
  {"GL_LINEAR", GL_LINEAR, GL_LINEAR},
  {"GL_NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST},
  {"GL_LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR},
  {"GL_NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST},
  {"GL_LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR}
};

/*
===============
Draw_TextureMode_f
===============
*/
void Draw_TextureMode_f (void)
{
  int		i;
  gltexture_t	*glt;

  if (Cmd_Argc() == 1)
  {
    for (i=0 ; i< 6 ; i++)
      if (gl_filter_min == modes[i].minimize)
      {
        Con_Printf ("%s\n", modes[i].name);
        return;
      }
    Con_Printf ("current filter is unknown???\n");
    return;
  }

  for (i=0 ; i< 6 ; i++)
  {
    if (!Q_strcasecmp (modes[i].name, Cmd_Argv(1) ) )
      break;
  }
  if (i == 6)
  {
    Con_Printf ("bad filter name\n");
    return;
  }

  gl_filter_min = modes[i].minimize;
  gl_filter_max = modes[i].maximize;

  // change all the existing mipmap texture objects
  for (i=0, glt=gltextures ; i<numgltextures ; i++, glt++)
  {
    if (glt->mipmap)
    {
      GL_Bind (glt->texnum);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_min);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
    }
  }
}

/*
===============
Draw_Init
===============
*/
void Draw_Init (void)
{
  int		i;
  qpic_t	*cb;
  byte	*dest, *src;
  int		x, y;
  char	ver[40];
  glpic_t	*gl;
  int		start;
  byte	*ncdata;
  int		f, fstep;


  Cvar_RegisterVariable (&gl_nobind);
  Cvar_RegisterVariable (&gl_max_size);
  Cvar_RegisterVariable (&gl_picmip);

  // 3dfx can only handle 256 wide textures
  if (!Q_strncasecmp ((char *)gl_renderer, "3dfx",4) ||
    strstr((char *)gl_renderer, "Glide"))
    Cvar_Set ("gl_max_size", "256");

  Cmd_AddCommand ("gl_texturemode", &Draw_TextureMode_f);

  // load the console background and the charset
  // by hand, because we need to write the version
  // string into the background before turning
  // it into a texture
  draw_chars = (byte*) W_GetLumpName ("conchars");
  for (i=0 ; i<256*64 ; i++)
    if (draw_chars[i] == 0)
      draw_chars[i] = 255;	// proper transparent color

  // now turn them into textures
  char_texture = GL_LoadTexture ("charset", 128, 128, draw_chars, false, true);

  start = Hunk_LowMark();

  cb = (qpic_t *)COM_LoadTempFile ("gfx/conback.lmp");
  if (!cb)
    Sys_Error ("Couldn't load gfx/conback.lmp");
  SwapPic (cb);

  // hack the version number directly into the pic
#if defined(__linux__)
  sprintf (ver, "(Linux %2.2f, gl %4.2f) %4.2f", (float)LINUX_VERSION, (float)GLQUAKE_VERSION, (float)VERSION);
#else
  sprintf (ver, "(gl %4.2f) %4.2f", (float)GLQUAKE_VERSION, (float)VERSION);
#endif
  dest = cb->data + 320*186 + 320 - 11 - 8*strlen(ver);
  y = strlen(ver);
  for (x=0 ; x<y ; x++)
    Draw_CharToConback (ver[x], dest+(x<<3));

#if 0
  conback->width = vid.conwidth;
  conback->height = vid.conheight;

   // scale console to vid size
   dest = ncdata = Hunk_AllocName(vid.conwidth * vid.conheight, "conback");

   for (y=0 ; y<vid.conheight ; y++, dest += vid.conwidth)
   {
     src = cb->data + cb->width * (y*cb->height/vid.conheight);
     if (vid.conwidth == cb->width)
       memcpy (dest, src, vid.conwidth);
     else
     {
       f = 0;
       fstep = cb->width*0x10000/vid.conwidth;
       for (x=0 ; x<vid.conwidth ; x+=4)
       {
         dest[x] = src[f>>16];
         f += fstep;
         dest[x+1] = src[f>>16];
         f += fstep;
         dest[x+2] = src[f>>16];
         f += fstep;
         dest[x+3] = src[f>>16];
         f += fstep;
       }
     }
   }
#else
  conback->g.width = cb->width;
  conback->g.height = cb->height;
  ncdata = cb->data;
#endif

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  gl = &conback->g.glpic;
  gl->texnum = GL_LoadTexture ("conback", conback->g.width, conback->g.height, ncdata, false, false);
  gl->sl = 0;
  gl->sh = 1;
  gl->tl = 0;
  gl->th = 1;
  conback->g.width = vid.width;
  conback->g.height = vid.height;

  // free loaded console
  Hunk_FreeToLowMark(start);

  // save a texture slot for translated picture
  translate_texture = texture_extension_number++;

  // save slots for scraps
  scrap_texnum = texture_extension_number;
  texture_extension_number += MAX_SCRAPS;

  //
  // get the other pics we need
  //
  draw_disc = Draw_PicFromWad ("disc");
  draw_backtile = Draw_PicFromWad ("backtile");
}



/*
================
Draw_Character

Draws one 8*8 graphics character with 0 being transparent.
It can be clipped to the top of the screen to allow the console to be
smoothly scrolled off.
================
*/
void Draw_Character (int x, int y, int num)
{
  byte			*dest;
  byte			*source;
  unsigned short	*pusdest;
  int				drawline;
  int				row, col;
  float			frow, fcol, size;

  if (num == 32)
    return;		// space

  num &= 255;

  if (y <= -8)
    return;			// totally off screen

  row = num>>4;
  col = num&15;

  frow = row*0.0625;
  fcol = col*0.0625;
  size = 0.0625;

  GL_Bind (char_texture);

#ifdef USE_OPENGLES
  DrawQuad(x, y, 8, 8, fcol, frow, size, size);
#else
  glBegin (GL_QUADS);
  glTexCoord2f (fcol, frow);
  glVertex2f (x, y);
  glTexCoord2f (fcol + size, frow);
  glVertex2f (x+8, y);
  glTexCoord2f (fcol + size, frow + size);
  glVertex2f (x+8, y+8);
  glTexCoord2f (fcol, frow + size);
  glVertex2f (x, y+8);
  glEnd ();
#endif
}

/*
================
Draw_String
================
*/
void Draw_String (int x, int y, const char *str)
{
  while (*str)
  {
    Draw_Character (x, y, *str);
    str++;
    x += 8;
  }
}

/*
================
Draw_DebugChar

Draws a single character directly to the upper right corner of the screen.
This is for debugging lockups by drawing different chars in different parts
of the code.
================
*/
void Draw_DebugChar (char num)
{
}

/*
=============
Draw_AlphaPic
=============
*/
void Draw_AlphaPic (int x, int y, packedGlpic_t *ppic, float alpha)
{
  byte			*dest, *source;
  unsigned short	*pusdest;
  int				v, u;
  glpic_t			*gl;

  if (scrap_dirty)
    Scrap_Upload ();
  gl = & ppic->g.glpic;
  glDisable(GL_ALPHA_TEST);
  glEnable (GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glCullFace(GL_FRONT);
  glColor4f (1,1,1,alpha);
  GL_Bind (gl->texnum);
#ifdef USE_OPENGLES
    DrawQuad(x, y, ppic->g.width, ppic->g.height, gl->sl, gl->tl, gl->sh - gl->sl, gl->th - gl->tl);
#else
  glBegin (GL_QUADS);
  glTexCoord2f (gl->sl, gl->tl);
  glVertex2f (x, y);
  glTexCoord2f (gl->sh, gl->tl);
  glVertex2f (x+pic->width, y);
  glTexCoord2f (gl->sh, gl->th);
  glVertex2f (x+pic->width, y+pic->height);
  glTexCoord2f (gl->sl, gl->th);
  glVertex2f (x, y+pic->height);
  glEnd ();
#endif
  glColor4f (1,1,1,1);
  glEnable(GL_ALPHA_TEST);
  glDisable (GL_BLEND);
}


/*
=============
Draw_Pic
=============
*/
void Draw_Pic (int x, int y, qpic_t *pic)
{
  byte			*dest, *source;
  unsigned short	*pusdest;
  int				v, u;
  glpic_t			*gl;

  if (scrap_dirty)
    Scrap_Upload ();
  glpic_t temp;
  memcpy(&temp, pic->data, sizeof(temp));
  gl = & temp;
  glColor4f (1,1,1,1);
  GL_Bind (gl->texnum);
#ifdef USE_OPENGLES
    DrawQuad(x, y, pic->width, pic->height, gl->sl, gl->tl, gl->sh - gl->sl, gl->th - gl->tl);
#else
  glBegin (GL_QUADS);
  glTexCoord2f (gl->sl, gl->tl);
  glVertex2f (x, y);
  glTexCoord2f (gl->sh, gl->tl);
  glVertex2f (x+pic->width, y);
  glTexCoord2f (gl->sh, gl->th);
  glVertex2f (x+pic->width, y+pic->height);
  glTexCoord2f (gl->sl, gl->th);
  glVertex2f (x, y+pic->height);
  glEnd ();
#endif
}


/*
=============
Draw_TransPic
=============
*/
void Draw_TransPic (int x, int y, qpic_t *pic)
{
  byte	*dest, *source, tbyte;
  unsigned short	*pusdest;
  int				v, u;

  if (x < 0 || (unsigned)(x + pic->width) > vid.width || y < 0 ||
     (unsigned)(y + pic->height) > vid.height)
  {
    Sys_Error ("Draw_TransPic: bad coordinates");
  }

  Draw_Pic (x, y, pic);
}


/*
=============
Draw_TransPicTranslate

Only used for the player color selection menu
=============
*/
void Draw_TransPicTranslate (int x, int y, qpic_t *pic, byte *translation)
{
  int				v, u, c;
  unsigned		trans[64*64], *dest;
  byte			*src;
  int				p;

  GL_Bind (translate_texture);

  c = pic->width * pic->height;

  dest = trans;
  for (v=0 ; v<64 ; v++, dest += 64)
  {
    src = &menuplyr_pixels[ ((v*pic->height)>>6) *pic->width];
    for (u=0 ; u<64 ; u++)
    {
      p = src[(u*pic->width)>>6];
      if (p == 255)
        dest[u] = p;
      else
        dest[u] =  d_8to24table[translation[p]];
    }
  }

  glTexImage2DHelper (GL_TEXTURE_2D, 0, gl_alpha_format, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, trans);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glColor3f (1,1,1);
#ifdef USE_OPENGLES
    DrawQuad(x, y, pic->width, pic->height, 0, 0, 1, 1);
#else
  glBegin (GL_QUADS);
  glTexCoord2f (0, 0);
  glVertex2f (x, y);
  glTexCoord2f (1, 0);
  glVertex2f (x+pic->width, y);
  glTexCoord2f (1, 1);
  glVertex2f (x+pic->width, y+pic->height);
  glTexCoord2f (0, 1);
  glVertex2f (x, y+pic->height);
  glEnd ();
#endif
}


/*
================
Draw_ConsoleBackground

================
*/
void Draw_ConsoleBackground (int lines)
{
  int y = (vid.height * 3) >> 2;

  if (lines > y)
    Draw_Pic(0, lines - vid.height, &conback->qpic);
  else
    Draw_AlphaPic (0, lines - vid.height, conback, (float)(1.2 * lines)/y);
}


/*
=============
Draw_TileClear

This repeats a 64*64 tile graphic to fill the screen around a sized down
refresh window.
=============
*/

typedef union ByteToInt_t {
    byte b[4];
    int i;
} ByteToInt;

void Draw_TileClear (int x, int y, int w, int h)
{
  glColor3f (1,1,1);
  ByteToInt b;
  memcpy(b.b, draw_backtile->data, sizeof(b.b));
  GL_Bind (b.i);
#ifdef USE_OPENGLES
  DrawQuad(x, y, w, h, x/64.0, y/64.0, w/64.0, h/64.0);
#else
  glBegin (GL_QUADS);
  glTexCoord2f (x/64.0, y/64.0);
  glVertex2f (x, y);
  glTexCoord2f ( (x+w)/64.0, y/64.0);
  glVertex2f (x+w, y);
  glTexCoord2f ( (x+w)/64.0, (y+h)/64.0);
  glVertex2f (x+w, y+h);
  glTexCoord2f ( x/64.0, (y+h)/64.0 );
  glVertex2f (x, y+h);
  glEnd ();
#endif
}


/*
=============
Draw_Fill

Fills a box of pixels with a single color
=============
*/
void Draw_Fill (int x, int y, int w, int h, int c)
{
  glDisable (GL_TEXTURE_2D);
  glColor3f (host_basepal[c*3]/255.0,
    host_basepal[c*3+1]/255.0,
    host_basepal[c*3+2]/255.0);

#ifdef USE_OPENGLES
  DrawQuad_NoTex(x, y, w, h);
#else
  glBegin (GL_QUADS);

  glVertex2f (x,y);
  glVertex2f (x+w, y);
  glVertex2f (x+w, y+h);
  glVertex2f (x, y+h);

  glEnd ();
#endif
  glColor3f (1,1,1);
  glEnable (GL_TEXTURE_2D);
}
//=============================================================================

/*
================
Draw_FadeScreen

================
*/
void Draw_FadeScreen (void)
{
  glEnable (GL_BLEND);
  glDisable (GL_TEXTURE_2D);
  glColor4f (0, 0, 0, 0.8);
#ifdef USE_OPENGLES
  DrawQuad_NoTex(0, 0, vid.width, vid.height);
#else
  glBegin (GL_QUADS);

  glVertex2f (0,0);
  glVertex2f (vid.width, 0);
  glVertex2f (vid.width, vid.height);
  glVertex2f (0, vid.height);

  glEnd ();
#endif
  glColor4f (1,1,1,1);
  glEnable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);

  Sbar_Changed();
}

//=============================================================================

/*
================
Draw_BeginDisc

Draws the little blue disc in the corner of the screen.
Call before beginning any disc IO.
================
*/
void Draw_BeginDisc (void)
{
  if (!draw_disc)
    return;
#ifdef USE_OPENGLES
  // !!! Implement this
#else
  glDrawBuffer  (GL_FRONT);
  Draw_Pic (vid.width - 24, 0, draw_disc);
  glDrawBuffer  (GL_BACK);
#endif
}


/*
================
Draw_EndDisc

Erases the disc icon.
Call after completing any disc IO
================
*/
void Draw_EndDisc (void)
{
}

/*
================
GL_Set2D

Setup as if the screen was 320*200
================
*/
void GL_Set2D (void)
{
  glViewport (glx, gly, glwidth, glheight);

  glMatrixMode(GL_PROJECTION);
    glLoadIdentity ();
#ifdef USE_OPENGLES
  glOrthof (0, vid.width, vid.height, 0, -99999, 99999);
 //glOrthof (0, 1280, 800, 0, -99999, 99999);
#else
  glOrtho  (0, vid.width, vid.height, 0, -99999, 99999);
#endif

  glMatrixMode(GL_MODELVIEW);
    glLoadIdentity ();

  glDisable (GL_DEPTH_TEST);
  glDisable (GL_CULL_FACE);
  glDisable (GL_BLEND);
  glEnable (GL_ALPHA_TEST);
//	glDisable (GL_ALPHA_TEST);

  glColor4f (1,1,1,1);
}

//====================================================================

/*
================
GL_FindTexture
================
*/
int GL_FindTexture (const char *identifier)
{
  int		i;
  gltexture_t	*glt;

  for (i=0, glt=gltextures ; i<numgltextures ; i++, glt++)
  {
    if (!strcmp (identifier, glt->identifier))
      return gltextures[i].texnum;
  }

  return -1;
}

/*
================
GL_ResampleTexture
================
*/
void GL_ResampleTexture (unsigned *in, int inwidth, int inheight, unsigned *out,  int outwidth, int outheight)
{
  int		i, j;
  unsigned	*inrow;
  unsigned	frac, fracstep;

  fracstep = inwidth*0x10000/outwidth;
  for (i=0 ; i<outheight ; i++, out += outwidth)
  {
    inrow = in + inwidth*(i*inheight/outheight);
    frac = fracstep >> 1;
    for (j=0 ; j<outwidth ; j+=4)
    {
      out[j] = inrow[frac>>16];
      frac += fracstep;
      out[j+1] = inrow[frac>>16];
      frac += fracstep;
      out[j+2] = inrow[frac>>16];
      frac += fracstep;
      out[j+3] = inrow[frac>>16];
      frac += fracstep;
    }
  }
}

/*
================
GL_Resample8BitTexture -- JACK
================
*/
void GL_Resample8BitTexture (unsigned char *in, int inwidth, int inheight, unsigned char *out,  int outwidth, int outheight)
{
  int		i, j;
  unsigned	char *inrow;
  unsigned	frac, fracstep;

  fracstep = inwidth*0x10000/outwidth;
  for (i=0 ; i<outheight ; i++, out += outwidth)
  {
    inrow = in + inwidth*(i*inheight/outheight);
    frac = fracstep >> 1;
    for (j=0 ; j<outwidth ; j+=4)
    {
      out[j] = inrow[frac>>16];
      frac += fracstep;
      out[j+1] = inrow[frac>>16];
      frac += fracstep;
      out[j+2] = inrow[frac>>16];
      frac += fracstep;
      out[j+3] = inrow[frac>>16];
      frac += fracstep;
    }
  }
}


/*
================
GL_MipMap

Operates in place, quartering the size of the texture
================
*/
void GL_MipMap (byte *in, int width, int height)
{
  int		i, j;
  byte	*out;

  width <<=2;
  height >>= 1;
  out = in;
  for (i=0 ; i<height ; i++, in+=width)
  {
    for (j=0 ; j<width ; j+=8, out+=4, in+=8)
    {
      out[0] = (in[0] + in[4] + in[width+0] + in[width+4])>>2;
      out[1] = (in[1] + in[5] + in[width+1] + in[width+5])>>2;
      out[2] = (in[2] + in[6] + in[width+2] + in[width+6])>>2;
      out[3] = (in[3] + in[7] + in[width+3] + in[width+7])>>2;
    }
  }
}

#ifdef SUPPORT_8BIT_MIPMAPGENERATION
/*
================
GL_MipMap8Bit

Mipping for 8 bit textures

The "in" and "out" arguments can point to the same buffer if desired
================
*/
void GL_MipMap8Bit (byte *in, byte* out, int width, int height)
{
  int		i, j;
  unsigned short     r,g,b;
  byte	*at1, *at2, *at3, *at4;

//	width <<=2;
  height >>= 1;
  for (i=0 ; i<height ; i++, in+=width)
  {
    for (j=0 ; j<width ; j+=2, out+=1, in+=2)
    {
      at1 = (byte *) (d_8to24table + in[0]);
      at2 = (byte *) (d_8to24table + in[1]);
      at3 = (byte *) (d_8to24table + in[width+0]);
      at4 = (byte *) (d_8to24table + in[width+1]);

       r = (at1[0]+at2[0]+at3[0]+at4[0]); r>>=5;
       g = (at1[1]+at2[1]+at3[1]+at4[1]); g>>=5;
       b = (at1[2]+at2[2]+at3[2]+at4[2]); b>>=5;

      out[0] = d_15to8table[(r<<0) + (g<<5) + (b<<10)];
    }
  }
}

#endif // SUPPORT_8BIT_MIPMAPGENERATION

void glTexImage2DHelper( GLenum target,
   GLint level,
   GLint internalformat,
   GLsizei width,
   GLsizei height,
   GLint border,
   GLenum format,
   GLenum type,
   const GLvoid *pixels )
{
  // In full OpenGL The internalformat can be 1..4, to indicate how many components of the data are valid.
  // OpenGL ES requires the internalformat argument match the format for glTexImage2D.

  glTexImage2D(target, level, format, width, height, border, format, type, pixels);
}


// Uncomment to enable manual MipMap generation
#define USE_MANUAL_MIPMAP_GEN

// Uncomment one of the following:

// #define USE_16BPP_WITH_8888_ALPHA
// #define USE_16BPP_WITH_5551_ALPHA // <--- This has bugs on the simulator and the device. (Device has all alpha images invisible.)
#define USE_16BPP_WITH_4444_ALPHA // <--- This has bugs on the simulator, works in device
// #define USE_32BPP
// #define USE_32BPP_MANUAL_MIPMAP_GEN

#ifdef USE_MANUAL_MIPMAP_GEN

inline unsigned int average4(unsigned int a, unsigned int b,
        unsigned int c, unsigned int d,
        unsigned int shift, unsigned int mask) {
    unsigned int aElem = (a >> shift) & mask;
    unsigned int bElem = (b >> shift) & mask;
    unsigned int cElem = (c >> shift) & mask;
    unsigned int dElem = (d >> shift) & mask;
    unsigned int avgElem = ((aElem + bElem + cElem + dElem) >> 2) & mask;
    return avgElem << shift;
}

inline unsigned int average2(unsigned int a, unsigned int b,
        unsigned int shift, unsigned int mask) {
    unsigned int aElem = (a >> shift) & mask;
    unsigned int bElem = (b >> shift) & mask;
    unsigned int avgElem = ((aElem + bElem) >> 1) & mask;
    return avgElem << shift;
}

inline unsigned int average4444(unsigned int a, unsigned int b) {
    return
        average2(a,b,0,0xf) |
        average2(a,b,4,0xf) |
        average2(a,b,8,0xf) |
        average2(a,b,12,0xf);
}

inline unsigned int average565(unsigned int a, unsigned int b) {
    return
        average2(a,b,0,0x1f) |
        average2(a,b,5,0x3f) |
        average2(a,b,11,0x1f);
}

inline unsigned int average2_8888(unsigned int a, unsigned int b) {
    return
        average2(a,b,0,0xff) |
        average2(a,b,8,0xff) |
        average2(a,b,16,0xff) |
        average2(a,b,24,0xff);
}

inline unsigned int average4_8888(unsigned int a, unsigned int b,
        unsigned int c, unsigned int d) {
    return
        average4(a,b,c,d,0,0xff) |
        average4(a,b,c,d,8,0xff) |
        average4(a,b,c,d,16,0xff) |
        average4(a,b,c,d,24,0xff);
}

#endif

// pData is 8 bpp 32-bit color


void sendTexture(int mipLevel, int width, int height, unsigned int* pData, qboolean alpha) {
    if (alpha) {
#if defined(USE_16BPP_WITH_8888_ALPHA)
        // 8888
        glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
#elif defined(USE_16BPP_WITH_5551_ALPHA)
        // 5551
        glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, 0);
        glTexSubImage2D(GL_TEXTURE_2D, mipLevel, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pData);
#else

        // 4444
        //glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, 0);
        glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        //Con_Printf("glTexSubImage2D mipLevel =%d, w=%d h=%d pData = %p",mipLevel,width,height,pData);

       // glTexSubImage2D(GL_TEXTURE_2D, mipLevel, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pData);
       glTexSubImage2D(GL_TEXTURE_2D, mipLevel, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pData);

#endif
    }
    else {

#if 0
        // 8888
        glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
#else
        // 565
        static	unsigned short scaled[1024*512];	// [512*256];
        glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 0);
        // Some OpenGL ES implementations do not have to be able to convert from GL_RGBA to GL_RGB format, so
        // we must do it manually here:
        unsigned char* pSrc = (unsigned char*) pData;
        unsigned short* pDest = scaled;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                *pDest++ = ((pSrc[0] >> 3) << 11) |
                    ((pSrc[1] >> 2) << 5) |
                    (pSrc[2] >> 3);
                pSrc += 4;
            }
        }
        //Con_Printf("glTexSubImage2D No alpha mipLevel =%d, w=%d h=%d pData = %p",mipLevel,width,height,pData);
        //glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 0);
        glTexSubImage2D(GL_TEXTURE_2D, mipLevel, 0, 0, width, height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, scaled);
#endif
    }
}

/*
===============
GL_Upload32
===============
*/
void GL_Upload32 (unsigned *data, int width, int height,  qboolean mipmap, qboolean alpha)
{
  int			samples;
  int			scaled_width, scaled_height;
  static	unsigned	scaled[1024*512];	// [512*256];

  for (scaled_width = 1 ; scaled_width < width ; scaled_width<<=1)
    ;
  for (scaled_height = 1 ; scaled_height < height ; scaled_height<<=1)
    ;

  scaled_width >>= (int)gl_picmip.value;
  scaled_height >>= (int)gl_picmip.value;

  if (scaled_width > gl_max_size.value)
    scaled_width = (int) gl_max_size.value;
  if (scaled_height > gl_max_size.value)
    scaled_height = (int) gl_max_size.value;

  if (scaled_width * scaled_height > (int) sizeof(scaled)/4)
    Sys_Error ("GL_LoadTexture: too big");

  samples = alpha ? gl_alpha_format : gl_solid_format;

    texels += scaled_width * scaled_height;

  if (scaled_width == width && scaled_height == height)
  {
#if 0 // Disable this optimization, we want to be able to easily switch texture formats
    if (!mipmap)
    {
      glTexImage2DHelper (GL_TEXTURE_2D, 0, samples, scaled_width, scaled_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      goto done;
    }
#endif
    memcpy (scaled, data, width*height*4);
  }
  else
    GL_ResampleTexture (data, width, height, scaled, scaled_width, scaled_height);

#if defined(USE_16BPP_WITH_8888_ALPHA) || defined(USE_16BPP_WITH_5551_ALPHA) || defined(USE_16BPP_WITH_4444_ALPHA)
  // Upload as 16 bpp

#ifdef USE_MANUAL_MIPMAP_GEN
#else
  // Use automatic MIPMAP generation
  if (mipmap)
  {
    glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, 1);
  }
#endif

  sendTexture(0, scaled_width, scaled_height, scaled, alpha);

#ifdef USE_MANUAL_MIPMAP_GEN
  if (mipmap) {
      // Compute mip levels
      int mipWidth = scaled_width;
      int mipHeight = scaled_height;
      int mipLevel = 1;
      while (mipWidth > 1 || mipHeight > 1) {
          if (mipWidth > 1 && mipHeight > 1) {
              // Scale horizontally and vertically
              int srcWidth = mipWidth;
                mipWidth >>= 1;
                mipHeight >>= 1;
                const unsigned int* pIn = (const unsigned int*) scaled;
                unsigned int* pOut = (unsigned int*) scaled;
                for(int y = 0; y < mipHeight; y++) {
                    for (int x = 0; x < mipWidth; x++) {
                        *pOut++ = average4_8888(pIn[0], pIn[1],
                                pIn[srcWidth], pIn[srcWidth+1]);
                        pIn += 2;
                    }
                    pIn += srcWidth;
                }
          }
          else {
              // Scale horizontally:
              if (mipWidth > 1) {
                  mipWidth >>= 1;
                  const unsigned int* pIn = (const unsigned int*) scaled;
                  unsigned int* pOut = (unsigned int*) scaled;
                  unsigned int numTexels = mipHeight * mipWidth;
                  for(unsigned int i = 0; i < numTexels; i++) {
                      *pOut++ = average2_8888(pIn[0], pIn[1]);
                      pIn += 2;
                  }
              }
              // Scale vertically:
              if (mipHeight > 1) {
                  mipHeight >>= 1;
                  const unsigned int* pIn = (const unsigned int*) scaled;
                  unsigned int* pOut = (unsigned int*) scaled;
                  for(int y = 0; y < mipHeight; y++) {
                      for (int x = 0; x < mipWidth; x++) {
                          *pOut++ = average2_8888(pIn[0], pIn[mipWidth]);
                          pIn += 1;
                      }
                      pIn += mipWidth;
                  }
              }
          }

          sendTexture(mipLevel, mipWidth, mipHeight, scaled, alpha);
          mipLevel++;
      }
  }

#else
  if (mipmap)
  {
    glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, 0);
  }
#endif

#elif defined(USE_32BPP)
  // 8888
  // Use automatic MIPMAP generation
  if (mipmap)
  {
    glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, 1);
  }
  glTexImage2DHelper (GL_TEXTURE_2D, 0, samples, scaled_width, scaled_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scaled);
  if (mipmap)
  {
    glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, 0);
  }
#else
  glTexImage2DHelper (GL_TEXTURE_2D, 0, samples, scaled_width, scaled_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scaled);
  if (mipmap)
  {
    int		miplevel;

    miplevel = 0;
    while (scaled_width > 1 || scaled_height > 1)
    {
      GL_MipMap ((byte *)scaled, scaled_width, scaled_height);
      scaled_width >>= 1;
      scaled_height >>= 1;
      if (scaled_width < 1)
        scaled_width = 1;
      if (scaled_height < 1)
        scaled_height = 1;
      miplevel++;
      glTexImage2DHelper (GL_TEXTURE_2D, miplevel, samples, scaled_width, scaled_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scaled);
    }
  }
#endif
done: ;

  if (mipmap)
  {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_min);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
  }
  else
  {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_max);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
  }
}

#ifdef USE_OPENGLES

void GL_Upload8_EXT (byte *data, int width, int height,  qboolean mipmap, qboolean alpha)
{
  int			i, s, bytesUsed;
  qboolean	noalpha;
  int			p;
  static unsigned j;
    static	unsigned char compressedTextureBuffer[1024*512];	// [512*256];
  unsigned char* pTex = compressedTextureBuffer;
  int			scaled_width, scaled_height;
  int miplevel = 0;

  int originalScaledWidth;
  int originalScaledHeight;

  s = width*height;
  // if there are no transparent pixels, make it a 3 component
  // texture even if it was specified as otherwise
  if (alpha)
  {
    noalpha = true;
    for (i=0 ; i<s ; i++)
    {
      if (data[i] == 255)
        noalpha = false;
    }

    if (alpha && noalpha)
      alpha = false;
  }
  for (scaled_width = 1 ; scaled_width < width ; scaled_width<<=1)
    ;
  for (scaled_height = 1 ; scaled_height < height ; scaled_height<<=1)
    ;

  scaled_width >>= (int)gl_picmip.value;
  scaled_height >>= (int)gl_picmip.value;

  if (scaled_width > gl_max_size.value)
    scaled_width = (int) gl_max_size.value;
  if (scaled_height > gl_max_size.value)
    scaled_height = (int) gl_max_size.value;

  if (scaled_width * scaled_height > ((int) (sizeof(compressedTextureBuffer) * 3 / 4)))
    Sys_Error ("GL_LoadTexture: too big");

  // Copy the palette

  int entrySize = alpha ? 4 : 3;
  int paletteSize = entrySize * 256;
  {
    byte* pDest = compressedTextureBuffer;
    const byte* pSrc = host_basepal;
    if(alpha)
    {
      for(int i = 0; i< 255; i++)
      {
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = 0xff;
      }
      // Entry 255 is transparent
      *pDest++ = 0x00;
      *pDest++ = 0x00;
      *pDest++ = 0x00;
      *pDest++ = 0x00;
    }
    else
    {
      memcpy(pDest, pSrc, paletteSize);
    }
  }

  bytesUsed = paletteSize;
  pTex += paletteSize;

  texels += scaled_width * scaled_height;

  if (scaled_width == width && scaled_height == height)
  {
    memcpy (pTex, data, scaled_width*scaled_height);
  }
  else
    GL_Resample8BitTexture (data, width, height, pTex, scaled_width, scaled_height);

  bytesUsed += scaled_width * scaled_height;

  miplevel = 0;

  originalScaledWidth = scaled_width;
  originalScaledHeight = scaled_height;

  if (mipmap)
  {
#ifdef SUPPORT_8BIT_MIPMAPGENERATION
        miplevel = 1;
    while (scaled_width > 1 || scaled_height > 1)
    {
      byte* pDest = (byte*) pTex + scaled_width * scaled_height;
      GL_MipMap8Bit ((byte *)pTex, pDest, scaled_width, scaled_height);
      pTex = pDest;
      scaled_width >>= 1;
      scaled_height >>= 1;
      if (scaled_width < 1)
        scaled_width = 1;
      if (scaled_height < 1)
        scaled_height = 1;
      bytesUsed += scaled_width * scaled_height;
      miplevel++;
    }
#else
  Sys_Error("Unsupported attempt to generate 8 bit mip mapped texture. #define SUPPORT_8BIT_MIPMAPGENERATION");
#endif
  }

  GLint internalFormat = alpha ? GL_PALETTE8_RGBA8_OES : GL_PALETTE8_RGB8_OES;
  glCompressedTexImage2D (GL_TEXTURE_2D, -miplevel, internalFormat,
            originalScaledWidth, originalScaledHeight,
            0, bytesUsed, compressedTextureBuffer);

  if (mipmap)
  {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_min);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
  }
  else
  {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_max);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
  }
}

#else

void GL_Upload8_EXT (byte *data, int width, int height,  qboolean mipmap, qboolean alpha)
{
  int			i, s;
  qboolean	noalpha;
  int			p;
  static unsigned j;
  int			samples;
    static	unsigned char scaled[1024*512];	// [512*256];
  int			scaled_width, scaled_height;

  s = width*height;
  // if there are no transparent pixels, make it a 3 component
  // texture even if it was specified as otherwise
  if (alpha)
  {
    noalpha = true;
    for (i=0 ; i<s ; i++)
    {
      if (data[i] == 255)
        noalpha = false;
    }

    if (alpha && noalpha)
      alpha = false;
  }
  for (scaled_width = 1 ; scaled_width < width ; scaled_width<<=1)
    ;
  for (scaled_height = 1 ; scaled_height < height ; scaled_height<<=1)
    ;

  scaled_width >>= (int)gl_picmip.value;
  scaled_height >>= (int)gl_picmip.value;

  if (scaled_width > gl_max_size.value)
    scaled_width = gl_max_size.value;
  if (scaled_height > gl_max_size.value)
    scaled_height = gl_max_size.value;

  if (scaled_width * scaled_height > (int) sizeof(scaled))
    Sys_Error ("GL_LoadTexture: too big");

  samples = 1; // alpha ? gl_alpha_format : gl_solid_format;

  texels += scaled_width * scaled_height;

  if (scaled_width == width && scaled_height == height)
  {
    if (!mipmap)
    {
      glTexImage2D (GL_TEXTURE_2D, 0, GL_COLOR_INDEX8_EXT, scaled_width, scaled_height, 0, GL_COLOR_INDEX , GL_UNSIGNED_BYTE, data);
      goto done;
    }
    memcpy (scaled, data, width*height);
  }
  else
    GL_Resample8BitTexture (data, width, height, scaled, scaled_width, scaled_height);

  glCompressedTexImage2D (GL_TEXTURE_2D, 0, GL_PALETTE8_RGB8_OES, scaled_width, scaled_height, 0, s, scaled);
  if (mipmap)
  {
#ifdef SUPPORT_8BIT_MIPMAPGENERATION
    int		miplevel;

    miplevel = 0;
    while (scaled_width > 1 || scaled_height > 1)
    {
      GL_MipMap8Bit ((byte *)scaled, (byte*) scaled, scaled_width, scaled_height);
      scaled_width >>= 1;
      scaled_height >>= 1;
      if (scaled_width < 1)
        scaled_width = 1;
      if (scaled_height < 1)
        scaled_height = 1;
      miplevel++;
      glTexImage2D (GL_TEXTURE_2D, miplevel, GL_COLOR_INDEX8_EXT, scaled_width, scaled_height, 0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, scaled);
    }
#else
  Sys_Error("Unsupported attept to generate 8 bit mip mapped texture.");
#endif
  }
done: ;


  if (mipmap)
  {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_min);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
  }
  else
  {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_max);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
  }
}

#endif // ! OPENGL_ES

/*
===============
GL_Upload8
===============
*/
void GL_Upload8 (byte *data, int width, int height,  qboolean mipmap, qboolean alpha)
{
static	unsigned	trans[640*480];		// FIXME, temporary
  int			i, s;
  qboolean	noalpha;
  int			p;

  s = width*height;
  // if there are no transparent pixels, make it a 3 component
  // texture even if it was specified as otherwise
  if (alpha)
  {
    noalpha = true;
    for (i=0 ; i<s ; i++)
    {
      p = data[i];
      if (p == 255)
        noalpha = false;
      trans[i] = d_8to24table[p];
    }

    if (alpha && noalpha)
      alpha = false;
  }
  else
  {
    if (s&3)
      Sys_Error ("GL_Upload8: s&3");
    for (i=0 ; i<s ; i+=4)
    {
      trans[i] = d_8to24table[data[i]];
      trans[i+1] = d_8to24table[data[i+1]];
      trans[i+2] = d_8to24table[data[i+2]];
      trans[i+3] = d_8to24table[data[i+3]];
    }
  }

   if (VID_Is8bit() && (data!=scrap_texels[0])
#if !defined(USE_OPENGLES)
    && !alpha
#endif
  ) {
     GL_Upload8_EXT (data, width, height, mipmap, alpha);
     return;
  }

  GL_Upload32 (trans, width, height, mipmap, alpha);

}

/*
================
GL_LoadTexture
================
*/
int GL_LoadTexture (const char *identifier, int width, int height, byte *data, qboolean mipmap, qboolean alpha)
{
  qboolean	noalpha;
  int			i, p, s;
  gltexture_t	*glt;

  // see if the texture is allready present
  if (identifier[0])
  {
    for (i=0, glt=gltextures ; i<numgltextures ; i++, glt++)
    {
      if (!strcmp (identifier, glt->identifier))
      {
        if (width != glt->width || height != glt->height)
          Sys_Error ("GL_LoadTexture: cache mismatch");
        return gltextures[i].texnum;
      }
    }
#ifdef USE_OPENGLES
    // Surely we want to remember this new texture.
    // Doing this costs 1% fps per timedemo on a DX7 PC,
    // probably because of the linear search through the
    // texture cache, but it saves 10 MB of VM growth per
    // level load. It also makes the GL_TEXTUREMODE
    // console command work correctly.
    numgltextures++;
#endif
  }
  else {
    glt = &gltextures[numgltextures];
    numgltextures++;
  }

  strcpy (glt->identifier, identifier);
  glt->texnum = texture_extension_number;
  glt->width = width;
  glt->height = height;
  glt->mipmap = mipmap;

    GL_Bind(texture_extension_number);

#ifdef USE_TEXTURE_STORE

  textureStore::get()->create(width, height, data, mipmap, alpha);

#else

    GL_Upload8 (data, width, height, mipmap, alpha);

#endif

  texture_extension_number++;
  return texture_extension_number-1;
}


/****************************************/

static GLenum oldtarget = TEXTURE0_SGIS;

void GL_SelectTexture (GLenum target)
{
  if (!gl_mtexable)
    return;
#ifdef USE_OPENGLES
  glActiveTexture(target);
#else
  qglSelectTextureSGIS(target);
#endif
  if (target == oldtarget)
    return;
  cnttextures[oldtarget-TEXTURE0_SGIS] = currenttexture;
  currenttexture = cnttextures[target-TEXTURE0_SGIS];
  oldtarget = target;
}

// OpenGL ES compatible DrawQuad utility

#define BEGIN_QUAD glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
#define END_QUAD glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

void DrawQuad_NoTex(float x, float y, float w, float h)
{
  BEGIN_QUAD

  float vertex[2*4] = {x,y,x+w,y, x+w, y+h, x, y+h};
  short index[4] = {0, 1, 2, 3};
  glVertexPointer( 2, GL_FLOAT, 0, vertex);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, index);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  END_QUAD
}

void DrawQuad(float x, float y, float w, float h, float u, float v, float uw, float vh)
{
  BEGIN_QUAD

    float texcoord[2*4] = {u, v, u + uw, v, u + uw, v + vh, u, v + vh};
  float vertex[2*4] = {x,y,x+w,y, x+w, y+h, x, y+h};
  unsigned short index[4] = {0, 1, 2, 3};
  glTexCoordPointer( 2, GL_FLOAT, 0, texcoord);
  glVertexPointer( 2, GL_FLOAT, 0, vertex);
  glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, index);

  END_QUAD
}

#ifdef USE_OPENGLES

// Reimplementation of OpenGL functions that are missing in OpenGL ES

void glColor3f(GLfloat r, GLfloat g, GLfloat b)
{
  glColor4f(r, g, b, 1.0f);
}

void glColor4fv(GLfloat* pColor)
{
  glColor4f(pColor[0], pColor[1], pColor[2], pColor[3]);
}

float gVertexBuffer[VERTEXARRAYSIZE];
float gColorBuffer[VERTEXARRAYSIZE];
float gTexCoordBuffer[VERTEXARRAYSIZE];

// Called when we've lost the OpenGL context and have to recreate it.
extern void GL_Init();
extern void R_InitParticleTexture2();
extern void GL_UploadLightmaps();
extern void R_ReloadSky();

void GL_ReInit() {
  GL_Init();
#ifdef USE_TEXTURE_STORE
  textureStore::get()->rebindAll();
#endif
  scrap_dirty = true;
  R_InitParticleTexture2();
  GL_UploadLightmaps();
  R_ReloadSky();
}

#endif

#ifdef DEBUG_OPENGL_CALLS
void checkGLImp(const char* state, const char* file, int line) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
    	Sys_Error("%s: error 0x%04X at %s:%d\n", state, error, file, line);
    }
}

#endif
