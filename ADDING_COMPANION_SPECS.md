<!--
// SPDX-FileCopyrightText: 2024 basysKom GmbH
//
// SPDX-License-Identifier: CC0-1.0
-->

# Companion Specification Specific Dashboards (Developer Guide)

# Introduction

Some OPC UA companion specifications define top-level object types that represent machines or devices.
This OPC UA browser provides interfaces in the code to list the companion specifications offered by the connected server and to find all objects of these top-level types or one of their subtypes.

The top-level object types to look for and the code to assemble dashboards for such an object must be added by a developer as described in the following paragraphs.

# Dashboard Creator Classes

To collect the interesting nodes for a top-level object, a class derived from `CompanionSpecDashboardCreator` must be created that implements `virtual void createDashboardsForObject(const QString &nodeId)`. This is where one or more dashboards for the OPC UA object can be defined and populated with variables to display. The `createDashboardsForObject()` method might just resolve known variables using the browse path or perform a more complicated lookup involving dynamic sub objects.

Once it is sure that there are actually any values that can be displayed on a dashboard, an entry can be added to the list of default variable dashboards:
```
const auto dashboardName =
        QStringLiteral("%1 (Woodworking)").arg(device->name());

// Register the dashboard so it is displayed in the default dashboard combo box
backend()->addDefaultVariableDashboard(dashboardName);

// Add variable to the companion spec variable dashboard
backend()->addNodeIdToCompanionSpecVariableDashboard(
        dashboardName, targets.first().targetId().nodeId());
```

The files `companionspecdashboardcreator.h` and `companionspecdashboardcreator.cpp` contain the reference implementation for the `Woodworking` companion specification and can be used as a blueprint to add support for other companion specifications.

# Registering a Top-Level Type

In order to retrieve objects for it, a top-level type must be added to the `knownCsEntryPoints` list in `BackEnd::extractCompanionSpecs()`. This is done using an instance of the `CompanionSpecEntryPoint` struct:
```
struct CompanionSpecEntryPoint
{
    QOpcUaExpandedNodeId typeId;
    QOpcUaExpandedNodeId parentId;
    std::shared_ptr<CompanionSpecDashboardCreator> dashboardCreator;
};
```
This struct contains the expanded node ids of the top-level type's ObjectType node and the entry point in the address space where the instances of the type are attached. The `dashboardCreator` members refers to an object of a subclass of `CompanionSpecDashboardCreator` as described in the previous paragraph.

The following example registers the `WwMachineType` from the Woodworking companion specification:
```
{ QOpcUaExpandedNodeId(QStringLiteral("http://opcfoundation.org/UA/Woodworking/"),
                       QOpcUa::nodeIdFromInteger(0, 2)),
  // Objects -> Machines
  QOpcUaExpandedNodeId(QStringLiteral("http://opcfoundation.org/UA/Machinery/"),
                       QOpcUa::nodeIdFromInteger(0, 1001)),
  std::make_shared<WoodworkingDashboardCreator>(this) }
```