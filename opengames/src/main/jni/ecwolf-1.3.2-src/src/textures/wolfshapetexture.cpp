/*
** wolfshapetexture.cpp
** Wolfenstein "Shape" support, somewhat similar to a doom patch.
**
**---------------------------------------------------------------------------
** Copyright 2011 Braden Obrzut
** Copyright 2004-2006 Randy Heit
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
**
*/

#include "wl_def.h"
#include "files.h"
#include "w_wad.h"
#include "m_swap.h"
#include "templates.h"
#include "v_palette.h"
#include "textures.h"
#include "lumpremap.h"

//==========================================================================
//
// A texture that is a Wolfenstein "shape"
//
//==========================================================================

class FWolfShapeTexture : public FTexture
{
public:
	FWolfShapeTexture (int lumpnum, FileReader &file);
	~FWolfShapeTexture ();

	const BYTE *GetColumn (unsigned int column, const Span **spans_out);
	const BYTE *GetPixels ();
	void Unload ();

protected:
	BYTE *Pixels;
	Span **Spans;
	int TopCrop;


	virtual void MakeTexture ();
};

//==========================================================================
//
// Checks if the lump is a Wolfenstein "shape"
//
//==========================================================================

static bool CheckIfWolfShape(FileReader &file)
{
	if(file.GetLength() < 4) return false; // No header
	
	WORD header[2];
	file.Seek(0, SEEK_SET);
	file.Read(header, 4);

	WORD Left = LittleShort(header[0]);
	WORD Right = LittleShort(header[1]);

	WORD Width = Right-Left;
	if(Width <= 0 || Width > 256 || file.GetLength() < 4+Width*2)
		return false;

	WORD* offsets = new WORD[Width];
	file.Read(offsets, Width*2);
	for(int i = 0;i < Width;i++)
	{
		if(LittleShort(offsets[i]) >= file.GetLength())
		{
			delete[] offsets;
			return false;
		}
	}
	delete[] offsets;
	return true;
}

//==========================================================================
//
//
//
//==========================================================================

FTexture *WolfShapeTexture_TryCreate(FileReader &file, int lumpnum)
{
	if(!CheckIfWolfShape(file))
		return NULL;
	return new FWolfShapeTexture(lumpnum, file);
}

//==========================================================================
//
//
//
//==========================================================================

FWolfShapeTexture::FWolfShapeTexture(int lumpnum, FileReader &file)
: FTexture(NULL, lumpnum), Pixels(0), Spans(0)
{
	// left, right, offsets...
	WORD header[2];
	file.Seek(0, SEEK_SET);
	file.Read(header, 4);
	header[0] = LittleShort(header[0]);
	header[1] = LittleShort(header[1]);

	Width = header[1]-header[0]+1;
	Height = 64;
	LeftOffset = 32-header[0];
	TopOffset = 64;
	if(LumpRemapper::IsPSprite(lumpnum))
	{
		// Magic numbers!!!
		// Set the offset of this sprite such that it would match what it would
		// be for on a Doom player sprite.
		// Also scale it up 2.5 times, which is about what is needed to emulate
		// the size of vanilla wolf within precision limits.
		TopOffset = 4;
		LeftOffset -= 64;
		xScale = 2*FRACUNIT/5;
		yScale = 2*FRACUNIT/5;
	}

	// Crop the height!
	int minStart = 64;
	int maxEnd = 0;
	FMemLump lump = Wads.ReadLump (lumpnum);
	const BYTE* data = (const BYTE*)lump.GetMem();
	for(int x = 0;x < Width;x++)
	{
		const BYTE* column = data+ReadLittleShort(&data[4+x*2]);
		int start, end;
		while((end = ReadLittleShort(column)) != 0)
		{
			end >>= 1;
			start = ReadLittleShort(column+4)>>1;
			if(start < minStart)
				minStart = start;
			if(end > maxEnd)
				maxEnd = end;
			column += 6;
		}
	}

	TopCrop = minStart;
	Height = maxEnd-minStart;
	TopOffset -= minStart;

	CalcBitSize ();
}

//==========================================================================
//
//
//
//==========================================================================

FWolfShapeTexture::~FWolfShapeTexture ()
{
	Unload ();
	if (Spans != NULL)
	{
		FreeSpans (Spans);
		Spans = NULL;
	}
}

//==========================================================================
//
//
//
//==========================================================================

void FWolfShapeTexture::Unload ()
{
	if(Pixels != NULL)
	{
		delete[] Pixels;
		Pixels = NULL;
	}
}

//==========================================================================
//
//
//
//==========================================================================

const BYTE *FWolfShapeTexture::GetPixels ()
{
	if (Pixels == NULL)
	{
		MakeTexture ();
	}
	return Pixels;
}

//==========================================================================
//
//
//
//==========================================================================

const BYTE *FWolfShapeTexture::GetColumn (unsigned int column, const Span **spans_out)
{
	if (Pixels == NULL)
	{
		MakeTexture ();
	}
	if ((unsigned)column >= (unsigned)Width)
	{
		if (WidthMask + 1 == Width)
		{
			column &= WidthMask;
		}
		else
		{
			column %= Width;
		}
	}
	if (spans_out != NULL)
	{
		if (Spans == NULL)
		{
			Spans = CreateSpans(Pixels);
		}
		*spans_out = Spans[column];
	}
	return Pixels + column*Height;
}


//==========================================================================
//
//
//
//==========================================================================

void FWolfShapeTexture::MakeTexture ()
{
	FMemLump lump = Wads.ReadLump (SourceLump);
	const BYTE* data = (const BYTE*)lump.GetMem();

	Pixels = new BYTE[Width*Height];
	memset(Pixels, 0, Width*Height);

	for(int x = 0;x < Width;x++)
	{
		BYTE* out = Pixels+(x*Height);
		const BYTE* column = data+ReadLittleShort(&data[4+x*2]);
		int start, end;
		while((end = ReadLittleShort(column)) != 0)
		{
			end = (end>>1) - TopCrop;
			const BYTE* in = data+int16_t(ReadLittleShort(column+2))+TopCrop;
			start = (ReadLittleShort(column+4)>>1) - TopCrop;
			column += 6;
			for(int y = start;y < end;y++)
				out[y] = GPalette.Remap[in[y]];
		}
	}
}
