import QtQuick
import QtQuick.Controls

TabButton {
    readonly property bool isCurrentTab: (TabBar.tabBar.currentIndex == TabBar.index)

    width: Math.max(100, implicitWidth)

    background: Rectangle {
        color: "transparent"

        Rectangle {
            id: roundedRect
            anchors.fill: parent
            color: isCurrentTab ? "white" : "dimgray"
            radius: 10
        }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: roundedRect.radius
            color: isCurrentTab ? "white" : "dimgray"
        }
    }
}
