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

#include "constants.h"
#include "logging.h"

// Register data stream operators to save the event filter's where clauses

#include <QOpcUaSimpleAttributeOperand>
#include <QOpcUaQualifiedName>

QDataStream &operator<<(QDataStream &out, const QOpcUaQualifiedName &n)
{
    out << n.namespaceIndex() << n.name();
    return out;
}

QDataStream &operator>>(QDataStream &in, QOpcUaQualifiedName &n)
{
    quint16 namespaceIndex = 0;
    QString name;
    in >> namespaceIndex;
    in >> name;
    n.setNamespaceIndex(namespaceIndex);
    n.setName(name);
    return in;
}

QDataStream &operator<<(QDataStream &out, const QOpcUaSimpleAttributeOperand &o)
{
    out << o.attributeId() << o.browsePath() << o.indexRange() << o.typeId();
    return out;
}

QDataStream &operator>>(QDataStream &in, QOpcUaSimpleAttributeOperand &o)
{
    QOpcUa::NodeAttribute attr = QOpcUa::NodeAttribute::None;
    QList<QOpcUaQualifiedName> browsePath;
    QString indexRange;
    QString typeId;

    in >> attr;
    in >> browsePath;
    in >> indexRange;
    in >> typeId;
    o.setAttributeId(attr);
    o.setBrowsePath(browsePath);
    o.setIndexRange(indexRange);
    o.setTypeId(typeId);
    return in;
}

int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    qputenv("ANDROID_OPENSSL_SUFFIX", "_3");
#endif

    qRegisterMetaType<QOpcUaQualifiedName>("QOpcUaQualifiedName");
    qRegisterMetaType<QOpcUaSimpleAttributeOperand>("QOpcUaSimpleAttributeOperand");
    qRegisterMetaType<QList<QOpcUaSimpleAttributeOperand>>();
    qRegisterMetaType<QList<QList<QOpcUaSimpleAttributeOperand>>>();

    qSetMessagePattern(Constants::Logging::MessagePattern);
    const auto originalHandler = qInstallMessageHandler(Logging::logMessageHandler);
    Logging::setAdditionalMessageHandler(originalHandler);

    QGuiApplication::setAttribute(Qt::AA_Use96Dpi);
    QGuiApplication app(argc, argv);

    // initialize global settings
    app.setOrganizationName(Constants::App::OrganizationName);
    app.setApplicationName(Constants::App::Name);
    app.setApplicationVersion(Constants::App::Version);

    QQuickStyle::setStyle(Constants::App::Style);

    Q_ASSERT(QFile::exists(Constants::Font::SwanseaBold));
    Q_ASSERT(QFile::exists(Constants::Font::Swansea));
    QFontDatabase::addApplicationFont(Constants::Font::SwanseaBold);
    QFontDatabase::addApplicationFont(Constants::Font::Swansea);

    // Set application font
    QGuiApplication::setFont(QFont(Constants::Font::Application));

    QLoggingCategory::setFilterRules(Constants::Logging::FilterRules);

    QQmlApplicationEngine engine;
    QObject::connect(
            &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
            []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    engine.loadFromModule("OPC_UA_Browser", "Main");

    return app.exec();
}
