/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <QString>
#include <qopcuatype.h>

class Attribute
{
public:
    Attribute(QOpcUa::NodeAttribute attribute, const QString &value);

    QOpcUa::NodeAttribute attribute() const noexcept;
    const QString &attributeName() const noexcept;
    const QString &value() const noexcept;

    void setValue(const QString &value);

private:
    QOpcUa::NodeAttribute mAttribute;
    QString mAttributeName;
    QString mValue;
};

#endif // ATTRIBUTE_H
