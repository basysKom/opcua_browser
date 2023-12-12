#ifndef DASHBOARDITEM_H
#define DASHBOARDITEM_H

#include <QObject>
#include <QQmlEngine>
#include <QString>

#include "types.h"

class MonitoredItemModel;
class QAbstractListModel;

class DashboardItem : public QObject
{
public:
    explicit DashboardItem(Types::DashboardType type, const QString &name = QString());
    ~DashboardItem();

    const QString &name() const noexcept;
    void setName(const QString &name);

    Types::DashboardType type() const noexcept;
    QAbstractListModel *monitoredItemModel() const noexcept;

private:
    QString mName;
    Types::DashboardType mType;
    MonitoredItemModel *mMonitoredItemModel = nullptr;
};

#endif // DASHBOARDITEM_H
