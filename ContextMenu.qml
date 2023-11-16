import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: contextMenu
    modal: true

    property var currentTreeViewIndex
    property bool showMonitoringItem: false;

    implicitWidth: contentItem.childrenRect.width
    implicitHeight: contentItem.childrenRect.height
    padding: 0

    background: Rectangle {
        id: transparentBorderRect
        width: contextMenu.width
        height: contextMenu.height
        radius: 3
        color: "orange"
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
                            BackEnd.opcUaModel.refreshIndex(contextMenu.currentTreeViewIndex)
                        } else if (1 === model.index) {
                            // monitor pressed
                            BackEnd.opcUaModel.monitorIndex(contextMenu.currentTreeViewIndex)
                        }

                        contextMenu.close()
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    radius: 3
                    color: "lightgray"
                    opacity: popupListView.currentIndex === index ? 0.5 : 0
                }

                RowLayout {
                    anchors.fill: parent
                    spacing: 15

                    Image {
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
                        text: model.name;
                    }
                }
            }
        }
    }
}
