import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    GridLayout {
        id: gridLayout
        Layout.fillWidth: true
        columns: 2
        visible: !BackEnd.isConnected

        readonly property int textColumnWidth: 70
        readonly property int columnHeight: 30

        Text {
            Layout.preferredWidth: gridLayout.textColumnWidth
            Layout.preferredHeight: gridLayout.columnHeight
            verticalAlignment: Qt.AlignVCenter
            text: qsTr("State")
        }

        Text {
            Layout.fillWidth: true
            Layout.preferredHeight: gridLayout.columnHeight
            verticalAlignment: Qt.AlignVCenter
            text: BackEnd.stateText
        }

        Text {
            Layout.preferredWidth: gridLayout.textColumnWidth
            Layout.preferredHeight: gridLayout.columnHeight
            verticalAlignment: Qt.AlignVCenter
            text: qsTr("Host")
        }

        TextField {
            id: hostUrl
            Layout.fillWidth: true
            Layout.preferredHeight: gridLayout.columnHeight
            verticalAlignment: Qt.AlignVCenter
            text: "opc.tcp://localhost:43344"
            placeholderText: "opc.tcp://localhost:4080"
            enabled: !BackEnd.isConnected
            onTextChanged: {
                BackEnd.clearServerList();
                BackEnd.clearEndpointList();
            }
        }

        Text {
            Layout.preferredWidth: gridLayout.textColumnWidth
            Layout.preferredHeight: gridLayout.columnHeight
            verticalAlignment: Qt.AlignVCenter
            text: qsTr("Server")
            visible: serverListBox.visible
        }

        ComboBox {
            id: serverListBox
            Layout.fillWidth: true
            Layout.preferredHeight: gridLayout.columnHeight
            model: BackEnd.serverList
            visible: model.length > 0
            enabled: !BackEnd.isConnected
            onCurrentIndexChanged: BackEnd.clearEndpointList();
        }

        Text {
            Layout.preferredWidth: gridLayout.textColumnWidth
            Layout.preferredHeight: gridLayout.columnHeight
            verticalAlignment: Qt.AlignVCenter
            text: qsTr("Endpoint")
            visible: endpointListBox.visible
        }

        ComboBox {
            id: endpointListBox
            Layout.fillWidth: true
            Layout.preferredHeight: gridLayout.columnHeight
            model: BackEnd.endpointList
            visible: model.length > 0
            enabled: !BackEnd.isConnected
        }
    }

    RowLayout {
        Layout.fillWidth: true

        Rectangle {
            Layout.preferredWidth: 15
            Layout.preferredHeight: Layout.preferredWidth
            visible: (serverListBox.model.length > 0) && (endpointListBox.model.length > 0)

            radius: Layout.preferredWidth / 2
            color: (2 === BackEnd.connectionState) ? "#4CAF50" :
                   (1 === BackEnd.connectionState) ? "#F2C94C" : "#F44336"
        }

        RoundButton {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            radius: 5
            text: (serverListBox.model.length === 0)   ? qsTr("Find server") :
                  (endpointListBox.model.length === 0) ? qsTr("Get endpoints") :
                  (BackEnd.isConnected)                ? qsTr("Disconnect") : qsTr("Connect")

            onClicked: {
                if (serverListBox.model.length === 0) {
                    BackEnd.findServers(hostUrl.text)
                } else if (endpointListBox.model.length === 0) {
                    BackEnd.getEndpoints(serverListBox.currentIndex)
                } else if (BackEnd.isConnected) {
                    BackEnd.disconnectFromEndpoint()
                } else {
                    BackEnd.connectToEndpoint(serverListBox.currentIndex)
                }
            }
        }
    }
}
