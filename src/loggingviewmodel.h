/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef LOGGINGVIEWMODEL_H
#define LOGGINGVIEWMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>

class LoggingViewModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit LoggingViewModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    void addLogMessage(QtMsgType type, const QString &message);
    Q_INVOKABLE void clearItems();

private:
    struct LogMessage
    {
        QtMsgType mType;
        QString mCategory;
        QString mTimestamp;
        QString mMessage;

        LogMessage() { }

        LogMessage(QtMsgType type, const QString &category, const QString &timestamp,
                   const QString &message)
            : mType(type), mCategory(category), mTimestamp(timestamp), mMessage(message)
        {
        }
    };

    QList<LogMessage> mItems;
};

#endif // LOGGINGVIEWMODEL_H
