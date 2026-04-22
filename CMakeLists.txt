cmake_minimum_required(VERSION 3.16)

project(AttendanceServer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Core Gui NetWork Sql Widgets)

qt_standard_project_setup(REQUIRES 6.8)

set(ARCFACE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/arcface")
set(ARCFACE_INCLUDE_DIR "${ARCFACE_ROOT}/include")
set(ARCFACE_LIB_DIR "${ARCFACE_ROOT}/lib")

qt_add_executable(appAttendanceServer
    main.cpp
    src/TcpServer/tcpserver.cpp
)

qt_add_qml_module(appAttendanceServer
    URI AttendanceServer
    QML_FILES
        Main.qml
        SOURCES src/TcpServer/tcpserver.h src/TcpServer/tcpserver.cpp
        SOURCES src/DataManager/datamanager.h src/DataManager/datamanager.cpp
        SOURCES src/Models/Person.h
        SOURCES src/Models/attendancerecord.h
        SOURCES src/Models/device.h
        SOURCES src/Models/faceData.h src/Models/faceData.cpp
        SOURCES src/Models/Person.cpp
        SOURCES src/Models/attendancerecord.cpp
        SOURCES src/Models/device.cpp
        SOURCES src/Controllers/networkcontroller.h src/Controllers/networkcontroller.cpp
        SOURCES src/Protocol/protocol.h
        SOURCES src/DeviceMonitor/devicemonitor.h src/DeviceMonitor/devicemonitor.cpp
        SOURCES src/SyncManager/syncmanager.h src/SyncManager/syncmanager.cpp
        SOURCES src/AttendanceAnalyzer/attendanceanalyzer.h src/AttendanceAnalyzer/attendanceanalyzer.cpp
        SOURCES src/FaceDataManager/facedatamanager.h src/FaceDataManager/facedatamanager.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appAttendanceServer PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appAttendanceServer
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appAttendanceServer
    PRIVATE Qt6::Quick
            Qt6::Core
            Qt6::Gui
            Qt6::Network
            Qt6::Sql
            Qt6::Widgets
            "${ARCFACE_LIB_DIR}/libarcsoft_face_engine.lib"
)

target_include_directories(appAttendanceServer
    PRIVATE
        "${ARCFACE_INCLUDE_DIR}"
)

# Ensure ArcSoft runtime DLLs are beside the exe (Windows).
if(WIN32)
    add_custom_command(TARGET appAttendanceServer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${ARCFACE_LIB_DIR}/libarcsoft_face_engine.dll"
            "$<TARGET_FILE_DIR:appAttendanceServer>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${ARCFACE_LIB_DIR}/libarcsoft_face.dll"
            "$<TARGET_FILE_DIR:appAttendanceServer>"
        VERBATIM
    )
endif()

include(GNUInstallDirs)
install(TARGETS appAttendanceServer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
