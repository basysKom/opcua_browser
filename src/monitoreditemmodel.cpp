/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "monitoreditem.h"
#include "monitoreditemmodel.h"

enum Roles : int {
    DisplayNameRole = Qt::DisplayRole,
    ValueRole = Qt::UserRole,
    IsAddItemRole,
    StatusRole,
    HasErrorRole,
    LastEventsRole,
};

MonitoredItemModel::MonitoredItemModel(QObject *parent) : QAbstractListModel{ parent }
{
    // Insert MonitoredItem without OPC UA node for Add item in dashboard
    mItems.push_back(new MonitoredItem(nullptr));
}

MonitoredItemModel::~MonitoredItemModel()
{
    qDeleteAll(mItems);
}

QHash<int, QByteArray> MonitoredItemModel::roleNames() const
{
    return { { DisplayNameRole, "name" },    { ValueRole, "value" },
             { IsAddItemRole, "isAddItem" }, { StatusRole, "status" },
             { HasErrorRole, "hasError" },   { LastEventsRole, "lastEvents" } };
}

int MonitoredItemModel::rowCount(const QModelIndex &) const
{
    return mItems.size();
}

QVariant MonitoredItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case DisplayNameRole:
        return mItems[index.row()]->displayName();
    case ValueRole:
        return mItems[index.row()]->value();
    case IsAddItemRole:
        return mItems[index.row()]->nodeId().isEmpty();
    case StatusRole:
        return mItems[index.row()]->status();
    case HasErrorRole:
        return mItems[index.row()]->hasError();
    case LastEventsRole:
        return QVariant::fromValue(mItems[index.row()]->lastEvents());
    }

    return QVariant();
}

bool MonitoredItemModel::containsItem(const QString &nodeId) const noexcept
{
    auto it = std::find_if(mItems.begin(), mItems.end(),
                           [&](const MonitoredItem *item) { return (item->nodeId() == nodeId); });
    return (it != mItems.end());
}

void MonitoredItemModel::addItem(QOpcUaNode *node)
{
    const int pos = mItems.size() - 1;
    MonitoredItem *monitoredItem = new MonitoredItem(node);
    connect(monitoredItem, &MonitoredItem::displayNameChanged, this,
            [=]() { emit dataChanged(index(pos), index(pos), QList<int>() << DisplayNameRole); });

    connect(monitoredItem, &MonitoredItem::valueChanged, this,
            [=]() { emit dataChanged(index(pos), index(pos), QList<int>() << ValueRole); });

    connect(monitoredItem, &MonitoredItem::hasErrorChanged, this,
            [=]() { emit dataChanged(index(pos), index(pos), QList<int>() << HasErrorRole); });

    connect(monitoredItem, &MonitoredItem::statusChanged, this,
            [=]() { emit dataChanged(index(pos), index(pos), QList<int>() << StatusRole); });

    beginInsertRows(QModelIndex(), pos, pos);
    mItems.insert(pos, monitoredItem);
    endInsertRows();
}

void MonitoredItemModel::addEventItem(QOpcUaNode *node,
                                      const QOpcUaMonitoringParameters::EventFilter &eventFilter)
{
    const int pos = mItems.size() - 1;
    MonitoredItem *monitoredItem = new MonitoredItem(node, eventFilter);
    connect(monitoredItem, &MonitoredItem::displayNameChanged, this,
            [=]() { emit dataChanged(index(pos), index(pos), QList<int>() << DisplayNameRole); });

    connect(monitoredItem, &MonitoredItem::lastEventsChanged, this,
            [=]() { emit dataChanged(index(pos), index(pos), QList<int>() << LastEventsRole); });

    beginInsertRows(QModelIndex(), pos, pos);
    mItems.insert(pos, monitoredItem);
    endInsertRows();
}

void MonitoredItemModel::clearItems()
{
    beginResetModel();
    qDeleteAll(mItems);
    mItems.clear();
    // Insert MonitoredItem without OPC UA node for Add item in dashboard
    mItems.push_back(new MonitoredItem(nullptr));
    endResetModel();
}

void MonitoredItemModel::clearEventsForItem(int index)
{
    if (mItems.size() - 1 < index)
        return;

    mItems.at(index)->clearEvents();
}

QStringList MonitoredItemModel::getNodeIds() const
{
    QStringList nodeIds;
    for (const auto item : mItems) {
        const QString nodeId = item->nodeId();
        if (!nodeId.isEmpty()) {
            nodeIds << item->nodeId();
        }
    }

    return nodeIds;
}

void MonitoredItemModel::disableMonitoring(int index)
{
    // Add item is last item
    if (index == mItems.size() - 1)
        return;

    beginRemoveRows(QModelIndex(), index, index);
    auto item = mItems.takeAt(index);
    endRemoveRows();

    item->deleteLater();
}

QList<QOpcUaMonitoringParameters::EventFilter> MonitoredItemModel::eventFilters()
{
    QList<QOpcUaMonitoringParameters::EventFilter> filters;
    if (mItems.isEmpty() || mItems.constFirst()->type() != MonitoredItem::Type::Event)
        return {};

    for (const auto &item : mItems) {
        const auto filter = item->getEventFilter();
        filters.push_back(item->getEventFilter());
    }

    return filters;
}
