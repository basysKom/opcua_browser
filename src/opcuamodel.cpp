/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QColor>
#include <QLoggingCategory>
#include <QTimer>

#include <QOpcUaClient>
#include <QOpcUaExtensionObject>
#include <QOpcUaNode>

#include "opcuamodel.h"
#include "opcuahelper.h"

#ifdef HAS_GENERIC_STRUCT_HANDLER
#  include <QOpcUaGenericStructValue>
#endif

Q_LOGGING_CATEGORY(opcuaModelLog, "opcua_browser.model");

enum Roles : int {
    ColorRole = Qt::UserRole,
    ValueRole,
    NodeIdRole,
    AttributesRole,
    ReferencesRole,
    SelectedRole,
    CurrentItemRole,
    CanMonitoringRole,
    HasEventNotifierRole
};

QHash<int, QByteArray> OpcUaModel::roleNames() const
{
    auto names = QAbstractItemModel::roleNames();
    names[ColorRole] = "indicatorColor";
    names[ValueRole] = "value";
    names[NodeIdRole] = "nodeId";
    names[AttributesRole] = "attributes";
    names[ReferencesRole] = "references";
    names[SelectedRole] = "isSelected";
    names[CurrentItemRole] = "isCurrentItem";
    names[CanMonitoringRole] = "canMonitoring";
    names[HasEventNotifierRole] = "hasEventNotifier";
    return names;
}

OpcUaModel::OpcUaModel(QObject *parent) : QAbstractItemModel{ parent }
{
    connect(this, &OpcUaModel::browsingForReferenceTypesFinished, this, [=]() {
        mBrowsedTypes.setFlag(EBrowseType::ReferenceTypes);
        if (allLookupsFinished()) {
            resetModel();
        }
    });

    connect(this, &OpcUaModel::browsingForDataTypesFinished, this, [=]() {
        mBrowsedTypes.setFlag(EBrowseType::DataTypes);
        if (allLookupsFinished()) {
            resetModel();
        }
    });

    connect(this, &OpcUaModel::browsingForEnumStringsFinished, this, [=]() {
        mBrowsedTypes.setFlag(EBrowseType::EnumStrings);
        if (allLookupsFinished()) {
            resetModel();
        }
    });

    connect(this, &OpcUaModel::rowsInserted, this,
            [=](const QModelIndex &parent, int first, int last) {
                // if mSelectedNodeId is not empty refresh (browse) inverse nodes to find
                // mSelectedNodeId
                if (!parent.isValid() || mCurrentNodeId.isEmpty())
                    return;

                for (int i = first; i <= last; i++) {
                    auto item = static_cast<TreeItem *>(parent.internalPointer())->child(i);
                    if (item == nullptr)
                        continue;

                    const QString nodeId = item->nodeId();
                    if (mCurrentNodeId == nodeId) {
                        mCurrentNodeId = QString();
                        mInverseNodeIds.clear();
                        item->refresh();
                        // Workaround, otherwise the item is displayed twice in the list
                        QTimer::singleShot(10, this,
                                           [=]() { setCurrentIndex(index(i, 0, parent)); });
                        return;
                    }

                    if (mInverseNodeIds.contains(nodeId)) {
                        mInverseNodeIds.removeAll(nodeId);
                        item->refresh();
                    }
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

#ifdef HAS_GENERIC_STRUCT_HANDLER
        mGenericStructHandler.reset(new QOpcUaGenericStructHandler(mOpcUaClient));
        QObject::connect(mGenericStructHandler.get(),
                         &QOpcUaGenericStructHandler::initializedChanged, this,
                         [=](bool initialized) {
                             Q_UNUSED(initialized)
                             mBrowsedTypes.setFlag(EBrowseType::GenericStructs);

                             // The generic struct handler must be initialized before enum values
                             // can be processed
                             auto enumerationNode = client->node(QOpcUa::namespace0Id(
                                     QOpcUa::NodeIds::Namespace0::Enumeration));
                             browseEnumStrings(enumerationNode);
                         });
        mGenericStructHandler->initialize();
#else
        auto enumerationNode =
                client->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::Enumeration));
        browseEnumStrings(enumerationNode);
#endif
    } else {
        resetModel();
    }
}

QOpcUaClient *OpcUaModel::opcUaClient() const noexcept
{
    return mOpcUaClient;
}

bool OpcUaModel::isHierarchicalReference(const QString &refTypeId) const
{
    if (!mReferencesList.contains(refTypeId))
        return false;

    return mReferencesList[refTypeId].mIsHierarchicalReference;
}

QString OpcUaModel::getStringForRefTypeId(const QString &refTypeId, bool isForward) const
{
    if (!mReferencesList.contains(refTypeId)) {
        static QString emptyString;
        return emptyString;
    }

    return isForward ? mReferencesList[refTypeId].mDisplayName
                     : mReferencesList[refTypeId].mInverseName;
}

QString OpcUaModel::getStringForDataTypeId(const QString &dataTypeId) const
{
    static QString emptyString;
    return mDataTypesList.value(dataTypeId, emptyString);
}

QHash<qint32, QString> OpcUaModel::getEnumStringsForDataTypeId(const QString &dataTypeId)
{
    const auto entry = mEnumStringsList.constFind(dataTypeId);
    if (entry == mEnumStringsList.constEnd())
        return {};

    return entry.value();
}

#ifdef HAS_GENERIC_STRUCT_HANDLER
QOpcUaGenericStructHandler *OpcUaModel::genericStructHandler() const
{
    return mGenericStructHandler.get();
}
#endif

bool OpcUaModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || (role != SelectedRole))
        return false;

    const TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
    if (item != nullptr) {
        const QString nodeId = item->nodeId();
        const bool isSelected = value.toBool();
        if (isSelected && !mSelectedNodeIds.contains(nodeId)) {
            mSelectedNodeIds << nodeId;
            emit dataChanged(index, index, QList<int>() << SelectedRole);
        } else if (!isSelected && mSelectedNodeIds.contains(nodeId)) {
            mSelectedNodeIds.removeAll(nodeId);
            emit dataChanged(index, index, QList<int>() << SelectedRole);
        }
        emit hasSelectedItemsChanged();
    }

    return true;
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
        return mSelectedNodeIds.contains(item->nodeId());
    case CurrentItemRole:
        return (index == mCurrentIndex);
    case CanMonitoringRole:
        return item->canMonitored();
    case HasEventNotifierRole:
        return item->hasEventNotifier();
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

int OpcUaModel::columnCount(const QModelIndex &) const
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
        mCurrentNodeId = nodeId;
        collectInverseNodeIds(nodeId, true);
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
    emit dataChanged(index, index, QList<int>() << CurrentItemRole);
    if (lastCurrentIndex.isValid())
        emit dataChanged(lastCurrentIndex, lastCurrentIndex, QList<int>() << CurrentItemRole);

    emit currentIndexChanged(index);
}

void OpcUaModel::refreshCurrentIndex()
{
    if (!mCurrentIndex.isValid())
        return;

    auto treeItem = static_cast<TreeItem *>(mCurrentIndex.internalPointer());
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

void OpcUaModel::clearSelectionList()
{
    const QStringList copyList = mSelectedNodeIds;
    for (const auto &nodeId : copyList) {
        const auto foundIndices =
                match(index(0, 0), NodeIdRole, nodeId, 1, Qt::MatchExactly | Qt::MatchRecursive);
        if (!foundIndices.isEmpty()) {
            setData(foundIndices.constFirst(), false, SelectedRole);
        }
    }

    // after resetting the selected nodes, the list mSelectedNodeIds should be empty
    Q_ASSERT(mSelectedNodeIds.isEmpty());
    mSelectedNodeIds.clear();
    emit hasSelectedItemsChanged();
}

bool OpcUaModel::hasSelectedItems() const noexcept
{
    return !mSelectedNodeIds.isEmpty();
}

const QStringList &OpcUaModel::selectedNodes() const noexcept
{
    return mSelectedNodeIds;
}

void OpcUaModel::resetModel()
{
    mSelectedNodeIds.clear();
    emit hasSelectedItemsChanged();

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

void OpcUaModel::collectInverseNodeIds(const QString &nodeId, bool init)
{
    static bool foundKnownNode = false;
    static QStringList browsedNodesIds;

    if (init) {
        foundKnownNode = false;
        browsedNodesIds.clear();
    }

    auto node = opcUaClient()->node(nodeId);
    if (nullptr == node)
        return;

    connect(node, &QOpcUaNode::browseFinished, this,
            [=](const QList<QOpcUaReferenceDescription> &refNodes,
                QOpcUa::UaStatusCode statusCode) {
                // stop if node was found
                if (foundKnownNode || mCurrentNodeId.isEmpty()) {
                    node->deleteLater();
                    return;
                }

                if (statusCode != QOpcUa::Good) {
                    qCWarning(opcuaModelLog)
                            << "Browsing node" << node->nodeId() << "finally failed:" << statusCode;
                    node->deleteLater();
                    return;
                }

                for (const auto &item : refNodes) {
                    if (item.isForwardReference())
                        continue;

                    const QString refNodeId = item.targetNodeId().nodeId();
                    if (browsedNodesIds.contains(refNodeId))
                        continue;

                    const auto indices = match(index(0, 0), NodeIdRole, refNodeId, 1,
                                               Qt::MatchExactly | Qt::MatchRecursive);

                    if (indices.isEmpty()) {
                        collectInverseNodeIds(refNodeId);
                    } else {
                        const QModelIndex index = indices.constFirst();
                        if (index.isValid()) {
                            auto treeItem = static_cast<TreeItem *>(index.internalPointer());
                            if (treeItem != nullptr) {
                                foundKnownNode = true;
                                browsedNodesIds.removeAll(treeItem->nodeId());
                                mInverseNodeIds = browsedNodesIds;
                                treeItem->refresh();
                            }
                        }
                    }
                }

                node->deleteLater();
            });

    browsedNodesIds.push_front(node->nodeId());

    QOpcUaBrowseRequest request;
    request.setBrowseDirection(QOpcUaBrowseRequest::BrowseDirection::Inverse);
    request.setReferenceTypeId(QOpcUa::ReferenceTypeId::HierarchicalReferences);
    request.setIncludeSubtypes(true);
    if (!node->browse(request)) {
        qCWarning(opcuaModelLog) << "Browsing node" << node->nodeId() << "failed";
        node->deleteLater();
        return;
    }
}

void OpcUaModel::browseReferenceTypes(QOpcUaNode *node, bool isHierachical)
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
            mReferencesList[nodeId] = ReferenceType(displayName, inverseName, isHierachical);
        }

        // Third step: delete node
        deleteNode(node);
    });

    connect(node, &QOpcUaNode::browseFinished, this,
            [=](const QList<QOpcUaReferenceDescription> &children, QOpcUa::UaStatusCode) {
                if (nullptr == mOpcUaClient) {
                    qCWarning(opcuaModelLog) << "OPC UA client is null" << node->nodeId();
                    deleteNode(node);
                    return;
                }

                for (const auto &item : children) {
                    auto childNode = mOpcUaClient->node(item.targetNodeId());
                    if (!childNode) {
                        qCWarning(opcuaModelLog)
                                << "Failed to instantiate node:" << item.targetNodeId().nodeId();
                        continue;
                    }

                    if (knownNodeIds.contains(childNode->nodeId())) {
                        childNode->deleteLater();
                        continue;
                    }

                    const bool isHierarchicalReferencesNode =
                            (childNode->nodeId()
                             == nodeIdFromReferenceType(
                                     QOpcUa::ReferenceTypeId::HierarchicalReferences));
                    browseReferenceTypes(childNode, isHierachical || isHierarchicalReferencesNode);
                }

                // Second step: read attributes
                if (!node->readAttributes(QOpcUa::NodeAttribute::NodeId
                                          | QOpcUa::NodeAttribute::DisplayName
                                          | QOpcUa::NodeAttribute::InverseName)) {
                    qCWarning(opcuaModelLog) << "Reading attributes" << node->nodeId() << "failed";
                    deleteNode(node);
                }
            });

    knownNodeIds << node->nodeId();
    cntNodes++;
    // First step: browse for children
    if (!node->browseChildren(QOpcUa::ReferenceTypeId::HasSubtype,
                              QOpcUa::NodeClass::ReferenceType)) {
        qCWarning(opcuaModelLog) << "Browsing node" << node->nodeId() << "failed";
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
            [=](const QList<QOpcUaReferenceDescription> &children, QOpcUa::UaStatusCode) {
                if (nullptr == mOpcUaClient) {
                    qCWarning(opcuaModelLog) << "OPC UA client is null" << node->nodeId();
                    deleteNode(node);
                    return;
                }

                for (const auto &item : children) {
                    const QString nodeId = item.targetNodeId().nodeId();
                    mDataTypesList[nodeId] =
                            formattedDataString.arg(item.displayName().text(), nodeId);

                    auto childNode = mOpcUaClient->node(item.targetNodeId());
                    if (!childNode) {
                        qCWarning(opcuaModelLog)
                                << "Failed to instantiate node:" << item.targetNodeId().nodeId();
                        continue;
                    }

                    if (knownNodeIds.contains(childNode->nodeId())) {
                        childNode->deleteLater();
                        continue;
                    }

                    browseDataTypes(childNode);
                }

                // Second step: delete node
                deleteNode(node);
            });

    knownNodeIds << node->nodeId();
    cntNodes++;
    // First step: browse for children
    if (!node->browseChildren(QOpcUa::ReferenceTypeId::HasSubtype, QOpcUa::NodeClass::DataType)) {
        qCWarning(opcuaModelLog) << "Browsing node" << node->nodeId() << "failed";
        deleteNode(node);
    }
}

void OpcUaModel::browseEnumStrings(QOpcUaNode *node)
{
    static int cntNodes = 0;
    static QStringList knownNodeIds;

    auto deleteNode = [=](QOpcUaNode *n) {
        n->deleteLater();
        --cntNodes;

        if (0 == cntNodes) {
            knownNodeIds.clear();
            // all enum values nodes have been read
            emit browsingForEnumStringsFinished();
        }
    };

    connect(node, &QOpcUaNode::browseFinished, this,
            [=](const QList<QOpcUaReferenceDescription> &children, QOpcUa::UaStatusCode) {
                if (nullptr == mOpcUaClient) {
                    qCWarning(opcuaModelLog) << "OPC UA client is null" << node->nodeId();
                    deleteNode(node);
                    return;
                }

                for (const auto &item : children) {
                    if (knownNodeIds.contains(item.targetNodeId().nodeId()))
                        continue;

                    // We only care about subtypes or their EnumStrings/EnumValues properties
                    if (item.nodeClass() != QOpcUa::NodeClass::DataType
                        && !(item.nodeClass() == QOpcUa::NodeClass::Variable
                             && (item.browseName()
                                         == QOpcUaQualifiedName(0, QStringLiteral("EnumStrings"))
                                 || item.browseName()
                                         == QOpcUaQualifiedName(0, QStringLiteral("EnumValues")))))
                        continue;

                    auto childNode = mOpcUaClient->node(item.targetNodeId());
                    if (!childNode) {
                        qCWarning(opcuaModelLog)
                                << "Failed to instantiate node:" << item.targetNodeId().nodeId();
                        continue;
                    }

                    if (item.nodeClass() == QOpcUa::NodeClass::DataType) {
                        // See if there are subtypes or
                        browseEnumStrings(childNode);
                    } else {
                        // Read the value attribute of the EnumStrings/EnumValues property
                        const auto parentNodeId = node->nodeId();
                        connect(childNode, &QOpcUaNode::attributeRead, this,
                                [=](const QOpcUa::NodeAttributes &attributes) {
                                    Q_UNUSED(attributes)
                                    const auto data =
                                            childNode->valueAttribute().canConvert<QVariantList>()
                                            ? childNode->valueAttribute().toList()
                                            : QVariantList{ childNode->valueAttribute() };

                                    if (!data.empty()) {
                                        if (data.first().canConvert<QOpcUaLocalizedText>()) {
                                            QHash<qint32, QString> entries;
                                            for (int i = 0; i < data.size(); ++i)
                                                entries[i] = data.at(i)
                                                                     .value<QOpcUaLocalizedText>()
                                                                     .text();
                                            if (!entries.isEmpty())
                                                mEnumStringsList[parentNodeId] = entries;
                                        } else if (data.first()
                                                           .canConvert<QOpcUaExtensionObject>()) {
#ifdef HAS_GENERIC_STRUCT_HANDLER
                                            if (mGenericStructHandler) {
                                                QHash<qint32, QString> entries;

                                                const auto displayNameKey =
                                                        QStringLiteral("DisplayName");
                                                const auto valueKey = QStringLiteral("Value");

                                                for (const auto &entry : data) {
                                                    const auto decoded =
                                                            mGenericStructHandler->decode(
                                                                    entry.value<
                                                                            QOpcUaExtensionObject>());

                                                    if (decoded.has_value()) {
                                                        if (!decoded->fields().contains(
                                                                    displayNameKey)
                                                            || !decoded->fields().contains(
                                                                    valueKey))
                                                            continue;
                                                        const auto name =
                                                                decoded.value()
                                                                        .fields()
                                                                        .value(displayNameKey)
                                                                        .value<QOpcUaLocalizedText>()
                                                                        .text();
                                                        const auto value = decoded.value()
                                                                                   .fields()
                                                                                   .value(valueKey)
                                                                                   .value<qint32>();

                                                        if (!name.isEmpty())
                                                            entries[value] = name;
                                                    }
                                                }

                                                if (!entries.isEmpty())
                                                    mEnumStringsList[parentNodeId] = entries;
                                            }
#endif
                                        }
                                    }

                                    deleteNode(childNode);
                                });

                        ++cntNodes;
                        childNode->readValueAttribute();
                    }
                }

                deleteNode(node);
            });

    knownNodeIds << node->nodeId();
    cntNodes++;

    if (!node->browseChildren(QOpcUa::ReferenceTypeId::HierarchicalReferences,
                              QOpcUa::NodeClass::DataType | QOpcUa::NodeClass::Variable)) {
        qCWarning(opcuaModelLog) << "Browsing node" << node->nodeId() << "failed";
        deleteNode(node);
    }
}

bool OpcUaModel::allLookupsFinished() const
{
    return mBrowsedTypes.testFlags({ EBrowseType::DataTypes, EBrowseType::EnumStrings,
                                     EBrowseType::ReferenceTypes,
#ifdef HAS_GENERIC_STRUCT_HANDLER
                                     EBrowseType::GenericStructs
#endif
    });
}
