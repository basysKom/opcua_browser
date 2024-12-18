/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.impl // IconImage
import QtQuick.Layouts
import com.basyskom.quickcontrols as QQC

Column {
    id: root

    property alias leftContentMargin: contentRect.leftContentMargin
    property alias title: title.text
    property int titlePointSize: 14
    property alias listView: list

    signal editItemClicked(name: string)
    signal deleteItemClicked(name: string)

    spacing: 5

    QQC.Label {
        id: title

        font {
            pointSize: root.titlePointSize
            bold: true
        }
    }

    Rectangle {
        id: contentRect

        property int leftContentMargin: 10

        anchors.leftMargin: contentRect.leftContentMargin
        anchors.left: parent.left
        anchors.right: parent.right

        height: childrenRect.height

        color: palette.light
        radius: 5

        ListView {
            id: list

            width: parent.width
            height: Math.min(200, contentHeight)

            clip: true

            boundsBehavior: Flickable.StopAtBounds
            boundsMovement: Flickable.StopAtBounds

            QQC.ScrollBar.vertical: QQC.ScrollBar {
                policy: QQC.ScrollBar.AsNeeded
            }

            delegate: Rectangle {
                id: listViewDelegate

                required property int index
                required property string display

                radius: 5
                width: list.width
                implicitHeight: childrenRect.height
                color: palette.light

                RowLayout {
                    width: parent.width
                    height: 30
                    spacing: 10

                    QQC.Label {
                        Layout.fillWidth: true
                        Layout.rightMargin: 5
                        Layout.leftMargin: 5
                        font {
                            pointSize: 11
                        }
                        text: listViewDelegate.display
                        elide: Text.ElideRight
                    }

                    IconImage {
                        Layout.alignment: Qt.AlignVCenter
                        sourceSize.width: 24
                        sourceSize.height: 24
                        source: "qrc:/icons/edit.svg"
                        color: palette.windowText

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.editItemClicked(listViewDelegate.display)
                        }
                    }

                    IconImage {
                        Layout.alignment: Qt.AlignVCenter
                        Layout.rightMargin: 10
                        sourceSize.width: 24
                        sourceSize.height: 24
                        source: "qrc:/icons/delete.svg"
                        color: palette.windowText

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.deleteItemClicked(listViewDelegate.display)
                        }
                    }
                }
            }
        }
    }
}
