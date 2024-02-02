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

    property ThemeLoggingView theme: Style.loggingView

    color: theme.background

    ListView {
        id: listView
        anchors.fill: parent
        clip: true

        model: BackEnd.loggingViewModel
        ScrollBar.vertical: StyledScrollBar {}

        readonly property var typeString: [ qsTr("Debug"), qsTr("Warning"), qsTr("Critical"), qsTr("Fatal"), qsTr("Info")]
        readonly property var typeColor: [ view.theme.debugColor, view.theme.warningColor, view.theme.criticalColor, view.theme.fatalColor, view.theme.infoColor]

        delegate: Rectangle {
            id: listViewDelegate

            required property int index
            required property int type
            required property string timestamp
            required property string category
            required property string message

            width: listView.width
            implicitHeight: childrenRect.height
            color: listView.typeColor[type]

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
                    text: listViewDelegate.timestamp + " " + listView.typeString[listViewDelegate.type]
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
