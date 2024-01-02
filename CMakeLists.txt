cmake_minimum_required(VERSION 3.16)
include(FetchContent)

project(OPC_UA_Browser VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QML_IMPORT_PATH ${CMAKE_SOURCE_DIR}/qml ${CMAKE_BINARY_DIR}/imports CACHE STRING "" FORCE)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Quick QuickControls2 OpcUa)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appOPC_UA_Browser
    main.cpp
)

if (ANDROID)
    FetchContent_Declare(
      android_openssl
      DOWNLOAD_EXTRACT_TIMESTAMP true
      URL      https://github.com/KDAB/android_openssl/archive/refs/heads/master.zip
    )
    FetchContent_MakeAvailable(android_openssl)
    include(${android_openssl_SOURCE_DIR}/android_openssl.cmake)

    add_android_openssl_libraries(appOPC_UA_Browser)
endif()

set_source_files_properties(qml/style/Style.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

set_property(TARGET appOPC_UA_Browser APPEND PROPERTY
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)

qt_add_qml_module(appOPC_UA_Browser
    URI OPC_UA_Browser
    VERSION 1.0
    QML_FILES qml/Main.qml
              qml/BrowserView.qml
              qml/ConnectionView.qml
              qml/ContentView.qml
              qml/ContextMenu.qml
              qml/DashboardConfigurationView.qml
              qml/DashboardView.qml
              qml/ImprintView.qml
              qml/NodeAttributeList.qml
              qml/NodeDetailView.qml
              qml/NodeReferenceList.qml
              qml/NodesView.qml
              qml/SettingsView.qml
              qml/controls/SideMenu.qml
              qml/controls/StyledButton.qml
              qml/controls/StyledComboBox.qml
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
              qml/style/ThemeListView.qml
              qml/style/ThemeMainWindow.qml
              qml/style/ThemeNodesView.qml
              qml/style/ThemeScrollBar.qml
              qml/style/ThemeSettingsView.qml
              qml/style/ThemeSideMenu.qml
              qml/style/ThemeTabButton.qml
              qml/style/ThemeTextField.qml
    SOURCES attribute.h attribute.cpp
    SOURCES attributemodel.h attributemodel.cpp
    SOURCES backend.h backend.cpp
    SOURCES dashboarditem.h dashboarditem.cpp
    SOURCES dashboarditemmodel.h dashboarditemmodel.cpp
    SOURCES monitoreditem.h monitoreditem.cpp
    SOURCES monitoreditemmodel.h monitoreditemmodel.cpp
    SOURCES opcuahelper.h opcuahelper.cpp
    SOURCES opcuamodel.h opcuamodel.cpp
    SOURCES reference.h reference.cpp
    SOURCES referencemodel.h referencemodel.cpp
    SOURCES treeitem.h treeitem.cpp
    SOURCES types.h
    SOURCES uisettings.h uisettings.cpp
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
          icons/logo_basyskom.svg
          icons/menu.svg
          icons/plus.svg
          icons/refresh.svg
          icons/save.svg
          icons/settings.svg)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appOPC_UA_Browser PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appOPC_UA_Browser
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appOPC_UA_Browser
    PRIVATE Qt6::Core Qt6::Quick Qt6::QuickControls2 Qt::OpcUa
)

include(GNUInstallDirs)
install(TARGETS appOPC_UA_Browser
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
