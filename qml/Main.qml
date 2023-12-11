import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.VirtualKeyboard
import QtQuick.Layouts

import OPC_UA_Browser

ApplicationWindow {
    id: window

    property ThemeMainWindow theme: Style.mainWindow
    property int themeIndex: 0
    onThemeIndexChanged: Style.currentThemeIndex = themeIndex

    Settings {
        property alias themeIndex: window.themeIndex
    }

    Component.onCompleted: {
        Style.currentThemeIndex = window.themeIndex
    }

    width: 350
    height: 640
    visible: true
    color: theme.background
    title: qsTr("OPC UA Browser")
    header: Rectangle {
        height: 40
        color: theme.header.background

        IconImage {
            id: leftImage

            readonly property alias showBackButton: contentView.showBackButtonInHeader

            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            height: 25
            width: height
            source: leftImage.showBackButton ? "qrc:/icons/back.png" : "qrc:/icons/menu.png"
            color: theme.header.iconColor

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    if (leftImage.showBackButton) {
                        contentView.goBack()
                    } else {
                        sideMenu.open()
                    }
                }
            }
        }

        Image {
            anchors.centerIn: parent
            height: parent.height - 10
            fillMode: Image.PreserveAspectFit
            source: "qrc:/icons/logo_basyskom.svg"
        }

        IconImage {
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            height: 25
            width: height
            source: "qrc:/icons/save.png"
            color: theme.header.iconColor
            visible: contentView.canSaveDashboard

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {

                }
            }
        }
    }

    SideMenu {
        id: sideMenu

        y: -header.height
        menuHeight: parent.height + header.height
        menuWidth: 0.8 * parent.width

        onAddConnectionSelected: {
            // ToDo: add new connection
            BackEnd.disconnectFromEndpoint()
            BackEnd.clearServerList()
            BackEnd.clearEndpointList()
            contentView.showConnectionView()
        }

        onCloseConnectionSelected: {
            BackEnd.disconnectFromEndpoint()
            contentView.showConnectionView()
        }
        onShowDashboardsSelected: contentView.showDashboardView()
        onShowExpertModeSelected: contentView.showExpertBrowserView()
        onShowImprintSelected: contentView.showImprintView()
        onShowSettingsSelected: contentView.showSettingsView()
    }

    ContentView {
        id: contentView

        anchors.fill: parent
    }

    InputPanel {
        id: inputPanel

        z: 99
        x: 0
        y: window.height
        width: window.width
        visible: active

        states: State {
            name: "visible"
            when: inputPanel.active
            PropertyChanges {
                target: inputPanel
                y: window.height - inputPanel.height
            }
        }

        transitions: Transition {
            from: ""
            to: "visible"
            reversible: true
            ParallelAnimation {
                NumberAnimation {
                    properties: "y"
                    duration: 250
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }
}
