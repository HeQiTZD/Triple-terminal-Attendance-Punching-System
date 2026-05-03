import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

/*
  统一面板：标题栏 + 内容区。
  使用 ColumnLayout 顶层布局；内容默认带内边距，可以通过 padding 调整。
  默认属性 contentChildren 收纳子节点到内容区（contentItem.data）。
*/
Rectangle {
    id: root
    color: Theme.surface
    border.color: Theme.border
    border.width: 1
    radius: Theme.radiusMd
    clip: true

    property string title: ""
    property string subtitle: ""
    property int padding: Theme.spacingMd
    /// false：高度随表单等内容增长（表单页）；true：纵向占满占位，便于 DataTable/JsonViewer anchors.fill。
    property bool stretchContent: false
    property alias headerRight: headerRightSlot.data
    default property alias contentChildren: contentItem.data

    implicitWidth: 200
    implicitHeight: layout.implicitHeight

    ColumnLayout {
        id: layout
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 38
            visible: root.title.length > 0 || root.subtitle.length > 0 || headerRightSlot.children.length > 0

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Theme.spacingMd
                anchors.rightMargin: Theme.spacingMd
                spacing: Theme.spacingMd

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter
                    spacing: 1

                    Label {
                        text: root.title
                        color: Theme.text
                        font.pixelSize: Theme.fontLg
                        font.family: Theme.fontFamily
                        font.bold: true
                        visible: text.length > 0
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                    Label {
                        text: root.subtitle
                        color: Theme.textMuted
                        font.pixelSize: Theme.fontXs
                        font.family: Theme.fontFamily
                        visible: text.length > 0
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                }

                Row {
                    id: headerRightSlot
                    spacing: Theme.spacingSm
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Theme.border
            visible: root.title.length > 0 || root.subtitle.length > 0 || headerRightSlot.children.length > 0
        }

        Item {
            id: contentWrap
            Layout.fillWidth: true
            Layout.fillHeight: root.stretchContent
            implicitHeight: root.stretchContent ? 1 : Math.max(contentItem.childrenRect.height + 2 * root.padding,
                                                                  Theme.spacingMd)

            Item {
                id: contentItem
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: root.padding
                height: root.stretchContent ? Math.max(0, contentWrap.height - 2 * root.padding) : implicitHeight
                implicitHeight: childrenRect.height
                clip: true
            }
        }
    }
}
