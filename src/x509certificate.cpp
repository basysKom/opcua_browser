/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QDir>
#include <QFile>
#include <QOpcUaKeyPair>
#include <QOpcUaX509CertificateSigningRequest>
#include <QOpcUaX509ExtensionBasicConstraints>
#include <QOpcUaX509ExtensionExtendedKeyUsage>
#include <QOpcUaX509ExtensionKeyUsage>
#include <QOpcUaX509ExtensionSubjectAlternativeName>

#include "x509certificate.h"

bool X509Certificate::createCertificate(const QString &pkiDir)
{
    const QString certsPath = pkiDir % QStringLiteral("/own/certs");
    const QString privatePath = pkiDir % QStringLiteral("/own/private");
    if (!QDir().mkpath(certsPath))
        return false;

    if (!QDir().mkpath(privatePath))
        return false;

    // Generate RSA Key
    QOpcUaKeyPair key;
    key.generateRsaKey(QOpcUaKeyPair::RsaKeyStrength::Bits2048);

    // Save private key to file
    const QByteArray keyData =
            key.privateKeyToByteArray(QOpcUaKeyPair::Cipher::Unencrypted, QString());

    QFile keyFile(privatePath % QStringLiteral("/opcuabrowser.pem"));
    if (!keyFile.open(QIODevice::WriteOnly))
        return false;

    keyFile.write(keyData);
    keyFile.close();

    // Create a certificate signing request
    QOpcUaX509CertificateSigningRequest csr;
    csr.setEncoding(QOpcUaX509CertificateSigningRequest::Encoding::DER);

    // Set the subject of the certificate
    QOpcUaX509DistinguishedName dn;
    dn.setEntry(QOpcUaX509DistinguishedName::Type::CommonName, QStringLiteral("OpcUaBrowser"));
    dn.setEntry(QOpcUaX509DistinguishedName::Type::CountryName, QStringLiteral("DE"));
    dn.setEntry(QOpcUaX509DistinguishedName::Type::LocalityName, QStringLiteral("Darmstadt"));
    dn.setEntry(QOpcUaX509DistinguishedName::Type::StateOrProvinceName, QStringLiteral("Hesse"));
    dn.setEntry(QOpcUaX509DistinguishedName::Type::OrganizationName,
                QStringLiteral("basysKom GmbH"));
    csr.setSubject(dn);

    // The subject alternative name extension is needed for OPC UA
    QOpcUaX509ExtensionSubjectAlternativeName *san = new QOpcUaX509ExtensionSubjectAlternativeName;
    san->addEntry(QOpcUaX509ExtensionSubjectAlternativeName::Type::DNS, QStringLiteral("foo.com"));
    san->addEntry(QOpcUaX509ExtensionSubjectAlternativeName::Type::URI,
                  QStringLiteral("urn:foo.com:basysKom%20GmbH:OpcUaBrowser"));
    san->setCritical(true);
    csr.addExtension(san);

    // Set the certificate basic constraints
    QOpcUaX509ExtensionBasicConstraints *bc = new QOpcUaX509ExtensionBasicConstraints;
    bc->setCa(false);
    bc->setCritical(true);
    csr.addExtension(bc);

    // The required values for key usage and extended key usage are defined in OPC
    // UA Part 6, 6.2.2, Table 43

    // Set the key usage constraints
    QOpcUaX509ExtensionKeyUsage *ku = new QOpcUaX509ExtensionKeyUsage;
    ku->setCritical(true);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DigitalSignature);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::NonRepudiation);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::KeyEncipherment);
    ku->setKeyUsage(QOpcUaX509ExtensionKeyUsage::KeyUsage::DataEncipherment);
    csr.addExtension(ku);

    // Set the extended key usage constraints
    QOpcUaX509ExtensionExtendedKeyUsage *eku = new QOpcUaX509ExtensionExtendedKeyUsage;
    eku->setCritical(true);
    eku->setKeyUsage(QOpcUaX509ExtensionExtendedKeyUsage::KeyUsage::TlsWebClientAuthentication);
    csr.addExtension(eku);

    const QByteArray selfSignedCertificateData = csr.createSelfSignedCertificate(key, 365 * 20);

    QFile certFile(certsPath % QStringLiteral("/opcuabrowser.der"));
    if (!certFile.open(QIODevice::WriteOnly))
        return false;

    certFile.write(selfSignedCertificateData);
    certFile.close();

    return true;
}
