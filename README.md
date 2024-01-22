<!--
// SPDX-FileCopyrightText: 2024 basysKom GmbH
// SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
//
// SPDX-License-Identifier: CC0-1.0
-->
# OPC UA Browser

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

- Android using clang for arm64-v8a, armeabi-v7a, x86, x86_64

- iOS

## Dependencies

- Qt 6.5+
- Qt OPC UA (available at [https://doc.qt.io/qt-6/qtopcua-index.html))
- OpenSSL >= 3.0

## Building

To be done

## Contributing

Contributions are welcome! If you encounter any issues or would like to request a feature, feel free to open an issue or submit a pull request.

## License

This project is released under the GPLv3.0-or-later License.