pragma Singleton

import QtQuick

QtObject {
    property QtObject themeDark: ThemeDark {}
    property QtObject themeBright: ThemeBright {}

    property var themes: [themeDark, themeBright]
    property int currentThemeIndex: 0
    property QtObject currentTheme: themes[currentThemeIndex]

    property QtObject mainWindow: QtObject {
        property color background: currentTheme.mainWindow.background

        property QtObject header: QtObject {
            property color background: currentTheme.mainWindow.header.background
            property color iconColor: currentTheme.mainWindow.header.iconColor
        }
    }

    property QtObject browserView: QtObject {
        property color background: currentTheme.browserView.background
    }

    property QtObject nodesView: QtObject {
        property color backgroundSelected: currentTheme.nodesView.backgroundSelected
        property color textColor: currentTheme.nodesView.textColor
        property color textColorSelected: currentTheme.nodesView.textColorSelected
    }

    property QtObject connectionView: QtObject {
        property color disconnected: currentTheme.connectionView.disconnected
        property color connecting: currentTheme.connectionView.connecting
        property color connected: currentTheme.connectionView.connected
        property color textColor: currentTheme.connectionView.textColor
        property color textFieldBackground: currentTheme.connectionView.textFieldBackground
        property color comboBoxBackground: currentTheme.connectionView.comboBoxBackground
    }

    property QtObject dashboard: QtObject {
        property color background: currentTheme.dashboard.background

        property QtObject item: QtObject {
            property color background: currentTheme.dashboard.item.background
            property color backgroundHeld: currentTheme.dashboard.item.backgroundHeld
            property color textColor: currentTheme.dashboard.item.textColor
        }
    }

    property QtObject contextMenu: QtObject {
        property color background: currentTheme.contextMenu.background
        property color backgroundSelected: currentTheme.contextMenu.backgroundSelected
        property color textColor: currentTheme.contextMenu.textColor
    }

    property QtObject listView: QtObject {
        property color background: currentTheme.listView.background
        property color headerBackground: currentTheme.listView.headerBackground
        property color color1: currentTheme.listView.color1
        property color color2: currentTheme.listView.color2
        property color textColor: currentTheme.listView.textColor
        property color divider: currentTheme.listView.divider
    }

    property QtObject button: QtObject {
        property color background: currentTheme.button.background
        property color borderColor: currentTheme.button.borderColor
        property color textColor: currentTheme.button.textColor
        property color highlightedBackground: currentTheme.button.highlightedBackground
        property color highlightedBorderColor: currentTheme.button.highlightedBorderColor
        property color highlightedTextColor: currentTheme.button.highlightedTextColor
    }

    property QtObject tabButton: QtObject {
        property color background: currentTheme.tabButton.background
        property color backgroundSelected: currentTheme.tabButton.backgroundSelected
        property color dividerColor: currentTheme.tabButton.dividerColor
        property color dividerColorSelected: currentTheme.tabButton.dividerColorSelected
        property color textColor: currentTheme.tabButton.textColor
    }

    property QtObject iconTabButton: QtObject {
        property color background: currentTheme.iconTabButton.background
        property color backgroundSelected: currentTheme.iconTabButton.backgroundSelected
        property color textColor: currentTheme.iconTabButton.textColor
        property color textColorSelected: currentTheme.iconTabButton.textColorSelected
    }

    property QtObject scrollBar: QtObject {
        property color background: currentTheme.scrollBar.background
        property color selector: currentTheme.scrollBar.selector
    }

    property QtObject itemSelector: QtObject {
        property color background: currentTheme.itemSelector.background
        property real backgroundOpacity: currentTheme.itemSelector.backgroundOpacity
        property color borderColor: currentTheme.itemSelector.borderColor
        property color checkMarkColor: currentTheme.itemSelector.checkMarkColor
    }
}
