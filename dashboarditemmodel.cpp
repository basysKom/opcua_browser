#include <QSettings>

#include "dashboarditem.h"
#include "dashboarditemmodel.h"
#include "monitoreditemmodel.h"

enum Roles : int {
    DisplayNameRole = Qt::DisplayRole,
    TypeRole = Qt::UserRole,
    MonitoringModelRole
};

DashboardItemModel::DashboardItemModel(QObject *parent) : QAbstractListModel{ parent }
{
    // Insert Add item
    mItems.push_back(new DashboardItem(Types::DashboardType::Add));
}

DashboardItemModel::~DashboardItemModel()
{
    qDeleteAll(mItems);
}

QHash<int, QByteArray> DashboardItemModel::roleNames() const
{
    return {
        { DisplayNameRole, "name" },
        { TypeRole, "type" },
        { MonitoringModelRole, "monitoringModel" },
    };
}

int DashboardItemModel::rowCount(const QModelIndex &parent) const
{
    return mItems.size();
}

QVariant DashboardItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case DisplayNameRole:
        return mItems[index.row()]->name();
    case TypeRole:
        return static_cast<int>(mItems[index.row()]->type());
    case MonitoringModelRole:
        return QVariant::fromValue<QObject *>(mItems[index.row()]->monitoredItemModel());
    }

    return QVariant();
}

int DashboardItemModel::addItem(Types::DashboardType type, const QString &name)
{
    const int pos = mItems.size() - 1;
    DashboardItem *item = new DashboardItem(type, name);

    beginInsertRows(QModelIndex(), pos, pos);
    mItems.insert(pos, item);
    endInsertRows();

    return pos;
}

void DashboardItemModel::removeItem(int index)
{
    // Add item is last item
    if (index == mItems.size() - 1)
        return;

    beginRemoveRows(QModelIndex(), index, index);
    auto item = mItems.takeAt(index);
    endRemoveRows();

    item->deleteLater();
}

void DashboardItemModel::clearItems()
{
    beginResetModel();
    qDeleteAll(mItems);
    mItems.clear();
    // Insert Add item
    mItems.push_back(new DashboardItem(Types::DashboardType::Add));
    endResetModel();
}

MonitoredItemModel *DashboardItemModel::getMonitoredItemModel(int index) const
{
    if (index >= mItems.size())
        return nullptr;

    return dynamic_cast<MonitoredItemModel *>(mItems[index]->monitoredItemModel());
}

MonitoredItemModel *DashboardItemModel::getCurrentMonitoredItemModel() const
{
    return getMonitoredItemModel(mCurrentIndex);
}

Types::DashboardType DashboardItemModel::getCurrentDashboardType() const
{
    if (mCurrentIndex >= mItems.size())
        return Types::DashboardType::Unknown;

    return mItems[mCurrentIndex]->type();
}

void DashboardItemModel::setCurrentDashboardName(const QString &name)
{
    if (mCurrentIndex >= mItems.size())
        return;

    mItems[mCurrentIndex]->setName(name);

    const QModelIndex index = createIndex(mCurrentIndex, 0);
    emit dataChanged(index, index, QList<int>() << DisplayNameRole);
}

bool DashboardItemModel::isAddItem(uint index) const
{
    if (index >= mItems.size())
        return false;

    return (mItems.at(index)->type() == Types::DashboardType::Add);
}

void DashboardItemModel::setCurrentIndex(uint index)
{
    if (index < mItems.size()) {
        mCurrentIndex = index;
    }
}

void DashboardItemModel::saveDashboardsToSettings() const
{
    QSettings settings;
    settings.remove("lastDashboards");

    settings.beginWriteArray("lastDashboards");
    for (qsizetype i = 0; i < mItems.count(); ++i) {
        if (mItems[i]->type() == Types::DashboardType::Add)
            continue;

        settings.setArrayIndex(i);
        settings.setValue("name", mItems[i]->name());
        settings.setValue("type", (int)mItems[i]->type());
        settings.setValue("nodeIDs", mItems[i]->getMonitoredNodeIds());
    }
    settings.endArray();
}
