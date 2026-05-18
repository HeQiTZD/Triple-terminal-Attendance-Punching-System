import QtQuick
import QtQuick.Controls

import AttendanceAdmin

Row {
    id: root

    spacing: 2

    property bool showMinimize: true
    property bool showMaximize: true

    readonly property var hostWindow: Window.window

    ToolButton {
        visible: root.showMinimize
        text: "\u2212"
        font.pixelSize: Theme.fontLg
        onClicked: {
            if (root.hostWindow)
                root.hostWindow.showMinimized()
        }
    }

    ToolButton {
        id: maxBtn
        visible: root.showMaximize
        font.pixelSize: Theme.fontSm
        text: root.hostWindow && root.hostWindow.visibility === Window.Maximized
              ? "\u2750"
              : "\u25A1"
        onClicked: {
            if (!root.hostWindow)
                return
            if (root.hostWindow.visibility === Window.Maximized)
                root.hostWindow.showNormal()
            else
                root.hostWindow.showMaximized()
        }
    }

    ToolButton {
        text: "\u00d7"
        font.pixelSize: Theme.fontXl
        onClicked: {
            if (root.hostWindow)
                root.hostWindow.close()
        }
    }

    Connections {
        target: root.hostWindow
        function onVisibilityChanged() {
            if (maxBtn.visible)
                maxBtn.text = root.hostWindow && root.hostWindow.visibility === Window.Maximized
                        ? "\u2750"
                        : "\u25A1"
        }
    }
}
