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
    explicit MonitoredItem(QOpcUaNode *node, QObject *parent = nullptr);

    const QString &nodeId() const noexcept;
    const QString &displayName() const noexcept;
    const QString &value() const noexcept;
    QString status() const;
    bool hasError() const noexcept;

signals:
    void valueChanged();
    void displayNameChanged();
    void hasErrorChanged();
    void statusChanged();

private slots:
    void handleAttributes(const QOpcUa::NodeAttributes &attributes);

private:
    void setStatusCode(QOpcUa::UaStatusCode statusCode);

    std::unique_ptr<QOpcUaNode> mOpcNode;
    QString mNodeId;
    QString mDisplayName;
    QString mValue;
    QOpcUa::UaStatusCode mStatusCode = QOpcUa::Good;
    bool mHasReadValue = false;
};

#endif // MONITOREDITEM_H
