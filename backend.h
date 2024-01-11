#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QQmlEngine>
#include <QStringListModel>

#include <QOpcUaClient>
#include <QOpcUaProvider>
#include <QOpcUaErrorState>

#include "opcuamodel.h"

class DashboardItemModel;
class MonitoredItemModel;

class BackEnd : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    Q_PROPERTY(QString stateText READ stateText NOTIFY stateTextChanged FINAL)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStateChanged FINAL)
    Q_PROPERTY(int connectionState READ connectionState NOTIFY connectionStateChanged FINAL)
    Q_PROPERTY(QVector<QString> serverList READ serverList NOTIFY serverListChanged FINAL)
    Q_PROPERTY(QVector<QString> endpointList READ endpointList NOTIFY endpointListChanged FINAL)
    Q_PROPERTY(QAbstractItemModel *opcUaModel READ opcUaModel NOTIFY opcUaModelChanged FINAL)
    Q_PROPERTY(QAbstractItemModel *dashboardItemModel READ dashboardItemModel NOTIFY
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

    explicit BackEnd(QObject *parent = nullptr);
    ~BackEnd();

    bool isConnected() const;
    int connectionState() const;
    QString stateText() const noexcept;
    QVector<QString> serverList() const noexcept;
    QVector<QString> endpointList() const;
    OpcUaModel *opcUaModel() const noexcept;
    QAbstractItemModel *dashboardItemModel() const noexcept;
    QStringListModel *defaultVariableDashboards() const noexcept;
    QStringListModel *defaultEventDashboards() const noexcept;
    QStringListModel *savedVariableDashboards() const noexcept;
    QStringListModel *savedEventDashboards() const noexcept;
    bool hasLastDashboards() const noexcept;

    Q_INVOKABLE void clearServerList();
    Q_INVOKABLE void clearEndpointList();

    Q_INVOKABLE void findServers(const QString &urlString);
    Q_INVOKABLE void getEndpoints(int serverIndex);
    Q_INVOKABLE void connectToEndpoint(int endpointIndex);
    Q_INVOKABLE void connectToEndpointWithPassword(int endpointIndex, const QString &userName,
                                                   const QString &password);
    Q_INVOKABLE void disconnectFromEndpoint();

    Q_INVOKABLE void monitorSelectedNodes();

    Q_INVOKABLE void saveCurrentDashboard(const QString &name);
    Q_INVOKABLE void loadDashboard(const QString &name);

    Q_INVOKABLE void loadLastDashboardsFromSettings();

signals:
    void serverListChanged();
    void endpointListChanged();
    void stateTextChanged();
    void connectionStateChanged();
    void opcUaModelChanged();
    void defaultVariableDashboardsChanged();
    void defaultEventDashboardsChanged();
    void savedVariableDashboardsChanged();
    void savedEventDashboardsChanged();

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

    void monitorNode(MonitoredItemModel *model, const QString &nodeId);

    void connectToEndpoint(int endpointIndex, bool usePassword, const QString &userName = QString(),
                           const QString &password = QString());
    void saveLastDashboards();

    OpcUaModel *mOpcUaModel;
    QOpcUaProvider *mOpcUaProvider;
    QOpcUaClient *mOpcUaClient = nullptr;
    QOpcUaApplicationIdentity mIdentity;
    QOpcUaPkiConfiguration mPkiConfig;

    QString mState;
    QString mServerHost;
    QVector<QString> mServerList;
    QList<QOpcUaEndpointDescription> mEndpointList;
    QOpcUaEndpointDescription mCurrentEndpoint;
    DashboardItemModel *mDashboardItemModel = nullptr;

    QStringListModel *mDefaultVariableDashboardsModel;
    QStringListModel *mDefaultEventDashboardsModel;
    QStringListModel *mSavedVariableDashboardsModel;
    QStringListModel *mSavedEventDashboardsModel;

    bool mHasLastDashboards = false;
};

#endif // BACKEND_H
