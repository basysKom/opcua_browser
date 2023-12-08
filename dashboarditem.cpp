#include <QCoreApplication>

#include "dashboarditem.h"
#include "monitoreditemmodel.h"

QString getDefaultNameForType(DashboardItem::Type type)
{
    switch (type) {
    case DashboardItem::Type::Variables:
        return QCoreApplication::translate("OpcUaBrowser", "Dashboard");
    case DashboardItem::Type::Events:
        return QCoreApplication::translate("OpcUaBrowser", "Event");
    case DashboardItem::Type::Add:
        return QCoreApplication::translate("OpcUaBrowser", "Add");
    }

    Q_UNREACHABLE();
    return QString();
}

DashboardItem::DashboardItem(Type type, const QString &name) : mName(name), mType(type)
{
    if (mName.isEmpty()) {
        mName = getDefaultNameForType(type);
    }

    if (type != Type::Add) {
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

DashboardItem::Type DashboardItem::type() const noexcept
{
    return mType;
}

QAbstractListModel *DashboardItem::monitoredItemModel() const noexcept
{
    return mMonitoredItemModel;
}
