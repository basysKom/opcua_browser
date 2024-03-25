/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MONITOREDITEM_H
#define MONITOREDITEM_H

#include <QObject>
#include <QString>
#include <QOpcUaNode>

class MonitoredItem : public QObject
{
    Q_OBJECT

public:
    enum class Type {
        Unknown = 0,
        Variable = 1,
        Event = 2,
    };
    Q_ENUM(Type)

    explicit MonitoredItem(QOpcUaNode *node, QObject *parent = nullptr);
    explicit MonitoredItem(QOpcUaNode *node,
                           const QOpcUaMonitoringParameters::EventFilter &eventFilter,
                           QObject *parent = nullptr);

    const QString &nodeId() const noexcept;
    const QString &displayName() const noexcept;
    const QString &value() const noexcept;
    QString status() const;
    bool hasError() const noexcept;

    Type type() const;

    const QList<QVariantMap> &lastEvents() const;
    void clearEvents();

    QOpcUaMonitoringParameters::EventFilter getEventFilter() const;

signals:
    void valueChanged();
    void displayNameChanged();
    void hasErrorChanged();
    void statusChanged();
    void lastEventsChanged();

private slots:
    void handleAttributes(const QOpcUa::NodeAttributes &attributes);
    void handleEvent(const QVariantList &eventFields);

    void eventFieldTypeLookup();

private:
    void setStatusCode(QOpcUa::UaStatusCode statusCode);

    std::unique_ptr<QOpcUaNode> mOpcNode;
    QString mNodeId;
    QString mDisplayName;
    QString mValue;
    QOpcUa::UaStatusCode mStatusCode = QOpcUa::Good;
    bool mHasReadValue = false;
    Type mType;

    QStringList mEventFieldNames;
    QStringList mEventFieldTypeNodeIds;
    QList<QVariantMap> mLastEvents;
    QOpcUaMonitoringParameters::EventFilter mEventFilter;
};

#endif // MONITOREDITEM_H
