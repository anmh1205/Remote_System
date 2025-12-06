#include "TcpClient.h"
#include <QDebug>

TcpClient::TcpClient(QObject *parent)
    : QObject(parent)
    , m_socket(nullptr)
    , m_port(0)
    , m_autoReconnect(false)
    , m_reconnectTimer(nullptr)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
    connect(m_socket, &QAbstractSocket::errorOccurred, this, &TcpClient::onError);
    
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &TcpClient::attemptReconnect);
}

TcpClient::~TcpClient()
{
    disconnectFromHost();
}

void TcpClient::connectToHost(const QString &host, quint16 port)
{
    m_host = host;
    m_port = port;
    
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }
    
    m_socket->connectToHost(host, port);
}

void TcpClient::disconnectFromHost()
{
    m_autoReconnect = false;
    m_reconnectTimer->stop();
    
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }
}

bool TcpClient::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void TcpClient::sendData(const QByteArray &data)
{
    if (isConnected()) {
        m_socket->write(data);
    }
}

void TcpClient::setAutoReconnect(bool enabled)
{
    m_autoReconnect = enabled;
    if (!enabled) {
        m_reconnectTimer->stop();
    }
}

void TcpClient::setReconnectInterval(int msec)
{
    m_reconnectTimer->setInterval(msec);
}

void TcpClient::onConnected()
{
    m_reconnectTimer->stop();
    emit connected();
}

void TcpClient::onDisconnected()
{
    emit disconnected();
    
    if (m_autoReconnect && !m_host.isEmpty() && m_port > 0) {
        m_reconnectTimer->start();
    }
}

void TcpClient::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    emit dataReceived(data);
}

void TcpClient::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    emit errorOccurred(m_socket->errorString());
    
    if (m_autoReconnect && !m_host.isEmpty() && m_port > 0) {
        m_reconnectTimer->start();
    }
}

void TcpClient::attemptReconnect()
{
    if (m_socket->state() == QAbstractSocket::UnconnectedState) {
        m_socket->connectToHost(m_host, m_port);
    }
}

