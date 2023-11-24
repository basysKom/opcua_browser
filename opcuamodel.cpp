#include <QColor>

#include <QOpcUaClient>
#include <QOpcUaNode>

#include "opcuamodel.h"
#include "opcuahelper.h"

enum Roles : int {
    ColorRole = Qt::UserRole,
    ValueRole,
    NodeIdRole,
    AttributesRole,
    ReferencesRole,
    SelectedRole,
    CanMonitoringRole
};

QHash<int, QByteArray> OpcUaModel::roleNames() const
{
    auto names = QAbstractItemModel::roleNames();
    names[ColorRole] = "color";
    names[ValueRole] = "value";
    names[NodeIdRole] = "nodeId";
    names[AttributesRole] = "attributes";
    names[ReferencesRole] = "references";
    names[SelectedRole] = "isSelected";
    names[CanMonitoringRole] = "canMonitoring";
    return names;
}

OpcUaModel::OpcUaModel(QObject *parent) : QAbstractItemModel{ parent }
{
    connect(this, &OpcUaModel::browsingForReferenceTypesFinished, this, [=]() { resetModel(); });
}

void OpcUaModel::setOpcUaClient(QOpcUaClient *client)
{
    mCurrentIndex = QModelIndex();
    mReferencesList.clear();
    mOpcUaClient = client;

    if (nullptr != mOpcUaClient) {
        auto referencesNode =
                client->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::References));
        browseReferenceTypes(referencesNode);
    } else {
        resetModel();
    }
}

QOpcUaClient *OpcUaModel::opcUaClient() const noexcept
{
    return mOpcUaClient;
}

QString OpcUaModel::getStringForRefTypeId(const QString &refTypeId, bool isForward) const
{
    if (!mReferencesList.contains(refTypeId)) {
        static QString emptyString;
        return emptyString;
    }

    return isForward ? mReferencesList[refTypeId].first : mReferencesList[refTypeId].second;
}

QVariant OpcUaModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

    switch (role) {
    case Qt::DisplayRole:
        return item->displayName();
    case ColorRole:
        return item->nodeClassColor();
    case ValueRole:
        return item->value();
    case NodeIdRole:
        return item->nodeId();
    case AttributesRole:
        return QVariant::fromValue<QObject *>(item->attributes());
    case ReferencesRole:
        return QVariant::fromValue<QObject *>(item->references());
    case SelectedRole:
        return (index == mCurrentIndex);
    case CanMonitoringRole:
        return item->canMonitored();
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
            ? static_cast<TreeItem *>(parent.internalPointer())->child(row)
            : mRootItem.get();

    return item ? createIndex(row, column, item) : QModelIndex();
}

QModelIndex OpcUaModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto childItem = static_cast<TreeItem *>(index.internalPointer());
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

    auto parentItem = static_cast<TreeItem *>(parent.internalPointer());
    return parentItem ? parentItem->childCount() : 0;
}

int OpcUaModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

void OpcUaModel::setCurrentIndex(const QModelIndex &index)
{
    if (!index.isValid() || (index == mCurrentIndex))
        return;

    auto treeItem = static_cast<TreeItem *>(index.internalPointer());
    if (nullptr != treeItem) {
        treeItem->refreshAttributes();
    }

    const auto lastCurrentIndex = mCurrentIndex;
    mCurrentIndex = index;
    emit dataChanged(index, index, QList<int>() << SelectedRole);
    if (lastCurrentIndex.isValid())
        emit dataChanged(lastCurrentIndex, lastCurrentIndex, QList<int>() << SelectedRole);
}

void OpcUaModel::refreshIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    auto treeItem = static_cast<TreeItem *>(index.internalPointer());
    if (nullptr != treeItem) {
        treeItem->refresh();
    }
}

void OpcUaModel::refreshAttributesForCurrentIndex()
{
    if (!mCurrentIndex.isValid())
        return;

    auto treeItem = static_cast<TreeItem *>(mCurrentIndex.internalPointer());
    if (nullptr != treeItem) {
        treeItem->refreshAttributes();
    }
}

void OpcUaModel::resetModel()
{
    beginResetModel();
    if (nullptr == mOpcUaClient) {
        mRootItem.reset();
    } else {
        mRootItem.reset(new TreeItem(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::RootFolder),
                                     this, QOpcUa::NodeClass::Object, nullptr));
    }
    endResetModel();
}

void OpcUaModel::browseReferenceTypes(QOpcUaNode *node)
{
    static int cntNodes = 0;

    auto deleteNode = [=](QOpcUaNode *n) {
        n->deleteLater();
        --cntNodes;

        if (0 == cntNodes) {
            // all reference type nodes have been read
            emit browsingForReferenceTypesFinished();
        }
    };

    connect(node, &QOpcUaNode::attributeRead, this, [=](const QOpcUa::NodeAttributes &attributes) {
        QString nodeId;
        if (attributes.testFlag(QOpcUa::NodeAttribute::NodeId)) {
            nodeId = QOpcUaHelper::getAttributeValue(node, QOpcUa::NodeAttribute::NodeId);
        }

        QString displayName;
        if (attributes.testFlag(QOpcUa::NodeAttribute::DisplayName)) {
            displayName = QOpcUaHelper::getAttributeValue(node, QOpcUa::NodeAttribute::DisplayName);
        }

        QString inverseName;
        if (attributes.testFlag(QOpcUa::NodeAttribute::InverseName)) {
            inverseName = QOpcUaHelper::getAttributeValue(node, QOpcUa::NodeAttribute::InverseName);
        }

        if (!nodeId.isEmpty()) {
            mReferencesList[nodeId] = std::make_pair(displayName, inverseName);
        }

        // Third step: delete node
        deleteNode(node);
    });

    connect(node, &QOpcUaNode::browseFinished, this,
            [=](const QList<QOpcUaReferenceDescription> &children,
                QOpcUa::UaStatusCode statusCode) {
                if (nullptr == mOpcUaClient) {
                    qWarning() << "OPC UA client is null" << node->nodeId();
                    deleteNode(node);
                    return;
                }

                for (const auto &item : children) {
                    auto childNode = mOpcUaClient->node(item.targetNodeId());
                    if (!childNode) {
                        qWarning() << "Failed to instantiate node:" << item.targetNodeId().nodeId();
                        continue;
                    }

                    browseReferenceTypes(childNode);
                }

                // Second step: read attributes
                if (!node->readAttributes(QOpcUa::NodeAttribute::NodeId
                                          | QOpcUa::NodeAttribute::DisplayName
                                          | QOpcUa::NodeAttribute::InverseName)) {
                    qWarning() << "Reading attributes" << node->nodeId() << "failed";
                    deleteNode(node);
                }
            });

    cntNodes++;
    // First step: browse for children
    if (!node->browseChildren(QOpcUa::ReferenceTypeId::HierarchicalReferences,
                              QOpcUa::NodeClass::ReferenceType)) {
        qWarning() << "Browsing node" << node->nodeId() << "failed";
        deleteNode(node);
    }
}
