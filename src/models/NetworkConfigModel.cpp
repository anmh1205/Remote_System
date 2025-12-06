#include "NetworkConfigModel.h"
#include <QNetworkInterface>
#include <QHostAddress>

NetworkConfigModel::NetworkConfigModel(QObject *parent)
    : QObject(parent)
    , m_cameraPort(554)  // Default RTSP port
{
    updateLocalNetworkInfo();
}

void NetworkConfigModel::setLocalIp(const QString &ip)
{
    if (m_localIp != ip) {
        m_localIp = ip;
        emit localIpChanged(ip);
    }
}

void NetworkConfigModel::setSubnetMask(const QString &mask)
{
    if (m_subnetMask != mask) {
        m_subnetMask = mask;
        emit subnetMaskChanged(mask);
    }
}

void NetworkConfigModel::setDefaultGateway(const QString &gateway)
{
    if (m_defaultGateway != gateway) {
        m_defaultGateway = gateway;
        emit defaultGatewayChanged(gateway);
    }
}

void NetworkConfigModel::setDnsServer(const QString &dns)
{
    if (m_dnsServer != dns) {
        m_dnsServer = dns;
        emit dnsServerChanged(dns);
    }
}

void NetworkConfigModel::setCameraIp(const QString &ip)
{
    if (m_cameraIp != ip) {
        m_cameraIp = ip;
        emit cameraIpChanged(ip);
    }
}

void NetworkConfigModel::setCameraPort(quint16 port)
{
    if (m_cameraPort != port) {
        m_cameraPort = port;
        emit cameraPortChanged(port);
    }
}

void NetworkConfigModel::setPanTiltIp(const QString &ip)
{
    if (m_panTiltIp != ip) {
        m_panTiltIp = ip;
        emit panTiltIpChanged(ip);
    }
}

void NetworkConfigModel::setJoystickIp(const QString &ip)
{
    if (m_joystickIp != ip) {
        m_joystickIp = ip;
        emit joystickIpChanged(ip);
    }
}

void NetworkConfigModel::updateLocalNetworkInfo()
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    
    for (const QNetworkInterface &interface : interfaces) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            interface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            
            QList<QNetworkAddressEntry> entries = interface.addressEntries();
            for (const QNetworkAddressEntry &entry : entries) {
                QHostAddress address = entry.ip();
                if (address.protocol() == QAbstractSocket::IPv4Protocol) {
                    setLocalIp(address.toString());
                    setSubnetMask(entry.netmask().toString());
                    break;
                }
            }
        }
    }
}

