#[[
SPDX-FileCopyrightText: 2024 basysKom GmbH
SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
SPDX-License-Identifier: BSD-3-Clause
]]

cmake_minimum_required(VERSION 3.19)

set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)
set(QML_IMPORT_PATH ${QT_QML_OUTPUT_DIRECTORY} CACHE STRING "Import paths for Qt Creator's code model" FORCE)

qt_add_executable(appOPC_UA_Browser
    main.cpp
)

set_source_files_properties(qml/style/Style.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(appOPC_UA_Browser
    URI OPC_UA_Browser
    VERSION 1.0
    DEPENDENCIES QtCore QtQuick
    QML_FILES qml/Main.qml
              qml/BrowserView.qml
              qml/ConnectionView.qml
              qml/ContentView.qml
              qml/ContextMenu.qml
              qml/DashboardConfigurationView.qml
              qml/DashboardView.qml
              qml/ImprintView.qml
              qml/LicenseView.qml
              qml/LoggingView.qml
              qml/MessageView.qml
              qml/NodeAttributeList.qml
              qml/NodeDetailView.qml
              qml/NodeReferenceList.qml
              qml/NodesView.qml
              qml/SettingsView.qml
              qml/controls/SideMenu.qml
              qml/controls/StyledButton.qml
              qml/controls/StyledComboBox.qml
              qml/controls/StyledEndpointComboBox.qml
              qml/controls/StyledIconTabButton.qml
              qml/controls/StyledItemSelector.qml
              qml/controls/StyledMenuItem.qml
              qml/controls/StyledMenuSeparator.qml
              qml/controls/StyledScrollBar.qml
              qml/controls/StyledTabButton.qml
              qml/controls/StyledTextField.qml
              qml/style/Style.qml
              qml/style/StyleDefinitions.qml
              qml/style/ThemeBright.qml
              qml/style/ThemeBrowserView.qml
              qml/style/ThemeButton.qml
              qml/style/ThemeComboBox.qml
              qml/style/ThemeConnectionView.qml
              qml/style/ThemeContextMenu.qml
              qml/style/ThemeDark.qml
              qml/style/ThemeDashboard.qml
              qml/style/ThemeDashboardItem.qml
              qml/style/ThemeDefault.qml
              qml/style/ThemeHeader.qml
              qml/style/ThemeIconTabButton.qml
              qml/style/ThemeImprintView.qml
              qml/style/ThemeItemSelector.qml
              qml/style/ThemeLicenseView.qml
              qml/style/ThemeListView.qml
              qml/style/ThemeLoggingView.qml
              qml/style/ThemeMainWindow.qml
              qml/style/ThemeMessageView.qml
              qml/style/ThemeNodesView.qml
              qml/style/ThemeScrollBar.qml
              qml/style/ThemeSettingsView.qml
              qml/style/ThemeSideMenu.qml
              qml/style/ThemeTabButton.qml
              qml/style/ThemeTextField.qml
    SOURCES attribute.h attribute.cpp
            attributemodel.h attributemodel.cpp
            backend.h backend.cpp
            certificateitemmodel.h certificateitemmodel.cpp
            dashboarditem.h dashboarditem.cpp
            dashboarditemmodel.h dashboarditemmodel.cpp
            logging.h logging.cpp
            loggingviewfiltermodel.h loggingviewfiltermodel.cpp
            loggingviewmodel.h loggingviewmodel.cpp
            monitoreditem.h monitoreditem.cpp
            monitoreditemmodel.h monitoreditemmodel.cpp
            opcuahelper.h opcuahelper.cpp
            opcuamodel.h opcuamodel.cpp
            reference.h reference.cpp
            referencemodel.h referencemodel.cpp
            treeitem.h treeitem.cpp
            uisettings.h uisettings.cpp
            x509certificate.h x509certificate.cpp
)

qt_add_resources(appOPC_UA_Browser "icons"
    PREFIX "/"
    FILES font/Swansea.ttf
          font/SwanseaBold.ttf
          icons/arrow_right.svg
          icons/back.svg
          icons/checkmark.svg
          icons/connect.svg
          icons/dashboard.svg
          icons/delete.svg
          icons/disconnect.svg
          icons/event.svg
          icons/expert.svg
          icons/forward.svg
          icons/info.svg
          icons/inverse.svg
          icons/license.svg
          icons/log.svg
          icons/logo_basyskom.svg
          icons/menu.svg
          icons/plus.svg
          icons/refresh.svg
          icons/save.svg
          icons/settings.svg)

target_compile_options(appOPC_UA_Browser PRIVATE -Wall -Wextra -Wdeprecated)

target_link_libraries(appOPC_UA_Browser
    PRIVATE Qt6::Core Qt6::Quick Qt6::QuickControls2 Qt::OpcUa
)

set_property(TARGET appOPC_UA_Browser APPEND PROPERTY
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appOPC_UA_Browser PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appOPC_UA_Browser
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER com.basyskom.opcua.browser
    XCODE_ATTRIBUTE_DEVELOPMENT_TEAM SCCL9D8G29 # basysKom GmbH
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if (IOS)
    qt6_import_plugins(appOPC_UA_Browser INCLUDE Qt::QOpen62541Plugin)
endif()

include(GNUInstallDirs)
install(TARGETS appOPC_UA_Browser
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
