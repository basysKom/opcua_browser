/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import com.basyskom.quickcontrols as QQC

import OPC_UA_Browser

Rectangle {
    id: root

    signal popupRequested(name: string, valueText: string)

    property alias attributes: attributeList.model

    function maxDelegateWidth() {
        var max = flickable.width
        for (var child in attributeList.contentItem.children) {
            max = Math.max(
                        max,
                        attributeList.contentItem.children[child].childrenRect.width)
        }
        return max
    }

    color: palette.window
    opacity: attributeList.model ? 1 : 0.3
    clip: true

    Flickable {
        id: flickable

        anchors.fill: parent
        clip: true

        contentWidth: attributeList.width
        contentHeight: attributeList.height

        boundsBehavior: Flickable.StopAtBounds
        QQC.ScrollBar.horizontal: QQC.ScrollBar {}

        ListView {
            id: attributeList

            property bool wasOvershooted: false

            height: flickable.height
            width: root.maxDelegateWidth()

            model: root.attributes
            //boundsBehavior: Flickable.StopAtBounds
            QQC.ScrollBar.vertical: QQC.ScrollBar {}

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

                width: root.maxDelegateWidth()
                implicitHeight: childrenRect.height
                color: palette.base

                TapHandler {
                    onTapped: function() { // qmllint disable signal-handler-parameters
                        root.popupRequested(listViewDelegate.attribute, listViewDelegate.value)
                    }
                }

                ColumnLayout {
                    spacing: 0

                    Rectangle {
                        Layout.preferredWidth: flickable.width
                        Layout.preferredHeight: 1
                        visible: listViewDelegate.index > 0
                        color: palette.mid
                    }

                    QQC.Label {
                        id: attributeText

                        Layout.topMargin: 5
                        Layout.leftMargin: 5
                        Layout.fillWidth: true
                        verticalAlignment: Qt.AlignVCenter
                        text: listViewDelegate.attribute
                        elide: Qt.ElideRight
                        font {
                            pointSize: 11
                            bold: true
                        }
                    }

                    QQC.Label {
                        id: valueLabel

                        Layout.leftMargin: 5
                        Layout.bottomMargin: 5
                        Layout.fillWidth: true
                        verticalAlignment: Qt.AlignVCenter
                        text: listViewDelegate.value
                    }
                }
            }
        }
    }

    QQC.BusyIndicator {
        anchors.centerIn: parent
        running: attributeList.verticalOvershoot < 0
    }
}
