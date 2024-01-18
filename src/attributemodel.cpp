/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "attributemodel.h"

enum Roles : int {
    AttributeRole = Qt::DisplayRole,
    ValueRole = Qt::UserRole,
};

AttributeModel::AttributeModel(QObject *parent) : QAbstractListModel{ parent } { }

QHash<int, QByteArray> AttributeModel::roleNames() const
{
    return {
        { AttributeRole, "attribute" },
        { ValueRole, "value" },
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
        return mAttributes[index.row()].attributeName();
    case ValueRole:
        return mAttributes[index.row()].value();
    }

    return QVariant();
}

void AttributeModel::setAttribute(QOpcUa::NodeAttribute attribute, const QString &value)
{
    auto it = std::find_if(mAttributes.begin(), mAttributes.end(),
                           [&](const Attribute &attr) { return (attr.attribute() == attribute); });
    if (it == mAttributes.end()) {
        int count = mAttributes.size();
        beginInsertRows(QModelIndex(), count, count);
        mAttributes << Attribute(attribute, value);
        endInsertRows();
    } else {
        it->setValue(value);
        const int index = std::distance(mAttributes.begin(), it);
        const QModelIndex modelIndex = this->index(index);
        emit dataChanged(modelIndex, modelIndex, QList<int>() << ValueRole);
    }
}

void AttributeModel::setAttributes(const QList<Attribute> &attributes)
{
    beginResetModel();
    mAttributes = attributes;
    endResetModel();
}

void AttributeModel::clearAttributes()
{
    beginResetModel();
    mAttributes.clear();
    endResetModel();
}
