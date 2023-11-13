import QtQuick
import QtQuick.Window
import QtQuick.VirtualKeyboard
import QtQuick.Layouts

Window {
    id: window
    width: 480
    height: 640
    visible: true
    color: "orange"
    title: qsTr("OPC UA Browser")

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 5

        ConnectionView {
            Layout.fillWidth: true
        }

        ContentView {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    InputPanel {
        id: inputPanel
        z: 99
        x: 0
        y: window.height
        width: window.width

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
