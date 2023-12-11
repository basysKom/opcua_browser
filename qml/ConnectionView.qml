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

        StyledTextField {
            id: hostUrl

            enabled: !BackEnd.isConnected
            visible: serverListBox.model.length === 0
            captionText: qsTr("Host")
            text: "opc.tcp://localhost:43344"
            placeholderText: "opc.tcp://localhost:4080"
        }

        StyledComboBox {
            id: serverListBox

            enabled: !BackEnd.isConnected
            visible: model.length > 0 && endpointListBox.model.length === 0
            captionText: qsTr("Server")
            model: BackEnd.serverList
        }

        StyledComboBox {
            id: endpointListBox

            enabled: !BackEnd.isConnected
            visible: model.length > 0
            captionText: qsTr("Endpoint")
            model: BackEnd.endpointList
        }

        StyledComboBox {
            id: authenticationListBox

            enabled: !BackEnd.isConnected
            visible: endpointListBox.model.length > 0
            captionText: qsTr("Authentication")
            model: ["Anonymous", "Username", "Certificate"]
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

            Rectangle {
                Layout.preferredWidth: 15
                Layout.preferredHeight: Layout.preferredWidth
                visible: (serverListBox.model.length > 0)
                         && (endpointListBox.model.length > 0)

                radius: Layout.preferredWidth / 2
                color: (2 === BackEnd.connectionState) ? theme.connected : (1 === BackEnd.connectionState) ? theme.connecting : theme.disconnected
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

    StyledButton {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        visible: serverListBox.model.length > 0
        height: 35
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
