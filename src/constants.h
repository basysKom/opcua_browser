/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace Constants {
class Font
{
public:
    const static QString Swansea;
    const static QString SwanseaBold;
    const static QString Application;
};

class App
{
public:
    const static QString FallbackLocale;
    const static QString OrganizationName;
    const static QString Name;
    const static QString Version;
    const static QString Style;
};

class Logging
{
public:
    const static QString FilterRules;
    const static QString MessagePattern;
};

class SettingsKey
{
public:
    const static QString LastDashboards;
    const static QString DashboardsVariables;
    const static QString DashboardsEvents;
    const static QString Name;
    const static QString Type;
    const static QString NodeIds;
};

class CertInfo
{
public:
    const static QString CommonName;
    const static QString CountryName;
    const static QString LocalityName;
    const static QString StateOrProvinceName;
    const static QString OrganizationName;
    const static QString AlternativeNameDNS;
    const static QString AlternativeNameUri;
};
} // namespace Constants

#endif // CONSTANTS_H
