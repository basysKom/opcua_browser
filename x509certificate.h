#ifndef X509CERTIFICATE_H
#define X509CERTIFICATE_H

#include <QString>

class X509Certificate
{
public:
    static bool createCertificate(const QString &pkiDir);
};

#endif // X509CERTIFICATE_H
