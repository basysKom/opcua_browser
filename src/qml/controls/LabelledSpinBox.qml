/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import com.basyskom.quickcontrols as QQC

ColumnLayout {
    id: layout

    property alias captionText: caption.text
    property alias from: spinBox.from
    property alias to: spinBox.to
    property alias value: spinBox.value

    QQC.Label {
        id: caption

        verticalAlignment: Qt.AlignVCenter
        font.bold: true
    }

    QQC.SpinBox {
        id: spinBox
        height: 30

        from: spinBox.from
        to: spinBox.to
        value: spinBox.value
    }
}
