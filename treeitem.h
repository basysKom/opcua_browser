#ifndef TREEITEM_H
#define TREEITEM_H

#include <QObject>
#include <QHash>
#include <QQmlPropertyMap>

#include <QOpcUaNode>

class AttributeModel;
class OpcUaModel;
class QAbstractItemModel;
class QAbstractListModel;
class QSortFilterProxyModel;

class TreeItem : public QObject
{
    Q_OBJECT
public:
    explicit TreeItem(OpcUaModel *model);
    TreeItem(QOpcUaNode *node, OpcUaModel *model, QOpcUa::NodeClass nodeClass, TreeItem *parent);
    TreeItem(QOpcUaNode *node, OpcUaModel *model, const QOpcUaReferenceDescription &browsingData, TreeItem *parent);
    ~TreeItem();

    QAbstractItemModel *attributes() const noexcept;
    QAbstractListModel *references() const noexcept;

    TreeItem *child(int row);
    int childIndex(const TreeItem *child) const;
    int childCount();
    int row() const;
    TreeItem *parentItem();
    void appendChild(TreeItem *child);
    bool hasChildNodeItem(const QString &nodeId) const;

    const QString &displayName() const noexcept;
    const QColor &nodeClassColor() const noexcept;
    QString value() const noexcept;
    bool canMonitored() const noexcept;

    void refresh();
    void refreshAttributes();

    void enableMonitoring();
    void disableMonitoring();

private slots:
    void startBrowsing();
    void handleAttributes(const QOpcUa::NodeAttributes &attr);
    void browseFinished(const QList<QOpcUaReferenceDescription> &children, QOpcUa::UaStatusCode statusCode);

private:
    std::unique_ptr<QOpcUaNode> mOpcNode;
    OpcUaModel *mModel = nullptr;
    bool mAttributesReady = false;
    bool mBrowseStarted = false;
    QList<TreeItem *> mChildItems;
    QSet<QString> mChildNodeIds;
    TreeItem *mParentItem = nullptr;

    AttributeModel *mAttributeModel;
    QSortFilterProxyModel *mSortedAttributeProxyModel;
    QString mNodeBrowseName;
    QString mNodeId;
    QOpcUa::NodeClass mNodeClass = QOpcUa::NodeClass::Undefined;
    QString mValue;
};

#endif // TREEITEM_H
