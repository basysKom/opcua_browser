cmake_minimum_required(VERSION 3.16)

project(OPC_UA_Browser VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QML_IMPORT_PATH ${CMAKE_SOURCE_DIR}/qml ${CMAKE_BINARY_DIR}/imports CACHE STRING "" FORCE)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick QuickControls2 OpcUa)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appOPC_UA_Browser
    main.cpp
)

qt_add_qml_module(appOPC_UA_Browser
    URI OPC_UA_Browser
    VERSION 1.0
    QML_FILES Main.qml
              BrowserView.qml
              ConnectionView.qml
              ContentView.qml
              ContextMenu.qml
              DashboardView.qml
              NodeAttributeList.qml
              NodeDetailView.qml
              NodeReferenceList.qml
              NodesView.qml
              StyledScrollBar.qml
              StyledTabButton.qml
    SOURCES attribute.h attribute.cpp
    SOURCES attributemodel.h attributemodel.cpp
    SOURCES backend.h backend.cpp
    SOURCES monitoreditemmodel.h monitoreditemmodel.cpp
    SOURCES opcuahelper.h opcuahelper.cpp
    SOURCES opcuamodel.h opcuamodel.cpp
    SOURCES reference.h reference.cpp
    SOURCES referencemodel.h referencemodel.cpp
    SOURCES treeitem.h treeitem.cpp
)

qt_add_resources(appOPC_UA_Browser "icons"
    PREFIX "/"
    FILES icons/delete.png
          icons/eye.png
          icons/refresh.png)

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
    PRIVATE Qt6::Quick Qt6::QuickControls2 Qt::OpcUa
)

include(GNUInstallDirs)
install(TARGETS appOPC_UA_Browser
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
