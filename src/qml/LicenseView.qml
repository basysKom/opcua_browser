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

Rectangle {
    id: view

    property ThemeLicenseView theme: Style.licenseView
    readonly property int sideMargins: 10

    color: theme.background

    Flickable {
        anchors.fill: parent
        anchors.bottomMargin: 10
        clip: true

        contentWidth: view.width
        contentHeight: content.childrenRect.height

        boundsBehavior: Flickable.StopAtBounds
        boundsMovement: Flickable.StopAtBounds

        ScrollBar.vertical: StyledScrollBar {
            policy: ScrollBar.AsNeeded
        }


        Column {
            id: content

            width: view.width - 2 * view.sideMargins
            leftPadding: view.sideMargins
            rightPadding: view.sideMargins
            spacing: 10

            Column {
                width: parent.width
                spacing: 5

                Text {
                    width: parent.width
                    color: view.theme.textColor
                    font {
                        pointSize: 16
                        bold: true
                    }
                    wrapMode: Text.WordWrap
                    text: qsTr("Copyright")
                }

                Text {
                    width: parent.width
                    color: view.theme.textColor
                    font.pointSize: 12
                    wrapMode: Text.WordWrap
                    text: "Copyright © 2024 basysKom GmbH"
                }

                Text {
                    width: parent.width
                    color: view.theme.textColor
                    font {
                        pointSize: 16
                        bold: true
                    }
                    wrapMode: Text.WordWrap
                    text: qsTr("License")
                }

                Text {
                    width: parent.width
                    color: view.theme.textColor
                    font.pointSize: 12
                    wrapMode: Text.WordWrap
                    text: "This project is released under the GPLv3.0-or-later License."
                }

                Text {
                    width: parent.width
                    linkColor: view.theme.linkColor
                    font.pointSize: 12
                    text: ("<a href=\"https://spdx.org/licenses/%1.html\">%2</a>").arg("GPL-3.0-or-later").arg("GNU General Public License v3.0 or later")
                    onLinkActivated: (link) => Qt.openUrlExternally(link)
                }

                Item {
                    width: parent.width
                    height: 15
                }
            }

            Column {
                width: parent.width
                spacing: 5

                Text {
                    color: view.theme.textColor
                    font {
                        pointSize: 16
                        bold: true
                    }
                    text: qsTr("Libraries in use")
                }

                Repeater {
                    id: licenseListView
                    width: parent.width

                    model: ListModel {
                        id: licenseModel
                        ListElement {
                            name: "Qt"
                            copyright: "© The Qt Project"
                            licenseId: "LGPL-3.0-only"
                            licenseName: "GNU Lesser General Public License v3.0 only"
                            url: "https://www.qt.io/"
                        }
                        ListElement {
                            name: "OpenSSL"
                            copyright: "© 1998-2018 The OpenSSL Project"
                            licenseId: "OpenSSL"
                            licenseName: "OpenSSL License"
                            url: "https://www.openssl.org/"
                        }
                        ListElement {
                            name: "Material Design icons by Google"
                            copyright: "© 2023 Google LLC"
                            licenseId: "Apache-2.0"
                            licenseName: "Apache License 2.0"
                            url: "https://github.com/google/material-design-icons"
                        }
                        ListElement {
                            name: "Swansea Font"
                            copyright: "Copyright (C) 1993 by Roger White"
                            licenseId: "PDDL-1.0"
                            licenseName: "Open Data Commons Public Domain Dedication & License 1.0"
                            licenseId2: "GPL-2.0-or-later"
                            licenseName2: "GNU General Public License v2.0 or later"
                            licenseId3: "OFL-1.0"
                            licenseName3: "SIL Open Font License 1.0"
                        }

                    }

                    delegate: ColumnLayout {
                        id: licenseDelegate

                        component TextItem : Text {
                            Layout.fillWidth: true
                            color: view.theme.textColor
                            linkColor: view.theme.linkColor
                            font.pointSize: 12
                            wrapMode: Text.WordWrap
                        }

                        required property int index
                        required property string name
                        required property string copyright
                        required property string licenseId
                        required property string licenseName
                        required property string licenseId2
                        required property string licenseName2
                        required property string licenseId3
                        required property string licenseName3
                        required property string url

                        width: licenseListView.width

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.topMargin: 5
                            Layout.preferredHeight: 1
                            visible: licenseDelegate.index > 0
                            color: view.theme.textColor
                        }

                        TextItem {
                            font {
                                pointSize: 14
                                bold: false
                            }
                            text: licenseDelegate.name
                        }

                        TextItem {
                            text: licenseDelegate.copyright
                        }

                        TextItem {
                            text: ("<a href=\"https://spdx.org/licenses/%1.html\">%2</a>").arg(licenseDelegate.licenseId).arg(licenseDelegate.licenseName)
                            onLinkActivated: (link) => Qt.openUrlExternally(link)
                        }

                        TextItem {
                            visible: licenseDelegate.licenseId2.length > 0
                            text: ("<a href=\"https://spdx.org/licenses/%1.html\">%2</a>").arg(licenseDelegate.licenseId2).arg(licenseDelegate.licenseName2)
                            onLinkActivated: (link) => Qt.openUrlExternally(link)
                        }

                        TextItem {
                            visible: licenseDelegate.licenseId3.length > 0
                            text: ("<a href=\"https://spdx.org/licenses/%1.html\">%2</a>").arg(licenseDelegate.licenseId3).arg(licenseDelegate.licenseName3)
                            onLinkActivated: (link) => Qt.openUrlExternally(link)
                        }

                        TextItem {
                            visible: licenseDelegate.url.length > 0
                            text: visible ? ("<a href=\"%1\">%1</a>").arg(licenseDelegate.url) : ""
                            onLinkActivated: (link) => Qt.openUrlExternally(link)
                        }
                    }
                }
            }
        }
    }
}
