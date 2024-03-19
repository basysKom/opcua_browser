/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl // IconImage
import QtQuick.Layouts

import OPC_UA_Browser

Rectangle {
    id: root

    property alias references: referenceList.model
    property ThemeListView theme: Style.listView
    readonly property color textColor: theme.textColor

    function maxDelegateWidth() {
        var max = flickable.width
        for (var child in referenceList.contentItem.children) {
            max = Math.max(
                        max,
                        referenceList.contentItem.children[child].childrenRect.width)
        }
        return max
    }

    color: theme.background
    opacity: referenceList.model ? 1 : 0.3
    clip: true

    Flickable {
        id: flickable

        anchors.fill: parent
        clip: true

        contentWidth: referenceList.width
        contentHeight: referenceList.height

        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.horizontal: StyledScrollBar {}

        ListView {
            id: referenceList

            height: flickable.height
            width: root.maxDelegateWidth()

            //boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: StyledScrollBar {}

            headerPositioning: ListView.OverlayHeader

            header: Rectangle {
                id: headerItem

                width: referenceList.width
                implicitHeight: childrenRect.height
                z: 2

                color: root.theme.headerBackground

                RowLayout {
                    spacing: 0
                    height: 30

                    Text {
                        Layout.margins: 5
                        Layout.fillHeight: true
                        Layout.preferredWidth: root.width / 3 + 20
                        verticalAlignment: Qt.AlignVCenter
                        text: qsTr("Reference")
                        elide: Qt.ElideRight
                        font.bold: true
                        color: root.textColor
                    }

                    Rectangle {
                        Layout.fillHeight: true
                        Layout.preferredWidth: 1
                        color: root.textColor
                    }

                    Text {
                        Layout.margins: 5
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        verticalAlignment: Qt.AlignVCenter
                        text: qsTr("Target")
                        elide: Qt.ElideRight
                        font.bold: true
                        color: root.textColor
                    }
                }
            }

            delegate: Rectangle {
                id: listViewDelegate

                required property int index
                required property string type
                required property string typeNodeId
                required property string target
                required property string targetNodeId

                readonly property real padding: 5

                width: root.maxDelegateWidth()
                implicitHeight: childrenRect.height
                color: ((listViewDelegate.index % 2) == 0) ? root.theme.color1 : root.theme.color2

                RowLayout {
                    spacing: 0
                    height: 30

                    IconImage {
                        Layout.leftMargin: 5
                        Layout.alignment: Qt.AlignVCenter
                        sourceSize.width: 15
                        sourceSize.height: 15
                        source: referenceList.model.isForward ? "qrc:/icons/forward.svg" : "qrc:/icons/inverse.svg"
                        color: root.textColor
                    }

                    Text {
                        Layout.margins: 5
                        Layout.fillHeight: true
                        Layout.preferredWidth: root.width / 3
                        verticalAlignment: Qt.AlignVCenter
                        text: listViewDelegate.type
                        elide: Qt.ElideRight
                        color: root.textColor

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: BackEnd.opcUaModel.setCurrentNodeId(listViewDelegate.typeNodeId)
                        }
                    }

                    Rectangle {
                        Layout.fillHeight: true
                        Layout.preferredWidth: 1
                        color: root.textColor
                    }

                    Text {
                        Layout.margins: 5
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.minimumWidth: root.width - x
                        verticalAlignment: Qt.AlignVCenter
                        text: listViewDelegate.target
                        color: root.textColor

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: BackEnd.opcUaModel.setCurrentNodeId(listViewDelegate.targetNodeId)
                        }
                    }
                }
            }
        }
    }
}
