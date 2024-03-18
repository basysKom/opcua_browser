/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QSettings>

#include "constants.h"
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
    mItems.push_back(new DashboardItem(DashboardItem::DashboardType::Add));
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

int DashboardItemModel::rowCount(const QModelIndex &) const
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

bool DashboardItemModel::containsItem(const QString &name) const noexcept
{
    return std::find_if(mItems.constBegin(), mItems.constEnd(),
                        [name](const DashboardItem *item) { return item->name() == name; })
            != mItems.constEnd();
}

int DashboardItemModel::getIndexOfItem(const QString &name) const noexcept
{
    const auto index =
            std::find_if(mItems.constBegin(), mItems.constEnd(),
                         [name](const DashboardItem *item) { return item->name() == name; });

    if (index == mItems.constEnd())
        return -1;

    return std::distance(mItems.constBegin(), index);
}

int DashboardItemModel::addItem(DashboardItem::DashboardType type, const QString &name)
{
    const int pos = mItems.size() - 1;
    DashboardItem *item = new DashboardItem(type, name);

    beginInsertRows(QModelIndex(), pos, pos);
    mItems.insert(pos, item);
    endInsertRows();

    return pos;
}

void DashboardItemModel::renameItem(const QString &previousName, const QString &newName)
{
    const auto item =
            std::find_if(mItems.begin(), mItems.end(), [previousName](const DashboardItem *item) {
                return item->name() == previousName;
            });

    if (item == mItems.end())
        return;

    (*item)->setName(newName);
    const auto index = createIndex(std::distance(mItems.begin(), item), 0);
    emit dataChanged(index, index, QList<int>() << DisplayNameRole);

    saveDashboardsToSettings();
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
    mItems.push_back(new DashboardItem(DashboardItem::DashboardType::Add));
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

DashboardItem::DashboardType DashboardItemModel::getCurrentDashboardType() const
{
    if (mCurrentIndex >= mItems.size())
        return DashboardItem::DashboardType::Unknown;

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

    return (mItems.at(index)->type() == DashboardItem::DashboardType::Add);
}

void DashboardItemModel::setCurrentIndex(uint index)
{
    if (index < mItems.size()) {
        mCurrentIndex = index;
    }
}

void DashboardItemModel::saveDashboardsToSettings() const
{
    // Prevent dashboards from being saved if there is only the "Add" button
    if (mItems.count() <= 1)
        return;

    QSettings settings;
    settings.remove(Constants::SettingsKey::LastDashboards);

    settings.beginWriteArray(Constants::SettingsKey::LastDashboards);
    for (qsizetype i = 0; i < mItems.count(); ++i) {
        if (mItems[i]->type() == DashboardItem::DashboardType::Add)
            continue;

        settings.setArrayIndex(i);
        settings.setValue(Constants::SettingsKey::Name, mItems[i]->name());
        settings.setValue(Constants::SettingsKey::Type, (int)mItems[i]->type());
        settings.setValue(Constants::SettingsKey::NodeIds, mItems[i]->getMonitoredNodeIds());

        if (mItems[i]->type() == DashboardItem::DashboardType::Events) {
            const auto eventFilters =
                    static_cast<MonitoredItemModel *>(mItems[i]->monitoredItemModel())
                            ->eventFilters();
            QList<QList<QOpcUaSimpleAttributeOperand>> selectClauses;
            for (const auto &filter : eventFilters)
                selectClauses.push_back(filter.selectClauses());
            settings.setValue(Constants::SettingsKey::EventFilters,
                              QVariant::fromValue(selectClauses));
        }
    }
    settings.endArray();
}
