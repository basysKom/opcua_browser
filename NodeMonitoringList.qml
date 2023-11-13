import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    property alias monitoredAttributes: attributeList.model

    color: "lightgray"
    opacity: attributeList.model ? 1 : 0.3
    clip: true

    ListView {
        id: attributeList
        anchors.fill: parent

        ScrollBar.vertical: StyledScrollBar { }
        ScrollBar.horizontal: StyledScrollBar { }

        model: root.attributes

        delegate: Rectangle {
            id: listViewDelegate
            readonly property real padding: 5

            width: attributeList.width
            height: 30

            color: ((index % 2) == 0) ? "#9AE4E6" : "#9AE69A"

            // ToDo
            /*Text {
                id: attribute
                anchors.left: parent.left
                anchors.leftMargin: listViewDelegate.padding
                width: listViewDelegate.width / 3
                height: listViewDelegate.height
                verticalAlignment: Qt.AlignVCenter
                clip: true
                text: model.attribute
            }

            Rectangle {
                id: divider
                anchors.left: attribute.right
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
