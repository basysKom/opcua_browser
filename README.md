<!--
// SPDX-FileCopyrightText: 2024 basysKom GmbH
// SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
//
// SPDX-License-Identifier: CC0-1.0
-->
# OPC UA Browser

![Provided](https://img.shields.io/badge/provided%20by-basysKom%20GmbH-orange)

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

coming soon in Google Play store

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
To be done

### Android
To be done

### iOS
To be done

## Contributing

Contributions are welcome! If you encounter any issues or would like to request a feature, feel free to open an issue or submit a pull request.

## License

This project is released under the GPLv3.0-or-later License.
