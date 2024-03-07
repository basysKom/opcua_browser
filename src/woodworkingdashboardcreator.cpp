/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "constants.h"
#include "woodworkingdashboardcreator.h"

WoodworkingDashboardCreator::WoodworkingDashboardCreator(BackEnd *backend)
    : CompanionSpecDashboardCreator(backend)
{
}

void WoodworkingDashboardCreator::createDashboardsForObject(const QString &nodeId)
{
    if (nodeId.isEmpty())
        return;

    if (!backend())
        return;

    auto client = backend()->getOpcUaClient();

    if (!client)
        return;

    std::shared_ptr<QOpcUaNode> node(client->node(nodeId));
    if (!node)
        return;

    const auto indexCandidate =
            client->namespaceArray().indexOf(Constants::NamespaceUri::Woodworking);

    if (indexCandidate < 0)
        return;

    const quint16 wwIndex = indexCandidate;

    QObject::connect(node.get(), &QOpcUaNode::resolveBrowsePathFinished, this,
                     [this, nodeId, node](const QList<QOpcUaBrowsePathTarget> &targets,
                                          const QList<QOpcUaRelativePathElement> &path,
                                          QOpcUa::UaStatusCode statusCode) {
                         Q_UNUSED(path)
                         Q_UNUSED(statusCode)

                         if (targets.empty())
                             return;

                         if (!targets.first().isFullyResolved())
                             return;

                         if (!backend())
                             return;

                         const auto device = backend()->getCompanionSpecDeviceForNodeId(nodeId);

                         if (device) {
                             const auto dashboardName =
                                     QStringLiteral("%1 (Woodworking)").arg(device->name());

                             // Register the dashboard so it is displayed in the default dashboard
                             // combo box
                             backend()->addDefaultVariableDashboard(dashboardName);

                             // Add variable to the companion spec variable dashboard
                             backend()->addNodeIdToCompanionSpecVariableDashboard(
                                     dashboardName, targets.first().targetId().nodeId());
                         }
                     });

    node->resolveBrowsePath(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Overview") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("CurrentMode") },
                QOpcUa::ReferenceTypeId::HasComponent } });
    node->resolveBrowsePath(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Overview") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("CurrentState") },
                QOpcUa::ReferenceTypeId::HasComponent } });
    node->resolveBrowsePath(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Alarm") }, QOpcUa::ReferenceTypeId::HasComponent } });
    node->resolveBrowsePath(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Calibrated") },
                QOpcUa::ReferenceTypeId::HasComponent } });
    node->resolveBrowsePath(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Emergency") },
                QOpcUa::ReferenceTypeId::HasComponent } });
    node->resolveBrowsePath(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Error") }, QOpcUa::ReferenceTypeId::HasComponent } });
    node->resolveBrowsePath(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("MachineInitialized") },
                QOpcUa::ReferenceTypeId::HasComponent } });
    node->resolveBrowsePath(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("MachineOn") },
                QOpcUa::ReferenceTypeId::HasComponent } });
    node->resolveBrowsePath(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("PowerPresent") },
                QOpcUa::ReferenceTypeId::HasComponent } });
    node->resolveBrowsePath(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("RecipeInRun") },
                QOpcUa::ReferenceTypeId::HasComponent } });
    node->resolveBrowsePath(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Warning") }, QOpcUa::ReferenceTypeId::HasComponent } });
    node->resolveBrowsePath(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Values") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("RelativeMachineOnTime") },
                QOpcUa::ReferenceTypeId::HasComponent } });
}
