/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef DASHBOARDITEMMODEL_H
#define DASHBOARDITEMMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>

#include "dashboarditem.h"

class MonitoredItemModel;

class DashboardItemModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit DashboardItemModel(QObject *parent = nullptr);
    ~DashboardItemModel();

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    bool containsItem(const QString &name) const noexcept;
    Q_INVOKABLE int addItem(DashboardItem::DashboardType type, const QString &name = QString());
    Q_INVOKABLE void removeItem(int index);
    void clearItems();

    MonitoredItemModel *getMonitoredItemModel(int index) const;
    MonitoredItemModel *getCurrentMonitoredItemModel() const;
    DashboardItem::DashboardType getCurrentDashboardType() const;
    void setCurrentDashboardName(const QString &name);

    Q_INVOKABLE bool isAddItem(uint index) const;
    Q_INVOKABLE void setCurrentIndex(uint index);

    void saveDashboardsToSettings() const;

private:
    QList<DashboardItem *> mItems;
    quint32 mCurrentIndex = 0;
};

#endif // DASHBOARDITEMMODEL_H
