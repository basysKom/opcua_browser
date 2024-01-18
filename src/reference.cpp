/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "reference.h"

Reference::Reference(const QString &type, const QString &typeNodeId, bool isForward,
                     const QString &target, const QString &targetNodeId)
    : mType(type),
      mTypeNodeId(typeNodeId),
      mIsForward(isForward),
      mTarget(target),
      mTargetNodeId(targetNodeId)
{
}

const QString &Reference::type() const noexcept
{
    return mType;
}

const QString &Reference::typeNodeId() const noexcept
{
    return mTypeNodeId;
}

bool Reference::isForward() const noexcept
{
    return mIsForward;
}

const QString &Reference::target() const noexcept
{
    return mTarget;
}

const QString &Reference::targetNodeId() const noexcept
{
    return mTargetNodeId;
}
