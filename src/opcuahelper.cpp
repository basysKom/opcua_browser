/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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

#include "backend.h"
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

QStringList eventNotifierToStringList(quint8 byte)
{
    QStringList eventNotifier;
    if ((byte & 0x01) != 0)
        eventNotifier << QStringLiteral("SubscribeToEvents");
    if ((byte & 0x02) != 0)
        eventNotifier << QStringLiteral("Unknown");
    if ((byte & 0x04) != 0)
        eventNotifier << QStringLiteral("HistoryRead");
    if ((byte & 0x08) != 0)
        eventNotifier << QStringLiteral("HistoryWrite");
    if ((byte & 0x10) != 0)
        eventNotifier << QStringLiteral("Unknown");
    if ((byte & 0x20) != 0)
        eventNotifier << QStringLiteral("Unknown");
    if ((byte & 0x40) != 0)
        eventNotifier << QStringLiteral("Unknown");
    if ((byte & 0x80) != 0)
        eventNotifier << QStringLiteral("Unknown");
    return eventNotifier;
}

QStringList accessLevelToStringList(quint8 byte)
{
    QStringList accessLevels;
    if ((byte & 0x01) != 0)
        accessLevels << QStringLiteral("CurrentRead");
    if ((byte & 0x02) != 0)
        accessLevels << QStringLiteral("CurrentWrite");
    if ((byte & 0x04) != 0)
        accessLevels << QStringLiteral("HistoryRead");
    if ((byte & 0x08) != 0)
        accessLevels << QStringLiteral("HistoryWrite");
    if ((byte & 0x10) != 0)
        accessLevels << QStringLiteral("SemanticChange");
    if ((byte & 0x20) != 0)
        accessLevels << QStringLiteral("StatusWrite");
    if ((byte & 0x40) != 0)
        accessLevels << QStringLiteral("TimestampWrite");
    if ((byte & 0x80) != 0)
        accessLevels << QStringLiteral("Unknown");
    return accessLevels;
}

QStringList writeMaskToStringList(quint32 value)
{
    QStringList attributes;
    if ((value & 0x00000001) != 0)
        attributes << QStringLiteral("AccessLevel");
    if ((value & 0x00000002) != 0)
        attributes << QStringLiteral("ArrayDimensions");
    if ((value & 0x00000004) != 0)
        attributes << QStringLiteral("BrowseName");
    if ((value & 0x00000008) != 0)
        attributes << QStringLiteral("ContainsNoLoops");
    if ((value & 0x00000010) != 0)
        attributes << QStringLiteral("DataType");
    if ((value & 0x00000020) != 0)
        attributes << QStringLiteral("Description");
    if ((value & 0x00000040) != 0)
        attributes << QStringLiteral("DisplayName");
    if ((value & 0x00000080) != 0)
        attributes << QStringLiteral("EventNotifier");
    if ((value & 0x00000100) != 0)
        attributes << QStringLiteral("Executable");
    if ((value & 0x00000200) != 0)
        attributes << QStringLiteral("Historizing");
    if ((value & 0x00000400) != 0)
        attributes << QStringLiteral("InverseName");
    if ((value & 0x00000800) != 0)
        attributes << QStringLiteral("IsAbstract");
    if ((value & 0x00001000) != 0)
        attributes << QStringLiteral("MinimumSamplingInterval");
    if ((value & 0x00002000) != 0)
        attributes << QStringLiteral("NodeClass");
    if ((value & 0x00004000) != 0)
        attributes << QStringLiteral("NodeId");
    if ((value & 0x00008000) != 0)
        attributes << QStringLiteral("Symmetric");
    if ((value & 0x00010000) != 0)
        attributes << QStringLiteral("UserAccessLevel");
    if ((value & 0x00020000) != 0)
        attributes << QStringLiteral("UserExecutable");
    if ((value & 0x00040000) != 0)
        attributes << QStringLiteral("UserWriteMask");
    if ((value & 0x00080000) != 0)
        attributes << QStringLiteral("ValueRank");
    if ((value & 0x00100000) != 0)
        attributes << QStringLiteral("WriteMask");
    if ((value & 0x00200000) != 0)
        attributes << QStringLiteral("ValueForVariableType");
    if ((value & 0x00400000) != 0)
        attributes << QStringLiteral("DataTypeDefinition");
    if ((value & 0x00800000) != 0)
        attributes << QStringLiteral("RolePermissions");
    if ((value & 0x01000000) != 0)
        attributes << QStringLiteral("AccessLevel");
    if ((value & 0x02000000) != 0)
        attributes << QStringLiteral("AccessRestrictions");
    if ((value & 0x04000000) != 0)
        attributes << QStringLiteral("AccessLevelEx");
    if ((value & 0x08000000) != 0)
        attributes << QStringLiteral("Unknown");
    if ((value & 0x10000000) != 0)
        attributes << QStringLiteral("Unknown");
    if ((value & 0x20000000) != 0)
        attributes << QStringLiteral("Unknown");
    if ((value & 0x40000000) != 0)
        attributes << QStringLiteral("Unknown");
    if ((value & 0x80000000) != 0)
        attributes << QStringLiteral("Unknown");
    return attributes;
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

QString QOpcUaHelper::getRawAttributeValue(QOpcUaNode *node, QOpcUa::NodeAttribute attr)
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
    case QOpcUa::NodeAttribute::Description:
    case QOpcUa::NodeAttribute::InverseName:
        return node->attribute(attr).value<QOpcUaLocalizedText>().text();
    case QOpcUa::NodeAttribute::NodeId:
    case QOpcUa::NodeAttribute::IsAbstract:
    case QOpcUa::NodeAttribute::Symmetric:
    case QOpcUa::NodeAttribute::ContainsNoLoops:
    case QOpcUa::NodeAttribute::DataType:
    case QOpcUa::NodeAttribute::Historizing:
    case QOpcUa::NodeAttribute::Executable:
    case QOpcUa::NodeAttribute::UserExecutable:
        return node->attribute(attr).toString();
    case QOpcUa::NodeAttribute::ValueRank:
        return QString::number(node->attribute(attr).toInt());
    case QOpcUa::NodeAttribute::ArrayDimensions:
        return QString::number(node->attribute(attr).toUInt());
    case QOpcUa::NodeAttribute::MinimumSamplingInterval:
        return QString::number(node->attribute(attr).toDouble());
    case QOpcUa::NodeAttribute::EventNotifier: {
        const quint32 byte = node->attribute(attr).toUInt();
        const QStringList eventNotifierTypes = QStringList()
                << QStringLiteral("0b%1").arg(byte, 8, 2, QChar('0'))
                << eventNotifierToStringList((quint8)byte);
        return eventNotifierTypes.join("\n");
    }
    case QOpcUa::NodeAttribute::AccessLevel:
    case QOpcUa::NodeAttribute::UserAccessLevel: {
        const quint32 byte = node->attribute(attr).toUInt();
        const QStringList accessLevelTypes = QStringList()
                << QStringLiteral("0b%1").arg(byte, 8, 2, QChar('0'))
                << accessLevelToStringList((quint8)byte);
        return accessLevelTypes.join("\n");
    }
    case QOpcUa::NodeAttribute::WriteMask:
    case QOpcUa::NodeAttribute::UserWriteMask: {
        const quint32 value = node->attribute(attr).toUInt();
        const QStringList accessLevelTypes = QStringList()
                << QStringLiteral("0x%1").arg(value, 8, 16, QChar('0'))
                << writeMaskToStringList(value);
        return accessLevelTypes.join("\n");
    }
    case QOpcUa::NodeAttribute::Value: {
        const QString type = node->attribute(QOpcUa::NodeAttribute::DataType).toString();
        const QVariant attrValue = node->attribute(attr);
        const auto valueString = variantToString(attrValue, type);

        if (!valueString.isEmpty()) {
            const auto model = BackEnd::getOpcUaModelForNode(node);
            if (model) {
                const auto enumCandidate = model->getEnumStringsForDataTypeId(type);
                if (!enumCandidate.isEmpty()) {
                    const auto value = node->valueAttribute().value<qint32>();
                    const auto entry = enumCandidate.constFind(value);
                    if (entry != enumCandidate.constEnd())
                        return QStringLiteral("%1 (%2)").arg(valueString, entry.value());
                }
            }
        }

        return valueString;
    }
    case QOpcUa::NodeAttribute::None:
        return QString();
    }

    Q_UNREACHABLE();
    return QString();
}

QString QOpcUaHelper::getFormattedAttributeValue(QOpcUaNode *node, QOpcUa::NodeAttribute attr)
{
    switch (attr) {
    case QOpcUa::NodeAttribute::BrowseName:
        return QStringLiteral("%1:%2")
                .arg(node->attribute(attr).value<QOpcUaQualifiedName>().namespaceIndex())
                .arg(node->attribute(attr).value<QOpcUaQualifiedName>().name());
    case QOpcUa::NodeAttribute::DisplayName:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::Description:
        Q_FALLTHROUGH();
    case QOpcUa::NodeAttribute::InverseName:
        return QStringLiteral("\"%1\", \"%2\"")
                .arg(node->attribute(attr).value<QOpcUaLocalizedText>().locale(),
                     node->attribute(attr).value<QOpcUaLocalizedText>().text());
    case QOpcUa::NodeAttribute::NodeClass:
    case QOpcUa::NodeAttribute::NodeId:
    case QOpcUa::NodeAttribute::WriteMask:
    case QOpcUa::NodeAttribute::UserWriteMask:
    case QOpcUa::NodeAttribute::IsAbstract:
    case QOpcUa::NodeAttribute::Symmetric:
    case QOpcUa::NodeAttribute::ContainsNoLoops:
    case QOpcUa::NodeAttribute::DataType:
    case QOpcUa::NodeAttribute::Historizing:
    case QOpcUa::NodeAttribute::Executable:
    case QOpcUa::NodeAttribute::UserExecutable:
    case QOpcUa::NodeAttribute::ValueRank:
    case QOpcUa::NodeAttribute::ArrayDimensions:
    case QOpcUa::NodeAttribute::MinimumSamplingInterval:
    case QOpcUa::NodeAttribute::EventNotifier:
    case QOpcUa::NodeAttribute::AccessLevel:
    case QOpcUa::NodeAttribute::UserAccessLevel:
    case QOpcUa::NodeAttribute::Value:
    case QOpcUa::NodeAttribute::None:
        return getRawAttributeValue(node, attr);
    }

    Q_UNREACHABLE();
    return QString();
}
