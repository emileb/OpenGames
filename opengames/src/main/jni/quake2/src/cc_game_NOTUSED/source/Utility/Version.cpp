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
// cc_version.cpp
// 
//

#include "Local.h"
#include "Utility/Version.h"

// Paril: Don't do this if we don't have STDC (Win32)
#if defined(CC_STDC_CONFORMANCE)
#undef VERSION_CHECKING
#define VERSION_CHECKING VC_NONE
#endif

// These three stay constant
void CheckNewVersion ();
void CheckVersionReturnance ();

#if VERSION_CHECKING == VC_CURL
#define CURL_STATICLIB
#include "curl\curl.h"
#endif

#define VERSION_PATH GAMENAME"/version.ver"
#define VERSION_URL "http://alteredsoftworks.com/cleancode/version.ver"

void WriteVersion ()
{
	CFile File (VERSION_PATH, FILEMODE_WRITE|FILEMODE_CREATE);

	if (!File.Valid())
		return;

	File.Print ("%s %u %u %u", CLEANCODE_VERSION_PREFIX, CLEANCODE_VERSION_MAJOR_N, CLEANCODE_VERSION_MINOR_N, CLEANCODE_VERSION_BUILD_N);
}

void VerifyVersionFile ()
{
	CFileBuffer Buffer (VERSION_PATH, true);
	CParser Parser (Buffer.GetBuffer<char> (), PSP_COMMENT_LINE);

	String prefix;
	Parser.ParseToken (PSF_ALLOW_NEWLINES, prefix);

	uint8 minor;
	uint16 major;
	uint32 build;
	Parser.ParseDataType<uint16> (PSF_ALLOW_NEWLINES, &major, 1);
	Parser.ParseDataType<uint8> (PSF_ALLOW_NEWLINES, &minor, 1);
	Parser.ParseDataType<uint32> (PSF_ALLOW_NEWLINES, &build, 1);

	if (CompareVersion (prefix.CString(), major, minor, build))
	{
		ServerPrint ("Version file out of date; updating...\n");
		WriteVersion ();
	}
}

/*
======================================
BLANKS
======================================
*/

#if (VERSION_CHECKING == VC_NONE)
void CheckNewVersion ()
{
}

void CheckVersionReturnance ()
{
}

/*
======================================
WININET
======================================
*/

#elif (VERSION_CHECKING == VC_WININET)

#include <WinInet.h>
bool				VersionCheckReady;
String				VersionPrefix;
uint8				VersionMajor;
uint16				VersionMinor;
uint32				VersionBuild;
EVersionComparison	VersionReturnance;
String				receiveBuffer;

#if defined(WIN32) && !defined(NO_MULTITHREAD_VERSION_CHECK)
HANDLE				hThread;
DWORD				iID;
long WINAPI CheckNewVersionThread (long lParam);

void CheckNewVersion ()
{
	if (hThread)
		return; // Already checking version..

	hThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)CheckNewVersionThread, NULL, 0, &iID);

	if (!hThread)
	{
		// Run it without a thread
		CheckNewVersionThread (0);
		CheckVersionReturnance ();
	}
}

const int READ_BYTES_SIZE = 16;

long WINAPI CheckNewVersionThread (long lParam)
#else
void CheckNewVersion ()
#endif
{
	HINTERNET iInternetHandle = InternetOpenA ("wininet-agent/1.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	
	receiveBuffer.Clear();

	if (iInternetHandle)
	{
		HINTERNET iInternetFile = InternetOpenUrlA (iInternetHandle, VERSION_URL, NULL, 0, INTERNET_FLAG_RESYNCHRONIZE, INTERNET_NO_CALLBACK);

		if (iInternetFile)
		{
			// Start writing the file
			DWORD numBytesRead = 0;

			char buf[READ_BYTES_SIZE];
			while (true)
			{
				buf[0] = 0;
				bool Passed = (!!InternetReadFile (iInternetFile, &buf, READ_BYTES_SIZE, &numBytesRead));

				if (!Passed || Passed && ((numBytesRead == 0) || numBytesRead < READ_BYTES_SIZE))
					break;

				receiveBuffer += buf;
			}

			InternetCloseHandle (iInternetFile);
		}

		InternetCloseHandle (iInternetHandle);
	}

	VersionCheckReady = true;
	return 0;
}

void CheckVersionReturnance ()
{
#if defined(WIN32) && !defined(NO_MULTITHREAD_VERSION_CHECK)
	if (VersionCheckReady)
	{
		if (!receiveBuffer.IsNullOrEmpty() && (receiveBuffer[0] != '<'))
		{
			CParser Parser (receiveBuffer.CString(), PSP_COMMENT_LINE);

			String prefix;
			Parser.ParseToken (PSF_ALLOW_NEWLINES, prefix);

			uint8 minor;
			uint16 major;
			uint32 build;
			Parser.ParseDataType<uint16> (PSF_ALLOW_NEWLINES, &major, 1);
			Parser.ParseDataType<uint8> (PSF_ALLOW_NEWLINES, &minor, 1);
			Parser.ParseDataType<uint32> (PSF_ALLOW_NEWLINES, &build, 1);

	#if defined(WIN32) && !defined(NO_MULTITHREAD_VERSION_CHECK)
			VersionReturnance = CompareVersion (prefix.CString(), major, minor, build);
			VersionPrefix = prefix;
			VersionMinor = minor;
			VersionMajor = major;
			VersionBuild = build;
			VersionCheckReady = true;
	#else
			if (CompareVersion (prefix.CString(), minor, major, build) == VERSION_NEWER)
				ServerPrintf (
				"==================================\n"
				"*****************************\n"
				"There is an update available for CleanCode!\n"
				"Please go to http://code.google.com/p/cleancodequake2 and update accordingly.\n"
				"Your version:   "CLEANCODE_VERSION_PRINT"\n"
				"Update version: "CLEANCODE_VERSION_PRINT"\n"
				"*****************************\n"
				"==================================\n",
				CLEANCODE_VERSION_PRINT_ARGS,
				prefix.CString(), major, minor, build);
			else
				ServerPrint ("Your version of CleanCode is up to date.\n");
	#endif
		}

		if (VersionReturnance == VERSION_NEWER)
			ServerPrintf (
			"==================================\n"
			"*****************************\n"
			"There is an update available for CleanCode!\n"
			"Please go to http://code.google.com/p/cleancodequake2 and update accordingly\nor run the auto updater."
			"Your version:   "CLEANCODE_VERSION_PRINT"\n"
			"Update version: "CLEANCODE_VERSION_PRINT"\n"
			"*****************************\n"
			"==================================\n",
			CLEANCODE_VERSION_PRINT_ARGS,
			VersionPrefix.CString(), VersionMajor, VersionMinor, VersionBuild);
		else
			ServerPrint ("Your version of CleanCode is up to date.\n");

		VersionReturnance = VERSION_SAME;
		VersionCheckReady = false;

		CloseHandle (hThread);

		hThread = NULL;
		iID = 0;
	}
#endif
}

/*
======================================
CURL
======================================
*/

#elif (VERSION_CHECKING == VC_CURL)
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

struct MemoryStruct
{
	char *memory;
	size_t size;
};

static void *myrealloc(void *ptr, size_t size)
{
	/* There might be a realloc() out there that doesn't like reallocing
	NULL pointers, so we take care of it here */
	if(ptr)
		return Mem_ReAlloc (ptr, size);
	else
		return Mem_Alloc (size);
}

static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;

	mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);

	if (mem->memory)
	{
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}

	return realsize;
}

#if defined(WIN32) && !defined(NO_MULTITHREAD_VERSION_CHECK)
HANDLE				hThread;
DWORD				iID;
long WINAPI			CheckNewVersionThread (long lParam);
void				CheckVersionReturnance ();
bool				VersionCheckReady;
String				VersionPrefix;
uint8				VersionMajor;
uint16				VersionMinor;
uint32				VersionBuild;
EVersionComparison	VersionReturnance;

void CheckNewVersion ()
{
	if (hThread)
		return; // Already checking version..

	hThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)CheckNewVersionThread, NULL, 0, &iID);

	if (!hThread)
	{
		// Run it without a thread
		CheckNewVersionThread (0);
		CheckVersionReturnance ();
	}
}

void CheckVersionReturnance ()
{
	if (VersionCheckReady)
	{
		if (VersionReturnance == VERSION_NEWER)
			ServerPrintf (
			"==================================\n"
			"*****************************\n"
			"There is an update available for Cl	eanCode!\n"
			"Please go to http://code.google.com/p/cleancodequake2 and update accordingly.\n"
			"Your version:   "CLEANCODE_VERSION_PRINT"\n"
			"Update version: "CLEANCODE_VERSION_PRINT"\n"
			"*****************************\n"
			"==================================\n",
			CLEANCODE_VERSION_PRINT_ARGS,
			VersionPrefix.CString(), VersionMajor, VersionMinor, VersionBuild);
		else
			ServerPrint ("Your version of CleanCode is up to date.\n");

		VersionReturnance = VERSION_SAME;
		VersionCheckReady = false;

		CloseHandle (hThread);

		hThread = NULL;
		iID = 0;
	}
}

long WINAPI CheckNewVersionThread (long lParam)
#else
void CheckNewVersion ()
#endif
{
	CURL *curl_handle;

	struct MemoryStruct chunk;

	chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
	chunk.size = 0;    /* no data at this point */

	curl_global_init(CURL_GLOBAL_ALL);

	/* init the curl session */
	curl_handle = curl_easy_init();

	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, "http://cleancodequake2.googlecode.com/svn/trunk/version.ver");

	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

	/* some servers don't like requests that are made without a user-agent
	field, so we provide one */
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/* get it! */
	curl_easy_perform(curl_handle);

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);

	/*
	* Now, our chunk.memory points to a memory block that is chunk.size
	* bytes big and contains the remote file.
	*
	* Do something nice with it!
	*
	* You should be aware of the fact that at this point we might have an
	* allocated data block, and nothing has yet deallocated that data. So when
	* you're done with it, you should free() it as a nice application.
	*/

	if(chunk.memory)
	{
		CParser Parser (chunk.memory, PSP_COMMENT_LINE);

		const char *token;

		String prefix;
		Parser.ParseToken (PSF_ALLOW_NEWLINES, &token);
		prefix = token;

		uint8 minor;
		uint16 major;
		uint32 build;
		Parser.ParseDataType<uint16> (PSF_ALLOW_NEWLINES, &major, 1);
		Parser.ParseDataType<uint8> (PSF_ALLOW_NEWLINES, &minor, 1);
		Parser.ParseDataType<uint32> (PSF_ALLOW_NEWLINES, &build, 1);

#if defined(WIN32) && !defined(NO_MULTITHREAD_VERSION_CHECK)
		VersionReturnance = CompareVersion (prefix.CString(), major, minor, build);
		VersionPrefix = prefix;
		VersionMinor = minor;
		VersionMajor = major;
		VersionBuild = build;
		VersionCheckReady = true;
#else
		if (CompareVersion (prefix.CString(), minor, major, build) == VERSION_NEWER)
			ServerPrintf (
			"==================================\n"
			"*****************************\n"
			"There is an update available for CleanCode!\n"
			"Please go to http://code.google.com/p/cleancodequake2 and update accordingly.\n"
			"Your version:   "CLEANCODE_VERSION_PRINT"\n"
			"Update version: "CLEANCODE_VERSION_PRINT"\n"
			"*****************************\n"
			"==================================\n",
			CLEANCODE_VERSION_PRINT_ARGS,
			prefix.CString(), major, minor, build);
		else
			ServerPrint ("Your version of CleanCode is up to date.\n");
#endif

		Mem_Free (chunk.memory);
	}

	/* we're done with libcurl, so clean it up */
	curl_global_cleanup();

	return 0;
}
#endif

/**
\fn	void InitVersion ()

\brief	Initialises the version system. 

\author	Paril
\date	25/05/2010
**/
void InitVersion ()
{
#if (VERSION_CHECKING != VC_NONE)
	ServerPrint ("Checking for new version...\n");

	if (!CFile::Exists(VERSION_PATH))
	{
		ServerPrint ("Version file non-existant, writing... ");
		WriteVersion ();
		ServerPrint ("done\n");
	}
	else
		VerifyVersionFile ();
	
	CheckNewVersion ();
#endif
}

String ConfigTimeString ();

void CVersionCommand::Execute ()
{
	Player->PrintToClient (PRINT_HIGH, "This server is running CleanCode version "CLEANCODE_VERSION_PRINT", built on %s\n", CLEANCODE_VERSION_PRINT_ARGS, ConfigTimeString().CString());
}

void CServerVersionCommand::Execute ()
{
#if (VERSION_CHECKING != VC_NONE)
	if (ArgGets (2).IsNullOrEmpty())
#endif
		ServerPrintf ("This server is running CleanCode version "CLEANCODE_VERSION_PRINT" built on %s\n", CLEANCODE_VERSION_PRINT_ARGS, ConfigTimeString().CString());
#if (VERSION_CHECKING != VC_NONE)
	else
		CheckNewVersion ();
#endif
}

