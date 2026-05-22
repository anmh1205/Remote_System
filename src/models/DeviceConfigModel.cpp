#include "DeviceConfigModel.h"

DeviceConfigModel::DeviceConfigModel(QObject *parent)
    : QObject(parent)
    , m_cameraType(CameraType::None)
    , m_panTiltPort(8080)
    , m_joystickPort(8081)
{
}

void DeviceConfigModel::setCameraType(CameraType type)
{
    if (m_cameraType != type) {
        m_cameraType = type;
        emit cameraTypeChanged(type);
    }
}

void DeviceConfigModel::setRtspUrl(const QString &url)
{
    if (m_rtspUrl != url) {
        m_rtspUrl = url;
        emit rtspUrlChanged(url);
    }
}

void DeviceConfigModel::setHttpUrl(const QString &url)
{
    if (m_httpUrl != url) {
        m_httpUrl = url;
        emit httpUrlChanged(url);
    }
}

void DeviceConfigModel::setIriunDeviceId(const QString &deviceId)
{
    if (m_iriunDeviceId != deviceId) {
        m_iriunDeviceId = deviceId;
        emit iriunDeviceIdChanged(deviceId);
    }
}

void DeviceConfigModel::setPanTiltIp(const QString &ip)
{
    if (m_panTiltIp != ip) {
        m_panTiltIp = ip;
        emit panTiltIpChanged(ip);
    }
}

void DeviceConfigModel::setPanTiltPort(quint16 port)
{
    if (m_panTiltPort != port) {
        m_panTiltPort = port;
        emit panTiltPortChanged(port);
    }
}

void DeviceConfigModel::setJoystickPort(quint16 port)
{
    if (m_joystickPort != port) {
        m_joystickPort = port;
        emit joystickPortChanged(port);
    }
}

