#ifndef REFERENCE_H
#define REFERENCE_H

#include <QString>

class Reference
{
public:
    explicit Reference(const QString& type, bool isForward, const QString &target);

    const QString &type() const noexcept;
    bool isForward() const noexcept;
    const QString &target() const noexcept;

private:
    QString mType;
    bool mIsForward;
    QString mTarget;
};

#endif // REFERENCE_H
