

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
    property alias selectedServerUrl: serverListBox.currentText
    property alias selectedEndpointUrl: endpointListBox.currentEndpointUrl

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
            visible: hostUrl.visible && (recentConnections.model.length > 0)

            StyledComboBox {
                id: recentConnections

                captionText: qsTranslate("Connection", "Recent connections")
                model: BackEnd.recentConnections
            }

            StyledButton {
                Layout.fillWidth: true
                text: qsTranslate("Connection", "Discover")

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

                captionText: qsTranslate("Connection", "Host")
                //text: "opc.tcp://192.168.178.25:43344"
                //text: "opc.tcp://localhost:43344"
                //text: "opc.tcp://10.0.2.2:43344"
                placeholderText: "localhost:4840"
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                prefixText: "opc.tcp://"
                validator: RegularExpressionValidator {
                    regularExpression: /[^\/].*/
                }
            }

            StyledButton {
                Layout.fillWidth: true
                text: qsTranslate("Connection", "Discover")
                enabled: hostUrl.acceptableInput
                highlighted: enabled

                onClicked: {
                    view.selectedHostUrl = hostUrl.prefixText + hostUrl.text
                    BackEnd.findServers(view.selectedHostUrl)
                }
            }

        }

        ColumnLayout {
            id: browseServerLayout

            visible: serverListBox.model.length > 0
                     && endpointListBox.model.length === 0

            StyledComboBox {
                id: serverListBox

                captionText: qsTranslate("Connection", "Server")
                model: BackEnd.serverList
            }

            StyledButton {
                Layout.fillWidth: true
                text: qsTranslate("Connection", "Browse")

                onClicked: BackEnd.getEndpoints(serverListBox.currentIndex)
            }
        }

        ColumnLayout {
            id: connectToEndpointLayout
            visible: endpointListBox.model.length > 0

            StyledEndpointComboBox {
                id: endpointListBox

                enabled: !BackEnd.isConnected
                captionText: qsTranslate("Connection", "Endpoint")
                model: BackEnd.endpointList
            }

            StyledComboBox {
                id: authenticationListBox

                enabled: !BackEnd.isConnected
                captionText: qsTranslate("Connection", "Authentication")
                model: [qsTranslate("Connection", "Anonymous"),
                    qsTranslate("Connection", "Username") /*,
                    qsTranslate("Certificate", "Certificate")*/ ]
            }

            StyledTextField {
                id: userName

                enabled: authenticationListBox.enabled
                visible: authenticationListBox.currentIndex === 1
                captionText: qsTranslate("Connection", "Username")
            }

            StyledTextField {
                id: password

                enabled: authenticationListBox.enabled
                visible: authenticationListBox.currentIndex === 1
                captionText: qsTranslate("Connection", "Password")
                echoMode: TextInput.Password
            }

            StyledTextField {
                id: certificate

                enabled: authenticationListBox.enabled
                visible: authenticationListBox.currentIndex === 2
                captionText: qsTranslate("Certificate", "Certificate")
            }

            StyledTextField {
                id: privateKey

                enabled: authenticationListBox.enabled
                visible: authenticationListBox.currentIndex === 2
                captionText: qsTranslate("Connection", "Private key")
            }

            RowLayout {
                Layout.fillWidth: true

                Rectangle {
                    Layout.preferredWidth: 15
                    Layout.preferredHeight: Layout.preferredWidth

                    radius: Layout.preferredWidth / 2
                    color: (2 === BackEnd.connectionState) ? view.theme.connected : (1 === BackEnd.connectionState) ? view.theme.connecting : view.theme.disconnected
                }

                StyledButton {
                    Layout.fillWidth: true
                    text: BackEnd.isConnected ? qsTranslate("Connection", "Disconnect") : qsTranslate("Connection", "Connect")

                    onClicked: {
                        if (BackEnd.isConnected) {
                            BackEnd.disconnectFromEndpoint()
                        } else {
                            BackEnd.connectToEndpoint(
                                        endpointListBox.currentIndex,
                                        authenticationListBox.currentIndex === 1,
                                        userName.text, password.text)
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
        text: qsTranslate("General", "Back")

        onClicked: {
            if (endpointListBox.model.length === 0) {
                BackEnd.clearServerList()
            } else {
                BackEnd.clearEndpointList()
            }
        }
    }
}
