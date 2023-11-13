#include <QColor>

#include "opcuamodel.h"
#include "treeitem.h"

enum Roles : int {
    ColorRole = Qt::UserRole,
    AttributesRole,
    ReferencesRole,
    MonitoredAttributesRole,
    SelectedRole
};

QHash<int, QByteArray> OpcUaModel::roleNames() const {
    auto names = QAbstractItemModel::roleNames();
    names[ColorRole] = "color";
    names[AttributesRole] = "attributes";
    names[ReferencesRole] = "references";
    names[MonitoredAttributesRole] = "monitoredAttributes";
    names[SelectedRole] = "isSelected";
    return names;
}

OpcUaModel::OpcUaModel(QObject *parent)
    : QAbstractItemModel{parent}
{

}

void OpcUaModel::setOpcUaClient(QOpcUaClient *client)
{
    beginResetModel();
    mOpcUaClient = client;
    if (mOpcUaClient)
        mRootItem.reset(new TreeItem(client->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::RootFolder)), this, QOpcUa::NodeClass::Object, nullptr));
    else
        mRootItem.reset(nullptr);
    endResetModel();
}

QOpcUaClient *OpcUaModel::opcUaClient() const
{
    return mOpcUaClient;
}

QVariant OpcUaModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto item = static_cast<TreeItem *>(index.internalPointer());

    switch (role) {
    case Qt::DisplayRole:
        return item->displayName();
    case ColorRole:
        return item->nodeClassColor();
    case AttributesRole:
        return QVariant::fromValue<QObject *>(item->attributes());
    case ReferencesRole:
        return QVariant::fromValue<QObject *>(item->references());
    case MonitoredAttributesRole:
        return QVariant::fromValue<QObject *>(item->monitoredAttributes());
    case SelectedRole:
        return (index == mCurrentIndex);
    default:
        break;
    }

    return QVariant();
}

QModelIndex OpcUaModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *item = parent.isValid()
                         ? static_cast<TreeItem*>(parent.internalPointer())->child(row)
                         : mRootItem.get();

    return item ? createIndex(row, column, item) : QModelIndex();
}

QModelIndex OpcUaModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto childItem = static_cast<TreeItem*>(index.internalPointer());
    auto parentItem = childItem->parentItem();

    if (childItem == mRootItem.get() || !parentItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int OpcUaModel::rowCount(const QModelIndex &parent) const
{
    if (!mOpcUaClient)
        return 0;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return 1; // only one root item

    auto parentItem = static_cast<TreeItem*>(parent.internalPointer());
    return parentItem ? parentItem->childCount() : 0;
}

int OpcUaModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

void OpcUaModel::setCurrentIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    auto treeItem = static_cast<TreeItem*>(index.internalPointer());
    if (nullptr != treeItem) {
        treeItem->refreshAttributes();
    }

    const auto lastCurrentIndex = mCurrentIndex;
    mCurrentIndex = index;
    emit dataChanged(lastCurrentIndex, lastCurrentIndex, QList<int>() << SelectedRole);
    emit dataChanged(index, index, QList<int>() << SelectedRole);
}

void OpcUaModel::refreshIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    auto treeItem = static_cast<TreeItem*>(index.internalPointer());
    if (nullptr != treeItem) {
        treeItem->refresh();
    }
}
