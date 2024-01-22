/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

StyledComboBox {
    id: comboBox

    comboBoxColumnHeight: _comboBox.contentItem.implicitHeight

    function getDisplayTextPart(str, part) {
        var parts = str.split("#")
        if (part >= parts.length) {
            return ""
        }

        return parts[part]
    }

    _comboBox.contentItem: Column {
        topPadding: 6
        bottomPadding: topPadding
        leftPadding: 12
        Text {
            font.pointSize: 12
            text: comboBox.getDisplayTextPart(comboBox._comboBox.displayText, 0)
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            leftPadding: 12
            font.pointSize: 10
            text: comboBox.getDisplayTextPart(comboBox._comboBox.displayText, 1)
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            leftPadding: 12
            font.pointSize: 10
            text: comboBox.getDisplayTextPart(comboBox._comboBox.displayText, 2)
            verticalAlignment: Text.AlignVCenter
        }
    }

    _comboBox.delegate: ItemDelegate {
        id: delegate
        topPadding: 6
        bottomPadding: topPadding

        required property var model
        required property int index

        property string delegateText: model[comboBox._comboBox.textRole]

        width: comboBox._comboBox.width
        highlighted: comboBox._comboBox.highlightedIndex === delegate.index

        contentItem: Column {
            Text {
                font.pointSize: 12
                text: comboBox.getDisplayTextPart(delegate.delegateText, 0)
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                leftPadding: 12
                font.pointSize: 10
                text: comboBox.getDisplayTextPart(delegate.delegateText, 1)
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                leftPadding: 12
                font.pointSize: 10
                text: comboBox.getDisplayTextPart(delegate.delegateText, 2)
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
