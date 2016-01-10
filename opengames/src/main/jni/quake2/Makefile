#
# Quake2 Makefile for Linux 2.0
#
# Nov '97 by Zoid <zoid@idsoftware.com>
#
# ELF only
#

# start of configurable options

# Here are your build options:
# (Note: not all options are available for all platforms).
# quake2 (uses OSS for sound, cdrom ioctls for cd audio) is automatically built.
# game$(ARCH).so is automatically built.
BUILD_SDLQUAKE2=NO	# sdlquake2 executable (uses SDL for cdrom and sound)
BUILD_SVGA=NO		# SVGAlib driver. Seems to work fine.
BUILD_X11=NO		# X11 software driver. Works somewhat ok.
BUILD_GLX=NO		# X11 GLX driver. Works somewhat ok.
BUILD_FXGL=NO		# FXMesa driver. Not tested. (used only for V1 and V2).
BUILD_SDL=NO		# SDL software driver. Works fine for some people.
BUILD_SDLGL=YES		# SDL OpenGL driver. Works fine for some people.
BUILD_CTFDLL=YES	# game$(ARCH).so for ctf
BUILD_XATRIX=NO		# game$(ARCH).so for xatrix (see README.r for details)
BUILD_ROGUE=NO		# game$(ARCH).so for rogue (see README.r for details)
BUILD_JOYSTICK=NO	# build in joystick support
BUILD_ARTS=NO		# build in support for libaRts sound.
BUILD_ALSA=YES		# build in support for ALSA (default sound on 2.6)
BUILD_DEDICATED=NO	# build a dedicated quake2 server
BUILD_AA=NO		# build the ascii soft renderer.
BUILD_QMAX=NO		# build the fancier GL graphics
BUILD_RETEXTURE=NO	# build a version supporting retextured graphics
BUILD_REDBLUE=NO	# build a red-blue 3d glasses renderer...
STATICSDL=NO
SDLDIR=/usr/local/lib

# Other compile-time options:
# Compile with IPv6 (protocol independent API). Tested on FreeBSD
HAVE_IPV6=NO

# (hopefully) end of configurable options

# Check OS type.
OSTYPE := $(shell uname -s)

ifneq ($(OSTYPE),Linux)
ifneq ($(OSTYPE),FreeBSD)
ifeq ($(OSTYPE),SunOS)
$(error OS $(OSTYPE) detected, use "Makefile.Solaris" instead.)
else
ifeq ($(OSTYPE),IRIX64)
$(error OS $(OSTYPE) detected, use "Makefile.IRIX" instead.)
else
$(error OS $(OSTYPE) is currently not supported)
endif
endif
endif
endif


# this nice line comes from the linux kernel makefile
ARCH := $(shell uname -m | sed -e s/i.86/i386/ -e s/sun4u/sparc/ -e s/sparc64/sparc/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/alpha/axp/)

ifneq ($(ARCH),x86_64)
ifneq ($(ARCH),i386)
ifneq ($(ARCH),axp)
ifneq ($(ARCH),ppc)
ifneq ($(ARCH),sparc)
$(error arch $(ARCH) is currently not supported)
endif
endif
endif
endif
endif

CC=gcc

ifndef OPT_CFLAGS
ifeq ($(ARCH),axp)
OPT_CFLAGS=-ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations
endif

ifeq ($(ARCH),ppc)
OPT_CFLAGS=-O2 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations
endif

ifeq ($(ARCH),sparc)
OPT_CFLAGS=-ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations
endif

ifeq ($(ARCH),i386)
OPT_CFLAGS=-O2 -ffast-math -funroll-loops -falign-loops=2 \
	-falign-jumps=2 -falign-functions=2 -fno-strict-aliasing
# compiler bugs with gcc 2.96 and 3.0.1 can cause bad builds with heavy opts.
#OPT_CFLAGS=-O6 -m486 -ffast-math -funroll-loops \
#	-fomit-frame-pointer -fexpensive-optimizations -malign-loops=2 \
#	-malign-jumps=2 -malign-functions=2
endif

ifeq ($(ARCH),x86_64)
#_LIB := 64
OPT_CFLAGS=-O2 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations -fno-strict-aliasing
endif
endif
RELEASE_CFLAGS=$(BASE_CFLAGS) $(OPT_CFLAGS)

VERSION=3.21+r0.16

MOUNT_DIR=src

BUILD_DEBUG_DIR=debug$(ARCH)
BUILD_RELEASE_DIR=release$(ARCH)
CLIENT_DIR=$(MOUNT_DIR)/client
SERVER_DIR=$(MOUNT_DIR)/server
REF_SOFT_DIR=$(MOUNT_DIR)/ref_soft
REF_GL_DIR=$(MOUNT_DIR)/ref_gl
COMMON_DIR=$(MOUNT_DIR)/qcommon
LINUX_DIR=$(MOUNT_DIR)/linux
GAME_DIR=$(MOUNT_DIR)/game
CTF_DIR=$(MOUNT_DIR)/ctf
XATRIX_DIR=$(MOUNT_DIR)/xatrix
ROGUE_DIR=$(MOUNT_DIR)/rogue
NULL_DIR=$(MOUNT_DIR)/null

BASE_CFLAGS=-Wall -pipe -Dstricmp=strcasecmp
ifeq ($(HAVE_IPV6),YES)
BASE_CFLAGS+= -DHAVE_IPV6
ifeq ($(OSTYPE),FreeBSD)
BASE_CFLAGS+= -DHAVE_SIN6_LEN
endif
NET_UDP=net_udp6
else
NET_UDP=net_udp
endif

ifdef DEFAULT_BASEDIR
BASE_CFLAGS += -DDEFAULT_BASEDIR=\\\"$(DEFAULT_BASEDIR)\\\"
endif
ifdef DEFAULT_LIBDIR
BASE_CFLAGS += -DDEFAULT_LIBDIR=\\\"$(DEFAULT_LIBDIR)\\\"
endif

ifeq ($(strip $(BUILD_QMAX)),YES)
	BASE_CFLAGS+=-DQMAX
endif

ifeq ($(strip $(BUILD_RETEXTURE)),YES)
	BASE_CFLAGS+=-DRETEX
endif

ifeq ($(strip $(BUILD_JOYSTICK)),YES)
BASE_CFLAGS+=-DJoystick
endif
ifeq ($(strip $(BUILD_ARTS)),YES)
BASE_CFLAGS+=$(shell artsc-config --cflags)
endif

ifneq ($(ARCH),i386)
 BASE_CFLAGS+=-DC_ONLY
endif

DEBUG_CFLAGS=$(BASE_CFLAGS) -g

ifeq ($(OSTYPE),FreeBSD)
LDFLAGS=-lm
endif
ifeq ($(OSTYPE),Linux)
LDFLAGS=-lm -ldl
endif

ifeq ($(strip $(BUILD_ARTS)),YES)
LDFLAGS+=$(shell artsc-config --libs)
endif

ifeq ($(strip $(BUILD_ALSA)),YES)
LDFLAGS+=-lasound
endif


SVGALDFLAGS=-lvga

XCFLAGS=-I/usr/X11R6/include
XLDFLAGS=-L/usr/X11R6/lib$(_LIB) -lX11 -lXext -lXxf86dga -lXxf86vm
AALDFLAGS=-lm -laa

SDLCFLAGS=$(shell sdl-config --cflags)
ifeq ($(strip $(STATICSDL)),YES)
	SDLLDFLAGS += -L/usr/X11R6/lib$(_LIB) -Wl,-Bstatic $(SDLDIR)/libSDL.a
	SDLLDFLAGS += $(SDLDIR)/libesd.a $(SDLDIR)/libartsc.a -Wl,-Bdynamic
	SDLLDFLAGS += -lpthread -lX11 -lXext -lXxf86dga -lXxf86vm -lXv \
		-lXinerama
else
	SDLLDFLAGS=$(shell sdl-config --libs)
endif

ifeq ($(strip $(BUILD_JOYSTICK)),YES)
SDLCFLAGS+=-DJoystick
endif

FXGLCFLAGS=-I/usr/X11R6/include
FXGLLDFLAGS=-L/usr/local/glide/lib -L/usr/X11/lib -L/usr/local/lib \
	-L/usr/X11R6/lib -lX11 -lXext -lGL -lvga

GLXCFLAGS=-I/usr/X11R6/include -DOPENGL
GLXLDFLAGS=-L/usr/X11R6/lib$(_LIB) -lX11 -lXext -lXxf86dga -lXxf86vm

SDLGLCFLAGS=$(SDLCFLAGS) -DOPENGL
SDLGLLDFLAGS=$(SDLLDFLAGS)

ifeq ($(strip $(BUILD_QMAX)),YES)
GLXLDFLAGS+=-ljpeg
SDLGLLDFLAGS+=-ljpeg
REF_GL_DIR = $(MOUNT_DIR)/ref_candygl
CL_FX = cl_fxmax.c
else
CL_FX = cl_fx.c
endif

SHLIBEXT=so

SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

DO_CC=$(CC) $(CFLAGS) -o $@ -c $<
DO_DED_CC=$(CC) $(CFLAGS) -DDEDICATED_ONLY -o $@ -c $<
DO_DED_DEBUG_CC=$(CC) $(DEBUG_CFLAGS) -DDEDICATED_ONLY -o $@ -c $<
DO_SHLIB_CC=$(CC) $(CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<
DO_GL_SHLIB_CC=$(CC) $(CFLAGS) $(SHLIBCFLAGS) $(GLCFLAGS) -o $@ -c $<
DO_AS=$(CC) $(CFLAGS) -DELF -x assembler-with-cpp -o $@ -c $<
DO_SHLIB_AS=$(CC) $(CFLAGS) $(SHLIBCFLAGS) -DELF -x assembler-with-cpp -o $@ -c $<

#############################################################################
# SETUP AND BUILD
#############################################################################

.PHONY : targets build_debug build_release clean clean-debug clean-release clean2

TARGETS=$(BUILDDIR)/quake2 $(BUILDDIR)/game$(ARCH).$(SHLIBEXT)
ifeq ($(strip $(BUILD_DEDICATED)),YES)
 TARGETS += $(BUILDDIR)/q2ded
endif
ifeq ($(strip $(BUILD_CTFDLL)),YES)
 TARGETS += $(BUILDDIR)/ctf/game$(ARCH).$(SHLIBEXT)
endif

ifeq ($(strip $(BUILD_XATRIX)),YES)
 TARGETS += $(BUILDDIR)/xatrix/game$(ARCH).$(SHLIBEXT)
endif

ifeq ($(strip $(BUILD_ROGUE)),YES)
 TARGETS += $(BUILDDIR)/rogue/game$(ARCH).$(SHLIBEXT)
endif

ifeq ($(ARCH),axp)
 ifeq ($(strip $(BUILD_SDLQUAKE2)),YES)
  $(warning Warning: SDLQuake2 not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_SVGA)),YES)
  $(warning Warning: SVGAlib support not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_X11)),YES)
  $(warning Warning: X11 support not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_GLX)),YES)
  $(warning Warning: support not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_FXGL)),YES)
  $(warning Warning: FXGL support not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_SDL)),YES)
  $(warning Warning: SDL support not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_SDLGL)),YES)
  $(warning Warning: SDLGL support not supported for $(ARCH))
 endif
endif # ARCH axp

ifeq ($(ARCH),ppc)
 ifeq ($(strip $(BUILD_SDLQUAKE2)),YES)
  TARGETS += $(BUILDDIR)/sdlquake2
 endif
 
 ifeq ($(strip $(BUILD_SVGA)),YES)
  $(warning Warning: SVGAlib support not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_X11)),YES)
  TARGETS += $(BUILDDIR)/ref_softx.$(SHLIBEXT)
 endif

 ifeq ($(strip $(BUILD_GLX)),YES)
  TARGETS += $(BUILDDIR)/ref_glx.$(SHLIBEXT)
 endif

 ifeq ($(strip $(BUILD_FXGL)),YES)
  $(warning Warning: FXGL support not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_SDL)),YES)
  TARGETS += $(BUILDDIR)/ref_softsdl.$(SHLIBEXT)
 endif

 ifeq ($(strip $(BUILD_SDLGL)),YES)
  TARGETS += $(BUILDDIR)/ref_sdlgl.$(SHLIBEXT)
 endif
endif # ARCH ppc

ifeq ($(ARCH),sparc)
 ifeq ($(strip $(BUILD_SDLQUAKE2)),YES)
  TARGETS += $(BUILDDIR)/sdlquake2
 endif
 
 ifeq ($(strip $(BUILD_SVGA)),YES)
  $(warning Warning: SVGAlib support not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_X11)),YES)
  TARGETS += $(BUILDDIR)/ref_softx.$(SHLIBEXT)
 endif

 ifeq ($(strip $(BUILD_GLX)),YES)
  $(warning Warning: GLX support not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_FXGL)),YES)
  $(warning Warning: FXGL support not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_SDL)),YES)
  TARGETS += $(BUILDDIR)/ref_softsdl.$(SHLIBEXT)
 endif

 ifeq ($(strip $(BUILD_SDLGL)),YES)
  $(warning Warning: SDLGL support not supported for $(ARCH))
 endif
endif # ARCH sparc

ifeq ($(ARCH),x86_64)
 ifeq ($(strip $(BUILD_SDLQUAKE2)),YES)
  TARGETS += $(BUILDDIR)/sdlquake2
 endif

 ifeq ($(strip $(BUILD_SVGA)),YES)
  $(warning Warning: SVGA not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_X11)),YES)
  $(warning Warning: Software X Renderer not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_SDL)),YES)
  $(warning Warning: Software SDL Renderer not supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_GLX)),YES)
  TARGETS += $(BUILDDIR)/ref_glx.$(SHLIBEXT)
 endif

 ifeq ($(strip $(BUILD_FXGL)),YES)
  $(warning Warning: FXGL not currently supported for $(ARCH))
 endif

 ifeq ($(strip $(BUILD_SDLGL)),YES)
  TARGETS += $(BUILDDIR)/ref_sdlgl.$(SHLIBEXT)
 endif
endif # ARCH x86_64

ifeq ($(ARCH),i386)
 ifeq ($(strip $(BUILD_SDLQUAKE2)),YES)
  TARGETS += $(BUILDDIR)/sdlquake2
 endif

 ifeq ($(strip $(BUILD_SVGA)),YES)
  TARGETS += $(BUILDDIR)/ref_soft.$(SHLIBEXT)
 endif

 ifeq ($(strip $(BUILD_X11)),YES)
  TARGETS += $(BUILDDIR)/ref_softx.$(SHLIBEXT)
 endif

 ifeq ($(strip $(BUILD_GLX)),YES)
  TARGETS += $(BUILDDIR)/ref_glx.$(SHLIBEXT)
 endif

 ifeq ($(strip $(BUILD_FXGL)),YES)
  TARGETS += $(BUILDDIR)/ref_gl.$(SHLIBEXT)
 endif

 ifeq ($(strip $(BUILD_SDL)),YES)
  TARGETS += $(BUILDDIR)/ref_softsdl.$(SHLIBEXT)
 endif

 ifeq ($(strip $(BUILD_SDLGL)),YES)
  TARGETS += $(BUILDDIR)/ref_sdlgl.$(SHLIBEXT)
 endif
endif # ARCH i386

ifeq ($(strip $(BUILD_AA)),YES)
	TARGETS += $(BUILDDIR)/ref_softaa.$(SHLIBEXT)
endif

ifeq ($(strip $(BUILD_REDBLUE)),YES)
	BASE_CFLAGS += -DREDBLUE
endif

all: build_debug build_release

build_debug:
	@-mkdir -p $(BUILD_DEBUG_DIR) \
		$(BUILD_DEBUG_DIR)/client \
		$(BUILD_DEBUG_DIR)/ded \
		$(BUILD_DEBUG_DIR)/ref_soft \
		$(BUILD_DEBUG_DIR)/ref_gl \
		$(BUILD_DEBUG_DIR)/game \
		$(BUILD_DEBUG_DIR)/ctf \
		$(BUILD_DEBUG_DIR)/xatrix \
		$(BUILD_DEBUG_DIR)/rogue
	$(MAKE) targets BUILDDIR=$(BUILD_DEBUG_DIR) CFLAGS="$(DEBUG_CFLAGS) -DLINUX_VERSION='\"$(VERSION) Debug\"'"

build_release:
	@-mkdir -p $(BUILD_RELEASE_DIR) \
		$(BUILD_RELEASE_DIR)/client \
		$(BUILD_RELEASE_DIR)/ded \
		$(BUILD_RELEASE_DIR)/ref_soft \
		$(BUILD_RELEASE_DIR)/ref_gl \
		$(BUILD_RELEASE_DIR)/game \
		$(BUILD_RELEASE_DIR)/ctf \
		$(BUILD_RELEASE_DIR)/xatrix \
		$(BUILD_RELEASE_DIR)/rogue
	$(MAKE) targets BUILDDIR=$(BUILD_RELEASE_DIR) CFLAGS="$(RELEASE_CFLAGS) -DLINUX_VERSION='\"$(VERSION)\"'"

targets: $(TARGETS)

#############################################################################
# CLIENT/SERVER
#############################################################################

QUAKE2_OBJS = \
	$(BUILDDIR)/client/cl_cin.o \
	$(BUILDDIR)/client/cl_ents.o \
	$(BUILDDIR)/client/cl_fx.o \
	$(BUILDDIR)/client/cl_input.o \
	$(BUILDDIR)/client/cl_inv.o \
	$(BUILDDIR)/client/cl_main.o \
	$(BUILDDIR)/client/cl_parse.o \
	$(BUILDDIR)/client/cl_pred.o \
	$(BUILDDIR)/client/cl_tent.o \
	$(BUILDDIR)/client/cl_scrn.o \
	$(BUILDDIR)/client/cl_view.o \
	$(BUILDDIR)/client/cl_newfx.o \
	$(BUILDDIR)/client/console.o \
	$(BUILDDIR)/client/keys.o \
	$(BUILDDIR)/client/menu.o \
	$(BUILDDIR)/client/snd_dma.o \
	$(BUILDDIR)/client/snd_mem.o \
	$(BUILDDIR)/client/snd_mix.o \
	$(BUILDDIR)/client/qmenu.o \
	$(BUILDDIR)/client/m_flash.o \
	\
	$(BUILDDIR)/client/cmd.o \
	$(BUILDDIR)/client/cmodel.o \
	$(BUILDDIR)/client/common.o \
	$(BUILDDIR)/client/crc.o \
	$(BUILDDIR)/client/cvar.o \
	$(BUILDDIR)/client/files.o \
	$(BUILDDIR)/client/md4.o \
	$(BUILDDIR)/client/net_chan.o \
	\
	$(BUILDDIR)/client/sv_ccmds.o \
	$(BUILDDIR)/client/sv_ents.o \
	$(BUILDDIR)/client/sv_game.o \
	$(BUILDDIR)/client/sv_init.o \
	$(BUILDDIR)/client/sv_main.o \
	$(BUILDDIR)/client/sv_send.o \
	$(BUILDDIR)/client/sv_user.o \
	$(BUILDDIR)/client/sv_world.o \
	\
	$(BUILDDIR)/client/q_shlinux.o \
	$(BUILDDIR)/client/vid_menu.o \
	$(BUILDDIR)/client/vid_so.o \
	$(BUILDDIR)/client/sys_linux.o \
	$(BUILDDIR)/client/glob.o \
	$(BUILDDIR)/client/$(NET_UDP).o \
	\
	$(BUILDDIR)/client/q_shared.o \
	$(BUILDDIR)/client/pmove.o

QUAKE2_LNX_OBJS = \
	$(BUILDDIR)/client/cd_linux.o 
ifeq ($(BUILD_ARTS),YES)
QUAKE2_LNX_OBJS += $(BUILDDIR)/client/snd_arts.o
else
ifeq ($(strip $(BUILD_ALSA)),YES)
QUAKE2_LNX_OBJS += $(BUILDDIR)/client/snd_alsa.o
else
QUAKE2_LNX_OBJS += $(BUILDDIR)/client/snd_linux.o
endif
endif

QUAKE2_SDL_OBJS = \
	$(BUILDDIR)/client/cd_sdl.o \
	$(BUILDDIR)/client/snd_sdl.o

ifeq ($(ARCH),i386)
QUAKE2_AS_OBJS = \
	$(BUILDDIR)/client/snd_mixa.o
else
QUAKE2_AS_OBJS =  #blank
endif

$(BUILDDIR)/quake2 : $(QUAKE2_OBJS) $(QUAKE2_LNX_OBJS) $(QUAKE2_AS_OBJS)
	$(CC) $(CFLAGS) -o $@ $(QUAKE2_OBJS) $(QUAKE2_LNX_OBJS) $(QUAKE2_AS_OBJS) $(LDFLAGS)

$(BUILDDIR)/sdlquake2 : $(QUAKE2_OBJS) $(QUAKE2_SDL_OBJS) $(QUAKE2_AS_OBJS)
	$(CC) $(CFLAGS) -o $@ $(QUAKE2_OBJS) $(QUAKE2_SDL_OBJS) $(QUAKE2_AS_OBJS) $(LDFLAGS) $(SDLLDFLAGS)

$(BUILDDIR)/client/cl_cin.o :     $(CLIENT_DIR)/cl_cin.c
	$(DO_CC)

$(BUILDDIR)/client/cl_ents.o :    $(CLIENT_DIR)/cl_ents.c
	$(DO_CC)

$(BUILDDIR)/client/cl_fx.o :      $(CLIENT_DIR)/$(CL_FX)
	$(DO_CC)

$(BUILDDIR)/client/cl_input.o :   $(CLIENT_DIR)/cl_input.c
	$(DO_CC)

$(BUILDDIR)/client/cl_inv.o :     $(CLIENT_DIR)/cl_inv.c
	$(DO_CC)

$(BUILDDIR)/client/cl_main.o :    $(CLIENT_DIR)/cl_main.c
	$(DO_CC)

$(BUILDDIR)/client/cl_parse.o :   $(CLIENT_DIR)/cl_parse.c
	$(DO_CC)

$(BUILDDIR)/client/cl_pred.o :    $(CLIENT_DIR)/cl_pred.c
	$(DO_CC)

$(BUILDDIR)/client/cl_tent.o :    $(CLIENT_DIR)/cl_tent.c
	$(DO_CC)

$(BUILDDIR)/client/cl_scrn.o :    $(CLIENT_DIR)/cl_scrn.c
	$(DO_CC)

$(BUILDDIR)/client/cl_view.o :    $(CLIENT_DIR)/cl_view.c
	$(DO_CC)

$(BUILDDIR)/client/cl_newfx.o :	  $(CLIENT_DIR)/cl_newfx.c
	$(DO_CC)

$(BUILDDIR)/client/console.o :    $(CLIENT_DIR)/console.c
	$(DO_CC)

$(BUILDDIR)/client/keys.o :       $(CLIENT_DIR)/keys.c
	$(DO_CC)

$(BUILDDIR)/client/menu.o :       $(CLIENT_DIR)/menu.c
	$(DO_CC)

$(BUILDDIR)/client/snd_dma.o :    $(CLIENT_DIR)/snd_dma.c
	$(DO_CC)

$(BUILDDIR)/client/snd_mem.o :    $(CLIENT_DIR)/snd_mem.c
	$(DO_CC)

$(BUILDDIR)/client/snd_mix.o :    $(CLIENT_DIR)/snd_mix.c
	$(DO_CC)

$(BUILDDIR)/client/qmenu.o :      $(CLIENT_DIR)/qmenu.c
	$(DO_CC)

$(BUILDDIR)/client/m_flash.o :    $(GAME_DIR)/m_flash.c
	$(DO_CC)

$(BUILDDIR)/client/cmd.o :        $(COMMON_DIR)/cmd.c
	$(DO_CC)

$(BUILDDIR)/client/cmodel.o :     $(COMMON_DIR)/cmodel.c
	$(DO_CC)

$(BUILDDIR)/client/common.o :     $(COMMON_DIR)/common.c
	$(DO_CC)

$(BUILDDIR)/client/crc.o :        $(COMMON_DIR)/crc.c
	$(DO_CC)

$(BUILDDIR)/client/cvar.o :       $(COMMON_DIR)/cvar.c
	$(DO_CC)

$(BUILDDIR)/client/files.o :      $(COMMON_DIR)/files.c
	$(DO_CC)

$(BUILDDIR)/client/md4.o :        $(COMMON_DIR)/md4.c
	$(DO_CC)

$(BUILDDIR)/client/net_chan.o :   $(COMMON_DIR)/net_chan.c
	$(DO_CC)

$(BUILDDIR)/client/q_shared.o :   $(GAME_DIR)/q_shared.c
	$(DO_CC)

$(BUILDDIR)/client/pmove.o :      $(COMMON_DIR)/pmove.c
	$(DO_CC)

$(BUILDDIR)/client/sv_ccmds.o :   $(SERVER_DIR)/sv_ccmds.c
	$(DO_CC)

$(BUILDDIR)/client/sv_ents.o :    $(SERVER_DIR)/sv_ents.c
	$(DO_CC)

$(BUILDDIR)/client/sv_game.o :    $(SERVER_DIR)/sv_game.c
	$(DO_CC)

$(BUILDDIR)/client/sv_init.o :    $(SERVER_DIR)/sv_init.c
	$(DO_CC)

$(BUILDDIR)/client/sv_main.o :    $(SERVER_DIR)/sv_main.c
	$(DO_CC)

$(BUILDDIR)/client/sv_send.o :    $(SERVER_DIR)/sv_send.c
	$(DO_CC)

$(BUILDDIR)/client/sv_user.o :    $(SERVER_DIR)/sv_user.c
	$(DO_CC)

$(BUILDDIR)/client/sv_world.o :   $(SERVER_DIR)/sv_world.c
	$(DO_CC)

$(BUILDDIR)/client/q_shlinux.o :  $(LINUX_DIR)/q_shlinux.c
	$(DO_CC)

$(BUILDDIR)/client/vid_menu.o :   $(LINUX_DIR)/vid_menu.c
	$(DO_CC)

$(BUILDDIR)/client/vid_so.o :     $(LINUX_DIR)/vid_so.c
	$(DO_CC)

$(BUILDDIR)/client/snd_mixa.o :   $(LINUX_DIR)/snd_mixa.s
	$(DO_AS)

$(BUILDDIR)/client/sys_linux.o :  $(LINUX_DIR)/sys_linux.c
	$(DO_CC)

$(BUILDDIR)/client/glob.o :       $(LINUX_DIR)/glob.c
	$(DO_CC)

$(BUILDDIR)/client/net_udp.o :    $(LINUX_DIR)/net_udp.c
	$(DO_CC)

$(BUILDDIR)/client/net_udp6.o :    $(LINUX_DIR)/net_udp6.c
	$(DO_CC)

$(BUILDDIR)/client/cd_linux.o :   $(LINUX_DIR)/cd_linux.c
	$(DO_CC)

$(BUILDDIR)/client/snd_arts.o :  $(LINUX_DIR)/snd_arts.c
	$(DO_CC)

$(BUILDDIR)/client/snd_alsa.o :  $(LINUX_DIR)/snd_alsa.c
	$(DO_CC)

$(BUILDDIR)/client/snd_linux.o :  $(LINUX_DIR)/snd_linux.c
	$(DO_CC)

$(BUILDDIR)/client/cd_sdl.o :     $(LINUX_DIR)/cd_sdl.c
	$(DO_CC) $(SDLCFLAGS)

$(BUILDDIR)/client/snd_sdl.o :     $(LINUX_DIR)/snd_sdl.c
	$(DO_CC) $(SDLCFLAGS)

#############################################################################
# DEDICATED SERVER
#############################################################################

Q2DED_OBJS = \
       $(BUILDDIR)/ded/cmd.o \
       $(BUILDDIR)/ded/cmodel.o \
       $(BUILDDIR)/ded/common.o \
       $(BUILDDIR)/ded/crc.o \
       $(BUILDDIR)/ded/cvar.o \
       $(BUILDDIR)/ded/files.o \
       $(BUILDDIR)/ded/md4.o \
       $(BUILDDIR)/ded/net_chan.o \
       $(BUILDDIR)/ded/sv_ccmds.o \
       $(BUILDDIR)/ded/sv_ents.o \
       $(BUILDDIR)/ded/sv_game.o \
       $(BUILDDIR)/ded/sv_init.o \
       $(BUILDDIR)/ded/sv_main.o \
       $(BUILDDIR)/ded/sv_send.o \
       $(BUILDDIR)/ded/sv_user.o \
       $(BUILDDIR)/ded/sv_world.o \
       $(BUILDDIR)/ded/q_shlinux.o \
       $(BUILDDIR)/ded/sys_linux.o \
       $(BUILDDIR)/ded/glob.o \
       $(BUILDDIR)/ded/net_udp.o \
       $(BUILDDIR)/ded/q_shared.o \
       $(BUILDDIR)/ded/pmove.o \
       $(BUILDDIR)/ded/cl_null.o \
       $(BUILDDIR)/ded/cd_null.o

$(BUILDDIR)/q2ded : $(Q2DED_OBJS)
	$(CC) $(CFLAGS) -o $@ $(Q2DED_OBJS) $(LDFLAGS)

$(BUILDDIR)/ded/cmd.o :        $(COMMON_DIR)/cmd.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/cmodel.o :     $(COMMON_DIR)/cmodel.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/common.o :     $(COMMON_DIR)/common.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/crc.o :	 $(COMMON_DIR)/crc.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/cvar.o :	$(COMMON_DIR)/cvar.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/files.o :      $(COMMON_DIR)/files.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/md4.o :	 $(COMMON_DIR)/md4.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/net_chan.o :   $(COMMON_DIR)/net_chan.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/q_shared.o :   $(GAME_DIR)/q_shared.c
	$(DO_DED_DEBUG_CC)

$(BUILDDIR)/ded/pmove.o :      $(COMMON_DIR)/pmove.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/sv_ccmds.o :   $(SERVER_DIR)/sv_ccmds.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/sv_ents.o :    $(SERVER_DIR)/sv_ents.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/sv_game.o :    $(SERVER_DIR)/sv_game.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/sv_init.o :    $(SERVER_DIR)/sv_init.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/sv_main.o :    $(SERVER_DIR)/sv_main.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/sv_send.o :    $(SERVER_DIR)/sv_send.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/sv_user.o :    $(SERVER_DIR)/sv_user.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/sv_world.o :   $(SERVER_DIR)/sv_world.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/q_shlinux.o :  $(LINUX_DIR)/q_shlinux.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/sys_linux.o :  $(LINUX_DIR)/sys_linux.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/glob.o :	$(LINUX_DIR)/glob.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/net_udp.o :    $(LINUX_DIR)/net_udp.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/cd_null.o :    $(NULL_DIR)/cd_null.c
	$(DO_DED_CC)

$(BUILDDIR)/ded/cl_null.o :    $(NULL_DIR)/cl_null.c
	$(DO_DED_CC)


#############################################################################
# GAME
#############################################################################

GAME_OBJS = \
	$(BUILDDIR)/game/g_ai.o \
	$(BUILDDIR)/game/p_client.o \
	$(BUILDDIR)/game/g_chase.o \
	$(BUILDDIR)/game/g_cmds.o \
	$(BUILDDIR)/game/g_svcmds.o \
	$(BUILDDIR)/game/g_combat.o \
	$(BUILDDIR)/game/g_func.o \
	$(BUILDDIR)/game/g_items.o \
	$(BUILDDIR)/game/g_main.o \
	$(BUILDDIR)/game/g_misc.o \
	$(BUILDDIR)/game/g_monster.o \
	$(BUILDDIR)/game/g_phys.o \
	$(BUILDDIR)/game/g_save.o \
	$(BUILDDIR)/game/g_spawn.o \
	$(BUILDDIR)/game/g_target.o \
	$(BUILDDIR)/game/g_trigger.o \
	$(BUILDDIR)/game/g_turret.o \
	$(BUILDDIR)/game/g_utils.o \
	$(BUILDDIR)/game/g_weapon.o \
	$(BUILDDIR)/game/m_actor.o \
	$(BUILDDIR)/game/m_berserk.o \
	$(BUILDDIR)/game/m_boss2.o \
	$(BUILDDIR)/game/m_boss3.o \
	$(BUILDDIR)/game/m_boss31.o \
	$(BUILDDIR)/game/m_boss32.o \
	$(BUILDDIR)/game/m_brain.o \
	$(BUILDDIR)/game/m_chick.o \
	$(BUILDDIR)/game/m_flipper.o \
	$(BUILDDIR)/game/m_float.o \
	$(BUILDDIR)/game/m_flyer.o \
	$(BUILDDIR)/game/m_gladiator.o \
	$(BUILDDIR)/game/m_gunner.o \
	$(BUILDDIR)/game/m_hover.o \
	$(BUILDDIR)/game/m_infantry.o \
	$(BUILDDIR)/game/m_insane.o \
	$(BUILDDIR)/game/m_medic.o \
	$(BUILDDIR)/game/m_move.o \
	$(BUILDDIR)/game/m_mutant.o \
	$(BUILDDIR)/game/m_parasite.o \
	$(BUILDDIR)/game/m_soldier.o \
	$(BUILDDIR)/game/m_supertank.o \
	$(BUILDDIR)/game/m_tank.o \
	$(BUILDDIR)/game/p_hud.o \
	$(BUILDDIR)/game/p_trail.o \
	$(BUILDDIR)/game/p_view.o \
	$(BUILDDIR)/game/p_weapon.o \
	$(BUILDDIR)/game/q_shared.o \
	$(BUILDDIR)/game/m_flash.o

$(BUILDDIR)/game$(ARCH).$(SHLIBEXT) : $(GAME_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GAME_OBJS)

$(BUILDDIR)/game/g_ai.o :        $(GAME_DIR)/g_ai.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_chase.o :     $(GAME_DIR)/g_chase.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/p_client.o :    $(GAME_DIR)/p_client.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_cmds.o :      $(GAME_DIR)/g_cmds.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_svcmds.o :    $(GAME_DIR)/g_svcmds.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_combat.o :    $(GAME_DIR)/g_combat.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_func.o :      $(GAME_DIR)/g_func.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_items.o :     $(GAME_DIR)/g_items.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_main.o :      $(GAME_DIR)/g_main.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_misc.o :      $(GAME_DIR)/g_misc.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_monster.o :   $(GAME_DIR)/g_monster.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_phys.o :      $(GAME_DIR)/g_phys.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_save.o :      $(GAME_DIR)/g_save.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_spawn.o :     $(GAME_DIR)/g_spawn.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_target.o :    $(GAME_DIR)/g_target.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_trigger.o :   $(GAME_DIR)/g_trigger.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_turret.o :    $(GAME_DIR)/g_turret.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_utils.o :     $(GAME_DIR)/g_utils.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/g_weapon.o :    $(GAME_DIR)/g_weapon.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_actor.o :     $(GAME_DIR)/m_actor.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_berserk.o :   $(GAME_DIR)/m_berserk.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_boss2.o :     $(GAME_DIR)/m_boss2.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_boss3.o :     $(GAME_DIR)/m_boss3.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_boss31.o :     $(GAME_DIR)/m_boss31.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_boss32.o :     $(GAME_DIR)/m_boss32.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_brain.o :     $(GAME_DIR)/m_brain.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_chick.o :     $(GAME_DIR)/m_chick.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_flipper.o :   $(GAME_DIR)/m_flipper.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_float.o :     $(GAME_DIR)/m_float.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_flyer.o :     $(GAME_DIR)/m_flyer.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_gladiator.o : $(GAME_DIR)/m_gladiator.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_gunner.o :    $(GAME_DIR)/m_gunner.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_hover.o :     $(GAME_DIR)/m_hover.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_infantry.o :  $(GAME_DIR)/m_infantry.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_insane.o :    $(GAME_DIR)/m_insane.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_medic.o :     $(GAME_DIR)/m_medic.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_move.o :      $(GAME_DIR)/m_move.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_mutant.o :    $(GAME_DIR)/m_mutant.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_parasite.o :  $(GAME_DIR)/m_parasite.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_soldier.o :   $(GAME_DIR)/m_soldier.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_supertank.o : $(GAME_DIR)/m_supertank.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_tank.o :      $(GAME_DIR)/m_tank.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/p_hud.o :       $(GAME_DIR)/p_hud.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/p_trail.o :     $(GAME_DIR)/p_trail.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/p_view.o :      $(GAME_DIR)/p_view.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/p_weapon.o :    $(GAME_DIR)/p_weapon.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/q_shared.o :    $(GAME_DIR)/q_shared.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/game/m_flash.o :     $(GAME_DIR)/m_flash.c
	$(DO_SHLIB_CC)

#############################################################################
# CTF
#############################################################################

CTF_OBJS = \
	$(BUILDDIR)/ctf/g_ai.o \
	$(BUILDDIR)/ctf/g_chase.o \
	$(BUILDDIR)/ctf/g_cmds.o \
	$(BUILDDIR)/ctf/g_combat.o \
	$(BUILDDIR)/ctf/g_ctf.o \
	$(BUILDDIR)/ctf/g_func.o \
	$(BUILDDIR)/ctf/g_items.o \
	$(BUILDDIR)/ctf/g_main.o \
	$(BUILDDIR)/ctf/g_misc.o \
	$(BUILDDIR)/ctf/g_monster.o \
	$(BUILDDIR)/ctf/g_phys.o \
	$(BUILDDIR)/ctf/g_save.o \
	$(BUILDDIR)/ctf/g_spawn.o \
	$(BUILDDIR)/ctf/g_svcmds.o \
	$(BUILDDIR)/ctf/g_target.o \
	$(BUILDDIR)/ctf/g_trigger.o \
	$(BUILDDIR)/ctf/g_utils.o \
	$(BUILDDIR)/ctf/g_weapon.o \
	$(BUILDDIR)/ctf/m_move.o \
	$(BUILDDIR)/ctf/p_client.o \
	$(BUILDDIR)/ctf/p_hud.o \
	$(BUILDDIR)/ctf/p_menu.o \
	$(BUILDDIR)/ctf/p_trail.o \
	$(BUILDDIR)/ctf/p_view.o \
	$(BUILDDIR)/ctf/p_weapon.o \
	$(BUILDDIR)/ctf/q_shared.o

$(BUILDDIR)/ctf/game$(ARCH).$(SHLIBEXT) : $(CTF_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(CTF_OBJS)

$(BUILDDIR)/ctf/g_ai.o :       $(CTF_DIR)/g_ai.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_chase.o :    $(CTF_DIR)/g_chase.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_cmds.o :     $(CTF_DIR)/g_cmds.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_combat.o :   $(CTF_DIR)/g_combat.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_ctf.o :      $(CTF_DIR)/g_ctf.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_func.o :     $(CTF_DIR)/g_func.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_items.o :    $(CTF_DIR)/g_items.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_main.o :     $(CTF_DIR)/g_main.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_misc.o :     $(CTF_DIR)/g_misc.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_monster.o :  $(CTF_DIR)/g_monster.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_phys.o :     $(CTF_DIR)/g_phys.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_save.o :     $(CTF_DIR)/g_save.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_spawn.o :    $(CTF_DIR)/g_spawn.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_svcmds.o :   $(CTF_DIR)/g_svcmds.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_target.o :   $(CTF_DIR)/g_target.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_trigger.o :  $(CTF_DIR)/g_trigger.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_utils.o :    $(CTF_DIR)/g_utils.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/g_weapon.o :   $(CTF_DIR)/g_weapon.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/m_move.o :     $(CTF_DIR)/m_move.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/p_client.o :   $(CTF_DIR)/p_client.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/p_hud.o :      $(CTF_DIR)/p_hud.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/p_menu.o :     $(CTF_DIR)/p_menu.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/p_trail.o :    $(CTF_DIR)/p_trail.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/p_view.o :     $(CTF_DIR)/p_view.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/p_weapon.o :   $(CTF_DIR)/p_weapon.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ctf/q_shared.o :   $(CTF_DIR)/q_shared.c
	$(DO_SHLIB_CC)

#############################################################################
# XATRIX
#############################################################################

XATRIX_OBJS = \
	$(BUILDDIR)/xatrix/g_ai.o \
	$(BUILDDIR)/xatrix/g_chase.o \
	$(BUILDDIR)/xatrix/g_cmds.o \
	$(BUILDDIR)/xatrix/g_combat.o \
	$(BUILDDIR)/xatrix/g_func.o \
	$(BUILDDIR)/xatrix/g_items.o \
	$(BUILDDIR)/xatrix/g_main.o \
	$(BUILDDIR)/xatrix/g_misc.o \
	$(BUILDDIR)/xatrix/g_monster.o \
	$(BUILDDIR)/xatrix/g_phys.o \
	$(BUILDDIR)/xatrix/g_save.o \
	$(BUILDDIR)/xatrix/g_spawn.o \
	$(BUILDDIR)/xatrix/g_svcmds.o \
	$(BUILDDIR)/xatrix/g_target.o \
	$(BUILDDIR)/xatrix/g_trigger.o \
	$(BUILDDIR)/xatrix/g_turret.o \
	$(BUILDDIR)/xatrix/g_utils.o \
	$(BUILDDIR)/xatrix/g_weapon.o \
	$(BUILDDIR)/xatrix/m_actor.o \
	$(BUILDDIR)/xatrix/m_berserk.o \
	$(BUILDDIR)/xatrix/m_boss2.o \
	$(BUILDDIR)/xatrix/m_boss3.o \
	$(BUILDDIR)/xatrix/m_boss31.o \
	$(BUILDDIR)/xatrix/m_boss32.o \
	$(BUILDDIR)/xatrix/m_boss5.o \
	$(BUILDDIR)/xatrix/m_brain.o \
	$(BUILDDIR)/xatrix/m_chick.o \
	$(BUILDDIR)/xatrix/m_fixbot.o \
	$(BUILDDIR)/xatrix/m_flash.o \
	$(BUILDDIR)/xatrix/m_flipper.o \
	$(BUILDDIR)/xatrix/m_float.o \
	$(BUILDDIR)/xatrix/m_flyer.o \
	$(BUILDDIR)/xatrix/m_gekk.o \
	$(BUILDDIR)/xatrix/m_gladb.o \
	$(BUILDDIR)/xatrix/m_gladiator.o \
	$(BUILDDIR)/xatrix/m_gunner.o \
	$(BUILDDIR)/xatrix/m_hover.o \
	$(BUILDDIR)/xatrix/m_infantry.o \
	$(BUILDDIR)/xatrix/m_insane.o \
	$(BUILDDIR)/xatrix/m_medic.o \
	$(BUILDDIR)/xatrix/m_move.o \
	$(BUILDDIR)/xatrix/m_mutant.o \
	$(BUILDDIR)/xatrix/m_parasite.o \
	$(BUILDDIR)/xatrix/m_soldier.o \
	$(BUILDDIR)/xatrix/m_supertank.o \
	$(BUILDDIR)/xatrix/m_tank.o \
	$(BUILDDIR)/xatrix/p_client.o \
	$(BUILDDIR)/xatrix/p_hud.o \
	$(BUILDDIR)/xatrix/p_trail.o \
	$(BUILDDIR)/xatrix/p_view.o \
	$(BUILDDIR)/xatrix/p_weapon.o \
	$(BUILDDIR)/xatrix/q_shared.o

$(BUILDDIR)/xatrix/game$(ARCH).$(SHLIBEXT) : $(XATRIX_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(XATRIX_OBJS)

$(BUILDDIR)/xatrix/g_ai.o :        $(XATRIX_DIR)/g_ai.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_chase.o :     $(XATRIX_DIR)/g_chase.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_cmds.o :      $(XATRIX_DIR)/g_cmds.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_combat.o :    $(XATRIX_DIR)/g_combat.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_func.o :      $(XATRIX_DIR)/g_func.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_items.o :     $(XATRIX_DIR)/g_items.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_main.o :      $(XATRIX_DIR)/g_main.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_misc.o :      $(XATRIX_DIR)/g_misc.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_monster.o :   $(XATRIX_DIR)/g_monster.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_phys.o :      $(XATRIX_DIR)/g_phys.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_save.o :      $(XATRIX_DIR)/g_save.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_spawn.o :     $(XATRIX_DIR)/g_spawn.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_svcmds.o :    $(XATRIX_DIR)/g_svcmds.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_target.o :    $(XATRIX_DIR)/g_target.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_trigger.o :   $(XATRIX_DIR)/g_trigger.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_turret.o :    $(XATRIX_DIR)/g_turret.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_utils.o :     $(XATRIX_DIR)/g_utils.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/g_weapon.o :    $(XATRIX_DIR)/g_weapon.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_actor.o :     $(XATRIX_DIR)/m_actor.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_berserk.o :   $(XATRIX_DIR)/m_berserk.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_boss2.o :     $(XATRIX_DIR)/m_boss2.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_boss3.o :     $(XATRIX_DIR)/m_boss3.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_boss31.o :    $(XATRIX_DIR)/m_boss31.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_boss32.o :    $(XATRIX_DIR)/m_boss32.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_boss5.o :     $(XATRIX_DIR)/m_boss5.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_brain.o :     $(XATRIX_DIR)/m_brain.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_chick.o :     $(XATRIX_DIR)/m_chick.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_fixbot.o :    $(XATRIX_DIR)/m_fixbot.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_flash.o :     $(XATRIX_DIR)/m_flash.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_flipper.o :   $(XATRIX_DIR)/m_flipper.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_float.o :     $(XATRIX_DIR)/m_float.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_flyer.o :     $(XATRIX_DIR)/m_flyer.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_gekk.o :      $(XATRIX_DIR)/m_gekk.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_gladb.o :     $(XATRIX_DIR)/m_gladb.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_gladiator.o : $(XATRIX_DIR)/m_gladiator.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_gunner.o :    $(XATRIX_DIR)/m_gunner.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_hover.o :     $(XATRIX_DIR)/m_hover.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_infantry.o :  $(XATRIX_DIR)/m_infantry.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_insane.o :    $(XATRIX_DIR)/m_insane.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_medic.o :     $(XATRIX_DIR)/m_medic.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_move.o :      $(XATRIX_DIR)/m_move.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_mutant.o :    $(XATRIX_DIR)/m_mutant.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_parasite.o :  $(XATRIX_DIR)/m_parasite.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_soldier.o :   $(XATRIX_DIR)/m_soldier.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_supertank.o : $(XATRIX_DIR)/m_supertank.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/m_tank.o :      $(XATRIX_DIR)/m_tank.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/p_client.o :    $(XATRIX_DIR)/p_client.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/p_hud.o :       $(XATRIX_DIR)/p_hud.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/p_trail.o :     $(XATRIX_DIR)/p_trail.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/p_view.o :      $(XATRIX_DIR)/p_view.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/p_weapon.o :    $(XATRIX_DIR)/p_weapon.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/xatrix/q_shared.o :    $(XATRIX_DIR)/q_shared.c
	$(DO_SHLIB_CC)

#############################################################################
# ROGUE
#############################################################################

ROGUE_OBJS = \
	$(BUILDDIR)/rogue/dm_ball.o \
	$(BUILDDIR)/rogue/dm_tag.o \
	$(BUILDDIR)/rogue/g_ai.o \
	$(BUILDDIR)/rogue/g_chase.o \
	$(BUILDDIR)/rogue/g_cmds.o \
	$(BUILDDIR)/rogue/g_combat.o \
	$(BUILDDIR)/rogue/g_func.o \
	$(BUILDDIR)/rogue/g_items.o \
	$(BUILDDIR)/rogue/g_main.o \
	$(BUILDDIR)/rogue/g_misc.o \
	$(BUILDDIR)/rogue/g_monster.o \
	$(BUILDDIR)/rogue/g_newai.o \
	$(BUILDDIR)/rogue/g_newdm.o \
	$(BUILDDIR)/rogue/g_newfnc.o \
	$(BUILDDIR)/rogue/g_newtarg.o \
	$(BUILDDIR)/rogue/g_newtrig.o \
	$(BUILDDIR)/rogue/g_newweap.o \
	$(BUILDDIR)/rogue/g_phys.o \
	$(BUILDDIR)/rogue/g_save.o \
	$(BUILDDIR)/rogue/g_spawn.o \
	$(BUILDDIR)/rogue/g_sphere.o \
	$(BUILDDIR)/rogue/g_svcmds.o \
	$(BUILDDIR)/rogue/g_target.o \
	$(BUILDDIR)/rogue/g_trigger.o \
	$(BUILDDIR)/rogue/g_turret.o \
	$(BUILDDIR)/rogue/g_utils.o \
	$(BUILDDIR)/rogue/g_weapon.o \
	$(BUILDDIR)/rogue/m_actor.o \
	$(BUILDDIR)/rogue/m_berserk.o \
	$(BUILDDIR)/rogue/m_boss2.o \
	$(BUILDDIR)/rogue/m_boss3.o \
	$(BUILDDIR)/rogue/m_boss31.o \
	$(BUILDDIR)/rogue/m_boss32.o \
	$(BUILDDIR)/rogue/m_brain.o \
	$(BUILDDIR)/rogue/m_carrier.o \
	$(BUILDDIR)/rogue/m_chick.o \
	$(BUILDDIR)/rogue/m_flash.o \
	$(BUILDDIR)/rogue/m_flipper.o \
	$(BUILDDIR)/rogue/m_float.o \
	$(BUILDDIR)/rogue/m_flyer.o \
	$(BUILDDIR)/rogue/m_gladiator.o \
	$(BUILDDIR)/rogue/m_gunner.o \
	$(BUILDDIR)/rogue/m_hover.o \
	$(BUILDDIR)/rogue/m_infantry.o \
	$(BUILDDIR)/rogue/m_insane.o \
	$(BUILDDIR)/rogue/m_medic.o \
	$(BUILDDIR)/rogue/m_move.o \
	$(BUILDDIR)/rogue/m_mutant.o \
	$(BUILDDIR)/rogue/m_parasite.o \
	$(BUILDDIR)/rogue/m_soldier.o \
	$(BUILDDIR)/rogue/m_stalker.o \
	$(BUILDDIR)/rogue/m_supertank.o \
	$(BUILDDIR)/rogue/m_tank.o \
	$(BUILDDIR)/rogue/m_turret.o \
	$(BUILDDIR)/rogue/m_widow.o \
	$(BUILDDIR)/rogue/m_widow2.o \
	$(BUILDDIR)/rogue/p_client.o \
	$(BUILDDIR)/rogue/p_hud.o \
	$(BUILDDIR)/rogue/p_trail.o \
	$(BUILDDIR)/rogue/p_view.o \
	$(BUILDDIR)/rogue/p_weapon.o \
	$(BUILDDIR)/rogue/q_shared.o


$(BUILDDIR)/rogue/game$(ARCH).$(SHLIBEXT) : $(ROGUE_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(ROGUE_OBJS)

$(BUILDDIR)/rogue/dm_ball.o :      $(ROGUE_DIR)/dm_ball.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/dm_tag.o :       $(ROGUE_DIR)/dm_tag.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_ai.o :         $(ROGUE_DIR)/g_ai.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_chase.o :      $(ROGUE_DIR)/g_chase.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_cmds.o :       $(ROGUE_DIR)/g_cmds.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_combat.o :     $(ROGUE_DIR)/g_combat.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_func.o :       $(ROGUE_DIR)/g_func.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_items.o :      $(ROGUE_DIR)/g_items.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_main.o :	$(ROGUE_DIR)/g_main.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_misc.o :	$(ROGUE_DIR)/g_misc.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_monster.o :    $(ROGUE_DIR)/g_monster.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_newai.o :      $(ROGUE_DIR)/g_newai.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_newdm.o :      $(ROGUE_DIR)/g_newdm.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_newfnc.o :     $(ROGUE_DIR)/g_newfnc.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_newtarg.o :    $(ROGUE_DIR)/g_newtarg.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_newtrig.o :    $(ROGUE_DIR)/g_newtrig.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_newweap.o :    $(ROGUE_DIR)/g_newweap.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_phys.o :	$(ROGUE_DIR)/g_phys.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_save.o :	$(ROGUE_DIR)/g_save.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_spawn.o :      $(ROGUE_DIR)/g_spawn.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_sphere.o :     $(ROGUE_DIR)/g_sphere.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_svcmds.o :     $(ROGUE_DIR)/g_svcmds.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_target.o :     $(ROGUE_DIR)/g_target.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_trigger.o :    $(ROGUE_DIR)/g_trigger.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_turret.o :     $(ROGUE_DIR)/g_turret.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_utils.o :      $(ROGUE_DIR)/g_utils.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/g_weapon.o :     $(ROGUE_DIR)/g_weapon.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_actor.o :      $(ROGUE_DIR)/m_actor.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_berserk.o :    $(ROGUE_DIR)/m_berserk.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_boss2.o :      $(ROGUE_DIR)/m_boss2.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_boss3.o :      $(ROGUE_DIR)/m_boss3.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_boss31.o :     $(ROGUE_DIR)/m_boss31.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_boss32.o :     $(ROGUE_DIR)/m_boss32.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_brain.o :      $(ROGUE_DIR)/m_brain.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_carrier.o :    $(ROGUE_DIR)/m_carrier.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_chick.o :      $(ROGUE_DIR)/m_chick.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_flash.o :      $(ROGUE_DIR)/m_flash.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_flipper.o :    $(ROGUE_DIR)/m_flipper.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_float.o :      $(ROGUE_DIR)/m_float.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_flyer.o :      $(ROGUE_DIR)/m_flyer.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_gladiator.o :  $(ROGUE_DIR)/m_gladiator.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_gunner.o :     $(ROGUE_DIR)/m_gunner.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_hover.o :      $(ROGUE_DIR)/m_hover.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_infantry.o :   $(ROGUE_DIR)/m_infantry.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_insane.o :     $(ROGUE_DIR)/m_insane.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_medic.o :      $(ROGUE_DIR)/m_medic.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_move.o :	$(ROGUE_DIR)/m_move.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_mutant.o :     $(ROGUE_DIR)/m_mutant.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_parasite.o :   $(ROGUE_DIR)/m_parasite.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_soldier.o :    $(ROGUE_DIR)/m_soldier.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_stalker.o :    $(ROGUE_DIR)/m_stalker.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_supertank.o :  $(ROGUE_DIR)/m_supertank.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_tank.o :	$(ROGUE_DIR)/m_tank.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_turret.o :     $(ROGUE_DIR)/m_turret.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_widow.o :      $(ROGUE_DIR)/m_widow.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/m_widow2.o :     $(ROGUE_DIR)/m_widow2.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/p_client.o :     $(ROGUE_DIR)/p_client.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/p_hud.o :	 $(ROGUE_DIR)/p_hud.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/p_trail.o :      $(ROGUE_DIR)/p_trail.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/p_view.o :	$(ROGUE_DIR)/p_view.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/p_weapon.o :     $(ROGUE_DIR)/p_weapon.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/rogue/q_shared.o :     $(ROGUE_DIR)/q_shared.c
	$(DO_SHLIB_CC)

#############################################################################
# REF_SOFT
#############################################################################

REF_SOFT_OBJS = \
	$(BUILDDIR)/ref_soft/r_aclip.o \
	$(BUILDDIR)/ref_soft/r_alias.o \
	$(BUILDDIR)/ref_soft/r_bsp.o \
	$(BUILDDIR)/ref_soft/r_draw.o \
	$(BUILDDIR)/ref_soft/r_edge.o \
	$(BUILDDIR)/ref_soft/r_image.o \
	$(BUILDDIR)/ref_soft/r_light.o \
	$(BUILDDIR)/ref_soft/r_main.o \
	$(BUILDDIR)/ref_soft/r_misc.o \
	$(BUILDDIR)/ref_soft/r_model.o \
	$(BUILDDIR)/ref_soft/r_part.o \
	$(BUILDDIR)/ref_soft/r_poly.o \
	$(BUILDDIR)/ref_soft/r_polyse.o \
	$(BUILDDIR)/ref_soft/r_rast.o \
	$(BUILDDIR)/ref_soft/r_scan.o \
	$(BUILDDIR)/ref_soft/r_sprite.o \
	$(BUILDDIR)/ref_soft/r_surf.o \
	\
	$(BUILDDIR)/ref_soft/q_shared.o \
	$(BUILDDIR)/ref_soft/q_shlinux.o \
	$(BUILDDIR)/ref_soft/glob.o

ifeq ($(strip $(BUILD_JOYSTICK)),YES)
REF_SOFT_OBJS += $(BUILDDIR)/ref_soft/joystick.o
endif

ifeq ($(ARCH),i386)
REF_SOFT_OBJS += \
	$(BUILDDIR)/ref_soft/r_aclipa.o \
	$(BUILDDIR)/ref_soft/r_draw16.o \
	$(BUILDDIR)/ref_soft/r_drawa.o \
	$(BUILDDIR)/ref_soft/r_edgea.o \
	$(BUILDDIR)/ref_soft/r_scana.o \
	$(BUILDDIR)/ref_soft/r_spr8.o \
	$(BUILDDIR)/ref_soft/r_surf8.o \
	$(BUILDDIR)/ref_soft/math.o \
	$(BUILDDIR)/ref_soft/d_polysa.o \
	$(BUILDDIR)/ref_soft/r_varsa.o \
	$(BUILDDIR)/ref_soft/sys_dosa.o
endif

REF_SOFT_SVGA_OBJS = \
	$(BUILDDIR)/ref_soft/rw_svgalib.o \
	$(BUILDDIR)/ref_soft/d_copy.o \
	$(BUILDDIR)/ref_soft/rw_in_svgalib.o

REF_SOFT_X11_OBJS = \
	$(BUILDDIR)/ref_soft/rw_linux.o \
	$(BUILDDIR)/ref_soft/rw_x11.o

REF_SOFT_SDL_OBJS = \
	$(BUILDDIR)/ref_soft/rw_linux.o \
	$(BUILDDIR)/ref_soft/rw_sdl.o

REF_SOFT_AA_OBJS = \
	$(BUILDDIR)/ref_soft/rw_aa.o \
	$(BUILDDIR)/ref_soft/rw_in_aa.o

$(BUILDDIR)/ref_soft.$(SHLIBEXT) : $(REF_SOFT_OBJS) $(REF_SOFT_SVGA_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(REF_SOFT_OBJS) \
		$(REF_SOFT_SVGA_OBJS) $(SVGALDFLAGS)

$(BUILDDIR)/ref_softx.$(SHLIBEXT) : $(REF_SOFT_OBJS) $(REF_SOFT_X11_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(REF_SOFT_OBJS) \
		$(REF_SOFT_X11_OBJS) $(XLDFLAGS)

$(BUILDDIR)/ref_softsdl.$(SHLIBEXT) : $(REF_SOFT_OBJS) $(REF_SOFT_SDL_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(REF_SOFT_OBJS) \
		$(REF_SOFT_SDL_OBJS) $(SDLLDFLAGS)

$(BUILDDIR)/ref_softaa.$(SHLIBEXT) : $(REF_SOFT_OBJS) $(REF_SOFT_AA_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(REF_SOFT_OBJS) \
		$(REF_SOFT_AA_OBJS) $(AALDFLAGS)

$(BUILDDIR)/ref_soft/r_aclip.o :      $(REF_SOFT_DIR)/r_aclip.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_alias.o :      $(REF_SOFT_DIR)/r_alias.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_bsp.o :	 $(REF_SOFT_DIR)/r_bsp.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_draw.o :	$(REF_SOFT_DIR)/r_draw.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_edge.o :	$(REF_SOFT_DIR)/r_edge.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_image.o :      $(REF_SOFT_DIR)/r_image.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_light.o :      $(REF_SOFT_DIR)/r_light.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_main.o :       $(REF_SOFT_DIR)/r_main.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_misc.o :       $(REF_SOFT_DIR)/r_misc.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_model.o :      $(REF_SOFT_DIR)/r_model.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_part.o :       $(REF_SOFT_DIR)/r_part.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_poly.o :       $(REF_SOFT_DIR)/r_poly.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_polyse.o :     $(REF_SOFT_DIR)/r_polyse.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_rast.o :       $(REF_SOFT_DIR)/r_rast.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_scan.o :       $(REF_SOFT_DIR)/r_scan.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_sprite.o :     $(REF_SOFT_DIR)/r_sprite.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_surf.o :       $(REF_SOFT_DIR)/r_surf.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/r_aclipa.o :     $(LINUX_DIR)/r_aclipa.s
	$(DO_SHLIB_AS)

$(BUILDDIR)/ref_soft/r_draw16.o :     $(LINUX_DIR)/r_draw16.s
	$(DO_SHLIB_AS)

$(BUILDDIR)/ref_soft/r_drawa.o :      $(LINUX_DIR)/r_drawa.s
	$(DO_SHLIB_AS)

$(BUILDDIR)/ref_soft/r_edgea.o :      $(LINUX_DIR)/r_edgea.s
	$(DO_SHLIB_AS)

$(BUILDDIR)/ref_soft/r_scana.o :      $(LINUX_DIR)/r_scana.s
	$(DO_SHLIB_AS)

$(BUILDDIR)/ref_soft/r_spr8.o :       $(LINUX_DIR)/r_spr8.s
	$(DO_SHLIB_AS)

$(BUILDDIR)/ref_soft/r_surf8.o :      $(LINUX_DIR)/r_surf8.s
	$(DO_SHLIB_AS)

$(BUILDDIR)/ref_soft/math.o :         $(LINUX_DIR)/math.s
	$(DO_SHLIB_AS)

$(BUILDDIR)/ref_soft/d_polysa.o :     $(LINUX_DIR)/d_polysa.s
	$(DO_SHLIB_AS)

$(BUILDDIR)/ref_soft/r_varsa.o :      $(LINUX_DIR)/r_varsa.s
	$(DO_SHLIB_AS)

$(BUILDDIR)/ref_soft/sys_dosa.o :     $(LINUX_DIR)/sys_dosa.s
	$(DO_SHLIB_AS)

$(BUILDDIR)/ref_soft/q_shared.o :     $(GAME_DIR)/q_shared.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/q_shlinux.o :    $(LINUX_DIR)/q_shlinux.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/glob.o :         $(LINUX_DIR)/glob.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/d_copy.o :       $(LINUX_DIR)/d_copy.s
	$(DO_SHLIB_AS)

$(BUILDDIR)/ref_soft/rw_svgalib.o :   $(LINUX_DIR)/rw_svgalib.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/rw_in_svgalib.o : $(LINUX_DIR)/rw_in_svgalib.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/rw_x11.o :       $(LINUX_DIR)/rw_x11.c
	$(DO_SHLIB_CC) $(XCFLAGS)

$(BUILDDIR)/ref_soft/joystick.o :       $(LINUX_DIR)/joystick.c
	$(DO_SHLIB_CC) $(XCFLAGS)

$(BUILDDIR)/ref_soft/rw_linux.o :       $(LINUX_DIR)/rw_linux.c
	$(DO_SHLIB_CC) $(XCFLAGS)

$(BUILDDIR)/ref_soft/rw_sdl.o :       $(LINUX_DIR)/rw_sdl.c
	$(DO_SHLIB_CC) $(SDLCFLAGS)

$(BUILDDIR)/ref_soft/rw_aa.o :       $(LINUX_DIR)/rw_aa.c
	$(DO_SHLIB_CC)

$(BUILDDIR)/ref_soft/rw_in_aa.o : $(LINUX_DIR)/rw_in_aa.c
	$(DO_SHLIB_CC)

#############################################################################
# REF_GL
#############################################################################



	
REF_GL_OBJS = \
	$(BUILDDIR)/ref_gl/nanoWrap.o \
	\
	$(BUILDDIR)/ref_gl/gl_draw.o \
	$(BUILDDIR)/ref_gl/gl_image.o \
	$(BUILDDIR)/ref_gl/gl_light.o \
	$(BUILDDIR)/ref_gl/gl_mesh.o \
	$(BUILDDIR)/ref_gl/gl_model.o \
	$(BUILDDIR)/ref_gl/gl_rmain.o \
	$(BUILDDIR)/ref_gl/gl_rmisc.o \
	$(BUILDDIR)/ref_gl/gl_rsurf.o \
	$(BUILDDIR)/ref_gl/gl_warp.o \
	\
	$(BUILDDIR)/ref_gl/qgl_linux.o \
	$(BUILDDIR)/ref_gl/q_shared.o \
	$(BUILDDIR)/ref_gl/q_shlinux.o \
	$(BUILDDIR)/ref_gl/glob.o

REF_GLX_OBJS = \
	$(BUILDDIR)/ref_gl/gl_glx.o \
	$(BUILDDIR)/ref_gl/rw_linux.o \
	$(BUILDDIR)/ref_gl/joystick.o
#	$(BUILDDIR)/ref_gl/rw_x11.o

REF_CANDY_GL_OBJS = $(REF_GL_OBJS)

REF_CANDY_GLX_OBJS = $(REF_GLX_OBJS)

REF_FXGL_OBJS = \
	$(BUILDDIR)/ref_gl/rw_in_svgalib.o \
	$(BUILDDIR)/ref_gl/gl_fxmesa.o

REF_SDLGL_OBJS = \
	$(BUILDDIR)/ref_gl/rw_sdl.o \
	$(BUILDDIR)/ref_gl/rw_linux.o \
	$(BUILDDIR)/ref_gl/joystick.o

$(BUILDDIR)/ref_glx.$(SHLIBEXT) : $(REF_GL_OBJS) $(REF_GLX_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(REF_GL_OBJS) $(REF_GLX_OBJS) $(GLXLDFLAGS)

$(BUILDDIR)/ref_gl.$(SHLIBEXT) : $(REF_GL_OBJS) $(REF_FXGL_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(REF_GL_OBJS) $(REF_FXGL_OBJS) $(FXGLLDFLAGS)

$(BUILDDIR)/ref_sdlgl.$(SHLIBEXT) : $(REF_GL_OBJS) $(REF_SDLGL_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(REF_GL_OBJS) $(REF_SDLGL_OBJS) $(SDLGLLDFLAGS)

$(BUILDDIR)/ref_candyglx.$(SHLIBEXT):$(REF_CANDY_GL_OBJS) $(REF_CANDY_GLX_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(REF_CANDY_GL_OBJS) $(REF_CANDY_GLX_OBJS) $(GLXLDFLAGS)

$(BUILDDIR)/ref_candysdlgl.$(SHLIBEXT) : $(REF_CANDY_GL_OBJS) $(REF_SDLGL_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(REF_CANDY_GL_OBJS) $(REF_SDLGL_OBJS) $(SDLGLLDFLAGS)



$(BUILDDIR)/ref_gl/nanoWrap.o : src/android/nanoWrap.c
	$(DO_GL_SHLIB_CC)
	
$(BUILDDIR)/ref_gl/gl_draw.o :        $(REF_GL_DIR)/gl_draw.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_image.o :       $(REF_GL_DIR)/gl_image.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_light.o :       $(REF_GL_DIR)/gl_light.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_mesh.o :        $(REF_GL_DIR)/gl_mesh.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_model.o :       $(REF_GL_DIR)/gl_model.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_rmain.o :       $(REF_GL_DIR)/gl_rmain.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_rmisc.o :       $(REF_GL_DIR)/gl_rmisc.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_rsurf.o :       $(REF_GL_DIR)/gl_rsurf.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_warp.o :        $(REF_GL_DIR)/gl_warp.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/qgl_linux.o :      $(LINUX_DIR)/qgl_linux.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/q_shared.o :       $(GAME_DIR)/q_shared.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/q_shlinux.o :      $(LINUX_DIR)/q_shlinux.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/glob.o :           $(LINUX_DIR)/glob.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/gl_glx.o :         $(LINUX_DIR)/gl_glx.c
	$(DO_GL_SHLIB_CC) $(GLXCFLAGS)

$(BUILDDIR)/ref_gl/rw_linux.o :     $(LINUX_DIR)/rw_linux.c
	$(DO_GL_SHLIB_CC) $(XCFLAGS)

$(BUILDDIR)/ref_gl/joystick.o :     $(LINUX_DIR)/joystick.c
	$(DO_GL_SHLIB_CC) $(XCFLAGS)

$(BUILDDIR)/ref_gl/rw_x11.o :         $(LINUX_DIR)/rw_x11.c
	$(DO_GL_SHLIB_CC) $(GLXCFLAGS)

$(BUILDDIR)/ref_gl/gl_fxmesa.o :      $(LINUX_DIR)/gl_fxmesa.c
	$(DO_GL_SHLIB_CC) $(FXGLCFLAGS)

$(BUILDDIR)/ref_gl/rw_in_svgalib.o :  $(LINUX_DIR)/rw_in_svgalib.c
	$(DO_GL_SHLIB_CC)

$(BUILDDIR)/ref_gl/rw_sdl.o :         $(LINUX_DIR)/rw_sdl.c
	$(DO_GL_SHLIB_CC) $(SDLGLCFLAGS)

#############################################################################
# MISC
#############################################################################

clean: clean-debug clean-release

clean-debug:
	$(MAKE) clean2 BUILDDIR=$(BUILD_DEBUG_DIR) CFLAGS="$(DEBUG_CFLAGS)"

clean-release:
	$(MAKE) clean2 BUILDDIR=$(BUILD_RELEASE_DIR) CFLAGS="$(DEBUG_CFLAGS)"

clean2:
	rm -f \
	$(QUAKE2_OBJS) \
	$(QUAKE2_AS_OBJS) \
	$(GAME_OBJS) \
	$(CTF_OBJS) \
	$(ROGUE_OBJS) \
	$(XATRIX_OBJS) \
	$(REF_SOFT_OBJS) \
	$(REF_SOFT_SVGA_OBJS) \
	$(REF_SOFT_X11_OBJS) \
	$(REF_GL_OBJS)

distclean:
	@-rm -rf $(BUILD_DEBUG_DIR) $(BUILD_RELEASE_DIR)
	@-rm -f `find . \( -not -type d \) -and \
		\( -name '*~' \) -type f -print`
