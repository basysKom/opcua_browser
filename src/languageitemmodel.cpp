/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QSettings>
#include <QTranslator>

#include "languageitemmodel.h"

Q_LOGGING_CATEGORY(languageLog, "opcua_browser.language");

static constexpr std::string_view SETTINGS_NAME = "language";

enum Roles : int {
    DisplayNameRole = Qt::DisplayRole,
    FlagFilenameRole = Qt::UserRole,
    CurrentItemRole
};

LanguageItemModel::LanguageItemModel(QObject *parent) : QAbstractListModel{ parent }
{
    QSettings settings;
    const QString settingsFileame = settings.value(SETTINGS_NAME, "").toString();

    const QString fallbackLocale = QStringLiteral("en_GB");
    const QString systemLocale = QLocale().name();
    const QString qmDirectory = QStringLiteral(":/i18n/");
    const QStringList qmFiles = QDir(qmDirectory).entryList(QStringList("*.qm"));
    qCDebug(languageLog) << "installed languages" << qmFiles << systemLocale;

    qint32 settingsIndex = -1;
    qint32 systemLocaleIndex = -1;
    qint32 fallbackLocaleIndex = -1;
    for (const auto &file : qmFiles) {
        QString name = file.split("_").first();
        const QString pngFilename(QStringLiteral("qrc:/languages/%1.png").arg(name));
        QFile utf8File(QStringLiteral(":/languages/%1.utf8").arg(name));
        if (utf8File.open(QIODevice::ReadOnly)) {
            name = utf8File.readAll();
        } else {
            qCWarning(languageLog) << "could not open language name file" << utf8File.fileName();
        }

        if (settingsFileame.endsWith(file)) {
            settingsIndex = mItems.size();
        } else if (file.chopped(3).endsWith(systemLocale)) {
            systemLocaleIndex = mItems.size();
        } else if (file.chopped(3).endsWith(fallbackLocale)) {
            fallbackLocaleIndex = mItems.size();
        }

        mItems << LanguageData(name, qmDirectory % file, pngFilename);
    }

    if (settingsIndex >= 0) {
        setCurrentIndex((quint32)settingsIndex);
    } else if (systemLocaleIndex >= 0) {
        setCurrentIndex((quint32)systemLocaleIndex);
    } else if (fallbackLocaleIndex >= 0) {
        setCurrentIndex((quint32)fallbackLocaleIndex);
    }
}

QHash<int, QByteArray> LanguageItemModel::roleNames() const
{
    return {
        { DisplayNameRole, "displayName" },
        { FlagFilenameRole, "flagFilename" },
        { CurrentItemRole, "isCurrentItem" },
    };
}

int LanguageItemModel::rowCount(const QModelIndex &) const
{
    return mItems.size();
}

QVariant LanguageItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case DisplayNameRole:
        return mItems[index.row()].mName;
    case FlagFilenameRole:
        return mItems[index.row()].mFlagFilename;
    case CurrentItemRole:
        return mCurrentIndex == index.row();
    }

    return QVariant();
}

void LanguageItemModel::setCurrentIndex(qint32 row)
{
    if ((row < mItems.size()) && (row != mCurrentIndex)) {
        const int lastIndex = mCurrentIndex;
        mCurrentIndex = row;
        emit dataChanged(index(row), index(row), QList<int>() << CurrentItemRole);
        emit dataChanged(index(lastIndex), index(lastIndex), QList<int>() << CurrentItemRole);

        setLanguage(mItems[row].mLanguageFilename);
    }
}

void LanguageItemModel::setLanguage(const QString &langugeFilename)
{
    if (mTranslator.isNull()) {
        mTranslator = new QTranslator(this);
        qApp->installTranslator(mTranslator.data());
    }

    if (mTranslator->load(langugeFilename)) {
        static QRegularExpression re("([A-Z,a-z,_]+)_([a-z]{2,2})_([A-Z]{2,2})");
        QRegularExpressionMatch match = re.match(langugeFilename);
        QLocale locale = QLocale(match.captured(2) % "_" % match.captured(3));
        QLocale::setDefault(locale);

        if (auto engine = qmlEngine(this))
            engine->retranslate();

        qCDebug(languageLog) << "changed language to" << mItems[mCurrentIndex].mName;

        QSettings settings;
        settings.setValue(SETTINGS_NAME, langugeFilename);
    } else {
        qCWarning(languageLog) << "could not load language file" << langugeFilename;
    }
}
