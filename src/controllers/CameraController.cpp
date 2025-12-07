#include "CameraController.h"
#include "camera/CameraManager.h"
#include "models/ConnectionStatusModel.h"
#include "models/DeviceConfigModel.h"
#include "models/NetworkConfigModel.h"
#include "network/NetworkUtils.h"
#include "views/CameraWidget.h"
#include <QDebug>
#include <QTimer>
#include <QUrl>

CameraController::CameraController(QObject *parent)
    : QObject(parent), m_cameraManager(nullptr), m_cameraWidget(nullptr),
      m_deviceConfigModel(nullptr), m_connectionStatusModel(nullptr),
      m_networkConfigModel(nullptr), m_connectTimer(nullptr) {
  m_cameraManager = new CameraManager(this);

  // Create debounce timer for connectCamera calls
  m_connectTimer = new QTimer(this);
  m_connectTimer->setSingleShot(true);
  m_connectTimer->setInterval(500); // 500ms debounce
  connect(m_connectTimer, &QTimer::timeout, this,
          &CameraController::connectCamera);

  setupConnections();
}

CameraController::~CameraController() {}

void CameraController::setupConnections() {
  if (m_cameraManager) {
    connect(m_cameraManager, &CameraManager::streamStarted, this,
            &CameraController::onStreamStarted);
    connect(m_cameraManager, &CameraManager::streamStopped, this,
            &CameraController::onStreamStopped);
    connect(m_cameraManager, &CameraManager::streamError, this,
            &CameraController::onStreamError);
    // Forward log messages from CameraManager to CameraController
    connect(m_cameraManager, &CameraManager::logMessage, this,
            &CameraController::logMessage);
  }
}

void CameraController::setCameraWidget(CameraWidget *widget) {
  m_cameraWidget = widget;
  if (m_cameraWidget && m_cameraManager) {
    // Use QVideoWidget if available, otherwise use videoSink
    QVideoWidget *videoWidget = m_cameraWidget->videoWidget();
    if (videoWidget) {
      m_cameraManager->setVideoOutput(videoWidget);
    } else {
      m_cameraManager->setVideoOutput(m_cameraWidget->videoSink());
    }
  }
}

void CameraController::setVideoWidget(QVideoWidget *videoWidget) {
  if (m_cameraManager && videoWidget) {
    m_cameraManager->setVideoOutput(videoWidget);
  }
}

void CameraController::setDeviceConfigModel(DeviceConfigModel *model) {
  m_deviceConfigModel = model;
}

void CameraController::setConnectionStatusModel(ConnectionStatusModel *model) {
  m_connectionStatusModel = model;
}

void CameraController::setNetworkConfigModel(NetworkConfigModel *model) {
  if (m_networkConfigModel) {
    disconnect(m_networkConfigModel, nullptr, this, nullptr);
  }

  m_networkConfigModel = model;

  if (m_networkConfigModel) {
    // Auto-connect camera when IP, port, protocol, path, username, or password
    // changes
    connect(m_networkConfigModel, &NetworkConfigModel::cameraIpChanged, this,
            &CameraController::connectCamera);
    connect(m_networkConfigModel, &NetworkConfigModel::cameraPortChanged, this,
            &CameraController::connectCamera);
    connect(m_networkConfigModel, &NetworkConfigModel::cameraProtocolChanged,
            this, &CameraController::connectCamera);
    connect(m_networkConfigModel, &NetworkConfigModel::cameraRtspPathChanged,
            this, &CameraController::connectCamera);
    connect(m_networkConfigModel, &NetworkConfigModel::cameraUsernameChanged,
            this, &CameraController::connectCamera);
    connect(m_networkConfigModel, &NetworkConfigModel::cameraPasswordChanged,
            this, &CameraController::connectCamera);
  }
}

QString CameraController::buildStreamUrl() const {
  if (!m_networkConfigModel) {
    qDebug() << "CameraController::buildStreamUrl: NetworkConfigModel is null";
    return QString();
  }

  QString ip = m_networkConfigModel->cameraIp();
  quint16 port = m_networkConfigModel->cameraPort();
  CameraProtocol protocol = m_networkConfigModel->cameraProtocol();
  QString path = m_networkConfigModel->cameraRtspPath();
  QString username = m_networkConfigModel->cameraUsername();
  QString password = m_networkConfigModel->cameraPassword();

  // Note: Cannot emit logMessage here because this is a const method
  // Logging will be done in connectCamera() instead

  if (ip.isEmpty() || port == 0) {
    return QString();
  }

  // Build URL with optional authentication
  // Format: [http|rtsp]://[username:password@]IP:PORT[/PATH]
  QString protocolStr = (protocol == CameraProtocol::HTTP) ? "http" : "rtsp";
  QString streamUrl;

  if (!username.isEmpty() && !password.isEmpty()) {
    // URL encode username and password to handle special characters
    QString encodedUsername = QUrl::toPercentEncoding(username);
    QString encodedPassword = QUrl::toPercentEncoding(password);
    if (path.isEmpty()) {
      streamUrl = QString("%1://%2:%3@%4:%5")
                      .arg(protocolStr, encodedUsername, encodedPassword, ip)
                      .arg(port);
    } else {
      streamUrl = QString("%1://%2:%3@%4:%5%6")
                      .arg(protocolStr, encodedUsername, encodedPassword, ip)
                      .arg(port)
                      .arg(path);
    }
  } else {
    if (path.isEmpty()) {
      streamUrl = QString("%1://%2:%3").arg(protocolStr, ip).arg(port);
    } else {
      streamUrl =
          QString("%1://%2:%3%4").arg(protocolStr, ip).arg(port).arg(path);
    }
  }

  return streamUrl;
}

void CameraController::connectCamera() {
  // Stop any pending debounce timer
  if (m_connectTimer && m_connectTimer->isActive()) {
    m_connectTimer->stop();
  }

  if (!m_networkConfigModel) {
    emit logMessage(
        "CameraController::connectCamera: NetworkConfigModel is null");
    return;
  }

  QString streamUrl = buildStreamUrl();

  if (streamUrl.isEmpty()) {
    emit logMessage(
        "CameraController::connectCamera: Cannot connect - IP or port not set");
    if (m_connectionStatusModel) {
      m_connectionStatusModel->setCameraState(ConnectionState::Disconnected);
    }
    return;
  }

  CameraProtocol protocol = m_networkConfigModel->cameraProtocol();
  bool success = false;

  emit logMessage("CameraController::connectCamera: Attempting to connect");
  emit logMessage(QString("  Protocol: %1")
                      .arg(protocol == CameraProtocol::HTTP ? "HTTP" : "RTSP"));
  emit logMessage(QString("  URL: %1").arg(streamUrl));
  emit logMessage(QString("  IP: %1").arg(m_networkConfigModel->cameraIp()));
  emit logMessage(
      QString("  Port: %1").arg(m_networkConfigModel->cameraPort()));
  emit logMessage(
      QString("  Path: %1").arg(m_networkConfigModel->cameraRtspPath()));
  emit logMessage(QString("  Username: %1")
                      .arg(m_networkConfigModel->cameraUsername().isEmpty()
                               ? "none"
                               : "***"));

  // Log URL building details
  emit logMessage(QString("CameraController::buildStreamUrl: Protocol = %1, IP "
                          "= %2, Port = %3, Path = %4, Username = %5")
                      .arg(protocol == CameraProtocol::HTTP ? "HTTP" : "RTSP")
                      .arg(m_networkConfigModel->cameraIp())
                      .arg(m_networkConfigModel->cameraPort())
                      .arg(m_networkConfigModel->cameraRtspPath())
                      .arg(m_networkConfigModel->cameraUsername().isEmpty()
                               ? "none"
                               : "***"));
  emit logMessage(QString("CameraController::buildStreamUrl: Built URL = %1")
                      .arg(streamUrl));

  if (protocol == CameraProtocol::HTTP) {
    emit logMessage("CameraController::connectCamera: Starting HTTP stream");
    success = startHttpStream(streamUrl);
  } else {
    emit logMessage("CameraController::connectCamera: Starting RTSP stream");
    success = startRtspStream(streamUrl);
  }

  if (!success) {
    emit logMessage("CameraController::connectCamera: Failed to start stream");
    if (m_connectionStatusModel) {
      m_connectionStatusModel->setCameraState(ConnectionState::Error);
    }
  } else {
    emit logMessage("CameraController::connectCamera: Stream start command "
                    "sent successfully");
  }
}

void CameraController::disconnectCamera() { stopStream(); }

bool CameraController::startRtspStream(const QString &url) {
  if (!m_cameraManager) {
    return false;
  }

  bool success = m_cameraManager->startRtspStream(url);
  if (success && m_deviceConfigModel) {
    m_deviceConfigModel->setRtspUrl(url);
    m_deviceConfigModel->setCameraType(CameraType::RTSP);
  }
  return success;
}

bool CameraController::startHttpStream(const QString &url) {
  if (!m_cameraManager) {
    return false;
  }

  bool success = m_cameraManager->startHttpStream(url);
  if (success && m_deviceConfigModel) {
    m_deviceConfigModel->setHttpUrl(url);
    m_deviceConfigModel->setCameraType(CameraType::HTTP);
  }
  return success;
}

bool CameraController::startIriunStream(const QString &deviceId) {
  if (!m_cameraManager) {
    return false;
  }

  bool success = m_cameraManager->startIriunStream(deviceId);
  if (success && m_deviceConfigModel) {
    m_deviceConfigModel->setIriunDeviceId(deviceId);
    m_deviceConfigModel->setCameraType(CameraType::Iriun);
  }
  return success;
}

void CameraController::stopStream() {
  if (m_cameraManager) {
    m_cameraManager->stopStream();
  }

  if (m_deviceConfigModel) {
    m_deviceConfigModel->setCameraType(CameraType::None);
  }
}

void CameraController::scanIriunCameras() {
  // Placeholder: Iriun camera scanning
  // Will use NetworkUtils to discover Iriun cameras
  NetworkUtils *utils = new NetworkUtils(this);
  utils->startIriunDiscovery();
}

void CameraController::onStreamStarted() {
  qDebug() << "CameraController: Stream started successfully";
  if (m_connectionStatusModel) {
    m_connectionStatusModel->setCameraState(ConnectionState::Connected);
  }
  emit streamStatusChanged(true);
}

void CameraController::onStreamStopped() {
  if (m_connectionStatusModel) {
    m_connectionStatusModel->setCameraState(ConnectionState::Disconnected);
  }
  emit streamStatusChanged(false);
}

void CameraController::onStreamError(const QString &error) {
  qDebug() << "CameraController: Stream error:" << error;
  if (m_connectionStatusModel) {
    m_connectionStatusModel->setCameraState(ConnectionState::Error);
  }
  emit streamError(error);
}

void CameraController::scheduleConnectCamera() {
  // Stop any pending timer
  if (m_connectTimer) {
    m_connectTimer->stop();
  }
  // Restart timer - this will call connectCamera() after 500ms of no changes
  if (m_connectTimer) {
    m_connectTimer->start();
  }
}
