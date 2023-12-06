import QtQuick
import QtQuick.Controls

Button {
    highlighted: true

    palette.brightText: Style.button.highlightedTextColor
    palette.buttonText: Style.button.textColor

    background: Rectangle {
        color: highlighted ? Style.button.highlightedBackground : Style.button.background
        border.color: highlighted ? Style.button.highlightedBorderColor : Style.button.borderColor
        border.width: 1
        radius: 5
    }
}
