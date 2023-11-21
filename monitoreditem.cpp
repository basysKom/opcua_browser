#include <QOpcUaNode>

#include "monitoreditem.h"
#include "opcuahelper.h"

MonitoredItem::MonitoredItem(QOpcUaNode *node, QObject *parent)
    : QObject(parent)
    , mOpcNode(node)
    , mNodeId(node->nodeId())
{
    Q_ASSERT(node);

    connect(mOpcNode.get(), &QOpcUaNode::attributeRead, this, &MonitoredItem::handleAttributes);
    connect(mOpcNode.get(), &QOpcUaNode::attributeUpdated, this, &MonitoredItem::handleAttributes);

    node->readAttributes(QOpcUa::NodeAttribute::BrowseName | QOpcUa::NodeAttribute::DisplayName | QOpcUa::NodeAttribute::Value);

    QOpcUaMonitoringParameters p(100);
    node->enableMonitoring(QOpcUa::NodeAttribute::Value, p);
}

const QString &MonitoredItem::nodeId() const noexcept
{
    return mNodeId;
}

const QString &MonitoredItem::displayName() const noexcept
{
    return mDisplayName;
}

const QString &MonitoredItem::value() const noexcept
{
    return mValue;
}

void MonitoredItem::handleAttributes(const QOpcUa::NodeAttributes &attributes)
{
    QString displayName;
    if (attributes.testFlag(QOpcUa::NodeAttribute::DisplayName)) {
        displayName = QOpcUaHelper::getAttributeValue(mOpcNode.get(), QOpcUa::NodeAttribute::DisplayName);
    }

    if (displayName.isEmpty() && attributes.testFlag(QOpcUa::NodeAttribute::BrowseName)) {
        displayName = QOpcUaHelper::getAttributeValue(mOpcNode.get(), QOpcUa::NodeAttribute::BrowseName);
    }

    if (!displayName.isEmpty() && (displayName != mDisplayName)) {
        mDisplayName = displayName;
        emit displayNameChanged();
    }

    if (attributes.testFlag(QOpcUa::NodeAttribute::Value)) {
        const QString newValue = QOpcUaHelper::getAttributeValue(mOpcNode.get(), QOpcUa::NodeAttribute::Value);
        if (newValue != mValue) {
            mValue = newValue;
            emit valueChanged();
        }
    }
}
