#include <QCoreApplication>

#include "dashboarditem.h"
#include "monitoreditemmodel.h"

QString getDefaultNameForType(Types::DashboardType type)
{
    switch (type) {
    case Types::DashboardType::Variables:
        return QCoreApplication::translate("OpcUaBrowser", "Dashboard");
    case Types::DashboardType::Events:
        return QCoreApplication::translate("OpcUaBrowser", "Event");
    case Types::DashboardType::Add:
        return QCoreApplication::translate("OpcUaBrowser", "Add");
    default:
        break;
    }

    Q_UNREACHABLE();
    return QString();
}

DashboardItem::DashboardItem(Types::DashboardType type, const QString &name)
    : mName(name), mType(type)
{
    if (mName.isEmpty()) {
        mName = getDefaultNameForType(type);
    }

    if (type != Types::DashboardType::Add) {
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

Types::DashboardType DashboardItem::type() const noexcept
{
    return mType;
}

QAbstractListModel *DashboardItem::monitoredItemModel() const noexcept
{
    return mMonitoredItemModel;
}
