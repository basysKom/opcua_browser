#include "monitoreditemmodel.h"

enum Roles : int {
    DisplayNameRole = Qt::DisplayRole,
    ValueRole = Qt::UserRole,
};

MonitoredItemModel::MonitoredItemModel(QObject *parent)
    : QAbstractListModel{parent}
{
    qDeleteAll(mItems);
}

QHash<int, QByteArray> MonitoredItemModel::roleNames() const
{
    return {
        {DisplayNameRole, "name"},
        {ValueRole, "value"},
    };
}

int MonitoredItemModel::rowCount(const QModelIndex &parent) const
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
    }

    return QVariant();
}

bool MonitoredItemModel::containsItem(const QString &nodeId) const noexcept
{
    auto it = std::find_if(mItems.begin(), mItems.end(), [&](const MonitoredItem *item) { return (item->nodeId() == nodeId); });
    return (it != mItems.end());
}

void MonitoredItemModel::addItem(QOpcUaNode *node)
{
    const int count = mItems.size();
    MonitoredItem *monitoredItem = new MonitoredItem(node);
    connect(monitoredItem, &MonitoredItem::displayNameChanged, this, [=] () {
        emit dataChanged(index(count), index(count), QList<int>() << DisplayNameRole);
    });

    connect(monitoredItem, &MonitoredItem::valueChanged, this, [=] () {
        emit dataChanged(index(count), index(count), QList<int>() << ValueRole);
    });

    beginInsertRows(QModelIndex(), count, count);
    mItems.push_back(monitoredItem);
    endInsertRows();
}

void MonitoredItemModel::clearItems()
{
    beginResetModel();
    qDeleteAll(mItems);
    mItems.clear();
    endResetModel();
}

QStringList MonitoredItemModel::getNodeIds() const
{
    QStringList nodeIds;
    for (const auto item : mItems) {
        nodeIds << item->nodeId();
    }

    return nodeIds;
}

void MonitoredItemModel::disableMonitoring(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    auto item = mItems.takeAt(index);
    endRemoveRows();

    item->deleteLater();
}
