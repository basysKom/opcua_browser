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

static constexpr auto FontSwansea = "://font/Swansea.ttf";
static constexpr auto FontSwanseaBold = "://font/SwanseaBold.ttf";

int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    qputenv("ANDROID_OPENSSL_SUFFIX", "_3");
#endif

    QGuiApplication::setAttribute(Qt::AA_Use96Dpi);
    QGuiApplication app(argc, argv);

    // initialize global settings
    app.setOrganizationName("basysKom GmbH");
    app.setApplicationName("OPC UA Browser");
    app.setApplicationVersion("0.1.0");

    QQuickStyle::setStyle("Basic");

    Q_ASSERT(QFile::exists(FontSwanseaBold));
    Q_ASSERT(QFile::exists(FontSwansea));
    QFontDatabase::addApplicationFont(FontSwanseaBold);
    QFontDatabase::addApplicationFont(FontSwansea);

    // Set application font
    QGuiApplication::setFont(QFont("Swansea"));

    QLoggingCategory::setFilterRules(QStringLiteral("qt.qml.binding.removal.info=true"));

    QQmlApplicationEngine engine;
    QObject::connect(
            &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
            []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    engine.loadFromModule("OPC_UA_Browser", "Main");

    return app.exec();
}
