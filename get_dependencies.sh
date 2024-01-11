#!/bin/bash
QT_VERSION=6.6.1
mkdir -p dependencies
git clone --depth 1 --branch master https://github.com/KDAB/android_openssl.git dependencies/android_openssl
git clone --depth 1 --branch v${QT_VERSION} https://github.com/qt/qtopcua.git dependencies/qtopcua
