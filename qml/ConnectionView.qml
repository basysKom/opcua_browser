import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

Item {
    property ThemeConnectionView theme: Style.connectionView

    ColumnLayout {
        id: layout

        readonly property int textColumnHeight: 15
        readonly property int columnHeight: 30

        anchors.centerIn: parent
        width: parent.width - 20

        Text {
            Layout.preferredHeight: layout.textColumnHeight
            verticalAlignment: Qt.AlignVCenter
            color: theme.textColor
            font.bold: true
            text: qsTr("Host")
        }

        TextField {
            id: hostUrl

            Layout.fillWidth: true
            Layout.preferredHeight: layout.columnHeight
            verticalAlignment: Qt.AlignVCenter
            text: "opc.tcp://localhost:43344"
            placeholderText: "opc.tcp://localhost:4080"
            enabled: !BackEnd.isConnected

            background: Rectangle {
                color: theme.textFieldBackground
            }

            onTextChanged: {
                BackEnd.clearServerList()
                BackEnd.clearEndpointList()
            }
        }

        Text {
            Layout.preferredHeight: layout.textColumnHeight
            verticalAlignment: Qt.AlignVCenter
            color: theme.textColor
            font.bold: true
            text: qsTr("Server")
            visible: serverListBox.visible
        }

        ComboBox {
            id: serverListBox

            Layout.fillWidth: true
            Layout.preferredHeight: layout.columnHeight
            palette.button: theme.comboBoxBackground
            model: BackEnd.serverList
            visible: model.length > 0
            enabled: !BackEnd.isConnected

            onCurrentIndexChanged: BackEnd.clearEndpointList()
        }

        Text {
            Layout.preferredHeight: layout.textColumnHeight
            verticalAlignment: Qt.AlignVCenter
            color: theme.textColor
            font.bold: true
            text: qsTr("Endpoint")
            visible: endpointListBox.visible
        }

        ComboBox {
            id: endpointListBox

            Layout.fillWidth: true
            Layout.preferredHeight: layout.columnHeight
            palette.button: theme.comboBoxBackground
            model: BackEnd.endpointList
            visible: model.length > 0
            enabled: !BackEnd.isConnected
        }

        RowLayout {
            Layout.fillWidth: true

            Rectangle {
                Layout.preferredWidth: 15
                Layout.preferredHeight: Layout.preferredWidth
                visible: (serverListBox.model.length > 0)
                         && (endpointListBox.model.length > 0)

                radius: Layout.preferredWidth / 2
                color: (2 === BackEnd.connectionState) ? theme.connected : (1 === BackEnd.connectionState) ? theme.connecting : theme.disconnected

                MouseArea {
                    anchors.fill: parent
                    onClicked: Style.currentThemeIndex
                               = ((Style.currentThemeIndex + 1) % Style.themes.length)
                }
            }

            StyledButton {
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                text: (serverListBox.model.length === 0) ? qsTr("Discover") : (endpointListBox.model.length === 0) ? qsTr("Browse") : (BackEnd.isConnected) ? qsTr("Disconnect") : qsTr("Connect")

                onClicked: {
                    if (serverListBox.model.length === 0) {
                        BackEnd.findServers(hostUrl.text)
                    } else if (endpointListBox.model.length === 0) {
                        BackEnd.getEndpoints(serverListBox.currentIndex)
                    } else if (BackEnd.isConnected) {
                        BackEnd.disconnectFromEndpoint()
                    } else {
                        BackEnd.connectToEndpoint(serverListBox.currentIndex)
                    }
                }
            }
        }
    }
}
