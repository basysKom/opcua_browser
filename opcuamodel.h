#ifndef OPCUAMODEL_H
#define OPCUAMODEL_H

#include <QQmlEngine>

#include <QAbstractItemModel>
#include <QOpcUaClient>
#include <QOpcUaNode>

#include "treeitem.h"

class OpcUaModel : public QAbstractItemModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit OpcUaModel(QObject *parent = nullptr);

    void setOpcUaClient(QOpcUaClient *client);
    QOpcUaClient* opcUaClient() const;

    virtual QHash<int, QByteArray> roleNames() const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &index) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE void setCurrentIndex(const QModelIndex &index);
    Q_INVOKABLE void refreshIndex(const QModelIndex &index);

private:
    QOpcUaClient *mOpcUaClient = nullptr;
    std::unique_ptr<TreeItem> mRootItem;
    QModelIndex mCurrentIndex = QModelIndex();

    friend class TreeItem;
};

#endif // OPCUAMODEL_H
