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
    mItems.push_back(new DashboardItem(DashboardItem::Type::Add));
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

void DashboardItemModel::addItem(DashboardItem::Type type, const QString &name)
{
    const int pos = mItems.size() - 1;
    DashboardItem *item = new DashboardItem(type, name);

    beginInsertRows(QModelIndex(), pos, pos);
    mItems.insert(pos, item);
    endInsertRows();
}

void DashboardItemModel::clearItems()
{
    beginResetModel();
    qDeleteAll(mItems);
    mItems.clear();
    // Insert Add item
    mItems.push_back(new DashboardItem(DashboardItem::Type::Add));
    endResetModel();
}

MonitoredItemModel *DashboardItemModel::getCurrentMonitoredItemModel() const
{
    if (mCurrentIndex >= mItems.size())
        return nullptr;

    return dynamic_cast<MonitoredItemModel *>(mItems[mCurrentIndex]->monitoredItemModel());
}

bool DashboardItemModel::isAddItem(uint index) const
{
    if (index >= mItems.size())
        return false;

    return (mItems.at(index)->type() == DashboardItem::Type::Add);
}

void DashboardItemModel::setCurrentIndex(uint index)
{
    if (index < mItems.size()) {
        mCurrentIndex = index;
    }
}
