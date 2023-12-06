#ifndef MONITOREDITEMMODEL_H
#define MONITOREDITEMMODEL_H

#include <QAbstractListModel>

#include <QOpcUaNode>

#include "monitoreditem.h"

class TreeItem;

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
    void clearItems();

    QStringList getNodeIds() const;

    Q_INVOKABLE void disableMonitoring(int index);

private:
    QList<MonitoredItem *> mItems;
};

#endif // MONITOREDITEMMODEL_H
