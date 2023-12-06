pragma Singleton

import QtQuick

ThemeDefault {
    property ThemeDefault themeDark: ThemeDark {}
    property ThemeDefault themeBright: ThemeBright {}

    property var themes: [themeDark, themeBright]
    property int currentThemeIndex: 0
    property ThemeDefault currentTheme: themes[currentThemeIndex]

    mainWindow: currentTheme.mainWindow
    browserView: currentTheme.browserView
    nodesView: currentTheme.nodesView
    connectionView: currentTheme.connectionView
    dashboard: currentTheme.dashboard
    contextMenu: currentTheme.contextMenu
    listView: currentTheme.listView
    button: currentTheme.button
    tabButton: currentTheme.tabButton
    iconTabButton: currentTheme.iconTabButton
    scrollBar: currentTheme.scrollBar
    itemSelector: currentTheme.itemSelector
}
