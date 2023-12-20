#include <QFile>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "types.h"

static constexpr auto FontSwansea = "://font/Swansea.ttf";
static constexpr auto FontSwanseaBold = "://font/SwanseaBold.ttf";

int main(int argc, char *argv[])
{
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

    QQmlApplicationEngine engine;
    QObject::connect(
            &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
            []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    qmlRegisterUncreatableMetaObject(Types::staticMetaObject, "Types", 1, 0, "DashboardType",
                                     "Error: only enums");
    engine.loadFromModule("OPC_UA_Browser", "Main");

    return app.exec();
}
