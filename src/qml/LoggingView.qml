/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

Rectangle {
    id: view

    readonly property var typeString: [ qsTranslate("Logging", "Debug"), qsTranslate("Logging", "Warning"), qsTranslate("Logging", "Critical"), qsTranslate("Logging", "Fatal"), qsTranslate("Logging", "Info")]
    readonly property var typeColor: [ view.theme.debugColor, view.theme.warningColor, view.theme.criticalColor, view.theme.fatalColor, view.theme.infoColor]
    property ThemeLoggingView theme: Style.loggingView

    color: theme.background

    Settings {
        property alias loggingTypeMask: buttonRow.mask
    }

    ListView {
        id: buttonRow

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5
        spacing: 10
        height: contentItem.childrenRect.height + anchors.margins
        orientation: ListView.Horizontal

        model: [ 0, 4, 1, 2, 3]
        property int mask: 0xFE

        onMaskChanged: BackEnd.loggingViewModel.setMessageTypeFilterMask(mask)
        Component.onCompleted: BackEnd.loggingViewModel.setMessageTypeFilterMask(mask)

        delegate: StyledButton {
            id: button

            required property int modelData

            highlighted: ((buttonRow.mask & (1 << modelData)) > 0)
            width: Math.max(80, implicitWidth)
            radius: height / 2
            text: view.typeString[modelData]

            onClicked: {
                if (button.highlighted) {
                    buttonRow.mask -= (1 << modelData)
                } else {
                    buttonRow.mask |= (1 << modelData)
                }
            }
        }
    }

    ListView {
        id: logListView

        anchors.top: buttonRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        clip: true

        model: BackEnd.loggingViewModel
        ScrollBar.vertical: StyledScrollBar {}

        delegate: Rectangle {
            id: listViewDelegate

            required property int index
            required property int type
            required property string timestamp
            required property string category
            required property string message

            width: logListView.width
            implicitHeight: childrenRect.height
            color: view.typeColor[type]

            ColumnLayout {
                width: parent.width
                spacing: 0

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    visible: listViewDelegate.index > 0
                    color: view.theme.divider
                }

                Text {
                    Layout.topMargin: 5
                    Layout.leftMargin: 5
                    Layout.fillWidth: true
                    font.bold: true
                    text: listViewDelegate.timestamp + " " + view.typeString[listViewDelegate.type]
                    color: view.theme.textColor
                }

                Text {
                    Layout.leftMargin: 5
                    Layout.fillWidth: true
                    font.bold: true
                    visible: text.length > 0
                    text: listViewDelegate.category
                    color: view.theme.textColor
                }

                Text {
                    Layout.leftMargin: 5
                    Layout.bottomMargin: 5
                    Layout.fillWidth: true
                    text: listViewDelegate.message
                    wrapMode: Text.WordWrap
                    color: view.theme.textColor
                }
            }
        }
    }
}
