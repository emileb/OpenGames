cmake_minimum_required( VERSION 2.8 )

add_definitions(-DTIXML_USE_STL)

add_library(TinyXML STATIC
	tinystr.cpp
	tinyxml.cpp
	tinyxmlerror.cpp
	tinyxmlparser.cpp
)
