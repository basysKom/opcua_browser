/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import com.basyskom.quickcontrols as QQC

import OPC_UA_Browser

QQC.Menu {
    id: control

    property int menuHeight: implicitHeight
    property int menuWidth: implicitWidth

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
        color: palette.window
    }

    QQC.Label {
        topPadding: 10
        font.pointSize: 20
        font.bold: true
        text: "OPC UA Browser"
    }

    QQC.MenuSeparator {}

    QQC.MenuItem {
        enabled: BackEnd.isConnected
        text: qsTranslate("Connection", "Close connection")
        icon.source: "qrc:/icons/disconnect.svg"
        onTriggered: control.closeConnectionSelected()
    }


    /*QQC.MenuItem {
        enabled: BackEnd.isConnected
        text: qsTranslate("Connection", "Add connection")
        icon.source: "qrc:/icons/connect.svg"
        onTriggered: control.addConnectionSelected()
    }*/
    QQC.MenuItem {
        enabled: BackEnd.isConnected
        text: qsTranslate("ExpertMode", "Expert mode")
        icon.source: "qrc:/icons/expert.svg"
        onTriggered: control.showExpertModeSelected()
    }

    QQC.MenuItem {
        enabled: BackEnd.isConnected
        text: qsTranslate("Dashboard", "Dashboards")
        icon.source: "qrc:/icons/dashboard.svg"
        onTriggered: control.showDashboardsSelected()
    }

    QQC.MenuSeparator {
        enabled: BackEnd.isConnected
    }

    QQC.MenuItem {
        text: qsTranslate("General", "Settings")
        icon.source: "qrc:/icons/settings.svg"
        onTriggered: control.showSettingsSelected()
    }

    QQC.MenuItem {
        text: qsTranslate("Logging", "Log")
        icon.source: "qrc:/icons/log.svg"
        onTriggered: control.showLogSelected()
    }

    QQC.MenuItem {
        text: qsTranslate("Imprint", "Imprint")
        icon.source: "qrc:/icons/info.svg"
        onTriggered: control.showImprintSelected()
    }

    QQC.MenuItem {
        text: qsTranslate("License", "License")
        icon.source: "qrc:/icons/license.svg"
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
