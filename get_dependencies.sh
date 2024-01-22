#!/bin/bash

# SPDX-FileCopyrightText: 2024 basysKom GmbH
# SPDX-FileContributor: Arno Krapf <arno.krapf@basyskom.com>
# SPDX-License-Identifier: BSD-3-Clause

QT_VERSION=6.6.1
mkdir -p dependencies
git clone --depth 1 --branch master https://github.com/KDAB/android_openssl.git dependencies/android_openssl
git clone --depth 1 --branch v${QT_VERSION} https://github.com/qt/qtopcua.git dependencies/qtopcua
