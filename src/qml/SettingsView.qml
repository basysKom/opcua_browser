/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

Rectangle {
    id: view

    property ThemeSettingsView theme: Style.settingsView

    function setTheme(index) {
        Style.currentThemeIndex = index
    }

    color: theme.background

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

        ScrollBar.vertical: StyledScrollBar {
            policy: ScrollBar.AsNeeded
        }

        Column {
            id: content
            anchors.fill: parent

            leftPadding: 10
            rightPadding: leftPadding
            spacing: 10

            Text {
                color: view.theme.textColor
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
                    color: view.theme.textColor
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("General", "Theme")
                }

                ButtonGroup {
                    buttons: buttonRow.children
                }

                Row {
                    spacing: 5
                    id: buttonRow

                    StyledItemSelector {
                        id: darkItemSelector

                        height: 24
                        width: height
                        checkState: (Style.currentThemeIndex === 0) ? Qt.Checked : Qt.Unchecked
                        onToggled: view.setTheme(0)
                    }

                    Text {
                        anchors.verticalCenter: darkItemSelector.verticalCenter
                        color: view.theme.textColor
                        font.pointSize: 12
                        text: qsTranslate("General", "Dark")
                    }

                    Item {
                        width: 20
                        height: 1
                    }

                    StyledItemSelector {
                        id: brightItemSelector

                        height: 24
                        width: height
                        checkState: (Style.currentThemeIndex === 1) ? Qt.Checked : Qt.Unchecked
                        onToggled: view.setTheme(1)
                    }

                    Text {
                        anchors.verticalCenter: brightItemSelector.verticalCenter
                        color: view.theme.textColor
                        font.pointSize: 12
                        text: qsTranslate("General", "Bright")
                    }
                }
            }

            // Language list view
            Column {
                width: parent.width - content.leftPadding - content.rightPadding
                spacing: 5

                Text {
                    color: view.theme.textColor
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Settings", "Language")
                }

                Rectangle {
                    color: view.theme.backgroundListView
                    radius: 5

                    width: parent.width
                    height: childrenRect.height

                    ListView {
                        id: languageListView

                        width: parent.width
                        height: Math.min(200, contentHeight)

                        clip: true

                        model: languageModel
                        boundsBehavior: Flickable.StopAtBounds
                        boundsMovement: Flickable.StopAtBounds

                        ScrollBar.vertical: StyledScrollBar {
                            policy: ScrollBar.AsNeeded
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
                            color: isCurrentItem ? view.theme.backgroundSelected : "transparent"
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
                                    color: view.theme.textColor
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
                    color: view.theme.textColor
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Dashboard", "Saved variable dashboards")
                }

                Rectangle {
                    color: view.theme.backgroundListView
                    radius: 5

                    width: parent.width
                    height: childrenRect.height

                    ListView {
                        id: variableDashboardsListView

                        width: parent.width
                        height: Math.min(200, contentHeight)

                        clip: true

                        model: BackEnd.savedVariableDashboards

                        boundsBehavior: Flickable.StopAtBounds
                        boundsMovement: Flickable.StopAtBounds

                        ScrollBar.vertical: StyledScrollBar {
                            policy: ScrollBar.AsNeeded
                        }

                        delegate: Rectangle {
                            id: variableDashboardsListViewDelegate

                            required property int index
                            required property string display

                            radius: 5
                            width: variableDashboardsListView.width
                            implicitHeight: childrenRect.height
                            color: "transparent"
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
                                    color: view.theme.textColor
                                    elide: Text.ElideRight
                                }

                                IconImage {
                                    Layout.alignment: Qt.AlignVCenter
                                    sourceSize.width: 24
                                    sourceSize.height: 24
                                    source: "qrc:/icons/edit.svg"
                                    color: view.theme.textColor

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
                                    color: view.theme.textColor

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
                    color: view.theme.textColor
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Dashboard", "Saved event dashboards")
                }

                Rectangle {
                    color: view.theme.backgroundListView
                    radius: 5

                    width: parent.width
                    height: childrenRect.height

                    ListView {
                        id: eventDashboardsListView

                        width: parent.width
                        height: Math.min(200, contentHeight)

                        clip: true

                        model: BackEnd.savedEventDashboards

                        boundsBehavior: Flickable.StopAtBounds
                        boundsMovement: Flickable.StopAtBounds

                        ScrollBar.vertical: StyledScrollBar {
                            policy: ScrollBar.AsNeeded
                        }

                        delegate: Rectangle {
                            id: eventDashboardsListViewDelegate

                            required property int index
                            required property string display

                            radius: 5
                            width: eventDashboardsListView.width
                            implicitHeight: childrenRect.height
                            color: "transparent"
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
                                    color: view.theme.textColor
                                    elide: Text.ElideRight
                                }

                                IconImage {
                                    Layout.alignment: Qt.AlignVCenter
                                    sourceSize.width: 24
                                    sourceSize.height: 24
                                    source: "qrc:/icons/edit.svg"
                                    color: view.theme.textColor

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
                                    color: view.theme.textColor

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
                    color: view.theme.textColor
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Connection", "Recent connections")
                }

                Rectangle {
                    color: view.theme.backgroundListView
                    radius: 5

                    width: parent.width
                    height: childrenRect.height

                    ListView {
                        id: recentConnectionsListView

                        width: parent.width
                        height: Math.min(200, contentHeight)

                        clip: true

                        model: BackEnd.recentConnections

                        boundsBehavior: Flickable.StopAtBounds
                        boundsMovement: Flickable.StopAtBounds

                        ScrollBar.vertical: StyledScrollBar {
                            policy: ScrollBar.AsNeeded
                        }

                        delegate: Rectangle {
                            id: recentConnectionsListViewDelegate

                            required property int index
                            required property string modelData

                            radius: 5
                            width: recentConnectionsListView.width
                            implicitHeight: childrenRect.height
                            color: "transparent"
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
                                    color: view.theme.textColor
                                    elide: Text.ElideRight
                                }

                                IconImage {
                                    Layout.alignment: Qt.AlignVCenter
                                    Layout.rightMargin: 10
                                    sourceSize.width: 24
                                    sourceSize.height: 24
                                    source: "qrc:/icons/delete.svg"
                                    color: view.theme.textColor

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
                    color: view.theme.textColor
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Dashboard", "Dashboard")
                }

                StyledSpinBox {
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
                    color: view.theme.textColor
                    font {
                        pointSize: 14
                        bold: true
                    }
                    text: qsTranslate("Certificate", "Certificates")
                }

                Rectangle {
                    color: view.theme.backgroundListView
                    radius: 5

                    width: parent.width
                    height: childrenRect.height

                    ListView {
                        id: certificateListView

                        width: parent.width
                        height: 400

                        clip: true

                        model: BackEnd.certificateItemModel
                        boundsBehavior: Flickable.StopAtBounds
                        boundsMovement: Flickable.StopAtBounds

                        ScrollBar.vertical: StyledScrollBar {
                            policy: ScrollBar.AsNeeded
                        }

                        delegate: Rectangle {
                            id: listViewDelegate

                            component SubitemText : Text {
                                Layout.leftMargin: 5
                                Layout.rightMargin: 5
                                Layout.fillWidth: true
                                visible: listViewDelegate.isCurrentItem
                                verticalAlignment: Qt.AlignVCenter
                                color: view.theme.textColor
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
                            color: isCurrentItem ? view.theme.backgroundSelected : "transparent"
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
                                        color: view.theme.textColor
                                        elide: Text.ElideRight
                                    }

                                    IconImage {
                                        Layout.alignment: Qt.AlignVCenter
                                        Layout.rightMargin: 10
                                        sourceSize.width: 24
                                        sourceSize.height: 24
                                        source: "qrc:/icons/delete.svg"
                                        color: view.theme.textColor

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

    Popup {
        id: dashboardNameEditPopup
        modal: true

        implicitWidth: contentColumn.width
        implicitHeight: contentColumn.height
        padding: 0

        clip: true
        closePolicy: Popup.NoAutoClose

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
            color: view.theme.popupBackground
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
                color: view.theme.textColor
                text: qsTranslate("Settings", "Enter new dashboard name")
            }

            TextEdit {
                Layout.maximumWidth: contentColumn.width
                id: nameTextEdit
                padding: 3
                font {
                    pointSize: 10
                }
                color: view.theme.textColor
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
                    color: view.theme.textColor

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
                    color: enabled ? view.theme.textColor : "lightgrey"
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
