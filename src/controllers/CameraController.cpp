#include "CameraController.h"
#include "camera/CameraManager.h"
#include "models/ConnectionStatusModel.h"
#include "models/DeviceConfigModel.h"
#include "models/NetworkConfigModel.h"
#include "views/CameraWidget.h"
#include <QLabel>
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
  m_connectTimer->setInterval(300); // 300ms debounce
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
    connect(m_cameraManager, &CameraManager::logMessage, this,
            &CameraController::logMessage);
  }
}

void CameraController::setCameraWidget(CameraWidget *widget) {
  m_cameraWidget = widget;
  if (m_cameraWidget && m_cameraManager) {
    // Set QVideoWidget for RTSP streams
    QVideoWidget *videoWidget = m_cameraWidget->videoWidget();
    if (videoWidget) {
      m_cameraManager->setVideoOutput(videoWidget);
    }

    // Set QLabel for MJPEG streams
    QLabel *mjpegLabel = m_cameraWidget->mjpegLabel();
    if (mjpegLabel) {
      m_cameraManager->setMjpegOutput(mjpegLabel);
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
    // Auto-connect camera when configuration changes (debounced)
    connect(m_networkConfigModel, &NetworkConfigModel::cameraIpChanged, this,
            &CameraController::scheduleConnectCamera);
    connect(m_networkConfigModel, &NetworkConfigModel::cameraPortChanged, this,
            &CameraController::scheduleConnectCamera);
    connect(m_networkConfigModel, &NetworkConfigModel::cameraProtocolChanged,
            this, &CameraController::scheduleConnectCamera);
    connect(m_networkConfigModel, &NetworkConfigModel::cameraRtspPathChanged,
            this, &CameraController::scheduleConnectCamera);
    connect(m_networkConfigModel, &NetworkConfigModel::cameraUsernameChanged,
            this, &CameraController::scheduleConnectCamera);
    connect(m_networkConfigModel, &NetworkConfigModel::cameraPasswordChanged,
            this, &CameraController::scheduleConnectCamera);
  }
}

QString CameraController::buildStreamUrl() const {
  if (!m_networkConfigModel) {
    return QString();
  }

  QString ip = m_networkConfigModel->cameraIp();
  quint16 port = m_networkConfigModel->cameraPort();
  CameraProtocol protocol = m_networkConfigModel->cameraProtocol();
  QString path = m_networkConfigModel->cameraRtspPath();
  QString username = m_networkConfigModel->cameraUsername();
  QString password = m_networkConfigModel->cameraPassword();

  if (ip.isEmpty() || port == 0) {
    return QString();
  }

  // Build URL with optional authentication
  // Format: [http|rtsp]://[username:password@]IP:PORT[/PATH]
  QString protocolStr = (protocol == CameraProtocol::HTTP) ? "http" : "rtsp";
  QString streamUrl;

  if (!username.isEmpty() && !password.isEmpty()) {
    // URL encode username and password
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
  if (!m_networkConfigModel) {
    emit logMessage("CameraController: NetworkConfigModel is null");
    return;
  }

  QString streamUrl = buildStreamUrl();

  if (streamUrl.isEmpty()) {
    emit logMessage("CameraController: Cannot connect - IP or port not set");
    if (m_connectionStatusModel) {
      m_connectionStatusModel->setCameraState(ConnectionState::Disconnected);
    }
    return;
  }

  CameraProtocol protocol = m_networkConfigModel->cameraProtocol();
  QString username = m_networkConfigModel->cameraUsername();
  QString password = m_networkConfigModel->cameraPassword();

  emit logMessage(QString("CameraController: Connecting to %1").arg(streamUrl));
  emit logMessage(QString("  Protocol: %1")
                      .arg(protocol == CameraProtocol::HTTP ? "HTTP" : "RTSP"));

  bool success = false;

  if (protocol == CameraProtocol::HTTP) {
    success = startHttpStream(streamUrl);
  } else {
    success = startRtspStream(streamUrl);
  }

  if (!success) {
    emit logMessage("CameraController: Failed to start stream");
    if (m_connectionStatusModel) {
      m_connectionStatusModel->setCameraState(ConnectionState::Error);
    }
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

  QString username;
  QString password;

  if (m_networkConfigModel) {
    username = m_networkConfigModel->cameraUsername();
    password = m_networkConfigModel->cameraPassword();
  }

  bool success = m_cameraManager->startHttpStream(url, username, password);
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
  emit logMessage(
      "CameraController: Iriun camera scanning not yet implemented");
}

void CameraController::onStreamStarted() {
  emit logMessage("CameraController: Stream started successfully");

  // Update CameraWidget display based on stream type
  if (m_cameraWidget && m_cameraManager) {
    CameraType type = m_cameraManager->currentCameraType();
    if (type == CameraType::HTTP) {
      m_cameraWidget->showMjpegLabel();
    } else if (type == CameraType::RTSP) {
      m_cameraWidget->showVideoWidget();
    }
  }

  if (m_connectionStatusModel) {
    m_connectionStatusModel->setCameraState(ConnectionState::Connected);
  }
  emit streamStatusChanged(true);
}

void CameraController::onStreamStopped() {
  // Show placeholder when stream stops
  if (m_cameraWidget) {
    m_cameraWidget->showPlaceholder();
  }

  if (m_connectionStatusModel) {
    m_connectionStatusModel->setCameraState(ConnectionState::Disconnected);
  }
  emit streamStatusChanged(false);
}

void CameraController::onStreamError(const QString &error) {
  emit logMessage(QString("CameraController: Stream error: %1").arg(error));
  if (m_connectionStatusModel) {
    m_connectionStatusModel->setCameraState(ConnectionState::Error);
  }
  emit streamError(error);
}

void CameraController::scheduleConnectCamera() {
  // Stop any pending timer
  if (m_connectTimer && m_connectTimer->isActive()) {
    m_connectTimer->stop();
  }
  // Restart timer - this will call connectCamera() after 300ms of no changes
  if (m_connectTimer) {
    m_connectTimer->start();
  }
}
