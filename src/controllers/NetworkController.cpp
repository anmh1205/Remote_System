#include "NetworkController.h"
#include "network/TcpServer.h"
#include "network/TcpClient.h"
#include "models/DeviceConfigModel.h"
#include "models/ConnectionStatusModel.h"
#include <QTcpSocket>
#include <QDebug>

NetworkController::NetworkController(QObject *parent)
    : QObject(parent)
    , m_joystickServer(nullptr)
    , m_panTiltClient(nullptr)
    , m_deviceConfigModel(nullptr)
    , m_connectionStatusModel(nullptr)
{
    m_joystickServer = new TcpServer(this);
    m_panTiltClient = new TcpClient(this);
    
    setupConnections();
}

NetworkController::~NetworkController()
{
    stopJoystickServer();
    disconnectFromPanTilt();
}

void NetworkController::setupConnections()
{
    connect(m_joystickServer, &TcpServer::dataReceived,
            this, &NetworkController::onJoystickDataReceived);
    connect(m_joystickServer, &TcpServer::clientConnected,
            this, &NetworkController::onJoystickClientConnected);
    connect(m_joystickServer, &TcpServer::clientDisconnected,
            this, &NetworkController::onJoystickClientDisconnected);
    
    connect(m_panTiltClient, &TcpClient::connected,
            this, &NetworkController::onPanTiltConnected);
    connect(m_panTiltClient, &TcpClient::disconnected,
            this, &NetworkController::onPanTiltDisconnected);
    connect(m_panTiltClient, &TcpClient::dataReceived,
            this, &NetworkController::onPanTiltDataReceived);
    connect(m_panTiltClient, &TcpClient::errorOccurred,
            this, &NetworkController::onPanTiltError);
}

void NetworkController::setDeviceConfigModel(DeviceConfigModel *model)
{
    m_deviceConfigModel = model;
}

void NetworkController::setConnectionStatusModel(ConnectionStatusModel *model)
{
    m_connectionStatusModel = model;
}

bool NetworkController::startJoystickServer(quint16 port)
{
    if (!m_joystickServer) {
        return false;
    }
    
    bool success = m_joystickServer->start(port);
    return success;
}

void NetworkController::stopJoystickServer()
{
    if (m_joystickServer) {
        m_joystickServer->stop();
    }
}

bool NetworkController::connectToPanTilt(const QString &ip, quint16 port)
{
    if (!m_panTiltClient) {
        return false;
    }
    
    m_panTiltClient->setAutoReconnect(true);
    m_panTiltClient->setReconnectInterval(3000);
    m_panTiltClient->connectToHost(ip, port);
    
    return true;
}

void NetworkController::disconnectFromPanTilt()
{
    if (m_panTiltClient) {
        m_panTiltClient->disconnectFromHost();
    }
}

void NetworkController::sendPanTiltCommand(const QByteArray &command)
{
    if (m_panTiltClient && m_panTiltClient->isConnected()) {
        m_panTiltClient->sendData(command);
    }
}

void NetworkController::onJoystickDataReceived(QTcpSocket *socket, const QByteArray &data)
{
    Q_UNUSED(socket)
    emit joystickCommandReceived(data);
}

void NetworkController::onJoystickClientConnected(QTcpSocket *socket)
{
    Q_UNUSED(socket)
    if (m_connectionStatusModel) {
        m_connectionStatusModel->setJoystickState(ConnectionState::Connected);
    }
}

void NetworkController::onJoystickClientDisconnected(QTcpSocket *socket)
{
    Q_UNUSED(socket)
    if (m_connectionStatusModel) {
        m_connectionStatusModel->setJoystickState(ConnectionState::Disconnected);
    }
}

void NetworkController::onPanTiltConnected()
{
    if (m_connectionStatusModel) {
        m_connectionStatusModel->setPanTiltState(ConnectionState::Connected);
    }
}

void NetworkController::onPanTiltDisconnected()
{
    if (m_connectionStatusModel) {
        m_connectionStatusModel->setPanTiltState(ConnectionState::Disconnected);
    }
}

void NetworkController::onPanTiltDataReceived(const QByteArray &data)
{
    Q_UNUSED(data)
    // Handle response from pan/tilt unit if needed
}

void NetworkController::onPanTiltError(const QString &error)
{
    Q_UNUSED(error)
    if (m_connectionStatusModel) {
        m_connectionStatusModel->setPanTiltState(ConnectionState::Error);
    }
}

