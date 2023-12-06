import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

TabButton {
    id: root

    readonly property bool isCurrentTab: (TabBar.tabBar.currentIndex == TabBar.index)

    enum Type {
        Item,
        Event,
        Add
    }

    property int type: Type.Item

    width: 60

    background: Rectangle {
        color: isCurrentTab ? Style.iconTabButton.backgroundSelected : Style.iconTabButton.background
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
            color: isCurrentTab ? Style.iconTabButton.textColorSelected : Style.iconTabButton.textColor
            elide: Text.ElideRight
        }
    }
}
