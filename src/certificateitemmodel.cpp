/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QDir>
#include <QLoggingCategory>
#include <QOpcUaKeyPair>
#include <QSslKey>

#include "certificateitemmodel.h"

Q_LOGGING_CATEGORY(certificateLog, "opcua_browser.certificate");

enum Roles : int {
    IssuerDisplayNameRole = Qt::DisplayRole,
    EffectiveDateRole = Qt::UserRole,
    ExpiryDateRole,
    FingerprintRole,
    SerialNumberRole,
    VersionRole,
    CommonNameRole,
    OrganizationRole,
    OrganizationUnitRole,
    LocalityNameRole,
    CountryNameRole,
    StateOrProvinceNameRole,
    CurrentItemRole
};

CertificateItemModel::CertificateItemModel(const QString &trustedCertsPath, QObject *parent)
    : QAbstractListModel{ parent }, mTrustedCertsPath(trustedCertsPath)
{
    updateCertificateList();
}

QHash<int, QByteArray> CertificateItemModel::roleNames() const
{
    return {
        { IssuerDisplayNameRole, "issuerDisplayName" },
        { EffectiveDateRole, "effectiveDate" },
        { ExpiryDateRole, "expiryDate" },
        { FingerprintRole, "fingerprint" },
        { SerialNumberRole, "serialNumber" },
        { VersionRole, "version" },
        { CommonNameRole, "commonName" },
        { OrganizationRole, "organisation" },
        { OrganizationUnitRole, "organisationUnit" },
        { LocalityNameRole, "localityName" },
        { CountryNameRole, "countryName" },
        { StateOrProvinceNameRole, "stateOrProvince" },
        { CurrentItemRole, "isCurrentItem" },
    };
}

int CertificateItemModel::rowCount(const QModelIndex &) const
{
    return mItems.size();
}

QVariant CertificateItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case IssuerDisplayNameRole:
        return mItems[index.row()].mSslCertificate.issuerDisplayName();
    case EffectiveDateRole:
        return mItems[index.row()].mSslCertificate.effectiveDate();
    case ExpiryDateRole:
        return mItems[index.row()].mSslCertificate.expiryDate();
    case FingerprintRole:
        return mItems[index.row()].mFingerprint;
    case SerialNumberRole:
        return QString::fromUtf8(mItems[index.row()].mSslCertificate.serialNumber())
                .remove(QChar::fromLatin1(':'));
    case VersionRole:
        return mItems[index.row()].mSslCertificate.version();
    case CommonNameRole:
        return mItems[index.row()]
                .mSslCertificate.issuerInfo(QSslCertificate::CommonName)
                .join(QChar::fromLatin1(','));
    case OrganizationRole:
        return mItems[index.row()]
                .mSslCertificate.issuerInfo(QSslCertificate::Organization)
                .join(QChar::fromLatin1(','));
    case OrganizationUnitRole:
        return mItems[index.row()]
                .mSslCertificate.issuerInfo(QSslCertificate::OrganizationalUnitName)
                .join(QChar::fromLatin1(','));
    case LocalityNameRole:
        return mItems[index.row()]
                .mSslCertificate.issuerInfo(QSslCertificate::LocalityName)
                .join(QChar::fromLatin1(','));
    case CountryNameRole:
        return mItems[index.row()]
                .mSslCertificate.issuerInfo(QSslCertificate::CountryName)
                .join(QChar::fromLatin1(','));
    case StateOrProvinceNameRole:
        return mItems[index.row()]
                .mSslCertificate.issuerInfo(QSslCertificate::StateOrProvinceName)
                .join(QChar::fromLatin1(','));
    case CurrentItemRole:
        return mCurrentIndex == index.row();
    }

    return QVariant();
}

void CertificateItemModel::setCurrentIndex(int row)
{
    if (row != mCurrentIndex) {
        const int lastIndex = mCurrentIndex;
        mCurrentIndex = row;
        emit dataChanged(index(row), index(row), QList<int>() << CurrentItemRole);
        emit dataChanged(index(lastIndex), index(lastIndex), QList<int>() << CurrentItemRole);
    }
}

void CertificateItemModel::removeCertificate(int index)
{
    if (index >= mItems.size())
        return;

    const QString filename = mTrustedCertsPath % QChar::fromLatin1('/') % mItems[index].mFilename;
    beginRemoveRows(QModelIndex(), index, index);
    auto item = mItems.takeAt(index);
    endRemoveRows();

    if (mCurrentIndex == index) {
        setCurrentIndex(0);
    } else if (mCurrentIndex > index) {
        setCurrentIndex(mCurrentIndex - 1);
    }

    if (!QFile::remove(filename))
        qCWarning(certificateLog) << "could not delete certificate" << filename;
}

void CertificateItemModel::updateCertificateList()
{
    QList<Certificate> sslCertificates;
    const QStringList fileNames =
            QDir(mTrustedCertsPath).entryList(QStringList() << QStringLiteral("*.der"));
    for (const QString &fileName : fileNames) {
        QFile certificate(mTrustedCertsPath % fileName);
        if (certificate.open(QIODevice::ReadOnly)) {
            const QSslCertificate ssl(certificate.readAll(), QSsl::Der);
            const auto sha256Fingerprint =
                    QString::fromUtf8(ssl.digest(QCryptographicHash::Sha256).toHex());
            sslCertificates << Certificate(fileName, sha256Fingerprint, ssl);
        } else {
            qCWarning(certificateLog) << "cannot open certificate" << fileName;
        }
    }

    beginResetModel();
    std::swap(mItems, sslCertificates);
    endResetModel();

    setCurrentIndex(0);
}
