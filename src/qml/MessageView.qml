/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

Flickable {
    id: view

    anchors.bottomMargin: 10
    clip: true

    contentWidth: wrapper.width
    contentHeight: wrapper.height

    boundsBehavior: Flickable.StopAtBounds
    boundsMovement: Flickable.StopAtBounds

    ScrollBar.vertical: StyledScrollBar {
        policy: ScrollBar.AsNeeded
    }

    property ThemeMessageView theme: Style.messageView
    property string selectedHostUrl
    property string selectedServerUrl
    property string selectedEndpointUrl

    function getMessageOkText() {
        switch (BackEnd.messageType) {
        case BackEnd.MessageType.UrlMismatch:
        case BackEnd.MessageType.EndpointReplacement:
            return qsTr("Yes")
        case BackEnd.MessageType.TrustCertificate:
            return qsTr("Trust")
        default:
            break
        }

        return qsTr("Ok")
    }

    function getMessageCancelText() {
        switch (BackEnd.messageType) {
        case BackEnd.MessageType.UrlMismatch:
        case BackEnd.MessageType.EndpointReplacement:
            return qsTr("No")
        default:
            break
        }

        return qsTr("Cancel")
    }

    Item
    {
        id: wrapper
        width: view.width
        height: Math.max(view.height, content.height + 20)

        Column {
            id: content

            anchors.centerIn: parent
            width: view.width - 20
            spacing: 20

            Text {
                width: parent.width
                text: qsTr("GetEndpoints failed using the discovery URL %1 returned from FindServers. Do you want to try the URL %2 with the hostname to discover FindServers?")
                            .arg(view.selectedServerUrl).arg(view.selectedHostUrl)
                color: view.theme.textColor
                visible: BackEnd.messageType === BackEnd.MessageType.UrlMismatch
                wrapMode: Text.WordWrap
            }

            Text {
                width: parent.width
                text: qsTr("Connection to endpoint failed using the URL %1 returned from GetEndpoints. Do you want to try the URL %2 with the hostname to discover FindServers?")
                            .arg(view.selectedEndpointUrl).arg(view.selectedHostUrl)
                color: view.theme.textColor
                visible: BackEnd.messageType === BackEnd.MessageType.EndpointReplacement
                wrapMode: Text.WordWrap
            }

            ColumnLayout {
                width: parent.width
                spacing: 5
                visible: BackEnd.messageType === BackEnd.MessageType.TrustCertificate

                component CertificateTextItem : ColumnLayout {
                    Layout.leftMargin: 5
                    Layout.fillWidth: true
                    spacing: 0

                    property alias title: title.text
                    property alias value: value.text
                    property alias valueWrapMode: value.wrapMode

                    Text {
                        id: title
                        Layout.fillWidth: true
                        verticalAlignment: Qt.AlignVCenter
                        color: view.theme.textColor
                        elide: Qt.ElideRight
                        font {
                            pointSize: 11
                            bold: true
                        }
                    }

                    Text {
                        id: value
                        Layout.leftMargin: 10
                        Layout.fillWidth: true
                        verticalAlignment: Qt.AlignVCenter
                        color: view.theme.textColor
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    Layout.fillWidth: true
                    text: qsTr("The server certificate is unknown.")
                    color: Style.accent
                    wrapMode: Text.WordWrap
                }

                CertificateTextItem {
                    title: qsTr("Common name")
                    value: BackEnd.certificateInfo.commonName
                }

                CertificateTextItem {
                    title: qsTr("Organization")
                    value: BackEnd.certificateInfo.organization
                    visible: value.length > 0
                }

                CertificateTextItem {
                    title: qsTr("Organization unit")
                    value: BackEnd.certificateInfo.organizationUnit
                    visible: value.length > 0
                }

                CertificateTextItem {
                    title: qsTr("Locality")
                    value: BackEnd.certificateInfo.locality
                    visible: value.length > 0
                }

                CertificateTextItem {
                    title: qsTr("State")
                    value: BackEnd.certificateInfo.state
                    visible: value.length > 0
                }

                CertificateTextItem {
                    title: qsTr("Country")
                    value: BackEnd.certificateInfo.country
                    visible: value.length > 0
                }

                CertificateTextItem {
                    title: qsTr("Valid from")
                    value: BackEnd.certificateInfo.effectiveDate
                }

                CertificateTextItem {
                    title: qsTr("Valid to")
                    value: BackEnd.certificateInfo.expiryDate
                }

                CertificateTextItem {
                    title: qsTr("Fingerprint (SHA-256)")
                    value: BackEnd.certificateInfo.fingerprint
                }

                CertificateTextItem {
                    title: qsTr("Serial number")
                    value: BackEnd.certificateInfo.serialNumber
                }

                Text {
                    Layout.fillWidth: true
                    Layout.topMargin: 20
                    text: qsTr("Do you want to trust the server certificate?")
                    color: Style.accent
                    wrapMode: Text.WordWrap
                }
            }

            RowLayout {
                width: parent.width

                StyledButton {
                    Layout.preferredWidth: Math.min(view.width / 3, 200)
                    text: view.getMessageCancelText()

                    onClicked: BackEnd.hideMessage()
                }

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    Layout.preferredWidth: Math.min(view.width / 3, 200)
                    text: view.getMessageOkText()

                    onClicked: {
                        switch (BackEnd.messageType) {
                        case BackEnd.MessageType.UrlMismatch:
                            BackEnd.useHostUrlForEndpointRequest()
                            break;
                        case BackEnd.MessageType.EndpointReplacement:
                            BackEnd.useHostUrlForEndpointConnection()
                            break;
                        case BackEnd.MessageType.TrustCertificate:
                            BackEnd.trustCertificate()
                            break;
                        default:
                            console.error("Message type is unknown:", BackEnd.messageType)
                            break;
                        }
                    }
                }
            }
        }
    }
}
