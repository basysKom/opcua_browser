/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QOpcUaNode>
#include <QOpcUaQualifiedName>

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
            const auto stringValue = QOpcUaHelper::variantToString(nullptr, eventFields.at(i), {});
            eventStrings[mEventFieldNames.at(i)] = stringValue;
        }
    }

    mLastEvents.push_front(eventStrings);
    if (mLastEvents.length() > 25)
        mLastEvents.pop_back();

    emit lastEventsChanged();
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
