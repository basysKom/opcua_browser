/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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
            text: qsTranslate("Imprint", "Imprint")
        }

        Text {
            color: view.theme.textColor
            font.pointSize: 10
            text: "basysKom GmbH"
        }

        Text {
            color: view.theme.textColor
            font.pointSize: 10
            text: "Robert-Bosch-Str. 7<br>64293 Darmstadt<br>" + qsTranslate("Imprint", "Germany")
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
            text: qsTranslate("Imprint", "Registration court") + ": Amtsgericht Darmstadt<br>" +
                  qsTranslate("Imprint", "Register number") + ": HRB 9352<br>" +
                  qsTranslate("Imprint", "VAT-ID") + ": DE238913185"
        }

        Text {
            color: view.theme.textColor
            font.pointSize: 10
            text: "<b>" + qsTranslate("Imprint", "Executive Managers") + ":</b><br>Heike Ziegler, Alexander Sorg"
        }
    }
}
