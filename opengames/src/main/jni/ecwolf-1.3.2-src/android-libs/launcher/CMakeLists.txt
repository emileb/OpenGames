cmake_minimum_required(VERSION 2.8)

find_package(Java REQUIRED)
include(UseJava)

set(APK_OUTPUT "${CMAKE_BINARY_DIR}/ecwolf.apk")
set(APK_PACKAGE_NAME "com.beloko.wolf3dhg")
set(APK_ENTRY_ACTIVITY "com.beloko.wolf3d.EntryActivity")

set(ANDROID_SDK "ANDROID_SDK-NOTFOUND" CACHE PATH "Path to android.jar")
set(ANDROID_SDK_TOOLS "ANDROID_SDK_TOOLS-NOTFOUND" CACHE PATH "Path to android build-tools")
if(ANDROID_SDK STREQUAL "ANDROID_SDK-NOTFOUND" OR ANDROID_SDK_TOOLS STREQUAL "ANDROID_SDK_TOOLS-NOTFOUND")
	message(FATAL_ERROR "Please set ANDROID_SDK to location of android.jar and ANDROID_SDK_TOOLS to build-tools for the platform")
endif(ANDROID_SDK STREQUAL "ANDROID_SDK-NOTFOUND" OR ANDROID_SDK_TOOLS STREQUAL "ANDROID_SDK_TOOLS-NOTFOUND")

find_file(ANDROID_SDK_JAR android.jar PATHS "${ANDROID_SDK}")
find_file(ANDROID_AAPT_BINARY NAMES aapt aapt.exe PATHS "${ANDROID_SDK_TOOLS}")
find_file(ANDROID_DX_BINARY NAMES dx dx.exe PATHS "${ANDROID_SDK_TOOLS}")
find_file(ANDROID_ADB_BINARY NAMES adb adb.exe PATHS "${ANDROID_SDK_TOOLS}" "${ANDROID_SDK_TOOLS}/../../platform-tools")
find_file(ANDROID_JAR_SIGNER NAMES jarsigner jarsigner.exe PATHS "/usr/bin")
find_file(ANDROID_SUPPORT_V4_JAR android-support-v4.jar PATHS "${ANDROID_SDK_TOOLS}/../../extras/android/support/v4")

set(ANDROID_MOGA_CONTROLLER_JAR "" CACHE FILE "Path to com.bda.controller.jar")

set(ANDROID_SIGN_STOREPASS "" CACHE STRING "Keystore password for jarsigner")
set(ANDROID_SIGN_KEYSTORE "" CACHE FILE "Keystore name for jarsigner")
set(ANDROID_SIGN_KEYNAME "" CACHE STRING "Key name for jarsigner")

set(CMAKE_JAVA_COMPILE_FLAGS -target 1.6 -source 1.6 -Xlint:-options ${CMAKE_JAVA_COMPILE_FLAGS})

add_subdirectory(src)

add_custom_target(runadb
	COMMAND "${ANDROID_ADB_BINARY}" install -r "${APK_OUTPUT}"
	COMMAND "${ANDROID_ADB_BINARY}" shell am start "${APK_PACKAGE_NAME}/${APK_ENTRY_ACTIVITY}"
	COMMAND "${ANDROID_ADB_BINARY}" shell logcat
	DEPENDS ecwolf-android
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
	COMMENT "Launching apk via adb..."
)
