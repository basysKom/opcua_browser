/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "constants.h"

namespace Constants {
const QString Font::Swansea = QStringLiteral("://font/Swansea.ttf");
const QString Font::SwanseaBold = QStringLiteral("://font/SwanseaBold.ttf");
const QString Font::Application = QStringLiteral("Swansea");

const QString App::FallbackLocale = QStringLiteral("en_GB");
const QString App::OrganizationName = QStringLiteral("basysKom GmbH");
const QString App::Name = QStringLiteral("OPC UA Browser");
const QString App::Version = QStringLiteral("0.1.0");
const QString App::Style = QStringLiteral("Basic");

const QString Logging::FilterRules = QStringLiteral("qt.qml.binding.removal.info=true");
const QString Logging::MessagePattern =
        QStringLiteral("%{time hh:mm:ss.zzz}|##|"
                       "%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}"
                       "C%{endif}%{if-fatal}F%{endif}|##|"
                       "%{category}|##|"
                       "%{message}|##|"
                       // "%{function}|##|"
                       "%{line}");

const QString SettingsKey::LastDashboards = QStringLiteral("lastDashboards");
const QString SettingsKey::DashboardsVariables = QStringLiteral("dashboards/variables");
const QString SettingsKey::DashboardsEvents = QStringLiteral("dashboards/events");
const QString SettingsKey::Name = QStringLiteral("name");
const QString SettingsKey::Type = QStringLiteral("type");
const QString SettingsKey::NodeIds = QStringLiteral("nodeIDs");

const QString CertInfo::CommonName = QStringLiteral("OpcUaBrowser");
const QString CertInfo::CountryName = QStringLiteral("DE");
const QString CertInfo::LocalityName = QStringLiteral("Darmstadt");
const QString CertInfo::StateOrProvinceName = QStringLiteral("Hesse");
const QString CertInfo::OrganizationName = QStringLiteral("basysKom GmbH");
const QString CertInfo::AlternativeNameDNS = QStringLiteral("foo.com");
const QString CertInfo::AlternativeNameUri =
        QStringLiteral("urn:foo.com:basysKom%20GmbH:OpcUaBrowser");
} // namespace Constants
