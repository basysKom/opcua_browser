/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef COMPANIONSPECDASHBOARDCREATOR_H
#define COMPANIONSPECDASHBOARDCREATOR_H

#include "backend.h"

#include <QString>

class BackEnd;
struct CompanionSpecDevice;
class QOpcUaClient;

class CompanionSpecDashboardCreator : public QObject
{
    Q_OBJECT

public:
    CompanionSpecDashboardCreator(BackEnd *backend);
    virtual ~CompanionSpecDashboardCreator() = default;

    virtual void createDashboardsForObject(const QString &nodeId) = 0;

protected:
    BackEnd *backend() const;

private:
    QPointer<BackEnd> mBackend = nullptr;
};

#endif // COMPANIONSPECDASHBOARDCREATOR_H
