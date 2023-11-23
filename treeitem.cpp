#include <QColor>
#include <QSortFilterProxyModel>

#include "attributemodel.h"
#include "opcuamodel.h"
#include "opcuahelper.h"
#include "referencemodel.h"
#include "treeitem.h"

static constexpr QOpcUa::NodeAttributes objectAttributes = QOpcUa::NodeAttribute::EventNotifier;
static constexpr QOpcUa::NodeAttributes variableAttributes = QOpcUa::NodeAttribute::Value | QOpcUa::NodeAttribute::DataType | QOpcUa::NodeAttribute::ValueRank
    | QOpcUa::NodeAttribute::ArrayDimensions | QOpcUa::NodeAttribute::AccessLevel | QOpcUa::NodeAttribute::UserAccessLevel
    | QOpcUa::NodeAttribute::MinimumSamplingInterval | QOpcUa::NodeAttribute::Historizing;
static constexpr QOpcUa::NodeAttributes methodAttributes = QOpcUa::NodeAttribute::Executable | QOpcUa::NodeAttribute::UserExecutable;
static constexpr QOpcUa::NodeAttributes referenceTypeAttributes = QOpcUa::NodeAttribute::IsAbstract | QOpcUa::NodeAttribute::Symmetric | QOpcUa::NodeAttribute::InverseName;
static constexpr QOpcUa::NodeAttributes objectTypeAttributes = QOpcUa::NodeAttribute::IsAbstract;
static constexpr QOpcUa::NodeAttributes variableTypeAttributes = QOpcUa::NodeAttribute::Value | QOpcUa::NodeAttribute::DataType | QOpcUa::NodeAttribute::ValueRank
    | QOpcUa::NodeAttribute::ArrayDimensions | QOpcUa::NodeAttribute::IsAbstract;
static constexpr QOpcUa::NodeAttributes dataTypeAttributes = QOpcUa::NodeAttribute::IsAbstract;
static constexpr QOpcUa::NodeAttributes viewAttributes = QOpcUa::NodeAttribute::ContainsNoLoops | QOpcUa::NodeAttribute::EventNotifier;

void readNodeClassSpecificAttributes(QOpcUaNode *node, QOpcUa::NodeClass nodeClass)
{
    QOpcUa::NodeAttributes attributes = node->allBaseAttributes();
    switch (nodeClass) {
    case QOpcUa::NodeClass::Object:
        attributes |= objectAttributes;
        break;
    case QOpcUa::NodeClass::Variable:
        attributes |= variableAttributes;;
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

    if (!node->readAttributes(attributes))
        qWarning() << "Reading attributes" << node->nodeId() << "failed";
}

TreeItem::TreeItem(OpcUaModel *model)
    : QObject{nullptr}
    , mModel(model)
{

}

TreeItem::TreeItem(QOpcUaNode *node, OpcUaModel *model, QOpcUa::NodeClass nodeClass, TreeItem *parent)
    : QObject(parent)
    , mOpcNode(node)
    , mModel(model)
    , mParentItem(parent)
    , mAttributeModel(new AttributeModel(this))
    , mSortedAttributeProxyModel(new QSortFilterProxyModel(this))
    , mReferenceModel(new ReferenceModel(this))
    , mSortedReferenceProxyModel(new QSortFilterProxyModel(this))
    , mNodeClass(nodeClass)
{
    mSortedAttributeProxyModel->setSourceModel(mAttributeModel);
    mSortedAttributeProxyModel->sort(0);

    mSortedReferenceProxyModel->setSourceModel(mReferenceModel);
    mSortedReferenceProxyModel->sort(0);

    connect(mOpcNode.get(), &QOpcUaNode::attributeRead, this, &TreeItem::handleAttributes);
    connect(mOpcNode.get(), &QOpcUaNode::browseFinished, this, &TreeItem::browseFinished);

    mAttributeModel->setAttribute(QOpcUa::NodeAttribute::NodeId, mNodeId);
    refreshAttributes();
}

TreeItem::TreeItem(QOpcUaNode *node, OpcUaModel *model, const QOpcUaReferenceDescription &browsingData, TreeItem *parent)
    : TreeItem(node, model, browsingData.nodeClass(), parent)
{
    mNodeBrowseName = browsingData.browseName().name();
    mNodeId = browsingData.targetNodeId().nodeId();

    const QString type = model->getStringForRefTypeId(browsingData.refTypeId(), false);
    mReferenceModel->addReference(type, false, parent->displayName());
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
    return mNodeBrowseName;
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
    readNodeClassSpecificAttributes(mOpcNode.get(), mNodeClass);
}

void TreeItem::startBrowsing(bool forceRebrowse, QOpcUa::ReferenceTypeId referenceType)
{
    if (mBrowseStarted && !forceRebrowse)
        return;

    if (!mOpcNode->browseChildren(referenceType)) {
        qWarning() << "Browsing node" << mOpcNode->nodeId() << "failed";
    } else {
        mBrowseStarted = true;
        mBrowseNonHierarchicalReferences = (QOpcUa::ReferenceTypeId::NonHierarchicalReferences == referenceType);
    }
}

void TreeItem::handleAttributes(const QOpcUa::NodeAttributes &attributes)
{
    for (int i = 0; i <= 21; ++i) {
        const QOpcUa::NodeAttribute attr = static_cast<QOpcUa::NodeAttribute>(1 << i);
        if (!attributes.testFlag(attr))
            continue;

        const QString stringValue = QOpcUaHelper::getAttributeValue(mOpcNode.get(), attr);
        mAttributeModel->setAttribute(attr, stringValue);

        if (QOpcUa::NodeAttribute::NodeClass == attr) {
            mNodeClass = mOpcNode->attribute(attr).value<QOpcUa::NodeClass>();
        } else if (QOpcUa::NodeAttribute::BrowseName == attr) {
            mNodeBrowseName = stringValue;
        } else if (QOpcUa::NodeAttribute::Value == attr) {
            mValue = stringValue;
        }
    }

    mAttributesReady = true;
    emit mModel->dataChanged(mModel->createIndex(row(), 0, this), mModel->createIndex(row(), 0, this));
}

void TreeItem::browseFinished(const QList<QOpcUaReferenceDescription> &children, QOpcUa::UaStatusCode statusCode)
{
    if (statusCode != QOpcUa::Good) {
        qWarning() << "Browsing node" << mOpcNode->nodeId() << "finally failed:" << statusCode;
        return;
    }

    auto index = mModel->createIndex(row(), 0, this);

    for (const auto &item : children) {
        const QString type = mModel->getStringForRefTypeId(item.refTypeId(), true);
        mReferenceModel->addReference(type, true, item.displayName().text());

        if (!mBrowseNonHierarchicalReferences) {
            if (hasChildNodeItem(item.targetNodeId().nodeId()))
                continue;

            auto node = mModel->opcUaClient()->node(item.targetNodeId());
            if (!node) {
                qWarning() << "Failed to instantiate node:" << item.targetNodeId().nodeId();
                continue;
            }

            mModel->beginInsertRows(index, mChildItems.size(), mChildItems.size());
            appendChild(new TreeItem(node, mModel, item, this));
            mModel->endInsertRows();
        }
    }

    emit mModel->dataChanged(index, index);

    if (!mBrowseNonHierarchicalReferences) {
        startBrowsing(true, QOpcUa::ReferenceTypeId::NonHierarchicalReferences);
    }
}
