/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MONITOREDITEMMODEL_H
#define MONITOREDITEMMODEL_H

#include <QAbstractListModel>
#include <QOpcUaMonitoringParameters>

class QOpcUaNode;
class MonitoredItem;

class MonitoredItemModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit MonitoredItemModel(QObject *parent = nullptr);
    ~MonitoredItemModel();

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    bool containsItem(const QString &nodeId) const noexcept;
    void addItem(QOpcUaNode *node);
    void addEventItem(QOpcUaNode *node, const QOpcUaMonitoringParameters::EventFilter &eventFilter);
    void clearItems();

    Q_INVOKABLE void clearEventsForItem(int index);

    QStringList getNodeIds() const;

    Q_INVOKABLE void disableMonitoring(int index);

    QList<QOpcUaMonitoringParameters::EventFilter> eventFilters();

    Q_INVOKABLE void moveItem(int currentIndex, int newIndex);

signals:
    void updated();

private:
    QList<MonitoredItem *> mItems;
};

#endif // MONITOREDITEMMODEL_H
