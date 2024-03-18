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

#if __has_include(<QOpcUaGenericStructHandler>)
#  define HAS_GENERIC_STRUCT_HANDLER
#  include <QOpcUaGenericStructHandler>
#endif

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
    QHash<qint32, QString> getEnumStringsForDataTypeId(const QString &dataTypeId);

#ifdef HAS_GENERIC_STRUCT_HANDLER
    QOpcUaGenericStructHandler *genericStructHandler() const;
#endif

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
    const QList<QPersistentModelIndex> &selectedIndices() const noexcept;

    TreeItem *itemForIndex(const QModelIndex &index) const noexcept;

signals:
    void hasSelectedItemsChanged();
    void browsingForReferenceTypesFinished();
    void browsingForDataTypesFinished();
    void browsingForEnumStringsFinished();
    void currentIndexChanged(const QModelIndex &index);

private:
    void resetModel();
    void collectInverseNodeIds(const QString &nodeId, bool init = false);
    void browseReferenceTypes(QOpcUaNode *node, bool isHierachical = false);
    void browseDataTypes(QOpcUaNode *node);
    void browseEnumStrings(QOpcUaNode *node);

    bool allLookupsFinished() const;

    enum class EBrowseType {
        None = 0x00,
        ReferenceTypes = 0x01,
        DataTypes = 0x02,
        EnumStrings = 0x04,
        GenericStructs = 0x08
    };
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
    QHash<QString, QHash<qint32, QString>> mEnumStringsList;

#ifdef HAS_GENERIC_STRUCT_HANDLER
    std::unique_ptr<QOpcUaGenericStructHandler> mGenericStructHandler;
#endif

    QString mCurrentNodeId;
    QStringList mInverseNodeIds;
    QStringList mSelectedNodeIds;
    QList<QPersistentModelIndex> mSelectedIndices;

    friend class TreeItem;
};

#endif // OPCUAMODEL_H
