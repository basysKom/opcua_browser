#ifndef DASHBOARDITEM_H
#define DASHBOARDITEM_H

#include <QObject>
#include <QString>

class MonitoredItemModel;
class QAbstractListModel;

class DashboardItem : public QObject
{
public:
    enum class Type { Variables, Events, Add };

    explicit DashboardItem(Type type, const QString &name = QString());
    DashboardItem(const DashboardItem &item);
    ~DashboardItem();

    const QString &name() const noexcept;
    void setName(const QString &name);

    Type type() const noexcept;
    QAbstractListModel *monitoredItemModel() const noexcept;

private:
    QString mName;
    Type mType;
    MonitoredItemModel *mMonitoredItemModel = nullptr;
};

#endif // DASHBOARDITEM_H
