#include "referencemodel.h"

enum Roles : int {
    TypeRole = Qt::DisplayRole,
    ForwardRole = Qt::UserRole,
    TargetRole
};

ReferenceModel::ReferenceModel(QObject *parent)
    : QAbstractListModel{parent}
{

}

QHash<int, QByteArray> ReferenceModel::roleNames() const
{
    return {
        {TypeRole, "type"},
        {ForwardRole, "isForward"},
        {TargetRole, "target"},
    };
}

int ReferenceModel::rowCount(const QModelIndex &parent) const
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
    case ForwardRole:
        return mReferences[index.row()].isForward();
    case TargetRole:
        return mReferences[index.row()].target();
    }

    return QVariant();
}

void ReferenceModel::addReference(const QString &type, bool isForward, const QString &target)
{
    int count = mReferences.size();
    beginInsertRows(QModelIndex(), count, count);
    mReferences << Reference(type, isForward, target);
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
