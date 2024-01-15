import QtQuick

import OPC_UA_Browser

Rectangle {
    id: view

    property ThemeImprintView theme: Style.imprintView

    color: theme.background

    Column {
        leftPadding: 10
        rightPadding: leftPadding
        spacing: 10

        Text {
            color: view.theme.textColor
            font {
                pointSize: 18
                bold: true
            }
            text: qsTr("Imprint")
        }

        Text {
            color: view.theme.textColor
            font.pointSize: 10
            text: "basysKom GmbH"
        }

        Text {
            color: view.theme.textColor
            font.pointSize: 10
            text: "Robert-Bosch-Str. 7<br>64293 Darmstadt<br>" + qsTr("Germany")
        }

        Text {
            color: view.theme.textColor
            font.pointSize: 10
            textFormat: Text.RichText
            text: "T +49 6151 870589-0<br><a href=mailto:info@basyskom.com>info@basyskom.com</a>"
        }

        Text {
            color: view.theme.textColor
            font.pointSize: 10
            text: qsTr("Registration court") + ": Amtsgericht Darmstadt<br>" + qsTr(
                      "Register number") + ": HRB 9352<br>" + qsTr(
                      "VAT-ID") + ": DE238913185"
        }

        Text {
            color: view.theme.textColor
            font.pointSize: 10
            text: "<b>" + "Executive Managers" + ":</b><br>Heike Ziegler, Alexander Sorg"
        }
    }
}
