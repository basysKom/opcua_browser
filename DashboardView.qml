import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    color: Style.dashboard.background

    signal addMonitoredItems

    Component {
        id: dragDelegate

        MouseArea {
            id: dragArea

            property bool held: false
            required property string name
            required property string value
            required property bool isAddItem
            required property int index

            height: content.height
            width: content.width
            cursorShape: dragArea.isAddItem ? Qt.PointingHandCursor : Qt.ArrowCursor

            drag.target: held ? content : undefined

            onPressAndHold: held = !dragArea.isAddItem
            onReleased: held = false
            onClicked: {
                if (dragArea.isAddItem) {
                    addMonitoredItems()
                }
            }

            Rectangle {
                id: content

                Drag.active: dragArea.held
                Drag.source: dragArea
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2

                implicitWidth: 160
                implicitHeight: Math.max(70, childrenRect.height)
                radius: 5
                color: dragArea.held ? Style.dashboard.item.backgroundHeld : Style.dashboard.item.background

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
                    visible: !dragArea.isAddItem

                    RowLayout {
                        width: parent.width - 2 * parent.padding

                        Text {
                            Layout.fillWidth: true
                            color: Style.dashboard.item.textColor
                            text: dragArea.name
                            font {
                                pointSize: 10
                                bold: true
                            }
                            elide: Text.ElideRight
                        }

                        IconImage {
                            Layout.alignment: Qt.AlignVCenter
                            width: 20
                            height: 20
                            source: "qrc:/icons/delete.png"
                            color: Style.dashboard.item.textColor

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: BackEnd.monitoredItemModel.disableMonitoring(
                                               dragArea.index)
                            }
                        }
                    }

                    Text {
                        width: parent.width - 2 * parent.padding
                        font.pointSize: 8
                        color: Style.dashboard.item.textColor
                        text: dragArea.value
                        elide: Text.ElideRight
                        clip: true
                    }
                }

                IconImage {
                    anchors.centerIn: parent
                    width: 30
                    height: 30
                    visible: dragArea.isAddItem
                    source: "qrc:/icons/plus.png"
                    color: Style.dashboard.item.textColor
                }
            }

            DropArea {
                anchors.fill: parent
                anchors.margins: 10

                onEntered: drag => {
                               if (!dragArea.isAddItem) {
                                   visualModel.items.move(
                                       drag.source.DelegateModel.itemsIndex,
                                       dragArea.DelegateModel.itemsIndex)
                               }
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

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: tabBar.top
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

    TabBar {
        id: tabBar

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        spacing: 5

        background: Rectangle {
            color: "transparent"
        }

        Repeater {
            model: [qsTr("Dashboard 1"), qsTr("Dashboard 2"), qsTr(
                    "Events 1"), qsTr("Add")]
            StyledIconTabButton {
                type: (index < 2) ? StyledIconTabButton.Type.Item : (index === 2) ? StyledIconTabButton.Type.Event : StyledIconTabButton.Type.Add
                text: modelData
            }
        }
    }
}
