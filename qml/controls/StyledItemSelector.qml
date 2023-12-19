import QtQuick
import QtQuick.Controls

import OPC_UA_Browser

CheckBox {
    id: control

    property ThemeItemSelector theme: Style.itemSelector

    indicator: null
    background: Rectangle {
        id: background
        width: control.width
        height: width
        radius: width / 2
        color: theme.background
        opacity: theme.backgroundOpacity
    }

    Rectangle {
        anchors.fill: parent
        radius: background.radius
        border.width: 1
        border.color: theme.borderColor
        color: "transparent"

        IconImage {
            id: checkmark
            fillMode: Image.PreserveAspectFit
            anchors.centerIn: parent
            source: "qrc:/icons/checkmark.svg"
            sourceSize.width: parent.height * 0.65
            sourceSize.height: parent.width * 0.65
            color: theme.checkMarkColor
            visible: (checkState === Qt.Checked)
        }
    }
}
