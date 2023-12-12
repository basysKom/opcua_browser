#ifndef DASHBOARDITEMMODEL_H
#define DASHBOARDITEMMODEL_H

#include <QAbstractListModel>

#include "dashboarditem.h"

class DashboardItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DashboardItemModel(QObject *parent = nullptr);
    ~DashboardItemModel();

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    bool containsItem(const QString &name) const noexcept;
    Q_INVOKABLE void addItem(Types::DashboardType type, const QString &name = QString());
    void clearItems();

    MonitoredItemModel *getCurrentMonitoredItemModel() const;
    Types::DashboardType getCurrentDashboardType() const;
    void setCurrentDashboardName(const QString &name);

    Q_INVOKABLE bool isAddItem(uint index) const;
    Q_INVOKABLE void setCurrentIndex(uint index);

private:
    QList<DashboardItem *> mItems;
    quint32 mCurrentIndex = 0;
};

#endif // DASHBOARDITEMMODEL_H
