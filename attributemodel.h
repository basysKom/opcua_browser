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

    void setAttribute(QOpcUa::NodeAttribute attribute, const QString &value);
    void setAttributes(const QList<Attribute> &attributes);
    void clearAttributes();

private:
    QList<Attribute> mAttributes;
};

#endif // ATTRIBUTEMODEL_H
