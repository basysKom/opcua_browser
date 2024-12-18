/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtCore
import QtQuick
import QtQuick.Controls.impl // IconImage
import QtQuick.Layouts
import com.basyskom.quickcontrols as QQC

import OPC_UA_Browser

Rectangle {
    id: view

    property int themeIndex: 0

    readonly property int leftContentMargin: 10
    color: palette.window

    Component.onCompleted: {
        UiSettings.setStatusAndNavigationBarColor(palette.window)
    }

    onThemeIndexChanged: {
        Colors.isDarkMode = (themeIndex == 0)
        UiSettings.setStatusAndNavigationBarColor(palette.window)
    }

    Settings {
        property alias themeIndex: view.themeIndex
    }

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

            QQC.Label {
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

                QQC.Label {
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
                    id: buttonRow

                    anchors.leftMargin: view.leftContentMargin
                    anchors.left: parent.left

                    spacing: 25

                    QQC.CheckBox {
                        id: darkItemSelector

                        checkState: (view.themeIndex == 0) ? Qt.Checked : Qt.Unchecked
                        onToggled: view.themeIndex = 0
                        text: qsTranslate("General", "Dark")
                    }

                    QQC.CheckBox {
                        id: brightItemSelector

                        checkState: (view.themeIndex == 1) ? Qt.Checked : Qt.Unchecked
                        onToggled: view.themeIndex = 1
                        text: qsTranslate("General", "Bright")
                    }
                }
            }

            // Language list view
            SettingsList {
                id: settingsList

                width: parent.width - content.leftPadding - content.rightPadding

                leftContentMargin: view.leftContentMargin
                title: qsTranslate("Settings", "Language")
                listView.model: languageModel

                listView.delegate: Rectangle {
                    id: languageListViewDelegate

                    required property int index
                    required property bool isCurrentItem
                    required property string displayName
                    required property string flagFilename

                    radius: 5
                    width: settingsList.listView.width
                    implicitHeight: childrenRect.height
                    color: isCurrentItem ? palette.highlight : palette.light
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

                        QQC.Label {
                            Layout.fillWidth: true
                            Layout.rightMargin: 5
                            font.pointSize: 14
                            text: languageListViewDelegate.displayName
                            color: languageListViewDelegate.isCurrentItem ? palette.highlightedText :
                                                                            palette.windowText
                            elide: Text.ElideRight
                        }
                    }
                }
            }

            // Saved variable dashboards list view
            SettingsList {
                id: variableDashboardsList

                width: parent.width - content.leftPadding - content.rightPadding

                leftContentMargin: view.leftContentMargin
                title: qsTranslate("Dashboard", "Saved variable dashboards")
                listView.model: BackEnd.savedVariableDashboards

                onEditItemClicked: function(name) {
                    dashboardNameEditPopup.showEdit(name, false)
                }

                onDeleteItemClicked: function(name) {
                    BackEnd.removeSavedVariableDashboard(name)
                }
            }

            // Saved event dashboards list view
            SettingsList {
                id: eventDashboardsList

                width: parent.width - content.leftPadding - content.rightPadding

                leftContentMargin: view.leftContentMargin
                title: qsTranslate("Dashboard", "Saved event dashboards")
                listView.model: BackEnd.savedEventDashboards

                onEditItemClicked: function(name) {
                    dashboardNameEditPopup.showEdit(name, true)
                }

                onDeleteItemClicked: function(name) {
                    BackEnd.removeSavedEventDashboard(name)
                }
            }

            // Recent connections list view
            SettingsList {
                id: recentConnectionsList

                width: parent.width - content.leftPadding - content.rightPadding

                leftContentMargin: view.leftContentMargin
                title: qsTranslate("Connection", "Recent connections")
                listView.model: BackEnd.recentConnections

                listView.delegate: Rectangle {
                    id: recentConnectionsListDelegate

                    required property int index
                    required property string modelData

                    radius: 5
                    width: recentConnectionsList.listView.width
                    implicitHeight: childrenRect.height
                    color: palette.light
                    clip: true

                    RowLayout {
                        width: parent.width
                        height: 30
                        spacing: 10

                        QQC.Label {
                            Layout.fillWidth: true
                            Layout.rightMargin: 5
                            Layout.leftMargin: 5
                            font {
                                pointSize: 11
                            }
                            text: recentConnectionsListDelegate.modelData
                            elide: Text.ElideRight
                        }

                        IconImage {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.rightMargin: 10
                            sourceSize.width: 24
                            sourceSize.height: 24
                            source: "qrc:/icons/delete.svg"
                            color: palette.windowText

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: function() {
                                    BackEnd.removeRecentConnection(recentConnectionsListDelegate.modelData)
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

                QQC.Label {
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Dashboard", "Dashboard")
                }

                LabelledSpinBox {
                    anchors.leftMargin: view.leftContentMargin
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

                QQC.Label {
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Certificate", "Certificates")
                }

                SettingsList {
                    id: ownCertificateList

                    anchors.leftMargin: view.leftContentMargin
                    anchors.left: parent.left

                    width: parent.width - anchors.leftMargin - anchors.rightMargin

                    leftContentMargin: 0
                    title: qsTranslate("Certificate", "Own certificate")
                    titlePointSize: 12
                    listView.model: BackEnd.ownCertificateItemModel
                    listView.height: 265

                    listView.delegate: Rectangle {
                        id: ownCertListViewDelegate

                        component OwnSubitemText : QQC.Label {
                            Layout.leftMargin: 5
                            Layout.rightMargin: 5
                            Layout.fillWidth: true
                            verticalAlignment: Qt.AlignVCenter
                            color: palette.highlightedText
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
                        width: ownCertificateList.listView.width
                        implicitHeight: ownCertificateDelegateLayout.height
                        color: palette.highlight
                        clip: true

                        ColumnLayout {
                            id: ownCertificateDelegateLayout

                            width: parent.width
                            spacing: 0

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 36

                                QQC.Label {
                                    Layout.fillWidth: true
                                    Layout.leftMargin: 5
                                    font.pointSize: 14
                                    text: ownCertListViewDelegate.issuerDisplayName
                                    color: palette.highlightedText
                                    elide: Text.ElideRight
                                }

                                IconImage {
                                    Layout.alignment: Qt.AlignVCenter
                                    Layout.rightMargin: 10
                                    sourceSize.width: 24
                                    sourceSize.height: 24
                                    source: "qrc:/icons/refresh.svg"
                                    color: palette.highlightedText

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

                SettingsList {
                    id: trustedCertificateList

                    anchors.leftMargin: view.leftContentMargin
                    anchors.left: parent.left

                    width: parent.width - anchors.leftMargin - anchors.rightMargin

                    leftContentMargin: 0
                    title: qsTranslate("Certificate", "Trusted certificates")
                    titlePointSize: 12
                    listView.model: BackEnd.certificateItemModel
                    listView.height: 400

                    listView.delegate: Rectangle {
                        id: trustedCertListViewDelegate

                        component SubitemText : QQC.Label {
                            Layout.leftMargin: 5
                            Layout.rightMargin: 5
                            Layout.fillWidth: true
                            visible: trustedCertListViewDelegate.isCurrentItem
                            verticalAlignment: Qt.AlignVCenter
                            color: trustedCertListViewDelegate.isCurrentItem ? palette.highlightedText :
                                                                               palette.windowText
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
                        width: trustedCertificateList.listView.width
                        implicitHeight: trustedCertificateDelegateLayout.height
                        color: isCurrentItem ? palette.highlight : palette.light
                        clip: true

                        Behavior on implicitHeight {
                            NumberAnimation { duration: 100 }
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: BackEnd.certificateItemModel.setCurrentIndex(trustedCertListViewDelegate.index)
                        }

                        ColumnLayout {
                            id: trustedCertificateDelegateLayout

                            width: parent.width
                            spacing: 0

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 36

                                QQC.Label {
                                    Layout.fillWidth: true
                                    Layout.leftMargin: 5
                                    font.pointSize: 14
                                    text: trustedCertListViewDelegate.issuerDisplayName
                                    color: trustedCertListViewDelegate.isCurrentItem ? palette.highlightedText :
                                                                                       palette.windowText
                                    elide: Text.ElideRight
                                }

                                IconImage {
                                    Layout.alignment: Qt.AlignVCenter
                                    Layout.rightMargin: 10
                                    sourceSize.width: 24
                                    sourceSize.height: 24
                                    source: "qrc:/icons/delete.svg"
                                    color: trustedCertListViewDelegate.isCurrentItem ? palette.highlightedText :
                                                                                       palette.windowText

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: BackEnd.certificateItemModel.removeCertificate(trustedCertListViewDelegate.index)
                                    }
                                }
                            }

                            SubitemTitle {
                                text: qsTranslate("Certificate", "Valid from")
                            }

                            SubitemText {
                                text: trustedCertListViewDelegate.effectiveDate.toLocaleString(Qt.locale(), qsTranslate("General", "MM/dd/yyyy"))
                            }

                            SubitemTitle {
                                text: qsTranslate("Certificate", "Valid to")
                            }

                            SubitemText {
                                text: trustedCertListViewDelegate.expiryDate.toLocaleString(Qt.locale(), qsTranslate("General", "MM/dd/yyyy"))
                            }

                            SubitemTitle {
                                text: qsTranslate("Certificate", "Fingerprint (SHA-256)")
                            }

                            SubitemText {
                                text: trustedCertListViewDelegate.fingerprint
                                wrapMode: Text.Wrap
                            }

                            SubitemTitle {
                                text: qsTranslate("Certificate", "Common name")
                            }

                            SubitemText {
                                text: trustedCertListViewDelegate.commonName
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
                                text: trustedCertListViewDelegate.serialNumber
                                wrapMode: Text.Wrap
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
            QQC.Label {
                padding: 3
                font {
                    pointSize: 12
                    bold: true
                }
                color: palette.highlightedText
                text: qsTranslate("Settings", "Enter new dashboard name")
            }

            TextEdit {
                Layout.maximumWidth: contentColumn.width
                id: nameTextEdit
                padding: 3
                font {
                    pointSize: 10
                }
                color: palette.highlightedText
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
                    color: palette.highlightedText

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
                }

                IconImage {
                    Layout.margins: 5
                    Layout.alignment: Qt.AlignVCenter
                    sourceSize.width: 24
                    sourceSize.height: 24
                    source: "qrc:/icons/checkmark.svg"
                    color: enabled ? palette.active.highlightedText : palette.disabled.highlightedText
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
