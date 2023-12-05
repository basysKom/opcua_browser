import QtQuick
import QtQuick.Controls

TabButton {
    id: root

    readonly property bool isCurrentTab: (TabBar.tabBar.currentIndex == TabBar.index)

    background: Rectangle {
        color: Style.tabButton.background

        Rectangle {
            id: divider
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 4
            color: Style.tabButton.dividerColor
        }

        Rectangle {
            anchors.centerIn: divider
            width: divider.width / 2
            height: 4
            visible: isCurrentTab
            color: Style.tabButton.dividerColorSelected
        }
    }

    contentItem: Text {
        text: root.text
        font {
            pointSize: 11
            bold: true
            capitalization: Font.AllUppercase
        }
        color: Style.tabButton.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
