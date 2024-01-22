/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCoreApplication>

#include "dashboarditem.h"
#include "monitoreditemmodel.h"

QString getDefaultNameForType(DashboardItem::DashboardType type)
{
    switch (type) {
    case DashboardItem::DashboardType::Variables:
        return QCoreApplication::translate("OpcUaBrowser", "Dashboard");
    case DashboardItem::DashboardType::Events:
        return QCoreApplication::translate("OpcUaBrowser", "Event");
    case DashboardItem::DashboardType::Add:
        return QCoreApplication::translate("OpcUaBrowser", "Add");
    default:
        break;
    }

    Q_UNREACHABLE();
    return QString();
}

DashboardItem::DashboardItem(DashboardItem::DashboardType type, const QString &name)
    : mName(name), mType(type)
{
    if (mName.isEmpty()) {
        mName = getDefaultNameForType(type);
    }

    if (type != DashboardItem::DashboardType::Add) {
        mMonitoredItemModel = new MonitoredItemModel();
    }
}

DashboardItem::~DashboardItem()
{
    mMonitoredItemModel->deleteLater();
}

const QString &DashboardItem::name() const noexcept
{
    return mName;
}

void DashboardItem::setName(const QString &name)
{
    if (name.isEmpty()) {
        mName = getDefaultNameForType(mType);
    } else {
        mName = name;
    }
}

DashboardItem::DashboardType DashboardItem::type() const noexcept
{
    return mType;
}

QAbstractListModel *DashboardItem::monitoredItemModel() const noexcept
{
    return mMonitoredItemModel;
}

QStringList DashboardItem::getMonitoredNodeIds() const
{
    if (mMonitoredItemModel == nullptr)
        return QStringList();

    return mMonitoredItemModel->getNodeIds();
}
