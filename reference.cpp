#include "reference.h"

Reference::Reference(const QString &type, bool isForward, const QString &target)
    : mType(type), mIsForward(isForward), mTarget(target)
{
}

const QString &Reference::type() const noexcept
{
    return mType;
}

bool Reference::isForward() const noexcept
{
    return mIsForward;
}

const QString &Reference::target() const noexcept
{
    return mTarget;
}
