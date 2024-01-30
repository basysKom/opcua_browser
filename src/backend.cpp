/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCryptographicHash>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <QStringBuilder>

#include <QOpcUaAuthenticationInformation>

#include "backend.h"
#include "monitoreditemmodel.h"
#include "x509certificate.h"

static QString defaultPkiPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/pki";
}

static void addItemToStringListModel(QStringListModel *model, const QString &name)
{
    QStringList keys = model->stringList();
    if (keys.contains(name))
        return;

    keys << name;
    keys.sort(Qt::CaseInsensitive);
    model->setStringList(keys);
}

BackEnd::BackEnd(QObject *parent)
    : QObject{ parent },
      mOpcUaModel(new OpcUaModel(this)),
      mOpcUaProvider(new QOpcUaProvider(this)),
      mDashboardItemModel(new DashboardItemModel(this)),
      mDefaultVariableDashboardsModel(new QStringListModel(this)),
      mDefaultEventDashboardsModel(new QStringListModel(this)),
      mSavedVariableDashboardsModel(new QStringListModel(this)),
      mSavedEventDashboardsModel(new QStringListModel(this))
{
    setupPkiConfiguration();

    //! [Application Identity]
    mIdentity = mPkiConfig.applicationIdentity();
    //! [Application Identity]

    QSettings settings;
    settings.beginGroup("dashboards/variables");
    QStringList keys = settings.childKeys();
    settings.endGroup();
    keys.sort(Qt::CaseInsensitive);
    mSavedVariableDashboardsModel->setStringList(keys);

    settings.beginGroup("dashboards/events");
    keys = settings.childKeys();
    settings.endGroup();
    keys.sort(Qt::CaseInsensitive);
    mSavedEventDashboardsModel->setStringList(keys);

    const QStringList childGroups = settings.childGroups();
    mHasLastDashboards = childGroups.contains("lastDashboards");

    loadLastServerHostsFromSettings();
}

BackEnd::~BackEnd()
{
    if (isConnected()) {
        saveLastDashboards();
    }
}

bool BackEnd::isConnected() const
{
    return (QOpcUaClient::Connected == connectionState());
}

int BackEnd::connectionState() const
{
    if (nullptr == mOpcUaClient)
        return QOpcUaClient::Disconnected;

    return mOpcUaClient->state();
}

QString BackEnd::stateText() const noexcept
{
    return mState;
}

QVector<QString> BackEnd::recentConnections() const noexcept
{
    return mLastServerHosts;
}

QVector<QString> BackEnd::serverList() const noexcept
{
    return mServerList;
}

QVector<QString> BackEnd::endpointList() const
{
    static const std::array<const char *, 4> modes = { "Invalid", "None", "Sign",
                                                       "SignAndEncrypt" };

    QVector<QString> list;
    for (const auto &endpoint : mEndpointList) {
        int index = endpoint.securityMode();
        if ((index < 0) || (index >= modes.size())) {
            qWarning() << "Invalid security mode";
            index = 0;
        }

        list << QStringLiteral("%1#%2#%3")
                        .arg(endpoint.securityPolicy(), modes[index], endpoint.endpointUrl());
    }
    return list;
}

OpcUaModel *BackEnd::opcUaModel() const noexcept
{
    return mOpcUaModel;
}

DashboardItemModel *BackEnd::dashboardItemModel() const noexcept
{
    return mDashboardItemModel;
}

QStringListModel *BackEnd::defaultVariableDashboards() const noexcept
{
    return mDefaultVariableDashboardsModel;
}

QStringListModel *BackEnd::defaultEventDashboards() const noexcept
{
    return mDefaultEventDashboardsModel;
}

QStringListModel *BackEnd::savedVariableDashboards() const noexcept
{
    return mSavedVariableDashboardsModel;
}

QStringListModel *BackEnd::savedEventDashboards() const noexcept
{
    return mSavedEventDashboardsModel;
}

bool BackEnd::hasLastDashboards() const noexcept
{
    return mHasLastDashboards;
}

bool BackEnd::showUrlMismatchMessage() const noexcept
{
    return mShowUrlMismatchMessage;
}

bool BackEnd::showEndpointReplacementMessage() const noexcept
{
    return mShowEndpointReplacementMessage;
}

void BackEnd::clearServerList()
{
    mServerList.clear();
    emit serverListChanged();
}

void BackEnd::clearEndpointList()
{
    mEndpointList.clear();
    emit endpointListChanged();
}

void BackEnd::connectToEndpoint(int endpointIndex)
{
    connectToEndpoint(endpointIndex, false);
}

void BackEnd::connectToEndpointWithPassword(int endpointIndex, const QString &userName,
                                            const QString &password)
{
    connectToEndpoint(endpointIndex, true, userName, password);
}

void BackEnd::disconnectFromEndpoint()
{
    mOpcUaClient->disconnectFromEndpoint();
}

void BackEnd::monitorNode(MonitoredItemModel *model, const QString &nodeId)
{
    if ((model == nullptr) || model->containsItem(nodeId))
        return;

    if (nullptr == mOpcUaClient) {
        qWarning() << QStringLiteral("cannot monitor node %1, OPC UA client is null").arg(nodeId);
        return;
    }

    const auto node = mOpcUaClient->node(nodeId);
    if (nullptr == node) {
        qWarning() << QStringLiteral("cannot monitor node %1, could not get a node from the plugin")
                              .arg(nodeId);
        return;
    }

    model->addItem(node);
}

void BackEnd::connectToEndpoint(int endpointIndex, bool usePassword, const QString &userName,
                                const QString &password)
{
    if ((endpointIndex < 0) || (endpointIndex >= mEndpointList.size())) {
        setState(QStringLiteral("endpoint index out of range"));
        qCritical() << QStringLiteral(
                               "endpoint index out of range, index: %1, endpoint list size: %2")
                               .arg(endpointIndex)
                               .arg(mEndpointList.size());
        return;
    }

    connectToEndpoint(mEndpointList[endpointIndex], usePassword, userName, password);
}

void BackEnd::connectToEndpoint(const QOpcUaEndpointDescription &endpoint, bool usePassword,
                                const QString &userName, const QString &password)
{
    mCurrentEndpoint = endpoint;
    setState(QStringLiteral("connected to client \"%1\"").arg(mCurrentEndpoint.securityPolicy()));

    // Automatically add server certificate to the trusted certificates
    const QByteArray ba = mCurrentEndpoint.serverCertificate();
    // Use hash as file name to recognise whether the server certificate is already saved
    const QString hash = QString(QCryptographicHash::hash(ba, QCryptographicHash::Md5).toHex());
    const QString trustedCertsPath = defaultPkiPath() + "/trusted/certs/";
    if (QDir().mkpath(trustedCertsPath)) {
        const QString filename = trustedCertsPath + QStringLiteral("%1.der").arg(hash);
        if (!QFile::exists(filename)) {
            QFile file(filename);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(ba);
                file.close();
            }
        }
    }

    createClient();

    if (usePassword) {
        QOpcUaAuthenticationInformation authInfo;
        authInfo.setUsernameAuthentication(userName, password);
        mOpcUaClient->setAuthenticationInformation(authInfo);
    }

    mOpcUaClient->connectToEndpoint(mCurrentEndpoint);
}

void BackEnd::monitorSelectedNodes()
{
    Q_ASSERT(mDashboardItemModel);

    const auto monitoredItemModel = mDashboardItemModel->getCurrentMonitoredItemModel();
    if (monitoredItemModel == nullptr)
        return;

    const QStringList nodeIdList = mOpcUaModel->selectedNodes();
    for (const auto &nodeId : nodeIdList.toList()) {
        monitorNode(monitoredItemModel, nodeId);
    }
}

void BackEnd::saveCurrentDashboard(const QString &name)
{
    Q_ASSERT(mDashboardItemModel);

    if (name.isEmpty())
        return;

    const auto monitoredItemModel = mDashboardItemModel->getCurrentMonitoredItemModel();
    if (monitoredItemModel == nullptr)
        return;

    const QStringList nodeIds = monitoredItemModel->getNodeIds();
    if (nodeIds.isEmpty())
        return;

    mDashboardItemModel->setCurrentDashboardName(name);

    QSettings settings;
    switch (mDashboardItemModel->getCurrentDashboardType()) {
    case DashboardItem::DashboardType::Variables:
        settings.setValue("dashboards/variables/" % name, nodeIds);
        addItemToStringListModel(mSavedVariableDashboardsModel, name);
        break;
    case DashboardItem::DashboardType::Events:
        settings.setValue("dashboards/events/" % name, nodeIds);
        addItemToStringListModel(mSavedEventDashboardsModel, name);
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
}

void BackEnd::loadDashboard(const QString &name)
{
    Q_ASSERT(mDashboardItemModel);

    const auto monitoredItemModel = mDashboardItemModel->getCurrentMonitoredItemModel();
    if (monitoredItemModel == nullptr)
        return;

    QSettings settings;
    const QStringList nodeIds = settings.value("dashboards/variables/" % name).toStringList();
    for (const auto &nodeId : nodeIds) {
        monitorNode(monitoredItemModel, nodeId);
    }
}

void BackEnd::findServers(const QString &urlString)
{
    QUrl url(urlString);
    setState(QStringLiteral("Discovering servers on \"%1\"").arg(urlString));

    createClient();
    // set default port if missing
    if (url.port() == -1)
        url.setPort(4840);

    if (mOpcUaClient) {
        mHostUrl = url;
        mOpcUaClient->findServers(url);
        qDebug() << "Discovering servers on " << url.toString();
    }
}

void BackEnd::findServersComplete(const QList<QOpcUaApplicationDescription> &servers,
                                  QOpcUa::UaStatusCode statusCode)
{
    qDebug() << "findServersComplete " << statusCode;
    if (!isSuccessStatus(statusCode)) {
        setState(QStringLiteral("servers detection failed"));
        return;
    }

    setState(QStringLiteral("%1 server(s) detected").arg(servers.size()));
    saveServerHost(mHostUrl.toString());
    mServerList.clear();
    for (const auto &server : servers) {
        mServerList << server.discoveryUrls();
        qDebug() << server.applicationUri() << server.applicationName() << server.discoveryUrls()
                 << server.productUri();
    }

    emit serverListChanged();
}

void BackEnd::getEndpoints(int serverIndex)
{
    if ((serverIndex < 0) || (serverIndex >= mServerList.size())) {
        setState(QStringLiteral("server index out of range"));
        qCritical() << QStringLiteral("server index out of range, index: %1, server list size: %2")
                               .arg(serverIndex)
                               .arg(mServerList.size());
        return;
    }

    requestEndpoints(mServerList.at(serverIndex));
}

void BackEnd::requestEndpoints(const QString &serverUrl)
{
    mServerUrl = serverUrl;
    setState(QStringLiteral("Request endpoints for \"%1\"").arg(mServerUrl.toString()));
    qDebug() << "Request endpoints for " << mServerUrl.toString();
    createClient();
    mOpcUaClient->requestEndpoints(mServerUrl.toString());
}

void BackEnd::getEndpointsComplete(const QList<QOpcUaEndpointDescription> &endpoints,
                                   QOpcUa::UaStatusCode statusCode)
{
    qDebug() << "getEndpointsComplete " << statusCode;
    if (!isSuccessStatus(statusCode)) {
        if (mServerUrl != mHostUrl) {
            mShowUrlMismatchMessage = true;
            emit showUrlMismatchMessageChanged();
            return;
        }

        setState(QStringLiteral("request of endpoints failed"));
        return;
    }

    setState(QStringLiteral("%1 endpoint(s) received").arg(endpoints.size()));
    mEndpointList = endpoints;
    for (auto &endpoint : mEndpointList) {
        QUrl url(endpoint.endpointUrl());
        // Correct host address due to missing DNS access
        if (url.host() != mServerUrl.host()) {
            url.setHost(mServerUrl.host());
            endpoint.setEndpointUrl(url.toString());
        }
        qDebug() << endpoint.endpointUrl() << endpoint.securityLevel() << endpoint.securityMode()
                 << endpoint.securityPolicy();
    }

    emit endpointListChanged();
}

void BackEnd::clientConnected()
{
    qDebug() << "client connected";
    setState(QStringLiteral("client connected"));

    connect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this,
            &BackEnd::namespacesArrayUpdated);
    mOpcUaClient->updateNamespaceArray();
}

void BackEnd::clientDisconnected()
{
    qDebug() << "client disconnected";
    setState(QStringLiteral("client disconnected"));

    saveLastDashboards();
    mDashboardItemModel->clearItems();

    mOpcUaClient->deleteLater();
    mOpcUaClient = nullptr;
    mOpcUaModel->setOpcUaClient(nullptr);
}

void BackEnd::namespacesArrayUpdated(const QStringList &namespaceArray)
{
    if (namespaceArray.isEmpty()) {
        qWarning() << "Failed to retrieve the namespaces array";
        return;
    }

    qDebug() << "namespace array updated" << namespaceArray;
    disconnect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this,
               &BackEnd::namespacesArrayUpdated);

    mOpcUaModel->setOpcUaClient(mOpcUaClient);
}

void BackEnd::clientError(QOpcUaClient::ClientError error)
{
    qDebug() << "client error:" << error;
    setState(QStringLiteral("client error: %1").arg(error));
}

void BackEnd::clientState(QOpcUaClient::ClientState state)
{
    qDebug() << "client state:" << state;
    setState(QStringLiteral("client state changed: %1").arg(state));
}

void BackEnd::clientConnectError(QOpcUaErrorState *errorState)
{
    const QString statuscode = QOpcUa::statusToString(errorState->errorCode());
    const QString msg = errorState->isClientSideError() ? tr("The client reported: ")
                                                        : tr("The server reported: ");
    setState(tr("Connection Error") % "\n" % msg
             % QStringLiteral("0x%1 (%2)")
                       .arg(errorState->errorCode(), 8, 16, QLatin1Char('0'))
                       .arg(statuscode));

    if (mCurrentEndpoint.endpointUrl() != mHostUrl.toString()) {
        mShowEndpointReplacementMessage = true;
        emit showEndpointReplacementMessageChanged();
        return;
    }
}

void BackEnd::createClient()
{
    if (mOpcUaClient == nullptr) {
        mOpcUaClient = mOpcUaProvider->createClient("open62541");
        if (!mOpcUaClient) {
            const QString message(tr("A possible cause could be that the backend "
                                     "could not be loaded as a plugin."));
            setState(QStringLiteral("Failed to connect to server") % "\n" % message);
            return;
        }

        connect(mOpcUaClient, &QOpcUaClient::stateChanged, this, &BackEnd::connectionStateChanged);
        connect(mOpcUaClient, &QOpcUaClient::connectError, this, &BackEnd::clientConnectError);

        mOpcUaClient->setApplicationIdentity(mIdentity);
        mOpcUaClient->setPkiConfiguration(mPkiConfig);

        if (mOpcUaClient->supportedUserTokenTypes().contains(
                    QOpcUaUserTokenPolicy::TokenType::Certificate)) {
            QOpcUaAuthenticationInformation authInfo;
            authInfo.setCertificateAuthentication();
            mOpcUaClient->setAuthenticationInformation(authInfo);
        }

        connect(mOpcUaClient, &QOpcUaClient::connected, this, &BackEnd::clientConnected);
        connect(mOpcUaClient, &QOpcUaClient::disconnected, this, &BackEnd::clientDisconnected);
        connect(mOpcUaClient, &QOpcUaClient::errorChanged, this, &BackEnd::clientError);
        connect(mOpcUaClient, &QOpcUaClient::stateChanged, this, &BackEnd::clientState);
        connect(mOpcUaClient, &QOpcUaClient::endpointsRequestFinished, this,
                &BackEnd::getEndpointsComplete);
        connect(mOpcUaClient, &QOpcUaClient::findServersFinished, this,
                &BackEnd::findServersComplete);
    }
}

//! [PKI Configuration]
void BackEnd::setupPkiConfiguration()
{
    const QString pkiPath = defaultPkiPath();
    const QString certFileName(pkiPath % "/own/certs/opcuabrowser.der");
    const QString privateKeyFileName(pkiPath % "/own/private/opcuabrowser.pem");

    const bool createCertificate =
            !QFile::exists(certFileName) || !QFile::exists(privateKeyFileName);
    if (createCertificate && !X509Certificate::createCertificate(pkiPath))
        qFatal("****** Could not set up directory %s!", qUtf8Printable(pkiPath));

    mPkiConfig.setClientCertificateFile(certFileName);
    mPkiConfig.setPrivateKeyFile(privateKeyFileName);
    mPkiConfig.setTrustListDirectory(pkiPath % "/trusted/certs");
    mPkiConfig.setRevocationListDirectory(pkiPath % "/trusted/crl");
    mPkiConfig.setIssuerListDirectory(pkiPath % "/issuers/certs");
    mPkiConfig.setIssuerRevocationListDirectory(pkiPath % "/issuers/crl");

    const QStringList toCreate = { mPkiConfig.issuerListDirectory(),
                                   mPkiConfig.issuerRevocationListDirectory() };
    for (const QString &dir : toCreate) {
        if (!QDir().mkpath(dir))
            qFatal("Could not create directory %s!", qUtf8Printable(dir));
    }
}
//! [PKI Configuration]

void BackEnd::setState(const QString &state)
{
    if (state != mState) {
        mState = state;
        emit stateTextChanged();
    }
}

void BackEnd::loadLastDashboardsFromSettings()
{
    Q_ASSERT(mDashboardItemModel);

    QSettings settings;
    int size = settings.beginReadArray("lastDashboards");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        const QString name = settings.value("name").toString();
        const DashboardItem::DashboardType type =
                static_cast<DashboardItem::DashboardType>(settings.value("type", 0).toInt());

        const int index = mDashboardItemModel->addItem(type, name);
        auto model = mDashboardItemModel->getMonitoredItemModel(index);
        if (model != nullptr) {
            const QStringList nodeIds = settings.value("nodeIDs").toStringList();
            for (const auto &nodeId : nodeIds) {
                monitorNode(model, nodeId);
            }
        }
    }
    settings.endArray();
}

void BackEnd::applicationSuspended()
{
    if (isConnected()) {
        saveLastDashboards();
    }
}

void BackEnd::useHostUrlForEndpointRequest()
{
    hideUrlMismatchMessage();
    requestEndpoints(mHostUrl.toString());
}

void BackEnd::hideUrlMismatchMessage()
{
    mShowUrlMismatchMessage = false;
    emit showUrlMismatchMessageChanged();
}

void BackEnd::useHostUrlForEndpointConnection()
{
    hideEndpointReplacementMessage();

    mCurrentEndpoint.setEndpointUrl(mHostUrl.toString());
    connectToEndpoint(mCurrentEndpoint, false);
}

void BackEnd::useHostUrlForEndpointConnectionWithPassword(const QString &userName,
                                                          const QString &password)
{
    hideEndpointReplacementMessage();

    mCurrentEndpoint.setEndpointUrl(mHostUrl.toString());
    connectToEndpoint(mCurrentEndpoint, true, userName, password);
}

void BackEnd::hideEndpointReplacementMessage()
{
    mShowEndpointReplacementMessage = false;
    emit showEndpointReplacementMessageChanged();
}

void BackEnd::saveLastDashboards()
{
    Q_ASSERT(mDashboardItemModel);
    mDashboardItemModel->saveDashboardsToSettings();
}

void BackEnd::loadLastServerHostsFromSettings()
{
    mLastServerHosts.clear();

    QSettings settings;
    int size = settings.beginReadArray("recentConnections");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        mLastServerHosts << settings.value("url").toString();
    }
    settings.endArray();
}

void BackEnd::saveServerHost(const QString &host)
{
    if (mLastServerHosts.contains(host)) {
        if (mLastServerHosts.first() == host)
            return;

        mLastServerHosts.removeAll(host);
    }

    mLastServerHosts.prepend(host);
    emit recentConnectionsChanged();

    QSettings settings;
    settings.remove("recentConnections");

    settings.beginWriteArray("recentConnections");
    for (qsizetype i = 0; i < qMin(10, mLastServerHosts.count()); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("url", mLastServerHosts[i]);
    }
    settings.endArray();
}
