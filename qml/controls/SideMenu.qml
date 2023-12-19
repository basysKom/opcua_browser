import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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
    signal showImprintSelected
    signal showSettingsSelected

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
        color: theme.background
    }

    Text {
        topPadding: 10
        font.pointSize: 20
        font.bold: true
        color: theme.textColor
        text: "OPC UA Browser"
    }

    StyledMenuSeparator {}

    StyledMenuItem {
        enabled: BackEnd.isConnected
        text: qsTr("Close connection")
        iconSource: "qrc:/icons/disconnect.svg"
        onTriggered: closeConnectionSelected()
    }

    StyledMenuItem {
        enabled: BackEnd.isConnected
        text: qsTr("Add connection")
        iconSource: "qrc:/icons/connect.svg"
        onTriggered: addConnectionSelected()
    }

    StyledMenuItem {
        enabled: BackEnd.isConnected
        text: qsTr("Expert mode")
        iconSource: "qrc:/icons/expert.svg"
        onTriggered: showExpertModeSelected()
    }

    StyledMenuItem {
        enabled: BackEnd.isConnected
        text: qsTr("Dashboards")
        iconSource: "qrc:/icons/dashboard.svg"
        onTriggered: showDashboardsSelected()
    }

    StyledMenuSeparator {
        enabled: BackEnd.isConnected
    }

    StyledMenuItem {
        text: qsTr("Settings")
        iconSource: "qrc:/icons/settings.svg"
        onTriggered: showSettingsSelected()
    }

    StyledMenuItem {
        text: qsTr("Imprint")
        iconSource: "qrc:/icons/info.svg"
        onTriggered: showImprintSelected()
    }

    Item {
        width: control.menuWidth - leftPadding - rightPadding
        height: control.menuHeight - y - topPadding - bottomPadding

        Image {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 35
            fillMode: Image.PreserveAspectFit
            source: "qrc:/icons/logo_basyskom.svg"
        }
    }
}
