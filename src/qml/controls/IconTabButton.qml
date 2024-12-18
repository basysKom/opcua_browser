/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls.impl // IconImage
import QtQuick.Layouts
import com.basyskom.quickcontrols as QQC

import OPC_UA_Browser

QQC.TabButton {
    id: control

    property int type: DashboardItem.DashboardType.Variables

    property alias showActivityIndicator: activityIndicator.visible

    width: 80

    background: null

    contentItem: ColumnLayout {
        width: control.width
        spacing: 5

        IconImage {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            sourceSize.width: 40
            sourceSize.height: 40
            source: (DashboardItem.DashboardType.Add === control.type) ? "qrc:/icons/plus.svg" :
                    (DashboardItem.DashboardType.Events === control.type) ? "qrc:/icons/bolt.svg" : "qrc:/icons/dashboard.svg"
            color: label.color

            Rectangle {
                id: activityIndicator
                width: 10
                height: 10
                radius: 5

                color: palette.highlight

                anchors.right: parent.right
                anchors.top: parent.top
            }
        }

        QQC.Label {
            id: label

            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 8
            text: control.text
            color: control.isCurrentTab ? palette.highlight : palette.windowText
            elide: Text.ElideRight
        }
    }
}
