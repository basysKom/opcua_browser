/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import OPC_UA_Browser

Menu {
    id: control

    property int menuHeight: implicitHeight
    property int menuWidth: implicitWidth
    property ThemeSideMenu theme: Style.sideMenu

    signal addConnectionSelected
    signal closeConnectionSelected
    signal showDashboardsSelected
    signal showExpertModeSelected
    signal showLogSelected
    signal showImprintSelected
    signal showSettingsSelected
    signal showLicenseSelected

    modal: true
    leftPadding: 10
    rightPadding: 10
    bottomPadding: 10
    contentWidth: menuWidth
    contentHeight: menuHeight

    enter: Transition {
        NumberAnimation {
            properties: "width"
            from: 0
            to: control.menuWidth
        }
    }

    exit: Transition {
        NumberAnimation {
            properties: "width"
            from: control.menuWidth
            to: 0
        }
    }

    background: Rectangle {
        color: control.theme.background
    }

    Text {
        topPadding: 10
        font.pointSize: 20
        font.bold: true
        color: control.theme.textColor
        text: "OPC UA Browser"
    }

    StyledMenuSeparator {}

    StyledMenuItem {
        enabled: BackEnd.isConnected
        text: qsTr("Close connection")
        iconSource: "qrc:/icons/disconnect.svg"
        onTriggered: control.closeConnectionSelected()
    }


    /*StyledMenuItem {
        enabled: BackEnd.isConnected
        text: qsTr("Add connection")
        iconSource: "qrc:/icons/connect.svg"
        onTriggered: control.addConnectionSelected()
    }*/
    StyledMenuItem {
        enabled: BackEnd.isConnected
        text: qsTr("Expert mode")
        iconSource: "qrc:/icons/expert.svg"
        onTriggered: control.showExpertModeSelected()
    }

    StyledMenuItem {
        enabled: BackEnd.isConnected
        text: qsTr("Dashboards")
        iconSource: "qrc:/icons/dashboard.svg"
        onTriggered: control.showDashboardsSelected()
    }

    StyledMenuSeparator {
        enabled: BackEnd.isConnected
    }

    StyledMenuItem {
        text: qsTr("Settings")
        iconSource: "qrc:/icons/settings.svg"
        onTriggered: control.showSettingsSelected()
    }

    StyledMenuItem {
        text: qsTr("Log")
        iconSource: "qrc:/icons/log.svg"
        onTriggered: control.showLogSelected()
    }

    StyledMenuItem {
        text: qsTr("Imprint")
        iconSource: "qrc:/icons/info.svg"
        onTriggered: control.showImprintSelected()
    }

    StyledMenuItem {
        text: qsTr("License")
        iconSource: "qrc:/icons/license.svg"
        onTriggered: control.showLicenseSelected()
    }

    Item {
        width: control.menuWidth - control.leftPadding - control.rightPadding
        height: control.menuHeight - control.y - control.topPadding - control.bottomPadding

        Image {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 35
            fillMode: Image.PreserveAspectFit
            source: "qrc:/icons/logo_basyskom.svg"
        }
    }
}
