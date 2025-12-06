#include "TcpServer.h"
#include <QDebug>

TcpServer::TcpServer(QObject *parent)
    : QObject(parent)
    , m_server(nullptr)
{
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
}

TcpServer::~TcpServer()
{
    stop();
}

bool TcpServer::start(quint16 port)
{
    if (m_server->isListening()) {
        return true;
    }
    
    if (!m_server->listen(QHostAddress::Any, port)) {
        emit errorOccurred(m_server->errorString());
        return false;
    }
    
    return true;
}

void TcpServer::stop()
{
    if (m_server && m_server->isListening()) {
        m_server->close();
    }
    
    for (QTcpSocket *client : m_clients) {
        client->disconnectFromHost();
        if (client->state() != QAbstractSocket::UnconnectedState) {
            client->waitForDisconnected(1000);
        }
        client->deleteLater();
    }
    m_clients.clear();
}

bool TcpServer::isListening() const
{
    return m_server ? m_server->isListening() : false;
}

quint16 TcpServer::serverPort() const
{
    return m_server ? m_server->serverPort() : 0;
}

void TcpServer::sendToAllClients(const QByteArray &data)
{
    for (QTcpSocket *client : m_clients) {
        if (client->state() == QAbstractSocket::ConnectedState) {
            client->write(data);
        }
    }
}

void TcpServer::onNewConnection()
{
    QTcpSocket *client = m_server->nextPendingConnection();
    if (client) {
        m_clients.append(client);
        connect(client, &QTcpSocket::disconnected, this, &TcpServer::onClientDisconnected);
        connect(client, &QTcpSocket::readyRead, this, &TcpServer::onClientReadyRead);
        emit clientConnected(client);
    }
}

void TcpServer::onClientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        m_clients.removeAll(client);
        emit clientDisconnected(client);
        client->deleteLater();
    }
}

void TcpServer::onClientReadyRead()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        QByteArray data = client->readAll();
        emit dataReceived(client, data);
    }
}

