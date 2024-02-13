<!--
// SPDX-FileCopyrightText: 2024 basysKom GmbH
// SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
//
// SPDX-License-Identifier: CC0-1.0
-->
# OPC UA Browser

[![Provided](https://img.shields.io/badge/provided%20by-basysKom%20GmbH-orange)](https://www.basyskom.de/)

![Contributors](https://img.shields.io/github/contributors/basysKom/opcua_browser?style=plastic)
![GitHub Downloads (all assets, latest release)](https://img.shields.io/github/downloads-pre/basysKom/opcua_browser/latest/total)
![Watchers](https://img.shields.io/github/watchers/basysKom/opcua_browser)
![Forks](https://img.shields.io/github/forks/basysKom/opcua_browser)
![Stars](https://img.shields.io/github/stars/basysKom/opcua_browser)
![Licence](https://img.shields.io/github/license/basysKom/opcua_browser)
![Issues](https://img.shields.io/github/issues/basysKom/opcua_browser)

The OPC UA Browser from basysKom is an OPC UA client with a wide range of application options for OPC UA technology.

The OPC UA Browser can be used to establish an unencrypted connection to an OPC UA server as well as a connection encrypted via certificates. User authentication via user name and password is also supported.

The OPC UA Browser can be used to monitor selected nodes in order to obtain a quick overview of important components. Individual nodes can be easily added to and removed from the dashboard. It is possible to create several dashboards at the same time and quickly switch back and forth between them. The dashboards created can be easily saved so that they can be quickly reloaded in a new session.

The information model of the OPC UA server is displayed as a tree structure using the expert mode. Selecting a node displays its attributes and references. By clicking on a reference, you can quickly jump to the selected reference in the tree structure.

## Features

- Connecting to OPC UA Browser (uncrypted and encrypted via certificates, user authentication)
- Monitoring nodes in several dashboards
- Saving dashboards for reuse
- Browser for information model
- Windows, Linux, Android, iOS

## Getting OPC UA Browser

### Android

Google Play Store

[![QR code Play Store](src/android/qr_code-playstore.png)](https://play.google.com/store/apps/details?id=com.basyskom.opcua.app)

### iOS

coming soon in App Store

## Compatibility

The application has been tested on:
- Windows using the MinGW 64-Bit compiler and Qt 6.5+
- Linux (Ubuntu 22.04 LTS) and Qt 6.5+
- Android using Clang for arm64-v8a, armeabi-v7a, x86, x86_64
- iOS

## Dependencies

- Qt 6.5+
- Qt OPC UA (available at https://doc.qt.io/qt-6/qtopcua-index.html)
- OpenSSL >= 3.0

## Building

> [!IMPORTANT]
> Make sure that the versions of Qt and Qt OPC UA Plugin are identical.

### Linux
#### Install Qt OPC UA Plugin
```
git clone https://code.qt.io/qt/qtopcua.git
cd qtopcua
git checkout 6.6.1
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..
```
Check whether "Open62541 security support" and "Support for global discovery server" is activated. Otherwise, OpenSSL may need to be installed or the path to the OpenSSL library may need to be added to the path variable.

```
-- Configure summary:
Qt Opcua:
  Open62541 .............................. yes
  Unified Automation C++ SDK ............. no
  Support for namespace 0 NodeId names ... yes
  Namespace 0 NodeIds generator .......... no
  Open62541 security support ............. yes
  Support for global discovery server .... yes
```
Build OPC UA Plugin
```
ninja
```
Optional: Install the libraries in the Qt directory. Otherwise, the directory for the libraries of the Qt OPC UA Plugin must be passed to the build process of the OPC UA browser.  
```
ninja install
```

#### Build OPC UA Browser

```
git clone https://github.com/basysKom/opcua_browser.git
cd opcua_browser
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..
ninja
```

### Windows

#### Install Qt OPC UA Plugin
```
git clone https://code.qt.io/qt/qtopcua.git
cd qtopcua
git checkout 6.6.1
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DOPENSSL_ROOT_DIR=C:\Qt\Tools\OpenSSLv3\Win_x64 ..
```
Check whether "Open62541 security support" and "Support for global discovery server" is activated. Otherwise, OpenSSL may need to be installed or the path to the OpenSSL library may need to be added to the path variable.

```
-- Configure summary:
Qt Opcua:
  Open62541 .............................. yes
  Unified Automation C++ SDK ............. no
  Support for namespace 0 NodeId names ... yes
  Namespace 0 NodeIds generator .......... no
  Open62541 security support ............. yes
  Support for global discovery server .... yes
```
Build OPC UA Plugin
```
mingw32-make
```
Optional: Install the libraries in the Qt directory. Otherwise, the directory for the libraries of the Qt OPC UA Plugin must be passed to the build process of the OPC UA browser.  
```
mingw32-make install
```

#### Build OPC UA Browser

```
git clone https://github.com/basysKom/opcua_browser.git
cd opcua_browser
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DOPENSSL_ROOT_DIR=C:\Qt\Tools\OpenSSLv3\Win_x64 ..
mingw32-make
```

### Android
A cross compiler under Linux or Windows can be used to build for Android. However, this section describes how to create an APK using a Docker container.
We use a Docker container from the following GitHub repository:
https://github.com/state-of-the-art/qt6-docker

To use Docker, the Docker library must be installed.
```
sudo apt-get install docker.io
```

This build example is written for an APK build for the architecture arm64_v8a with Qt version 6.6.1.
If you are using a different version, you must change the Qt version for Qt OPC UA in the file getDependencies.sh and the CMAKE_INSTALL_PREFIX in the build example.
To build an APK for a different architecture, you must change the CMAKE_INSTALL_PREFIX and the OPENSSL_ROOT_DIR in the build example.
| Architecture  | CMAKE_INSTALL_PREFIX            | OPENSSL_ROOT_DIR                                       |
| ------------- | ------------------------------- | ------------------------------------------------------ |
| arm64_v8a     | /opt/Qt/6.6.1/android_arm64_v8a | ${PWD}/dependencies/android_openssl/ssl_3/arm64-v8a/   |
| armeabi_v7a   | /opt/Qt/6.6.1/android_armv7     | ${PWD}/dependencies/android_openssl/ssl_3/armeabi-v7a/ |
| x86           | /opt/Qt/6.6.1/android_x86       | ${PWD}/dependencies/android_openssl/ssl_3/x86/         |
| x86_64        | /opt/Qt/6.6.1/android_x86_64    | ${PWD}/dependencies/android_openssl/ssl_3/x86_64/      |

```
git clone https://github.com/basysKom/opcua_browser.git
mkdir build_opcua_browser
cd opcua_browser
./get_dependencies.sh
sudo docker run -it --rm -v "${PWD}:/home/user/project:ro" -v "${PWD}/../build_opcua_browser:/home/user/build:rw" stateoftheartio/qt6:6.6-android-aqt \
    sh -c 'rm -rf build/* && \
           cd project && \
           qt-cmake ./dependencies/qtopcua -G Ninja -B ../build/qtopcua -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/opt/Qt/6.6.1/android_arm64_v8a \
           	-DOPENSSL_INCLUDE_DIR=${PWD}/dependencies/android_openssl/ssl_3/include/ -DOPENSSL_ROOT_DIR=${PWD}/dependencies/android_openssl/ssl_3/arm64-v8a/ && \
           cmake --build ../build/qtopcua && \
           sudo /opt/Qt/Tools/CMake/bin/cmake --install ../build/qtopcua && \
           rm -rf ../build/qtopcua && \
           mkdir -p ../build/opcua_browser && \
           qt-cmake . -G Ninja -B ../build/opcua_browser -DCMAKE_BUILD_TYPE=Debug && \
           cmake --build ../build/opcua_browser'
```

### iOS
To be done

## basysKom Toolbox
The basysKom Toolbox is a state-of-the-art collection of best practices in Agile project management and software development. The toolbox is intended to be used during your kickoff meeting and is available in form of a card game. The cards allow the team to quickly select appropriate approaches and tools right at the beginning of the project, but they also serve as support during the project's execution, for self-reflection, and as a reminder.

You can take a look at the cards on the [basysKom homepage](https://www.basyskom.de/en/toolbox/).

You can find the texts of the cards also directly on [GitHub](https://github.com/basyskom/basyskom_toolbox).

We use the following toolbox cards for this project

[![Avoid deprecate Qt](https://img.shields.io/badge/Avoid%20deprecate%20Qt-orange)](https://www.basyskom.de/en/basyskom/toolbox/avoid-deprecated-qt/)
[![CI keeps your project healty](https://img.shields.io/badge/CI%20keeps%20your%20project%20healty-gray)](https://www.basyskom.de/en/basyskom/toolbox/ci/)
[![Do regular tests on the target](https://img.shields.io/badge/Do%20regular%20tests%20on%20the%20target-orange)](https://www.basyskom.de/en/basyskom/toolbox/test-on-target/)
[![Enable build time checks for your QML files](https://img.shields.io/badge/Enable%20build%20time%20checks%20for%20your%20QML%20files-gray)](https://www.basyskom.de/en/basyskom/toolbox/build-qml/)
[![Make good use of compiler warnings](https://img.shields.io/badge/Make%20good%20use%20of%20compiler%20warnings-orange)](https://www.basyskom.de/en/basyskom/toolbox/use-warnings/)
[![Implement QML Best Practices](https://img.shields.io/badge/Implement%20QML%20Best%20Practices-gray)](https://www.basyskom.de/en/basyskom/toolbox/qml-best-practices/)
[![Proper automated builds](https://img.shields.io/badge/Proper%20automated%20builds-orange)](https://www.basyskom.de/en/basyskom/toolbox/automated-builds/)
[![Use the AdressSanitizer](https://img.shields.io/badge/Use%20the%20AdressSanitizer-gray)](https://www.basyskom.de/en/basyskom/toolbox/AdressSanitizer/)
[![Use clazy](https://img.shields.io/badge/Use%20clazy-orange)](https://www.basyskom.de/en/basyskom/toolbox/clazy/)
[![Valgrind or the MemorySanitizer help to ship reliable software](https://img.shields.io/badge/Valgrind%20or%20the%20MemorySanitizer%20help%20to%20ship%20reliable%20software-gray)](https://www.basyskom.de/en/basyskom/toolbox/valgrind-memorysanitizer/)

## Contributing

Contributions are welcome! If you encounter any issues or would like to request a feature, feel free to open an issue or submit a pull request.

## License

This project is released under the GPLv3.0-or-later License.