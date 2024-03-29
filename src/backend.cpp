/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCryptographicHash>
#include <QDir>
#include <QLoggingCategory>
#include <QSettings>
#include <QStandardPaths>
#include <QStringBuilder>

#include <QOpcUaAuthenticationInformation>

#include "backend.h"
#include "logging.h"
#include "monitoreditemmodel.h"
#include "x509certificate.h"

Q_LOGGING_CATEGORY(backendLog, "opcua_browser.backend");

static QString defaultPkiPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/pki";
}

static QString defaultTrustedCertsPath()
{
    return defaultPkiPath() % "/trusted/certs/";
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
      mCertificateItemModel(new CertificateItemModel(defaultTrustedCertsPath(), this)),
      mLoggingViewModel(new LoggingViewModel(this)),
      mOpcUaModel(new OpcUaModel(this)),
      mOpcUaProvider(new QOpcUaProvider(this)),
      mDashboardItemModel(new DashboardItemModel(this)),
      mDefaultVariableDashboardsModel(new QStringListModel(this)),
      mDefaultEventDashboardsModel(new QStringListModel(this)),
      mSavedVariableDashboardsModel(new QStringListModel(this)),
      mSavedEventDashboardsModel(new QStringListModel(this))
{
    Logging::setLoggingViewModel(mLoggingViewModel);

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

const QString &BackEnd::stateText() const noexcept
{
    return mState;
}

const QVector<QString> &BackEnd::recentConnections() const noexcept
{
    return mLastServerHosts;
}

const QVector<QString> &BackEnd::serverList() const noexcept
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
        if ((index < 0) || (index >= (int)modes.size())) {
            qCWarning(backendLog) << "Invalid security mode";
            index = 0;
        }

        list << QStringLiteral("%1#%2#%3")
                        .arg(endpoint.securityPolicy(), modes[index], endpoint.endpointUrl());
    }
    return list;
}

CertificateItemModel *BackEnd::certificateItemModel() const noexcept
{
    return mCertificateItemModel;
}

LoggingViewFilterModel *BackEnd::loggingViewModel() const noexcept
{
    return mLoggingViewModel->getFilteredModel();
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

BackEnd::MessageType BackEnd::messageType() const noexcept
{
    return mMessageType;
}

const CertificateInfo &BackEnd::certificateInfo() const noexcept
{
    return mCertificateInfo;
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

void BackEnd::connectToEndpoint(int endpointIndex, bool usePassword, const QString &userName,
                                const QString &password)
{
    if ((endpointIndex < 0) || (endpointIndex >= mEndpointList.size())) {
        setState(QStringLiteral("endpoint index out of range"));
        qCCritical(backendLog)
                << QStringLiteral("endpoint index out of range, index: %1, endpoint list size: %2")
                           .arg(endpointIndex)
                           .arg(mEndpointList.size());
        return;
    }

    mConnectionConfiguration.mEndpoint = mEndpointList[endpointIndex];
    mConnectionConfiguration.mUsePassword = usePassword;
    if (usePassword) {
        mConnectionConfiguration.mUsername = userName;
        mConnectionConfiguration.mPassword = password;
    }

    // Automatically add server certificate to the trusted certificates
    const QByteArray ba = mEndpointList[endpointIndex].serverCertificate();
    // Use hash as file name to recognise whether the server certificate is already saved
    const QString hash = QString(QCryptographicHash::hash(ba, QCryptographicHash::Md5).toHex());
    const QString trustedCertsPath = defaultTrustedCertsPath();
    if (QDir().mkpath(trustedCertsPath)) {
        const QString filename = trustedCertsPath + QStringLiteral("%1.der").arg(hash);
        if (!QFile::exists(filename)) {
            const QSslCertificate ssl(ba, QSsl::Der);
            mCertificateInfo.mFilename = filename;
            mCertificateInfo.mServerCertificate = ba;
            mCertificateInfo.mExpiryDate =
                    ssl.expiryDate().toString(QLocale().dateFormat(QLocale::LongFormat));
            mCertificateInfo.mEffectiveDate =
                    ssl.effectiveDate().toString(QLocale().dateFormat(QLocale::LongFormat));
            mCertificateInfo.mIssuerCommonName =
                    ssl.issuerInfo(QSslCertificate::CommonName).join(",");
            mCertificateInfo.mIssuerOrganization =
                    ssl.issuerInfo(QSslCertificate::Organization).join(",");
            mCertificateInfo.mIssuerOrganizationUnit =
                    ssl.issuerInfo(QSslCertificate::OrganizationalUnitName).join(",");
            mCertificateInfo.mIssuerLocality =
                    ssl.issuerInfo(QSslCertificate::LocalityName).join(",");
            mCertificateInfo.mIssuerState =
                    ssl.issuerInfo(QSslCertificate::StateOrProvinceName).join(",");
            mCertificateInfo.mIssuerCountry =
                    ssl.issuerInfo(QSslCertificate::CountryName).join(",");
            mCertificateInfo.mFingerprint = ssl.digest(QCryptographicHash::Sha256).toHex();
            mCertificateInfo.mSerialNumber = QString(ssl.serialNumber()).remove(':');
            emit certificateInfoChanged();

            mMessageType = MessageType::TrustCertificate;
            emit messageTypeChanged();
            return;
        }
    }

    connectToEndpoint();
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
        qCWarning(backendLog)
                << QStringLiteral("cannot monitor node %1, OPC UA client is null").arg(nodeId);
        return;
    }

    const auto node = mOpcUaClient->node(nodeId);
    if (nullptr == node) {
        qCWarning(backendLog)
                << QStringLiteral("cannot monitor node %1, could not get a node from the plugin")
                           .arg(nodeId);
        return;
    }

    model->addItem(node);
}

void BackEnd::connectToEndpoint()
{
    setState(QStringLiteral("connected to client \"%1\"")
                     .arg(mConnectionConfiguration.mEndpoint.securityPolicy()));

    createClient();

    if (mConnectionConfiguration.mUsePassword) {
        QOpcUaAuthenticationInformation authInfo;
        authInfo.setUsernameAuthentication(mConnectionConfiguration.mUsername,
                                           mConnectionConfiguration.mPassword);
        mOpcUaClient->setAuthenticationInformation(authInfo);
    }

    mOpcUaClient->connectToEndpoint(mConnectionConfiguration.mEndpoint);
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
        qCDebug(backendLog) << "Discovering servers on " << url.toString();
    }
}

void BackEnd::findServersComplete(const QList<QOpcUaApplicationDescription> &servers,
                                  QOpcUa::UaStatusCode statusCode)
{
    if (!isSuccessStatus(statusCode)) {
        qCWarning(backendLog) << "servers detection failed " << statusCode;
        setState(QStringLiteral("servers detection failed"));
        return;
    }

    const QString state = QStringLiteral("%1 server(s) detected").arg(servers.size());
    qCDebug(backendLog) << state;
    setState(state);
    saveServerHost(mHostUrl.toString());
    mServerList.clear();
    for (const auto &server : servers) {
        mServerList << server.discoveryUrls();
        qCDebug(backendLog) << server.applicationUri() << server.applicationName()
                            << server.discoveryUrls() << server.productUri();
    }

    emit serverListChanged();
}

void BackEnd::getEndpoints(int serverIndex)
{
    if ((serverIndex < 0) || (serverIndex >= mServerList.size())) {
        setState(QStringLiteral("server index out of range"));
        qCCritical(backendLog)
                << QStringLiteral("server index out of range, index: %1, server list size: %2")
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
    qCDebug(backendLog) << "Request endpoints for " << mServerUrl.toString();
    createClient();
    mOpcUaClient->requestEndpoints(mServerUrl.toString());
}

void BackEnd::getEndpointsComplete(const QList<QOpcUaEndpointDescription> &endpoints,
                                   QOpcUa::UaStatusCode statusCode)
{
    if (!isSuccessStatus(statusCode)) {
        qCWarning(backendLog) << "request of endpoints failed, code:" << statusCode;
        if (mServerUrl != mHostUrl) {
            mMessageType = MessageType::UrlMismatch;
            emit messageTypeChanged();
            return;
        }

        setState(QStringLiteral("request of endpoints failed"));
        return;
    }

    const QString state = QStringLiteral("%1 endpoint(s) received").arg(endpoints.size());
    qCDebug(backendLog) << state;
    setState(state);
    mEndpointList = endpoints;
    for (auto &endpoint : mEndpointList) {
        QUrl url(endpoint.endpointUrl());
        // Correct host address due to missing DNS access
        if (url.host() != mServerUrl.host()) {
            url.setHost(mServerUrl.host());
            endpoint.setEndpointUrl(url.toString());
        }
        qCDebug(backendLog) << endpoint.endpointUrl() << endpoint.securityLevel()
                            << endpoint.securityMode() << endpoint.securityPolicy();

        for (const auto &token : endpoint.userIdentityTokens()) {
            qCDebug(backendLog) << token.tokenType() << token.issuedTokenType()
                                << token.issuerEndpointUrl() << token.policyId()
                                << token.securityPolicy();
        }
    }

    emit endpointListChanged();
}

void BackEnd::clientConnected()
{
    qCDebug(backendLog) << "client connected";
    setState(QStringLiteral("client connected"));

    connect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this,
            &BackEnd::namespacesArrayUpdated);
    mOpcUaClient->updateNamespaceArray();
}

void BackEnd::clientDisconnected()
{
    qCDebug(backendLog) << "client disconnected";
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
        qCWarning(backendLog) << "Failed to retrieve the namespaces array";
        return;
    }

    qCDebug(backendLog) << "namespace array updated" << namespaceArray;
    disconnect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this,
               &BackEnd::namespacesArrayUpdated);

    mOpcUaModel->setOpcUaClient(mOpcUaClient);
}

void BackEnd::clientError(QOpcUaClient::ClientError error)
{
    qCDebug(backendLog) << "client error:" << error;
    setState(QStringLiteral("client error: %1").arg(error));
}

void BackEnd::clientState(QOpcUaClient::ClientState state)
{
    qCDebug(backendLog) << "client state:" << state;
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

    if (mConnectionConfiguration.mEndpoint.endpointUrl() != mHostUrl.toString()) {
        mMessageType = MessageType::EndpointReplacement;
        emit messageTypeChanged();
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
        qCFatal(backendLog, "Could not set up directory %s!", qUtf8Printable(pkiPath));

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
            qCFatal(backendLog, "Could not create directory %s!", qUtf8Printable(dir));
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

void BackEnd::hideMessage()
{
    mMessageType = MessageType::NoMessage;
    emit messageTypeChanged();
}

void BackEnd::useHostUrlForEndpointRequest()
{
    hideMessage();
    requestEndpoints(mHostUrl.toString());
}

void BackEnd::useHostUrlForEndpointConnection()
{
    hideMessage();

    mConnectionConfiguration.mEndpoint.setEndpointUrl(mHostUrl.toString());
    connectToEndpoint();
}

void BackEnd::trustCertificate()
{
    QFile file(mCertificateInfo.mFilename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(mCertificateInfo.mServerCertificate);
        file.close();

        if (mCertificateItemModel) {
            mCertificateItemModel->updateCertificateList();
        }
    } else {
        qCWarning(backendLog) << "cannot save server certificate" << mCertificateInfo.mFilename;
    }

    hideMessage();
    connectToEndpoint();
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
