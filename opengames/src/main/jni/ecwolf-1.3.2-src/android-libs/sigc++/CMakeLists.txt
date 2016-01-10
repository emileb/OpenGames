cmake_minimum_required( VERSION 2.8 )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(sigc++ STATIC
	sigc++/adaptors/lambda/lambda.cc
	sigc++/functors/slot.cc
	sigc++/functors/slot_base.cc
	sigc++/connection.cc
	sigc++/signal.cc
	sigc++/signal_base.cc
	sigc++/trackable.cc
)
