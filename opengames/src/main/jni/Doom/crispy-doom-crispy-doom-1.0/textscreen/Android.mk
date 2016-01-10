
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := textscreen_choc


LOCAL_CFLAGS :=  -O2 -g -Wall -Wdeclaration-after-statement  -D_GNU_SOURCE=1 -D_REENTRANT

LOCAL_LDLIBS += -lGLESv1_CM -lEGL

LOCAL_LDLIBS += -llog

LOCAL_CFLAGS += -march=armv7-a -mfloat-abi=softfp
LOCAL_LDLIBS += -Wl,--fix-cortex-a8


LOCAL_C_INCLUDES :=  $(TOP_DIR)/SDL/include $(TOP_DIR)/SDL_net/include $(TOP_DIR)/SDL_mixer $(TOP_DIR)/SDL_image  $(TOP_DIR)/libpng $(TOP_DIR)/jpeg8d $(TOP_DIR)/TouchControls  $(TOP_DIR)/TinyXML $(TOP_DIR)/
LOCAL_C_INCLUDES +=  $(LOCAL_PATH)/../src

libtextscreen_a_SOURCES = \
	textscreen.h                                      \
	txt_checkbox.c           txt_checkbox.h           \
	txt_desktop.c            txt_desktop.h            \
	txt_dropdown.c           txt_dropdown.h           \
	txt_fileselect.c         txt_fileselect.h         \
	txt_gui.c                txt_gui.h                \
	txt_inputbox.c           txt_inputbox.h           \
	txt_io.c                 txt_io.h                 \
	                         txt_main.h               \
	txt_button.c             txt_button.h             \
	txt_label.c              txt_label.h              \
	txt_radiobutton.c        txt_radiobutton.h        \
	txt_scrollpane.c         txt_scrollpane.h         \
	txt_separator.c          txt_separator.h          \
	txt_spinctrl.c           txt_spinctrl.h           \
	txt_sdl.c                txt_sdl.h                \
	                         txt_largefont.h          \
	                         txt_smallfont.h          \
	txt_strut.c              txt_strut.h              \
	txt_table.c              txt_table.h              \
	txt_utf8.c               txt_utf8.h               \
	txt_widget.c             txt_widget.h             \
	txt_window.c             txt_window.h             \
	txt_window_action.c      txt_window_action.h      \
	                         txt_font.h



LOCAL_SRC_FILES = $(libtextscreen_a_SOURCES) 

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog  -lz
LOCAL_STATIC_LIBRARIES := SDL_net libjpeg
LOCAL_SHARED_LIBRARIES :=  SDL 

include $(BUILD_STATIC_LIBRARY)






