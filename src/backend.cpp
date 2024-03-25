/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFutureWatcher>
#include <QLoggingCategory>
#include <QSettings>
#include <QStandardPaths>
#include <QStringBuilder>

#include <QOpcUaAuthenticationInformation>

#include "backend.h"
#include "constants.h"
#include "companionspecdashboardcreator.h"
#include "logging.h"
#include "monitoreditemmodel.h"
#include "woodworkingdashboardcreator.h"
#include "x509certificate.h"

Q_LOGGING_CATEGORY(backendLog, "opcua_browser.backend");

QHash<QOpcUaClient *, QPointer<BackEnd>> BackEnd::mBackendMapping = {};

static QString defaultPkiPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
            + QStringLiteral("/pki");
}

static QString defaultTrustedCertsPath()
{
    return defaultPkiPath() % QStringLiteral("/trusted/certs/");
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

static void removeItemFromStringListModel(QStringListModel *model, const QString &name)
{
    QStringList keys = model->stringList();
    if (!keys.contains(name))
        return;

    keys.removeAll(name);
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
    settings.beginGroup(Constants::SettingsKey::DashboardsVariables);
    QStringList keys = settings.childKeys();
    settings.endGroup();
    keys.sort(Qt::CaseInsensitive);
    mSavedVariableDashboardsModel->setStringList(keys);

    settings.beginGroup(Constants::SettingsKey::DashboardsEvents);
    keys = settings.childKeys();
    settings.endGroup();
    keys.sort(Qt::CaseInsensitive);
    mSavedEventDashboardsModel->setStringList(keys);

    const QStringList childGroups = settings.childGroups();
    mHasLastDashboards = childGroups.contains(Constants::SettingsKey::LastDashboards);

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
                        .arg(endpoint.securityPolicy(), QString::fromUtf8(modes[index]),
                             endpoint.endpointUrl());
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

const QVector<CompanionSpecDevice> &BackEnd::companionSpecDevices() const noexcept
{
    return mCompanionSpecDevices;
}

OpcUaModel *BackEnd::getOpcUaModelForNode(QOpcUaNode *node)
{
    if (!node)
        return nullptr;

    const auto entry = mBackendMapping.constFind(node->client());
    if (entry == mBackendMapping.constEnd())
        return nullptr;

    return entry.value() ? entry.value()->mOpcUaModel : nullptr;
}

QOpcUaClient *BackEnd::getOpcUaClient()
{
    return mOpcUaClient;
}

void BackEnd::addDefaultVariableDashboard(const QString &name)
{
    if (!mDefaultVariableDashboardsModel->stringList().contains(name)) {
        auto list = mDefaultVariableDashboardsModel->stringList();
        list << name;
        mDefaultVariableDashboardsModel->setStringList(list);
    }
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
        setState(tr("endpoint index out of range"));
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
    const QString hash =
            QString::fromUtf8(QCryptographicHash::hash(ba, QCryptographicHash::Md5).toHex());
    const QString trustedCertsPath = defaultTrustedCertsPath();
    if (QDir().mkpath(trustedCertsPath)) {
        const QString filename = trustedCertsPath + QStringLiteral("%1.der").arg(hash);
        if (!QFile::exists(filename)) {
            const QSslCertificate ssl(ba, QSsl::Der);
            mCertificateInfo.mFilename = filename;
            mCertificateInfo.mServerCertificate = ba;
            mCertificateInfo.mExpiryDate = ssl.expiryDate();
            mCertificateInfo.mEffectiveDate = ssl.effectiveDate();
            mCertificateInfo.mIssuerCommonName =
                    ssl.issuerInfo(QSslCertificate::CommonName).join(QChar::fromLatin1(','));
            mCertificateInfo.mIssuerOrganization =
                    ssl.issuerInfo(QSslCertificate::Organization).join(QChar::fromLatin1(','));
            mCertificateInfo.mIssuerOrganizationUnit =
                    ssl.issuerInfo(QSslCertificate::OrganizationalUnitName)
                            .join(QChar::fromLatin1(','));
            mCertificateInfo.mIssuerLocality =
                    ssl.issuerInfo(QSslCertificate::LocalityName).join(QChar::fromLatin1(','));
            mCertificateInfo.mIssuerState = ssl.issuerInfo(QSslCertificate::StateOrProvinceName)
                                                    .join(QChar::fromLatin1(','));
            mCertificateInfo.mIssuerCountry =
                    ssl.issuerInfo(QSslCertificate::CountryName).join(QChar::fromLatin1(','));
            mCertificateInfo.mFingerprint =
                    QString::fromUtf8(ssl.digest(QCryptographicHash::Sha256).toHex());
            mCertificateInfo.mSerialNumber =
                    QString::fromUtf8(ssl.serialNumber()).remove(QChar::fromLatin1(':'));
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

void BackEnd::monitorNode(MonitoredItemModel *model, const QString &nodeId,
                          const std::optional<QOpcUaMonitoringParameters::EventFilter> &eventFilter)
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

    if (!eventFilter.has_value())
        model->addItem(node);
    else
        model->addEventItem(node, eventFilter.value());
}

void BackEnd::connectToEndpoint()
{
    setState(tr("connect with security policy \"%1\"")
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

    if (mDashboardItemModel->getCurrentDashboardType() == DashboardItem::DashboardType::Events) {
        if (mSelectedEventSourceNodes.isEmpty())
            return;

        QOpcUaMonitoringParameters::EventFilter filter;

        for (const auto &index : mOpcUaModel->selectedIndices()) {
            const auto item = mOpcUaModel->itemForIndex(index);
            if (!item)
                continue;

            const auto operand = item->calculateBrowsePathToEventType();
            if (!operand.has_value())
                continue;

            filter << operand.value();
        }

        for (const auto &nodeId : mSelectedEventSourceNodes) {
            monitorNode(monitoredItemModel, nodeId, filter);
        }
    } else {
        const QStringList nodeIdList = mOpcUaModel->selectedNodes();
        for (const auto &nodeId : nodeIdList.toList()) {
            monitorNode(monitoredItemModel, nodeId);
        }
    }
}

void BackEnd::cacheSelectedEventSourceNodes()
{
    mSelectedEventSourceNodes = mOpcUaModel->selectedNodes();
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
        settings.setValue(Constants::SettingsKey::DashboardsVariables % QChar::fromLatin1('/')
                                  % name,
                          nodeIds);
        addItemToStringListModel(mSavedVariableDashboardsModel, name);
        break;
    case DashboardItem::DashboardType::Events: {
        settings.setValue(Constants::SettingsKey::DashboardsEvents % QChar::fromLatin1('/') % name,
                          nodeIds);

        QList<QList<QOpcUaSimpleAttributeOperand>> selectClauses;
        const auto eventFilters = monitoredItemModel->eventFilters();
        for (const auto &filter : eventFilters)
            selectClauses.push_back(filter.selectClauses());

        settings.setValue(Constants::SettingsKey::DashboardsEvents % QChar::fromLatin1('/') % name
                                  % QChar::fromLatin1('/') % Constants::SettingsKey::EventFilters,
                          QVariant::fromValue(selectClauses));

        addItemToStringListModel(mSavedEventDashboardsModel, name);
        break;
    }
    default:
        Q_UNREACHABLE();
        break;
    }
}

void BackEnd::removeSavedVariableDashboard(const QString &name)
{
    Q_ASSERT(mDashboardItemModel);

    if (name.isEmpty())
        return;

    QSettings settings;
    settings.remove(Constants::SettingsKey::DashboardsVariables % QChar::fromLatin1('/') % name);
    removeItemFromStringListModel(mSavedVariableDashboardsModel, name);
}

void BackEnd::removeSavedEventDashboard(const QString &name)
{
    Q_ASSERT(mDashboardItemModel);

    if (name.isEmpty())
        return;

    QSettings settings;
    settings.remove(Constants::SettingsKey::DashboardsEvents % QChar::fromLatin1('/') % name);
    settings.remove(Constants::SettingsKey::DashboardsEvents % QChar::fromLatin1('/') % name
                    % QChar::fromLatin1('/') % Constants::SettingsKey::EventFilters);
    removeItemFromStringListModel(mSavedEventDashboardsModel, name);
}

void BackEnd::loadDashboard(const QString &name)
{
    Q_ASSERT(mDashboardItemModel);

    const auto monitoredItemModel = mDashboardItemModel->getCurrentMonitoredItemModel();
    if (monitoredItemModel == nullptr)
        return;

    const QString variableCandidate =
            Constants::SettingsKey::DashboardsVariables % QChar::fromLatin1('/') % name;
    const QString eventCandidate =
            Constants::SettingsKey::DashboardsEvents % QChar::fromLatin1('/') % name;

    QSettings settings;
    QStringList nodeIds = settings.value(variableCandidate).toStringList();
    if (!nodeIds.empty()) {
        for (const auto &nodeId : nodeIds) {
            monitorNode(monitoredItemModel, nodeId);
        }
    } else {
        nodeIds = settings.value(eventCandidate).toStringList();
        if (!nodeIds.isEmpty()) {
            const auto selectClauses = settings.value(eventCandidate % QChar::fromLatin1('/')
                                                      % Constants::SettingsKey::EventFilters)
                                               .value<QList<QList<QOpcUaSimpleAttributeOperand>>>();
            if (!selectClauses.isEmpty()) {
                for (int i = 0; i < nodeIds.size(); ++i) {
                    if (selectClauses.size() - 1 < i || selectClauses.at(i).isEmpty())
                        continue;

                    QOpcUaMonitoringParameters::EventFilter filter;
                    filter.setSelectClauses(selectClauses.at(i));
                    monitorNode(monitoredItemModel, nodeIds.at(i), filter);
                }
            }
        }
    }
}

int BackEnd::instantiateDefaultVariableDashboard(const QString &name)
{
    if (mCompanionSpecVariableDashboards.contains(name)
        && !getNodeIdsForCompanionSpecVariableDashboard(name).isEmpty())
        return instantiateCompanionSpecVariableDashboard(name);

    // Instantiate any other default dashboards here

    return -1;
}

void BackEnd::renameSavedVariableDashboard(const QString &previousName, const QString &newName)
{

    QSettings settings;
    const QString settingsGroupName =
            Constants::SettingsKey::DashboardsVariables % QChar::fromLatin1('/') % previousName;

    if (previousName == newName
        || !mSavedVariableDashboardsModel->stringList().contains(previousName)
        || mSavedVariableDashboardsModel->stringList().contains(newName)
        || !settings.contains(settingsGroupName))
        return;

    const auto nodeIds = settings.value(settingsGroupName).toStringList();

    removeSavedVariableDashboard(previousName);

    settings.setValue(Constants::SettingsKey::DashboardsVariables % QChar::fromLatin1('/')
                              % newName,
                      nodeIds);
    addItemToStringListModel(mSavedVariableDashboardsModel, newName);

    mDashboardItemModel->renameItem(previousName, newName);
}

void BackEnd::renameSavedEventDashboard(const QString &previousName, const QString &newName)
{
    QSettings settings;
    const QString settingsGroupName =
            Constants::SettingsKey::DashboardsEvents % QChar::fromLatin1('/') % previousName;

    if (previousName == newName || !mSavedEventDashboardsModel->stringList().contains(previousName)
        || mSavedEventDashboardsModel->stringList().contains(newName)
        || !settings.contains(settingsGroupName))
        return;

    const auto nodeIds = settings.value(settingsGroupName).toStringList();
    const auto eventFilter = settings.value(settingsGroupName % QChar::fromLatin1('/')
                                            + Constants::SettingsKey::EventFilters);

    removeSavedEventDashboard(previousName);

    settings.setValue(Constants::SettingsKey::DashboardsEvents % QChar::fromLatin1('/') % newName,
                      nodeIds);
    settings.setValue(Constants::SettingsKey::DashboardsEvents % QChar::fromLatin1('/') % newName
                              % QChar::fromLatin1('/') % Constants::SettingsKey::EventFilters,
                      eventFilter);
    addItemToStringListModel(mSavedEventDashboardsModel, newName);

    mDashboardItemModel->renameItem(previousName, newName);
}

bool BackEnd::hasSavedVariableDashboard(const QString &name) const
{
    return mSavedVariableDashboardsModel->stringList().contains(name);
}

bool BackEnd::hasSavedEventDashboard(const QString &name) const
{
    return mSavedEventDashboardsModel->stringList().contains(name);
}

int BackEnd::instantiateCompanionSpecVariableDashboard(const QString &name)
{
    assert(mDashboardItemModel);

    if (!mCompanionSpecVariableDashboards.contains(name)
        || mCompanionSpecVariableDashboards.value(name).isEmpty())
        return -1;

    if (mDashboardItemModel->containsItem(name))
        return mDashboardItemModel->getIndexOfItem(name);

    const auto index = mDashboardItemModel->addItem(DashboardItem::DashboardType::Variables, name);

    for (const auto &id : getNodeIdsForCompanionSpecVariableDashboard(name))
        monitorNode(mDashboardItemModel->getMonitoredItemModel(index), id);

    return index;
}

void BackEnd::findServers(const QString &urlString)
{
    QUrl url(urlString);
    setState(tr("Discovering servers on \"%1\"").arg(urlString));

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
        setState(tr("servers detection failed"));
        return;
    }

    const QString state = tr("%1 server(s) detected").arg(servers.size());
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
        setState(tr("server index out of range"));
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
    setState(tr("Request endpoints for \"%1\"").arg(mServerUrl.toString()));
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

        setState(tr("request of endpoints failed"));
        return;
    }

    const QString state = tr("%1 endpoint(s) received").arg(endpoints.size());
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
    setState(tr("client connected"));

    connect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this,
            &BackEnd::namespacesArrayUpdated);
    mOpcUaClient->updateNamespaceArray();
}

void BackEnd::clientDisconnected()
{
    qCDebug(backendLog) << "client disconnected";
    setState(tr("client disconnected"));

    saveLastDashboards();
    mDashboardItemModel->clearItems();

    mOpcUaClient->deleteLater();
    mBackendMapping.remove(mOpcUaClient);
    mOpcUaClient = nullptr;
    mOpcUaModel->setOpcUaClient(nullptr);
}

void BackEnd::namespacesArrayUpdated(const QStringList &namespaceArray)
{
    if (namespaceArray.isEmpty()) {
        qCWarning(backendLog) << "Failed to retrieve the namespaces array";
        return;
    }

    findCompanionSpecObjects();

    qCDebug(backendLog) << "namespace array updated" << namespaceArray;
    disconnect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this,
               &BackEnd::namespacesArrayUpdated);

    mOpcUaModel->setOpcUaClient(mOpcUaClient);
}

void BackEnd::clientError(QOpcUaClient::ClientError error)
{
    qCDebug(backendLog) << "client error:" << error;
    setState(tr("client error: %1").arg(error));
}

void BackEnd::clientState(QOpcUaClient::ClientState state)
{
    qCDebug(backendLog) << "client state:" << state;
    setState(tr("client state changed: %1").arg(state));
}

void BackEnd::clientConnectError(QOpcUaErrorState *errorState)
{
    const QString statuscode = QOpcUa::statusToString(errorState->errorCode());
    const QString msg = errorState->isClientSideError() ? tr("The client reported: ")
                                                        : tr("The server reported: ");
    setState(tr("Connection Error") % QChar::fromLatin1('\n') % msg
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
        mOpcUaClient = mOpcUaProvider->createClient(QStringLiteral("open62541"));
        if (!mOpcUaClient) {
            const QString message(tr("A possible cause could be that the backend "
                                     "could not be loaded as a plugin."));
            setState(tr("Failed to connect to server") % QChar::fromLatin1('\n') % message);
            return;
        }

        mBackendMapping.insert(mOpcUaClient, this);

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
    const QString certFileName(pkiPath % QStringLiteral("/own/certs/opcuabrowser.der"));
    const QString privateKeyFileName(pkiPath % QStringLiteral("/own/private/opcuabrowser.pem"));

    const bool createCertificate =
            !QFile::exists(certFileName) || !QFile::exists(privateKeyFileName);
    if (createCertificate && !X509Certificate::createCertificate(pkiPath))
        qCFatal(backendLog, "Could not set up directory %s!", qUtf8Printable(pkiPath));

    mPkiConfig.setClientCertificateFile(certFileName);
    mPkiConfig.setPrivateKeyFile(privateKeyFileName);
    mPkiConfig.setTrustListDirectory(pkiPath % QStringLiteral("/trusted/certs"));
    mPkiConfig.setRevocationListDirectory(pkiPath % QStringLiteral("/trusted/crl"));
    mPkiConfig.setIssuerListDirectory(pkiPath % QStringLiteral("/issuers/certs"));
    mPkiConfig.setIssuerRevocationListDirectory(pkiPath % QStringLiteral("/issuers/crl"));

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
    int size = settings.beginReadArray(Constants::SettingsKey::LastDashboards);
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        const QString name = settings.value(Constants::SettingsKey::Name).toString();
        const DashboardItem::DashboardType type = static_cast<DashboardItem::DashboardType>(
                settings.value(Constants::SettingsKey::Type, 0).toInt());

        const int index = mDashboardItemModel->addItem(type, name);
        auto model = mDashboardItemModel->getMonitoredItemModel(index);
        if (model != nullptr) {
            const QStringList nodeIds =
                    settings.value(Constants::SettingsKey::NodeIds).toStringList();
            const auto selectClauses = settings.value(Constants::SettingsKey::EventFilters)
                                               .value<QList<QList<QOpcUaSimpleAttributeOperand>>>();

            for (int j = 0; j < nodeIds.size(); ++j) {
                if (type == DashboardItem::DashboardType::Events) {
                    QOpcUaMonitoringParameters::EventFilter filter;
                    if (selectClauses.size() - 1 < j || selectClauses.at(j).isEmpty())
                        continue;

                    filter.setSelectClauses(selectClauses.at(j));
                    monitorNode(model, nodeIds.at(j), filter);
                } else {
                    monitorNode(model, nodeIds.at(j));
                }
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

void BackEnd::removeRecentConnection(const QString &name)
{
    if (mLastServerHosts.contains(name)) {
        mLastServerHosts.removeAll(name);

        emit recentConnectionsChanged();
        syncRecentConnectionsToSettings();
    }
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
    int size = settings.beginReadArray(Constants::SettingsKey::RecentConnections);
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        mLastServerHosts << settings.value(Constants::SettingsKey::Url).toString();
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
    syncRecentConnectionsToSettings();
}

void BackEnd::syncRecentConnectionsToSettings()
{
    QSettings settings;
    settings.remove(Constants::SettingsKey::RecentConnections);

    settings.beginWriteArray(Constants::SettingsKey::RecentConnections);
    for (qsizetype i = 0; i < qMin(10, mLastServerHosts.count()); ++i) {
        settings.setArrayIndex(i);
        settings.setValue(Constants::SettingsKey::Url, mLastServerHosts.at(i));
    }
    settings.endArray();
}

QFuture<QString> BackEnd::findAllSubtypes(const QString &nodeId,
                                          std::shared_ptr<QSet<QString>> visitedNodes)
{
    if (!visitedNodes)
        visitedNodes = std::make_shared<QSet<QString>>();

    auto promise = std::make_shared<QPromise<QString>>();
    auto future = promise->future();

    if (visitedNodes->contains(nodeId)) {
        promise->finish();
        return future;
    }

    if (nodeId.isEmpty()) {
        promise->finish();
        return future;
    }

    const auto node = mOpcUaClient->node(nodeId);
    if (!node) {
        promise->finish();
        return future;
    }

    promise->addResult(nodeId);
    visitedNodes->insert(nodeId);

    QObject::connect(
            node, &QOpcUaNode::browseFinished, this,
            [this, node, promise, visitedNodes](const QList<QOpcUaReferenceDescription> &children,
                                                QOpcUa::UaStatusCode statusCode) {
                Q_UNUSED(statusCode)

                if (children.empty()) {
                    promise->finish();
                } else {
                    auto childCount = std::make_shared<int>(children.size());

                    for (const auto &child : children) {
                        auto childFuture =
                                findAllSubtypes(child.targetNodeId().nodeId(), visitedNodes);

                        auto watcher = std::make_shared<QFutureWatcher<QString>>();
                        watcher->setFuture(childFuture);
                        QObject::connect(watcher.get(), &QFutureWatcher<QString>::finished, this,
                                         [promise, watcher, childCount]() {
#if QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
                                             const auto results = watcher->future().results();
                                             for (const auto &res : results)
                                                 promise->addResult(res);
#else
                                             promise->addResults(watcher->future().results());
#endif
                                             --*childCount;
                                             if (*childCount == 0)
                                                 promise->finish();
                                         });
                    }
                }

                node->deleteLater();
            });

    node->browseChildren(QOpcUa::ReferenceTypeId::HasSubtype, QOpcUa::NodeClass::DataType);

    return future;
}

CompanionSpecDevice *BackEnd::getCompanionSpecDeviceForNodeId(const QString &nodeId)
{
    const auto entry = std::find_if(
            mCompanionSpecDevices.begin(), mCompanionSpecDevices.end(),
            [nodeId](const CompanionSpecDevice &entry) { return entry.nodeId() == nodeId; });

    if (entry == mCompanionSpecDevices.end())
        return nullptr;

    return &*entry;
}

QStringList BackEnd::getNodeIdsForCompanionSpecVariableDashboard(const QString &name)
{
    return mCompanionSpecVariableDashboards.value(name);
}

void BackEnd::addNodeIdToCompanionSpecVariableDashboard(const QString &name, const QString &nodeId)
{
    mCompanionSpecVariableDashboards[name].push_back(nodeId);
}

void BackEnd::findCompanionSpecObjects()
{
    mCompanionSpecDevices.clear();
    emit companionSpecDevicesChanged();

    if (!mOpcUaClient || mOpcUaClient->namespaceArray().isEmpty())
        return;

    const QList<CompanionSpecEntryPoint> knownCsEntryPoints{
        // WwMachineType
        { QOpcUaExpandedNodeId(Constants::NamespaceUri::Woodworking,
                               QOpcUa::nodeIdFromInteger(0, 2)),
          // Objects -> Machines
          QOpcUaExpandedNodeId(Constants::NamespaceUri::Machinery,
                               QOpcUa::nodeIdFromInteger(0, 1001)),
          std::make_shared<WoodworkingDashboardCreator>(this) },
    };

    for (const auto &entryPoint : knownCsEntryPoints) {
        bool success = false;
        const auto resolvedTypeId =
                mOpcUaClient->resolveExpandedNodeId(entryPoint.typeId, &success);

        if (!success)
            continue;

        const auto resolvedParentId =
                mOpcUaClient->resolveExpandedNodeId(entryPoint.parentId, &success);

        if (!success)
            continue;

        auto watcher = std::make_shared<QFutureWatcher<QString>>();
        watcher->setFuture(findAllSubtypes(resolvedTypeId));
        QObject::connect(
                watcher.get(), &QFutureWatcher<QString>::finished, this,
                [this, resolvedParentId, watcher, entryPoint]() {
                    auto node = mOpcUaClient->node(resolvedParentId);

                    if (!node)
                        return;

                    const auto typeIds = watcher->future().results();
                    const QSet<QString> permittedSubtypes(typeIds.constBegin(), typeIds.constEnd());

                    QObject::connect(
                            node, &QOpcUaNode::browseFinished, this,
                            [this, entryPoint, permittedSubtypes,
                             node](const QList<QOpcUaReferenceDescription> &children,
                                   QOpcUa::UaStatusCode statusCode) {
                                Q_UNUSED(statusCode)
                                node->deleteLater();
                                for (const auto &child : children) {
                                    if (permittedSubtypes.contains(
                                                child.typeDefinition().nodeId())) {
                                        mCompanionSpecDevices.push_back(
                                                { entryPoint.typeId.namespaceUri(),
                                                  child.targetNodeId().nodeId(),
                                                  child.displayName().text().isEmpty()
                                                          ? child.browseName().name()
                                                          : child.displayName().text() });
                                        emit companionSpecDevicesChanged();

                                        if (entryPoint.dashboardCreator)
                                            entryPoint.dashboardCreator->createDashboardsForObject(
                                                    child.targetNodeId().nodeId());
                                    }
                                }
                            });

                    node->browseChildren(QOpcUa::ReferenceTypeId::HierarchicalReferences,
                                         QOpcUa::NodeClass::Object);
                });
    }
}
