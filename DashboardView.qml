import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: "#FFBC42"

    Flickable {
        id: flowListView
        anchors.fill: parent
        clip: true

        contentWidth: parent.width;
        contentHeight: flow.childrenRect.height + flow.padding

        ScrollBar.vertical: StyledScrollBar { }

        Flow {
            id: flow
            padding: 10
            spacing: 10
            anchors.fill: parent

            Repeater {
                id: repeater
                model: BackEnd.monitoredItemModel

                Rectangle {
                    implicitWidth: 150
                    implicitHeight: childrenRect.height
                    radius: 5
                    border.width: 1
                    color: "orange"

                    Column {
                        width: parent.width
                        padding: 5
                        spacing: 5

                        RowLayout {
                            width: parent.width - 2 * parent.padding
                            Text {
                                Layout.fillWidth: true
                                text: model.name
                                font.bold: true
                                elide: Text.ElideRight
                            }
                            Image {
                                Layout.alignment: Qt.AlignVCenter
                                width: 20
                                height: 20
                                source: "qrc:/icons/delete.png"

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: BackEnd.monitoredItemModel.disableMonitoring(index)
                                }
                            }
                        }
                        Text {
                            width: parent.width - 2 * parent.padding
                            text: model.value
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }
    }
}
