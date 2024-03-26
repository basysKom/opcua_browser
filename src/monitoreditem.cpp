/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QOpcUaClient>
#include <QOpcUaNode>
#include <QOpcUaQualifiedName>

#include "backend.h"
#include "constants.h"
#include "monitoreditem.h"
#include "opcuahelper.h"

MonitoredItem::MonitoredItem(QOpcUaNode *node, QObject *parent)
    : QObject(parent), mOpcNode(node), mType(Type::Variable)
{
    // Add item in dashboard has a null pointer
    if (node != nullptr) {
        mNodeId = node->nodeId();

        connect(mOpcNode.get(), &QOpcUaNode::attributeRead, this, &MonitoredItem::handleAttributes);
        connect(mOpcNode.get(), &QOpcUaNode::attributeUpdated, this,
                &MonitoredItem::handleAttributes);

        node->readAttributes(QOpcUa::NodeAttribute::BrowseName | QOpcUa::NodeAttribute::DisplayName
                             | QOpcUa::NodeAttribute::DataType);

        QOpcUaMonitoringParameters p(100);
        node->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
    }
}

MonitoredItem::MonitoredItem(QOpcUaNode *node,
                             const QOpcUaMonitoringParameters::EventFilter &eventFilter,
                             QObject *parent)
    : QObject(parent), mOpcNode(node), mType(Type::Event), mEventFilter(eventFilter)
{
    // Extract event field names from the event filter's select clauses
    for (const auto &field : eventFilter.selectClauses()) {
        if (!field.browsePath().isEmpty())
            mEventFieldNames.push_back(field.browsePath().constLast().name());
        else // Make sure there is at least an empty string so the event fields are not off if
             // somebody passes a select clause with empty browse path
            mEventFieldNames.push_back({});
    }

    eventFieldTypeLookup();

    if (node != nullptr) {
        mNodeId = node->nodeId();

        connect(mOpcNode.get(), &QOpcUaNode::eventOccurred, this, &MonitoredItem::handleEvent);
        connect(mOpcNode.get(), &QOpcUaNode::attributeUpdated, this,
                &MonitoredItem::handleAttributes);

        node->readAttributes(QOpcUa::NodeAttribute::BrowseName
                             | QOpcUa::NodeAttribute::DisplayName);

        QOpcUaMonitoringParameters p(0);
        p.setQueueSize(50);
        p.setFilter(eventFilter);
        node->enableMonitoring(QOpcUa::NodeAttribute::EventNotifier, p);
    }
}

const QString &MonitoredItem::nodeId() const noexcept
{
    return mNodeId;
}

const QString &MonitoredItem::displayName() const noexcept
{
    return mDisplayName.isEmpty() ? nodeId() : mDisplayName;
}

const QString &MonitoredItem::value() const noexcept
{
    return mValue;
}

QString MonitoredItem::status() const
{
    return QOpcUa::statusToString(mStatusCode);
}

bool MonitoredItem::hasError() const noexcept
{
    return (mStatusCode != QOpcUa::Good);
}

void MonitoredItem::handleAttributes(const QOpcUa::NodeAttributes &attributes)
{
    QString newDisplayName;
    if (attributes.testFlag(QOpcUa::NodeAttribute::DisplayName)) {
        newDisplayName = QOpcUaHelper::getRawAttributeValue(mOpcNode.get(),
                                                            QOpcUa::NodeAttribute::DisplayName);
    }

    if (newDisplayName.isEmpty() && attributes.testFlag(QOpcUa::NodeAttribute::BrowseName)) {
        newDisplayName = QOpcUaHelper::getRawAttributeValue(mOpcNode.get(),
                                                            QOpcUa::NodeAttribute::BrowseName);
    }

    if (!newDisplayName.isEmpty() && (newDisplayName != mDisplayName)) {
        mDisplayName = newDisplayName;
        emit displayNameChanged();
    }

    if (attributes.testFlag(QOpcUa::NodeAttribute::Value)) {
        setStatusCode(mOpcNode->valueAttributeError());

        if (!hasError()) {
            const QString newValue = QOpcUaHelper::getFormattedAttributeValue(
                    mOpcNode.get(), QOpcUa::NodeAttribute::Value);
            if (newValue != mValue) {
                mValue = newValue;
                emit valueChanged();
            }

            // Workaround: enableMonitoring always returns QOpcUa::Good => Read in value again
            // Must be fixed in file qopen62541subscription.cpp line 339
            if (!mHasReadValue) {
                mHasReadValue = true;
                mOpcNode->readAttributes(QOpcUa::NodeAttribute::Value);
            }
        }
    }
}

void MonitoredItem::handleEvent(const QVariantList &eventFields)
{
    if (eventFields.length() != mEventFieldNames.length())
        return;

    QVariantMap eventStrings;

    for (int i = 0; i < eventFields.length(); ++i) {
        if (!eventFields.at(i).isNull()) {

            const auto stringValue = QOpcUaHelper::variantToString(
                    mOpcNode.get(), eventFields.at(i), mEventFieldTypeNodeIds.at(i));
            eventStrings[mEventFieldNames.at(i)] = stringValue;
        }
    }

    mLastEvents.push_front(eventStrings);

    const auto backend = BackEnd::getBackEndForNode(mOpcNode.get());
    const auto maxEvents =
            backend ? backend->maxEventsPerObject() : Constants::Defaults::MaxEventsPerObject;

    if (mLastEvents.length() > maxEvents)
        mLastEvents.resize(maxEvents);

    emit lastEventsChanged();
}

void MonitoredItem::eventFieldTypeLookup()
{
    if (mType != Type::Event || !mOpcNode)
        return;

    if (mEventFieldNames.isEmpty())
        return;

    mEventFieldTypeNodeIds = QStringList(mEventFieldNames.size(), {});

    for (int i = 0; i < mEventFilter.selectClauses().size(); ++i) {
        if (mEventFilter.selectClauses().at(i).browsePath().isEmpty())
            continue;

        auto eventTypeId = mEventFilter.selectClauses().at(i).typeId();
        if (eventTypeId.isEmpty())
            eventTypeId = QOpcUa::namespace0Id(QOpcUa::NodeIds::Namespace0::BaseEventType);

        auto typeNode = mOpcNode->client()->node(eventTypeId);

        if (!typeNode)
            continue;

        QList<QOpcUaRelativePathElement> pathElements;
        for (const auto &element : mEventFilter.selectClauses().at(i).browsePath()) {
            QOpcUaRelativePathElement pathElement(
                    QOpcUaQualifiedName(element.namespaceIndex(), element.name()),
                    QOpcUa::ReferenceTypeId::HierarchicalReferences);
            pathElement.setIncludeSubtypes(true);
            pathElements.push_back(pathElement);
        }

        QObject::connect(
                typeNode, &QOpcUaNode::resolveBrowsePathFinished, this,
                [this, typeNode, i](const QList<QOpcUaBrowsePathTarget> &targets,
                                    const QList<QOpcUaRelativePathElement> &path,
                                    QOpcUa::UaStatusCode statusCode) {
                    Q_UNUSED(path)
                    Q_UNUSED(statusCode)

                    if (!targets.isEmpty() && targets.constFirst().isFullyResolved()
                        && !targets.constFirst().targetId().nodeId().isEmpty()) {
                        auto fieldNode =
                                typeNode->client()->node(targets.constFirst().targetId().nodeId());

                        typeNode->deleteLater();

                        if (!fieldNode)
                            return;

                        QObject::connect(
                                fieldNode, &QOpcUaNode::attributeRead, this,
                                [this, fieldNode, i](const QOpcUa::NodeAttributes &attributes) {
                                    if (attributes & QOpcUa::NodeAttribute::DataType)
                                        mEventFieldTypeNodeIds[i] =
                                                fieldNode
                                                        ->attribute(QOpcUa::NodeAttribute::DataType)
                                                        .toString();

                                    fieldNode->deleteLater();
                                });

                        const auto success =
                                fieldNode->readAttributes(QOpcUa::NodeAttribute::DataType);
                        if (!success)
                            fieldNode->deleteLater();
                    }
                });

        const auto success = typeNode->resolveBrowsePath(pathElements);
        if (!success)
            typeNode->deleteLater();
    }
}

void MonitoredItem::setStatusCode(QOpcUa::UaStatusCode statusCode)
{
    if (mStatusCode != statusCode) {
        const QOpcUa::UaStatusCode lastStatusCode = mStatusCode;
        mStatusCode = statusCode;

        emit statusChanged();
        if ((statusCode == QOpcUa::Good) || (lastStatusCode == QOpcUa::Good)) {
            emit hasErrorChanged();
        }
    }
}

QOpcUaMonitoringParameters::EventFilter MonitoredItem::getEventFilter() const
{
    return mEventFilter;
}

const QList<QVariantMap> &MonitoredItem::lastEvents() const
{
    return mLastEvents;
}

void MonitoredItem::clearEvents()
{
    mLastEvents.clear();
    emit lastEventsChanged();
}

MonitoredItem::Type MonitoredItem::type() const
{
    return mType;
}
