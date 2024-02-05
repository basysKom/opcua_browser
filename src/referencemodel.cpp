/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "referencemodel.h"

enum Roles : int {
    TypeRole = Qt::DisplayRole,
    TypeNodeIdRole = Qt::UserRole,
    ForwardRole,
    TargetRole,
    TargetNodeIdRole
};

ReferenceModel::ReferenceModel(QObject *parent) : QAbstractListModel{ parent } { }

QHash<int, QByteArray> ReferenceModel::roleNames() const
{
    return {
        { TypeRole, "type" },
        { TypeNodeIdRole, "typeNodeId" },
        { ForwardRole, "isForward" },
        { TargetRole, "target" },
        { TargetNodeIdRole, "targetNodeId" },
    };
}

int ReferenceModel::rowCount(const QModelIndex &) const
{
    return mReferences.size();
}

QVariant ReferenceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case TypeRole:
        return mReferences[index.row()].type();
    case TypeNodeIdRole:
        return mReferences[index.row()].typeNodeId();
    case ForwardRole:
        return mReferences[index.row()].isForward();
    case TargetRole:
        return mReferences[index.row()].target();
    case TargetNodeIdRole:
        return mReferences[index.row()].targetNodeId();
    }

    return QVariant();
}

void ReferenceModel::addReference(const QString &type, const QString &typeNodeId, bool isForward,
                                  const QString &target, const QString &targetNodeId)
{
    int count = mReferences.size();
    beginInsertRows(QModelIndex(), count, count);
    mReferences << Reference(type, typeNodeId, isForward, target, targetNodeId);
    endInsertRows();
}

void ReferenceModel::setReferences(const QList<Reference> &references)
{
    beginResetModel();
    mReferences = references;
    endResetModel();
}

void ReferenceModel::clearReferences()
{
    beginResetModel();
    mReferences.clear();
    endResetModel();
}

void ReferenceModel::clearForwardReferences()
{
    QList<Reference> newReferences;
    for (const auto &ref : mReferences) {
        if (!ref.isForward()) {
            newReferences << ref;
        }
    }

    beginResetModel();
    mReferences = std::move(newReferences);
    endResetModel();
}
