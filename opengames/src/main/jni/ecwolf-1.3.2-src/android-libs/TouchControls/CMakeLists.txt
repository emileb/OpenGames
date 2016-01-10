cmake_minimum_required( VERSION 2.8 )

include_directories(${SIGCPP_INCLUDE_DIR} ${TINYXML_INCLUDE_DIR} ${LIBPNG_INCLUDE_DIR})

# Add -DUSE_GLES2 for OpenGLES 2
add_definitions(-DANDROID_NDK)

add_library(touchcontrols SHARED
    Button.cpp
    ControlSuper.cpp
    GLLines.cpp
    GLRect.cpp
    JNITouchControlsUtils.cpp
    Mouse.cpp
    MultitouchMouse.cpp
    OpenGLUtils.cpp
    RectF.cpp
    TouchControls.cpp
    TouchControlsContainer.cpp
    TouchJoy.cpp
    WheelSelect.cpp
)

target_link_libraries(touchcontrols GLESv1_CM log ${TINYXML_LIBRARY} ${SIGCPP_LIBRARY} ${LIBPNG_LIBRARY})
 
