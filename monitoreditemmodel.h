#ifndef MONITOREDITEMMODEL_H
#define MONITOREDITEMMODEL_H

#include <QAbstractListModel>

class TreeItem;

class MonitoredItemModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit MonitoredItemModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    void addItem(TreeItem *item);
    void removeItem(TreeItem *item);
    void setItems(const QList<TreeItem *> &items);

    void valueChanged(TreeItem *item);

    Q_INVOKABLE void disableMonitoring(int index);

private:
    QList<TreeItem *> mItems;
};

#endif // MONITOREDITEMMODEL_H
