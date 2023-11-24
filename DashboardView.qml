import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    color: "#FFBC42"

    Component {
        id: dragDelegate

        MouseArea {
            id: dragArea

            property bool held: false
            required property string name
            required property string value
            required property int index

            height: content.height
            width: content.width

            drag.target: held ? content : undefined

            onPressAndHold: held = true
            onReleased: held = false

            Rectangle {
                id: content

                Drag.active: dragArea.held
                Drag.source: dragArea
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2

                implicitWidth: 150
                implicitHeight: childrenRect.height
                radius: 5
                border.width: 1
                color: dragArea.held ? "lightsteelblue" : "orange"

                Behavior on color {
                    ColorAnimation {
                        duration: 100
                    }
                }

                states: State {
                    when: dragArea.held

                    ParentChange {
                        target: content
                        parent: root
                    }
                    AnchorChanges {
                        target: content
                        anchors {
                            horizontalCenter: undefined
                            verticalCenter: undefined
                        }
                    }
                }

                Column {
                    width: parent.width
                    padding: 5
                    spacing: 5

                    RowLayout {
                        width: parent.width - 2 * parent.padding

                        Text {
                            Layout.fillWidth: true
                            text: dragArea.name
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
                                onClicked: BackEnd.monitoredItemModel.disableMonitoring(
                                               dragArea.index)
                            }
                        }
                    }

                    Text {
                        width: parent.width - 2 * parent.padding
                        text: dragArea.value
                        elide: Text.ElideRight
                        clip: true
                    }
                }
            }

            DropArea {
                anchors.fill: parent
                anchors.margins: 10

                onEntered: drag => {
                               visualModel.items.move(
                                   drag.source.DelegateModel.itemsIndex,
                                   dragArea.DelegateModel.itemsIndex)
                           }
            }
        }
    }

    DelegateModel {
        id: visualModel

        model: BackEnd.monitoredItemModel
        delegate: dragDelegate
    }

    Flickable {
        id: flowListView

        anchors.fill: parent
        clip: true

        contentWidth: parent.width
        contentHeight: flow.childrenRect.height + flow.padding

        ScrollBar.vertical: StyledScrollBar {}

        Flow {
            id: flow

            padding: 10
            spacing: 10
            anchors.fill: parent

            Repeater {
                id: repeater

                model: visualModel
            }
        }
    }
}
