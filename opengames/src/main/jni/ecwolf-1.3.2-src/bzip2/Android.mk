LOCAL_PATH := $(call my-dir)



include $(CLEAR_VARS)


LOCAL_MODULE    := bzip2

LOCAL_CFLAGS =  -Wall -fomit-frame-pointer


LOCAL_CFLAGS += -march=armv7-a -mfloat-abi=softfp
LOCAL_LDLIBS += -Wl,--fix-cortex-a8

LOCAL_C_INCLUDES = 
	
	
SRC = \
	 blocksort.c \
    bzlib.c \
    compress.c \
    crctable.c \
    decompress.c \
    huffman.c \
    randtable.c \
	
 
LOCAL_SRC_FILES = $(SRC)

LOCAL_LDLIBS := -ldl -llog -lz -lm 
LOCAL_STATIC_LIBRARIES := 
LOCAL_SHARED_LIBRARIES := 

include $(BUILD_STATIC_LIBRARY)






