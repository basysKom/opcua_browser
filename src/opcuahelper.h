/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef OPCUAHELPER_H
#define OPCUAHELPER_H

#include <QString>

#include <QtOpcUa/qopcuatype.h>

class QOpcUaNode;

class QOpcUaHelper
{
public:
    static QString variantToString(QOpcUaNode *node, const QVariant &value,
                                   const QString &typeNodeId);
    static QString getRawAttributeValue(QOpcUaNode *node, QOpcUa::NodeAttribute attr);
    static QString getFormattedAttributeValue(QOpcUaNode *node, QOpcUa::NodeAttribute attr);
};

#endif // OPCUAHELPER_H
