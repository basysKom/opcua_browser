/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QMutex>
#include <QTimer>

#include "logging.h"
#include "loggingviewmodel.h"

QtMessageHandler additionalHandler = nullptr;
LoggingViewModel *loggingModel = nullptr;

Logging::Logging() { }

void Logging::logMessageHandler(QtMsgType type, const QMessageLogContext &context,
                                const QString &msg)
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    if (loggingModel) {
        const QString logMsg = qFormatLogMessage(type, context, msg);
        // Call function via slot to ensure that the functions are always executed in the GUI thread
        QTimer::singleShot(0, loggingModel, [=]() { loggingModel->addLogMessage(type, logMsg); });
    }

    if (additionalHandler)
        (*additionalHandler)(type, context, msg);
}

void Logging::setAdditionalMessageHandler(const QtMessageHandler &handler)
{
    additionalHandler = handler;
}

void Logging::setLoggingViewModel(LoggingViewModel *model)
{
    loggingModel = model;
}
