import QtQuick
import QtQuick.Controls

import OPC_UA_Browser

Item {
    id: root

    property var attributes
    property var references
    property bool canSelectVariables: false
    property bool canSelectEvents: false
    property ThemeNodesView theme: Style.nodesView

    Connections {
        target: BackEnd.opcUaModel
        function onCurrentIndexChanged(index) {
            treeView.expandToIndex(index)
            treeView.forceLayout()
            treeView.positionViewAtIndex(index, TableView.Visible)
        }
    }

    ContextMenu {
        id: contextMenu

        listModel: ListModel {
            ListElement {
                imageSource: "qrc:/icons/refresh.svg"
                name: qsTr("Refresh")
            }
        }

        onListItemClicked: function (index) {
            if (index === 0) {
                // refresh pressed
                BackEnd.opcUaModel.refreshCurrentIndex()
            }
        }
    }

    TreeView {
        id: treeView

        anchors.fill: parent
        clip: true

        model: BackEnd.opcUaModel

        ScrollBar.vertical: StyledScrollBar {}
        ScrollBar.horizontal: StyledScrollBar {}

        delegate: Rectangle {
            id: treeDelegate

            readonly property real isCurrentItem: model.isCurrentItem
            readonly property real indent: 20
            readonly property real padding: 5

            // Assigned to by TreeView:
            required property TreeView treeView
            required property bool isTreeNode
            required property bool expanded
            required property int hasChildren
            required property int depth

            onIsCurrentItemChanged: {
                if (isCurrentItem) {
                    root.attributes = model.attributes
                    root.references = model.references
                }
            }

            width: implicitWidth
            implicitWidth: Math.max(
                               root.width,
                               padding + label.x + label.implicitWidth + padding)
            implicitHeight: label.implicitHeight * 1.5
            color: isCurrentItem ? theme.backgroundSelected : "transparent"

            TapHandler {
                id: tapHandler

                onTapped: {
                    treeView.toggleExpanded(row)
                    BackEnd.opcUaModel.setCurrentIndex(treeView.index(row,
                                                                      column))
                }

                onLongPressed: {
                    BackEnd.opcUaModel.setCurrentIndex(treeView.index(row,
                                                                      column))

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

                    contextMenu.open()
                }
            }

            IconImage {
                id: indicator

                source: "qrc:/icons/arrow_right.svg"
                sourceSize.width: 20
                sourceSize.height: 20
                visible: treeDelegate.isTreeNode && treeDelegate.hasChildren
                x: treeDelegate.padding + (treeDelegate.depth * treeDelegate.indent)
                anchors.verticalCenter: label.verticalCenter
                rotation: treeDelegate.expanded ? 90 : 0
                color: label.color
            }

            Rectangle {
                id: nodeClassIndicator

                x: treeDelegate.padding
                   + (treeDelegate.isTreeNode ? (treeDelegate.depth + 1) * treeDelegate.indent : 0)
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
                color: model.isCurrentItem ? theme.textColorSelected : theme.textColor
            }

            StyledItemSelector {
                x: treeView.contentX + root.width - width - 15
                anchors.verticalCenter: treeDelegate.verticalCenter
                height: treeDelegate.height - 10
                width: height
                checkState: model.isSelected ? Qt.Checked : Qt.Unchecked
                visible: (canSelectVariables && model.canMonitoring)
                         || (canSelectEvents && model.hasEventNotifier)

                onToggled: model.isSelected = !model.isSelected
            }
        }
    }
}
