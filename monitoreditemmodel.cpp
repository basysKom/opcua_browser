#include "monitoreditem.h"
#include "monitoreditemmodel.h"

enum Roles : int {
    DisplayNameRole = Qt::DisplayRole,
    ValueRole = Qt::UserRole,
    IsAddItemRole,
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
    return {
        { DisplayNameRole, "name" },
        { ValueRole, "value" },
        { IsAddItemRole, "isAddItem" },
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
    case IsAddItemRole:
        return mItems[index.row()]->nodeId().isEmpty();
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
