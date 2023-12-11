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
    connectionView: currentTheme.connectionView
    imprintView: currentTheme.imprintView
    nodesView: currentTheme.nodesView
    settingsView: currentTheme.settingsView
    dashboard: currentTheme.dashboard
    contextMenu: currentTheme.contextMenu
    sideMenu: currentTheme.sideMenu
    listView: currentTheme.listView
    button: currentTheme.button
    comboBox: currentTheme.comboBox
    textField: currentTheme.textField
    tabButton: currentTheme.tabButton
    iconTabButton: currentTheme.iconTabButton
    scrollBar: currentTheme.scrollBar
    itemSelector: currentTheme.itemSelector
}
