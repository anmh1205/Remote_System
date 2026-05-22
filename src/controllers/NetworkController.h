#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <QObject>
#include "network/TcpServer.h"
#include "network/TcpClient.h"
#include "models/DeviceConfigModel.h"
#include "models/ConnectionStatusModel.h"

class NetworkController : public QObject
{
    Q_OBJECT

public:
    explicit NetworkController(QObject *parent = nullptr);
    ~NetworkController();
    
    void setDeviceConfigModel(DeviceConfigModel *model);
    void setConnectionStatusModel(ConnectionStatusModel *model);
    
    bool startJoystickServer(quint16 port);
    void stopJoystickServer();
    
    bool connectToPanTilt(const QString &ip, quint16 port);
    void disconnectFromPanTilt();
    
    void sendPanTiltCommand(const QByteArray &command);

signals:
    void joystickCommandReceived(const QByteArray &command);

private slots:
    void onJoystickDataReceived(QTcpSocket *socket, const QByteArray &data);
    void onJoystickClientConnected(QTcpSocket *socket);
    void onJoystickClientDisconnected(QTcpSocket *socket);
    void onPanTiltConnected();
    void onPanTiltDisconnected();
    void onPanTiltDataReceived(const QByteArray &data);
    void onPanTiltError(const QString &error);

private:
    TcpServer *m_joystickServer;
    TcpClient *m_panTiltClient;
    DeviceConfigModel *m_deviceConfigModel;
    ConnectionStatusModel *m_connectionStatusModel;
    
    void setupConnections();
};

#endif // NETWORKCONTROLLER_H

