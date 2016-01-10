cmake_minimum_required(VERSION 2.4)

option(WITH_LEARN "Compiles with patch dumping capabilities." OFF)

find_package(BZip2 REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${BZIP2_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})

if(WITH_LEARN)
	add_definitions(-DLEARN)
	set(ADDITIONAL_FILES bsdiff.c)
endif(WITH_LEARN)

add_executable(patchutil
	${ADDITIONAL_FILES}
	bspatch.c
	main.cpp
)

target_link_libraries(patchutil ${BZIP2_LIBRARIES} ${ZLIB_LIBRARIES})
