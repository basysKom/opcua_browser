#include "attributemodel.h"

enum Roles : int {
    AttributeRole = Qt::DisplayRole,
    ValueRole = Qt::UserRole,
};

AttributeModel::AttributeModel(QObject *parent)
    : QAbstractListModel{parent}
{

}

QHash<int, QByteArray> AttributeModel::roleNames() const
{
    return {
        {AttributeRole, "attribute"},
        {ValueRole, "value"},
    };
}

int AttributeModel::rowCount(const QModelIndex &parent) const
{
    return mAttributes.size();
}

QVariant AttributeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case AttributeRole:
        return mAttributes[index.row()].attribute();
    case ValueRole:
        return mAttributes[index.row()].value();
    }

    return QVariant();
}

void AttributeModel::setAttributes(const QList<Attribute> &attributes)
{
    beginResetModel();
    mAttributes = attributes;
    endResetModel();
}
