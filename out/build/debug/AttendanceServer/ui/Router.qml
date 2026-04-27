import QtQuick
import QtQuick.Controls
import "pages"

Item {
    id: root

    property string currentKey: "dashboard"
    property bool _navigating: false

    function navigate(key) {
        if (!key || key === currentKey) return
        _navigating = true
        currentKey = key
    }

    Loader {
        id: pageLoader
        anchors.fill: parent
        opacity: root._navigating ? 0.0 : 1.0
        Behavior on opacity { NumberAnimation { duration: 140; easing.type: Easing.OutCubic } }
        onLoaded: root._navigating = false
        sourceComponent: {
            switch (root.currentKey) {
            case "people": return peoplePage
            case "face": return facePage
            case "attendance": return attendancePage
            case "analyzer": return analyzerPage
            case "devices": return devicesPage
            case "settings": return settingsPage
            default: return dashboardPage
            }
        }
    }

    Component { id: dashboardPage; DashboardPage {} }
    Component { id: peoplePage; PeoplePage {} }
    Component { id: facePage; FacePage {} }
    Component { id: attendancePage; AttendancePage {} }
    Component { id: analyzerPage; AnalyzerPage {} }
    Component { id: devicesPage; DevicesPage {} }
    Component { id: settingsPage; SettingsPage {} }
}
