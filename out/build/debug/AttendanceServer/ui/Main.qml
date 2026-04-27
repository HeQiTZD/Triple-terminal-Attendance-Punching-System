pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// 说明：当前仓库里 `ui/components`、`ui/pages` 目录未在源代码中出现（仅在构建输出里存在），
// 所以这里做成一个自包含的简单主界面，方便你先把“主界面”跑通；后续再替换为真实页面组件即可。

ApplicationWindow{
	id:win
	width:1200
	height:800
	visible:true
	title:qsTr("考勤管理系统 - AttendanceServer")
	color: "#0b1220"

	property int currentIndex: 0

	function pageTitle(idx) {
		switch (idx) {
		case 0: return qsTr("仪表盘")
		case 1: return qsTr("考勤记录")
		case 2: return qsTr("人员管理")
		case 3: return qsTr("设备管理")
		case 4: return qsTr("设置")
		default: return qsTr("主页")
		}
	}

	Component.onCompleted: clockTimer.restart()

	Timer {
		id: clockTimer
		interval: 1000
		repeat: true
		running: true
		onTriggered: timeText.text = Qt.formatDateTime(new Date(), "yyyy-MM-dd  HH:mm:ss")
	}

	header: ToolBar {
		background: Rectangle { color: "#0f1b2d" }
		contentItem: RowLayout {
			spacing: 12
			ToolButton {
				text: "\u2630" // ☰
				onClicked: sideBar.visible ? sideBar.close() : sideBar.open()
				font.pixelSize: 18
			}
			Label {
				text: qsTr("AttendanceServer")
				font.pixelSize: 18
				font.bold: true
				color: "#e6eefc"
				Layout.alignment: Qt.AlignVCenter
			}
			Rectangle {
				Layout.fillWidth: true
				height: 1
				opacity: 0
			}
			Label {
				id: timeText
				text: Qt.formatDateTime(new Date(), "yyyy-MM-dd  HH:mm:ss")
				color: "#a9bddc"
				font.pixelSize: 12
				Layout.alignment: Qt.AlignVCenter
			}
			Rectangle {
				width: 10
				height: 10
				radius: 5
				color: "#22c55e"
				Layout.alignment: Qt.AlignVCenter
			}
			Label {
				text: qsTr("服务正常")
				color: "#a9bddc"
				font.pixelSize: 12
				Layout.alignment: Qt.AlignVCenter
			}
			Item { width: 6 }
		}
	}

	Drawer {
		id: sideBar
		width: 260
		height: win.height
		edge: Qt.LeftEdge
		modal: false
		interactive: true
		background: Rectangle { color: "#0f1b2d" }

		contentItem: Loader { anchors.fill: parent; sourceComponent: sideBarContent }
	}

	Component {
		id: sideBarContent
		ColumnLayout {
			anchors.fill: parent
			anchors.margins: 14
			spacing: 12

			Rectangle {
				Layout.fillWidth: true
				height: 78
				radius: 14
				color: "#12233c"
				border.color: "#1e3353"
				RowLayout {
					anchors.fill: parent
					anchors.margins: 12
					spacing: 10
					Rectangle {
						width: 42
						height: 42
						radius: 12
						color: "#1d4ed8"
						Label {
							anchors.centerIn: parent
							text: "AS"
							color: "white"
							font.bold: true
						}
					}
					ColumnLayout {
						Layout.fillWidth: true
						spacing: 2
						Label { text: qsTr("考勤管理系统"); color: "#e6eefc"; font.bold: true; font.pixelSize: 14 }
						Label { text: qsTr("简单主界面（示例）"); color: "#a9bddc"; font.pixelSize: 11 }
					}
				}
			}

			ListView {
				id: navList
				Layout.fillWidth: true
				Layout.fillHeight: true
				clip: true
				spacing: 8
				model: [
					{ icon: "\u25A3", title: qsTr("仪表盘") },
					{ icon: "\u23F1", title: qsTr("考勤记录") },
					{ icon: "\u263A", title: qsTr("人员管理") },
					{ icon: "\u2699", title: qsTr("设备管理") },
					{ icon: "\u2692", title: qsTr("设置") }
				]
				delegate: Item {
					id: navItem
					required property int index
					required property var modelData

					width: navList.width
					height: 44
					readonly property bool active: navItem.index === win.currentIndex

					Rectangle {
						anchors.fill: parent
						radius: 12
						color: navItem.active ? "#1a2f52" : "#00000000"
						border.color: navItem.active ? "#2b4f86" : "#1e3353"
						border.width: 1
					}

					MouseArea {
						anchors.fill: parent
						hoverEnabled: true
						onClicked: {
							win.currentIndex = navItem.index
							sideBar.close()
						}
					}

					RowLayout {
						anchors.fill: parent
						anchors.margins: 10
						spacing: 10
						Label {
							text: navItem.modelData.icon
							color: navItem.active ? "#e6eefc" : "#a9bddc"
							font.pixelSize: 14
							Layout.alignment: Qt.AlignVCenter
						}
						Label {
							text: navItem.modelData.title
							color: navItem.active ? "#e6eefc" : "#a9bddc"
							font.pixelSize: 13
							Layout.fillWidth: true
							elide: Text.ElideRight
							Layout.alignment: Qt.AlignVCenter
						}
					}
				}
			}

			Rectangle {
				Layout.fillWidth: true
				height: 54
				radius: 14
				color: "#0b1220"
				border.color: "#1e3353"
				RowLayout {
					anchors.fill: parent
					anchors.margins: 12
					spacing: 10
					Label { text: qsTr("版本"); color: "#a9bddc"; font.pixelSize: 12 }
					Label { text: "v0.1"; color: "#e6eefc"; font.pixelSize: 12; font.bold: true }
					Rectangle { Layout.fillWidth: true; height: 1; opacity: 0 }
					Button {
						text: qsTr("退出")
						onClicked: Qt.quit()
					}
				}
			}
		}
	}

	RowLayout {
		anchors.fill: parent
		spacing: 0

		// 桌面端同时显示一个固定侧栏；小屏可用 Drawer（上方按钮）
		Rectangle {
			visible: win.width >= 1000
			Layout.preferredWidth: 260
			Layout.fillHeight: true
			color: "#0f1b2d"
			border.color: "#12233c"
			border.width: 1

			// 复用 Drawer 内部内容（避免重复定义一套逻辑）
			Loader {
				anchors.fill: parent
				sourceComponent: sideBarContent
			}
		}

		Rectangle {
			Layout.fillWidth: true
			Layout.fillHeight: true
			color: "#0b1220"

			ColumnLayout {
				anchors.fill: parent
				anchors.margins: 18
				spacing: 14

				RowLayout {
					Layout.fillWidth: true
					spacing: 12
					Label {
						text: win.pageTitle(win.currentIndex)
						color: "#e6eefc"
						font.pixelSize: 22
						font.bold: true
						Layout.alignment: Qt.AlignVCenter
					}
					Rectangle { Layout.fillWidth: true; height: 1; opacity: 0 }
					TextField {
						placeholderText: qsTr("搜索…")
						Layout.preferredWidth: 260
					}
					Button {
						text: qsTr("刷新")
					}
				}

				Rectangle {
					Layout.fillWidth: true
					Layout.fillHeight: true
					radius: 16
					color: "#0f1b2d"
					border.color: "#1e3353"

					Loader {
						id: pageLoader
						anchors.fill: parent
						anchors.margins: 16
						sourceComponent: win.currentIndex === 0 ? dashboardPage
							: win.currentIndex === 1 ? attendancePage
							: win.currentIndex === 2 ? peoplePage
							: win.currentIndex === 3 ? devicesPage
							: settingsPage
					}
				}
			}
		}
	}

	Component {
		id: dashboardPage
		ColumnLayout {
			spacing: 12
			Label { text: qsTr("今日概览"); color: "#e6eefc"; font.pixelSize: 16; font.bold: true }
			RowLayout {
				spacing: 12
				Repeater {
					model: [
						{ title: qsTr("到岗人数"), value: "86" },
						{ title: qsTr("缺勤人数"), value: "4" },
						{ title: qsTr("设备在线"), value: "12" }
					]
					delegate: Rectangle {
						id: dashTile
						required property var modelData
						Layout.fillWidth: true
						Layout.preferredHeight: 96
						radius: 14
						color: "#12233c"
						border.color: "#1e3353"
						Column {
							anchors.fill: parent
							anchors.margins: 14
							spacing: 6
							Text { text: dashTile.modelData.title; color: "#a9bddc"; font.pixelSize: 12 }
							Text { text: dashTile.modelData.value; color: "#e6eefc"; font.pixelSize: 28; font.bold: true }
						}
					}
				}
			}
			Rectangle {
				Layout.fillWidth: true
				Layout.fillHeight: true
				radius: 14
				color: "#0b1220"
				border.color: "#1e3353"
				Column {
					anchors.fill: parent
					anchors.margins: 14
					spacing: 8
					Text { text: qsTr("提示"); color: "#e6eefc"; font.pixelSize: 14; font.bold: true }
					Text {
						text: qsTr("这里可以放置告警、最新打卡记录、趋势图等内容。")
						color: "#a9bddc"
						wrapMode: Text.WordWrap
					}
				}
			}
		}
	}

	Component {
		id: attendancePage
		ColumnLayout {
			spacing: 10
			Label { text: qsTr("考勤记录"); color: "#e6eefc"; font.pixelSize: 16; font.bold: true }
			TableView {
				Layout.fillWidth: true
				Layout.fillHeight: true
				clip: true
				model: 20
				delegate: Rectangle {
					id: attRow
					required property int row
					implicitHeight: 40
					color: (row % 2 === 0) ? "#0b1220" : "#0e1728"
					Text {
						anchors.verticalCenter: parent.verticalCenter
						anchors.left: parent.left
						anchors.leftMargin: 12
						text: qsTr("记录 #%1  -  2026-04-26  09:%2").arg(attRow.row + 1).arg((attRow.row % 60).toString().padStart(2, "0"))
						color: "#a9bddc"
					}
				}
			}
		}
	}

	Component {
		id: peoplePage
		ColumnLayout {
			spacing: 10
			Label { text: qsTr("人员管理"); color: "#e6eefc"; font.pixelSize: 16; font.bold: true }
			RowLayout {
				spacing: 10
				Button { text: qsTr("新增人员") }
				Button { text: qsTr("导入") }
				Button { text: qsTr("导出") }
				Rectangle { Layout.fillWidth: true; height: 1; opacity: 0 }
			}
			Rectangle {
				Layout.fillWidth: true
				Layout.fillHeight: true
				radius: 14
				color: "#0b1220"
				border.color: "#1e3353"
				ListView {
					anchors.fill: parent
					anchors.margins: 10
					clip: true
					model: 30
					delegate: Rectangle {
						id: personRow
						required property int index
						width: parent.width
						height: 46
						radius: 12
						color: "#00000000"
						border.color: "#12233c"
						RowLayout {
							anchors.fill: parent
							anchors.margins: 10
							Text { text: qsTr("员工 #%1").arg(personRow.index + 1); color: "#e6eefc" }
							Rectangle { Layout.fillWidth: true; height: 1; opacity: 0 }
							Text { text: qsTr("正常"); color: "#22c55e" }
						}
					}
				}
			}
		}
	}

	Component {
		id: devicesPage
		ColumnLayout {
			spacing: 10
			Label { text: qsTr("设备管理"); color: "#e6eefc"; font.pixelSize: 16; font.bold: true }
			RowLayout {
				spacing: 10
				Button { text: qsTr("扫描设备") }
				Button { text: qsTr("添加设备") }
				Rectangle { Layout.fillWidth: true; height: 1; opacity: 0 }
			}
			Flow {
				Layout.fillWidth: true
				Layout.fillHeight: true
				spacing: 12
				Repeater {
					model: 8
					delegate: Rectangle {
						id: devCard
						required property int index
						width: 260
						height: 110
						radius: 14
						color: "#12233c"
						border.color: "#1e3353"
						Column {
							anchors.fill: parent
							anchors.margins: 14
							spacing: 6
							Text { text: qsTr("设备 #%1").arg(devCard.index + 1); color: "#e6eefc"; font.bold: true }
							Text { text: qsTr("IP: 192.168.1.%1").arg(10 + devCard.index); color: "#a9bddc" }
							Text { text: (devCard.index % 3 === 0) ? qsTr("离线") : qsTr("在线"); color: (devCard.index % 3 === 0) ? "#f97316" : "#22c55e" }
						}
					}
				}
			}
		}
	}

	Component {
		id: settingsPage
		ColumnLayout {
			spacing: 10
			Label { text: qsTr("设置"); color: "#e6eefc"; font.pixelSize: 16; font.bold: true }
			Rectangle {
				Layout.fillWidth: true
				radius: 14
				color: "#12233c"
				border.color: "#1e3353"
				ColumnLayout {
					anchors.fill: parent
					anchors.margins: 14
					spacing: 12
					RowLayout {
						Label { text: qsTr("自动刷新"); color: "#e6eefc"; Layout.fillWidth: true }
						Switch { checked: true }
					}
					RowLayout {
						Label { text: qsTr("主题"); color: "#e6eefc"; Layout.fillWidth: true }
						ComboBox { model: [qsTr("深色"), qsTr("浅色")] }
					}
					RowLayout {
						Label { text: qsTr("服务端地址"); color: "#e6eefc"; Layout.fillWidth: true }
						TextField { text: "http://127.0.0.1:8080"; Layout.preferredWidth: 260 }
					}
				}
			}
		}
	}
}
