/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "companionspecdashboardcreator.h"

CompanionSpecDashboardCreator::CompanionSpecDashboardCreator(BackEnd *backend)
    : QObject(backend), mBackend(backend)
{
    assert(backend);
}

BackEnd *CompanionSpecDashboardCreator::backend() const
{
    return mBackend;
}
