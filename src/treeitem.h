/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QObject>

#include <QOpcUaReferenceDescription>

class AttributeModel;
class ReferenceModel;
class OpcUaModel;
class QAbstractItemModel;
class QSortFilterProxyModel;

class TreeItem : public QObject
{
    Q_OBJECT
public:
    TreeItem(const QString &nodeId, OpcUaModel *model, QOpcUa::NodeClass nodeClass,
             TreeItem *parent);
    TreeItem(const QString &nodeId, OpcUaModel *model,
             const QOpcUaReferenceDescription &browsingData, TreeItem *parent);
    ~TreeItem();

    QAbstractItemModel *attributes() const noexcept;
    QAbstractItemModel *references() const noexcept;

    TreeItem *child(int row);
    int childIndex(const TreeItem *child) const;
    int childCount();
    int row() const;
    TreeItem *parentItem();
    void appendChild(TreeItem *child);
    bool hasChildNodeItem(const QString &nodeId) const;

    const QString &nodeId() const noexcept;
    const QString &displayName() const noexcept;
    const QColor &nodeClassColor() const noexcept;
    const QString &value() const noexcept;
    bool canMonitored() const noexcept;
    bool hasEventNotifier() const noexcept;

    void refresh();
    void refreshAttributes();

private:
    void addItemToReferenceModel(const QOpcUaReferenceDescription &item);
    void startBrowsing();
    bool browse();

    OpcUaModel *mModel = nullptr;
    bool mBrowseStarted = false;
    QList<TreeItem *> mChildItems;
    QSet<QString> mChildNodeIds;
    TreeItem *mParentItem = nullptr;

    AttributeModel *mAttributeModel;
    QSortFilterProxyModel *mSortedAttributeProxyModel;
    ReferenceModel *mReferenceModel;
    QSortFilterProxyModel *mSortedReferenceProxyModel;
    QString mDisplayName;
    QString mNodeId;
    QOpcUa::NodeClass mNodeClass = QOpcUa::NodeClass::Undefined;
    QString mValue;
    quint32 mEventNotifier = 0;
};

#endif // TREEITEM_H
