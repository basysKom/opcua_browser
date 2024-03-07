/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef WOODWORKINGDASHBOARDCREATOR_H
#define WOODWORKINGDASHBOARDCREATOR_H

#include "companionspecdashboardcreator.h"

class BackEnd;

class WoodworkingDashboardCreator : public CompanionSpecDashboardCreator
{
public:
    WoodworkingDashboardCreator(BackEnd *backend);

    void createDashboardsForObject(const QString &nodeId);

private:
};

#endif // WOODWORKINGDASHBOARDCREATOR_H
