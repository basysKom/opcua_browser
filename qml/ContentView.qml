import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    readonly property bool showBackButtonInHeader: (stackLayout.currentIndex === 2)
                                                   || (stackLayout.currentIndex === 3)
    readonly property bool canSaveDashboard: dashboard.canSaveDashboard
                                             && (stackLayout.currentIndex === 1)

    function showDashboardView() {
        stackLayout.currentIndex = 1
    }

    function showExpertBrowserView() {
        stackLayout.currentIndex = 0
        browser.type = BrowserView.Type.ExpertMode
    }

    function showImprintView() {
        stackLayout.lastStoredViewIndex = stackLayout.currentIndex
        stackLayout.currentIndex = 2
    }

    function showSettingsView() {
        stackLayout.lastStoredViewIndex = stackLayout.currentIndex
        stackLayout.currentIndex = 3
    }

    function goBack() {
        stackLayout.currentIndex = stackLayout.lastStoredViewIndex
    }

    StackLayout {
        id: stackLayout

        property int lastStoredViewIndex: 0

        anchors.fill: parent
        visible: true

        currentIndex: 1

        BrowserView {
            id: browser

            onSelectionCancelled: stackLayout.currentIndex = 1
            onSelectionAccepted: stackLayout.currentIndex = 1
        }

        DashboardView {
            id: dashboard

            onAddMonitoredItems: {
                stackLayout.currentIndex = 0
                browser.type = BrowserView.Type.SelectMonitoredItem
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
