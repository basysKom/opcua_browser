/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef UISETTINGS_H
#define UISETTINGS_H

#include <QColor>
#include <QObject>
#include <QQmlEngine>

class UiSettings : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit UiSettings(QObject *parent = nullptr);

    static Q_INVOKABLE void setStatusAndNavigationBarColor(const QColor &color);

private:
    static void setStatusAndNavigationBarTheme(bool isLight);
};

#endif // UISETTINGS_H
