/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma Singleton

import QtQuick

QtObject {
    id: colors

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
        window: colors.foreground
        text: colors.anthrazite
        base: colors.light
        alternateBase: colors.mediumLight
        mid: colors.medium
        dark: colors.dark
        light: colors.light
        // Could use accent in Qt 6.6+
        highlight: colors.accent

        brightText: highlight

        active.windowText: text
        inactive.windowText: colors.dark
        disabled.windowText: colors.mediumLight

        active.highlightedText: colors.foreground
        inactive.highlightedText: colors.light
        disabled.highlightedText: colors.mediumLight

        placeholderText: colors.medium

        active.button: colors.mediumLight
        inactive.button: colors.light
        disabled.button: colors.foreground

        active.buttonText: text
        inactive.buttonText: colors.dark
        disabled.buttonText: colors.mediumLight
    }

    readonly property Palette darkPalette: Palette {
        window: colors.anthrazite
        text: colors.foreground
        base: colors.dark
        alternateBase: colors.mediumDark
        mid: colors.mediumDark
        dark: colors.light
        light: colors.dark
        // Could use accent in Qt 6.6+
        highlight: colors.accent

        brightText: highlight

        link: "#2A82DA"

        active.windowText: text
        inactive.windowText:colors.light
        disabled.windowText: colors.mediumDark

        placeholderText: colors.medium

        active.highlightedText: text
        inactive.highlightedText: colors.light
        disabled.highlightedText: colors.mediumDark

        active.button: colors.mediumDark
        inactive.button: colors.dark
        disabled.button: colors.anthrazite

        active.buttonText: text
        inactive.buttonText: colors.light
        disabled.buttonText: colors.mediumDark
    }
}
