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

    virtual QHash<int, QByteArray> roleNames() const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &index) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE void setCurrentIndex(const QModelIndex &index);
    Q_INVOKABLE void refreshIndex(const QModelIndex &index);

    Q_INVOKABLE void refreshAttributesForCurrentIndex();

signals:
    void browsingForReferenceTypesFinished();

private:
    void resetModel();
    void browseReferenceTypes(QOpcUaNode *node);

    QOpcUaClient *mOpcUaClient = nullptr;
    std::unique_ptr<TreeItem> mRootItem;
    QModelIndex mCurrentIndex = QModelIndex();
    QHash<QString, QPair<QString, QString>> mReferencesList;

    friend class TreeItem;
};

#endif // OPCUAMODEL_H
