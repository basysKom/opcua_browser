/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef X509CERTIFICATE_H
#define X509CERTIFICATE_H

#include <QString>

class X509Certificate
{
public:
    static bool createCertificate(const QString &pkiDir);
};

#endif // X509CERTIFICATE_H
