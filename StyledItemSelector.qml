import QtQuick
import QtQuick.Controls

CheckBox {
    id: control

    //property ThemeItemSelector theme: Style.itemSelector
    indicator: null
    background: Rectangle {
        id: background
        width: control.width
        height: width
        radius: width / 2
        color: Style.itemSelector.background
        opacity: Style.itemSelector.backgroundOpacity
    }

    Rectangle {
        anchors.fill: parent
        radius: background.radius
        border.width: 1
        border.color: Style.itemSelector.borderColor
        color: "transparent"

        IconImage {
            id: checkmark
            fillMode: Image.PreserveAspectFit
            anchors.centerIn: parent
            source: "qrc:/icons/checkmark.svg"
            height: parent.height * 0.65
            width: parent.width * 0.65
            color: Style.itemSelector.checkMarkColor
            visible: (checkState === Qt.Checked)
        }
    }
}
