/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma Singleton

import QtQuick

QtObject {
    property bool isDarkMode: true

    readonly property Palette palette: isDarkMode ? darkPalette : lightPalette

    readonly property color anthrazite: "#373f43"
    readonly property color dark: "#464e52"
    readonly property color mediumDark: "#596165"
    readonly property color medium: "#B1B1B1"
    readonly property color mediumLight: "#D2D2D2"
    readonly property color light: "#E1E1E1"
    readonly property color foreground: "#F3F3F3"
    readonly property color accent: "#EF7D00"

    // Colors for special backgrounds in DashboardView.qml
    readonly property color dashboardErrorBackground: isDarkMode ? "#E00F41" : "#F08080"
    readonly property color dashboardHeldBackground: isDarkMode ? "#365680" : "#B0C4DE"

    // Colors for connection states in ConnectionView.qml
    readonly property color green: "#4CAF50"
    readonly property color yellow: "#F2C94C"
    readonly property color red: "#F44336"

    // Colors for logging categories in LoggingView.qml
    readonly property color debug: "transparent"
    readonly property color info: isDarkMode ? "#325D81" : "#9EBED9"
    readonly property color warning: isDarkMode ? "#828200" : "#FEFF78"
    readonly property color critical: isDarkMode ? "#B30000" : "#FFa9a9"
    readonly property color fatal: isDarkMode ? "#510000" : "#FF7878"

    readonly property Palette lightPalette: Palette {
        window: Colors.foreground
        text: Colors.anthrazite
        base: Colors.light
        alternateBase: Colors.mediumLight
        mid: Colors.medium
        dark: Colors.dark
        light: Colors.light
        // Could use accent in Qt 6.6+
        highlight: Colors.accent

        brightText: highlight

        active.windowText: text
        inactive.windowText: Colors.dark
        disabled.windowText: Colors.mediumLight

        active.highlightedText: Colors.foreground
        inactive.highlightedText: Colors.light
        disabled.highlightedText: Colors.mediumLight

        placeholderText: Colors.medium

        active.button: Colors.mediumLight
        inactive.button: Colors.light
        disabled.button: Colors.foreground

        active.buttonText: text
        inactive.buttonText: Colors.dark
        disabled.buttonText: Colors.mediumLight
    }

    readonly property Palette darkPalette: Palette {
        window: Colors.anthrazite
        text: Colors.foreground
        base: Colors.dark
        alternateBase: Colors.mediumDark
        mid: Colors.mediumDark
        dark: Colors.light
        light: Colors.dark
        // Could use accent in Qt 6.6+
        highlight: Colors.accent

        brightText: highlight

        link: "#2A82DA"

        active.windowText: text
        inactive.windowText:Colors.light
        disabled.windowText: Colors.mediumDark

        placeholderText: Colors.medium

        active.highlightedText: text
        inactive.highlightedText: Colors.light
        disabled.highlightedText: Colors.mediumDark

        active.button: Colors.mediumDark
        inactive.button: Colors.dark
        disabled.button: Colors.anthrazite

        active.buttonText: text
        inactive.buttonText: Colors.light
        disabled.buttonText: Colors.mediumDark
    }
}
