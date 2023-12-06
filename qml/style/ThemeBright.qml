import QtQuick

ThemeDefault {
    browserView.background: light

    mainWindow {
        background: light

        header {
            iconColor: anthrazite
        }
    }

    nodesView {
        backgroundSelected: mediumLight
        textColor: anthrazite
        textColorSelected: anthrazite
    }

    connectionView {
        textColor: anthrazite
        textFieldBackground: foreground
        comboBoxBackground: mediumLight
    }

    dashboard {
        background: light

        item {
            background: foreground
            backgroundHeld: "lightsteelblue"
            textColor: anthrazite
        }
    }

    contextMenu {
        background: foreground
        backgroundSelected: accent
        textColor: anthrazite
    }

    listView {
        background: light
        headerBackground: mediumLight
        color1: light
        color2: foreground
        textColor: anthrazite
        divider: mediumDark
    }

    button {
        borderColor: anthrazite
        textColor: anthrazite
    }

    tabButton {
        background: light
        backgroundSelected: accent
        dividerColor: dark
        dividerColorSelected: accent
        textColor: anthrazite
    }

    iconTabButton {
        textColor: anthrazite
    }

    scrollBar {
        background: "transparent"
        selector: medium
    }

    itemSelector {
        background: mainWindow.background
    }
}
