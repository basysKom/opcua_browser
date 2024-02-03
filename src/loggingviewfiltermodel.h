/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef LOGGINGVIEWFILTERMODEL_H
#define LOGGINGVIEWFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <QQmlEngine>

class LoggingViewFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit LoggingViewFilterModel(QObject *parent = nullptr);

    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    Q_INVOKABLE void setMessageTypeFilterMask(quint8 mask);

private:
    quint8 mTypeFilterMask = 0xFF;
};

#endif // LOGGINGVIEWFILTERMODEL_H
