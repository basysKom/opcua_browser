import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser
import Types

Rectangle {
    id: view

    readonly property bool canSaveDashboard: repeater.count > 1
    readonly property string currentDashboardName: (tabBar.currentItem
                                                    === null) ? "" : tabBar.currentItem.text

    property ThemeDashboard theme: Style.dashboard

    color: theme.background

    signal addMonitoredItems
    signal addEvents
    signal addNewDashboard

    function addMonitoredItemsDashboard(name) {
        tabRepeater.model.addItem(DashboardType.Variables, name)
        tabBar.currentIndex = tabRepeater.count - 2
    }

    function addEventsDashboard(name) {
        tabRepeater.model.addItem(DashboardType.Events, name)
        tabBar.currentIndex = tabRepeater.count - 2
    }

    ContextMenu {
        id: contextMenu

        property int selectedIndex

        listModel: ListModel {
            ListElement {
                imageSource: "qrc:/icons/delete.png"
                name: qsTr("Delete")
            }
        }

        onListItemClicked: function (index) {
            if (index === 0) {
                // delete pressed
                if (tabBar.currentIndex === selectedIndex) {
                    // allow selecting add item to force the change of the current item
                    tabBar.allowSelectingAddItem = true
                    tabBar.currentIndex = (selectedIndex > 0) ? 0 : 1
                    tabBar.allowSelectingAddItem = false
                }

                tabRepeater.model.removeItem(selectedIndex)
            }
        }
    }

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
                color: dragArea.held ? theme.item.backgroundHeld : theme.item.background

                Behavior on color {
                    ColorAnimation {
                        duration: 100
                    }
                }

                states: State {
                    when: dragArea.held

                    ParentChange {
                        target: content
                        parent: view
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
                            color: theme.item.textColor
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
                            color: theme.item.textColor

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: visualModel.model.disableMonitoring(
                                               dragArea.index)
                            }
                        }
                    }

                    Text {
                        width: parent.width - 2 * parent.padding
                        font.pointSize: 8
                        color: theme.item.textColor
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
                    color: theme.item.textColor
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

        model: (tabBar.currentItem === null) ? null : tabBar.currentItem.monitoringModel
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

        property int lastCurrentIndex: 0
        property bool allowSelectingAddItem: false

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        leftPadding: Math.max(15, (width - contentWidth) / 2)
        spacing: 5

        background: Rectangle {
            color: "transparent"
        }

        onCurrentIndexChanged: {
            if (tabRepeater.model.isAddItem(currentIndex)
                    && (tabRepeater.count > 1)
                    && !tabBar.allowSelectingAddItem) {
                tabBar.currentIndex = lastCurrentIndex
            } else {
                lastCurrentIndex = currentIndex
                tabRepeater.model.setCurrentIndex(currentIndex)
            }
        }

        Repeater {
            id: tabRepeater
            model: BackEnd.dashboardItemModel

            StyledIconTabButton {
                id: tabButton

                type: model.type
                text: model.name
                property var monitoringModel: model.monitoringModel

                onClicked: {
                    if (type === DashboardType.Add) {
                        addNewDashboard()
                    }
                }

                onPressAndHold: {
                    if (type === DashboardType.Add)
                        return

                    var pressedPoint = mapToItem(view, tabButton.pressX,
                                                 tabButton.pressY)
                    var xPos = pressedPoint.x - contextMenu.width / 2
                    contextMenu.x = Math.max(
                                0, Math.min(xPos,
                                            view.width - contextMenu.width))
                    contextMenu.y = pressedPoint.y - contextMenu.height

                    contextMenu.selectedIndex = index
                    contextMenu.open()
                }
            }
        }
    }
}
