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
            width: maxDelegateWidth()

            //boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: StyledScrollBar {}

            headerPositioning: ListView.OverlayHeader

            header: Rectangle {
                id: headerItem

                width: referenceList.width
                implicitHeight: childrenRect.height
                z: 2

                color: "lightblue"

                RowLayout {
                    spacing: 0
                    height: 30

                    Text {
                        Layout.margins: 5
                        Layout.fillHeight: true
                        Layout.preferredWidth: root.width / 3 + 20
                        verticalAlignment: Qt.AlignVCenter
                        text: qsTr("Reference")
                        elide: Qt.ElideRight
                        font.bold: true
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
                        text: qsTr("Target")
                        elide: Qt.ElideRight
                        font.bold: true
                    }
                }
            }

            delegate: Rectangle {
                id: listViewDelegate

                readonly property real padding: 5

                width: maxDelegateWidth()
                implicitHeight: childrenRect.height
                color: ((index % 2) == 0) ? "#9AE4E6" : "#9AE69A"

                RowLayout {
                    spacing: 0
                    height: 30

                    Image {
                        Layout.leftMargin: 5
                        Layout.alignment: Qt.AlignVCenter
                        width: 15
                        height: width
                        source: model.isForward ? "qrc:/icons/forward.png" : "qrc:/icons/inverse.png"
                    }

                    Text {
                        Layout.margins: 5
                        Layout.fillHeight: true
                        Layout.preferredWidth: root.width / 3
                        verticalAlignment: Qt.AlignVCenter
                        text: model.type
                        elide: Qt.ElideRight

                        MouseArea {
                            anchors.fill: parent
                            onClicked: BackEnd.opcUaModel.setCurrentNodeId(
                                           model.typeNodeId)
                        }
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
                        Layout.minimumWidth: root.width - x
                        verticalAlignment: Qt.AlignVCenter
                        text: model.target

                        MouseArea {
                            anchors.fill: parent
                            onClicked: BackEnd.opcUaModel.setCurrentNodeId(
                                           model.targetNodeId)
                        }
                    }
                }
            }
        }
    }
}
