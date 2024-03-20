/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCoreApplication>

#include "dashboarditem.h"
#include "monitoreditemmodel.h"

DashboardItem::DashboardItem(DashboardItem::DashboardType type, const QString &name)
    : mName(name), mType(type)
{
    if (type != DashboardItem::DashboardType::Add) {
        mMonitoredItemModel = new MonitoredItemModel();

        QObject::connect(mMonitoredItemModel, &MonitoredItemModel::updated, this, [this]() {
            if (!mHasBackgroundActivity && !mIsCurrentItem) {
                mHasBackgroundActivity = true;
                emit hasBackgroundActivityChanged();
            }
        });
    }
}

DashboardItem::~DashboardItem()
{
    if (mMonitoredItemModel)
        mMonitoredItemModel->deleteLater();
}

const QString &DashboardItem::name() const noexcept
{
    return mName;
}

void DashboardItem::setName(const QString &name)
{
    mName = name;
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

bool DashboardItem::hasBackgroundActivity() const
{
    return mHasBackgroundActivity;
}

void DashboardItem::setIsCurrentItem(bool isCurrentItem)
{
    if (isCurrentItem != mIsCurrentItem) {
        mIsCurrentItem = isCurrentItem;

        if (isCurrentItem && mHasBackgroundActivity) {
            mHasBackgroundActivity = false;
            emit hasBackgroundActivityChanged();
        }
    }
}
