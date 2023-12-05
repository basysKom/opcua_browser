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
    }

    property QtObject tabButton: QtObject {
        property color background: currentTheme.tabButton.background
        property color backgroundSelected: currentTheme.tabButton.backgroundSelected
        property color dividerColor: currentTheme.tabButton.dividerColor
        property color dividerColorSelected: currentTheme.tabButton.dividerColorSelected
        property color textColor: currentTheme.tabButton.textColor
    }

    property QtObject scrollBar: QtObject {
        property color background: currentTheme.scrollBar.background
        property color selector: currentTheme.scrollBar.selector
    }


    /*
    property QtObject darkMode: QtObject {

        property QtObject mainWindow: QtObject {
            property color background: anthrazite //"orange"
        }

        property QtObject view: QtObject {
            property color background: anthrazite //"#FFBC42"
        }

        property QtObject nodesView: QtObject {
            property color backgroundSelected: mediumDark //"#584b53"
            property color textColor: mediumLight // "black"
            property color textColorSelected: mediumLight // "white"
        }

        property QtObject connectionView: QtObject {
            property color disconnected: "#F44336"
            property color connecting: "#F2C94C"
            property color connected: "#4CAF50"
            property color textColor: foreground
            property color textFieldBackground: foreground
            property color comboBoxBackground: mediumLight
            property color buttonBackground: mediumLight
        }

        property QtObject dashboard: QtObject {
            property color background: anthrazite //"#FFBC42"

            property QtObject item: QtObject {
                property color background: dark //"orange"
                property color backgroundHeld: "lightsteelblue" //"lightsteelblue"
                property color textColor: mediumLight
            }
        }

        property QtObject contextMenu: QtObject {
            property color background: dark //"orange"
            property color backgroundSelected: accent //"lightgray"
        }

        property QtObject listView: QtObject {
            property color background: anthrazite //"lightgray"
            property color headerBackground: mediumDark // "lightblue"
            property color color1: anthrazite //"#9AE4E6"
            property color color2: dark //"#9AE69A"
            property color textColor: mediumLight
            property color divider: dark
        }

        property QtObject tabButton: QtObject {
            property color background: anthrazite //"dimgray"
            property color backgroundSelected: accent //"white"
            property color dividerColor: dark
            property color dividerColorSelected: accent
            property color textColor: mediumLight
        }

        property QtObject scrollBar: QtObject {
            property color background: "transparent"
            property color selector: mediumDark //"darkgray"
        }
    }

    property QtObject brightMode: QtObject {

        property QtObject mainWindow: QtObject {
            property color background: light
        }

        property QtObject view: QtObject {
            property color background: light
        }

        property QtObject nodesView: QtObject {
            property color backgroundSelected: mediumLight
            property color textColor: anthrazite
            property color textColorSelected: anthrazite
        }

        property QtObject connectionView: QtObject {
            property color disconnected: "#F44336"
            property color connecting: "#F2C94C"
            property color connected: "#4CAF50"
            property color textColor: anthrazite
            property color textFieldBackground: foreground
            property color comboBoxBackground: mediumLight
            property color buttonBackground: medium
        }

        property QtObject dashboard: QtObject {
            property color background: light

            property QtObject item: QtObject {
                property color background: foreground
                property color backgroundHeld: "lightsteelblue"
                property color textColor: anthrazite
            }
        }

        property QtObject contextMenu: QtObject {
            property color background: foreground
            property color backgroundSelected: accent
        }

        property QtObject listView: QtObject {
            property color background: light
            property color headerBackground: mediumLight
            property color color1: light
            property color color2: foreground
            property color textColor: anthrazite
            property color divider: mediumDark
        }

        property QtObject tabButton: QtObject {
            property color background: light
            property color backgroundSelected: accent
            property color dividerColor: dark
            property color dividerColorSelected: accent
            property color textColor: anthrazite
        }

        property QtObject scrollBar: QtObject {
            property color background: "transparent"
            property color selector: medium
        }
    }*/
}
