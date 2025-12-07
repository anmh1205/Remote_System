#include "NetworkConfigModel.h"
#include <QNetworkInterface>
#include <QHostAddress>

NetworkConfigModel::NetworkConfigModel(QObject *parent)
    : QObject(parent)
    , m_cameraPort(8080)  // Default HTTP port
    , m_cameraProtocol(CameraProtocol::HTTP)  // Default to HTTP
    , m_cameraRtspPath("")  // No default path
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

void NetworkConfigModel::setCameraProtocol(int protocol)
{
    CameraProtocol newProtocol = static_cast<CameraProtocol>(protocol);
    if (m_cameraProtocol != newProtocol) {
        m_cameraProtocol = newProtocol;
        emit cameraProtocolChanged(protocol);
    }
}

void NetworkConfigModel::setCameraRtspPath(const QString &path)
{
    // Normalize path: if not empty, ensure it starts with /
    // Empty path should remain empty (for root URL)
    QString normalizedPath = path;
    if (!normalizedPath.isEmpty() && !normalizedPath.startsWith("/")) {
        normalizedPath = "/" + normalizedPath;
    }
    
    if (m_cameraRtspPath != normalizedPath) {
        m_cameraRtspPath = normalizedPath;
        emit cameraRtspPathChanged(normalizedPath);
    }
}

void NetworkConfigModel::setCameraUsername(const QString &username)
{
    if (m_cameraUsername != username) {
        m_cameraUsername = username;
        emit cameraUsernameChanged(username);
    }
}

void NetworkConfigModel::setCameraPassword(const QString &password)
{
    if (m_cameraPassword != password) {
        m_cameraPassword = password;
        emit cameraPasswordChanged(password);
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
    
    QNetworkInterface wifiInterface;
    QNetworkInterface ethernetInterface;
    
    // First pass: Find WiFi and Ethernet interfaces (excluding WSL)
    for (const QNetworkInterface &interface : interfaces) {
        if (!interface.flags().testFlag(QNetworkInterface::IsUp) ||
            !interface.flags().testFlag(QNetworkInterface::IsRunning) ||
            interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            continue;
        }
        
        QString name = interface.humanReadableName().toLower();
        QString description = interface.name().toLower();
        
        // Skip WSL interfaces
        if (name.contains("wsl", Qt::CaseInsensitive) ||
            name.contains("vEthernet", Qt::CaseInsensitive) ||
            description.contains("wsl", Qt::CaseInsensitive) ||
            description.contains("vEthernet", Qt::CaseInsensitive) ||
            description.contains("WSL", Qt::CaseInsensitive)) {
            continue;
        }
        
        // Check if it's a WiFi adapter
        if (name.contains("wi-fi", Qt::CaseInsensitive) ||
            name.contains("wifi", Qt::CaseInsensitive) ||
            name.contains("wireless", Qt::CaseInsensitive) ||
            description.contains("wireless", Qt::CaseInsensitive)) {
            if (wifiInterface.isValid()) {
                // Prefer the first WiFi interface found
                continue;
            }
            wifiInterface = interface;
        }
        // Check if it's an Ethernet adapter
        else if (name.contains("ethernet", Qt::CaseInsensitive) ||
                 name.contains("lan", Qt::CaseInsensitive) ||
                 description.contains("ethernet", Qt::CaseInsensitive)) {
            if (!ethernetInterface.isValid()) {
                ethernetInterface = interface;
            }
        }
    }
    
    // Priority: WiFi > Ethernet (only these two, no fallback)
    QNetworkInterface selectedInterface;
    if (wifiInterface.isValid()) {
        selectedInterface = wifiInterface;
    } else if (ethernetInterface.isValid()) {
        selectedInterface = ethernetInterface;
    }
    
    // Extract IP and subnet mask from selected interface
    if (selectedInterface.isValid()) {
        QList<QNetworkAddressEntry> entries = selectedInterface.addressEntries();
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

