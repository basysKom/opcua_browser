/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef LOGGING_H
#define LOGGING_H

#include <QMessageLogContext>

class LoggingViewModel;

class Logging
{
public:
    Logging();

    static void logMessageHandler(QtMsgType type, const QMessageLogContext &context,
                                  const QString &msg);

    static void setAdditionalMessageHandler(const QtMessageHandler &handler);

    static void setLoggingViewModel(LoggingViewModel *model);
};

#endif // LOGGING_H
