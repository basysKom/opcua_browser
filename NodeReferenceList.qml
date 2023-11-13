import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    property alias references: referenceList.model

    color: "lightgray"
    opacity: referenceList.model ? 1 : 0.3
    clip: true

    ListView {
        id: referenceList
        anchors.fill: parent

        ScrollBar.vertical: StyledScrollBar { }
        ScrollBar.horizontal: StyledScrollBar { }

        model: root.references

        delegate: Rectangle {
            id: listViewDelegate
            readonly property real padding: 5

            width: referenceList.width
            height: 30

            color: ((index % 2) == 0) ? "#9AE4E6" : "#9AE69A"

            // ToDo
            /*Text {
                id: reference
                anchors.left: parent.left
                anchors.leftMargin: listViewDelegate.padding
                width: listViewDelegate.width / 3
                height: listViewDelegate.height
                verticalAlignment: Qt.AlignVCenter
                clip: true
                text: model.reference
            }

            Rectangle {
                id: divider
                anchors.left: reference.right
                width: 1
                height: listViewDelegate.height
                color: "black"
            }

            Text {
                id: valueLabel
                anchors.left: divider.right
                anchors.leftMargin: listViewDelegate.padding
                height: listViewDelegate.height
                verticalAlignment: Qt.AlignVCenter
                clip: true
                text: model.value
            }*/
        }
    }
}
