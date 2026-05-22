#include "NetworkUtils.h"
#include <QNetworkInterface>
#include <QHostAddress>
#include <QTimer>
#include <QDebug>

NetworkUtils::NetworkUtils(QObject *parent)
    : QObject(parent)
{
}

bool NetworkUtils::isValidIpAddress(const QString &ip)
{
    QHostAddress address;
    return address.setAddress(ip) && address.protocol() == QAbstractSocket::IPv4Protocol;
}

bool NetworkUtils::isValidPort(quint16 port)
{
    return port > 0 && port <= 65535;
}

QList<QHostAddress> NetworkUtils::getLocalIpAddresses()
{
    QList<QHostAddress> addresses;
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    
    for (const QNetworkInterface &interface : interfaces) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            interface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            
            QList<QNetworkAddressEntry> entries = interface.addressEntries();
            for (const QNetworkAddressEntry &entry : entries) {
                QHostAddress address = entry.ip();
                if (address.protocol() == QAbstractSocket::IPv4Protocol) {
                    addresses.append(address);
                }
            }
        }
    }
    
    return addresses;
}

QString NetworkUtils::getLocalIpAddress()
{
    QList<QHostAddress> addresses = getLocalIpAddresses();
    if (!addresses.isEmpty()) {
        return addresses.first().toString();
    }
    return QString();
}

void NetworkUtils::startIriunDiscovery()
{
    // Placeholder: Iriun camera discovery
    // Will be implemented using UDP broadcast or HTTP discovery
    // For now, just clear the list
    m_discoveredCameras.clear();
}

void NetworkUtils::stopIriunDiscovery()
{
    // Placeholder: Stop discovery
}

QList<IriunCameraInfo> NetworkUtils::discoveredCameras() const
{
    return m_discoveredCameras;
}

void NetworkUtils::onDiscoveryTimeout()
{
    // Placeholder: Handle discovery timeout
}

