#include "ConnectionStatusModel.h"

ConnectionStatusModel::ConnectionStatusModel(QObject *parent)
    : QObject(parent)
    , m_cameraState(ConnectionState::Disconnected)
    , m_panTiltState(ConnectionState::Disconnected)
    , m_joystickState(ConnectionState::Disconnected)
{
}

void ConnectionStatusModel::setCameraState(ConnectionState state)
{
    if (m_cameraState != state) {
        m_cameraState = state;
        emit cameraStateChanged(state);
    }
}

void ConnectionStatusModel::setPanTiltState(ConnectionState state)
{
    if (m_panTiltState != state) {
        m_panTiltState = state;
        emit panTiltStateChanged(state);
    }
}

void ConnectionStatusModel::setJoystickState(ConnectionState state)
{
    if (m_joystickState != state) {
        m_joystickState = state;
        emit joystickStateChanged(state);
    }
}

