/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import com.basyskom.quickcontrols as QQC

LabelledComboBox {
    id: comboBox

    comboBoxColumnHeight: _comboBox.contentItem.implicitHeight

    readonly property string currentEndpointUrl: getDisplayTextPart(comboBox._comboBox.displayText, 3)

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

        QQC.Label {
            font.pointSize: 12
            text: comboBox.getDisplayTextPart(comboBox._comboBox.displayText, 0)
            verticalAlignment: Text.AlignVCenter
        }

        QQC.Label {
            leftPadding: 12
            font.pointSize: 10
            text: comboBox.getDisplayTextPart(comboBox._comboBox.displayText, 1)
            verticalAlignment: Text.AlignVCenter
        }

        QQC.Label {
            leftPadding: 12
            font.pointSize: 10
            text: comboBox.getDisplayTextPart(comboBox._comboBox.displayText, 2)
        }
    }

    _comboBox.delegate: QQC.MenuItem {
        id: delegate
        topPadding: 6
        bottomPadding: topPadding
        enabled: comboBox.getDisplayTextPart(delegate.delegateText, 3).startsWith("opc.tcp://")

        required property var model
        required property int index

        readonly property string delegateText: model[comboBox._comboBox.textRole]
        readonly property string textColor: delegate.highlighted ? comboBox.palette.highlightedText : comboBox.palette.windowText

        width: ListView.view.width
        font.weight: comboBox._comboBox.currentIndex === index ? Font.DemiBold : Font.Normal
        highlighted: comboBox._comboBox.highlightedIndex === index
        hoverEnabled: comboBox._comboBox.hoverEnabled

        height: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                         implicitContentHeight + topPadding + bottomPadding,
                         implicitIndicatorHeight + topPadding + bottomPadding)

        contentItem: Column {
            QQC.Label {
                font.pointSize: 12
                text: comboBox.getDisplayTextPart(delegate.delegateText, 0)
                verticalAlignment: Text.AlignVCenter
                color: delegate.textColor
            }

            QQC.Label {
                leftPadding: 12
                font.pointSize: 10
                text: comboBox.getDisplayTextPart(delegate.delegateText, 1)
                verticalAlignment: Text.AlignVCenter
                color: delegate.textColor
            }

            QQC.Label {
                leftPadding: 12
                font.pointSize: 10
                text: comboBox.getDisplayTextPart(delegate.delegateText, 2)
                verticalAlignment: Text.AlignVCenter
                color: delegate.textColor
            }

            QQC.Label {
                leftPadding: 12
                font.pointSize: 10
                text: comboBox.getDisplayTextPart(delegate.delegateText, 3)
                verticalAlignment: Text.AlignVCenter
                color: delegate.textColor
            }
        }

        background: Rectangle {
            implicitWidth: 200
            implicitHeight: 20

            color: delegate.palette.highlight
            visible: delegate.down || delegate.highlighted
        }
    }
}
