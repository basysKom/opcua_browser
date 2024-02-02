/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "loggingviewmodel.h"

enum Roles : int {
    MessageRole = Qt::DisplayRole,
    TimestampRole = Qt::UserRole,
    TypeRole,
    CategoryRole
};

LoggingViewModel::LoggingViewModel(QObject *parent) : QAbstractListModel{ parent } { }

QHash<int, QByteArray> LoggingViewModel::roleNames() const
{
    return {
        { MessageRole, "message" },
        { TimestampRole, "timestamp" },
        { TypeRole, "type" },
        { CategoryRole, "category" },
    };
}

int LoggingViewModel::rowCount(const QModelIndex &parent) const
{
    return mItems.size();
}

QVariant LoggingViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case MessageRole:
        return mItems[index.row()].mMessage;
    case TimestampRole:
        return mItems[index.row()].mTimestamp;
    case TypeRole:
        return static_cast<int>(mItems[index.row()].mType);
    case CategoryRole:
        return mItems[index.row()].mCategory;
    }

    return QVariant();
}

void LoggingViewModel::addLogMessage(QtMsgType type, const QString &message)
{
    const QStringList parts = message.split("|##|");
    if (parts.count() < 4)
        return;

    const LogMessage msg(type, parts.at(2), parts.at(0), parts.at(3));

    beginInsertRows(QModelIndex(), 0, 0);
    mItems.prepend(msg);
    endInsertRows();
}

void LoggingViewModel::clearItems()
{
    beginResetModel();
    mItems.clear();
    endResetModel();
}
