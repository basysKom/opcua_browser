import QtQuick

import OPC_UA_Browser

Rectangle {
    id: view

    property ThemeSettingsView theme: Style.settingsView

    function setTheme(index) {
        window.themeIndex = index
        if (index === 0) {
            darkItemSelector.checkState = Qt.Checked
            brightItemSelector.checkState = Qt.Unchecked
        } else {
            darkItemSelector.checkState = Qt.Unchecked
            brightItemSelector.checkState = Qt.Checked
        }
    }

    color: theme.background

    Column {
        leftPadding: 10
        rightPadding: leftPadding
        spacing: 10

        Text {
            color: theme.textColor
            font {
                pointSize: 18
                bold: true
            }
            text: qsTr("Settings")
        }

        Text {
            color: theme.textColor
            font {
                pointSize: 14
                bold: true
            }
            text: qsTr("Theme")
        }

        Row {
            spacing: 5

            StyledItemSelector {
                id: darkItemSelector

                height: 24
                width: height
                checkState: (Style.currentThemeIndex === 0) ? Qt.Checked : Qt.Unchecked
                onToggled: setTheme(0)
            }

            Text {
                anchors.verticalCenter: darkItemSelector.verticalCenter
                color: theme.textColor
                font.pointSize: 12
                text: qsTr("Dark")
            }

            Item {
                width: 20
                height: 1
            }

            StyledItemSelector {
                id: brightItemSelector

                height: 24
                width: height
                checkState: (Style.currentThemeIndex === 1) ? Qt.Checked : Qt.Unchecked
                onToggled: setTheme(1)
            }

            Text {
                anchors.verticalCenter: brightItemSelector.verticalCenter
                color: theme.textColor
                font.pointSize: 12
                text: qsTr("Bright")
            }
        }
    }
}
