/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QFile>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "logging.h"

static const auto FontSwansea = QStringLiteral("://font/Swansea.ttf");
static const auto FontSwanseaBold = QStringLiteral("://font/SwanseaBold.ttf");

int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    qputenv("ANDROID_OPENSSL_SUFFIX", "_3");
#endif

    qSetMessagePattern(QStringLiteral(
            "%{time hh:mm:ss.zzz}|##|"
            "%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}"
            "C%{endif}%{if-fatal}F%{endif}|##|"
            "%{category}|##|"
            "%{message}|##|"
            "%{function}|##|"
            "%{line}"));
    const auto originalHandler = qInstallMessageHandler(Logging::logMessageHandler);
    Logging::setAdditionalMessageHandler(originalHandler);

    QGuiApplication::setAttribute(Qt::AA_Use96Dpi);
    QGuiApplication app(argc, argv);

    // initialize global settings
    app.setOrganizationName(QStringLiteral("basysKom GmbH"));
    app.setApplicationName(QStringLiteral("OPC UA Browser"));
    app.setApplicationVersion(QStringLiteral("0.1.0"));

    QQuickStyle::setStyle(QStringLiteral("Basic"));

    Q_ASSERT(QFile::exists(FontSwanseaBold));
    Q_ASSERT(QFile::exists(FontSwansea));
    QFontDatabase::addApplicationFont(FontSwanseaBold);
    QFontDatabase::addApplicationFont(FontSwansea);

    // Set application font
    QGuiApplication::setFont(QFont(QStringLiteral("Swansea")));

    QLoggingCategory::setFilterRules(QStringLiteral("qt.qml.binding.removal.info=true"));

    QQmlApplicationEngine engine;
    QObject::connect(
            &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
            []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    engine.loadFromModule("OPC_UA_Browser", "Main");

    return app.exec();
}
