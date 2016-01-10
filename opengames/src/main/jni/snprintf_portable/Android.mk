
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)  
LOCAL_MODULE := snprintf_portable

LOCAL_CFLAGS := -DFLOATING_POINT

#LOCAL_SRC_FILES:= fvwrite.c vfprintf.c vsnprintf.c strtod.c
LOCAL_SRC_FILES:= snprintf.c
 

LOCAL_LDLIBS := 


include $(BUILD_STATIC_LIBRARY)



include 
