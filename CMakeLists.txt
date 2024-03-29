#[[
SPDX-FileCopyrightText: 2024 basysKom GmbH
SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
SPDX-License-Identifier: BSD-3-Clause
]]

cmake_minimum_required(VERSION 3.19)
include(FetchContent)

project(OPC_UA_Browser
    VERSION 1.0.0
    DESCRIPTION "OPC UA Browser"
    HOMEPAGE_URL "https://github.com/basysKom/opcua_browser"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Quick QuickControls2 OpcUa)

if(QT_KNOWN_POLICY_QTP0002)
    qt_policy(SET QTP0002 NEW)
endif()

qt_standard_project_setup(REQUIRES 6.5)
add_compile_definitions(QT_DISABLE_DEPRECATED_UP_TO=0x060500)

add_subdirectory(src)

if (ANDROID)
    include(dependencies/android_openssl/android_openssl.cmake)
    add_android_openssl_libraries(appOPC_UA_Browser)
    set_property(TARGET appOPC_UA_Browser APPEND PROPERTY QT_ANDROID_VERSION_CODE ${ANDROID_VERSION_CODE})
    set_property(TARGET appOPC_UA_Browser APPEND PROPERTY QT_ANDROID_VERSION_NAME ${ANDROID_VERSION_NAME})
endif()
