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

    const auto indexCandidate =
            client->namespaceArray().indexOf(Constants::NamespaceUri::Woodworking);

    if (indexCandidate < 0)
        return;

    const quint16 wwIndex = indexCandidate;

    auto node(client->node(nodeId));
    if (!node)
        return;

    const auto refCount = std::make_shared<int>(0);

    QObject::connect(
            node, &QOpcUaNode::resolveBrowsePathFinished, this,
            [this, nodeId, node, refCount, wwIndex](const QList<QOpcUaBrowsePathTarget> &targets,
                                                    const QList<QOpcUaRelativePathElement> &path,
                                                    QOpcUa::UaStatusCode statusCode) mutable {
                Q_UNUSED(path)
                Q_UNUSED(statusCode)

                const auto guard = qScopeGuard([refCount, node]() {
                    --(*refCount);

                    if (*refCount == 0)
                        node->deleteLater();
                });

                if (targets.empty())
                    return;

                if (!targets.first().isFullyResolved())
                    return;

                if (!backend())
                    return;

                const auto device = backend()->getCompanionSpecDeviceForNodeId(nodeId);

                if (device) {
                    if (path.size() == 1) {
                        // This was the Events object child node
                        auto eventsNode = backend()->getOpcUaClient()->node(
                                targets.first().targetId().nodeId());

                        if (!eventsNode)
                            return;

                        QObject::connect(
                                eventsNode, &QOpcUaNode::attributeRead, this,
                                [this, eventsNode, device, wwIndex](QOpcUa::NodeAttributes attr) {
                                    if (attr & QOpcUa::NodeAttribute::EventNotifier
                                        && eventsNode->attribute(
                                                   QOpcUa::NodeAttribute::EventNotifier)
                                                == 1) {

                                        if (device) {
                                            const auto dashboardName =
                                                    QStringLiteral("%1 (WwBaseEventType)")
                                                            .arg(device->name());

                                            // Register the dashboard so it is displayed in the
                                            // default dashboard combo box
                                            backend()->addDefaultEventDashboard(dashboardName);

                                            const auto wwEventTypeId =
                                                    QOpcUa::nodeIdFromInteger(wwIndex, 13);

                                            // Add event monitored item to the companion spec event
                                            // dashboard
                                            const auto filter =
                                                    QOpcUaMonitoringParameters::EventFilter()
                                                    << QOpcUaSimpleAttributeOperand(
                                                               QStringLiteral("Time"), 0)
                                                    << QOpcUaSimpleAttributeOperand(
                                                               QStringLiteral("Message"), 0)
                                                    << QOpcUaSimpleAttributeOperand(
                                                               QStringLiteral("Severity"), 0)
                                                    << QOpcUaSimpleAttributeOperand(
                                                               QStringLiteral("Arguments"), wwIndex,
                                                               wwEventTypeId)
                                                    << QOpcUaSimpleAttributeOperand(
                                                               QStringLiteral("EventCategory"),
                                                               wwIndex, wwEventTypeId)
                                                    << QOpcUaSimpleAttributeOperand(
                                                               QStringLiteral("Group"), wwIndex,
                                                               wwEventTypeId)
                                                    << QOpcUaSimpleAttributeOperand(
                                                               QStringLiteral("LocalizedMessages"),
                                                               wwIndex, wwEventTypeId)
                                                    << QOpcUaSimpleAttributeOperand(
                                                               QStringLiteral("MessageId"), wwIndex,
                                                               wwEventTypeId)
                                                    << QOpcUaSimpleAttributeOperand(
                                                               QStringLiteral("MessageName"),
                                                               wwIndex, wwEventTypeId)
                                                    << QOpcUaSimpleAttributeOperand(
                                                               QStringLiteral("PathParts"), wwIndex,
                                                               wwEventTypeId);
                                            backend()->addObjectToCompanionSpecEventDashboard(
                                                    dashboardName,
                                                    { eventsNode->nodeId(), filter });
                                        }
                                    }

                                    eventsNode->deleteLater();
                                });

                        eventsNode->readAttributes(QOpcUa::NodeAttribute::EventNotifier);
                    } else {
                        const auto dashboardName =
                                QStringLiteral("%1 (Woodworking)").arg(device->name());

                        // Register the dashboard so it is displayed in the default
                        // dashboard combo box
                        backend()->addDefaultVariableDashboard(dashboardName);

                        // Add variable to the companion spec variable dashboard
                        backend()->addNodeIdToCompanionSpecVariableDashboard(
                                dashboardName, targets.first().targetId().nodeId());
                    }
                }
            });

    const auto enqueueResolveRequest = [node,
                                        refCount](const QList<QOpcUaRelativePathElement> &path) {
        const auto success = node->resolveBrowsePath(path);
        if (success)
            ++(*refCount);
    };

    enqueueResolveRequest(
            { { { wwIndex, QStringLiteral("Events") }, QOpcUa::ReferenceTypeId::HasComponent } });

    enqueueResolveRequest(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Overview") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("CurrentMode") },
                QOpcUa::ReferenceTypeId::HasComponent } });

    enqueueResolveRequest(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Overview") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("CurrentState") },
                QOpcUa::ReferenceTypeId::HasComponent } });

    enqueueResolveRequest(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Alarm") }, QOpcUa::ReferenceTypeId::HasComponent } });
    enqueueResolveRequest(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Calibrated") },
                QOpcUa::ReferenceTypeId::HasComponent } });

    enqueueResolveRequest(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Emergency") },
                QOpcUa::ReferenceTypeId::HasComponent } });

    enqueueResolveRequest(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Error") }, QOpcUa::ReferenceTypeId::HasComponent } });

    enqueueResolveRequest(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("MachineInitialized") },
                QOpcUa::ReferenceTypeId::HasComponent } });

    enqueueResolveRequest(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("MachineOn") },
                QOpcUa::ReferenceTypeId::HasComponent } });

    enqueueResolveRequest(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("PowerPresent") },
                QOpcUa::ReferenceTypeId::HasComponent } });

    enqueueResolveRequest(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("RecipeInRun") },
                QOpcUa::ReferenceTypeId::HasComponent } });

    enqueueResolveRequest(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Flags") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Warning") }, QOpcUa::ReferenceTypeId::HasComponent } });

    enqueueResolveRequest(
            { { { wwIndex, QStringLiteral("State") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Machine") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("Values") }, QOpcUa::ReferenceTypeId::HasComponent },
              { { wwIndex, QStringLiteral("RelativeMachineOnTime") },
                QOpcUa::ReferenceTypeId::HasComponent } });

    if (!refCount)
        node->deleteLater();
}
