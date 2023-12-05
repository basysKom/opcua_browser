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

    color: Style.listView.background
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
                color: ((index % 1)
                        == 0) ? Style.listView.color1 : Style.listView.color2

                ColumnLayout {
                    spacing: 0

                    Rectangle {
                        Layout.preferredWidth: flickable.width
                        Layout.preferredHeight: 1
                        visible: model.index > 0
                        color: Style.listView.divider
                    }

                    Text {
                        id: attribute

                        Layout.topMargin: 5
                        Layout.leftMargin: 5
                        Layout.fillWidth: true
                        verticalAlignment: Qt.AlignVCenter
                        text: model.attribute
                        elide: Qt.ElideRight
                        color: Style.listView.textColor
                        font {
                            pointSize: 11
                            bold: true
                        }
                    }

                    Text {
                        id: valueLabel

                        Layout.leftMargin: 5
                        Layout.bottomMargin: 5
                        Layout.fillWidth: true
                        verticalAlignment: Qt.AlignVCenter
                        text: model.value
                        color: attribute.color
                    }
                }


                /*RowLayout {
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
                        color: Style.listView.textColor
                    }

                    Rectangle {
                        id: divider

                        Layout.fillHeight: true
                        width: 1
                        color: attribute.color
                    }

                    Text {
                        id: valueLabel

                        Layout.margins: 5
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        verticalAlignment: Qt.AlignVCenter
                        text: model.value
                        color: attribute.color
                    }
                }*/
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: attributeList.verticalOvershoot < 0
    }
}
