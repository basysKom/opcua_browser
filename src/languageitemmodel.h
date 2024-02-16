/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef LANGUAGEITEMMODEL_H
#define LANGUAGEITEMMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>

class QTranslator;

class LanguageItemModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit LanguageItemModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    Q_INVOKABLE void setCurrentIndex(qint32 index);

private:
    void setLanguage(const QString &langugeFilename);

    struct LanguageData
    {
        QString mName;
        QString mLanguageFilename;
        QString mFlagFilename;

        LanguageData() { }
        LanguageData(const QString &name, const QString &languageFilename,
                     const QString &flagFilename)
            : mName(name), mLanguageFilename(languageFilename), mFlagFilename(flagFilename)
        {
        }
    };

    QList<LanguageData> mItems;
    qint32 mCurrentIndex = -1;
    QPointer<QTranslator> mTranslator;
};

#endif // LANGUAGEITEMMODEL_H
