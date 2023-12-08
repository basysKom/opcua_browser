import QtQuick
import QtQuick.Controls

import OPC_UA_Browser

MenuSeparator {
    property ThemeSideMenu theme: Style.sideMenu

    height: enabled ? implicitHeight : 0

    contentItem: Rectangle {
        implicitHeight: 1
        color: theme.iconColor
    }
}
