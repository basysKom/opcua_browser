#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <QStringBuilder>

#include <QOpcUaAuthenticationInformation>

#include "backend.h"
#include "monitoreditemmodel.h"
#include "opcuamodel.h"

static bool copyDirRecursively(const QString &from, const QString &to)
{
    const QDir srcDir(from);
    const QDir targetDir(to);
    if (!QDir().mkpath(to))
        return false;

    const QFileInfoList infos = srcDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo &info : infos) {
        const QString srcItemPath = info.absoluteFilePath();
        const QString dstItemPath = targetDir.absoluteFilePath(info.fileName());
        if (info.isDir()) {
            if (!copyDirRecursively(srcItemPath, dstItemPath))
                return false;
        } else if (info.isFile()) {
            if (!QFile::copy(srcItemPath, dstItemPath))
                return false;
        }
    }
    return true;
}

BackEnd::BackEnd(QObject *parent)
    : QObject{parent}
    , mOpcUaModel(new OpcUaModel(this))
    , mOpcUaProvider(new QOpcUaProvider(this))
    , mMonitoredItemModel(new MonitoredItemModel(this))
{

    //! [Application Identity]
    //m_identity = m_pkiConfig.applicationIdentity();
    //! [Application Identity]

    QSettings settings;
    mStoredMonitoredNodeIds = settings.value("monitoredNodeIds").toStringList();
}

BackEnd::~BackEnd()
{
    QSettings settings;
    if (isConnected()) {
        settings.setValue("monitoredNodeIds", mMonitoredItemModel->getNodeIds());
    } else {
        settings.setValue("monitoredNodeIds", mStoredMonitoredNodeIds);
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

QVector<QString> BackEnd::serverList() const noexcept
{
    return mServerList;
}

QVector<QString> BackEnd::endpointList() const
{
    static const std::array<const char *, 4> modes = {
        "Invalid",
        "None",
        "Sign",
        "SignAndEncrypt"
    };

    QVector<QString> list;
    for (const auto &endpoint : mEndpointList) {
        int index = endpoint.securityMode();
        if ((index < 0) || (index >= modes.size())) {
            qWarning() << "Invalid security mode";
            index = 0;
        }

        list << QStringLiteral("%1 (%2)").arg(endpoint.securityPolicy(), modes[index]);
    }
    return list;
}

OpcUaModel *BackEnd::opcUaModel() const noexcept
{
    return mOpcUaModel;
}

QAbstractItemModel *BackEnd::monitoredItemModel() const noexcept
{
    return mMonitoredItemModel;
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
    if ((endpointIndex < 0) || (endpointIndex >= mEndpointList.size())) {
        setState(QStringLiteral("endpoint index out of range"));
        qCritical() << QStringLiteral("endpoint index out of range, index: %1, endpoint list size: %2").arg(endpointIndex).arg(mEndpointList.size());
        return;
    }

    mCurrentEndpoint = mEndpointList[endpointIndex];
    setState(QStringLiteral("connected to client \"%1\"").arg(mCurrentEndpoint.securityPolicy()));
    createClient();
    mOpcUaClient->connectToEndpoint(mCurrentEndpoint);
}

void BackEnd::disconnectFromEndpoint()
{
    mOpcUaClient->disconnectFromEndpoint();
}

void BackEnd::monitorNode(const QString &nodeId)
{
    Q_ASSERT(mMonitoredItemModel);
    if (mMonitoredItemModel->containsItem(nodeId))
        return;

    if (nullptr == mOpcUaClient) {
        qWarning() << QStringLiteral("cannot monitor node %1, OPC UA client is null").arg(nodeId);
        return;
    }

    const auto node = mOpcUaClient->node(nodeId);
    if (nullptr == node) {
        qWarning() << QStringLiteral("cannot monitor node %1, could not get a node from the plugin").arg(nodeId);
        return;
    }

    mMonitoredItemModel->addItem(node);
}

void BackEnd::findServers(const QString &urlString)
{
    QUrl url(urlString);
    setState(QStringLiteral("Discovering servers on \"%1\"").arg(urlString));

    createClient();
    // set default port if missing
    if (url.port() == -1) url.setPort(4840);

    if (mOpcUaClient) {
        mOpcUaClient->findServers(url);
        qDebug() << "Discovering servers on " << url.toString();
    }

}

void BackEnd::findServersComplete(const QList<QOpcUaApplicationDescription> &servers, QOpcUa::UaStatusCode statusCode)
{
    qDebug() << "findServersComplete " << statusCode;
    if (!isSuccessStatus(statusCode)) {
        setState(QStringLiteral("servers detection failed"));
        return;
    }

    setState(QStringLiteral("%1 server(s) detected").arg(servers.size()));
    mServerList.clear();
    for (const auto &server : servers) {
        mServerList << server.discoveryUrls();
        qDebug() << server.applicationUri() << server.applicationName() << server.discoveryUrls() << server.productUri();
    }

    emit serverListChanged();
}

void BackEnd::getEndpoints(int serverIndex)
{
    if ((serverIndex < 0) || (serverIndex >= mServerList.size())) {
        setState(QStringLiteral("server index out of range"));
        qCritical() << QStringLiteral("server index out of range, index: %1, server list size: %2").arg(serverIndex).arg(mServerList.size());
        return;
    }

    const auto serverUrl = mServerList.at(serverIndex);
    setState(QStringLiteral("Request endpoints for \"%1\"").arg(serverUrl));
    qDebug() << "Request endpoints for " << serverUrl;
    createClient();
    mOpcUaClient->requestEndpoints(serverUrl);
}

void BackEnd::getEndpointsComplete(const QList<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode)
{
    qDebug() << "getEndpointsComplete " << statusCode;
    if (!isSuccessStatus(statusCode)) {
        setState(QStringLiteral("request of endpoints failed"));
        return;
    }

    setState(QStringLiteral("%1 endpoint(s) received").arg(endpoints.size()));
    mEndpointList = endpoints;
    for (const auto &endpoint : endpoints) {
        qDebug() << endpoint.endpointUrl() << endpoint.securityLevel()
                 << endpoint.securityMode() << endpoint.securityPolicy();
    }

    emit endpointListChanged();
}

void BackEnd::clientConnected()
{
    qDebug() << "client connected";
    setState(QStringLiteral("client connected"));

    connect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this, &BackEnd::namespacesArrayUpdated);
    mOpcUaClient->updateNamespaceArray();
    restoreMonitoredNodeIds();
}

void BackEnd::clientDisconnected()
{
    qDebug() << "client disconnected";
    setState(QStringLiteral("client disconnected"));

    mOpcUaClient->deleteLater();
    mOpcUaClient = nullptr;
    mOpcUaModel->setOpcUaClient(nullptr);

    mStoredMonitoredNodeIds = mMonitoredItemModel->getNodeIds();
    mMonitoredItemModel->clearItems();
}

void BackEnd::namespacesArrayUpdated(const QStringList &namespaceArray)
{
    if (namespaceArray.isEmpty()) {
        qWarning() << "Failed to retrieve the namespaces array";
        return;
    }

    qDebug() << "namespace array updated" << namespaceArray;
    disconnect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this, &BackEnd::namespacesArrayUpdated);

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
    const QString msg = errorState->isClientSideError() ? tr("The client reported: ") : tr("The server reported: ");
    setState(tr("Connection Error") % "\n" % msg
             % QStringLiteral("0x%1 (%2)").arg(errorState->errorCode(), 8, 16, QLatin1Char('0')).arg(statuscode));
}

void BackEnd::createClient()
{
    if (mOpcUaClient == nullptr) {
        mOpcUaClient = mOpcUaProvider->createClient("open62541");
        if (!mOpcUaClient) {
            const QString message(tr("A possible cause could be that the backend could not be loaded as a plugin."));
            setState(QStringLiteral("Failed to connect to server") % "\n" % message);
            return;
        }

        connect(mOpcUaClient, &QOpcUaClient::stateChanged, this, &BackEnd::connectionStateChanged);
        connect(mOpcUaClient, &QOpcUaClient::connectError, this, &BackEnd::clientConnectError);
        //mOpcUaClient->setApplicationIdentity(m_identity);
        //mOpcUaClient->setPkiConfiguration(m_pkiConfig);

        //if (mOpcUaClient->supportedUserTokenTypes().contains(QOpcUaUserTokenPolicy::TokenType::Certificate)) {
        //    QOpcUaAuthenticationInformation authInfo;
        //    authInfo.setCertificateAuthentication();
        //    mOpcUaClient->setAuthenticationInformation(authInfo);
        //}

        connect(mOpcUaClient, &QOpcUaClient::connected, this, &BackEnd::clientConnected);
        connect(mOpcUaClient, &QOpcUaClient::disconnected, this, &BackEnd::clientDisconnected);
        connect(mOpcUaClient, &QOpcUaClient::errorChanged, this, &BackEnd::clientError);
        connect(mOpcUaClient, &QOpcUaClient::stateChanged, this, &BackEnd::clientState);
        connect(mOpcUaClient, &QOpcUaClient::endpointsRequestFinished, this, &BackEnd::getEndpointsComplete);
        connect(mOpcUaClient, &QOpcUaClient::findServersFinished, this, &BackEnd::findServersComplete);
    }
}

//! [PKI Configuration]
void BackEnd::setupPkiConfiguration()
{
    const QDir pkidir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/pki");

    if (!pkidir.exists() && !copyDirRecursively(":/pki", pkidir.path()))
        qFatal("Could not set up directory %s!", qUtf8Printable(pkidir.path()));

    mPkiConfig.setClientCertificateFile(pkidir.absoluteFilePath("own/certs/opcuaviewer.der"));
    mPkiConfig.setPrivateKeyFile(pkidir.absoluteFilePath("own/private/opcuaviewer.pem"));
    mPkiConfig.setTrustListDirectory(pkidir.absoluteFilePath("trusted/certs"));
    mPkiConfig.setRevocationListDirectory(pkidir.absoluteFilePath("trusted/crl"));
    mPkiConfig.setIssuerListDirectory(pkidir.absoluteFilePath("issuers/certs"));
    mPkiConfig.setIssuerRevocationListDirectory(pkidir.absoluteFilePath("issuers/crl"));

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

void BackEnd::restoreMonitoredNodeIds()
{
    for (const auto &nodeId : mStoredMonitoredNodeIds) {
        monitorNode(nodeId);
    }
}
