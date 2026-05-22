#ifndef CONNECTIONSTATUSMODEL_H
#define CONNECTIONSTATUSMODEL_H

#include <QObject>

enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected,
    Error
};

class ConnectionStatusModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ConnectionState cameraState READ cameraState NOTIFY cameraStateChanged)
    Q_PROPERTY(ConnectionState panTiltState READ panTiltState NOTIFY panTiltStateChanged)
    Q_PROPERTY(ConnectionState joystickState READ joystickState NOTIFY joystickStateChanged)

public:
    explicit ConnectionStatusModel(QObject *parent = nullptr);
    
    ConnectionState cameraState() const { return m_cameraState; }
    ConnectionState panTiltState() const { return m_panTiltState; }
    ConnectionState joystickState() const { return m_joystickState; }
    
    void setCameraState(ConnectionState state);
    void setPanTiltState(ConnectionState state);
    void setJoystickState(ConnectionState state);

signals:
    void cameraStateChanged(ConnectionState state);
    void panTiltStateChanged(ConnectionState state);
    void joystickStateChanged(ConnectionState state);

private:
    ConnectionState m_cameraState;
    ConnectionState m_panTiltState;
    ConnectionState m_joystickState;
};

#endif // CONNECTIONSTATUSMODEL_H

