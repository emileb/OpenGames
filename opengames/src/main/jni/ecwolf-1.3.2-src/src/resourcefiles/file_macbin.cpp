/*
** file_macbin.cpp
**
**---------------------------------------------------------------------------
** Copyright 2013 Braden Obrzut
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

#include "resourcefile.h"
#include "w_wad.h"
#include "zstring.h"

#pragma pack(push, 1)
struct FMacBinHeader
{
public:
	BYTE version;
	BYTE filenameLength;
	char filename[63];
	char fileType[4];
	char fileCreator[4];
	BYTE finderFlags;
	BYTE pad0;
	WORD verticalPosition;
	WORD horizontalPosition;
	WORD folderID;
	BYTE protectedFlag;
	BYTE pad1;
	DWORD dataForkLength;
	DWORD resourceForkLength;
	DWORD creationDate;
	DWORD modifiedDate;
	char pad2[27];
	WORD reserved;
};

struct FResHeader
{
public:
	DWORD resourceOffset;
	DWORD mapOffset;
	DWORD resourceLength;
	DWORD mapLength;
};

struct FMapHeader
{
public:
	// FResHeader copy;
	DWORD reservedHandle;
	WORD referenceNumber;
	WORD forkAttributes;
	WORD typeListOffset;
	WORD nameListOffset;
};

struct FResType
{
public:
	char type[4];
	WORD numResources;
	WORD offset;
};

struct FResReference
{
public:
	// We need to have this structure in two parts because...
	struct RefData
	{
		WORD resID;
		WORD nameOffset;
		BYTE attributes;
		BYTE hiDataOffset; // Yay a 24-bit type!
		WORD loDataOffset;
		DWORD reservedHandle;
	} ref;

	// Stuff we're going to calculate later
	DWORD dataOffset;
	FString name;
};
#pragma pack(pop)

class FMacBin : public FResourceFile
{
	private:
		FUncompressedLump *Lumps;

	public:
		FMacBin(const char* filename, FileReader *file) : FResourceFile(filename, file), Lumps(NULL)
		{
		}
		~FMacBin()
		{
			delete[] Lumps;
		}

		FResourceLump *GetLump(int index) { return &Lumps[index]; }

		bool Open(bool quiet)
		{
			// Kill data types we definitely don't care about.
			static const char* const IgnoreTypes[] = {
				"ALRT", "BNDL", "cicn", "DITL", "DLOG",
				"FREF", "hfdr", "icl4", "icl8", "ICN#",
				"ics#", "ics4", "ics8", "MBAR", "MDRV",
				"mstr", "MENU", "PICT", "STR#", "TEXT",
				"vers", "WDEF", "hmnu", "SMOD", "CODE",
				"XREF", "DATA", "SIZE", "cfrg",
				NULL
			};

			FMacBinHeader header;
			FResHeader resHeader;
			FMapHeader mapHeader;
			FResType* resTypes = NULL;
			FResReference* refs = NULL;
			DWORD resourceForkOffset;
			DWORD resTypeListOffset;
			WORD numTypes;

			// Read MacBin version 0 header
			Reader->Read(&header, sizeof(FMacBinHeader));
			header.dataForkLength = BigLong(header.dataForkLength);
			header.resourceForkLength = BigLong(header.resourceForkLength);
			resourceForkOffset = sizeof(FMacBinHeader) + ((header.dataForkLength+127)&(~0x7F));
			if(header.version != 0)
				return false;

			// We only care about the resource fork so seek to it and read the header
			Reader->Seek(resourceForkOffset, SEEK_SET);
			Reader->Read(&resHeader, sizeof(FResHeader));
			resHeader.resourceOffset = BigLong(resHeader.resourceOffset);
			resHeader.mapOffset = BigLong(resHeader.mapOffset);
			resHeader.resourceLength = BigLong(resHeader.resourceLength);
			resHeader.mapLength = BigLong(resHeader.mapLength);

			// Next we need to read the resource map header
			Reader->Seek(resourceForkOffset + resHeader.mapOffset + sizeof(FResHeader), SEEK_SET);
			Reader->Read(&mapHeader, sizeof(FMapHeader));
			mapHeader.typeListOffset = BigShort(mapHeader.typeListOffset);
			mapHeader.nameListOffset = BigShort(mapHeader.nameListOffset);
			resTypeListOffset = resourceForkOffset + resHeader.mapOffset + mapHeader.typeListOffset;

			// Get the types and the number of chunks within each
			unsigned int numTotalResources = 0;
			Reader->Seek(resTypeListOffset, SEEK_SET);
			*Reader >> numTypes;
			numTypes = BigShort(numTypes)+1;
			resTypes = new FResType[numTypes];
			for(unsigned int i = 0;i < numTypes;++i)
			{
				Reader->Read(&resTypes[i], sizeof(FResType));
				resTypes[i].numResources = BigShort(resTypes[i].numResources)+1;
				resTypes[i].offset = BigShort(resTypes[i].offset);

				// Discard types we want to ignore.
				bool doIgnore = false;
				for(const char* const * ignType = IgnoreTypes;*ignType != NULL;++ignType)
				{
					if(strncmp(*ignType, resTypes[i].type, 4) == 0)
					{
						--i;
						--numTypes;
						doIgnore = true;
						break;
					}
				}
				if(doIgnore)
					continue;

				numTotalResources += resTypes[i].numResources;
			}

			// Read in the refs
			refs = new FResReference[numTotalResources];
			{
				FResReference *refPtr = refs;
				for(unsigned int i = 0;i < numTypes;++i)
				{
					Reader->Seek(resTypes[i].offset + resTypeListOffset, SEEK_SET);
					for(unsigned int j = 0;j < resTypes[i].numResources;++j)
					{
						Reader->Read(refPtr, sizeof(FResReference::RefData));
						refPtr->ref.resID = BigShort(refPtr->ref.resID);
						refPtr->ref.nameOffset = BigShort(refPtr->ref.nameOffset);
						refPtr->dataOffset = (refPtr->ref.hiDataOffset<<16)|BigShort(refPtr->ref.loDataOffset);

						++refPtr;
					}
				}
			}

			// TODO: Do we need to bother with this?
			// Get the names
			{
				DWORD nameListOffset = resourceForkOffset + resHeader.mapOffset + mapHeader.nameListOffset;
				FResReference *refPtr = refs;
				char resNamesBuffer[256];
				for(unsigned int i = 0;i < numTotalResources;++i, ++refPtr)
				{
					if(refPtr->ref.nameOffset == 0xFFFF)
						continue;

					Reader->Seek(nameListOffset + refPtr->ref.nameOffset, SEEK_SET);
					BYTE length;
					*Reader >> length;
					Reader->Read(resNamesBuffer, length);
					resNamesBuffer[length] = 0;

					refPtr->name = resNamesBuffer;
				}
			}

			// Now we can finally load the lumps, assigning temporary names
			NumLumps = numTotalResources;
			Lumps = new FUncompressedLump[NumLumps];
			{
				FUncompressedLump *lump = Lumps;
				FResReference *refPtr = refs;
				for(unsigned int i = 0;i < numTypes;++i)
				{
					for(unsigned int j = 0;j < resTypes[i].numResources;++j, ++refPtr, ++lump)
					{
						char name[9];
						DWORD length;
						lump->Position = resourceForkOffset + refPtr->dataOffset + resHeader.resourceOffset + 4;
						lump->Namespace = ns_global;
						lump->Owner = this;
						lump->LumpNameSetup(name);

						Reader->Seek(lump->Position - 4, SEEK_SET);
						*Reader >> length;
						lump->LumpSize = BigLong(length);
					}
				}
			}

			delete[] refs;
			delete[] resTypes;
			if(!quiet) Printf(", %d lumps\n", NumLumps);

			return true;
		}
};

FResourceFile *CheckMacBin(const char *filename, FileReader *file, bool quiet)
{
	if(file->GetLength() > 128)
	{
		char type[8];
		unsigned int sizes[2];
		file->Seek(65, SEEK_SET);
		file->Read(type, 8);
		file->Seek(83, SEEK_SET);
		file->Read(&sizes, 8);
		file->Seek(0, SEEK_SET);

		if((strncmp(type, "APPLWOLF", 8) == 0 || strncmp(type, "MAPSWOLF", 8) == 0) &&
			BigLong(sizes[0]) + BigLong(sizes[1]) < static_cast<unsigned>(file->GetLength()))
		{
			FResourceFile *rf = new FMacBin(filename, file);
			if(rf->Open(quiet)) return rf;
			rf->Reader = NULL; // to avoid destruction of reader
			delete rf;
		}
	}
	return NULL;
}