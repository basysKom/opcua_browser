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

signals:
    void valueChanged();
    void displayNameChanged();

private slots:
    void handleAttributes(const QOpcUa::NodeAttributes &attributes);

private:
    std::unique_ptr<QOpcUaNode> mOpcNode;
    QString mNodeId;
    QString mDisplayName;
    QString mValue;
};

#endif // MONITOREDITEM_H
