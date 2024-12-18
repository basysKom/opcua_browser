

/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
import QtQuick
import QtQuick.Layouts
import com.basyskom.quickcontrols as QQC

import OPC_UA_Browser

Item {
    id: view

    property string selectedHostUrl
    property alias selectedServerUrl: serverListBox.currentText
    property alias selectedEndpointUrl: endpointListBox.currentEndpointUrl

    function clearLoginMethod() {
        authenticationListBox.currentIndex = 0
        userName.text = ""
        password.text = ""
    }

    ColumnLayout {
        id: layout

        readonly property int textColumnHeight: 15
        readonly property int columnHeight: 30

        anchors.centerIn: parent
        width: parent.width - 20
        spacing: 20

        QQC.Label {
            Layout.fillWidth: true
            text: BackEnd.stateText
            color: palette.highlight
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        ColumnLayout {
            visible: hostUrl.visible && (recentConnections.model.length > 0)

            LabelledComboBox {
                id: recentConnections

                captionText: qsTranslate("Connection", "Recent connections")
                model: BackEnd.recentConnections

                // Reset the authentication when when changing servers
                _comboBox.onActivated: view.clearLoginMethod()
            }

            QQC.Button {
                Layout.fillWidth: true
                text: qsTranslate("Connection", "Discover")
                highlighted: true

                onClicked: {
                    view.selectedHostUrl = recentConnections.currentText
                    BackEnd.findServers(view.selectedHostUrl)
                }
            }

        }

        ColumnLayout {
            visible: serverListBox.model.length === 0

            LabelledTextField {
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

            QQC.Button {
                Layout.fillWidth: true
                text: qsTranslate("Connection", "Discover")
                enabled: hostUrl.acceptableInput
                highlighted: enabled

                onClicked: {
                    view.selectedHostUrl = hostUrl.prefixText + hostUrl.text
                    BackEnd.findServers(view.selectedHostUrl)
                    view.clearLoginMethod()
                }
            }

        }

        ColumnLayout {
            id: browseServerLayout

            visible: serverListBox.model.length > 0
                     && endpointListBox.model.length === 0

            LabelledComboBox {
                id: serverListBox

                captionText: qsTranslate("Connection", "Server")
                model: BackEnd.serverList

                onModelChanged: {
                    const urlIndex = serverListBox._comboBox.find(view.selectedHostUrl, Qt.MatchStartsWith)

                    if (urlIndex !== -1)
                        serverListBox.currentIndex = urlIndex
                }
            }

            QQC.Button {
                Layout.fillWidth: true
                text: qsTranslate("Connection", "Browse")
                highlighted: true

                onClicked: BackEnd.getEndpoints(serverListBox.currentIndex)
            }
        }

        ColumnLayout {
            id: connectToEndpointLayout
            visible: endpointListBox.model.length > 0

            LabelledEndpointComboBox {
                id: endpointListBox

                enabled: !BackEnd.isConnected
                captionText: qsTranslate("Connection", "Endpoint")
                model: BackEnd.endpointList
            }

            LabelledComboBox {
                id: authenticationListBox

                enabled: !BackEnd.isConnected
                captionText: qsTranslate("Connection", "Authentication")
                model: [qsTranslate("Connection", "Anonymous"),
                    qsTranslate("Connection", "Username") /*,
                    qsTranslate("Certificate", "Certificate")*/ ]
            }

            LabelledTextField {
                id: userName

                enabled: authenticationListBox.enabled
                visible: authenticationListBox.currentIndex === 1
                captionText: qsTranslate("Connection", "Username")
            }

            LabelledTextField {
                id: password

                enabled: authenticationListBox.enabled
                visible: authenticationListBox.currentIndex === 1
                captionText: qsTranslate("Connection", "Password")
                echoMode: TextInput.Password
            }

            LabelledTextField {
                id: certificate

                enabled: authenticationListBox.enabled
                visible: authenticationListBox.currentIndex === 2
                captionText: qsTranslate("Certificate", "Certificate")
            }

            LabelledTextField {
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
                    color: (2 === BackEnd.connectionState) ? Colors.green : (1 === BackEnd.connectionState) ? Colors.yellow : Colors.red
                }

                QQC.Button {
                    Layout.fillWidth: true
                    text: BackEnd.isConnected ? qsTranslate("Connection", "Disconnect") : qsTranslate("Connection", "Connect")
                    highlighted: true

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

    QQC.Button {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        visible: serverListBox.model.length > 0
        width: parent.width / 3
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
