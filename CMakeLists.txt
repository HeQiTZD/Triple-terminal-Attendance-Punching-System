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
        LocalStorage/FaceFeatureRepository.cpp
        LocalStorage/FaceFeatureRepository.h
        LocalStorage/AttendanceOutboxRepository.cpp
        LocalStorage/AttendanceOutboxRepository.h
        LocalStorage/SyncMetaRepository.cpp
        LocalStorage/SyncMetaRepository.h
        LocalStorage/DeviceLocalRepository.cpp
        LocalStorage/DeviceLocalRepository.h
        Sync/SyncManager.cpp
        Sync/SyncManager.h
        Command/CommandHandler.cpp
        Command/CommandHandler.h
        Attendance/AttendanceConfigParser.cpp
        Attendance/AttendanceConfigParser.h
        Attendance/AttendanceConfigSyncHandler.cpp
        Attendance/AttendanceConfigSyncHandler.h
        Attendance/AttendanceConfigValidator.cpp
        Attendance/AttendanceConfigValidator.h
        Attendance/AttendanceRuleEngine.cpp
        Attendance/AttendanceRuleEngine.h
        Attendance/AttendanceReporter.cpp
        Attendance/AttendanceReporter.h
        Utils/Logger.cpp
        Utils/Logger.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AttendanceSystem
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        LocalStorage/localstorage.h LocalStorage/localstorage.cpp
        LocalStorage/FaceFeatureRepository.h LocalStorage/FaceFeatureRepository.cpp
        LocalStorage/AttendanceOutboxRepository.h LocalStorage/AttendanceOutboxRepository.cpp
        LocalStorage/SyncMetaRepository.h LocalStorage/SyncMetaRepository.cpp
        LocalStorage/DeviceLocalRepository.h LocalStorage/DeviceLocalRepository.cpp
        CameraCapture/cameracapture.h CameraCapture/cameracapture.cpp
        CameraCapture/videoframecapture.h CameraCapture/videoframecapture.cpp
        CameraCapture/videoframeconverter.h CameraCapture/videoframeconverter.cpp

        FaceRecognition/arcfaceengine.h FaceRecognition/arcfaceengine.cpp

        FaceRecognition/facerecognizer.cpp
        third_party/arcface/include/merror.h
        FaceRecognition/facedatabasemanager.h FaceRecognition/facedatabasemanager.cpp
        FaceRecognition/facerecognizer.h FaceRecognition/facerecognizer.cpp
        FaceRecognition/facefeatureextractor.h FaceRecognition/facefeatureextractor.cpp
        NetworkClient/networkclient.h NetworkClient/networkclient.cpp
        NetworkClient/connectionmanager.h NetworkClient/connectionmanager.cpp
        NetworkClient/heartbeatmanager.h NetworkClient/heartbeatmanager.cpp
        NetworkClient/messagewriter.h NetworkClient/messagewriter.cpp
        NetworkClient/messagereader.h NetworkClient/messagereader.cpp
        NetworkClient/messagequeue.h NetworkClient/messagequeue.cpp
        NetworkClient/serverprotocol.h NetworkClient/serverprotocol.cpp
        Auth/tokenmanager.h Auth/tokenmanager.cpp
        Auth/tokenrefresher.h Auth/tokenrefresher.cpp
        UI/setwindow.h UI/setwindow.cpp UI/setwindow.ui
        Config/configmanager.h Config/configmanager.cpp
        UI/facevideowidget.h UI/facevideowidget.cpp
        UI/Theme/ThemeManager.h UI/Theme/ThemeManager.cpp
        UI/Components/ActionButton.h UI/Components/ActionButton.cpp
        UI/Components/IconButton.h UI/Components/IconButton.cpp
        UI/Components/StatusIndicator.h UI/Components/StatusIndicator.cpp
        Sync/SyncManager.h Sync/SyncManager.cpp
        Command/CommandHandler.h Command/CommandHandler.cpp
        Attendance/AttendanceConfigParser.h Attendance/AttendanceConfigParser.cpp
        Attendance/AttendanceConfigSyncHandler.h Attendance/AttendanceConfigSyncHandler.cpp
        Attendance/AttendanceConfigValidator.h Attendance/AttendanceConfigValidator.cpp
        Attendance/AttendanceRuleEngine.h Attendance/AttendanceRuleEngine.cpp
        Attendance/AttendanceReporter.h Attendance/AttendanceReporter.cpp
        Utils/Logger.h Utils/Logger.cpp
        resources/resources.qrc



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
    ${CMAKE_SOURCE_DIR}
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
