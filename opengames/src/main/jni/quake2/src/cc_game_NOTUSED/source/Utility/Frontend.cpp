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
/*
This source file is contained as part of CleanCode Quake2, a project maintained
by Paril, to 'clean up' and make Quake2 an easier source base to read and work with.

You may use any part of this code to help create your own bases and own mods off
this code if you wish. It is under the same license as Quake 2 source (as above),
therefore you are free to have to fun with it. All I ask is you email me so I can
list the mod on my page for CleanCode Quake2 to help get the word around. Thanks.
*/

//
// cc_frontend.cpp
// Contains all the functions required for CleanCode to act as a frontend to
// gamex86.dll (ie, in release directory)
//

#include "Local.h"

#ifdef _FRONTEND

HMODULE Library;

gameExport_t *_ge;
void (*_SpawnEntities) (char *mapName, char *entString, char *spawnPoint);
gameExport_t	*(*Game_GetGameAPI) (gameImport_t *);
bool Sys_GetGameAPI ()
{
	string fileName;

	CCvar Game ("game", "");
	fileName += Game.StringValue();
	fileName += "/gamex86.dll";

	Library = LoadLibrary(fileName.CString());

	if (!Library)
		return false;

	Game_GetGameAPI = (gameExport_t *(__cdecl *)(gameImport_t *))GetProcAddress (Library, "GetGameAPI");

	if (!Game_GetGameAPI)
	{
		FreeLibrary (Library);
		return false;
	}
	return true;
}

void New_SpawnEntities (char *mapName, char *entString, char *spawnPoint)
{
	entString = CC_ParseSpawnEntities (mapName, entString);

	_SpawnEntities(mapName, entString, spawnPoint);
}

gameExport_t *GetGameAPI (gameImport_t *import)
{
	gi = *import;

	if (!Sys_GetGameAPI())
		return NULL;

	_ge = Game_GetGameAPI(import);
	_SpawnEntities = _ge->SpawnEntities;
	_ge->SpawnEntities = New_SpawnEntities;

	if (_ge->apiVersion != GAME_APIVERSION)
		return NULL;

	return _ge;
}
#endif
