#ifndef REFERENCE_H
#define REFERENCE_H

#include <QString>

class Reference
{
public:
    explicit Reference(const QString &type, const QString &typeNodeId, bool isForward,
                       const QString &target, const QString &targetNodeId);

    const QString &type() const noexcept;
    const QString &typeNodeId() const noexcept;
    bool isForward() const noexcept;
    const QString &target() const noexcept;
    const QString &targetNodeId() const noexcept;

private:
    QString mType;
    QString mTypeNodeId;
    bool mIsForward;
    QString mTarget;
    QString mTargetNodeId;
};

#endif // REFERENCE_H
