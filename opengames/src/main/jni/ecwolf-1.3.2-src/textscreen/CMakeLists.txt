cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if( CMAKE_COMPILER_IS_GNUC )
	set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fomit-frame-pointer" )
endif( CMAKE_COMPILER_IS_GNUC )

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})

add_library( textscreen
	txt_radiobutton.c
	txt_window_action.c
	txt_io.c
	txt_strut.c
	txt_inputbox.c
	txt_table.c
	txt_dropdown.c
	txt_desktop.c
	txt_widget.c
	txt_window.c
	txt_scrollpane.c
	txt_spinctrl.c
	txt_separator.c
	txt_button.c
	txt_utf8.c
	txt_sdl.c
	txt_checkbox.c
	txt_fileselect.c
	txt_gui.c
	txt_label.c
)
target_link_libraries( textscreen ${SDL_LIBRARY} )
