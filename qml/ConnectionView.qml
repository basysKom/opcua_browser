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
        spacing: 20

        Text {
            Layout.fillWidth: true
            text: BackEnd.stateText
            color: Style.accent
        }

        ColumnLayout {
            visible: hostUrl.visible && (recentConnections.model !== undefined)
                     && (recentConnections.model.rowCount() > 0)

            StyledComboBox {
                id: recentConnections

                captionText: qsTr("Recent connections")
                model: BackEnd.recentConnections
                textRole: "display"
            }

            StyledButton {
                Layout.fillWidth: true
                text: qsTr("Connect")
            }
        }

        ColumnLayout {
            visible: serverListBox.model.length === 0

            StyledTextField {
                id: hostUrl

                captionText: qsTr("Host")
                //text: "opc.tcp://192.168.178.25:43344"
                text: "opc.tcp://localhost:43344"
                placeholderText: "opc.tcp://localhost:4080"
            }

            StyledButton {
                Layout.fillWidth: true
                text: qsTr("Discover")

                onClicked: BackEnd.findServers(hostUrl.text)
            }
        }

        ColumnLayout {
            visible: serverListBox.model.length > 0
                     && endpointListBox.model.length === 0

            StyledComboBox {
                id: serverListBox

                captionText: qsTr("Server")
                model: BackEnd.serverList
            }

            StyledButton {
                Layout.fillWidth: true
                text: qsTr("Browse")

                onClicked: BackEnd.getEndpoints(serverListBox.currentIndex)
            }
        }

        StyledEndpointComboBox {
            id: endpointListBox

            enabled: !BackEnd.isConnected
            visible: endpointListBox.model.length > 0
            captionText: qsTr("Endpoint")
            model: BackEnd.endpointList
        }

        StyledComboBox {
            id: authenticationListBox

            enabled: !BackEnd.isConnected
            visible: endpointListBox.visible
            captionText: qsTr("Authentication")
            model: ["Anonymous", "Username" /*, "Certificate"*/
            ]
        }

        StyledTextField {
            id: userName

            enabled: authenticationListBox.enabled
            visible: authenticationListBox.visible
                     && authenticationListBox.currentIndex === 1
            captionText: qsTr("Username")
        }

        StyledTextField {
            id: password

            enabled: authenticationListBox.enabled
            visible: authenticationListBox.visible
                     && authenticationListBox.currentIndex === 1
            captionText: qsTr("Password")
            echoMode: TextInput.Password
        }

        StyledTextField {
            id: certificate

            enabled: authenticationListBox.enabled
            visible: authenticationListBox.visible
                     && authenticationListBox.currentIndex === 2
            captionText: qsTr("Certificate")
        }

        StyledTextField {
            id: privateKey

            enabled: authenticationListBox.enabled
            visible: authenticationListBox.visible
                     && authenticationListBox.currentIndex === 2
            captionText: qsTr("Private key")
        }

        RowLayout {
            Layout.fillWidth: true
            visible: endpointListBox.visible

            Rectangle {
                Layout.preferredWidth: 15
                Layout.preferredHeight: Layout.preferredWidth

                radius: Layout.preferredWidth / 2
                color: (2 === BackEnd.connectionState) ? theme.connected : (1 === BackEnd.connectionState) ? theme.connecting : theme.disconnected
            }

            StyledButton {
                Layout.fillWidth: true
                text: BackEnd.isConnected ? qsTr("Disconnect") : qsTr("Connect")

                onClicked: {
                    if (BackEnd.isConnected) {
                        BackEnd.disconnectFromEndpoint()
                    } else {
                        if (authenticationListBox.currentIndex === 1) {
                            BackEnd.connectToEndpointWithPassword(
                                        endpointListBox.currentIndex,
                                        userName.text, password.text)
                        } else {
                            BackEnd.connectToEndpoint(
                                        endpointListBox.currentIndex)
                        }
                    }
                }
            }
        }
    }

    StyledButton {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        visible: serverListBox.model.length > 0
        width: parent.width / 3
        highlighted: false
        text: qsTr("Back")

        onClicked: {
            if (endpointListBox.model.length === 0) {
                BackEnd.clearServerList()
            } else {
                BackEnd.clearEndpointList()
            }
        }
    }
}
