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
    id: root

    property alias attributes: attributeList.model
    property ThemeListView theme: Style.listView

    function maxDelegateWidth() {
        var max = flickable.width
        for (var child in attributeList.contentItem.children) {
            max = Math.max(
                        max,
                        attributeList.contentItem.children[child].childrenRect.width)
        }
        return max
    }

    color: theme.background
    opacity: attributeList.model ? 1 : 0.3
    clip: true

    Flickable {
        id: flickable

        anchors.fill: parent
        clip: true

        contentWidth: attributeList.width
        contentHeight: attributeList.height

        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.horizontal: StyledScrollBar {}

        ListView {
            id: attributeList

            property bool wasOvershooted: false

            height: flickable.height
            width: root.maxDelegateWidth()

            model: root.attributes
            //boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: StyledScrollBar {}

            onVerticalOvershootChanged: {
                // update attribute list on vertical overshoot
                if (wasOvershooted && (verticalOvershoot >= 0)) {
                    BackEnd.opcUaModel.refreshAttributesForCurrentIndex()
                }
                wasOvershooted = (verticalOvershoot < 0)
            }

            delegate: Rectangle {
                id: listViewDelegate

                required property int index
                required property string attribute
                required property string value

                readonly property real padding: 5

                width: root.maxDelegateWidth()
                implicitHeight: childrenRect.height
                color: ((listViewDelegate.index % 1) == 0) ? root.theme.color1 : root.theme.color2

                ColumnLayout {
                    spacing: 0

                    Rectangle {
                        Layout.preferredWidth: flickable.width
                        Layout.preferredHeight: 1
                        visible: listViewDelegate.index > 0
                        color: root.theme.divider
                    }

                    Text {
                        id: attributeText

                        Layout.topMargin: 5
                        Layout.leftMargin: 5
                        Layout.fillWidth: true
                        verticalAlignment: Qt.AlignVCenter
                        text: listViewDelegate.attribute
                        elide: Qt.ElideRight
                        color: root.theme.textColor
                        font {
                            pointSize: 11
                            bold: true
                        }
                    }

                    Text {
                        id: valueLabel

                        Layout.leftMargin: 5
                        Layout.bottomMargin: 5
                        Layout.fillWidth: true
                        verticalAlignment: Qt.AlignVCenter
                        text: listViewDelegate.value
                        color: attributeText.color
                    }
                }
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: attributeList.verticalOvershoot < 0
    }
}
