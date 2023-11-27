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
    connect(this, &OpcUaModel::browsingForReferenceTypesFinished, this, [=]() {
        mBrowsedTypes.setFlag(EBrowseType::ReferenceTypes);
        if (mBrowsedTypes.testFlag(EBrowseType::DataTypes)) {
            resetModel();
        }
    });

    connect(this, &OpcUaModel::browsingForDataTypesFinished, this, [=]() {
        mBrowsedTypes.setFlag(EBrowseType::DataTypes);
        if (mBrowsedTypes.testFlag(EBrowseType::ReferenceTypes)) {
            resetModel();
        }
    });
}

void OpcUaModel::setOpcUaClient(QOpcUaClient *client)
{
    mCurrentIndex = QModelIndex();
    mBrowsedTypes = EBrowseType::None;
    mReferencesList.clear();
    mDataTypesList.clear();
    mOpcUaClient = client;

    if (nullptr != mOpcUaClient) {
        auto referencesNode =
                client->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::References));
        browseReferenceTypes(referencesNode);
        auto dataNode =
                client->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::BaseDataType));
        browseDataTypes(dataNode);
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

QString OpcUaModel::getStringForDataTypeId(const QString &dataTypeId) const
{
    static QString emptyString;
    return mDataTypesList.value(dataTypeId, emptyString);
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

void OpcUaModel::setCurrentNodeId(const QString &nodeId)
{
    if (nodeId.isEmpty())
        return;

    const auto indices =
            match(index(0, 0), NodeIdRole, nodeId, 1, Qt::MatchExactly | Qt::MatchRecursive);

    if (indices.isEmpty()) {
        qWarning() << QStringLiteral("No index for node %1 found").arg(nodeId);
    } else {
        setCurrentIndex(indices.constFirst());
    }
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

    emit currentIndexChanged(index);
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

    if (nullptr != mRootItem.get()) {
        setCurrentIndex(index(0, 0));
    }
}

void OpcUaModel::browseReferenceTypes(QOpcUaNode *node)
{
    static int cntNodes = 0;
    static QStringList knownNodeIds;
    static const QString formattedReferenceString = QStringLiteral("%1 (%2)");

    auto deleteNode = [=](QOpcUaNode *n) {
        n->deleteLater();
        --cntNodes;

        if (0 == cntNodes) {
            knownNodeIds.clear();
            // all reference type nodes have been read
            emit browsingForReferenceTypesFinished();
        }
    };

    connect(node, &QOpcUaNode::attributeRead, this, [=](const QOpcUa::NodeAttributes &attributes) {
        QString nodeId;
        if (attributes.testFlag(QOpcUa::NodeAttribute::NodeId)) {
            nodeId = QOpcUaHelper::getRawAttributeValue(node, QOpcUa::NodeAttribute::NodeId);
        }

        QString displayName;
        if (attributes.testFlag(QOpcUa::NodeAttribute::DisplayName)) {
            displayName = formattedReferenceString.arg(
                    QOpcUaHelper::getRawAttributeValue(node, QOpcUa::NodeAttribute::DisplayName),
                    nodeId);
        }

        QString inverseName;
        if (attributes.testFlag(QOpcUa::NodeAttribute::InverseName)) {
            inverseName = formattedReferenceString.arg(
                    QOpcUaHelper::getRawAttributeValue(node, QOpcUa::NodeAttribute::InverseName),
                    nodeId);
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

                    if (knownNodeIds.contains(childNode->nodeId())) {
                        childNode->deleteLater();
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

    knownNodeIds << node->nodeId();
    cntNodes++;
    // First step: browse for children
    if (!node->browseChildren(QOpcUa::ReferenceTypeId::HasSubtype,
                              QOpcUa::NodeClass::ReferenceType)) {
        qWarning() << "Browsing node" << node->nodeId() << "failed";
        deleteNode(node);
    }
}

void OpcUaModel::browseDataTypes(QOpcUaNode *node)
{
    static int cntNodes = 0;
    static QStringList knownNodeIds;
    static const QString formattedDataString = QStringLiteral("%1 (%2)");

    auto deleteNode = [=](QOpcUaNode *n) {
        n->deleteLater();
        --cntNodes;

        if (0 == cntNodes) {
            knownNodeIds.clear();
            // all reference type nodes have been read
            emit browsingForDataTypesFinished();
        }
    };

    connect(node, &QOpcUaNode::attributeRead, this, [=](const QOpcUa::NodeAttributes &attributes) {
        QString nodeId;
        if (attributes.testFlag(QOpcUa::NodeAttribute::NodeId)) {
            nodeId = QOpcUaHelper::getRawAttributeValue(node, QOpcUa::NodeAttribute::NodeId);
        }

        if (!nodeId.isEmpty()) {
            QString displayName;
            if (attributes.testFlag(QOpcUa::NodeAttribute::DisplayName)) {
                displayName =
                        formattedDataString.arg(QOpcUaHelper::getRawAttributeValue(
                                                        node, QOpcUa::NodeAttribute::DisplayName),
                                                nodeId);
            }

            mDataTypesList[nodeId] = displayName;
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

                    if (knownNodeIds.contains(childNode->nodeId())) {
                        childNode->deleteLater();
                        continue;
                    }

                    browseDataTypes(childNode);
                }

                // Second step: read attributes
                if (!node->readAttributes(QOpcUa::NodeAttribute::NodeId
                                          | QOpcUa::NodeAttribute::DisplayName)) {
                    qWarning() << "Reading attributes" << node->nodeId() << "failed";
                    deleteNode(node);
                }
            });

    knownNodeIds << node->nodeId();
    cntNodes++;
    // First step: browse for children
    if (!node->browseChildren(QOpcUa::ReferenceTypeId::HasSubtype, QOpcUa::NodeClass::DataType)) {
        qWarning() << "Browsing node" << node->nodeId() << "failed";
        deleteNode(node);
    }
}
