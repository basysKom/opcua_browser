#include "monitoreditemmodel.h"
#include "treeitem.h"

enum Roles : int {
    DisplayNameRole = Qt::DisplayRole,
    ValueRole = Qt::UserRole,
};

MonitoredItemModel::MonitoredItemModel(QObject *parent)
    : QAbstractListModel{parent}
{

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

    qDebug() << mItems[index.row()]->displayName() << role;
    switch (role) {
    case DisplayNameRole:
        return mItems[index.row()]->displayName();
    case ValueRole:
        return mItems[index.row()]->value();
    }

    return QVariant();
}

void MonitoredItemModel::addItem(TreeItem *item)
{
    int count = mItems.size();
    beginInsertRows(QModelIndex(), count, count);
    mItems << item;
    endInsertRows();
}

void MonitoredItemModel::removeItem(TreeItem *item)
{
    const int index = mItems.indexOf(item);
    if (index < 0)
        return;

    beginRemoveRows(QModelIndex(), index, index);
    mItems.remove(index);
    endRemoveRows();
}

void MonitoredItemModel::setItems(const QList<TreeItem *> &items)
{
    beginResetModel();
    mItems = items;
    endResetModel();
}

void MonitoredItemModel::valueChanged(TreeItem *item)
{
    const int index = mItems.indexOf(item);
    if (index < 0)
        return;

    const auto modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex, QList<int>() << ValueRole);
}

void MonitoredItemModel::disableMonitoring(int index)
{
    if ((index < 0) || (index >= mItems.size()))
        return;

    mItems.at(index)->disableMonitoring();
}
