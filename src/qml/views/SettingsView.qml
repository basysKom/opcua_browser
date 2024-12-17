/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.impl // IconImage
import QtQuick.Layouts
import com.basyskom.quickcontrols as QQC

import OPC_UA_Browser

Rectangle {
    id: view

    function setTheme(index) {
        appWindow.themeIndex = index
    }

    readonly property int settingsMargin: 10
    color: palette.window

    LanguageItemModel {
        id: languageModel
    }

    Flickable {
        anchors.fill: parent
        anchors.bottomMargin: 10
        clip: true

        contentWidth: parent.width
        contentHeight: content.childrenRect.height

        boundsBehavior: Flickable.StopAtBounds
        boundsMovement: Flickable.StopAtBounds

        QQC.ScrollBar.vertical: QQC.ScrollBar {
            policy: QQC.ScrollBar.AsNeeded
        }

        Column {
            id: content
            anchors.fill: parent

            leftPadding: 10
            rightPadding: leftPadding
            spacing: 10

            Text {
                color: view.palette.windowText
                font {
                    pointSize: 18
                    bold: true
                }
                text: qsTranslate("General", "Settings")
            }

            // Theme
            Column {
                width: parent.width - content.leftPadding - content.rightPadding
                spacing: 5

                Text {
                    color: view.palette.windowText
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("General", "Theme")
                }

                QQC.ButtonGroup {
                    buttons: buttonRow.children
                }

                Row {
                    anchors.leftMargin: view.settingsMargin
                    anchors.left: parent.left

                    spacing: 25
                    id: buttonRow

                    QQC.CheckBox {
                        id: darkItemSelector

                        checkState: (appWindow.themeIndex === 0) ? Qt.Checked : Qt.Unchecked
                        onToggled: view.setTheme(0)
                        text: qsTranslate("General", "Dark")
                    }

                    QQC.CheckBox {
                        id: brightItemSelector

                        checkState: (appWindow.themeIndex === 1) ? Qt.Checked : Qt.Unchecked
                        onToggled: view.setTheme(1)
                        text: qsTranslate("General", "Bright")
                    }
                }
            }

            // Language list view
            Column {
                width: parent.width - content.leftPadding - content.rightPadding
                spacing: 5

                Text {
                    color: view.palette.windowText
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Settings", "Language")
                }

                Rectangle {
                    anchors.leftMargin: view.settingsMargin
                    anchors.left: parent.left
                    anchors.right: parent.right

                    height: childrenRect.height

                    color: view.palette.light
                    radius: 5

                    ListView {
                        id: languageListView

                        width: parent.width
                        height: Math.min(200, contentHeight)

                        clip: true

                        model: languageModel
                        boundsBehavior: Flickable.StopAtBounds
                        boundsMovement: Flickable.StopAtBounds

                        QQC.ScrollBar.vertical: QQC.ScrollBar {
                            policy: QQC.ScrollBar.AsNeeded
                        }

                        delegate: Rectangle {
                            id: languageListViewDelegate

                            required property int index
                            required property bool isCurrentItem
                            required property string displayName
                            required property string flagFilename

                            radius: 5
                            width: languageListView.width
                            implicitHeight: childrenRect.height
                            color: isCurrentItem ? view.palette.highlight : view.palette.light
                            clip: true

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: languageModel.setCurrentIndex(languageListViewDelegate.index)
                            }

                            RowLayout {
                                width: parent.width
                                height: 48
                                spacing: 10

                                Image {
                                    Layout.alignment: Qt.AlignVCenter
                                    Layout.leftMargin: 5
                                    Layout.preferredWidth: 32
                                    Layout.preferredHeight: 32
                                    source: languageListViewDelegate.flagFilename
                                    fillMode: Image.PreserveAspectFit
                                }

                                Text {
                                    Layout.fillWidth: true
                                    Layout.rightMargin: 5
                                    font.pointSize: 14
                                    text: languageListViewDelegate.displayName
                                    color: languageListViewDelegate.isCurrentItem ? view.palette.highlightedText :
                                                                                    view.palette.windowText
                                    elide: Text.ElideRight
                                }
                            }
                        }
                    }
                }
            }

            // Saved variable dashboards list view
            Column {
                width: parent.width - content.leftPadding - content.rightPadding
                spacing: 5

                Text {
                    color: view.palette.windowText
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Dashboard", "Saved variable dashboards")
                }

                Rectangle {
                    anchors.leftMargin: view.settingsMargin
                    anchors.left: parent.left
                    anchors.right: parent.right

                    height: childrenRect.height

                    color: view.palette.light
                    radius: 5

                    ListView {
                        id: variableDashboardsListView

                        width: parent.width
                        height: Math.min(200, contentHeight)

                        clip: true

                        model: BackEnd.savedVariableDashboards

                        boundsBehavior: Flickable.StopAtBounds
                        boundsMovement: Flickable.StopAtBounds

                        QQC.ScrollBar.vertical: QQC.ScrollBar {
                            policy: QQC.ScrollBar.AsNeeded
                        }

                        delegate: Rectangle {
                            id: variableDashboardsListViewDelegate

                            required property int index
                            required property string display

                            radius: 5
                            width: variableDashboardsListView.width
                            implicitHeight: childrenRect.height
                            color: view.palette.light
                            clip: true

                            RowLayout {
                                width: parent.width
                                height: 30
                                spacing: 10

                                Text {
                                    id: dashboardName
                                    Layout.fillWidth: true
                                    Layout.rightMargin: 5
                                    Layout.leftMargin: 5
                                    font {
                                        pointSize: 11
                                    }
                                    text: display
                                    color: view.palette.windowText
                                    elide: Text.ElideRight
                                }

                                IconImage {
                                    Layout.alignment: Qt.AlignVCenter
                                    sourceSize.width: 24
                                    sourceSize.height: 24
                                    source: "qrc:/icons/edit.svg"
                                    color: view.palette.windowText

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: function() {
                                            dashboardNameEditPopup.showEdit(display, false)
                                        }
                                    }
                                }

                                IconImage {
                                    Layout.alignment: Qt.AlignVCenter
                                    Layout.rightMargin: 10
                                    sourceSize.width: 24
                                    sourceSize.height: 24
                                    source: "qrc:/icons/delete.svg"
                                    color: view.palette.windowText

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: function() {
                                            BackEnd.removeSavedVariableDashboard(display)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Saved event dashboards list view
            Column {
                width: parent.width - content.leftPadding - content.rightPadding
                spacing: 5

                Text {
                    color: view.palette.windowText
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Dashboard", "Saved event dashboards")
                }

                Rectangle {
                    anchors.leftMargin: view.settingsMargin
                    anchors.left: parent.left
                    anchors.right: parent.right

                    height: childrenRect.height

                    color: view.palette.light
                    radius: 5

                    ListView {
                        id: eventDashboardsListView

                        width: parent.width
                        height: Math.min(200, contentHeight)

                        clip: true

                        model: BackEnd.savedEventDashboards

                        boundsBehavior: Flickable.StopAtBounds
                        boundsMovement: Flickable.StopAtBounds

                        QQC.ScrollBar.vertical: QQC.ScrollBar {
                            policy: QQC.ScrollBar.AsNeeded
                        }

                        delegate: Rectangle {
                            id: eventDashboardsListViewDelegate

                            required property int index
                            required property string display

                            radius: 5
                            width: eventDashboardsListView.width
                            implicitHeight: childrenRect.height
                            color: view.palette.light
                            clip: true

                            RowLayout {
                                width: parent.width
                                height: 30
                                spacing: 10

                                Text {
                                    id: eventDashboardName
                                    Layout.fillWidth: true
                                    Layout.rightMargin: 5
                                    Layout.leftMargin: 5
                                    font {
                                        pointSize: 11
                                    }
                                    text: display
                                    color: view.palette.windowText
                                    elide: Text.ElideRight
                                }

                                IconImage {
                                    Layout.alignment: Qt.AlignVCenter
                                    sourceSize.width: 24
                                    sourceSize.height: 24
                                    source: "qrc:/icons/edit.svg"
                                    color: view.palette.windowText

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: function() {
                                            dashboardNameEditPopup.showEdit(display, true)
                                        }
                                    }
                                }

                                IconImage {
                                    Layout.alignment: Qt.AlignVCenter
                                    Layout.rightMargin: 10
                                    sourceSize.width: 24
                                    sourceSize.height: 24
                                    source: "qrc:/icons/delete.svg"
                                    color: view.palette.windowText

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: function() {
                                            BackEnd.removeSavedEventDashboard(display)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Recent connections list view
            Column {
                width: parent.width - content.leftPadding - content.rightPadding
                spacing: 5

                Text {
                    color: view.palette.windowText
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Connection", "Recent connections")
                }

                Rectangle {
                    anchors.leftMargin: view.settingsMargin
                    anchors.left: parent.left
                    anchors.right: parent.right

                    height: childrenRect.height

                    color: view.palette.light
                    radius: 5

                    ListView {
                        id: recentConnectionsListView

                        width: parent.width
                        height: Math.min(200, contentHeight)

                        clip: true

                        model: BackEnd.recentConnections

                        boundsBehavior: Flickable.StopAtBounds
                        boundsMovement: Flickable.StopAtBounds

                        QQC.ScrollBar.vertical: QQC.ScrollBar {
                            policy: QQC.ScrollBar.AsNeeded
                        }

                        delegate: Rectangle {
                            id: recentConnectionsListViewDelegate

                            required property int index
                            required property string modelData

                            radius: 5
                            width: recentConnectionsListView.width
                            implicitHeight: childrenRect.height
                            color: view.palette.light
                            clip: true

                            RowLayout {
                                width: parent.width
                                height: 30
                                spacing: 10

                                Text {
                                    Layout.fillWidth: true
                                    Layout.rightMargin: 5
                                    Layout.leftMargin: 5
                                    font {
                                        pointSize: 11
                                    }
                                    text: modelData
                                    color: view.palette.windowText
                                    elide: Text.ElideRight
                                }

                                IconImage {
                                    Layout.alignment: Qt.AlignVCenter
                                    Layout.rightMargin: 10
                                    sourceSize.width: 24
                                    sourceSize.height: 24
                                    source: "qrc:/icons/delete.svg"
                                    color: view.palette.windowText

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: function() {
                                            BackEnd.removeRecentConnection(modelData)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Dashboard settings
            Column {
                width: parent.width - content.leftPadding - content.rightPadding
                spacing: 5

                Text {
                    color: view.palette.windowText
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Dashboard", "Dashboard")
                }

                LabelledSpinBox {
                    anchors.leftMargin: view.settingsMargin
                    anchors.left: parent.left

                    captionText: qsTranslate("Settings", "Max. events per object")
                    from: 1
                    to: 150
                    value: BackEnd.maxEventsPerObject
                    onValueChanged: BackEnd.maxEventsPerObject = value
                }
            }

            // Certificate list view
            Column {
                width: parent.width - content.leftPadding - content.rightPadding
                spacing: 5

                Text {
                    color: view.palette.windowText
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Certificate", "Certificates")
                }

                Text {
                    anchors.leftMargin: view.settingsMargin
                    anchors.left: parent.left

                    color: view.palette.windowText
                    font {
                        pointSize: 12
                        bold: true
                    }
                    text: qsTranslate("Certificate", "Own certificate")
                }

                Rectangle {
                    anchors.leftMargin: view.settingsMargin
                    anchors.left: parent.left
                    anchors.right: parent.right

                    height: childrenRect.height

                    color: view.palette.light
                    radius: 5

                    ListView {
                        id: ownCert

                        width: parent.width
                        height: 265

                        clip: true

                        model: BackEnd.ownCertificateItemModel
                        boundsBehavior: Flickable.StopAtBounds
                        boundsMovement: Flickable.StopAtBounds

                        QQC.ScrollBar.vertical: QQC.ScrollBar {
                            policy: QQC.ScrollBar.AsNeeded
                        }

                        delegate: Rectangle {
                            id: ownCertListViewDelegate

                            component OwnSubitemText : Text {
                                Layout.leftMargin: 5
                                Layout.rightMargin: 5
                                Layout.fillWidth: true
                                verticalAlignment: Qt.AlignVCenter
                                color: view.palette.highlightedText
                            }

                            component OwnSubitemTitle : OwnSubitemText {
                                elide: Qt.ElideRight
                                font {
                                    pointSize: 11
                                    bold: true
                                }
                            }

                            required property int index
                            required property string issuerDisplayName
                            required property date effectiveDate
                            required property string fingerprint
                            required property date expiryDate
                            required property string commonName
                            required property string serialNumber

                            radius: 5
                            width: ownCert.width
                            implicitHeight: delegateLayout.height
                            color: view.palette.highlight
                            clip: true

                            ColumnLayout {
                                id: delegateLayout

                                width: parent.width
                                spacing: 0

                                RowLayout {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 36

                                    Text {
                                        Layout.fillWidth: true
                                        Layout.leftMargin: 5
                                        font.pointSize: 14
                                        text: ownCertListViewDelegate.issuerDisplayName
                                        color: view.palette.highlightedText
                                        elide: Text.ElideRight
                                    }

                                    IconImage {
                                        Layout.alignment: Qt.AlignVCenter
                                        Layout.rightMargin: 10
                                        sourceSize.width: 24
                                        sourceSize.height: 24
                                        source: "qrc:/icons/refresh.svg"
                                        color: view.palette.highlightedText

                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: BackEnd.regenerateOwnCertificate()
                                        }
                                    }
                                }

                                OwnSubitemTitle {
                                    text: qsTranslate("Certificate", "Valid from")
                                }

                                OwnSubitemText {
                                    text: ownCertListViewDelegate.effectiveDate.toLocaleString(Qt.locale(), qsTranslate("General", "MM/dd/yyyy"))
                                }

                                OwnSubitemTitle {
                                    text: qsTranslate("Certificate", "Valid to")
                                }

                                OwnSubitemText {
                                    text: ownCertListViewDelegate.expiryDate.toLocaleString(Qt.locale(), qsTranslate("General", "MM/dd/yyyy"))
                                }

                                OwnSubitemTitle {
                                    text: qsTranslate("Certificate", "Fingerprint (SHA-256)")
                                }

                                OwnSubitemText {
                                    text: ownCertListViewDelegate.fingerprint
                                    wrapMode: Text.Wrap
                                }

                                OwnSubitemTitle {
                                    text: qsTranslate("Certificate", "Common name")
                                }

                                OwnSubitemText {
                                    text: ownCertListViewDelegate.commonName
                                }

                                OwnSubitemTitle {
                                    text: qsTranslate("Certificate", "Serial number")
                                }

                                OwnSubitemText {
                                    Layout.bottomMargin: 5
                                    text: ownCertListViewDelegate.serialNumber
                                    wrapMode: Text.Wrap
                                }
                            }
                        }
                    }
                }

                Text {
                    anchors.leftMargin: view.settingsMargin
                    anchors.left: parent.left

                    color: view.palette.windowText
                    font {
                        pointSize: 12
                        bold: true
                    }
                    text: qsTranslate("Certificate", "Trusted certificates")
                }

                Rectangle {
                    anchors.leftMargin: view.settingsMargin
                    anchors.left: parent.left
                    anchors.right: parent.right

                    height: childrenRect.height

                    color: view.palette.light
                    radius: 5

                    ListView {
                        id: certificateListView

                        width: parent.width
                        height: 400

                        clip: true

                        model: BackEnd.certificateItemModel
                        boundsBehavior: Flickable.StopAtBounds
                        boundsMovement: Flickable.StopAtBounds

                        QQC.ScrollBar.vertical: QQC.ScrollBar {
                            policy: QQC.ScrollBar.AsNeeded
                        }

                        delegate: Rectangle {
                            id: listViewDelegate

                            component SubitemText : Text {
                                Layout.leftMargin: 5
                                Layout.rightMargin: 5
                                Layout.fillWidth: true
                                visible: listViewDelegate.isCurrentItem
                                verticalAlignment: Qt.AlignVCenter
                                color: listViewDelegate.isCurrentItem ? view.palette.highlightedText :
                                                                        view.palette.windowText
                            }

                            component SubitemTitle : SubitemText {
                                elide: Qt.ElideRight
                                font {
                                    pointSize: 11
                                    bold: true
                                }
                            }

                            required property int index
                            required property bool isCurrentItem
                            required property string issuerDisplayName
                            required property date effectiveDate
                            required property string fingerprint
                            required property date expiryDate
                            required property string commonName
                            required property string organisation
                            required property string organisationUnit
                            required property string localityName
                            required property string countryName
                            required property string stateOrProvince
                            required property string serialNumber
                            required property string version

                            radius: 5
                            width: certificateListView.width
                            implicitHeight: delegateLayout.height
                            color: isCurrentItem ? view.palette.highlight : view.palette.light
                            clip: true

                            Behavior on implicitHeight {
                                NumberAnimation { duration: 100 }
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: BackEnd.certificateItemModel.setCurrentIndex(listViewDelegate.index)
                            }

                            ColumnLayout {
                                id: delegateLayout

                                width: parent.width
                                spacing: 0

                                RowLayout {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 36

                                    Text {
                                        Layout.fillWidth: true
                                        Layout.leftMargin: 5
                                        font.pointSize: 14
                                        text: listViewDelegate.issuerDisplayName
                                        color: listViewDelegate.isCurrentItem ? view.palette.highlightedText :
                                                                                        view.palette.windowText
                                        elide: Text.ElideRight
                                    }

                                    IconImage {
                                        Layout.alignment: Qt.AlignVCenter
                                        Layout.rightMargin: 10
                                        sourceSize.width: 24
                                        sourceSize.height: 24
                                        source: "qrc:/icons/delete.svg"
                                        color: listViewDelegate.isCurrentItem ? view.palette.highlightedText :
                                                                                        view.palette.windowText

                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: BackEnd.certificateItemModel.removeCertificate(listViewDelegate.index)
                                        }
                                    }
                                }

                                SubitemTitle {
                                    text: qsTranslate("Certificate", "Valid from")
                                }

                                SubitemText {
                                    text: listViewDelegate.effectiveDate.toLocaleString(Qt.locale(), qsTranslate("General", "MM/dd/yyyy"))
                                }

                                SubitemTitle {
                                    text: qsTranslate("Certificate", "Valid to")
                                }

                                SubitemText {
                                    text: listViewDelegate.expiryDate.toLocaleString(Qt.locale(), qsTranslate("General", "MM/dd/yyyy"))
                                }

                                SubitemTitle {
                                    text: qsTranslate("Certificate", "Fingerprint (SHA-256)")
                                }

                                SubitemText {
                                    text: listViewDelegate.fingerprint
                                    wrapMode: Text.Wrap
                                }

                                SubitemTitle {
                                    text: qsTranslate("Certificate", "Common name")
                                }

                                SubitemText {
                                    text: listViewDelegate.commonName
                                }

                                /*SubitemTitle {
                                    visible: organisationText.visible
                                    text: qsTranslate("Certificate", "Organization")
                                }

                                SubitemText {
                                    id: organisationText
                                    visible: listViewDelegate.isCurrentItem && text.length > 0
                                    text: listViewDelegate.organisation
                                }

                                SubitemTitle {
                                    visible: organisationUnitText.visible
                                    text: qsTranslate("Certificate", "Organization unit")
                                }

                                SubitemText {
                                    id: organisationUnitText
                                    visible: listViewDelegate.isCurrentItem && text.length > 0
                                    text: listViewDelegate.organisationUnit
                                }

                                SubitemTitle {
                                    visible: localityNameText.visible
                                    text: qsTranslate("Certificate", "Locality")
                                }

                                SubitemText {
                                    id: localityNameText
                                    visible: listViewDelegate.isCurrentItem && text.length > 0
                                    text: listViewDelegate.localityName
                                }

                                SubitemTitle {
                                    visible: countryNameText.visible
                                    text: qsTranslate("Certificate", "Country")
                                }

                                SubitemText {
                                    id: countryNameText
                                    visible: listViewDelegate.isCurrentItem && text.length > 0
                                    text: listViewDelegate.countryName
                                }

                                SubitemTitle {
                                    visible: stateOrProvinceText.visible
                                    text: qsTranslate("Certificate", "State")
                                }

                                SubitemText {
                                    id: stateOrProvinceText
                                    visible: listViewDelegate.isCurrentItem && text.length > 0
                                    text: listViewDelegate.stateOrProvince
                                }

                                SubitemTitle {
                                    text: qsTranslate("Certificate", "Version")
                                }

                                SubitemText {
                                    text: listViewDelegate.version
                                }*/

                                SubitemTitle {
                                    text: qsTranslate("Certificate", "Serial number")
                                }

                                SubitemText {
                                    Layout.bottomMargin: 5
                                    text: listViewDelegate.serialNumber
                                    wrapMode: Text.Wrap
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    QQC.Popup {
        id: dashboardNameEditPopup
        modal: true

        implicitWidth: contentColumn.width
        implicitHeight: contentColumn.height
        padding: 0

        clip: true
        closePolicy: QQC.Popup.NoAutoClose

        property bool isEventDashboard

        anchors.centerIn: parent

        property string previousName

        function showEdit(currentName: string, isEvent: bool) {
            isEventDashboard = isEvent
            previousName = currentName
            nameTextEdit.text = currentName
            nameTextEdit.cursorPosition = currentName.length
            open()
            nameTextEdit.forceActiveFocus()
        }

        background: Rectangle {
            radius: 3
            opacity: 0.8
            color: palette.highlight
        }

        ColumnLayout {
            width: view.width - 50
            id: contentColumn
            Text {
                padding: 3
                font {
                    pointSize: 12
                    bold: true
                }
                color: view.palette.highlightedText
                text: qsTranslate("Settings", "Enter new dashboard name")
            }

            TextEdit {
                Layout.maximumWidth: contentColumn.width
                id: nameTextEdit
                padding: 3
                font {
                    pointSize: 10
                }
                color: view.palette.highlightedText
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                cursorVisible: true
            }

            RowLayout {
                IconImage {
                    Layout.margins: 5
                    Layout.alignment: Qt.AlignVCenter
                    sourceSize.width: 24
                    sourceSize.height: 24
                    source: "qrc:/icons/cancel.svg"
                    color: view.palette.highlightedText

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: function() {
                            dashboardNameEditPopup.close()
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    height: 24
                }

                IconImage {
                    Layout.margins: 5
                    Layout.alignment: Qt.AlignVCenter
                    sourceSize.width: 24
                    sourceSize.height: 24
                    source: "qrc:/icons/checkmark.svg"
                    color: enabled ? view.palette.active.highlightedText : view.palette.disabled.highlightedText
                    enabled: nameTextEdit.text !== "" && ((!dashboardNameEditPopup.isEventDashboard && !BackEnd.hasSavedVariableDashboard(nameTextEdit.text))
                                                          || (dashboardNameEditPopup.isEventDashboard && !BackEnd.hasSavedEventDashboard(nameTextEdit.text)))

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: function() {
                            if (dashboardNameEditPopup.isEventDashboard)
                                BackEnd.renameSavedEventDashboard(dashboardNameEditPopup.previousName, nameTextEdit.text)
                            else
                                BackEnd.renameSavedVariableDashboard(dashboardNameEditPopup.previousName, nameTextEdit.text)
                            dashboardNameEditPopup.close()
                        }
                    }
                }
            }
        }
    }
}
