import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser
import Types

TabButton {
    id: control

    readonly property bool isCurrentTab: (TabBar.tabBar.currentIndex == TabBar.index)
                                         && (type !== DashboardType.Add)

    property ThemeIconTabButton theme: Style.iconTabButton

    property int type: DashboardType.Variables

    width: 80

    background: Rectangle {
        color: isCurrentTab ? theme.backgroundSelected : theme.background
    }

    contentItem: ColumnLayout {
        width: control.width
        spacing: 5

        IconImage {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            sourceSize.width: 40
            sourceSize.height: 40
            source: (DashboardType.Add === type) ? "qrc:/icons/plus.svg" : (DashboardType.Events === type) ? "qrc:/icons/event.svg" : "qrc:/icons/dashboard.svg"
            color: label.color
        }

        Text {
            id: label

            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 8
            text: control.text
            color: isCurrentTab ? theme.textColorSelected : theme.textColor
            elide: Text.ElideRight
        }
    }
}
