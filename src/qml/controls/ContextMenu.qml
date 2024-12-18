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

QQC.Popup {
    id: menu

    property string headerText
    property alias listModel: popupListView.model

    implicitWidth: contentItem.childrenRect.width
    implicitHeight: contentItem.childrenRect.height
    padding: 0

    modal: true

    property int lastImplicitHeight: -1;
    onImplicitHeightChanged: {
        if (lastImplicitHeight !== implicitHeight) {
            if (lastImplicitHeight >= 0) {
                y += (lastImplicitHeight - implicitHeight)
            }
            lastImplicitHeight = implicitHeight
        }
    }

    signal listItemClicked(int index)

    background: Rectangle {
        id: transparentBorderRect

        width: menu.width
        height: menu.height
        radius: 3
        color: palette.light
    }

    contentItem: Item {
        implicitWidth: popupListView.contentWidth
        implicitHeight: popupListView.contentHeight

        ColumnLayout {
            width: 200
            clip: true

            QQC.Label {
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: menu.headerText
                visible: menu.headerText != null && menu.headerText !== ""
                Layout.fillWidth: true
                leftPadding: 3
                font.bold: true
                font.pointSize: 12
            }

            ListView {
                id: popupListView

                implicitWidth: contentItem.childrenRect.width
                implicitHeight: contentItem.childrenRect.height
                clip: true

                delegate: Item {
                    id: delegateItem

                    height: 36
                    width: 200

                    required property int index
                    required property url imageSource
                    required property string name

                    readonly property bool isCurrentItem: popupListView.currentIndex === delegateItem.index

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true

                        onEntered: popupListView.currentIndex = delegateItem.index
                        onClicked: {
                            menu.listItemClicked(delegateItem.index)
                            menu.close()
                        }
                    }

                    Rectangle {
                        anchors.fill: parent
                        radius: transparentBorderRect.radius
                        color: palette.highlight
                        visible: delegateItem.isCurrentItem
                    }

                    RowLayout {
                        anchors.fill: parent
                        spacing: 15

                        IconImage {
                            Layout.leftMargin: 10
                            Layout.alignment: Qt.AlignVCenter
                            sourceSize.width: 24
                            sourceSize.height: 24
                            source: delegateItem.imageSource
                            color: delegateItem.isCurrentItem ? palette.highlightedText : palette.windowText
                        }

                        QQC.Label {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            font.pointSize: 12
                            color: delegateItem.isCurrentItem ? palette.highlightedText : palette.windowText
                            text: delegateItem.name
                        }
                    }
                }
            }
        }
    }
}
