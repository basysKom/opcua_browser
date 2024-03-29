/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick

import OPC_UA_Browser

Rectangle {
    id: view

    enum Type {
        ExpertMode,
        SelectMonitoredItem,
        SelectEvents
    }

    property int type: BrowserView.Type.ExpertMode
    property ThemeBrowserView theme: Style.browserView

    color: theme.background

    signal selectionCancelled
    signal selectionAccepted

    NodesView {
        id: nodesView

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: (view.type === BrowserView.Type.ExpertMode) ? spacer.top : okButton.top
        anchors.bottomMargin: 10

        canSelectVariables: (view.type === BrowserView.Type.SelectMonitoredItem)
        canSelectEvents: (view.type === BrowserView.Type.SelectEvents)
    }

    Item {
        id: spacer

        anchors.left: parent.left
        anchors.right: parent.right
        y: view.height - 200 - height
        height: nodeDetailView.visible ? 10 : 0

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.SplitVCursor
            drag.target: spacer
            drag.axis: Drag.YAxis
        }
    }

    NodeDetailView {
        id: nodeDetailView

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: spacer.bottom
        anchors.bottom: parent.bottom
        visible: BackEnd.isConnected
                 && (view.type === BrowserView.Type.ExpertMode)

        attributes: nodesView.attributes
        references: nodesView.references
    }

    StyledButton {
        id: cancelButton

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        width: parent.width / 3
        visible: (view.type !== BrowserView.Type.ExpertMode)
        highlighted: false
        text: qsTr("Cancel")

        onClicked: view.selectionCancelled()
    }

    StyledButton {
        id: okButton

        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        width: parent.width / 3
        visible: (view.type !== BrowserView.Type.ExpertMode) && BackEnd.opcUaModel.hasSelectedItems
        text: qsTr("Ok")

        onClicked: view.selectionAccepted()
    }
}
