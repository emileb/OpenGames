
LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)


LOCAL_MODULE    := abuse


LOCAL_CFLAGS := -O2 -DHAVE_OPENGL -DPACKAGE_VERSION=\"0.8\"   -DPACKAGE_NAME=\"Abuse\" -DHAVE_SYS_IOCTL_H -DHAVE_NETINET_IN_H -DASSETDIR=\"/sdcard/abuse/\"
LOCAL_CPPFLAGS := 


LOCAL_LDLIBS +=  -lGLESv1_CM -lEGL -llog -lz
LOCAL_STATIC_LIBRARIES := sigc libzip libpng  lz
LOCAL_SHARED_LIBRARIES := touchcontrols openal SDL SDL_mixer libvorbis libogg



LOCAL_C_INCLUDES +=  $(LOCAL_PATH)/imlib $(LOCAL_PATH)/lisp $(LOCAL_PATH)/lol $(LOCAL_PATH)/net
LOCAL_C_INCLUDES +=  $(TOP_DIR) $(TOP_DIR)/MobileTouchControls  $(TOP_DIR)/SDL/include $(TOP_DIR)/SDL_net/include $(TOP_DIR)/SDL_mixer

abuse_SOURCES = \
    common.h \
    \
    lol/matrix.cpp lol/matrix.h \
    lol/timer.cpp lol/timer.h \
    \
    specache.cpp specache.h \
    netcfg.cpp netcfg.h \
    innet.cpp \
    chat.cpp chat.h \
    endgame.cpp \
    loadgame.cpp loadgame.h \
    profile.cpp profile.h \
    cop.cpp cop.h \
    statbar.cpp \
    compiled.cpp compiled.h \
    ant.cpp ant.h \
    sensor.cpp \
    demo.cpp demo.h \
    lcache.cpp lcache.h \
    nfclient.cpp nfclient.h \
    clisp.cpp clisp.h \
    gui.cpp gui.h \
    transp.cpp transp.h \
    collide.cpp \
    property.cpp property.h \
    cache.cpp cache.h \
    particle.cpp particle.h \
    objects.cpp objects.h \
    extend.cpp extend.h \
    console.cpp console.h \
    ability.cpp ability.h \
    items.cpp items.h \
    dev.cpp dev.h \
    chars.cpp chars.h \
    level.cpp level.h \
    smallfnt.cpp \
    automap.cpp automap.h \
    help.cpp help.h \
    intsect.cpp intsect.h \
    loader2.cpp loader2.h \
    seq.cpp seq.h \
    points.cpp points.h \
    fnt6x13.cpp \
    morpher.cpp morpher.h \
    menu.cpp menu.h \
    director.cpp director.h \
    view.cpp view.h \
    configuration.cpp configuration.h \
    game.cpp game.h \
    light.cpp light.h \
    devsel.cpp devsel.h \
    crc.cpp crc.h \
    gamma.cpp gamma.h \
    id.h netface.h isllist.h sbar.h \
    nfserver.h \
    \
    ui/volumewindow.cpp ui/volumewindow.h \

libsdlport_a_SOURCES = \
    sdlport/video.cpp \
    sdlport/mouse.cpp \
    sdlport/event.cpp \
    sdlport/sound.cpp  \
    sdlport/timing.cpp \
    sdlport/jdir.cpp \
    sdlport/joystick.cpp  \
    sdlport/setup.cpp  \
    sdlport/hmi.cpp  \

libimlib_a_SOURCES = \
    imlib/filter.cpp filter.h \
    imlib/image.cpp image.h \
    imlib/transimage.cpp transimage.h \
    imlib/linked.cpp linked.h \
    imlib/input.cpp input.h \
    imlib/palette.cpp palette.h \
    imlib/include.cpp include.h \
    imlib/fonts.cpp fonts.h \
    imlib/specs.cpp specs.h \
    imlib/supmorph.cpp supmorph.h \
    imlib/pcxread.cpp pcxread.h \
    imlib/jrand.cpp jrand.h \
    imlib/keys.cpp keys.h \
    imlib/dprint.cpp dprint.h \
    imlib/status.cpp status.h \
    imlib/pmenu.cpp pmenu.h \
    imlib/scroller.cpp scroller.h \
    imlib/filesel.cpp filesel.h \
    imlib/tools.cpp tools.h \
    imlib/guistat.cpp guistat.h \
    imlib/sprite.cpp sprite.h \
    imlib/jwindow.cpp jwindow.h \
    imlib/video.cpp video.h \


liblisp_a_SOURCES = \
    lisp/lisp.cpp  \
    lisp/lisp_opt.cpp  \
    lisp/lisp_gc.cpp  \
    lisp/trig.cpp \

libnet_a_SOURCES = \
    net/gserver.cpp gserver.h \
    net/gclient.cpp gclient.h \
    net/fileman.cpp fileman.h \
    net/sock.cpp sock.h \
    net/tcpip.cpp tcpip.h \




LOCAL_SRC_FILES +=  android/android-jni.cpp android/in_android.cpp

LOCAL_SRC_FILES +=  $(libsdlport_a_SOURCES) $(libnet_a_SOURCES) $(liblisp_a_SOURCES) $(libimlib_a_SOURCES)  + $(abuse_SOURCES)



include $(BUILD_SHARED_LIBRARY)








