import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

Item {
    id: view

    enum Type {
        Unknown,
        SelectVariables,
        SelectEvents
    }

    readonly property int textColumnHeight: 15
    readonly property int columnHeight: 30
    property int type: DashboardConfigurationView.Type.Unknown
    property bool showLastSessionButton: true
    property bool showCancelButton: false

    signal useLastSession
    signal addMonitoredItems
    signal addEvents
    signal viewCanceled

    signal addSavedVariableDashboard(string name)
    signal addSavedEventDashboard(string name)

    ColumnLayout {
        id: selection

        visible: view.type === DashboardConfigurationView.Type.Unknown
        anchors.centerIn: parent
        width: parent.width - 20
        spacing: 20

        StyledButton {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            text: qsTr("Use last session")
            visible: view.showLastSessionButton && BackEnd.hasLastDashboards

            onClicked: useLastSession()
        }

        StyledButton {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            text: qsTr("Add data dashboard")

            onClicked: {
                if ((BackEnd.defaultVariableDashboards.rowCount() === 0)
                        && (BackEnd.savedVariableDashboards.rowCount() === 0)) {
                    addMonitoredItems()
                } else {
                    view.type = DashboardConfigurationView.Type.SelectVariables
                }
            }
        }

        StyledButton {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            text: qsTr("Add event dashboard")

            onClicked: {
                if ((BackEnd.defaultEventDashboards.rowCount() === 0)
                        && (BackEnd.savedEventDashboards.rowCount() === 0)) {
                    addEvents()
                } else {
                    view.type = DashboardConfigurationView.Type.SelectEvents
                }
            }
        }

        StyledButton {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            text: qsTr("Add alarms and conditions dashboard")
            // ToDo: Add alarms and conditons handling
            visible: false

            onClicked: {

            }
        }
    }

    ColumnLayout {
        visible: !selection.visible
        anchors.centerIn: parent
        width: parent.width - 20
        spacing: 20

        StyledButton {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            text: (view.type === DashboardConfigurationView.Type.SelectVariables) ? qsTr("Select monitored variables manually") : qsTr("Select events manually")

            onClicked: {
                if (view.type === DashboardConfigurationView.Type.SelectVariables) {
                    addMonitoredItems()
                } else {
                    addEvents()
                }
            }
        }

        ColumnLayout {
            visible: (defaultDashboardListBox.model !== undefined)
                     && (defaultDashboardListBox.model.rowCount() > 0)

            StyledComboBox {
                id: defaultDashboardListBox

                captionText: (view.type === DashboardConfigurationView.Type.SelectVariables) ? qsTr("Default data dashboards") : qsTr("Default event dashboards")
                model: (view.type === DashboardConfigurationView.Type.SelectVariables) ? BackEnd.defaultVariableDashboards : BackEnd.defaultEventDashboards
            }

            StyledButton {
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                text: qsTr("Add dashboard")

                onClicked: {

                }
            }
        }

        ColumnLayout {
            visible: (savedDashboardListBox.model !== undefined)
                     && (savedDashboardListBox.model.rowCount() > 0)

            StyledComboBox {
                id: savedDashboardListBox

                captionText: (view.type === DashboardConfigurationView.Type.SelectVariables) ? qsTr("Saved data dashboards") : qsTr("Saved event dashboards")
                model: (view.type === DashboardConfigurationView.Type.SelectVariables) ? BackEnd.savedVariableDashboards : BackEnd.savedEventDashboards
                textRole: "display"
            }

            StyledButton {
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                text: qsTr("Add dashboard")

                onClicked: {
                    if (view.type === DashboardConfigurationView.Type.SelectVariables) {
                        addSavedVariableDashboard(
                                    savedDashboardListBox.currentText)
                    } else {
                        addSavedEventDashboard(
                                    savedDashboardListBox.currentText)
                    }
                }
            }
        }
    }

    StyledButton {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        visible: !selection.visible && !showCancelButton
        height: 35
        width: parent.width / 3
        highlighted: false
        text: qsTr("Back")

        onClicked: view.type = DashboardConfigurationView.Type.Unknown
    }

    StyledButton {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        visible: showCancelButton
        height: 35
        width: parent.width / 3
        highlighted: false
        text: qsTr("Cancel")

        onClicked: viewCanceled()
    }
}
