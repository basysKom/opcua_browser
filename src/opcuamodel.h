/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef OPCUAMODEL_H
#define OPCUAMODEL_H

#include <QAbstractItemModel>
#include <QQmlEngine>

#include "treeitem.h"

class QOpcUaClient;
class QOpcUaNode;

class OpcUaModel : public QAbstractItemModel
{
    Q_OBJECT
    QML_ANONYMOUS

    Q_PROPERTY(bool hasSelectedItems READ hasSelectedItems NOTIFY hasSelectedItemsChanged FINAL)

public:
    explicit OpcUaModel(QObject *parent = nullptr);

    void setOpcUaClient(QOpcUaClient *client);
    QOpcUaClient *opcUaClient() const noexcept;

    bool isHierarchicalReference(const QString &refTypeId) const;
    QString getStringForRefTypeId(const QString &refTypeId, bool isForward) const;
    QString getStringForDataTypeId(const QString &dataTypeId) const;

    virtual QHash<int, QByteArray> roleNames() const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &index) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE void setCurrentNodeId(const QString &nodeId);
    Q_INVOKABLE void setCurrentIndex(const QModelIndex &index);
    Q_INVOKABLE void refreshCurrentIndex();

    Q_INVOKABLE void refreshAttributesForCurrentIndex();
    Q_INVOKABLE void clearSelectionList();

    bool hasSelectedItems() const noexcept;
    const QStringList &selectedNodes() const noexcept;

signals:
    void hasSelectedItemsChanged();
    void browsingForReferenceTypesFinished();
    void browsingForDataTypesFinished();
    void currentIndexChanged(const QModelIndex &index);

private:
    void resetModel();
    void collectInverseNodeIds(const QString &nodeId, bool init = false);
    void browseReferenceTypes(QOpcUaNode *node, bool isHierachical = false);
    void browseDataTypes(QOpcUaNode *node);

    enum class EBrowseType { None = 0x00, ReferenceTypes = 0x01, DataTypes = 0x02 };
    Q_DECLARE_FLAGS(BrowseTypes, EBrowseType)

    struct ReferenceType
    {
        QString mDisplayName;
        QString mInverseName;
        bool mIsHierarchicalReference = false;

        ReferenceType() { }

        ReferenceType(const QString &displayName, const QString &inverseName,
                      bool isHierarchicalReference)
            : mDisplayName(displayName),
              mInverseName(inverseName),
              mIsHierarchicalReference(isHierarchicalReference)
        {
        }
    };

    QOpcUaClient *mOpcUaClient = nullptr;
    std::unique_ptr<TreeItem> mRootItem;
    QModelIndex mCurrentIndex = QModelIndex();
    BrowseTypes mBrowsedTypes = EBrowseType::None;
    QHash<QString, ReferenceType> mReferencesList;
    QHash<QString, QString> mDataTypesList;

    QString mCurrentNodeId;
    QStringList mInverseNodeIds;
    QStringList mSelectedNodeIds;

    friend class TreeItem;
};

#endif // OPCUAMODEL_H
