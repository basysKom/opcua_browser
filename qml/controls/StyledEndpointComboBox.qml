import QtQuick
import QtQuick.Controls

StyledComboBox {
    comboBoxColumnHeight: _comboBox.contentItem.implicitHeight

    function getDisplayTextPart(str, part) {
        var parts = str.split("#")
        if (part >= parts.length) {
            return ""
        }

        return parts[part]
    }

    _comboBox.contentItem: Column {
        topPadding: 6
        bottomPadding: topPadding
        leftPadding: 12
        Text {
            font.pointSize: 12
            text: getDisplayTextPart(_comboBox.displayText, 0)
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            leftPadding: 12
            font.pointSize: 10
            text: getDisplayTextPart(_comboBox.displayText, 1)
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            leftPadding: 12
            font.pointSize: 10
            text: getDisplayTextPart(_comboBox.displayText, 2)
            verticalAlignment: Text.AlignVCenter
        }
    }

    _comboBox.delegate: ItemDelegate {
        id: delegate
        topPadding: 6
        bottomPadding: topPadding

        required property var model
        required property int index
        property string delegateText: model[_comboBox.textRole]

        width: _comboBox.width
        highlighted: _comboBox.highlightedIndex === index

        contentItem: Column {
            Text {
                font.pointSize: 12
                text: getDisplayTextPart(delegateText, 0)
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                leftPadding: 12
                font.pointSize: 10
                text: getDisplayTextPart(delegateText, 1)
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                leftPadding: 12
                font.pointSize: 10
                text: getDisplayTextPart(delegateText, 2)
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
