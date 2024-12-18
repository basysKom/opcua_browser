/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls.impl // IconImage
import QtQuick.Layouts
import com.basyskom.quickcontrols as QQC

import OPC_UA_Browser

QQC.ApplicationWindow {
    id: appWindow

    palette: Colors.palette

    Connections {
        target: Qt.application
        function onStateChanged(inState) {
            if (inState === Qt.ApplicationSuspended) {
                BackEnd.applicationSuspended()
            }
        }
    }

    width: 350
    height: 640
    visible: true
    color: appWindow.palette.window
    title: "OPC UA Browser"
    header: Rectangle {
        id: headerItem

        property bool isSaveMode: false

        height: childrenRect.height
        color: appWindow.palette.window
        clip: true

        Behavior on height {
            PropertyAnimation {}
        }

        ColumnLayout {
            width: parent.width

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 48

                Item {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    width: leftImage.width + 20
                    height: Math.min(parent.height, leftImage.height + 20)

                    IconImage {
                        id: leftImage

                        readonly property alias showBackButton: contentView.showBackButtonInHeader

                        anchors.centerIn: parent
                        sourceSize.width: 24
                        sourceSize.height: 24
                        source: leftImage.showBackButton ? "qrc:/icons/back.svg" : "qrc:/icons/menu.svg"
                        color: appWindow.palette.windowText
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (leftImage.showBackButton) {
                                contentView.goBack()
                            } else {
                                sideMenu.open()
                            }
                        }
                    }
                }

                Image {
                    anchors.centerIn: parent
                    height: parent.height - 10
                    fillMode: Image.PreserveAspectFit
                    source: "qrc:/icons/logo_basyskom.svg"
                }

                IconImage {
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    sourceSize.width: 24
                    sourceSize.height: 24
                    source: "qrc:/icons/save.svg"
                    color: appWindow.palette.windowText
                    visible: contentView.canSaveDashboard
                             && !headerItem.isSaveMode

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: headerItem.isSaveMode = true
                    }
                }
            }

            LabelledTextField {
                id: name

                visible: headerItem.isSaveMode
                Layout.fillWidth: true
                Layout.preferredHeight: implicitHeight
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                captionText: qsTranslate("General", "Name")
                text: contentView.currentDashboardName
            }

            Item {
                visible: headerItem.isSaveMode
                Layout.fillWidth: true
                Layout.preferredHeight: childrenRect.height
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                Layout.topMargin: 5
                Layout.bottomMargin: 5

                QQC.Button {
                    anchors.left: parent.left
                    width: parent.width / 2 - 5
                    text: qsTranslate("General", "Cancel")

                    onClicked: {
                        name.text = contentView.currentDashboardName
                        headerItem.isSaveMode = false
                    }
                }

                QQC.Button {
                    anchors.right: parent.right
                    width: parent.width / 2 - 5
                    highlighted: true
                    text: qsTranslate("General", "Ok")
                    visible: name.text.length > 0

                    onClicked: {
                        BackEnd.saveCurrentDashboard(name.text)
                        headerItem.isSaveMode = false
                    }
                }
            }

            Rectangle {
                id: rect
                visible: headerItem.isSaveMode
                Layout.fillWidth: true
                Layout.preferredHeight: 2
                Layout.leftMargin: 5
                Layout.rightMargin: 5
                color: appWindow.palette.light
            }
        }
    }

    SideMenu {
        id: sideMenu

        y: -appWindow.header.height
        menuHeight: parent.height + appWindow.header.height
        menuWidth: Math.min(310, 0.8 * parent.width)

        onAddConnectionSelected: {
            // ToDo: add new connection
            BackEnd.disconnectFromEndpoint()
            BackEnd.clearServerList()
            BackEnd.clearEndpointList()
            contentView.showConnectionView()
        }

        onCloseConnectionSelected: {
            BackEnd.disconnectFromEndpoint()
            contentView.showConnectionView()
        }
        onShowDashboardsSelected: contentView.showDashboardView()
        onShowExpertModeSelected: contentView.showExpertBrowserView()
        onShowImprintSelected: contentView.showImprintView()
        onShowSettingsSelected: contentView.showSettingsView()
        onShowLogSelected: contentView.showLoggingView()
        onShowLicenseSelected: contentView.showLicenseView()
    }

    ContentView {
        id: contentView

        anchors.fill: parent
    }
}
