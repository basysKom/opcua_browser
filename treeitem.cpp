#include <QColor>
#include <QSortFilterProxyModel>

#include <QOpcUaClient>
#include <QOpcUaNode>
#include <QOpcUaQualifiedName>

#include "attributemodel.h"
#include "opcuamodel.h"
#include "opcuahelper.h"
#include "referencemodel.h"
#include "treeitem.h"

static constexpr QOpcUa::NodeAttributes objectAttributes = QOpcUa::NodeAttribute::EventNotifier;
static constexpr QOpcUa::NodeAttributes variableAttributes = QOpcUa::NodeAttribute::Value
        | QOpcUa::NodeAttribute::DataType | QOpcUa::NodeAttribute::ValueRank
        | QOpcUa::NodeAttribute::ArrayDimensions | QOpcUa::NodeAttribute::AccessLevel
        | QOpcUa::NodeAttribute::UserAccessLevel | QOpcUa::NodeAttribute::MinimumSamplingInterval
        | QOpcUa::NodeAttribute::Historizing;
static constexpr QOpcUa::NodeAttributes methodAttributes =
        QOpcUa::NodeAttribute::Executable | QOpcUa::NodeAttribute::UserExecutable;
static constexpr QOpcUa::NodeAttributes referenceTypeAttributes = QOpcUa::NodeAttribute::IsAbstract
        | QOpcUa::NodeAttribute::Symmetric | QOpcUa::NodeAttribute::InverseName;
static constexpr QOpcUa::NodeAttributes objectTypeAttributes = QOpcUa::NodeAttribute::IsAbstract;
static constexpr QOpcUa::NodeAttributes variableTypeAttributes = QOpcUa::NodeAttribute::Value
        | QOpcUa::NodeAttribute::DataType | QOpcUa::NodeAttribute::ValueRank
        | QOpcUa::NodeAttribute::ArrayDimensions | QOpcUa::NodeAttribute::IsAbstract;
static constexpr QOpcUa::NodeAttributes dataTypeAttributes = QOpcUa::NodeAttribute::IsAbstract;
static constexpr QOpcUa::NodeAttributes viewAttributes =
        QOpcUa::NodeAttribute::ContainsNoLoops | QOpcUa::NodeAttribute::EventNotifier;

bool readNodeClassSpecificAttributes(QOpcUaNode *node, QOpcUa::NodeClass nodeClass)
{
    QOpcUa::NodeAttributes attributes = node->allBaseAttributes();
    switch (nodeClass) {
    case QOpcUa::NodeClass::Object:
        attributes |= objectAttributes;
        break;
    case QOpcUa::NodeClass::Variable:
        attributes |= variableAttributes;
        ;
        break;
    case QOpcUa::NodeClass::Method:
        attributes |= methodAttributes;
        break;
    case QOpcUa::NodeClass::ReferenceType:
        attributes |= referenceTypeAttributes;
        break;
    case QOpcUa::NodeClass::ObjectType:
        attributes |= objectTypeAttributes;
        break;
    case QOpcUa::NodeClass::VariableType:
        attributes |= variableTypeAttributes;
        break;
    case QOpcUa::NodeClass::DataType:
        attributes |= dataTypeAttributes;
        break;
    case QOpcUa::NodeClass::View:
        attributes |= viewAttributes;
        break;
    case QOpcUa::NodeClass::Undefined:
    default:
        Q_UNREACHABLE();
        break;
    }

    return node->readAttributes(attributes);
}

TreeItem::TreeItem(const QString &nodeId, OpcUaModel *model, QOpcUa::NodeClass nodeClass,
                   TreeItem *parent)
    : QObject(parent),
      mModel(model),
      mParentItem(parent),
      mAttributeModel(new AttributeModel(this)),
      mSortedAttributeProxyModel(new QSortFilterProxyModel(this)),
      mReferenceModel(new ReferenceModel(this)),
      mSortedReferenceProxyModel(new QSortFilterProxyModel(this)),
      mNodeId(nodeId),
      mNodeClass(nodeClass)
{
    mSortedAttributeProxyModel->setSourceModel(mAttributeModel);
    mSortedAttributeProxyModel->sort(0);

    mSortedReferenceProxyModel->setSourceModel(mReferenceModel);
    mSortedReferenceProxyModel->sort(0);

    mAttributeModel->setAttribute(QOpcUa::NodeAttribute::NodeId, mNodeId);
    refreshAttributes();
}

TreeItem::TreeItem(const QString &nodeId, OpcUaModel *model,
                   const QOpcUaReferenceDescription &browsingData, TreeItem *parent)
    : TreeItem(nodeId, model, browsingData.nodeClass(), parent)
{
    mDisplayName = browsingData.displayName().text();
    if (mDisplayName.isEmpty()) {
        mDisplayName = browsingData.browseName().name();
    }
}

TreeItem::~TreeItem()
{
    qDeleteAll(mChildItems);
}

QAbstractItemModel *TreeItem::attributes() const noexcept
{
    return mSortedAttributeProxyModel;
}

QAbstractItemModel *TreeItem::references() const noexcept
{
    return mSortedReferenceProxyModel;
}

TreeItem *TreeItem::child(int row)
{
    if (row >= mChildItems.size())
        qCritical() << "TreeItem in row" << row << "does not exist.";
    return mChildItems[row];
}

int TreeItem::childIndex(const TreeItem *child) const
{
    return mChildItems.indexOf(const_cast<TreeItem *>(child));
}

int TreeItem::childCount()
{
    startBrowsing();
    return mChildItems.size();
}

const QString &TreeItem::displayName() const noexcept
{
    return mDisplayName;
}

const QColor &TreeItem::nodeClassColor() const noexcept
{
    static const QColor objectColor = QColor(Qt::darkGreen);
    static const QColor variableColor = QColor(Qt::darkBlue);
    static const QColor methodColor = QColor(Qt::darkRed);
    static const QColor defaultColor = QColor(Qt::gray);

    switch (mNodeClass) {
    case QOpcUa::NodeClass::Object:
        return objectColor;
    case QOpcUa::NodeClass::Variable:
        return variableColor;
    case QOpcUa::NodeClass::Method:
        return methodColor;
    default:
        break;
    }

    return defaultColor;
}

const QString &TreeItem::value() const noexcept
{
    return mValue;
}

bool TreeItem::canMonitored() const noexcept
{
    return (QOpcUa::NodeClass::Variable == mNodeClass);
}

int TreeItem::row() const
{
    if (!mParentItem)
        return 0;
    return mParentItem->childIndex(this);
}

TreeItem *TreeItem::parentItem()
{
    return mParentItem;
}

void TreeItem::appendChild(TreeItem *child)
{
    if (!child)
        return;

    if (!hasChildNodeItem(child->mNodeId)) {
        mChildItems.append(child);
        mChildNodeIds.insert(child->mNodeId);
    } else {
        child->deleteLater();
    }
}

bool TreeItem::hasChildNodeItem(const QString &nodeId) const
{
    return mChildNodeIds.contains(nodeId);
}

const QString &TreeItem::nodeId() const noexcept
{
    return mNodeId;
}

void TreeItem::refresh()
{
    mChildNodeIds.clear();
    const auto index = mModel->createIndex(row(), 0, this);
    mModel->beginRemoveRows(index, 0, mChildItems.size() - 1);
    qDeleteAll(mChildItems);
    mChildItems.clear();
    mModel->endRemoveRows();

    if (nullptr != mReferenceModel) {
        mReferenceModel->clearForwardReferences();
    }

    mBrowseStarted = false;
    startBrowsing();
    refreshAttributes();
}

void TreeItem::refreshAttributes()
{
    auto node = mModel->opcUaClient()->node(mNodeId);
    if (nullptr == node)
        return;

    connect(node, &QOpcUaNode::attributeRead, this, [=](const QOpcUa::NodeAttributes &attributes) {
        QString displayName;
        QString browseName;
        for (int i = 0; i <= 21; ++i) {
            const QOpcUa::NodeAttribute attr = static_cast<QOpcUa::NodeAttribute>(1 << i);
            if (!attributes.testFlag(attr))
                continue;

            const QString stringValue = QOpcUaHelper::getFormattedAttributeValue(node, attr);
            if (QOpcUa::NodeAttribute::DataType == attr) {
                mAttributeModel->setAttribute(attr, mModel->getStringForDataTypeId(stringValue));
            } else {
                mAttributeModel->setAttribute(attr, stringValue);
            }

            if (QOpcUa::NodeAttribute::NodeClass == attr) {
                mNodeClass = node->attribute(attr).value<QOpcUa::NodeClass>();
            } else if (QOpcUa::NodeAttribute::DisplayName == attr) {
                displayName = QOpcUaHelper::getRawAttributeValue(node, attr);
            } else if (QOpcUa::NodeAttribute::BrowseName == attr) {
                browseName = QOpcUaHelper::getRawAttributeValue(node, attr);
            } else if (QOpcUa::NodeAttribute::Value == attr) {
                mValue = stringValue;
            }
        }

        mDisplayName = displayName.isEmpty() ? browseName : displayName;

        emit mModel->dataChanged(mModel->createIndex(row(), 0, this),
                                 mModel->createIndex(row(), 0, this));

        node->deleteLater();
    });

    if (!readNodeClassSpecificAttributes(node, mNodeClass)) {
        qWarning() << "Reading attributes" << node->nodeId() << "failed";
        node->deleteLater();
    }
}

void TreeItem::addItemToReferenceModel(const QOpcUaReferenceDescription &item)
{
    const QString targetNodeId = item.targetNodeId().nodeId();
    const QString typeNodeId = item.refTypeId();
    const QString type = mModel->getStringForRefTypeId(typeNodeId, true);
    mReferenceModel->addReference(type, typeNodeId, item.isForwardReference(),
                                  item.displayName().text(), targetNodeId);
}

bool TreeItem::browseChildren()
{
    auto node = mModel->opcUaClient()->node(mNodeId);
    if (nullptr == node)
        return false;

    connect(node, &QOpcUaNode::browseFinished, this,
            [=](const QList<QOpcUaReferenceDescription> &children,
                QOpcUa::UaStatusCode statusCode) {
                if (statusCode != QOpcUa::Good) {
                    qWarning() << "Browsing node" << node->nodeId()
                               << "finally failed:" << statusCode;
                    node->deleteLater();
                    return;
                }

                const auto index = mModel->createIndex(row(), 0, this);
                for (const auto &item : children) {
                    addItemToReferenceModel(item);
                    if (!item.isForwardReference())
                        continue;

                    const QString nodeId = item.targetNodeId().nodeId();
                    if (hasChildNodeItem(nodeId))
                        continue;

                    mModel->beginInsertRows(index, mChildItems.size(), mChildItems.size());
                    appendChild(new TreeItem(nodeId, mModel, item, this));
                    mModel->endInsertRows();
                }

                emit mModel->dataChanged(index, index);

                node->deleteLater();
            });

    QOpcUaBrowseRequest request;
    request.setBrowseDirection(QOpcUaBrowseRequest::BrowseDirection::Both);
    request.setReferenceTypeId(QOpcUa::ReferenceTypeId::HierarchicalReferences);
    request.setIncludeSubtypes(true);
    if (!node->browse(request)) {
        qWarning() << "Browsing node" << node->nodeId() << "failed";
        node->deleteLater();
        return false;
    }

    return true;
}

bool TreeItem::browseNonHierarchicalReferences()
{
    auto node = mModel->opcUaClient()->node(mNodeId);
    if (nullptr == node)
        return false;

    connect(node, &QOpcUaNode::browseFinished, this,
            [=](const QList<QOpcUaReferenceDescription> &children,
                QOpcUa::UaStatusCode statusCode) {
                if (statusCode != QOpcUa::Good) {
                    qWarning() << "Browsing node" << node->nodeId()
                               << "finally failed:" << statusCode;
                    node->deleteLater();
                    return;
                }

                for (const auto &item : children) {
                    addItemToReferenceModel(item);
                }

                const auto index = mModel->createIndex(row(), 0, this);
                emit mModel->dataChanged(index, index);

                node->deleteLater();
            });

    QOpcUaBrowseRequest request;
    request.setBrowseDirection(QOpcUaBrowseRequest::BrowseDirection::Both);
    request.setReferenceTypeId(QOpcUa::ReferenceTypeId::NonHierarchicalReferences);
    request.setIncludeSubtypes(true);
    if (!node->browse(request)) {
        qWarning() << "Browsing non hierarchical references of node" << node->nodeId() << "failed";
        node->deleteLater();
        return false;
    }

    return true;
}

void TreeItem::startBrowsing()
{
    if (mBrowseStarted)
        return;

    mBrowseStarted = browseChildren();
    browseNonHierarchicalReferences();
}
