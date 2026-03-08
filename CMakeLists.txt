cmake_minimum_required(VERSION 3.16)

project(AttendanceSystem VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Sql Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Sql Multimedia MultimediaWidgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        LocalStorage/localstorage.cpp
        LocalStorage/localstorage.h


)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AttendanceSystem
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        LocalStorage/localstorage.h LocalStorage/localstorage.cpp
        CameraCapture/cameracapture.h CameraCapture/cameracapture.cpp
        CameraCapture/videoframecapture.h CameraCapture/videoframecapture.cpp
        CameraCapture/videoframeconverter.h CameraCapture/videoframeconverter.cpp
        FaceRecognition/facerecognition.h FaceRecognition/facerecognition.cpp
        FaceRecognition/arcfaceengine.h FaceRecognition/arcfaceengine.cpp
        FaceRecognition/facefeaturedb.h FaceRecognition/facefeaturedb.cpp
        FaceRecognition/facerecognizer.h FaceRecognition/facerecognizer.cpp
        third_party/arcface/include/merror.h


    )
else()
    if(ANDROID)
        add_library(AttendanceSystem SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(AttendanceSystem
            ${PROJECT_SOURCES}
        )
    endif()
endif()

set(ARCFACE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/arcface/include)
set(ARCFACE_LIB_DIR ${CMAKE_SOURCE_DIR}/third_party/arcface/lib)

target_include_directories(AttendanceSystem PRIVATE
    ${ARCFACE_INCLUDE_DIR}
)

target_link_libraries(AttendanceSystem PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    ${ARCFACE_LIB_DIR}/libarcsoft_face_engine.lib
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AttendanceSystem)
endif()
