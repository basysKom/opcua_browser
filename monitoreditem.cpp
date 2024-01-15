#include <QOpcUaNode>

#include "monitoreditem.h"
#include "opcuahelper.h"

MonitoredItem::MonitoredItem(QOpcUaNode *node, QObject *parent) : QObject(parent), mOpcNode(node)
{
    // Add item in dashboard has a null pointer
    if (node != nullptr) {
        mNodeId = node->nodeId();

        connect(mOpcNode.get(), &QOpcUaNode::attributeRead, this, &MonitoredItem::handleAttributes);
        connect(mOpcNode.get(), &QOpcUaNode::attributeUpdated, this,
                &MonitoredItem::handleAttributes);

        node->readAttributes(QOpcUa::NodeAttribute::BrowseName | QOpcUa::NodeAttribute::DisplayName
                             | QOpcUa::NodeAttribute::Value);

        QOpcUaMonitoringParameters p(100);
        node->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
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
        }
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
