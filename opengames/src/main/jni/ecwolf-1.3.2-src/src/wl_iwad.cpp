/*
** wl_iwad.cpp
**
**---------------------------------------------------------------------------
** Copyright 2012 Braden Obrzut
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

#include "resourcefiles/resourcefile.h"
#include "config.h"
#include "filesys.h"
#include "lumpremap.h"
#include "scanner.h"
#include "tarray.h"
#include "version.h"
#include "w_wad.h"
#include "wl_iwad.h"
#include "zstring.h"

bool queryiwad = true;

int I_PickIWad(WadStuff *wads, int numwads, bool showwin, int defaultiwad);
#ifndef _WIN32
#include "wl_iwad_picker.cpp"
#endif

namespace IWad {

#if defined(_WIN32) || defined(__APPLE__) || defined(__ANDROID__)
#define ISCASEINSENSITIVE 1
#else
#define ISCASEINSENSITIVE 0
#endif

static TArray<IWadData> iwadTypes;
static const IWadData *selectedGame;
static unsigned int NumIWads;

static bool SplitFilename(const FString &filename, FString &name, FString &extension)
{
	long extensionSep = filename.LastIndexOf('.');
	if(extensionSep == -1 || filename.Len() < 3)
		return false;
	name = filename.Left(extensionSep);
	extension = filename.Mid(extensionSep+1);
	return true;
}

// Identifies the IWAD by examining the lumps against the possible requirements.
// Returns -1 if it isn't identifiable.
static int CheckData(WadStuff &wad)
{
	unsigned int* valid = new unsigned int[iwadTypes.Size()];
	memset(valid, 0, sizeof(unsigned int)*iwadTypes.Size());

	for(unsigned int i = 0;i < wad.Path.Size();++i)
	{
		FResourceFile *file = FResourceFile::OpenResourceFile(wad.Path[i], NULL, true);
		if(file)
		{
			LumpRemapper::RemapAll(); // Fix lump names if needed
			for(unsigned int j = file->LumpCount();j-- > 0;)
			{
				FResourceLump *lump = file->GetLump(j);

				for(unsigned int k = 0;k < iwadTypes.Size();++k)
				{
					for(unsigned int l = iwadTypes[k].Ident.Size();l-- > 0;)
					{
						if(iwadTypes[k].Ident[l].CompareNoCase(lump->Name) == 0 ||
							(lump->FullName && (strnicmp(lump->FullName, "maps/", 5) == 0 &&
							iwadTypes[k].Ident[l].CompareNoCase(FString(lump->FullName+5, strcspn(lump->FullName+5, "."))))))
						{
							valid[k] |= 1<<l;
						}
					}
				}
			}
			delete file;
		}
	}

	wad.Type = -1;
	for(unsigned int i = 0;i < iwadTypes.Size();++i)
	{
		if(static_cast<unsigned>((1<<iwadTypes[i].Ident.Size())-1) == valid[i])
		{
			wad.Name = iwadTypes[i].Name;
			wad.Type = i;
			break;
		}
	}
	delete[] valid;
	return wad.Type;
}

bool CheckGameFilter(FName filter)
{
	return selectedGame->Game == filter;
}

const IWadData &GetGame()
{
	return *selectedGame;
}

unsigned int GetNumIWads()
{
	return NumIWads;
}

enum
{
	FILE_AUDIOHED,
	FILE_AUDIOT,
	FILE_GAMEMAPS,
	FILE_MAPHEAD,
	FILE_VGADICT,
	FILE_VGAHEAD,
	FILE_VGAGRAPH,
	FILE_VSWAP,

	BASEFILES
};
struct BaseFile
{
	FString	extension;
	FString	filename[BASEFILES];
	BYTE	isValid;
};
/* Steam ships Spear of Destiny in the mission pack 3 state, so we need to go
 * and correct steam installs.
 *
 * Returns true if the install was fine (needs no correction)
 */
static bool VerifySpearInstall(const char* directory)
{
	const FString MissionFiles[3] =
	{
		"gamemaps.",
		"maphead.",
		"vswap."
	};

	File dir(directory);
	//if(!dir.isWritable())
	//	return true;

	// Check for gamemaps.sd1, if it doesn't exist assume we're good
	if(!File(dir, dir.getInsensitiveFile("gamemaps.sd1", false)).exists())
		return true;

	// Try to find what mission that the .sod files are.
	// If everything is present then we just need to worry about the sd1 files.
	int currentMission = 1;
	if(!File(dir, dir.getInsensitiveFile("gamemaps.sd3", false)).exists())
		currentMission = 3;
	else if(!File(dir, dir.getInsensitiveFile("gamemaps.sd2", false)).exists())
		currentMission = 2;
	else if(File(dir, dir.getInsensitiveFile("gamemaps.sod", false)).exists())
		currentMission = -1;

	Printf("Spear of Destiny is not set to the original mission. Attempting remap for files in: %s\n", directory);
	for(unsigned int i = 0;i < 3;++i)
	{
		File srcFile(dir, dir.getInsensitiveFile(MissionFiles[i] + "sod", false));
		File sd1File(dir, dir.getInsensitiveFile(MissionFiles[i] + "sd1", false));

		if(currentMission > 0)
			srcFile.rename(MissionFiles[i] + "sd" + (char)('0' + currentMission));
		sd1File.rename(MissionFiles[i] + "sod");
	}

	return false;
}

/* Find valid game data.  Due to the nature of WOlf3D we must collect
 * information by extensions.  An extension is considered valid if it has all
 * files needed.  If the OS is case sensitive then the case sensitivity only
 * applies to the extension itself.
 */
static void LookForGameData(FResourceFile *res, TArray<WadStuff> &iwads, const char* directory)
{
	static const unsigned int LoadableBaseFiles[] = { FILE_AUDIOT, FILE_GAMEMAPS, FILE_VGAGRAPH, FILE_VSWAP, BASEFILES };
	static const char* const BaseFileNames[BASEFILES] = {
		"audiohed", "audiot",
		"gamemaps", "maphead",
		"vgadict", "vgahead", "vgagraph",
		"vswap"
	};
	TArray<BaseFile> foundFiles;

	File dir(directory);
	if(!dir.exists())
		return;

	if(!VerifySpearInstall(directory))
		dir = File(directory); // Repopulate the file list

	TArray<FString> files = dir.getFileList();
	for(unsigned int i = 0;i < files.Size();++i)
	{
		FString name, extension;
		if(!SplitFilename(files[i], name, extension))
			continue;

		BaseFile *base = NULL;
		for(unsigned int j = 0;j < foundFiles.Size();++j)
		{
			#if ISCASEINSENSITIVE
			if(foundFiles[j].extension.CompareNoCase(extension) == 0)
			#else
			if(foundFiles[j].extension.Compare(extension) == 0)
			#endif
			{
				base = &foundFiles[j];
				break;
			}
		}
		if(!base)
		{
			int index = foundFiles.Push(BaseFile());
			base = &foundFiles[index];
			base->extension = extension;
			base->isValid = 0;
		}

		unsigned int baseName = 0;
		do
		{
			if(name.CompareNoCase(BaseFileNames[baseName]) == 0)
			{
				base->filename[baseName].Format("%s/%s", directory, files[i].GetChars());
				base->isValid |= 1<<baseName;
				break;
			}
		}
		while(++baseName < BASEFILES);
	}

	for(unsigned int i = 0;i < foundFiles.Size();++i)
	{
		if(!foundFiles[i].isValid)
			continue;

		WadStuff wadStuff;
		wadStuff.Extension = foundFiles[i].extension;
		for(unsigned int j = 0;LoadableBaseFiles[j] != BASEFILES;++j)
		{
			if((foundFiles[i].isValid & (1<<LoadableBaseFiles[j])))
				wadStuff.Path.Push(foundFiles[i].filename[LoadableBaseFiles[j]]);
		}

		// Before checking the data we must load the remap file.
		FString mapFile;
		mapFile.Format("%sMAP", foundFiles[i].extension.GetChars());
		for(unsigned int j = res->LumpCount();j-- > 0;)
		{
			FResourceLump *lump = res->GetLump(j);
			if(mapFile.CompareNoCase(lump->Name) == 0)
				LumpRemapper::LoadMap(foundFiles[i].extension, lump->FullName, (const char*) lump->CacheLump(), lump->LumpSize);
		}

		if(CheckData(wadStuff) > -1)
		{
			// Check to ensure there are no duplicates
			bool doPush = true;
			for(unsigned int j = 0;j < iwads.Size();++j)
			{
				if(iwads[j].Type == wadStuff.Type)
				{
					doPush = false;
					break;
				}
			}
			if(doPush)
				iwads.Push(wadStuff);
		}
	}

	LumpRemapper::ClearRemaps();
}

/**
 * Remove any iwads which depend on another, but the other isn't present.
 */
static void CheckForExpansionRequirements(TArray<WadStuff> &iwads)
{
	for(unsigned int i = iwads.Size();i-- > 0;)
	{
		const FString &req = iwadTypes[iwads[i].Type].Required;
		if(!req.IsEmpty())
		{
			bool reqSatisfied = false;

			for(unsigned int j = 0;j < iwads.Size();++j)
			{
				if(iwadTypes[iwads[j].Type].Name.Compare(req) == 0)
				{
					for(unsigned int k = iwads[j].Path.Size();k-- > 0;)
						iwads[i].Path.Insert(0, iwads[j].Path[k]);
					reqSatisfied = true;
					break;
				}
			}

			if(!reqSatisfied)
				iwads.Delete(i);
		}
	}
}

static void ParseIWad(Scanner &sc)
{
	IWadData iwad;
	iwad.Flags = 0;

	sc.MustGetToken('{');
	while(!sc.CheckToken('}'))
	{
		sc.MustGetToken(TK_Identifier);
		FString key = sc->str;
		sc.MustGetToken('=');
		if(key.CompareNoCase("Flags") == 0)
		{
			do
			{
				sc.MustGetToken(TK_Identifier);
				if(sc->str.CompareNoCase("HelpHack") == 0)
					iwad.Flags |= IWad::HELPHACK;
				else if(sc->str.CompareNoCase("Registered") == 0)
					iwad.Flags |= IWad::REGISTERED;
				else
					sc.ScriptMessage(Scanner::ERROR, "Unknown flag %s.", sc->str.GetChars());
			}
			while(sc.CheckToken(','));
		}
		else if(key.CompareNoCase("Game") == 0)
		{
			// This specifies a filter to be used for switching between things
			// like environment sounds.
			sc.MustGetToken(TK_StringConst);
			iwad.Game = sc->str;
		}
		else if(key.CompareNoCase("Name") == 0)
		{
			sc.MustGetToken(TK_StringConst);
			iwad.Name = sc->str;
		}
		else if(key.CompareNoCase("Mapinfo") == 0)
		{
			sc.MustGetToken(TK_StringConst);
			iwad.Mapinfo = sc->str;
		}
		else if(key.CompareNoCase("MustContain") == 0)
		{
			do
			{
				sc.MustGetToken(TK_StringConst);
				iwad.Ident.Push(sc->str);
			}
			while(sc.CheckToken(','));
		}
		else if(key.CompareNoCase("Required") == 0)
		{
			sc.MustGetToken(TK_StringConst);
			iwad.Required = sc->str;
		}
	}

	iwadTypes.Push(iwad);
}
static void ParseIWadInfo(FResourceFile *res)
{
	for(unsigned int i = res->LumpCount();i-- > 0;)
	{
		FResourceLump *lump = res->GetLump(i);

		if(lump->Namespace == ns_global && stricmp(lump->Name, "IWADINFO") == 0)
		{
			Scanner sc((const char*)lump->CacheLump(), lump->LumpSize);
			sc.SetScriptIdentifier(lump->FullName);

			while(sc.TokensLeft())
			{
				sc.MustGetToken(TK_Identifier);
				if(sc->str.CompareNoCase("IWad") == 0)
				{
					ParseIWad(sc);
				}
				else
					sc.ScriptMessage(Scanner::ERROR, "Unknown IWADINFO block '%s'.", sc->str.GetChars());
			}
			break;
		}
	}
}

void SelectGame(TArray<FString> &wadfiles, const char* iwad, const char* datawad, const FString &progdir)
{
	config.CreateSetting("DefaultIWad", 0);
	config.CreateSetting("ShowIWadPicker", 1);
	bool showPicker = config.GetSetting("ShowIWadPicker")->GetInteger() != 0;
	int defaultIWad = config.GetSetting("DefaultIWad")->GetInteger();

	bool useProgdir = false;
	FResourceFile *datawadRes = FResourceFile::OpenResourceFile(datawad, NULL, true);
	if(!datawadRes)
	{
		useProgdir = true;
		datawadRes = FResourceFile::OpenResourceFile(progdir + "/" + datawad, NULL, true);
	}
	if(!datawadRes)
		I_Error("Could not open %s!", datawad);

	ParseIWadInfo(datawadRes);

	// Get a list of potential data paths
	FString dataPaths;
	if(config.GetSetting("BaseDataPaths") == NULL)
	{
		FString configDir = FileSys::GetDirectoryPath(FileSys::DIR_Configuration);
		dataPaths = ".;$PROGDIR";

		// On OS X our default config directory is ~/Library/Preferences which isn't a good place to put data at all.
#if !defined(__APPLE__)
		dataPaths += FString(";") + configDir;
#endif

		// Add documents and application support directories if they're not mapped to the config directory.
		FString tmp;
		if((tmp = FileSys::GetDirectoryPath(FileSys::DIR_Documents)) != configDir)
			dataPaths += FString(";") + tmp;
		if((tmp = FileSys::GetDirectoryPath(FileSys::DIR_ApplicationSupport)) != configDir)
			dataPaths += FString(";") + tmp;

		config.CreateSetting("BaseDataPaths", dataPaths);
	}
	dataPaths = config.GetSetting("BaseDataPaths")->GetString();

	TArray<WadStuff> basefiles;
	long split = 0;
	do
	{
		long newSplit = dataPaths.IndexOf(';', split);
		FString path = dataPaths.Mid(split, newSplit-split);
		// Check for environment variable
		if(path[0] == '$')
		{
			FString envvar = path.Mid(1);
			if(envvar.CompareNoCase("PROGDIR") == 0)
				path = progdir;
			else
			{
				char* value = getenv(envvar);
				if(value)
					path = value;
				else
					path = "";
			}
		}

		// Skip empty paths
		if(!path.IsEmpty())
			LookForGameData(datawadRes, basefiles, path);
		split = newSplit+1;
	}
	while(split != 0);

	LookForGameData(datawadRes, basefiles, "/usr/local/share/games/wolf3d");

	// Look for a steam install. (Basically from ZDoom)
	{
		static const struct
		{
			FileSys::ESteamApp app;
			const char* const dir;
		} steamDirs[] =
		{
			{FileSys::APP_Wolfenstein3D, PATH_SEPARATOR "base"},
			{FileSys::APP_SpearOfDestiny, PATH_SEPARATOR "base"},
			{FileSys::APP_NoahsArk, ""}
		};
		for(unsigned int i = 0;i < countof(steamDirs);++i)
			LookForGameData(datawadRes, basefiles, FileSys::GetSteamPath(steamDirs[i].app) + steamDirs[i].dir);
	}

	delete datawadRes;

	// Check requirements now as opposed to with LookForGameData so that reqs
	// don't get loaded multiple times.
	CheckForExpansionRequirements(basefiles);

	if(basefiles.Size() == 0)
	{
		I_Error("Can not find base game data. (*.wl6, *.wl1, *.sdm, *.sod)");
	}

	int pick = -1;
	if(iwad)
	{
		for(unsigned int i = 0;i < basefiles.Size();++i)
		{
			#if ISCASEINSENSITIVE
			if(basefiles[i].Extension.CompareNoCase(iwad) == 0)
			#else
			if(basefiles[i].Extension.Compare(iwad) == 0)
			#endif
			{
				pick = i;
				break;
			}
		}
	}
	if(pick < 0)
	{
		if(basefiles.Size() > 1)
		{
			pick = I_PickIWad(&basefiles[0], basefiles.Size(), showPicker, defaultIWad);
			if(pick != -1 && (unsigned int) pick >= basefiles.Size()) // keep the pick within bounds
				pick = basefiles.Size()-1;
			config.GetSetting("ShowIWadPicker")->SetValue(queryiwad);
		}
		else
			pick = 0;
	}
	if(pick < 0)
		Quit("");

	config.GetSetting("DefaultIWad")->SetValue(pick);

	WadStuff &base = basefiles[pick];
	selectedGame = &iwadTypes[base.Type];

	if(!useProgdir)
		wadfiles.Push(datawad);
	else
		wadfiles.Push(progdir + "/" + datawad);
	for(unsigned int i = 0;i < base.Path.Size();++i)
	{
		wadfiles.Push(base.Path[i]);
	}

	NumIWads = base.Path.Size();
}

}
