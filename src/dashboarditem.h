/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef DASHBOARDITEM_H
#define DASHBOARDITEM_H

#include <QObject>
#include <QQmlEngine>
#include <QString>

class MonitoredItemModel;
class QAbstractListModel;

class DashboardItem : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("DashboardItem is created by DashboardItemModel")

public:
    enum class DashboardType { Unknown = -1, Variables, Events, Add };
    Q_ENUM(DashboardType)

    explicit DashboardItem(DashboardItem::DashboardType type, const QString &name = QString());
    ~DashboardItem();

    const QString &name() const noexcept;
    void setName(const QString &name);

    DashboardItem::DashboardType type() const noexcept;
    QAbstractListModel *monitoredItemModel() const noexcept;
    QStringList getMonitoredNodeIds() const;

private:
    QString mName;
    DashboardItem::DashboardType mType;
    MonitoredItemModel *mMonitoredItemModel = nullptr;
};

#endif // DASHBOARDITEM_H
