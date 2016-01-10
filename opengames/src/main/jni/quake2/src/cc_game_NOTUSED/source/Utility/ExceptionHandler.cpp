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
// cc_exceptionhandler.cpp
// Exception handler, stolen from EGL. Shh!
//

#include "Local.h"
#include "Utility/ExceptionHandler.h"

#if CC_USE_EXCEPTION_HANDLER && (MSVS_VERSION >= VS_9) && !defined(CC_STDC_CONFORMANCE)
#include "Utility/Version.h"
#include <cerrno>
#include <cfloat>

#include <fcntl.h>
#include <direct.h>
#include <io.h>
#include <conio.h>
#include <process.h>
#include <dbghelp.h>

#define HEX_VALUE_64 "0x%08I64X"
#define HEX_VALUE_32 "0x%08X"

#ifndef VER_SUITE_WH_SERVER
#define VER_SUITE_WH_SERVER 0x00008000 // sigh
#endif

#ifdef id386
#define USE_GZ
#include "zlib.h"
#endif

#define APP_FULLNAME "CleanCode Quake II"

/*
==============================================================================

	EXCEPTION HANDLER

==============================================================================
*/

typedef BOOL (WINAPI *ENUMERATELOADEDMODULES64) (HANDLE hProcess, PENUMLOADED_MODULES_CALLBACK64 EnumLoadedModulesCallback, PVOID UserContext);
typedef DWORD (WINAPI *SYMSETOPTIONS) (DWORD SymOptions);
typedef BOOL (WINAPI *SYMINITIALIZE) (HANDLE hProcess, PSTR UserSearchPath, BOOL fInvadeProcess);
typedef BOOL (WINAPI *SYMCLEANUP) (HANDLE hProcess);
typedef BOOL (WINAPI *STACKWALK64) (
								DWORD MachineType,
								HANDLE hProcess,
								HANDLE hThread,
								LPSTACKFRAME64 StackFrame,
								PVOID ContextRecord,
								PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
								PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
								PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
								PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress
								);

typedef PVOID	(WINAPI *SYMFUNCTIONTABLEACCESS64) (HANDLE hProcess, DWORD64 AddrBase);
typedef DWORD64 (WINAPI *SYMGETMODULEBASE64) (HANDLE hProcess, DWORD64 dwAddr);
typedef BOOL	(WINAPI *SYMFROMADDR) (HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PSYMBOL_INFO Symbol);
typedef BOOL	(WINAPI *SYMGETMODULEINFO64) (HANDLE hProcess, DWORD64 dwAddr, PIMAGEHLP_MODULE64 ModuleInfo);

typedef DWORD64 (WINAPI *SYMLOADMODULE64) (HANDLE hProcess, HANDLE hFile, PSTR ImageName, PSTR ModuleName, DWORD64 BaseOfDll, DWORD SizeOfDll);

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP) (
	HANDLE hProcess,
	DWORD ProcessId,
	HANDLE hFile,
	MINIDUMP_TYPE DumpType,
	PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	PMINIDUMP_CALLBACK_INFORMATION CallbackParam
	);

typedef HINSTANCE (WINAPI *SHELLEXECUTEA) (HWND hwnd, LPCTSTR lpOperation, LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd);

typedef BOOL (WINAPI *VERQUERYVALUE) (const LPVOID pBlock, LPTSTR lpSubBlock, PUINT lplpBuffer, PUINT puLen);
typedef DWORD (WINAPI *GETFILEVERSIONINFOSIZE) (LPTSTR lptstrFilename, LPDWORD lpdwHandle);
typedef BOOL (WINAPI *GETFILEVERSIONINFO) (LPTSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);

static SYMGETMODULEINFO64		fnSymGetModuleInfo64;
static SYMLOADMODULE64			fnSymLoadModule64;

static VERQUERYVALUE			fnVerQueryValue;
static GETFILEVERSIONINFOSIZE	fnGetFileVersionInfoSize;
static GETFILEVERSIONINFO		fnGetFileVersionInfo;

static CHAR						szModuleName[MAX_PATH];

/*
==================
EGLExceptionHandler
==================
*/
static sint32 Sys_FileLength (const char *path)
{
	WIN32_FILE_ATTRIBUTE_DATA	fileData;

	if (GetFileAttributesExA (path, GetFileExInfoStandard, &fileData))
		return fileData.nFileSizeLow;

	return -1;
}

#ifdef USE_CURL
static sint32 EGLUploadProgress (void *clientp, double downTotal, double downNow, double upTotal, double upNow)
{
	char	progressBuff[512];
	DWORD	len;
	double	speed;
	float	percent;
	MSG		msg;

	if (upTotal <= 0)
		return 0;

	curl_easy_getinfo ((CURL *)clientp, CURLINFO_SPEED_UPLOAD, &speed);

	if (upNow == upTotal)
		percent = 100.0f;
	else {
		percent = ((upNow / upTotal) * 100.0f);
		if (percent == 100.0f)
			percent = 99.99f;
	}

	len = snprintf (progressBuff, sizeof(progressBuff)-1, "[%6.2f%%] %g / %g bytes, %g bytes/sec.\n", percent, upNow, upTotal, speed);
	Sys_ConsolePrint (progressBuff);

	snprintf (progressBuff, sizeof(progressBuff)-1, "EGL Crash Dump Uploader [%6.2f%%]", percent);
	Sys_SetConsoleTitle (progressBuff);

	// Dispatch window messages
	while (PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE)) {
		GetMessage (&msg, NULL, 0, 0);
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}

	return 0;
}

static VOID EGLUploadCrashDump (LPCSTR crashDump, LPCSTR crashText)
{
	struct curl_httppost* post = NULL;
	struct curl_httppost* last = NULL;
	CURL	*curl;

	__try {
		DWORD	lenDmp = Sys_FileLength (crashDump);
		DWORD	lenTxt = Sys_FileLength (crashText);

		if (lenTxt == -1)
			return;

		Sys_SetConsoleTitle ("EGL Crash Dump Uploader");
		Sys_ConsolePrint ("Connecting...\n");

		curl = curl_easy_init ();

		// Add simple file section
		if (lenDmp > 0)
			curl_formadd (&post, &last, CURLFORM_PTRNAME, "minidump", CURLFORM_FILE, crashDump, CURLFORM_END);
		curl_formadd (&post, &last, CURLFORM_PTRNAME, "report", CURLFORM_FILE, crashText, CURLFORM_END);

		// Set the form info
		curl_easy_setopt (curl, CURLOPT_HTTPPOST, post);

//		if (cl_http_proxy)
//			curl_easy_setopt (curl, CURLOPT_PROXY, cl_http_proxy->string);

		//curl_easy_setopt (curl, CURLOPT_UPLOAD, 1);
		curl_easy_setopt (curl, CURLOPT_PROGRESSFUNCTION, EGLUploadProgress);
		curl_easy_setopt (curl, CURLOPT_PROGRESSDATA, curl);
		curl_easy_setopt (curl, CURLOPT_NOPROGRESS, 0);

		curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt (curl, CURLOPT_USERAGENT, APP_FULLNAME);
		curl_easy_setopt (curl, CURLOPT_URL, "http://egl.quakedev.com/reports/receiveCrashDump.php");

		if (curl_easy_perform (curl) != CURLE_OK) {
			MessageBox (NULL, "An error occured while trying to upload the crash dump. Please post it manually on the EGL forums.", "Upload Error", MB_ICONEXCLAMATION | MB_OK);
		}
		else {
			long	response;

			if (curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &response) == CURLE_OK) {
				switch (response) {
				case 200:
					MessageBox (NULL, "Upload completed. Thanks for submitting your crash report!\n\n"
										"If you would like feedback on the cause of this crash, post on the EGL forums describing what you were\n"
										"doing at the time the exception occured. If possible, please also attach the EGLCrashLog.txt file.",
										"Upload Complete", MB_ICONINFORMATION | MB_OK);
					break;
				default:
					MessageBox (NULL, "Upload failed, HTTP error.", "Upload Failed", MB_ICONEXCLAMATION | MB_OK);
					break;
				}
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		MessageBox (NULL, "An exception occured while trying to upload the crash dump. Please post it manually on the EGL forums.", "Upload Error", MB_ICONEXCLAMATION | MB_OK);
	}
}
#endif

namespace EEnumerateLoadedModulesProcSymInfoHeap
{
IMAGEHLP_MODULE64	symInfo;
FILE				*fhReport;

static const PCHAR SymTypeToString (SYM_TYPE SymType)
{
	switch (SymType)
	{
	case SymCoff:		return "COFF";
	case SymCv:			return "CV";
	case SymExport:		return "Export";
	case SymPdb:		return "PDB";
	case SymNone:		return "No";
	default:			return "Unknown";
	}
}

inline void NumSpaces (FILE *fhReport, char *name, const PCHAR Type)
{
	fprintf (fhReport, "%-60s", name);
	fprintf (fhReport, "%s\r\n", Type);
}

static BOOL CALLBACK EnumerateLoadedModulesProcSymInfo (PSTR ModuleName, DWORD64 ModuleBase, ULONG ModuleSize, PVOID UserContext)
{
	Mem_Zero (&symInfo, sizeof(symInfo));
	fhReport = (FILE *)UserContext;

	symInfo.SizeOfStruct = sizeof(symInfo);

	if (fnSymGetModuleInfo64 (GetCurrentProcess(), ModuleBase, &symInfo))
		NumSpaces (fhReport, symInfo.LoadedImageName, SymTypeToString(symInfo.SymType));
	else
		NumSpaces (fhReport, ModuleName, "couldn't check symbols");
	
	return TRUE;
}
}

static BOOL CALLBACK EnumerateLoadedModulesProcDump (PCSTR ModuleName, DWORD64 ModuleBase, ULONG ModuleSize, PVOID UserContext)
{
	BYTE	*verInfo;
	UINT	dummy;
	FILE	*fhReport = (FILE *)UserContext;
	CHAR	verString[32];
	CHAR	lowered[MAX_PATH];

	Q_strncpyz (lowered, ModuleName, sizeof(lowered)-1);
	Q_strlwr (lowered);

	if (fnGetFileVersionInfo && fnVerQueryValue && fnGetFileVersionInfoSize)
	{
		DWORD len;
		if ((len = (fnGetFileVersionInfoSize ((LPTSTR)ModuleName, (LPDWORD)&dummy))) != 0)
		{
			verInfo = (uint8*)LocalAlloc (LPTR, len);
/*			if (fnGetFileVersionInfo ((LPTSTR)ModuleName, dummy, len, verInfo))
			{
				if (fnVerQueryValue ((void*)verInfo, L"\\", (PUINT)&fileVersion, &dummy))
					Q_snprintfz (verString, sizeof(verString), "%d.%d.%d.%d", HIWORD(fileVersion->dwFileVersionMS), LOWORD(fileVersion->dwFileVersionMS), HIWORD(fileVersion->dwFileVersionLS), LOWORD(fileVersion->dwFileVersionLS));
				else
					Q_strncpyz (verString, "unknown", sizeof(verString));
			}
			else
				Q_strncpyz (verString, "unknown", sizeof(verString));
*/
			LocalFree (verInfo);
		}
		else
			Q_strncpyz (verString, "unknown", sizeof(verString));	
	}
	else
		Q_strncpyz (verString, "unknown", sizeof(verString));

	fprintf (fhReport, "["HEX_VALUE_64" - "HEX_VALUE_64"] %s (%lu bytes, version %s)\r\n", ModuleBase, ModuleBase + (DWORD64)ModuleSize, ModuleName, ModuleSize, verString);
	return TRUE;
}

static BOOL CALLBACK EnumerateLoadedModulesProcInfo (PSTR ModuleName, DWORD64 ModuleBase, ULONG ModuleSize, PVOID UserContext)
{
	DWORD	addr = (DWORD)UserContext;
	if (addr > ModuleBase && addr < ModuleBase+ModuleSize)
	{
		Q_strncpyz (szModuleName, ModuleName, sizeof(szModuleName)-1);
		return FALSE;
	}
	return TRUE;
}


HANDLE						hProcess;
HMODULE						hDbgHelp, hVersion;
ENUMERATELOADEDMODULES64	fnEnumerateLoadedModules64;
SYMSETOPTIONS				fnSymSetOptions;
SYMINITIALIZE				fnSymInitialize;
STACKWALK64					fnStackWalk64;
SYMFUNCTIONTABLEACCESS64	fnSymFunctionTableAccess64;
SYMGETMODULEBASE64			fnSymGetModuleBase64;
SYMFROMADDR					fnSymFromAddr;
SYMCLEANUP					fnSymCleanup;
MINIDUMPWRITEDUMP			fnMiniDumpWriteDump;
CHAR						searchPath[MAX_PATH], *tempPointer, *upMessage;
CHAR						reportPath[MAX_PATH];
CHAR						dumpPath[MAX_PATH];
MINIDUMP_EXCEPTION_INFORMATION miniInfo;
SYSTEMTIME					timeInfo;
FILE						*fhReport;
DWORD64						fnOffset;
DWORD64						InstructionPtr;
DWORD						ret;
STACKFRAME64				frame = {0};
CONTEXT						context;
SYMBOL_INFO					*symInfo;
OSVERSIONINFOEXA			osInfo;
#ifdef USE_CURL
bool						upload;
#endif

#ifdef USE_GZ
BYTE	gzBuff[0xFFFF];
#endif

#include <tchar.h>
#include <strsafe.h>

#pragma comment(lib, "User32.lib")

const int BUFSIZE = 256;

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

String GetOSDisplayString ()
{
	String Str;
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	PGPI pGPI;
	BOOL bOsVersionInfoEx;
	DWORD dwType;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( ((bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi))) == false)
		return "Unknown";

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

	HMODULE Kernel32 = GetModuleHandle(TEXT("kernel32.dll"));

	if (!Kernel32)
		return "Unknown";

	pGNSI = (PGNSI) GetProcAddress(
		Kernel32, 
		"GetNativeSystemInfo");
	if(NULL != pGNSI)
		pGNSI(&si);
	else GetSystemInfo(&si);

	if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && 
		osvi.dwMajorVersion > 4 )
	{
		Str += "Microsoft ";

		// Test for the specific product.

		if ( osvi.dwMajorVersion == 6 )
		{
			if( osvi.dwMinorVersion == 0 )
			{
				if( osvi.wProductType == VER_NT_WORKSTATION )
					Str += "Windows Vista ";
				else
					Str += "Windows Server 2008 ";
			}

			if ( osvi.dwMinorVersion == 1 )
			{
				if( osvi.wProductType == VER_NT_WORKSTATION )
					Str += "Windows 7 ";
				else
					Str += "Windows Server 2008 R2 ";
			}

			pGPI = (PGPI) GetProcAddress(
				Kernel32, 
				"GetProductInfo");

			pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

			switch( dwType )
			{
			case PRODUCT_ULTIMATE:
				Str += "Ultimate Edition";
				break;
			case PRODUCT_HOME_PREMIUM:
				Str += "Home Premium Edition";
				break;
			case PRODUCT_HOME_BASIC:
				Str += "Home Basic Edition";
				break;
			case PRODUCT_ENTERPRISE:
				Str += "Enterprise Edition";
				break;
			case PRODUCT_BUSINESS:
				Str += "Business Edition";
				break;
			case PRODUCT_STARTER:
				Str += "Starter Edition";
				break;
			case PRODUCT_CLUSTER_SERVER:
				Str += "Cluster Server Edition";
				break;
			case PRODUCT_DATACENTER_SERVER:
				Str += "Datacenter Edition";
				break;
			case PRODUCT_DATACENTER_SERVER_CORE:
				Str += "Datacenter Edition (core installation)";
				break;
			case PRODUCT_ENTERPRISE_SERVER:
				Str += "Enterprise Edition";
				break;
			case PRODUCT_ENTERPRISE_SERVER_CORE:
				Str += "Enterprise Edition (core installation)";
				break;
			case PRODUCT_ENTERPRISE_SERVER_IA64:
				Str += "Enterprise Edition for Itanium-based Systems";
				break;
			case PRODUCT_SMALLBUSINESS_SERVER:
				Str += "Small Business Server";
				break;
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
				Str += "Small Business Server Premium Edition";
				break;
			case PRODUCT_STANDARD_SERVER:
				Str += "Standard Edition";
				break;
			case PRODUCT_STANDARD_SERVER_CORE:
				Str += "Standard Edition (core installation)";
				break;
			case PRODUCT_WEB_SERVER:
				Str += "Web Server Edition";
				break;
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			if( GetSystemMetrics(SM_SERVERR2) )
				Str += "Windows Server 2003 R2, ";
			else if ( osvi.wSuiteMask==VER_SUITE_STORAGE_SERVER )
				Str += "Windows Storage Server 2003";
			else if ( osvi.wSuiteMask==VER_SUITE_WH_SERVER )
				Str += "Windows Home Server";
			else if( osvi.wProductType == VER_NT_WORKSTATION &&
				si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
				Str += "Windows XP Professional x64 Edition";
			else
				Str += "Windows Server 2003, ";

			// Test for the server type.
			if ( osvi.wProductType != VER_NT_WORKSTATION )
			{
				if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						Str += "Datacenter Edition for Itanium-based Systems";
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						Str += "Enterprise Edition for Itanium-based Systems";
				}

				else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						Str += "Datacenter x64 Edition";
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						Str += "Enterprise x64 Edition";
					else
						Str += "Standard x64 Edition";
				}

				else
				{
					if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
						Str += "Compute Cluster Edition";
					else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						Str += "Datacenter Edition";
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						Str += "Enterprise Edition";
					else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
						Str += "Web Edition";
					else
						Str += "Standard Edition";
				}
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			Str += "Windows XP ";
			if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
				Str += "Home Edition";
			else
				Str += "Professional";
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			Str += "Windows 2000 ";

			if ( osvi.wProductType == VER_NT_WORKSTATION )
				Str += "Professional";
			else 
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					Str += "Datacenter Server";
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					Str += "Advanced Server";
				else
					Str += "Server";
			}
		}

		// Include service pack (if any) and build number.

		if( _tcslen(osvi.szCSDVersion) > 0 )
		{
			char temp[128];
			Q_snprintfz (temp, sizeof(temp), " %s", osvi.szCSDVersion);
			Str += temp;
		}

		{
			char temp[64];
			Q_snprintfz (temp, sizeof(temp), " (build %d)", osvi.dwBuildNumber);
			Str += temp;
		}

		if ( osvi.dwMajorVersion >= 6 )
		{
			if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
				Str += ", 64-bit";
			else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
				Str += ", 32-bit";
		}

		return Str; 
	}
		
	return String("Unknown");
}

DWORD EGLExceptionHandler (DWORD exceptionCode, LPEXCEPTION_POINTERS exceptionInfo)
{
	context = *exceptionInfo->ContextRecord;

	// Show the mouse cursor
	ShowCursor (TRUE);

	// Continue searching?
#ifdef _DEBUG
	if (MessageBoxA (NULL, "An unhandled exception occured in CleanCode!\n\nThis version was built with debug information. Do you have a debugger you can attach? If so, do this now, then click Yes, otherwise click No.", "Unhandled Exception", MB_ICONERROR|MB_YESNO) == IDYES)
		return EXCEPTION_CONTINUE_SEARCH;
#endif

	// Load needed libraries and get the location of the needed functions
	hDbgHelp = LoadLibraryA ("DBGHELP");
	if (!hDbgHelp)
	{
		MessageBoxA (NULL, APP_FULLNAME " has encountered an unhandled exception and must be terminated. No crash report could be generated since " APP_FULLNAME " failed to load DBGHELP.DLL. Please obtain DBGHELP.DLL and place it in your Quake II directory to enable crash dump generation.", "Unhandled Exception", MB_OK | MB_ICONEXCLAMATION);
		return EXCEPTION_CONTINUE_SEARCH;
	}

	hVersion = LoadLibraryA ("VERSION");
	if (hVersion)
	{
		fnVerQueryValue = (VERQUERYVALUE)GetProcAddress (hVersion, "VerQueryValueA");
		fnGetFileVersionInfo = (GETFILEVERSIONINFO)GetProcAddress (hVersion, "GetFileVersionInfoA");
		fnGetFileVersionInfoSize = (GETFILEVERSIONINFOSIZE)GetProcAddress (hVersion, "GetFileVersionInfoSizeA");
	}

	fnEnumerateLoadedModules64 = (ENUMERATELOADEDMODULES64)GetProcAddress (hDbgHelp, "EnumerateLoadedModules64");
	fnSymSetOptions = (SYMSETOPTIONS)GetProcAddress (hDbgHelp, "SymSetOptions");
	fnSymInitialize = (SYMINITIALIZE)GetProcAddress (hDbgHelp, "SymInitialize");
	fnSymFunctionTableAccess64 = (SYMFUNCTIONTABLEACCESS64)GetProcAddress (hDbgHelp, "SymFunctionTableAccess64");
	fnSymGetModuleBase64 = (SYMGETMODULEBASE64)GetProcAddress (hDbgHelp, "SymGetModuleBase64");
	fnStackWalk64 = (STACKWALK64)GetProcAddress (hDbgHelp, "StackWalk64");
	fnSymFromAddr = (SYMFROMADDR)GetProcAddress (hDbgHelp, "SymFromAddr");
	fnSymCleanup = (SYMCLEANUP)GetProcAddress (hDbgHelp, "SymCleanup");
	fnSymGetModuleInfo64 = (SYMGETMODULEINFO64)GetProcAddress (hDbgHelp, "SymGetModuleInfo64");
	//fnSymLoadModule64 = (SYMLOADMODULE64)GetProcAddress (hDbgHelp, "SymLoadModule64");
	fnMiniDumpWriteDump = (MINIDUMPWRITEDUMP)GetProcAddress (hDbgHelp, "MiniDumpWriteDump");

	if (!fnEnumerateLoadedModules64 || !fnSymSetOptions || !fnSymInitialize || !fnSymFunctionTableAccess64
	|| !fnSymGetModuleBase64 || !fnStackWalk64 || !fnSymFromAddr || !fnSymCleanup || !fnSymGetModuleInfo64)
	// || !fnSymLoadModule64)
	{
		FreeLibrary (hDbgHelp);
		if (hVersion)
			FreeLibrary (hVersion);
		MessageBoxA (NULL, APP_FULLNAME " has encountered an unhandled exception and must be terminated. No crash report could be generated since " APP_FULLNAME " failed to load DBGHELP.DLL. Please obtain DBGHELP.DLL and place it in your Quake II directory to enable crash dump generation.", "Unhandled Exception", MB_OK | MB_ICONEXCLAMATION);
		return EXCEPTION_CONTINUE_SEARCH;
	}

	// Let the user know
	if (MessageBoxA (NULL, APP_FULLNAME " has encountered an unhandled exception and must be terminated. Would you like to generate a crash report?", "Unhandled Exception", MB_ICONEXCLAMATION | MB_YESNO) == IDNO)
	{
		FreeLibrary (hDbgHelp);
		if (hVersion)
			FreeLibrary (hVersion);
		return EXCEPTION_CONTINUE_SEARCH;
	}

	// Get the current process
	hProcess = GetCurrentProcess();

	fnSymSetOptions (SYMOPT_UNDNAME | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_LOAD_ANYTHING);

	// Used to determine the directory for dump placement
	GetModuleFileNameA (NULL, searchPath, sizeof(searchPath));
	tempPointer = strrchr (searchPath, '\\');
	if (tempPointer)
		*tempPointer = '\0';

	// Get the system time
	GetSystemTime (&timeInfo);

	// Find the next filename to use for this dump
	sint32 dumpNum = 1;
	for (; ; dumpNum++)
	{
		Q_snprintfz (reportPath, sizeof(reportPath), "%s\\CCCrashLog%.4d-%.2d-%.2d_%d.txt", searchPath, timeInfo.wYear, timeInfo.wMonth, timeInfo.wDay, dumpNum);
		if (Sys_FileLength (reportPath) == -1)
			break;
	}

	// Open the report dump file
	fhReport = fopen (reportPath, "wb");

	if (!fhReport)
	{
		FreeLibrary (hDbgHelp);
		if (hVersion)
			FreeLibrary (hVersion);
		return EXCEPTION_CONTINUE_SEARCH;
	}

	// Initialize symbols
	fnSymInitialize (hProcess, searchPath, TRUE);
#ifdef _M_AMD64
	InstructionPtr = context.Rip;
	frame.AddrPC.Offset = InstructionPtr;
	frame.AddrFrame.Offset = context.Rbp;
	frame.AddrPC.Offset = context.Rsp;
#else
	InstructionPtr = context.Eip;
	frame.AddrPC.Offset = InstructionPtr;
	frame.AddrFrame.Offset = context.Ebp;
	frame.AddrStack.Offset = context.Esp;
#endif

	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrStack.Mode = AddrModeFlat;

	symInfo = (SYMBOL_INFO*)LocalAlloc (LPTR, sizeof(*symInfo) + 128);
	symInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
	symInfo->MaxNameLen = 128;
	fnOffset = 0;

	// Get OS info
	Mem_Zero (&osInfo, sizeof(osInfo));
	osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (!GetVersionEx ((OSVERSIONINFO *)&osInfo))
	{
		osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx ((OSVERSIONINFO *)&osInfo);
	}

	// Find out which module threw the exception
	Q_strncpyz (szModuleName, "<unknown>", sizeof(szModuleName));
	fnEnumerateLoadedModules64 (hProcess, (PENUMLOADED_MODULES_CALLBACK64)EnumerateLoadedModulesProcInfo, (VOID *)InstructionPtr);
	Q_strlwr (szModuleName);

	if (strstr (szModuleName, "gamex86"))
	{
		upMessage =
			"CleanCode's Gamex86 seems to be the root problem.\r\n"
			"If this is not base CleanCode, send the report to the mod author,\r\n"
			"otherwise send it to Paril (see the log .txt for more info)\r\n";
#ifdef USE_CURL
		upload = false;
#endif
	}
	else
	{
		upMessage =
			"Unable to detect where the exception occured!\r\n";
#ifdef USE_CURL
		upload = true;
#endif
	}

	// Write out the report to file
	fprintf (fhReport,
		APP_FULLNAME " encountered an unhandled exception and was terminated. If you are\r\n"
		"able to reproduce this crash, please submit the crash report when prompted, or email\r\n"
		"the file to Paril or any CleanCode project member. Goto http://code.google.com/p/cleancodequake2 and click on\r\n"
		"Issues, and file a bug report, or email paril@alteredsoftworks.com with the report.\r\n"
		"\r\n"
		"*** PLEASE MAKE SURE THAT YOU ARE USING THE LATEST VERSION OF CLEANCODE BEFORE SUBMITTING ***\r\nYour Cleancode Version: "CLEANCODE_VERSION_PRINT"\r\n", CLEANCODE_VERSION_PRINT_ARGS);

	fprintf (fhReport, "\r\n");

	// Windows information
	fprintf (fhReport, "Windows information:\r\n");
	fprintf (fhReport, "--------------------------------------------------\r\n");
	fprintf (fhReport, "String:       %s\r\n", GetOSDisplayString().CString());
	fprintf (fhReport, "Version:      %d.%d\r\n", osInfo.dwMajorVersion, osInfo.dwMinorVersion);
	fprintf (fhReport, "Build:        %d\r\n", osInfo.dwBuildNumber);
	fprintf (fhReport, "Service Pack: %s\r\n", osInfo.szCSDVersion[0] ? osInfo.szCSDVersion : "none");

	fprintf (fhReport, "\r\n");

	// Exception information
	fprintf (fhReport, "Exception information:\r\n");
	fprintf (fhReport, "--------------------------------------------------\r\n");
	fprintf (fhReport, "Code:    "HEX_VALUE_32"\r\n", exceptionCode);
	fprintf (fhReport, "Address: "HEX_VALUE_64"\r\n", InstructionPtr);
	fprintf (fhReport, "Module:  %s\r\n", szModuleName);

	fprintf (fhReport, "\r\n");

	// Symbol information
	fprintf (fhReport, "Symbol information:\r\n");
	fprintf (fhReport, "Name                                                        Symbol Type\r\n");
	fprintf (fhReport, "-----------------------------------------------------------------------\r\n");
	fnEnumerateLoadedModules64 (hProcess, (PENUMLOADED_MODULES_CALLBACK64)EEnumerateLoadedModulesProcSymInfoHeap::EnumerateLoadedModulesProcSymInfo, (VOID *)fhReport);

	fprintf (fhReport, "\r\n");

	// Loaded modules
	fprintf (fhReport, "Loaded modules:\r\n");
	fprintf (fhReport, "--------------------------------------------------\r\n");
	fnEnumerateLoadedModules64 (hProcess, (PENUMLOADED_MODULES_CALLBACK64)EnumerateLoadedModulesProcDump, (VOID *)fhReport);

	fprintf (fhReport, "\r\n");

	// Stack trace
	fprintf (fhReport, "Stack trace:\r\n");
	fprintf (fhReport, "--------------------------------------------------\r\n");
	fprintf (fhReport, "Stack      EIP        Arg0       Arg1       Arg2       Arg3       Address\r\n");
	while (fnStackWalk64 (IMAGE_FILE_MACHINE_I386, hProcess, GetCurrentThread(), &frame, &context, NULL, (PFUNCTION_TABLE_ACCESS_ROUTINE64)fnSymFunctionTableAccess64, (PGET_MODULE_BASE_ROUTINE64)fnSymGetModuleBase64, NULL))
	{
		Q_strncpyz (szModuleName, "<unknown>", sizeof(szModuleName));
		fnEnumerateLoadedModules64 (hProcess, (PENUMLOADED_MODULES_CALLBACK64)EnumerateLoadedModulesProcInfo, (VOID *)(DWORD)frame.AddrPC.Offset);

		tempPointer = strrchr (szModuleName, '\\');
		if (tempPointer)
			tempPointer++;
		else
			tempPointer = szModuleName;

		fprintf (fhReport, ""HEX_VALUE_64" "HEX_VALUE_64" "HEX_VALUE_32" "HEX_VALUE_32" "HEX_VALUE_32" "HEX_VALUE_32" %-20s ! ",
			frame.AddrStack.Offset, frame.AddrPC.Offset, (DWORD)frame.Params[0], (DWORD)frame.Params[1], (DWORD)frame.Params[2], (DWORD)frame.Params[3], tempPointer);

		if (fnSymFromAddr (hProcess, frame.AddrPC.Offset, &fnOffset, symInfo) && !(symInfo->Flags & SYMFLAG_EXPORT))
			fprintf (fhReport, "%-24s + "HEX_VALUE_64" %lu\r\n", symInfo->Name, fnOffset, symInfo->Tag);
		else
			fprintf (fhReport, ""HEX_VALUE_64"\r\n", frame.AddrPC.Offset);
	}

	fprintf (fhReport, "\r\n");

	// Write a minidump
	if (fnMiniDumpWriteDump)
	{
		HANDLE	hFile;

		//GetTempPath (sizeof(dumpPath)-16, dumpPath);
		Q_snprintfz (dumpPath, sizeof(dumpPath), "%s\\CCCrashLog%.4d-%.2d-%.2d_%d.dmp", searchPath, timeInfo.wYear, timeInfo.wMonth, timeInfo.wDay, dumpNum);

		hFile = CreateFileA (dumpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			miniInfo.ClientPointers = TRUE;
			miniInfo.ExceptionPointers = exceptionInfo;
			miniInfo.ThreadId = GetCurrentThreadId ();
			if (fnMiniDumpWriteDump (hProcess, GetCurrentProcessId(), hFile, (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory|MiniDumpWithDataSegs), &miniInfo, NULL, NULL))
			{
				CloseHandle (hFile);

				FILE	*fh;
#ifdef USE_GZ
				CHAR	zPath[MAX_PATH];
#endif

				fh = fopen (dumpPath, "rb");
				if (fh)
#ifdef USE_GZ
				{
					gzFile	gz;

					Q_snprintfz (zPath, sizeof(zPath)-1, "%s\\CCCrashLog%.4d-%.2d-%.2d_%d.dmp.gz", searchPath, timeInfo.wYear, timeInfo.wMonth, timeInfo.wDay, dumpNum);
					gz = gzopen (zPath, "wb");
					if (gz)
					{
						size_t len;
						while ((len = fread (gzBuff, 1, sizeof(gzBuff), fh)) > 0)
							gzwrite (gz, gzBuff, (uint32)len);
						gzclose (gz);
#endif
						fclose (fh);
#ifdef USE_GZ
					}
				}
#endif
		
#ifdef USE_GZ
				DeleteFileA (dumpPath);
				Q_strncpyz (dumpPath, zPath, sizeof(dumpPath));
#endif

				fprintf (fhReport, "A "
#ifdef USE_GZ
					"minidump"
#else
					"dump"
#endif
					" was saved to %s.\r\nPlease include this file when posting a crash report.\r\n", dumpPath);
			}
			else
			{
				CloseHandle (hFile);
				DeleteFileA (dumpPath);
			}
		}
	}
	else
		fprintf (fhReport, "A minidump could not be created. Minidumps are only available on Windows XP or later.\r\n");

	// Done writing reports
	fclose (fhReport);
	LocalFree (symInfo);
	fnSymCleanup (hProcess);

	// Let the client know
	String temp = String::Format("Report written to: %s\r\nMini-dump written to %s\r\nPlease include both files if you submit manually!\r\n", reportPath, dumpPath);
	MessageBoxA (NULL, temp.CString(), "Unhandled Exception", MB_ICONEXCLAMATION | MB_OK);

#ifdef USE_CURL
	if (upload)
	{
		ret = MessageBox (NULL, "Would you like to automatically upload this crash report for analysis?\nPlease do not submit multiple reports of the same crash as this will only delay processing.", "Unhandled Exception", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
		if (ret == IDYES)
			EGLUploadCrashDump (dumpPath, reportPath);
		else
			MessageBox (NULL, "You have chosen to manually upload the crash report.\nPlease include BOTH the crash log and mini-dump when you post it on the EGL forums!\n", "Submit manually", MB_ICONEXCLAMATION|MB_OK);
	}
	else
#endif
	MessageBoxA (NULL, upMessage, "Unhandled Exception", MB_OK|MB_ICONEXCLAMATION);

	// Done
	FreeLibrary (hDbgHelp);
	if (hVersion)
		FreeLibrary (hVersion);

	return EXCEPTION_EXECUTE_HANDLER;
}
#endif