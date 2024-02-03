/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "loggingviewfiltermodel.h"

LoggingViewFilterModel::LoggingViewFilterModel(QObject *parent)
    : QSortFilterProxyModel{ parent } { }

bool LoggingViewFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    const int type = sourceModel()->data(index, filterRole()).toInt();
    return (((1 << type) & mTypeFilterMask) > 0);
}

void LoggingViewFilterModel::setMessageTypeFilterMask(quint8 mask)
{
    if (mask != mTypeFilterMask) {
        mTypeFilterMask = mask;
        invalidateRowsFilter();
    }
}
