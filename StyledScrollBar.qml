import QtQuick
import QtQuick.Controls

ScrollBar {
    id: control

    contentItem: Rectangle {
        implicitWidth: 10
        implicitHeight: 10
        radius: 5
        color: "darkgray"
        opacity: (control.policy === ScrollBar.AlwaysOn)
                 || (control.active && control.size < 1.0) ? 0.75 : 0

        // Animate the changes in opacity (default duration is 250 ms).
        Behavior on opacity {
            NumberAnimation {}
        }
    }

    background: Rectangle {
        color: "transparent"
    }
}
