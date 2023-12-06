import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

TabButton {
    id: root

    readonly property bool isCurrentTab: (TabBar.tabBar.currentIndex == TabBar.index)

    property ThemeIconTabButton theme: Style.iconTabButton

    enum Type {
        Item,
        Event,
        Add
    }

    property int type: Type.Item

    width: 60

    background: Rectangle {
        color: isCurrentTab ? theme.backgroundSelected : theme.background
    }

    contentItem: ColumnLayout {
        width: root.width
        spacing: 0

        IconImage {
            Layout.alignment: Qt.AlignCenter
            width: 30
            height: 30
            source: (StyledIconTabButton.Type.Add === type) ? "qrc:/icons/plus.png" : (StyledIconTabButton.Type.Event === type) ? "qrc:/icons/event.png" : "qrc:/icons/dashboard.png"
            color: label.color
        }

        Text {
            id: label

            Layout.preferredWidth: root.width
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 6
            text: root.text
            color: isCurrentTab ? theme.textColorSelected : theme.textColor
            elide: Text.ElideRight
        }
    }
}
