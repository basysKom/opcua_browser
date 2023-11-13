import QtQuick

Rectangle {
    id: root
    color: "#FFBC42"
    clip: true

    NodesView {
        id: nodesView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: nodeDetailView.top
    }

    NodeDetailView {
        id: nodeDetailView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        visible: BackEnd.isConnected
        height: 200

        attributes: nodesView.attributes
        references: nodesView.references
        monitoredAttributes: nodesView.monitoredAttributes
    }
}
