cmake_minimum_required(VERSION 3.16)

project(AttendanceAdmin VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Core Gui Network Widgets Charts)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appAttendanceAdmin
    main.cpp
)

set_source_files_properties(
    ui/theme/Theme.qml
    ui/models/Logger.qml
    ui/models/History.qml
    ui/models/Presets.qml
    ui/models/ErrorCatalog.qml
    ui/models/PermissionCatalog.qml
    ui/models/PushFeed.qml
    ui/models/LogReader.qml
    PROPERTIES
        QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(appAttendanceAdmin
    URI AttendanceAdmin
    QML_FILES
        ui/Main.qml
        ui/LoginPage.qml
        ui/theme/Theme.qml
        ui/models/Logger.qml
        ui/models/History.qml
        ui/models/Presets.qml
        ui/models/ErrorCatalog.qml
        ui/models/PermissionCatalog.qml
        ui/models/PushFeed.qml
        ui/models/LogReader.qml
        ui/components/Card.qml
        ui/components/BusyOverlay.qml
        ui/components/SideBar.qml
        ui/components/StatusBar.qml
        ui/components/BadgeStatus.qml
        ui/components/LabeledField.qml
        ui/components/ToolBarRow.qml
        ui/components/JsonEditor.qml
        ui/components/JsonViewer.qml
        ui/components/DataTable.qml
        ui/components/LogPanel.qml
        ui/components/ConfirmDialog.qml
        ui/components/LoginServerSettingsDialog.qml
        ui/components/WindowCaptionButtons.qml
        ui/components/PermissionButton.qml
        ui/components/PermissionDeniedDialog.qml
        ui/components/PermissionChipRow.qml
        ui/components/PageAccessPlaceholder.qml
        ui/components/AttendanceStatusPieChart.qml
        ui/components/RecentPunchList.qml
        ui/components/BarChart.qml
        ui/components/ChartLegend.qml
        ui/components/ChartContainer.qml
        ui/components/StatCard.qml
        ui/components/PageHost.qml
        ui/components/FaceSyncButton.qml
        ui/pages/PageDashboard.qml
        ui/pages/PagePerson.qml
        ui/pages/PageDevice.qml
        ui/pages/PageConfigDeploy.qml
        ui/pages/PageAttendance.qml
        ui/pages/PageFace.qml
        ui/pages/PageUser.qml
        ui/pages/PageRbac.qml
    SOURCES
        src/Models/Person.h src/Models/Person.cpp
        src/Models/attendancerecord.h src/Models/attendancerecord.cpp
        src/Models/device.h src/Models/device.cpp
        src/Models/faceData.h src/Models/faceData.cpp
        src/Protocol/protocol.h
        src/Network/TcpConnectionManager.h src/Network/TcpConnectionManager.cpp
        src/Auth/SessionManager.h src/Auth/SessionManager.cpp
        src/Person/PersonServer.h src/Person/PersonServer.cpp
        src/Attendance/AttendanceService.h src/Attendance/AttendanceService.cpp
        src/Rbac/RbacServer.h src/Rbac/RbacServer.cpp 
        src/Event/EventSubscriptionService.h src/Event/EventSubscriptionService.cpp
        src/Face/FaceServer.h src/Face/FaceServer.cpp
        src/Device/DeviceServer.h src/Device/DeviceServer.cpp
        src/Config/ConfigDeployServer.h src/Config/ConfigDeployServer.cpp
        src/User/UserServer.h src/User/UserServer.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appAttendanceAdmin PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appAttendanceAdmin
    PRIVATE Qt6::Quick
            Qt6::Core
            Qt6::Gui
            Qt6::Network
            Qt6::Widgets
            Qt6::Charts
)

include(GNUInstallDirs)
install(TARGETS appAttendanceAdmin
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
