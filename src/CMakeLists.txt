#[[
SPDX-FileCopyrightText: 2024 basysKom GmbH
SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
SPDX-License-Identifier: BSD-3-Clause
]]

cmake_minimum_required(VERSION 3.19)

set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)
set(QML_IMPORT_PATH ${QT_QML_OUTPUT_DIRECTORY} CACHE STRING "Import paths for Qt Creator's code model" FORCE)

qt6_add_executable(appOPC_UA_Browser
    main.cpp
)

add_subdirectory(com/basyskom/quickcontrols)

set_source_files_properties(qml/Colors.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt6_add_qml_module(appOPC_UA_Browser
    URI OPC_UA_Browser
    VERSION 1.0
    DEPENDENCIES QtCore QtQuick
    QML_FILES qml/Colors.qml
              qml/Main.qml
              qml/views/BrowserView.qml
              qml/views/ConnectionView.qml
              qml/views/ContentView.qml
              qml/views/DashboardConfigurationView.qml
              qml/views/DashboardView.qml
              qml/views/ImprintView.qml
              qml/views/LicenseView.qml
              qml/views/LoggingView.qml
              qml/views/MessageView.qml
              qml/views/NodeDetailView.qml
              qml/views/NodesView.qml
              qml/views/SettingsView.qml
              qml/controls/ContextMenu.qml
              qml/controls/IconTabButton.qml
              qml/controls/LabelledComboBox.qml
              qml/controls/LabelledEndpointComboBox.qml
              qml/controls/LabelledSpinBox.qml
              qml/controls/LabelledTextField.qml
              qml/controls/NodeAttributeList.qml
              qml/controls/NodeReferenceList.qml
              qml/controls/SideMenu.qml
    SOURCES attribute.h attribute.cpp
            attributemodel.h attributemodel.cpp
            backend.h backend.cpp
            certificateitemmodel.h certificateitemmodel.cpp
            dashboarditem.h dashboarditem.cpp
            dashboarditemmodel.h dashboarditemmodel.cpp
            languageitemmodel.h languageitemmodel.cpp
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
            constants.h constants.cpp
            companionspecdashboardcreator.h
            companionspecdashboardcreator.cpp
            woodworkingdashboardcreator.h
            woodworkingdashboardcreator.cpp
)

qt6_add_resources(appOPC_UA_Browser "icons"
    PREFIX "/"
    FILES font/Swansea.ttf
          font/SwanseaBold.ttf
          icons/keyboard_arrow_down.svg
          icons/arrow_right.svg
          icons/keyboard_arrow_up.svg
          icons/back.svg
          icons/bolt.svg
          icons/cancel.svg
          icons/checkmark.svg
          icons/clear_all.svg
          icons/connect.svg
          icons/dashboard.svg
          icons/delete.svg
          icons/disconnect.svg
          icons/edit.svg
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
          icons/minus.svg
          icons/save.svg
          icons/settings.svg
          languages/English.png
          languages/English.utf8
          languages/German.png
          languages/German.utf8)

qt6_add_translations(appOPC_UA_Browser
          QM_FILES_OUTPUT_VARIABLE APP_QM_FILES
          TS_FILES languages/English_en_GB.ts
                   languages/German_de_DE.ts
          LUPDATE_OPTIONS -locations none)

qt6_add_resources(appOPC_UA_Browser "i18n"
    PREFIX "/i18n"
    BASE "${CMAKE_CURRENT_BINARY_DIR}"
    FILES ${APP_QM_FILES})

target_compile_options(appOPC_UA_Browser PRIVATE -Wall -Wextra -Wdeprecated)

target_link_libraries(appOPC_UA_Browser PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt::OpcUa

    quickcontrols_moduleplugin
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
