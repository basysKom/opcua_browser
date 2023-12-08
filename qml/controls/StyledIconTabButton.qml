import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

TabButton {
    id: control

    readonly property bool isCurrentTab: (TabBar.tabBar.currentIndex == TabBar.index)
                                         && (type !== StyledIconTabButton.Type.Add)

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
        width: control.width
        spacing: 0

        IconImage {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            width: 30
            height: 30
            source: (StyledIconTabButton.Type.Add === type) ? "qrc:/icons/plus.png" : (StyledIconTabButton.Type.Event === type) ? "qrc:/icons/event.png" : "qrc:/icons/dashboard.png"
            color: label.color
        }

        Text {
            id: label

            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 6
            text: control.text
            color: isCurrentTab ? theme.textColorSelected : theme.textColor
            elide: Text.ElideRight
        }
    }
}
