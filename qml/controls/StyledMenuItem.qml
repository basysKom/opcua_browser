import QtQuick
import QtQuick.Controls

import OPC_UA_Browser

MenuItem {
    id: control

    property alias iconSource: control.icon.source
    property ThemeSideMenu theme: Style.sideMenu

    height: enabled ? implicitHeight : 0
    width: implicitWidth
    font.pointSize: 12
    palette.windowText: theme.textColor

    icon {
        color: theme.iconColor
        height: 20
        width: 20
    }

    background: Rectangle {
        color: theme.background
    }
}
