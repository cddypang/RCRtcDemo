cmake_minimum_required(VERSION 3.5)

project(RCRtcDemo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# find rc sdk
set(RTC_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../../01gitlab2/webrtc_pc)
set(RTC_LIBS_PATH ${CMAKE_SOURCE_DIR}/../../01gitlab2/webrtc_pc/out)

include_directories("${RTC_INCLUDE_PATH}/sdk/include")
include_directories("${RTC_INCLUDE_PATH}/sdk/include/im")
include_directories("${RTC_INCLUDE_PATH}/sdk/include/rtc")

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
        #link_directories("sdk/lib/win/x64")
        link_directories("${RTC_LIBS_PATH}/x64_debug")
        message("build on windows x64 system")
    ELSE ()
        link_directories("sdk/lib/win/x86")
        message("build on windows x86 system")
    ENDIF ()
ENDIF () # (CMAKE_SYSTEM_NAME MATCHES "Windows")

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mw_listener_impl.cpp
        mainwindow.h
        mainwindow.ui
        rtc_room_dialog.h
        rtc_room_dialog.cpp
        rtc_room_dialog.ui
        cutils.h
        cutils.cpp
        rtc_device_manger.h
        rtc_device_manger.cpp
        rtc_video_render.h
        rtc_video_render.cpp
        rtc_video_render.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RCRtcDemo
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET RCRtcDemo APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(RCRtcDemo SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(RCRtcDemo
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(RCRtcDemo Qt${QT_VERSION_MAJOR}::Widgets)
# link rc sdk
if(WIN32)
    target_link_libraries(RCRtcDemo "rongcommon.dll.lib")
    target_link_libraries(RCRtcDemo "rongim.dll.lib")
    target_link_libraries(RCRtcDemo "rongrtc.dll.lib")
elseif(APPLE)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(RCRtcDemo "librongcommon.so")
    target_link_libraries(RCRtcDemo "librongim.so")
    target_link_libraries(RCRtcDemo "librongrtc.so")
endif()

set_target_properties(RCRtcDemo PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RCRtcDemo)
endif()
