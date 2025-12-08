#include "NetworkConfigModel.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkInterface>

NetworkConfigModel::NetworkConfigModel(QObject *parent)
    : QObject(parent), m_cameraPort(8080) // Default HTTP port
      ,
      m_cameraProtocol(CameraProtocol::HTTP) // Default to HTTP
      ,
      m_cameraRtspPath("") // No default path
{
  updateLocalNetworkInfo();
}

void NetworkConfigModel::setLocalIp(const QString &ip) {
  if (m_localIp != ip) {
    m_localIp = ip;
    emit localIpChanged(ip);
  }
}

void NetworkConfigModel::setSubnetMask(const QString &mask) {
  if (m_subnetMask != mask) {
    m_subnetMask = mask;
    emit subnetMaskChanged(mask);
  }
}

void NetworkConfigModel::setDefaultGateway(const QString &gateway) {
  if (m_defaultGateway != gateway) {
    m_defaultGateway = gateway;
    emit defaultGatewayChanged(gateway);
  }
}

void NetworkConfigModel::setDnsServer(const QString &dns) {
  if (m_dnsServer != dns) {
    m_dnsServer = dns;
    emit dnsServerChanged(dns);
  }
}

void NetworkConfigModel::setCameraIp(const QString &ip) {
  if (m_cameraIp != ip) {
    m_cameraIp = ip;
    emit cameraIpChanged(ip);
  }
}

void NetworkConfigModel::setCameraPort(quint16 port) {
  if (m_cameraPort != port) {
    m_cameraPort = port;
    emit cameraPortChanged(port);
  }
}

void NetworkConfigModel::setCameraProtocol(int protocol) {
  CameraProtocol newProtocol = static_cast<CameraProtocol>(protocol);
  if (m_cameraProtocol != newProtocol) {
    m_cameraProtocol = newProtocol;
    emit cameraProtocolChanged(protocol);
  }
}

void NetworkConfigModel::setCameraRtspPath(const QString &path) {
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

void NetworkConfigModel::setCameraUsername(const QString &username) {
  if (m_cameraUsername != username) {
    m_cameraUsername = username;
    emit cameraUsernameChanged(username);
  }
}

void NetworkConfigModel::setCameraPassword(const QString &password) {
  if (m_cameraPassword != password) {
    m_cameraPassword = password;
    emit cameraPasswordChanged(password);
  }
}

void NetworkConfigModel::setPanTiltIp(const QString &ip) {
  if (m_panTiltIp != ip) {
    m_panTiltIp = ip;
    emit panTiltIpChanged(ip);
  }
}

void NetworkConfigModel::setJoystickIp(const QString &ip) {
  if (m_joystickIp != ip) {
    m_joystickIp = ip;
    emit joystickIpChanged(ip);
  }
}

void NetworkConfigModel::updateLocalNetworkInfo() {
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

bool NetworkConfigModel::loadFromFile(const QString &filePath) {
  QFile file(filePath);
  if (!file.exists()) {
    qDebug() << "NetworkConfigModel: config file does not exist, skipping load"
             << filePath;
    return false;
  }

  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "NetworkConfigModel: failed to open config file for read"
               << filePath << file.errorString();
    return false;
  }

  QByteArray data = file.readAll();
  file.close();

  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
    qWarning() << "NetworkConfigModel: invalid JSON in config file" << filePath
               << parseError.errorString();
    return false;
  }

  QJsonObject obj = doc.object();

  setLocalIp(obj.value("localIp").toString(m_localIp));
  setSubnetMask(obj.value("subnetMask").toString(m_subnetMask));
  setDefaultGateway(obj.value("defaultGateway").toString(m_defaultGateway));
  setDnsServer(obj.value("dnsServer").toString(m_dnsServer));
  setCameraIp(obj.value("cameraIp").toString(m_cameraIp));
  setCameraPort(
      static_cast<quint16>(obj.value("cameraPort").toInt(m_cameraPort)));
  setCameraProtocol(
      obj.value("cameraProtocol").toInt(static_cast<int>(m_cameraProtocol)));
  setCameraRtspPath(obj.value("cameraRtspPath").toString(m_cameraRtspPath));
  setCameraUsername(obj.value("cameraUsername").toString(m_cameraUsername));
  setCameraPassword(obj.value("cameraPassword").toString(m_cameraPassword));
  setPanTiltIp(obj.value("panTiltIp").toString(m_panTiltIp));
  setJoystickIp(obj.value("joystickIp").toString(m_joystickIp));

  qDebug() << "NetworkConfigModel: loaded config from" << filePath;
  return true;
}

bool NetworkConfigModel::saveToFile(const QString &filePath) const {
  QFileInfo info(filePath);
  QDir dir = info.dir();
  if (!dir.exists()) {
    if (!dir.mkpath(".")) {
      qWarning() << "NetworkConfigModel: failed to create config dir"
                 << dir.absolutePath();
      return false;
    }
  }

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    qWarning() << "NetworkConfigModel: failed to open config file for write"
               << filePath << file.errorString();
    return false;
  }

  QJsonObject obj;
  obj["localIp"] = m_localIp;
  obj["subnetMask"] = m_subnetMask;
  obj["defaultGateway"] = m_defaultGateway;
  obj["dnsServer"] = m_dnsServer;
  obj["cameraIp"] = m_cameraIp;
  obj["cameraPort"] = static_cast<int>(m_cameraPort);
  obj["cameraProtocol"] = static_cast<int>(m_cameraProtocol);
  obj["cameraRtspPath"] = m_cameraRtspPath;
  obj["cameraUsername"] = m_cameraUsername;
  obj["cameraPassword"] = m_cameraPassword;
  obj["panTiltIp"] = m_panTiltIp;
  obj["joystickIp"] = m_joystickIp;

  QJsonDocument doc(obj);
  qint64 bytes = file.write(doc.toJson(QJsonDocument::Indented));
  file.close();

  if (bytes <= 0) {
    qWarning() << "NetworkConfigModel: failed to write config file" << filePath;
    return false;
  }

  qDebug() << "NetworkConfigModel: saved config to" << filePath;
  return true;
}
