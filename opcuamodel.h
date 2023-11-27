#ifndef OPCUAMODEL_H
#define OPCUAMODEL_H

#include <QAbstractItemModel>

#include "treeitem.h"

class QOpcUaClient;
class QOpcUaNode;

class OpcUaModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit OpcUaModel(QObject *parent = nullptr);

    void setOpcUaClient(QOpcUaClient *client);
    QOpcUaClient *opcUaClient() const noexcept;

    QString getStringForRefTypeId(const QString &refTypeId, bool isForward) const;
    QString getStringForDataTypeId(const QString &dataTypeId) const;

    virtual QHash<int, QByteArray> roleNames() const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &index) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE void setCurrentNodeId(const QString &nodeId);
    Q_INVOKABLE void setCurrentIndex(const QModelIndex &index);
    Q_INVOKABLE void refreshIndex(const QModelIndex &index);

    Q_INVOKABLE void refreshAttributesForCurrentIndex();

signals:
    void browsingForReferenceTypesFinished();
    void browsingForDataTypesFinished();
    void currentIndexChanged(const QModelIndex &index);

private:
    void resetModel();
    void browseReferenceTypes(QOpcUaNode *node);
    void browseDataTypes(QOpcUaNode *node);

    enum class EBrowseType { None = 0x00, ReferenceTypes = 0x01, DataTypes = 0x02 };
    Q_DECLARE_FLAGS(BrowseTypes, EBrowseType)

    QOpcUaClient *mOpcUaClient = nullptr;
    std::unique_ptr<TreeItem> mRootItem;
    QModelIndex mCurrentIndex = QModelIndex();
    BrowseTypes mBrowsedTypes = EBrowseType::None;
    QHash<QString, QPair<QString, QString>> mReferencesList;
    QHash<QString, QString> mDataTypesList;

    friend class TreeItem;
};

#endif // OPCUAMODEL_H
