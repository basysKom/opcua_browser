# SPDX-FileCopyrightText: 2024 basysKom GmbH
# SPDX-FileContributor: Kai Uwe Broulik <kai.uwe.broulik@basyskom.com>
# SPDX-License-Identifier: BSD-3-Clause

qt6_add_qml_module(quickcontrols_module
    STATIC
    URI com.basyskom.quickcontrols
    VERSION 1.0
    QML_FILES
        Button.qml
        CheckBox.qml
        CheckIndicator.qml
        MenuItem.qml
        MenuSeparator.qml
        Popup.qml
        ScrollBar.qml
        SpinBox.qml
        TabButton.qml
        TextField.qml
    IMPORTS
        QtQuick.Controls.Fusion
)
