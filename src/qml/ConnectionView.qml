

/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
import QtQuick
import QtQuick.Layouts

import OPC_UA_Browser

Item {
    id: view

    property ThemeConnectionView theme: Style.connectionView
    property string selectedHostUrl
    readonly property bool showUrlMismatchMessage: browseServerLayout.visible && BackEnd.showUrlMismatchMessage
    readonly property bool showEndpointReplacementMessage: connectToEndpointLayout.visible && BackEnd.showEndpointReplacementMessage

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
            visible: !view.showUrlMismatchMessage && !view.showEndpointReplacementMessage
        }

        ColumnLayout {
            visible: hostUrl.visible && (recentConnections.model.length > 0)

            StyledComboBox {
                id: recentConnections

                captionText: qsTr("Recent connections")
                model: BackEnd.recentConnections
            }

            StyledButton {
                Layout.fillWidth: true
                text: qsTr("Discover")

                onClicked: {
                    view.selectedHostUrl = recentConnections.currentText
                    BackEnd.findServers(view.selectedHostUrl)
                }
            }
        }

        ColumnLayout {
            visible: serverListBox.model.length === 0

            StyledTextField {
                id: hostUrl

                captionText: qsTr("Host")
                //text: "opc.tcp://192.168.178.25:43344"
                //text: "opc.tcp://localhost:43344"
                //text: "opc.tcp://10.0.2.2:43344"
                placeholderText: "opc.tcp://localhost:4080"
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            }

            StyledButton {
                Layout.fillWidth: true
                text: qsTr("Discover")

                onClicked: {
                    view.selectedHostUrl = hostUrl.text
                    BackEnd.findServers(view.selectedHostUrl)
                }
            }

        }

        ColumnLayout {
            id: browseServerLayout

            Text {
                Layout.fillWidth: true
                text: qsTr("GetEndpoints failed using the discovery URL %1 returned from FindServers. Do you want to try the URL %2 with the hostname to discover FindServers?")
                            .arg(serverListBox.currentText).arg(view.selectedHostUrl)
                color: Style.accent
                visible: view.showUrlMismatchMessage
                wrapMode: Text.WordWrap
            }

            RowLayout {
                Layout.fillWidth: true
                visible: view.showUrlMismatchMessage

                StyledButton {
                    Layout.preferredWidth: Math.min(view.width / 3, 200)
                    text: qsTr("No")

                    onClicked: BackEnd.hideUrlMismatchMessage()
                }

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    Layout.preferredWidth: Math.min(view.width / 3, 200)
                    text: qsTr("Yes")

                    onClicked: BackEnd.useHostUrlForEndpointRequest()
                }
            }

            visible: serverListBox.model.length > 0
                     && endpointListBox.model.length === 0

            StyledComboBox {
                id: serverListBox

                captionText: qsTr("Server")
                model: BackEnd.serverList
                visible: !view.showUrlMismatchMessage
            }

            StyledButton {
                Layout.fillWidth: true
                text: qsTr("Browse")
                visible: !view.showUrlMismatchMessage

                onClicked: BackEnd.getEndpoints(serverListBox.currentIndex)
            }
        }

        ColumnLayout {
            id: connectToEndpointLayout
            visible: endpointListBox.model.length > 0

            Text {
                Layout.fillWidth: true
                text: qsTr("Connection to endpoint failed using the URL %1 returned from GetEndpoints. Do you want to try the URL %2 with the hostname to discover FindServers?")
                            .arg(endpointListBox.currentEndpointUrl).arg(view.selectedHostUrl)
                color: Style.accent
                visible: view.showEndpointReplacementMessage
                wrapMode: Text.WordWrap
            }

            RowLayout {
                Layout.fillWidth: true
                visible: view.showEndpointReplacementMessage

                StyledButton {
                    Layout.preferredWidth: Math.min(view.width / 3, 200)
                    text: qsTr("No")

                    onClicked: BackEnd.hideEndpointReplacementMessage()
                }

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    Layout.preferredWidth: Math.min(view.width / 3, 200)
                    text: qsTr("Yes")

                    onClicked: {
                        if (authenticationListBox.currentIndex === 1) {
                            BackEnd.useHostUrlForEndpointConnectionWithPassword(
                                        userName.text, password.text)
                        } else {
                            BackEnd.useHostUrlForEndpointConnection()
                        }
                    }
                }
            }

            StyledEndpointComboBox {
                id: endpointListBox

                enabled: !BackEnd.isConnected
                visible: !view.showEndpointReplacementMessage
                captionText: qsTr("Endpoint")
                model: BackEnd.endpointList
            }

            StyledComboBox {
                id: authenticationListBox

                enabled: !BackEnd.isConnected
                visible: !view.showEndpointReplacementMessage
                captionText: qsTr("Authentication")
                model: ["Anonymous", "Username" /*, "Certificate"*/ ]
            }

            StyledTextField {
                id: userName

                enabled: authenticationListBox.enabled
                visible: authenticationListBox.currentIndex === 1 && !view.showEndpointReplacementMessage
                captionText: qsTr("Username")
            }

            StyledTextField {
                id: password

                enabled: authenticationListBox.enabled
                visible: authenticationListBox.currentIndex === 1 && !view.showEndpointReplacementMessage
                captionText: qsTr("Password")
                echoMode: TextInput.Password
            }

            StyledTextField {
                id: certificate

                enabled: authenticationListBox.enabled
                visible: authenticationListBox.currentIndex === 2 && !view.showEndpointReplacementMessage
                captionText: qsTr("Certificate")
            }

            StyledTextField {
                id: privateKey

                enabled: authenticationListBox.enabled
                visible: authenticationListBox.currentIndex === 2 && !view.showEndpointReplacementMessage
                captionText: qsTr("Private key")
            }

            RowLayout {
                Layout.fillWidth: true
                visible: !view.showEndpointReplacementMessage

                Rectangle {
                    Layout.preferredWidth: 15
                    Layout.preferredHeight: Layout.preferredWidth

                    radius: Layout.preferredWidth / 2
                    color: (2 === BackEnd.connectionState) ? view.theme.connected : (1 === BackEnd.connectionState) ? view.theme.connecting : view.theme.disconnected
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
    }

    StyledButton {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        visible: serverListBox.model.length > 0 && !view.showUrlMismatchMessage && !view.showEndpointReplacementMessage
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
