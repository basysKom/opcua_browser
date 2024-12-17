/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import com.basyskom.quickcontrols as QQC

import OPC_UA_Browser

Rectangle {
    id: view

    enum Type {
        ExpertMode,
        SelectVariable,
        SelectEvents,
        SelectEventFields
    }

    property int type: BrowserView.Type.ExpertMode

    color: palette.window

    signal selectionCancelled
    signal selectionAccepted

    NodesView {
        id: nodesView

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: (view.type === BrowserView.Type.ExpertMode) ? spacer.top : okButton.top
        anchors.bottomMargin: 10

        canSelectVariables: (view.type === BrowserView.Type.SelectVariable)
        canSelectEvents: (view.type === BrowserView.Type.SelectEvents)
        canSelectEventFields: (view.type === BrowserView.Type.SelectEventFields)

        onCanSelectEventsChanged: function () {
            if (canSelectEvents) {
                BackEnd.opcUaModel.setCurrentNodeId("ns=0;i=85") // Root -> Objects
                fullPopup.name = qsTranslate("Dashboard", "Select event source nodes")
                fullPopup.value = qsTranslate("Dashboard", "Select objects to create event monitored items for and then press OK")
                fullPopup.open()
            }
        }

        onCanSelectEventFieldsChanged: function () {
            if (canSelectEventFields) {
                BackEnd.opcUaModel.setCurrentNodeId("ns=0;i=2050") // BaseEventType -> Message
                fullPopup.name = qsTranslate("Dashboard", "Select event fields")
                fullPopup.value = qsTranslate("Dashboard", "Select event fields to retrieve for events and then press OK")
                fullPopup.open()
            }
        }
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

        onPopupRequested: function (name, valueText) {
            fullPopup.name = name
            fullPopup.value = valueText
            fullPopup.open()
        }
    }

    QQC.Button {
        id: cancelButton

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        width: parent.width / 3
        visible: (view.type !== BrowserView.Type.ExpertMode)
        highlighted: false
        text: qsTranslate("General", "Cancel")

        onClicked: view.selectionCancelled()
    }

    QQC.Button {
        id: okButton

        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        width: parent.width / 3
        visible: (view.type !== BrowserView.Type.ExpertMode) && BackEnd.opcUaModel.hasSelectedItems
        text: qsTranslate("General", "Ok")

        onClicked: view.selectionAccepted()
    }

    QQC.Popup {
        id: fullPopup
        modal: true

        property alias name: nameText.text
        property alias value: valueText.text

        implicitWidth: popupFlickable.width
        implicitHeight: popupFlickable.height
        padding: 0

        clip: true

        anchors.centerIn: browser

        background: Rectangle {
            radius: 3
            opacity: 0.8
            color: palette.highlight
        }

        Flickable {
            id: popupFlickable
            width: browser.width * 0.8
            height: Math.max(100, Math.min(browser.height * 0.8, contentHeight))

            contentWidth: width
            contentHeight: contentItem.childrenRect.height

            Column {
                Text {
                    width: browser.width * 0.8
                    id: nameText
                    padding: 3
                    font {
                        pointSize: 12
                        bold: true
                    }
                    color: view.palette.highlightedText
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }

                Text {
                    width: browser.width * 0.8
                    id: valueText
                    padding: 3
                    font {
                        pointSize: 10
                    }
                    color: view.palette.highlightedText
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: function() {
                    fullPopup.close()
                }
            }
        }
    }
}
