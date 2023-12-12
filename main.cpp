#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "types.h"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QGuiApplication app(argc, argv);

    // initialize global settings
    app.setOrganizationName("basysKom GmbH");
    app.setApplicationName("OPC UA Browser");
    app.setApplicationVersion("0.1.0");

    QQuickStyle::setStyle("Basic");

    QQmlApplicationEngine engine;
    QObject::connect(
            &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
            []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    qmlRegisterUncreatableMetaObject(Types::staticMetaObject, "Types", 1, 0, "DashboardType",
                                     "Error: only enums");
    engine.loadFromModule("OPC_UA_Browser", "Main");

    return app.exec();
}
