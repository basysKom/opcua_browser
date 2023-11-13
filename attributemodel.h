#ifndef ATTRIBUTEMODEL_H
#define ATTRIBUTEMODEL_H

#include <QAbstractListModel>

#include "attribute.h"

class AttributeModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AttributeModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    void setAttributes(const QList<Attribute> &attributes);

private:
    QList<Attribute> mAttributes;
};

#endif // ATTRIBUTEMODEL_H
