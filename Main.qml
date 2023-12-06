import QtQuick
import QtQuick.Controls
import QtQuick.VirtualKeyboard
import QtQuick.Layouts

ApplicationWindow {
    id: window

    width: 350
    height: 640
    visible: true
    color: Style.mainWindow.background
    title: qsTr("OPC UA Browser")
    header: Rectangle {
        height: 40
        color: Style.mainWindow.header.background

        IconImage {
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            height: 25
            width: height
            source: "qrc:/icons/menu.png"
            color: Style.mainWindow.header.iconColor

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {

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
            color: Style.mainWindow.header.iconColor
            visible: contentView.canSaveDashboard

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {

                }
            }
        }
    }

    ConnectionView {
        anchors.fill: parent
        visible: !BackEnd.isConnected
    }

    ContentView {
        id: contentView

        anchors.fill: parent
        visible: BackEnd.isConnected
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
