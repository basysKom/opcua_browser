#include <QDateTime>
#include <QMetaEnum>

#include <QOpcUaArgument>
#include <QOpcUaAxisInformation>
#include <QOpcUaComplexNumber>
#include <QOpcUaDoubleComplexNumber>
#include <QOpcUaEUInformation>
#include <QOpcUaExpandedNodeId>
#include <QOpcUaExtensionObject>
#include <QOpcUaNode>
#include <QOpcUaQualifiedName>
#include <QOpcUaRange>
#include <QOpcUaXValue>

#include "opcuahelper.h"

template <typename T>
QString numberArrayToString(const QList<T> &vec)
{
    QStringList list;
    list.reserve(vec.size());
    for (const auto &i : vec) {
        list << QString::number(i);
    }

    return QStringLiteral("[%1]").arg(list.join(';'));
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
    return QStringLiteral("[UnitId: %1, NamespaceUri: \"%2\", DisplayName: %3, Description: %4]")
            .arg(info.unitId())
            .arg(info.namespaceUri(), localizedTextToString(info.displayName()),
                 localizedTextToString(info.description()));
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
        return QStringLiteral("[NamespaceIndex: %1, Name: \"%2\"]")
                .arg(name.namespaceIndex())
                .arg(name.name());
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
        return QStringLiteral("[Real: %1, Imaginary: %2]")
                .arg(complex.real())
                .arg(complex.imaginary());
    }
    if (value.canConvert<QOpcUaDoubleComplexNumber>()) {
        const auto complex = value.value<QOpcUaDoubleComplexNumber>();
        return QStringLiteral("[Real: %1, Imaginary: %2]")
                .arg(complex.real())
                .arg(complex.imaginary());
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
        return QStringLiteral("[EUInformation: %1, EURange: %2, Title: %3 , AxisScaleType: %4, "
                              "AxisSteps: %5]")
                .arg(euInformationToString(info.engineeringUnits()), rangeToString(info.eURange()),
                     localizedTextToString(info.title()),
                     info.axisScaleType() == QOpcUa::AxisScale::Linear         ? "Linear"
                             : (info.axisScaleType() == QOpcUa::AxisScale::Ln) ? "Ln"
                                                                               : "Log",
                     numberArrayToString(info.axisSteps()));
    }
    if (value.canConvert<QOpcUaExpandedNodeId>()) {
        const auto id = value.value<QOpcUaExpandedNodeId>();
        return QStringLiteral("[NodeId: \"%1\", ServerIndex: \"%2\", NamespaceUri: \"%3\"]")
                .arg(id.nodeId())
                .arg(id.serverIndex())
                .arg(id.namespaceUri());
    }
    if (value.canConvert<QOpcUaArgument>()) {
        const auto a = value.value<QOpcUaArgument>();

        return QStringLiteral("[Name: \"%1\", DataType: \"%2\", ValueRank: \"%3\", "
                              "ArrayDimensions: %4, Description: %5]")
                .arg(a.name(), a.dataTypeId())
                .arg(a.valueRank())
                .arg(numberArrayToString(a.arrayDimensions()),
                     localizedTextToString(a.description()));
    }
    if (value.canConvert<QOpcUaExtensionObject>()) {
        const auto obj = value.value<QOpcUaExtensionObject>();
        return QStringLiteral("[TypeId: \"%1\", Encoding: %2, Body: 0x%3]")
                .arg(obj.encodingTypeId(),
                     obj.encoding() == QOpcUaExtensionObject::Encoding::NoBody
                             ? "NoBody"
                             : (obj.encoding() == QOpcUaExtensionObject::Encoding::ByteString
                                        ? "ByteString"
                                        : "XML"))
                .arg(obj.encodedBody().isEmpty() ? "0" : QString(obj.encodedBody().toHex()));
    }

    if (value.canConvert<QString>())
        return value.toString();

    return QString();
}

QString QOpcUaHelper::getAttributeValue(QOpcUaNode *node, QOpcUa::NodeAttribute attr)
{
    switch (attr) {
    case QOpcUa::NodeAttribute::NodeClass: {
        const auto nodeClass = node->attribute(attr).value<QOpcUa::NodeClass>();
        const QMetaEnum metaEnum = QMetaEnum::fromType<QOpcUa::NodeClass>();
        return metaEnum.valueToKey(int(nodeClass));
    }
    case QOpcUa::NodeAttribute::BrowseName:
        return node->attribute(attr).value<QOpcUaQualifiedName>().name();
    case QOpcUa::NodeAttribute::DisplayName:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::Description:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::InverseName:
        return node->attribute(attr).value<QOpcUaLocalizedText>().text();
    case QOpcUa::NodeAttribute::NodeId:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::WriteMask:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::UserWriteMask:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::IsAbstract:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::Symmetric:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::ContainsNoLoops:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::DataType:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::Historizing:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::Executable:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::UserExecutable:
        return node->attribute(attr).toString();
    case QOpcUa::NodeAttribute::ValueRank:
        return QString::number(node->attribute(attr).toInt());
    case QOpcUa::NodeAttribute::ArrayDimensions:
        return QString::number(node->attribute(attr).toUInt());
    case QOpcUa::NodeAttribute::MinimumSamplingInterval:
        return QString::number(node->attribute(attr).toDouble());
    case QOpcUa::NodeAttribute::EventNotifier:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::AccessLevel:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::UserAccessLevel: {
        const quint32 byte = node->attribute(attr).toUInt();
        return QStringLiteral("0b%1").arg(byte, 8, 2, QChar('0'));
    }
    case QOpcUa::NodeAttribute::Value: {
        const QString type = node->attribute(QOpcUa::NodeAttribute::DataType).toString();
        const QVariant attrValue = node->attribute(attr);
        return variantToString(attrValue, type);
    }
    case QOpcUa::NodeAttribute::None:
        return QString();
    }

    Q_UNREACHABLE();
    return QString();
}
