import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property alias references: referenceList.model

    function maxDelegateWidth() {
        var max = flickable.width
        for (var child in referenceList.contentItem.children) {
            max = Math.max(
                        max,
                        referenceList.contentItem.children[child].childrenRect.width)
        }
        return max
    }

    color: "lightgray"
    opacity: referenceList.model ? 1 : 0.3
    clip: true

    Flickable {
        id: flickable

        anchors.fill: parent
        clip: true

        contentWidth: referenceList.width
        contentHeight: referenceList.height

        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.horizontal: StyledScrollBar {}

        ListView {
            id: referenceList

            height: flickable.height
            width: contentItem.childrenRect.width

            //boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: StyledScrollBar {}

            highlightFollowsCurrentItem: false

            delegate: Rectangle {
                id: listViewDelegate

                readonly property real padding: 5

                width: maxDelegateWidth()
                implicitHeight: childrenRect.height
                color: ((index % 2) == 0) ? "#9AE4E6" : "#9AE69A"

                RowLayout {
                    spacing: 0
                    height: 30

                    Text {
                        Layout.margins: 5
                        Layout.fillHeight: true
                        Layout.preferredWidth: root.width / 3
                        verticalAlignment: Qt.AlignVCenter
                        text: model.type
                        elide: Qt.ElideRight
                    }

                    Rectangle {
                        Layout.fillHeight: true
                        width: 1
                        color: "black"
                    }

                    Text {
                        Layout.margins: 5
                        Layout.fillHeight: true
                        Layout.preferredWidth: 25
                        verticalAlignment: Qt.AlignVCenter
                        text: model.isForward ? "true" : "false"
                    }

                    Rectangle {
                        Layout.fillHeight: true
                        width: 1
                        color: "black"
                    }

                    Text {
                        Layout.margins: 5
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        verticalAlignment: Qt.AlignVCenter
                        text: model.target
                    }
                }
            }
        }
    }
}
