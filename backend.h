#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QQmlEngine>

#include <QOpcUaClient>
#include <QOpcUaProvider>
#include <QOpcUaErrorState>

#include "opcuamodel.h"

class BackEnd : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    Q_PROPERTY(QString state READ state NOTIFY stateChanged FINAL)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStateChanged FINAL)
    Q_PROPERTY(QVector<QString> serverList READ serverList NOTIFY serverListChanged FINAL)
    Q_PROPERTY(QVector<QString> endpointList READ endpointList NOTIFY endpointListChanged FINAL)
    Q_PROPERTY(QAbstractItemModel *opcUaModel READ opcUaModel NOTIFY opcUaModelChanged FINAL)

    explicit BackEnd(QObject *parent = nullptr);

    bool isConnected() const noexcept;
    QString state() const noexcept;
    QVector<QString> serverList() const noexcept;
    QVector<QString> endpointList() const;
    OpcUaModel *opcUaModel() const noexcept;

    Q_INVOKABLE void clearServerList();
    Q_INVOKABLE void clearEndpointList();

    Q_INVOKABLE void findServers(const QString &urlString);
    Q_INVOKABLE void getEndpoints(int serverIndex);
    Q_INVOKABLE void connectToEndpoint(int endpointIndex);
    Q_INVOKABLE void disconnectFromEndpoint();

signals:
    void serverListChanged();
    void endpointListChanged();
    void stateChanged();
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

    bool mClientConnected = false;
    QString mState;
    QVector<QString> mServerList;
    QList<QOpcUaEndpointDescription> mEndpointList;
    QOpcUaEndpointDescription mCurrentEndpoint;
};

#endif // BACKEND_H
