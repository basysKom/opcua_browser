import QtQuick

Rectangle {
    id: root
    color: "#FFBC42"

    NodesView {
        id: nodesView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: spacer.top
    }

    Item {
        id: spacer
        anchors.left: parent.left
        anchors.right: parent.right
        y: root.height - 200 - height
        height: 10

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.SplitVCursor
            drag.target: spacer
            drag.axis: Drag.YAxis
        }
    }

    NodeDetailView {
        id: nodeDetailView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: spacer.bottom
        anchors.bottom: parent.bottom
        visible: BackEnd.isConnected

        attributes: nodesView.attributes
        references: nodesView.references
    }
}
