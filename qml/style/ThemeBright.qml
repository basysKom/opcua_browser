import QtQuick

ThemeDefault {
    browserView.background: light

    mainWindow {
        background: light

        header {
            iconColor: anthrazite
            dividerColor: foreground
        }
    }

    imprintView {
        background: light
        textColor: anthrazite
    }

    nodesView {
        backgroundSelected: mediumLight
        textColor: anthrazite
        textColorSelected: anthrazite
    }

    settingsView {
        background: light
        textColor: anthrazite
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

    sideMenu {
        background: light
        textColor: anthrazite
        iconColor: mediumDark
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

    comboBox {
        captionTextColor: anthrazite
        background: mediumLight
    }

    textField {
        captionTextColor: anthrazite
        background: foreground
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
