#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QQmlEngine>

#include <QOpcUaClient>
#include <QOpcUaProvider>
#include <QOpcUaErrorState>

#include "opcuamodel.h"

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
    Q_PROPERTY(QAbstractItemModel *monitoredItemModel READ monitoredItemModel NOTIFY opcUaModelChanged FINAL)

    explicit BackEnd(QObject *parent = nullptr);

    bool isConnected() const;
    int connectionState() const;
    QString stateText() const noexcept;
    QVector<QString> serverList() const noexcept;
    QVector<QString> endpointList() const;
    OpcUaModel *opcUaModel() const noexcept;
    QAbstractItemModel *monitoredItemModel() const noexcept;

    Q_INVOKABLE void clearServerList();
    Q_INVOKABLE void clearEndpointList();

    Q_INVOKABLE void findServers(const QString &urlString);
    Q_INVOKABLE void getEndpoints(int serverIndex);
    Q_INVOKABLE void connectToEndpoint(int endpointIndex);
    Q_INVOKABLE void disconnectFromEndpoint();

signals:
    void serverListChanged();
    void endpointListChanged();
    void stateTextChanged();
    void connectionStateChanged();
    void opcUaModelChanged();

private slots:
    void findServersComplete(const QList<QOpcUaApplicationDescription> &servers, QOpcUa::UaStatusCode statusCode);
    void getEndpointsComplete(const QList<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode);
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

    OpcUaModel *mOpcUaModel;
    QOpcUaProvider *mOpcUaProvider;
    QOpcUaClient *mOpcUaClient = nullptr;
    QOpcUaApplicationIdentity mIdentity;
    QOpcUaPkiConfiguration mPkiConfig;

    QString mState;
    QVector<QString> mServerList;
    QList<QOpcUaEndpointDescription> mEndpointList;
    QOpcUaEndpointDescription mCurrentEndpoint;
};

#endif // BACKEND_H
