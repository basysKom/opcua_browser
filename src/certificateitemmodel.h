/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CERTIFICATEITEMMODEL_H
#define CERTIFICATEITEMMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QSslCertificate>

class CertificateItemModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit CertificateItemModel(const QString &trustedCertsPath, QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    Q_INVOKABLE void setCurrentIndex(int index);
    Q_INVOKABLE void removeCertificate(int index);
    void updateCertificateList();

private:
    struct Certificate
    {
        QString mFilename;
        QString mFingerprint;
        QSslCertificate mSslCertificate;

        Certificate() { }
        Certificate(const QString &filename, const QString &fingerprint,
                    const QSslCertificate &sslCertificate)
            : mFilename(filename), mFingerprint(fingerprint), mSslCertificate(sslCertificate)
        {
        }
    };

    int mCurrentIndex = 0;
    QString mTrustedCertsPath;
    QList<Certificate> mItems;
};

#endif // CERTIFICATEITEMMODEL_H
