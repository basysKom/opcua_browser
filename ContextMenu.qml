import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: contextMenu

    property var currentTreeViewIndex
    property string currentNodeId
    property bool showMonitoringItem: false

    implicitWidth: contentItem.childrenRect.width
    implicitHeight: contentItem.childrenRect.height
    padding: 0

    modal: true

    background: Rectangle {
        id: transparentBorderRect

        width: contextMenu.width
        height: contextMenu.height
        radius: 3
        color: Style.contextMenu.background
    }

    contentItem: Item {
        implicitWidth: popupListView.contentWidth
        implicitHeight: popupListView.contentHeight

        ListModel {
            id: listModel

            ListElement {
                imageSource: "qrc:/icons/refresh.png"
                name: qsTr("Refresh")
            }
            ListElement {
                imageSource: "qrc:/icons/eye.png"
                name: qsTr("Monitor")
            }
        }

        ListView {
            id: popupListView

            implicitWidth: contentItem.childrenRect.width
            implicitHeight: contentItem.childrenRect.height
            model: listModel
            clip: true

            delegate: Item {
                height: visible ? 30 : 0
                width: 200
                // hide Monitor item, if monitoring is not supported for node class
                visible: contextMenu.showMonitoringItem || (1 !== index)

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true

                    onEntered: popupListView.currentIndex = index
                    onClicked: {
                        if (0 === model.index) {
                            // refresh pressed
                            BackEnd.opcUaModel.refreshIndex(
                                        contextMenu.currentTreeViewIndex)
                        } else if (1 === model.index) {
                            // monitor pressed
                            BackEnd.monitorNode(contextMenu.currentNodeId)
                        }

                        contextMenu.close()
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    radius: transparentBorderRect.radius
                    color: Style.contextMenu.backgroundSelected
                    opacity: popupListView.currentIndex === index ? 0.8 : 0
                }

                RowLayout {
                    anchors.fill: parent
                    spacing: 15

                    IconImage {
                        Layout.leftMargin: 10
                        Layout.alignment: Qt.AlignVCenter
                        width: 20
                        height: 20
                        source: model.imageSource
                    }
                    Text {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHLeft
                        color: Style.contextMenu.textColor
                        text: model.name
                    }
                }
            }
        }
    }
}
