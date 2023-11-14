#include <QColor>
#include <QSortFilterProxyModel>

#include <QOpcUaArgument>
#include <QOpcUaAxisInformation>
#include <QOpcUaComplexNumber>
#include <QOpcUaDoubleComplexNumber>
#include <QOpcUaEUInformation>
#include <QOpcUaExtensionObject>
#include <QOpcUaQualifiedName>
#include <QOpcUaRange>
#include <QOpcUaXValue>

#include "treeitem.h"
#include "opcuamodel.h"
#include "attributemodel.h"

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

template <typename T>
QString numberArrayToString(const QList<T> &vec)
{
    QString list(QLatin1Char('['));
    for (int i = 0, size = vec.size(); i < size; ++i) {
        if (i)
            list.append(QLatin1Char(';'));
        list.append(QString::number(vec.at(i)));
    }
    list.append(QLatin1Char(']'));
    return list;
}

QString localizedTextToString(const QOpcUaLocalizedText &text)
{
    return QStringLiteral("[Locale: \"%1\", Text: \"%2\"]").arg(text.locale(), text.text());
}

QString rangeToString(const QOpcUaRange &range)
{
    return QStringLiteral("[Low: %1, High: %2]").arg(range.low()).arg(range.high());
}

QString euInformationToString(const QOpcUaEUInformation &info)
{
    return QStringLiteral("[UnitId: %1, NamespaceUri: \"%2\", DisplayName: %3, Description: %4]").arg(info.unitId()).arg(
        info.namespaceUri(), localizedTextToString(info.displayName()), localizedTextToString(info.description()));
}

QString variantToString(const QVariant &value, const QString &typeNodeId)
{
    if (value.metaType().id() == QMetaType::QVariantList) {
        const auto list = value.toList();
        QString concat;
        for (int i = 0, size = list.size(); i < size; ++i) {
            if (i)
                concat.append(QLatin1Char('\n'));
            concat.append(variantToString(list.at(i), typeNodeId));
        }
        return concat;
    }

    if (typeNodeId == QLatin1String("ns=0;i=19")) { // StatusCode
        const char *name = QMetaEnum::fromType<QOpcUa::UaStatusCode>().valueToKey(value.toInt());
        return name ? QLatin1String(name) : QLatin1String("Unknown StatusCode");
    }
    if (typeNodeId == QLatin1String("ns=0;i=2")) // Char
        return QString::number(value.toInt());
    if (typeNodeId == QLatin1String("ns=0;i=3")) // SChar
        return QString::number(value.toUInt());
    if (typeNodeId == QLatin1String("ns=0;i=4")) // Int16
        return QString::number(value.toInt());
    if (typeNodeId == QLatin1String("ns=0;i=5")) // UInt16
        return QString::number(value.toUInt());
    if (value.metaType().id() == QMetaType::QByteArray)
        return QLatin1String("0x") + value.toByteArray().toHex();
    if (value.metaType().id() == QMetaType::QDateTime)
        return value.toDateTime().toString(Qt::ISODate);
    if (value.canConvert<QOpcUaQualifiedName>()) {
        const auto name = value.value<QOpcUaQualifiedName>();
        return QStringLiteral("[NamespaceIndex: %1, Name: \"%2\"]").arg(name.namespaceIndex()).arg(name.name());
    }
    if (value.canConvert<QOpcUaLocalizedText>()) {
        const auto text = value.value<QOpcUaLocalizedText>();
        return localizedTextToString(text);
    }
    if (value.canConvert<QOpcUaRange>()) {
        const auto range = value.value<QOpcUaRange>();
        return rangeToString(range);
    }
    if (value.canConvert<QOpcUaComplexNumber>()) {
        const auto complex = value.value<QOpcUaComplexNumber>();
        return QStringLiteral("[Real: %1, Imaginary: %2]").arg(complex.real()).arg(complex.imaginary());
    }
    if (value.canConvert<QOpcUaDoubleComplexNumber>()) {
        const auto complex = value.value<QOpcUaDoubleComplexNumber>();
        return QStringLiteral("[Real: %1, Imaginary: %2]").arg(complex.real()).arg(complex.imaginary());
    }
    if (value.canConvert<QOpcUaXValue>()) {
        const auto xv = value.value<QOpcUaXValue>();
        return QStringLiteral("[X: %1, Value: %2]").arg(xv.x()).arg(xv.value());
    }
    if (value.canConvert<QOpcUaEUInformation>()) {
        const auto info = value.value<QOpcUaEUInformation>();
        return euInformationToString(info);
    }
    if (value.canConvert<QOpcUaAxisInformation>()) {
        const auto info = value.value<QOpcUaAxisInformation>();
        return QStringLiteral("[EUInformation: %1, EURange: %2, Title: %3 , AxisScaleType: %4, AxisSteps: %5]").arg(
            euInformationToString(info.engineeringUnits()), rangeToString(info.eURange()), localizedTextToString(info.title()),
            info.axisScaleType() == QOpcUa::AxisScale::Linear ? "Linear" : (info.axisScaleType() == QOpcUa::AxisScale::Ln) ? "Ln" : "Log", numberArrayToString(info.axisSteps()));
    }
    if (value.canConvert<QOpcUaExpandedNodeId>()) {
        const auto id = value.value<QOpcUaExpandedNodeId>();
        return QStringLiteral("[NodeId: \"%1\", ServerIndex: \"%2\", NamespaceUri: \"%3\"]").arg(
                                                                                                id.nodeId()).arg(id.serverIndex()).arg(id.namespaceUri());
    }
    if (value.canConvert<QOpcUaArgument>()) {
        const auto a = value.value<QOpcUaArgument>();

        return QStringLiteral("[Name: \"%1\", DataType: \"%2\", ValueRank: \"%3\", ArrayDimensions: %4, Description: %5]").arg(
                                                                                                                              a.name(), a.dataTypeId()).arg(a.valueRank()).arg(numberArrayToString(a.arrayDimensions()),
                 localizedTextToString(a.description()));
    }
    if (value.canConvert<QOpcUaExtensionObject>()) {
        const auto obj = value.value<QOpcUaExtensionObject>();
        return QStringLiteral("[TypeId: \"%1\", Encoding: %2, Body: 0x%3]").arg(obj.encodingTypeId(),
                                                                                obj.encoding() == QOpcUaExtensionObject::Encoding::NoBody ?
                                                                                    "NoBody" : (obj.encoding() == QOpcUaExtensionObject::Encoding::ByteString ?
                                                                                                                                                 "ByteString" : "XML")).arg(obj.encodedBody().isEmpty() ? "0" : QString(obj.encodedBody().toHex()));
    }

    if (value.canConvert<QString>())
        return value.toString();

    return QString();
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
    , mNodeClass(nodeClass)
{
    mSortedAttributeProxyModel->setSourceModel(mAttributeModel);
    mSortedAttributeProxyModel->sort(0);

    connect(mOpcNode.get(), &QOpcUaNode::attributeRead, this, &TreeItem::handleAttributes);
    connect(mOpcNode.get(), &QOpcUaNode::attributeUpdated, this, &TreeItem::handleAttributes);
    connect(mOpcNode.get(), &QOpcUaNode::browseFinished, this, &TreeItem::browseFinished);

    refreshAttributes();
}

TreeItem::TreeItem(QOpcUaNode *node, OpcUaModel *model, const QOpcUaReferenceDescription &browsingData, TreeItem *parent)
    : TreeItem(node, model, browsingData.nodeClass(), parent)
{
    mNodeBrowseName = browsingData.browseName().name();
    mNodeId = browsingData.targetNodeId().nodeId();

}

TreeItem::~TreeItem()
{
    qDeleteAll(mChildItems);
}

QAbstractItemModel *TreeItem::attributes() const noexcept
{
    return mSortedAttributeProxyModel;
}

QAbstractListModel *TreeItem::references() const noexcept
{
    return nullptr;
}

QAbstractListModel *TreeItem::monitoredAttributes() const noexcept
{
    return nullptr;
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

bool TreeItem::canMonitored() const noexcept
{
    return (QOpcUa::NodeClass::Variable == mNodeClass);
}

bool TreeItem::isMonitored() const noexcept
{
    return mIsMonitored;
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

void TreeItem::refresh()
{
    startBrowsing();
    refreshAttributes();
}

void TreeItem::refreshAttributes()
{
    mAttributeList.clear();
    mAttributeList.insert(QOpcUa::NodeAttribute::NodeId, Attribute(QOpcUa::NodeAttribute::NodeId, mNodeId));
    readNodeClassSpecificAttributes(mOpcNode.get(), mNodeClass);
}

void TreeItem::enableMonitoring()
{
    QOpcUaMonitoringParameters p(100);
    mOpcNode->enableMonitoring(QOpcUa::NodeAttribute::Value, p);

    connect(mOpcNode.get(), &QOpcUaNode::enableMonitoringFinished, this, [this] (QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode) {
        mIsMonitored = true;
    });
}

void TreeItem::disableMonitoring()
{
    mOpcNode->disableMonitoring(QOpcUa::NodeAttribute::Value);

    connect(mOpcNode.get(), &QOpcUaNode::disableMonitoringFinished, this, [this] (QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode) {
        mIsMonitored = false;
    });

}

void TreeItem::startBrowsing()
{
    if (mBrowseStarted)
        return;

    if (!mOpcNode->browseChildren())
        qWarning() << "Browsing node" << mOpcNode->nodeId() << "failed";
    else
        mBrowseStarted = true;
}

void TreeItem::handleAttributes(QOpcUa::NodeAttributes attr)
{
    if (attr & QOpcUa::NodeAttribute::NodeClass) {
        mNodeClass = mOpcNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>();
        const QMetaEnum metaEnum = QMetaEnum::fromType<QOpcUa::NodeClass>();
        mAttributeList.insert(QOpcUa::NodeAttribute::NodeClass, Attribute(QOpcUa::NodeAttribute::NodeClass, metaEnum.valueToKey(int(mNodeClass))));
    }
    if (attr & QOpcUa::NodeAttribute::BrowseName) {
        mNodeBrowseName = mOpcNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>().name();
        mAttributeList.insert(QOpcUa::NodeAttribute::BrowseName, Attribute(QOpcUa::NodeAttribute::BrowseName, mNodeBrowseName));
    }
    if (attr & QOpcUa::NodeAttribute::DisplayName) {
        const QString value = mOpcNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>().text();
        mAttributeList.insert(QOpcUa::NodeAttribute::DisplayName, Attribute(QOpcUa::NodeAttribute::DisplayName, value));
    }
    if (attr & QOpcUa::NodeAttribute::Description) {
        const QString value = mOpcNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>().text();
        mAttributeList.insert(QOpcUa::NodeAttribute::Description, Attribute(QOpcUa::NodeAttribute::Description, value));
    }
    if (attr & QOpcUa::NodeAttribute::WriteMask) {
        const QString value = mOpcNode->attribute(QOpcUa::NodeAttribute::WriteMask).toString();
        mAttributeList.insert(QOpcUa::NodeAttribute::WriteMask, Attribute(QOpcUa::NodeAttribute::WriteMask, value));
    }
    if (attr & QOpcUa::NodeAttribute::UserWriteMask) {
        const QString value = mOpcNode->attribute(QOpcUa::NodeAttribute::UserWriteMask).toString();
        mAttributeList.insert(QOpcUa::NodeAttribute::UserWriteMask, Attribute(QOpcUa::NodeAttribute::UserWriteMask, value));
    }
    if (attr & QOpcUa::NodeAttribute::IsAbstract) {
        const QString value = mOpcNode->attribute(QOpcUa::NodeAttribute::IsAbstract).toString();
        mAttributeList.insert(QOpcUa::NodeAttribute::IsAbstract, Attribute(QOpcUa::NodeAttribute::IsAbstract, value));
    }
    if (attr & QOpcUa::NodeAttribute::Symmetric) {
        const QString value = mOpcNode->attribute(QOpcUa::NodeAttribute::Symmetric).toString();
        mAttributeList.insert(QOpcUa::NodeAttribute::Symmetric, Attribute(QOpcUa::NodeAttribute::Symmetric, value));
    }
    if (attr & QOpcUa::NodeAttribute::InverseName) {
        const QString value = mOpcNode->attribute(QOpcUa::NodeAttribute::InverseName).value<QOpcUaLocalizedText>().text();
        mAttributeList.insert(QOpcUa::NodeAttribute::InverseName, Attribute(QOpcUa::NodeAttribute::InverseName, value));
    }
    if (attr & QOpcUa::NodeAttribute::ContainsNoLoops) {
        const QString value = mOpcNode->attribute(QOpcUa::NodeAttribute::ContainsNoLoops).toString();
        mAttributeList.insert(QOpcUa::NodeAttribute::ContainsNoLoops, Attribute(QOpcUa::NodeAttribute::ContainsNoLoops, value));
    }
    if (attr & QOpcUa::NodeAttribute::EventNotifier) {
        const quint32 byte = mOpcNode->attribute(QOpcUa::NodeAttribute::EventNotifier).toUInt();
        const QString value = QStringLiteral("0b%1").arg(byte, 8, 2, QChar('0'));
        mAttributeList.insert(QOpcUa::NodeAttribute::EventNotifier, Attribute(QOpcUa::NodeAttribute::EventNotifier, value));
    }
    if (attr & QOpcUa::NodeAttribute::Value) {
        const QString type = mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString();
        const QVariant value = mOpcNode->attribute(QOpcUa::NodeAttribute::Value);
        mAttributeList.insert(QOpcUa::NodeAttribute::Value, Attribute(QOpcUa::NodeAttribute::Value, variantToString(value, type)));
    }
    if (attr & QOpcUa::NodeAttribute::DataType) {
        const QString value = mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString();
        mAttributeList.insert(QOpcUa::NodeAttribute::DataType, Attribute(QOpcUa::NodeAttribute::DataType, value));
    }
    if (attr & QOpcUa::NodeAttribute::ValueRank) {
        const QString value = QString::number(mOpcNode->attribute(QOpcUa::NodeAttribute::ValueRank).toInt());
        mAttributeList.insert(QOpcUa::NodeAttribute::ValueRank, Attribute(QOpcUa::NodeAttribute::ValueRank, value));
    }
    if (attr & QOpcUa::NodeAttribute::ArrayDimensions) {
        const QString value = QString::number(mOpcNode->attribute(QOpcUa::NodeAttribute::ArrayDimensions).toUInt());
        mAttributeList.insert(QOpcUa::NodeAttribute::ArrayDimensions, Attribute(QOpcUa::NodeAttribute::ArrayDimensions, value));
    }
    if (attr & QOpcUa::NodeAttribute::AccessLevel) {
        const quint32 byte = mOpcNode->attribute(QOpcUa::NodeAttribute::AccessLevel).toUInt();
        const QString value = QStringLiteral("0b%1").arg(byte, 8, 2, QChar('0'));
        mAttributeList.insert(QOpcUa::NodeAttribute::AccessLevel, Attribute(QOpcUa::NodeAttribute::AccessLevel, value));
    }
    if (attr & QOpcUa::NodeAttribute::UserAccessLevel) {
        const quint32 byte = mOpcNode->attribute(QOpcUa::NodeAttribute::UserAccessLevel).toUInt();
        const QString value = QStringLiteral("0b%1").arg(byte, 8, 2, QChar('0'));
        mAttributeList.insert(QOpcUa::NodeAttribute::UserAccessLevel, Attribute(QOpcUa::NodeAttribute::UserAccessLevel, value));
    }
    if (attr & QOpcUa::NodeAttribute::MinimumSamplingInterval) {
        const QString value = QString::number(mOpcNode->attribute(QOpcUa::NodeAttribute::MinimumSamplingInterval).toDouble());
        mAttributeList.insert(QOpcUa::NodeAttribute::MinimumSamplingInterval, Attribute(QOpcUa::NodeAttribute::MinimumSamplingInterval, value));
    }
    if (attr & QOpcUa::NodeAttribute::Historizing) {
        const QString value = mOpcNode->attribute(QOpcUa::NodeAttribute::Historizing).toString();
        mAttributeList.insert(QOpcUa::NodeAttribute::Historizing, Attribute(QOpcUa::NodeAttribute::Historizing, value));
    }
    if (attr & QOpcUa::NodeAttribute::Executable) {
        const QString value = mOpcNode->attribute(QOpcUa::NodeAttribute::Executable).toString();
        mAttributeList.insert(QOpcUa::NodeAttribute::Executable, Attribute(QOpcUa::NodeAttribute::Executable, value));
    }
    if (attr & QOpcUa::NodeAttribute::UserExecutable) {
        const QString value = mOpcNode->attribute(QOpcUa::NodeAttribute::UserExecutable).toString();
        mAttributeList.insert(QOpcUa::NodeAttribute::UserExecutable, Attribute(QOpcUa::NodeAttribute::UserExecutable, value));
    }

    mAttributeModel->setAttributes(mAttributeList.values());

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
        if (hasChildNodeItem(item.targetNodeId().nodeId()))
            continue;

        auto node = mModel->opcUaClient()->node(item.targetNodeId());
        if (!node) {
            qWarning() << "Failed to instantiate node:" << item.targetNodeId().nodeId();
            continue;
        }

        mModel->beginInsertRows(index, mChildItems.size(), mChildItems.size() + 1);
        appendChild(new TreeItem(node, mModel, item, this));
        mModel->endInsertRows();
    }

    emit mModel->dataChanged(index, index);
}
