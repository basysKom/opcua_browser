import QtQuick
import QtQuick.Controls

Button {
    palette.buttonText: Style.button.textColor

    background: Rectangle {
        color: Style.button.background
        border.color: Style.button.borderColor
        border.width: 1
        radius: 5
    }
}
