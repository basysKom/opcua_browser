import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property alias attributes: attributeList.model

    function maxDelegateWidth() {
        var max = flickable.width
        for (var child in attributeList.contentItem.children) {
            max = Math.max(
                        max,
                        attributeList.contentItem.children[child].childrenRect.width)
        }
        return max
    }

    color: "lightgray"
    opacity: attributeList.model ? 1 : 0.3
    clip: true

    Flickable {
        id: flickable

        anchors.fill: parent
        clip: true

        contentWidth: attributeList.width
        contentHeight: attributeList.height

        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.horizontal: StyledScrollBar {}

        ListView {
            id: attributeList

            property bool wasOvershooted: false

            height: flickable.height
            width: maxDelegateWidth()

            model: root.attributes
            //boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: StyledScrollBar {}

            onVerticalOvershootChanged: {
                // update attribute list on vertical overshoot
                if (wasOvershooted && (verticalOvershoot >= 0)) {
                    BackEnd.opcUaModel.refreshAttributesForCurrentIndex()
                }
                wasOvershooted = (verticalOvershoot < 0)
            }

            delegate: Rectangle {
                id: listViewDelegate

                readonly property real padding: 5

                width: maxDelegateWidth()
                implicitHeight: childrenRect.height
                color: ((index % 2) == 0) ? "#9AE4E6" : "#9AE69A"

                RowLayout {
                    spacing: 0
                    height: Math.max(30, valueLabel.implicitHeight)

                    Text {
                        id: attribute

                        Layout.margins: 5
                        Layout.fillHeight: true
                        Layout.preferredWidth: root.width / 3
                        verticalAlignment: Qt.AlignVCenter
                        text: model.attribute
                        elide: Qt.ElideRight
                    }

                    Rectangle {
                        id: divider

                        Layout.fillHeight: true
                        width: 1
                        color: "black"
                    }

                    Text {
                        id: valueLabel

                        Layout.margins: 5
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        verticalAlignment: Qt.AlignVCenter
                        text: model.value
                    }
                }
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: attributeList.verticalOvershoot < 0
    }
}
