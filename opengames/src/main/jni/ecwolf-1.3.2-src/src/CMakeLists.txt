if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

get_target_property( UPDATEREVISION_EXE updaterevision LOCATION )

add_custom_target(revision_check ALL
	COMMAND ${UPDATEREVISION_EXE} . ${CMAKE_CURRENT_SOURCE_DIR}/svnrevision.h
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS updaterevision
)

include(CheckCXXSourceCompiles)
include(CheckFunctionExists)
include(FindPkgConfig)

include_directories(
	${ECWolf_SOURCE_DIR}/src
	${ECWolf_SOURCE_DIR}/src/g_shared
	${LZMA_INCLUDE_DIR}
	${ECWolf_SOURCE_DIR}/gdtoa
	${CMAKE_BINARY_DIR}/gdtoa
)

if(GPL)
	add_definitions(-DUSE_GPL)
	set(OPL_SOURCES dosbox/dbopl.cpp)

	if(USE_LIBTEXTSCREEN)
		add_definitions(-DUSE_TEXTSCREEN)
		include_directories(${ECWolf_SOURCE_DIR}/textscreen)
		set(EXTRA_LIBRARIES textscreen)
	else(USE_LIBTEXTSCREEN)
		set(EXTRA_LIBRARIES)
	endif(USE_LIBTEXTSCREEN)
else(GPL)
	set(OPL_SOURCES mame/fmopl.cpp)
	set(EXTRA_LIBRARIES)
endif(GPL)

find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
include_directories(${SDLMIXER_INCLUDE_DIR} ${SDL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} ${BZIP2_INCLUDE_DIR} ${JPEG_INCLUDE_DIR})

check_function_exists(stricmp STRICMP_EXISTS)
check_function_exists(strnicmp STRNICMP_EXISTS)
check_function_exists(atoll ATOLL_EXISTS)
if(NOT STRICMP_EXISTS)
	add_definitions(-Dstricmp=strcasecmp)
endif(NOT STRICMP_EXISTS)
if(NOT STRNICMP_EXISTS)
	add_definitions(-Dstrnicmp=strncasecmp)
endif(NOT STRNICMP_EXISTS)
if(NOT ATOLL_EXISTS)
	add_definitions(-Datoll=_atoi64)
endif(NOT ATOLL_EXISTS)

check_cxx_source_compiles(
	"#include <stdarg.h>
	int main() { va_list list1, list2; va_copy(list1, list2); return 0; }"
	HAS_VA_COPY )
if( NOT HAS_VA_COPY )
	check_cxx_source_compiles(
		"#include <stdarg.h>
		int main() { va_list list1, list2; __va_copy(list1, list2); return 0; }"
		HAS___VA_COPY )
	if( HAS___VA_COPY )
		add_definitions( -Dva_copy=__va_copy )
	else( HAS___VA_COPY )
		add_definitions( -DNO_VA_COPY )
	endif( HAS___VA_COPY )
endif( NOT HAS_VA_COPY )

if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
	set( CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-missing-field-initializers ${CMAKE_CXX_FLAGS}" )
endif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )

if(WIN32)
	set(NO_GTK ON)
	set(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} comctl32)
	add_definitions(-DWINDOWS -DNO_STDIO_REDIRECT)

	# RC file
	if(CMAKE_COMPILER_IS_GNUCXX)
		add_custom_command(
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/windows.o
			COMMAND windres -o ${CMAKE_CURRENT_BINARY_DIR}/windows.o -i ${ECWolf_SOURCE_DIR}/src/win32/windows.rc
			WORKING_DIRECTORY ${ECWolf_SOURCE_DIR}/src/win32
			COMMENT "Generating windows resources (windows.rc)"
		)

		set(EXTRA_FILES windows.o)
	else(CMAKE_COMPILER_IS_GNUCXX)
		set(EXTRA_FILES ${ECWolf_SOURCE_DIR}/src/win32/windows.rc)
	endif(CMAKE_COMPILER_IS_GNUCXX)

	set(EXTRA_FILES
		${EXTRA_FILES}
		win32/i_crash.cpp
		win32/i_main.cpp
		win32/wl_iwad_win32.cpp
	)
	if(${SDL_VERSION_STRING} VERSION_LESS "2.0.0")
		set(EXTRA_FILES ${EXTRA_FILES} sdl_winmain.cpp)
	endif()
elseif(APPLE OR ANDROID)
	set(NO_GTK ON)
else(WIN32)
	option( NO_GTK "Disable GTK+ dialogs (Not applicable to Windows)" )

	# Use GTK+ for the IWAD picker, if available.
	if( NOT NO_GTK )
		pkg_check_modules( GTK2 gtk+-2.0 )
		if( GTK2_FOUND )
			set( EXTRA_LIBRARIES ${EXTRA_LIBRARIES} ${GTK2_LIBRARIES} )
			include_directories( ${GTK2_INCLUDE_DIRS} )
		else( GTK2_FOUND )
			set( NO_GTK ON )
		endif( GTK2_FOUND )
	endif( NOT NO_GTK )
endif(WIN32)

if(NO_GTK)
	add_definitions(-DNO_GTK=1)
endif(NO_GTK)

if(APPLE)
	set(EXTRA_FILES
		macosx/filesys.mm
		macosx/wl_iwad_picker_cocoa.mm
	)

	if(${SDL_VERSION_STRING} VERSION_LESS "2.0.0")
		set(EXTRA_FILES ${EXTRA_FILES} macosx/SDLMain.m)
	endif()

	include(${CMAKE_CURRENT_SOURCE_DIR}/macosx/install.txt)

	option(BUILD_BUNDLE "Build a app bundle on Mac OS X" OFF)
	if(BUILD_BUNDLE)
		set(PLATFORM_EXE_FLAGS MACOSX_BUNDLE)
	endif(BUILD_BUNDLE)
elseif(ANDROID)
	set(EXTRA_FILES
		android/android-jni.cpp
		android/in_android.cpp
		android/wl_iwad_picker_android.cpp
	)

	include_directories(${TOUCHCONTROLS_INCLUDE_DIR} ${SIGCPP_INCLUDE_DIR} ${TINYXML_INCLUDE_DIR} ${LIBPNG_INCLUDE_DIR})
	add_definitions(-Dmalloc_usable_size=dlmalloc_usable_size)

	set(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} ${TOUCHCONTROLS_LIBRARY} log)
else(APPLE)
	set(PLATFORM_EXE_FLAGS "")
endif(APPLE)

# Glob for the header files so project generation can include them
file(GLOB HEADER_FILES
	g_shared/*.h
	g_wolf/*.h
	r_data/*.h
	resourcefiles/*.h
	sfmt/*.h
	textures/*.h
	thingdef/*.h
	*.h
)

set(SOURCE_FILES
	g_shared/a_deathcam.cpp
	g_shared/a_inventory.cpp
	g_shared/a_keys.cpp
	g_shared/a_patrolpoint.cpp
	g_shared/a_playerpawn.cpp
	g_shared/a_randomspawner.cpp
	g_wolf/a_spearofdestiny.cpp
	g_wolf/wolf_sbar.cpp
	r_2d/r_draw.cpp
	r_2d/r_drawt.cpp
	r_2d/r_main.cpp
	r_2d/r_things.cpp
	r_data/colormaps.cpp
	r_data/r_translate.cpp
	r_data/renderstyle.cpp
	resourcefiles/resourcefile.cpp
	resourcefiles/ancientzip.cpp
	resourcefiles/file_7z.cpp
	resourcefiles/file_audiot.cpp
	resourcefiles/file_directory.cpp
	resourcefiles/file_gamemaps.cpp
	resourcefiles/file_grp.cpp
	resourcefiles/file_pak.cpp
	resourcefiles/file_rff.cpp
	resourcefiles/file_rtl.cpp
	resourcefiles/file_lump.cpp
	resourcefiles/file_macbin.cpp
	resourcefiles/file_vgagraph.cpp
	resourcefiles/file_vswap.cpp
	resourcefiles/file_wad.cpp
	resourcefiles/file_zip.cpp
	resourcefiles/wolfmapcommon.cpp
	sfmt/SFMT.cpp
	textures/anim_switches.cpp
	textures/animations.cpp
	textures/automaptexture.cpp
	textures/bitmap.cpp
	textures/ddstexture.cpp
	textures/emptytexture.cpp
	textures/flattexture.cpp
	textures/imgztexture.cpp
	textures/jpegtexture.cpp
	textures/multipatchtexture.cpp
	textures/patchtexture.cpp
	textures/pcxtexture.cpp
	textures/pngtexture.cpp
	textures/rawpagetexture.cpp
	textures/rottflattexture.cpp
	textures/solidtexture.cpp
	textures/texture.cpp
	textures/texturemanager.cpp
	textures/tgatexture.cpp
	textures/warptexture.cpp
	textures/wolfrawtexture.cpp
	textures/wolfshapetexture.cpp
	thingdef/thingdef.cpp
	thingdef/thingdef_codeptr.cpp
	thingdef/thingdef_expression.cpp
	thingdef/thingdef_properties.cpp
	thingdef/thingdef_type.cpp
	actor.cpp
	am_map.cpp
	colormatcher.cpp
	config.cpp
	c_cvars.cpp
	dobject.cpp
	dobjgc.cpp
	farchive.cpp
	files.cpp
	filesys.cpp
	filesys_steam.cpp
	g_conversation.cpp
	g_intermission.cpp
	g_mapinfo.cpp
	gamemap.cpp
	gamemap_planes.cpp
	gamemap_uwmf.cpp
	id_ca.cpp
	id_in.cpp
	id_sd.cpp
	id_us_1.cpp
	id_vh.cpp
	id_vl.cpp
	language.cpp
	lnspec.cpp
	lumpremap.cpp
	m_alloc.cpp
	m_classes.cpp
	m_random.cpp
	m_png.cpp
	name.cpp
	p_switch.cpp
	r_sprites.cpp
	scanner.cpp
	sdlvideo.cpp
	sndinfo.cpp
	sndseq.cpp
	thinker.cpp
	v_draw.cpp
	v_font.cpp
	v_palette.cpp
	v_pfx.cpp
	v_text.cpp
	v_video.cpp
	w_wad.cpp
	weaponslots.cpp
	wl_act2.cpp
	wl_agent.cpp
	wl_atmos.cpp
	wl_cloudsky.cpp
	wl_debug.cpp
	wl_draw.cpp
	wl_floorceiling.cpp
	wl_game.cpp
	wl_inter.cpp
	wl_iwad.cpp
	wl_loadsave.cpp
	wl_main.cpp
	wl_menu.cpp
	wl_parallax.cpp
	wl_play.cpp
	wl_state.cpp
	wl_text.cpp
	zstrformat.cpp
	zstring.cpp
)

if(NOT ANDROID)
	add_executable(ecwolf ${PLATFORM_EXE_FLAGS}
		${HEADER_FILES}
		${OPL_SOURCES}
		${SOURCE_FILES}
		${EXTRA_FILES}
	)
else(NOT ANDROID)
	add_library(ecwolf SHARED
		${HEADER_FILES}
		${OPL_SOURCES}
		${SOURCE_FILES}
		${EXTRA_FILES}
	)

	# Link test
	add_executable(link-test android/link-test.c)
	target_link_libraries(link-test ecwolf)
endif(NOT ANDROID)

add_dependencies(ecwolf lzma gdtoa revision_check)
target_link_libraries(ecwolf ${EXTRA_LIBRARIES} ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${ZLIB_LIBRARY} ${BZIP2_LIBRARIES} ${JPEG_LIBRARIES} lzma gdtoa)
set_target_properties(ecwolf PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# Install
if(NOT ANDROID)
	install(TARGETS ecwolf BUNDLE DESTINATION ${OUTPUT_DIR} RUNTIME DESTINATION bin COMPONENT Runtime)
endif(NOT ANDROID)

# Project file source groupings
source_group("Sources" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/.+")
source_group("Game Objects\\Decorate" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/thingdef/.+")
source_group("Game Objects\\Shared" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/g_shared/.+")
source_group("Game Objects\\Wolf3D" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/g_wolf/.+")
source_group("Game Objects" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/(actor|dobj|thinker).+")
source_group("OPL Emulator\\DOSBox" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/dosbox/.+")
source_group("OPL Emulator\\MAME" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/mame/.+")
source_group("Renderer" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/(r_|wl_draw|wl_floorceiling|wl_parallax|wl_atmos|wl_cloudsky).+")
source_group("Renderer\\2D Drawing" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/r_2d/.+")
source_group("Renderer\\Data" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/r_data/.+")
source_group("Resource Files" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/resourcefiles/.+")
source_group("SFMT" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/sfmt/.+")
source_group("Textures" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/textures/.+")
source_group("Win32" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/win32/.+")
