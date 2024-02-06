/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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

    loggingView {
        background: light
        textColor: anthrazite
        divider: mediumDark
        infoColor: "#9ebed9"
        warningColor: "#feff78"
        criticalColor: "#ffa9a9"
        fatalColor: "#ff7878"
    }

    nodesView {
        backgroundSelected: mediumLight
        textColor: anthrazite
        textColorSelected: anthrazite
    }

    settingsView {
        background: light
        backgroundListView: foreground
        backgroundSelected: mediumLight
        textColor: anthrazite
    }

    dashboard {
        background: light

        item {
            background: foreground
            backgroundHeld: "lightsteelblue"
            backgroundError: "#F26161"
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
