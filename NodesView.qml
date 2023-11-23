import QtQuick
import QtQuick.Controls

Item {
    id: root
    property var attributes
    property var references

    ContextMenu {
        id: contextMenu
    }

    TreeView {
        id: treeView
        anchors.fill: parent
        clip: true

        model: BackEnd.opcUaModel

        ScrollBar.vertical: StyledScrollBar { }
        ScrollBar.horizontal: StyledScrollBar { }

        delegate: Rectangle {
            id: treeDelegate

            width: implicitWidth
            implicitWidth: Math.max(root.width, padding + label.x + label.implicitWidth + padding)
            implicitHeight: label.implicitHeight * 1.5
            color: model.isSelected ? "#584b53" : "transparent"

            readonly property real indent: 20
            readonly property real padding: 5

            // Assigned to by TreeView:
            required property TreeView treeView
            required property bool isTreeNode
            required property bool expanded
            required property int hasChildren
            required property int depth

            function setCurrentIndexToModel(index) {
                root.attributes = model.attributes
                root.references = model.references
                BackEnd.opcUaModel.setCurrentIndex(index)
            }

            TapHandler {
                id: tapHandler
                onTapped: {
                    treeView.toggleExpanded(row)
                    setCurrentIndexToModel(treeView.index(row, column))
                }

                onLongPressed: {
                    var index = treeView.index(row, column);
                    setCurrentIndexToModel(index);

                    var xPos = tapHandler.point.position.x + treeDelegate.x - treeView.contentX
                    var yPos = tapHandler.point.position.y + treeDelegate.y - treeView.contentY
                    if (xPos + contextMenu.width > treeView.width) {
                        contextMenu.x = xPos - contextMenu.width
                    } else {
                        contextMenu.x = xPos
                    }

                    if (yPos + contextMenu.height > treeView.height) {
                        contextMenu.y = yPos - contextMenu.height
                    } else {
                        contextMenu.y = yPos
                    }

                    contextMenu.currentTreeViewIndex = index;
                    contextMenu.currentNodeId = model.nodeId;
                    contextMenu.showMonitoringItem = model.canMonitoring;
                    contextMenu.open();
                }
            }

            Text {
                id: indicator
                visible: treeDelegate.isTreeNode && treeDelegate.hasChildren
                x: treeDelegate.padding + (treeDelegate.depth * treeDelegate.indent)
                anchors.verticalCenter: label.verticalCenter
                text: "▸"
                rotation: treeDelegate.expanded ? 90 : 0
                color: model.isSelected ? "white" : "black"
            }

            Rectangle {
                id: nodeClassIndicator
                x: treeDelegate.padding + (treeDelegate.isTreeNode ? (treeDelegate.depth + 1) * treeDelegate.indent : 0)
                anchors.verticalCenter: label.verticalCenter
                width: height
                height: label.height / 2
                color: model.color
                border.width: 1
            }

            Text {
                id: label
                x: nodeClassIndicator.x + nodeClassIndicator.width + 5
                anchors.verticalCenter: parent.verticalCenter
                width: treeDelegate.width - treeDelegate.padding - x
                clip: true
                text: model.display
                color: model.isSelected ? "white" : "black"
            }
        }
    }
}
