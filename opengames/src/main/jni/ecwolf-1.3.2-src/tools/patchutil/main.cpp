#include <cstring>
#include <cmath>
#include <iostream>
#include <fstream>

#ifdef LEARN
#include <list>
#endif

#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <zlib.h>
#include <sys/stat.h>
//#include <sys/mman.h>
#include <dirent.h>

#define countof(x) (sizeof(x)/sizeof(x[0]))
#define START(x) static const unsigned char x[] = {
#define END(x) }; unsigned int x##size = countof(x)
#define P(x) {x, x##size}

#include "patchdata.cpp"
#include "patchdata_reg.cpp"
#include "patchdata_bs1.cpp"
#include "patchdata_bs6.cpp"
#include "patchdata_sd3.cpp"

using namespace std;

extern "C" int diff(int argc, char* argv[]);
extern "C" int patch(const unsigned char* data, unsigned int dataSize, const char* filename, const char* newfilename);

#if defined(_WIN32) || defined(__APPLE__)
extern "C" {

FILE *fmemopen_wrapper(void *data, size_t size, const char* mode)
{
	FILE *patchFile = fopen("patch.tmp", "wb");
	if(patchFile == NULL)
		return NULL;
	fwrite(data, 1, size, patchFile);
	fclose(patchFile);

	return fopen("patch.tmp", "rb");
}

#ifdef _WIN32
void errx(int eval, const char* error, ...)
{
	va_list list;
	va_start(list, error);
	vfprintf(stderr, error, list);
	va_end(list);
	exit(eval);
}
void err(int eval, const char* error, ...)
{
	va_list list;
	va_start(list, error);
	vfprintf(stderr, error, list);
	va_end(list);
	exit(eval);
}
#endif

}
#endif

struct PatchData
{
	const unsigned char*	data;
	unsigned int			size;
};
#define NULLP {NULL, 0}

struct FileChecksum
{
	unsigned int	crc;
	int				size;
	const char*		filename;
	const char*		newFilename;
	PatchData		patchData;
	bool			optional;
};

#define MAX_FILES 15
struct OldDataSet
{
	int				numFiles;
	FileChecksum	fileChecksum[MAX_FILES];
	const char*		setName;
} dataSets[] =
{
	{
		12,
		{
			{2636647909u, 1280, "audiohed.bs1", NULL, NULLP, false},
			{4257831234u, 1024, "vgadict.bs1", NULL, P(vgadict10bssw), false},
			//{241924712u, 276784, "sanim.bs1", NULL, NULLP, false},
			//{3189561458u, 18977, "ianim.bs1", NULL, NULLP, false},
			{501261956u, 9799, "jm_error.h", NULL, P(jm_error10bssw), true},
			{4293568872u, 600, "vgahead.bs1", NULL, P(vgahead10bssw), false},
			{387229244u, 124507, "bs_aog.exe", NULL, P(bs_aog10bssw), true},
			{3963407722u, 12203, "bs-help.exe", NULL, P(bshelp10bssw), true},
			{31637588u, 162268, "audiot.bs1", NULL, NULLP, false},
			{367123052u, 10748, "jamerr.exe", NULL, P(jamerr10bssw), true},
			//{4041681913u, 101, "bstone.bat", NULL, NULLP, true},
			{2920060064u, 620645, "vgagraph.bs1", NULL, P(vgagraph10bssw), false},
			{232286169u, 1996008, "vswap.bs1", NULL, P(vswap10bssw), false},
			{2383999590u, 91100, "maptemp.bs1", NULL, P(maptemp10bssw), false},
			{1223831186u, 834, "maphead.bs1", NULL, P(maphead10bssw), false}
		},
		"Blake Stone Shareware v1.0"
	},
	{
		13,
		{
			{2636647909u, 1280, "audiohed.bs1", NULL, P(audiohed20bssw), false},
			{2527841072u, 1024, "vgadict.bs1", NULL, P(vgadict20bssw), false},
			{1076595203u, 9883, "jm_error.h", NULL, NULLP, true},
			{1940699079u, 633, "vgahead.bs1", NULL, P(vgahead20bssw), false},
			{744711291u, 137680, "bs_aog.exe", NULL, P(bs_aog20bssw), true},
			{2649321649u, 17323, "bs-help.exe", NULL, P(bshelp20bssw), true},
			{31637588u, 162268, "audiot.bs1", NULL, P(audiot20bssw), false},
			{968162678u, 20786, "jamerr.exe", NULL, P(jamerr20bssw), true},
			{2489463136u, 623655, "vgagraph.bs1", NULL, P(vgagraph20bssw), false},
			{466066612u, 1996008, "vswap.bs1", NULL, NULLP, false},
			{1317237958u, 73724, "setblast.exe", NULL, NULLP, true},
			{1566297908u, 90786, "maptemp.bs1", NULL, NULLP, false},
			{1215957204u, 834, "maphead.bs1", NULL, NULLP, false}
		},
		"Blake Stone Shareware v2.0"
	},
	{
		13,
		{
			{2476200822u, 1280, "audiohed.bs1", NULL, NULLP, false},
			{3930217900u, 1024, "vgadict.bs1", NULL, P(vgadict21bssw), false},
			{1076595203u, 9883, "jm_error.h", NULL, NULLP, true},
			{3063551610u, 633, "vgahead.bs1", NULL, P(vgahead21bssw), false},
			{3208303576u, 138308, "bs_aog.exe", NULL, P(bs_aog21bssw), true},
			{3628097170u, 17563, "bs-help.exe", NULL, P(bshelp21bssw), true},
			{3006928850u, 267636, "audiot.bs1", NULL, NULLP, false},
			{1214717625u, 11212, "jamerr.exe", NULL, NULLP, true},
			{169675525u, 623562, "vgagraph.bs1", NULL, P(vgagraph21bssw), false},
			{466066612u, 1996008, "vswap.bs1", NULL, NULLP, false},
			{1317237958u, 73724, "setblast.exe", NULL, P(setblast21bssw), true},
			{1566297908u, 90786, "maptemp.bs1", NULL, NULLP, false},
			{1215957204u, 834, "maphead.bs1", NULL, NULLP, false}
		},
		"Blake Stone Shareware v2.1"
	},
	{
		13,
		{
			{2476200822u, 1280, "audiohed.bs1", NULL, NULLP, false},
			{3542687473u, 1024, "vgadict.bs1", NULL, NULLP, false},
			{1076595203u, 9883, "jm_error.h", NULL, NULLP, true},
			{3984136128u, 639, "vgahead.bs1", NULL, NULLP, false},
			{2977874630u, 138340, "bs_aog.exe", NULL, NULLP, true},
			{3038859855u, 17650, "bs-help.exe", NULL, NULLP, true},
			{3006928850u, 267636, "audiot.bs1", NULL, NULLP, false},
			{1214717625u, 11212, "jamerr.exe", NULL, NULLP, true},
			{1548084969u, 729462, "vgagraph.bs1", NULL, NULLP, false},
			{466066612u, 1996008, "vswap.bs1", NULL, NULLP, false},
			{4014968045u, 76751, "setblast.exe", NULL, NULLP, true},
			{1566297908u, 90786, "maptemp.bs1", NULL, NULLP, false},
			{1215957204u, 834, "maphead.bs1", NULL, NULLP, false}
		},
		"Blake Stone Shareware v3.0"
	},
	{
		14,
		{
			{3006928850u, 267636, "audiot.bs6", NULL, NULLP, false},
			//{2332524962u, 219918, "ganim.bs6", NULL, NULLP, false},
			//{3189561458u, 18977, "ianim.bs6", NULL, NULLP, false},
			{3848338552u, 520920, "maptemp.bs6", NULL, P(maptemp10bsreg), false},
			{501261956u, 9799, "jm_error.h", NULL, P(jm_error10bsreg), true},
			{2763409265u, 8939, "bshint.exe", "bs-hint.exe", P(bshint10bsreg), true},
			{2476200822u, 1280, "audiohed.bs6", NULL, NULLP, false},
			{3335382708u, 596336, "vgagraph.bs6", NULL, P(vgagraph10bsreg), false},
			{4237374826u, 127402, "bs_aog.exe", NULL, P(bs_aog10bsreg), true},
			//{91985267u, 186221, "eanim.bs6", NULL, NULLP},
			{3963407722u, 12203, "bshelp.exe", "bs-help.exe", P(bshelp10bsreg), true},
			{2822521763u, 639, "vgahead.bs6", NULL, P(vgahead10bsreg), false},
			{3911856855u, 1024, "vgadict.bs6", NULL, P(vgadict10bsreg), false},
			{1453722105u, 10748, "jamerr.exe", NULL, P(jamerr10bsreg), true},
			//{4041681913u, 101, "bstone.bat", NULL, NULLP, true},
			{3784759483u, 834, "maphead.bs6", NULL, P(maphead10bsreg), false},
			{2668177633u, 2878696, "svswap.bs6", NULL, P(svswap10bsreg), false},
			{3139577876u, 2767592, "vswap.bs6", NULL, P(vswap10bsreg), false}
			//{241924712u, 276784, "sanim.bs6", NULL, NULLP, false}
		},
		"Blake Stone: Aliens of Gold v1.0"
	},
	{
		15,
		{
			{3006928850u, 267636, "audiot.bs6", NULL, NULLP, false},
			{1943305533u, 526196, "maptemp.bs6", NULL, NULLP, false},
			{1076595203u, 9883, "jm_error.h", NULL, NULLP, true},
			{2948904724u, 9275, "bs-hint.exe", NULL, P(bshint20bsreg), true},
			{2476200822u, 1280, "audiohed.bs6", NULL, NULLP, false},
			{1397588851u, 599393, "vgagraph.bs6", NULL, P(vgagraph20bsreg), false},
			{4135401649u, 139197, "bs_aog.exe", NULL, P(bs_aog20bsreg), true},
			{2649321649u, 17323, "bs-help.exe", NULL, P(bshelp20bsreg), true},
			{1977437756u, 672, "vgahead.bs6", NULL, P(vgahead20bsreg), false},
			{3563729630u, 1024, "vgadict.bs6", NULL, P(vgadict20bsreg), false},
			{968162678u, 20786, "jamerr.exe", NULL, P(jamerr20bsreg), true},
			{3639110956u, 834, "maphead.bs6", NULL, NULLP, false},
			{193603092u, 2878696, "svswap.bs6", NULL, NULLP, false},
			{111090996u, 2767080, "vswap.bs6", NULL, NULLP, false},
			{1317237958u, 73724, "setblast.exe", NULL, NULLP, true}
		},
		"Blake Stone: Aliens of Gold v2.0"
	},
	{
		15,
		{
			{3006928850u, 267636, "audiot.bs6", NULL, NULLP, false},
			{1943305533u, 526196, "maptemp.bs6", NULL, NULLP, false},
			{1076595203u, 9883, "jm_error.h", NULL, NULLP, true},
			{2066597924u, 9499, "bs-hint.exe", NULL, NULLP, true},
			{2476200822u, 1280, "audiohed.bs6", NULL, NULLP, false},
			{2912368398u, 599397, "vgagraph.bs6", NULL, P(vgagraph21bsreg), false},
			{601520986u, 139809, "bs_aog.exe", NULL, P(bs_aog21bsreg), true},
			{3628097170u, 17563, "bs-help.exe", NULL, P(bshelp21bsreg), true},
			{654457892u, 672, "vgahead.bs6", NULL, P(vgahead21bsreg), false},
			{91230768u, 1024, "vgadict.bs6", NULL, P(vgadict21bsreg), false},
			{1214717625u, 11212, "jamerr.exe", NULL, NULLP, true},
			{3639110956u, 834, "maphead.bs6", NULL, NULLP, false},
			{193603092u, 2878696, "svswap.bs6", NULL, NULLP, false},
			{111090996u, 2767080, "vswap.bs6", NULL, NULLP, false},
			{1317237958u, 73724, "setblast.exe", NULL, P(setblast21bsreg), true}
		},
		"Blake Stone: Aliens of Gold v2.1"
	},
	{
		15,
		{
			{3006928850u, 267636, "audiot.bs6", NULL, NULLP, false},
			{1943305533u, 526196, "maptemp.bs6", NULL, NULLP, false},
			{1076595203u, 9883, "jm_error.h", NULL, NULLP, true},
			{2066597924u, 9499, "bs-hint.exe", NULL, NULLP, true},
			{2476200822u, 1280, "audiohed.bs6", NULL, NULLP, false},
			{2590218198u, 709440, "vgagraph.bs6", NULL, NULLP, false},
			{2084573591u, 139809, "bs_aog.exe", NULL, NULLP, true},
			{3038859855u, 17650, "bs-help.exe", NULL, NULLP, true},
			{2723953638u, 678, "vgahead.bs6", NULL, NULLP, false},
			{4254323192u, 1024, "vgadict.bs6", NULL, NULLP, false},
			{1214717625u, 11212, "jamerr.exe", NULL, NULLP, true},
			{3639110956u, 834, "maphead.bs6", NULL, NULLP, false},
			{193603092u, 2878696, "svswap.bs6", NULL, NULLP, false},
			{111090996u, 2767080, "vswap.bs6", NULL, NULLP, false},
			{4014968045u, 76751, "setblast.exe", NULL, NULLP, true}
		},
		"Blake Stone: Aliens of Gold v3.0"
	},
	{
		9,
		{
			{2142400748u, 304415, "audiot.vsi", NULL, NULLP, false},
			{1639609375u, 3195112, "vswap.vsi", NULL, NULLP, false},
			{1145553870u, 747, "vgahead.vsi", NULL, NULLP, false},
			{2824396135u, 1288, "audiohed.vsi", NULL, NULLP, false},
			//{3189561458u, 18977, "ianim.vsi", NULL, NULLP, false},
			//{2781635409u, 102, "planet.bat", NULL, NULLP,  true},
			{3915873607u, 710699, "vgagraph.vsi", NULL, NULLP, false},
			//{1682344223u, 370212, "eanim.vsi", NULL, NULLP, false},
			//{3556882997u, 22069, "jamerr.exe", NULL, NULLP, true},
			{3829735414u, 834, "maphead.vsi", NULL, NULLP, false},
			//{1056702345u, 18641, "pshelp.exe", NULL, NULLP, true},
			{3602237331u, 1024, "vgadict.vsi", NULL, NULLP, false},
			{2018431833u, 167158, "maptemp.vsi", NULL, NULLP, false},
			//{4014968045u, 76751, "setblast.exe", NULL, NULLP, true},
			{274830760u, 144374, "bs_fire.exe", NULL, P(bs_fire10), true},
		},
		"Blake Stone: Planet Strike v1.0"
	},
	{
		9,
		{
			{2142400748u, 304415, "AUDIOT.VSI", NULL, NULLP, false},
			{1639609375u, 3195112, "VSWAP.VSI", NULL, NULLP, false},
			{1145553870u, 747, "VGAHEAD.VSI", NULL, NULLP, false},
			{2824396135u, 1288, "AUDIOHED.VSI", NULL, NULLP, false},
			{3915873607u, 710699, "VGAGRAPH.VSI", NULL, NULLP, false},
			{3829735414u, 834, "MAPHEAD.VSI", NULL, NULLP, false},
			{3602237331u, 1024, "VGADICT.VSI", NULL, NULLP, false},
			{2018431833u, 167158, "MAPTEMP.VSI", NULL, NULLP, false},
			{4163451519u, 144390, "BS_FIRE.EXE", NULL, NULLP, true},
		},
		"Blake Stone: Planet Strike v1.01"
	},
	{
		3,
		{
			{31150618u, 1881760, "vswap.sd3", NULL, P(vswapuac), false},
			{194254884u, 148534, "gamemaps.sd3", NULL, P(gamemapsuac), false},
			{1314236310u, 86, "maphead.sd3", NULL, P(mapheaduac), false},
		},
		"Mission 3: Ultimate Challenge (UAC)"
	},
	{
		3,
		{
			{1713865351u, 1881760, "vswap.sd3", NULL, NULLP, false},
			{3397062772u, 148546, "gamemaps.sd3", NULL, NULLP, false},
			{2435737438u, 86, "maphead.sd3", NULL, NULLP, false},
		},
		"Mission 3: Ultimate Challenge"
	},
	{
		9,
		{
			{4096914596u, 295394, "vgagraph.wl1", NULL, P(vgagraph10sw), false},
			{4079205602u, 56618, "maptemp.wl1", "gamemaps.wl1", P(maptemp10sw), false},
			{944411798u, 988, "audiohed.wl1", NULL, P(audiohed10sw), false},
			{3814678634u, 402, "maphead.wl1", NULL, P(maphead10sw), false},
			{1818995766u, 94379, "wolf3d.exe", NULL, P(wolf3d10sw), true},
			{932624354u, 1024, "vgadict.wl1", NULL, P(vgadict10sw), false},
			{4185907760u, 707072, "vswap.wl1", NULL, P(vswap10sw), false},
			{4132450727u, 130696, "audiot.wl1", NULL, P(audiot10sw), false},
			{323179087u, 447, "vgahead.wl1", NULL, P(vgahead10sw), false},
		},
		"Wolfenstein 3D Shareware v1.0"
	},
	{
		9,
		{
			{4239209784u, 1024, "vgadict.wl1", NULL, P(vgadict11sw), false},
			{2001789396u, 27448, "gamemaps.wl1", NULL, P(gamemaps11sw), false},
			{1935056717u, 132613, "audiot.wl1", NULL, P(audiot11sw), false},
			{3203508363u, 97605, "wolf3d.exe", NULL, P(wolf3d11sw), true},
			{3551669308u, 462, "vgahead.wl1", NULL, P(vgahead11sw), false},
			{542266284u, 402, "maphead.wl1", NULL, P(maphead11sw), false},
			{3146117708u, 742912, "vswap.wl1", NULL, P(vswap11sw), false},
			{2629083114u, 296826, "vgagraph.wl1", NULL, P(vgagraph11sw), false},
			{1376992708u, 1156, "audiohed.wl1", NULL, NULLP, false},
		},
		"Wolfenstein 3D Shareware v1.1"
	},
	{
		9,
		{
			{2006621000u, 1024, "vgadict.wl1", NULL, P(vgadict12sw), false},
			{391230369u, 27450, "gamemaps.wl1", NULL, P(gamemaps12sw), false},
			{2277491609u, 132613, "audiot.wl1", NULL, NULLP, false},
			{1036300494u, 97676, "wolf3d.exe", NULL, P(wolf3d12sw), true},
			{3668255810u, 462, "vgahead.wl1", NULL, P(vgahead12sw), false},
			{256482848u, 402, "maphead.wl1", NULL, P(maphead12sw), false},
			{1887524789u, 742912, "vswap.wl1", NULL, P(vswap12sw), false},
			{4155133539u, 296813, "vgagraph.wl1", NULL, P(vgagraph12sw), false},
			{1376992708u, 1156, "audiohed.wl1", NULL, NULLP, false},
		},
		"Wolfenstein 3D Shareware v1.2"
	},
	{
		9,
		{
			{898283639u, 1024, "vgadict.wl1", NULL, NULLP, false},
			{3428045633u, 27425, "gamemaps.wl1", NULL, NULLP, false},
			{2277491609u, 132613, "audiot.wl1", NULL, NULLP, false},
			{1706878953u, 109959, "wolf3d.exe", NULL, P(wolf3d14sw), true},
			{2785712368u, 471, "vgahead.wl1", NULL, NULLP, false},
			{143619409u, 402, "maphead.wl1", NULL, NULLP, false},
			{2247076949u, 742912, "vswap.wl1", NULL, NULLP, false},
			{1953343984u, 326568, "vgagraph.wl1", NULL, NULLP, false},
			{1376992708u, 1156, "audiohed.wl1", NULL, NULLP, false},
		},
		"Wolfenstein 3D Shareware v1.4"
	},
	{
		9,
		{
			{898283639u, 1024, "vgadict.wl1", NULL, NULLP, false},
			{3428045633u, 27425, "gamemaps.wl1", NULL, NULLP, false},
			{2277491609u, 132613, "audiot.wl1", NULL, NULLP, false},
			{557669750u, 109959, "wolf3d.exe", NULL, NULLP, true},
			{2785712368u, 471, "vgahead.wl1", NULL, NULLP, false},
			{143619409u, 402, "maphead.wl1", NULL, NULLP, false},
			{2247076949u, 742912, "vswap.wl1", NULL, NULLP, false},
			{1953343984u, 326568, "vgagraph.wl1", NULL, NULLP, false},
			{1376992708u, 1156, "audiohed.wl1", NULL, NULLP, false},
		},
		"Wolfenstein 3D Shareware v1.4g"
	},
	{
		9,
		{
			{90883035u, 1156, "audiohed.wl6", NULL, NULLP, false},
			{361091845u, 317049, "audiot.wl6", NULL, NULLP, false},
			{3682273634u, 150746, "gamemaps.wl6", NULL, P(gamemaps11reg), false},
			{122388615u, 402, "maphead.wl6", NULL, P(maphead11reg), false},
			{677936410u, 1024, "vgadict.wl6", NULL, NULLP, false},
			{3788928607u, 302374, "vgagraph.wl6", NULL, NULLP, false},
			{1404351874u, 477, "vgahead.wl6", NULL, NULLP, false},
			{2550006541u, 1545400, "vswap.wl6", NULL, NULLP, false},
			{4004765388u, 98402, "wolf3d.exe", NULL, NULLP, true},
		},
		"Wolfenstein 3D Registered v1.1"
	},
	{
		9,
		{
			{90883035u, 1156, "audiohed.wl6", NULL, P(audiohed12reg), false},
			{361091845u, 317049, "audiot.wl6", NULL, P(audiot12reg), false},
			{271173415u, 150758, "gamemaps.wl6", NULL, P(gamemaps12reg), false},
			{1581501166u, 402, "maphead.wl6", NULL, P(maphead12reg), false},
			{677936410u, 1024, "vgadict.wl6", NULL, P(vgadict12reg), false},
			{3788928607u, 302374, "vgagraph.wl6", NULL, P(vgagraph12reg), false},
			{1404351874u, 477, "vgahead.wl6", NULL, P(vgahead12reg), false},
			{2550006541u, 1545400, "vswap.wl6", NULL, P(vswap12reg), false},
			{4004765388u, 98402, "wolf3d.exe", NULL, P(wolf3d12reg), true},
		},
		"Wolfenstein 3D Registered v1.2"
	},
	{
		9,
		{
			{792447856u, 1156, "audiohed.wl6", NULL, NULLP, false},
			{3349085516u, 320209, "audiot.wl6", NULL, NULLP, false},
			{2913323047u, 150652, "gamemaps.wl6", NULL, NULLP, false},
			{22545385u, 402, "maphead.wl6", NULL, NULLP, false},
			{199389440u, 1024, "vgadict.wl6", NULL, NULLP, false},
			{1789003941u, 334506, "vgagraph.wl6", NULL, NULLP, false},
			{3990717432u, 486, "vgahead.wl6", NULL, NULLP, false},
			{2241761276u, 1545400, "vswap.wl6", NULL, NULLP, false},
			{2140233287u, 110715, "wolf3d.exe", NULL, P(wolf3d14regap), true},
		},
		"Wolfenstein 3D Registered Apogee v1.4"
	},
	{
		9,
		{
			{792447856u, 1156, "audiohed.wl6", NULL, NULLP, false},
			{3349085516u, 320209, "audiot.wl6", NULL, NULLP, false},
			{2913323047u, 150652, "gamemaps.wl6", NULL, NULLP, false},
			{22545385u, 402, "maphead.wl6", NULL, NULLP, false},
			{199389440u, 1024, "vgadict.wl6", NULL, P(vgadict14regapg), false},
			{1789003941u, 334506, "vgagraph.wl6", NULL, P(vgagraph14regapg), false},
			{3990717432u, 486, "vgahead.wl6", NULL, P(vgahead14regapg), false},
			{2241761276u, 1545400, "vswap.wl6", NULL, NULLP, false},
			{4225834675u, 110715, "Wolf3d.exe", NULL, P(wolf3d14regapg), true},
		},
		"Wolfenstein 3D Registered Apogee v1.4g"
	},
	{
		9,
		{
			{792447856u, 1156, "audiohed.wl6", NULL, NULLP, false},
			{3349085516u, 320209, "audiot.wl6", NULL, NULLP, false},
			{2913323047u, 150652, "gamemaps.wl6", NULL, NULLP, false},
			{22545385u, 402, "maphead.wl6", NULL, NULLP, false},
			{2336936184u, 1024, "vgadict.wl6", NULL, NULLP, false},
			{3772718752u, 276096, "vgagraph.wl6", NULL, NULLP, false},
			{669923268u, 450, "vgahead.wl6", NULL, NULLP, false},
			{2241761276u, 1545400, "vswap.wl6", NULL, NULLP, false},
			{3479572251u, 259310, "wolf3d.exe", NULL, P(wolf3d14reggt1), true},
		},
		"Wolfenstein 3D Registered GT #1 v1.4"
	},
	{
		9,
		{
			{792447856u, 1156, "audiohed.wl6", NULL, NULLP, false},
			{3349085516u, 320209, "audiot.wl6", NULL, NULLP, false},
			{2913323047u, 150652, "gamemaps.wl6", NULL, NULLP, false},
			{22545385u, 402, "maphead.wl6", NULL, NULLP, false},
			{2336936184u, 1024, "vgadict.wl6", NULL, P(vgadict14regid), false},
			{3772718752u, 276096, "vgagraph.wl6", NULL, P(vgagraph14regid), false},
			{669923268u, 450, "vgahead.wl6", NULL, P(vgahead14regid), false},
			{2241761276u, 1545400, "vswap.wl6", NULL, P(vswap14regid), false},
			{3738828655u, 259310, "wolf3d.exe", NULL, P(wolf3d14regid), true},
		},
		"Wolfenstein 3D Registered ID Software v1.4"
	},
	{
		9,
		{
			{792447856u, 1156, "audiohed.wl6", NULL, NULLP, false},
			{3349085516u, 320209, "audiot.wl6", NULL, NULLP, false},
			{2913323047u, 150652, "gamemaps.wl6", NULL, NULLP, false},
			{22545385u, 402, "maphead.wl6", NULL, NULLP, false},
			{2854356302u, 1024, "vgadict.wl6", NULL, NULLP, false},
			{2763120157u, 275774, "vgagraph.wl6", NULL, NULLP, false},
			{2871739603u, 450, "vgahead.wl6", NULL, NULLP, false},
			{3859859044u, 1544376, "vswap.wl6", NULL, NULLP, false},
			{4242494513u, 109589, "wolf3d.exe", NULL, P(wolf3d14reggt2), true},
		},
		"Wolfenstein 3D Registered GT #2 v1.4"
	},
	{
		9,
		{
			{792447856u, 1156, "audiohed.wl6", NULL, NULLP, false},
			{3349085516u, 320209, "audiot.wl6", NULL, NULLP, false},
			{2913323047u, 150652, "gamemaps.wl6", NULL, NULLP, false},
			{22545385u, 402, "maphead.wl6", NULL, NULLP, false},
			{2854356302u, 1024, "vgadict.wl6", NULL, NULLP, false},
			{2763120157u, 275774, "vgagraph.wl6", NULL, NULLP, false},
			{2871739603u, 450, "vgahead.wl6", NULL, NULLP, false},
			{3859859044u, 1544376, "vswap.wl6", NULL, NULLP, false},
			{1837206974u, 108779, "wolf3d.exe", NULL, NULLP, true},
		},
		"Wolfenstein 3D Registered v1.4"
	}
};

unsigned int calcCrc32(const char* file, unsigned int size)
{
	fstream in(file, ios_base::in|ios_base::binary);
	char* fileData = new char[size];
	in.read(fileData, size);
	in.close();
	unsigned int crc = crc32(0, Z_NULL, 0);
	crc = crc32(crc, (Bytef*)fileData, size);
	in.close();
	delete[] fileData;
	return crc;
}

int main(int argc, char* argv[])
{
#ifdef LEARN
	if(argc == 3 && strcmp(argv[1], "learn") == 0)
	{
		DIR *oldDir = opendir("old");
		if(oldDir == NULL)
		{
			cout << "Place original files in old/ and new files in new/\n";
			return 0;
		}

		fstream out("patch.txt", ios_base::out|ios_base::binary);
		list<string> fileList;
		out << "{\n";
		dirent *file;
		int fileCount = 0;
		while((file = readdir(oldDir)))
		{
			if(file->d_name[0] == '.')
				continue;

			fileCount++;
		}
		closedir(oldDir);
		out << "\t" << fileCount << ",\n\t{\n";
		oldDir = opendir("old");
		while((file = readdir(oldDir)))
		{
			if(file->d_name[0] == '.')
				continue;

			string thisFile = string("old/") + file->d_name;
			fileList.push_back(file->d_name);
			struct stat *fileInfo = new struct stat;
			stat(thisFile.c_str(), fileInfo);
			unsigned int crc = calcCrc32(thisFile.c_str(), fileInfo->st_size);
			out << "\t\t{" << crc << ", " << (int) fileInfo->st_size << ", \"" << file->d_name << "\"},\n";
			delete fileInfo;
		}
		out << "\t},\n\t\"" << argv[2] << "\"\n}\n";
		out.close();

		closedir(oldDir);

		FILE *outf = fopen("patch.txt", "ab");
		const char* dargs[4] = {"patchutil", "", "", "tmp"};
		for(list<string>::const_iterator iter = fileList.begin();iter != fileList.end();iter++)
		{
			fprintf(outf, "//%s\n", iter->c_str());
			string oldFile = string("old/") + *iter;
			string newFile = string("new/") + *iter;
			dargs[1] = oldFile.c_str();
			dargs[2] = newFile.c_str();
			diff(4, (char**)dargs);

			FILE *in = fopen("tmp", "rb");
			char data[4096];
			while(feof(in) == 0)
			{
				size_t byteCount = fread(data, 1, 4096, in);
				for(size_t i = 0;i < byteCount;i++)
				{
					fprintf(outf, "0x%02X,", (unsigned char)data[i]);
					if(i%16 == 15)
						fprintf(outf, "\n");
				}
			}
			fclose(in);
			fprintf(outf, "\n");
		}
		fclose(outf);
		return 0;
	}
#endif

	cout << "Wolfenstein 3D Universal Patching Utility\n\n"
		"This utility recognizes the following (X marks latest):\n";
	for(unsigned int i = 0;i < countof(dataSets);++i)
	{
		bool latest = true;
		for(int j = 0;j < dataSets[i].numFiles;++j)
		{
			if(dataSets[i].fileChecksum[j].patchData.data)
			{
				latest = false;
				break;
			}
		}
		if(latest)
			cout << "  X " << dataSets[i].setName << "\n\n";
		else
			cout << "  * " << dataSets[i].setName << "\n";
	}
	cout << "This program will identify if you have an old version of a supported game, if\n"
		"so it will patch up to the version considered to be the latest. To patch, only\n"
		"the game data files are needed. Optionally, if the game executable are present\n"
		"they will be patched as well.\n\n"
		"This patch should be run in the same directory as the data you wish to patch.\n\n";

	char response;
	do
	{
		cout << "Continue? [Y/N] ";
		response = cin.get();
		if(response != '\n')
			cin.ignore(4096, '\n');
		if(response == 'n' || response == 'N')
			exit(0);
	}
	while(response != 'y' && response != 'Y');

	for(unsigned int i = 0;i < countof(dataSets);++i)
	{
		bool identified = true;
		bool applyPatch[MAX_FILES];
		memset(applyPatch, 1, MAX_FILES*sizeof(bool));
		struct stat *fileInfo = new struct stat;
		for(int f = 0;f < dataSets[i].numFiles;f++)
		{
			if(stat(dataSets[i].fileChecksum[f].filename, fileInfo) != 0)
			{
				if(dataSets[i].fileChecksum[f].optional)
				{
					applyPatch[f] = false;
					continue;
				}
				identified = false;
				break;
			}
			if(fileInfo->st_size != dataSets[i].fileChecksum[f].size)
			{
				if(dataSets[i].fileChecksum[f].optional)
				{
					applyPatch[f] = false;
					continue;
				}
				identified = false;
				break;
			}
			unsigned int crc = calcCrc32(dataSets[i].fileChecksum[f].filename, fileInfo->st_size);
			if(crc != dataSets[i].fileChecksum[f].crc)
			{
				if(dataSets[i].fileChecksum[f].optional)
				{
					applyPatch[f] = false;
					continue;
				}
				identified = false;
				break;
			}
		}
		delete fileInfo;

		if(!identified)
			continue;

		cout << "Patching: " << dataSets[i].setName << "\n";
		for(int f = 0;f < dataSets[i].numFiles;f++)
		{
			if(dataSets[i].fileChecksum[f].patchData.data == NULL || !applyPatch[f])
				continue;
			cout << "\t->" << dataSets[i].fileChecksum[f].filename << "\n";
			patch(dataSets[i].fileChecksum[f].patchData.data, dataSets[i].fileChecksum[f].patchData.size, dataSets[i].fileChecksum[f].filename, dataSets[i].fileChecksum[f].newFilename ? dataSets[i].fileChecksum[f].newFilename : dataSets[i].fileChecksum[f].filename);
		}
	}

	cout << "\nPatching complete! If you do not see any lines above that read \"Patching: ...\"\n"
			"then your data was not recognized. This probably indicates that your data is\n"
			"modified or otherwise corrupted and needs to be reinstalled.\n\n"
			"Press enter to dismiss...";
	cin.get();

	return 0;
}
