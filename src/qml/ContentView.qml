/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

Item {
    id: view

    readonly property bool showBackButtonInHeader: (stackLayout.currentIndex
                                                    === imprint.StackLayout.index)
                                                   || (stackLayout.currentIndex
                                                       === settings.StackLayout.index)
    readonly property bool canSaveDashboard: dashboard.canSaveDashboard
                                             && (stackLayout.currentIndex
                                                 === dashboard.StackLayout.index)
    readonly property alias currentDashboardName: dashboard.currentDashboardName

    function showConnectionView() {
        stackLayout.currentIndex = connection.StackLayout.index
    }

    function showDashboardView() {
        stackLayout.currentIndex = dashboard.StackLayout.index
    }

    function showExpertBrowserView() {
        stackLayout.currentIndex = browser.StackLayout.index
        browser.type = BrowserView.Type.ExpertMode
    }

    function showImprintView() {
        stackLayout.lastStoredViewIndex = stackLayout.currentIndex
        stackLayout.currentIndex = imprint.StackLayout.index
    }

    function showSettingsView() {
        stackLayout.lastStoredViewIndex = stackLayout.currentIndex
        stackLayout.currentIndex = settings.StackLayout.index
    }

    function goBack() {
        stackLayout.currentIndex = stackLayout.lastStoredViewIndex
    }

    Connections {
        target: BackEnd
        function onIsConnectedChanged() {
            if (BackEnd.isConnected
                    && (stackLayout.currentIndex === connection.StackLayout.index)) {
                // After successful connection, go to dashboard configuration view
                stackLayout.currentIndex = dashboardConfiguration.StackLayout.index
                dashboardConfiguration.type = DashboardConfigurationView.Type.Unknown
            } else if (!BackEnd.isConnected) {
                dashboardConfiguration.type = DashboardConfigurationView.Type.Unknown
                view.showConnectionView()
            }
        }
    }

    StackLayout {
        id: stackLayout

        property int lastStoredViewIndex: 0

        anchors.fill: parent
        currentIndex: 0

        ConnectionView {
            id: connection
        }

        DashboardConfigurationView {
            id: dashboardConfiguration

            onUseLastSession: {
                // Load last dashboards from settings
                BackEnd.loadLastDashboardsFromSettings()
                // Go to dashboard view
                stackLayout.currentIndex = dashboard.StackLayout.index
            }

            onAddMonitoredItems: {
                // Store index of dashboardConfiguration to return to it when cancelling
                stackLayout.lastStoredViewIndex = dashboardConfiguration.StackLayout.index
                // Go to browser view
                stackLayout.currentIndex = browser.StackLayout.index
                browser.addDashboardOnAccepted = true
                browser.type = BrowserView.Type.SelectMonitoredItem
            }

            onAddEvents: {
                // Store index of dashboardConfiguration to return to it when cancelling
                stackLayout.lastStoredViewIndex = dashboardConfiguration.StackLayout.index
                // Go to browser view
                stackLayout.currentIndex = browser.StackLayout.index
                browser.addDashboardOnAccepted = true
                browser.type = BrowserView.Type.SelectEvents
            }

            onAddSavedVariableDashboard: function (name) {
                // Add new variables dashboard
                dashboard.addMonitoredItemsDashboard(name)
                // Load node IDs for saved dashboard name to current dashboard
                BackEnd.loadDashboard(name)
                // Go to dashboard view
                stackLayout.currentIndex = dashboard.StackLayout.index
            }

            onAddSavedEventDashboard: function (name) {
                // Add new event dashboard
                dashboard.addEventsDashboard(name)
                // Load node IDs for saved dashboard name to current dashboard
                BackEnd.loadDashboard(name)
                // Go to dashboard view
                stackLayout.currentIndex = dashboard.StackLayout.index
            }

            onViewCanceled: {
                // Go back to dashboard view
                stackLayout.currentIndex = dashboard.StackLayout.index
            }
        }

        BrowserView {
            id: browser

            property bool addDashboardOnAccepted: true

            onSelectionCancelled: {
                // Go back to last view and clear selected node ID list
                stackLayout.currentIndex = stackLayout.lastStoredViewIndex
                BackEnd.opcUaModel.clearSelectionList()
            }

            onSelectionAccepted: {
                // Add new dashboard if necessary
                if (addDashboardOnAccepted) {
                    if (browser.type === BrowserView.Type.SelectMonitoredItem) {
                        dashboard.addMonitoredItemsDashboard("")
                    } else if (browser.type === BrowserView.Type.SelectEvents) {
                        dashboard.addEventsDashboard("")
                    }
                }

                // Add selected node IDs to current dashboard
                BackEnd.monitorSelectedNodes()
                // Clear selected node ID list
                BackEnd.opcUaModel.clearSelectionList()
                // Go to dashboard view
                stackLayout.currentIndex = dashboard.StackLayout.index
            }
        }

        DashboardView {
            id: dashboard

            onAddMonitoredItems: {
                // Store index of dashboard to return to it when cancelling
                stackLayout.lastStoredViewIndex = dashboard.StackLayout.index
                // Go to browser view
                stackLayout.currentIndex = browser.StackLayout.index
                browser.addDashboardOnAccepted = false
                browser.type = BrowserView.Type.SelectMonitoredItem
            }

            onAddEvents: {
                // Store index of dashboard to return to it when cancelling
                stackLayout.lastStoredViewIndex = dashboard.StackLayout.index
                // Go to browser view
                stackLayout.currentIndex = browser.StackLayout.index
                browser.addDashboardOnAccepted = false
                browser.type = BrowserView.Type.SelectEvents
            }

            onAddNewDashboard: {
                // Go to dashboard configuration view
                stackLayout.currentIndex = dashboardConfiguration.StackLayout.index
                // Show last session button only for new connection
                dashboardConfiguration.showLastSessionButton = false
                // Show cancel button to go back to dashboard view
                dashboardConfiguration.showCancelButton = true
                dashboardConfiguration.type = DashboardConfigurationView.Type.Unknown
            }
        }

        ImprintView {
            id: imprint
        }

        SettingsView {
            id: settings
        }
    }
}
