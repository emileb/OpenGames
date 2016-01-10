LOCAL_PATH := $(call my-dir)



include $(CLEAR_VARS)


LOCAL_MODULE    := lzma

LOCAL_CFLAGS =  -Wall -fomit-frame-pointer -D_7ZIP_ST


LOCAL_CFLAGS += -march=armv7-a -mfloat-abi=softfp
LOCAL_LDLIBS += -Wl,--fix-cortex-a8

LOCAL_C_INCLUDES = 
	
	
SRC = \
	C/7zBuf.c \
	C/7zCrc.c \
	C/7zCrcOpt.c \
	C/7zDec.c \
	C/7zIn.c \
	C/7zStream.c \
	C/Bcj2.c \
	C/Bra.c \
	C/Bra86.c \
	C/CpuArch.c \
	C/LzFind.c \
	C/Lzma2Dec.c \
	C/LzmaDec.c \
	C/LzmaEnc.c \ 
	
	
 
LOCAL_SRC_FILES = $(SRC)

LOCAL_LDLIBS := -ldl -llog -lz -lm 
LOCAL_STATIC_LIBRARIES := 
LOCAL_SHARED_LIBRARIES := 

include $(BUILD_STATIC_LIBRARY)






