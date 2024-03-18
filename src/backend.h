/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef BACKEND_H
#define BACKEND_H

#include <QFuture>
#include <QObject>
#include <QQmlEngine>
#include <QStringListModel>

#include <QOpcUaClient>
#include <QOpcUaProvider>
#include <QOpcUaErrorState>

#include "certificateitemmodel.h"
#include "dashboarditemmodel.h"
#include "loggingviewfiltermodel.h"
#include "loggingviewmodel.h"
#include "opcuamodel.h"

class CompanionSpecDashboardCreator;
class MonitoredItemModel;

struct CertificateInfo
{
    Q_GADGET
    QML_ANONYMOUS

    Q_PROPERTY(QString commonName MEMBER mIssuerCommonName)
    Q_PROPERTY(QString organization MEMBER mIssuerOrganization)
    Q_PROPERTY(QString organizationUnit MEMBER mIssuerOrganizationUnit)
    Q_PROPERTY(QString locality MEMBER mIssuerLocality)
    Q_PROPERTY(QString state MEMBER mIssuerState)
    Q_PROPERTY(QString country MEMBER mIssuerCountry)
    Q_PROPERTY(QDateTime expiryDate MEMBER mExpiryDate)
    Q_PROPERTY(QDateTime effectiveDate MEMBER mEffectiveDate)
    Q_PROPERTY(QString fingerprint MEMBER mFingerprint)
    Q_PROPERTY(QString serialNumber MEMBER mSerialNumber)

public:
    QString mFilename;
    QByteArray mServerCertificate;
    QString mIssuerCommonName;
    QString mIssuerOrganization;
    QString mIssuerOrganizationUnit;
    QString mIssuerLocality;
    QString mIssuerState;
    QString mIssuerCountry;
    QDateTime mExpiryDate;
    QDateTime mEffectiveDate;
    QString mSerialNumber;
    QString mFingerprint;
};

struct CompanionSpecDevice
{
    Q_GADGET
    QML_ANONYMOUS

    Q_PROPERTY(QString companionSpecUri READ companionSpecUri)
    Q_PROPERTY(QString dashboardName READ dashboardName)
    Q_PROPERTY(QString nodeId READ nodeId)
    Q_PROPERTY(QString name READ name)

public:
    CompanionSpecDevice(const QString &companionSpecUri, const QString &nodeId, const QString &name)
        : mCompanionSpecUri(companionSpecUri), mNodeId(nodeId), mName(name)
    {
    }

    QString companionSpecUri() const { return mCompanionSpecUri; }
    QString nodeId() const { return mNodeId; }
    QString name() const { return mName; }
    QString dashboardName() const { return mDashboardName.isEmpty() ? mName : mDashboardName; }
    void setDashboardName(const QString &dashboardName) { mDashboardName = dashboardName; }

private:
    QString mCompanionSpecUri;
    QString mNodeId;
    QString mName;
    QString mDashboardName;
};

// Workaround, otherwise qmllint doesn't recognise the QStringListModel
struct ThisIsAnnoying
{
    Q_GADGET
    QML_FOREIGN(QStringListModel)
    QML_ANONYMOUS
};

struct CompanionSpecEntryPoint
{
    QOpcUaExpandedNodeId typeId;
    QOpcUaExpandedNodeId parentId;
    std::shared_ptr<CompanionSpecDashboardCreator> dashboardCreator;
};

class BackEnd : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    enum class MessageType { NoMessage, UrlMismatch, EndpointReplacement, TrustCertificate };
    Q_ENUM(MessageType)

    Q_PROPERTY(QString stateText READ stateText NOTIFY stateTextChanged FINAL)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStateChanged FINAL)
    Q_PROPERTY(MessageType messageType READ messageType NOTIFY messageTypeChanged FINAL)
    Q_PROPERTY(CertificateInfo certificateInfo READ certificateInfo NOTIFY certificateInfoChanged
                       FINAL)
    Q_PROPERTY(int connectionState READ connectionState NOTIFY connectionStateChanged FINAL)
    Q_PROPERTY(QVector<QString> recentConnections READ recentConnections NOTIFY
                       recentConnectionsChanged FINAL)
    Q_PROPERTY(QVector<QString> serverList READ serverList NOTIFY serverListChanged FINAL)
    Q_PROPERTY(QVector<QString> endpointList READ endpointList NOTIFY endpointListChanged FINAL)
    Q_PROPERTY(CertificateItemModel *certificateItemModel READ certificateItemModel NOTIFY
                       certificateItemModelChanged FINAL)
    Q_PROPERTY(LoggingViewFilterModel *loggingViewModel READ loggingViewModel NOTIFY
                       loggingViewModelChanged FINAL)
    Q_PROPERTY(OpcUaModel *opcUaModel READ opcUaModel NOTIFY opcUaModelChanged FINAL)
    Q_PROPERTY(DashboardItemModel *dashboardItemModel READ dashboardItemModel NOTIFY
                       opcUaModelChanged FINAL)
    Q_PROPERTY(QStringListModel *defaultVariableDashboards READ defaultVariableDashboards NOTIFY
                       defaultVariableDashboardsChanged FINAL)
    Q_PROPERTY(QStringListModel *defaultEventDashboards READ defaultEventDashboards NOTIFY
                       defaultEventDashboardsChanged FINAL)
    Q_PROPERTY(QStringListModel *savedVariableDashboards READ savedVariableDashboards NOTIFY
                       savedVariableDashboardsChanged FINAL)
    Q_PROPERTY(QStringListModel *savedEventDashboards READ savedEventDashboards NOTIFY
                       savedEventDashboardsChanged FINAL)
    Q_PROPERTY(bool hasLastDashboards READ hasLastDashboards CONSTANT FINAL)

    Q_PROPERTY(QVector<CompanionSpecDevice> companionSpecDevices READ companionSpecDevices NOTIFY
                       companionSpecDevicesChanged FINAL)

    explicit BackEnd(QObject *parent = nullptr);
    ~BackEnd();

    bool isConnected() const;
    int connectionState() const;
    const QString &stateText() const noexcept;
    const QVector<QString> &recentConnections() const noexcept;
    const QVector<QString> &serverList() const noexcept;
    QVector<QString> endpointList() const;
    CertificateItemModel *certificateItemModel() const noexcept;
    LoggingViewFilterModel *loggingViewModel() const noexcept;
    OpcUaModel *opcUaModel() const noexcept;
    DashboardItemModel *dashboardItemModel() const noexcept;
    QStringListModel *defaultVariableDashboards() const noexcept;
    QStringListModel *defaultEventDashboards() const noexcept;
    QStringListModel *savedVariableDashboards() const noexcept;
    QStringListModel *savedEventDashboards() const noexcept;
    bool hasLastDashboards() const noexcept;

    MessageType messageType() const noexcept;
    const CertificateInfo &certificateInfo() const noexcept;

    const QVector<CompanionSpecDevice> &companionSpecDevices() const noexcept;
    CompanionSpecDevice *getCompanionSpecDeviceForNodeId(const QString &nodeId);
    QStringList getNodeIdsForCompanionSpecVariableDashboard(const QString &name);
    void addNodeIdToCompanionSpecVariableDashboard(const QString &name, const QString &nodeId);
    int instantiateCompanionSpecVariableDashboard(const QString &name);

    static OpcUaModel *getOpcUaModelForNode(QOpcUaNode *node);
    QOpcUaClient *getOpcUaClient();

    void addDefaultVariableDashboard(const QString &name);

    Q_INVOKABLE void clearServerList();
    Q_INVOKABLE void clearEndpointList();

    Q_INVOKABLE void findServers(const QString &urlString);
    Q_INVOKABLE void getEndpoints(int serverIndex);
    Q_INVOKABLE void connectToEndpoint(int endpointIndex, bool usePassword = false,
                                       const QString &userName = QString(),
                                       const QString &password = QString());
    Q_INVOKABLE void disconnectFromEndpoint();

    Q_INVOKABLE void monitorSelectedNodes();
    Q_INVOKABLE void cacheSelectedEventSourceNodes();

    Q_INVOKABLE void saveCurrentDashboard(const QString &name);
    Q_INVOKABLE void removeSavedVariableDashboard(const QString &name);
    Q_INVOKABLE void removeSavedEventDashboard(const QString &name);
    Q_INVOKABLE void loadDashboard(const QString &name);
    Q_INVOKABLE int instantiateDefaultVariableDashboard(const QString &name);
    Q_INVOKABLE void renameSavedVariableDashboard(const QString &previousName,
                                                  const QString &newName);
    Q_INVOKABLE void renameSavedEventDashboard(const QString &previousName, const QString &newName);
    Q_INVOKABLE bool hasSavedVariableDashboard(const QString &name) const;
    Q_INVOKABLE bool hasSavedEventDashboard(const QString &name) const;

    Q_INVOKABLE void loadLastDashboardsFromSettings();

    Q_INVOKABLE void applicationSuspended();

    Q_INVOKABLE void hideMessage();
    Q_INVOKABLE void useHostUrlForEndpointRequest();
    Q_INVOKABLE void useHostUrlForEndpointConnection();
    Q_INVOKABLE void trustCertificate();

    Q_INVOKABLE void removeRecentConnection(const QString &name);

signals:
    void recentConnectionsChanged();
    void serverListChanged();
    void endpointListChanged();
    void stateTextChanged();
    void connectionStateChanged();
    void certificateItemModelChanged();
    void loggingViewModelChanged();
    void opcUaModelChanged();
    void defaultVariableDashboardsChanged();
    void defaultEventDashboardsChanged();
    void savedVariableDashboardsChanged();
    void savedEventDashboardsChanged();

    void messageTypeChanged();
    void certificateInfoChanged();

    void companionSpecDevicesChanged();

private slots:
    void findServersComplete(const QList<QOpcUaApplicationDescription> &servers,
                             QOpcUa::UaStatusCode statusCode);
    void getEndpointsComplete(const QList<QOpcUaEndpointDescription> &endpoints,
                              QOpcUa::UaStatusCode statusCode);
    void clientConnected();
    void clientDisconnected();
    void namespacesArrayUpdated(const QStringList &namespaceArray);
    void clientError(QOpcUaClient::ClientError error);
    void clientState(QOpcUaClient::ClientState state);
    void clientConnectError(QOpcUaErrorState *errorState);

private:
    void createClient();
    void setupPkiConfiguration();
    void setState(const QString &state);

    void
    monitorNode(MonitoredItemModel *model, const QString &nodeId,
                const std::optional<QOpcUaMonitoringParameters::EventFilter> &eventFilter = {});

    void requestEndpoints(const QString &serverUrl);
    void connectToEndpoint(const QOpcUaEndpointDescription &endpoint, bool usePassword,
                           const QString &userName = QString(),
                           const QString &password = QString());
    void connectToEndpoint();
    void saveLastDashboards();
    void loadLastServerHostsFromSettings();
    void saveServerHost(const QString &host);
    void syncRecentConnectionsToSettings();

    void findCompanionSpecObjects();
    QFuture<QString> findAllSubtypes(const QString &nodeId,
                                     std::shared_ptr<QSet<QString>> visitedNodes = nullptr);

    CertificateItemModel *mCertificateItemModel;
    LoggingViewModel *mLoggingViewModel;

    OpcUaModel *mOpcUaModel;
    QOpcUaProvider *mOpcUaProvider;
    QOpcUaClient *mOpcUaClient = nullptr;
    QOpcUaApplicationIdentity mIdentity;
    QOpcUaPkiConfiguration mPkiConfig;

    QString mState;
    QUrl mHostUrl;
    QUrl mServerUrl;
    QVector<QString> mServerList;
    QList<QOpcUaEndpointDescription> mEndpointList;
    DashboardItemModel *mDashboardItemModel = nullptr;

    QVector<QString> mLastServerHosts;
    QStringListModel *mDefaultVariableDashboardsModel;
    QStringListModel *mDefaultEventDashboardsModel;
    QStringListModel *mSavedVariableDashboardsModel;
    QStringListModel *mSavedEventDashboardsModel;

    bool mHasLastDashboards = false;
    MessageType mMessageType = MessageType::NoMessage;
    CertificateInfo mCertificateInfo;

    struct
    {
        QOpcUaEndpointDescription mEndpoint;
        bool mUsePassword = false;
        QString mUsername;
        QString mPassword;
    } mConnectionConfiguration;

    static QHash<QOpcUaClient *, QPointer<BackEnd>> mBackendMapping;

    QVector<CompanionSpecDevice> mCompanionSpecDevices;
    QHash<QString, QStringList> mCompanionSpecVariableDashboards;

    QStringList mSelectedEventSourceNodes;
};

#endif // BACKEND_H
