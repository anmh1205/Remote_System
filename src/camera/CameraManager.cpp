#include "CameraManager.h"
#include <QDebug>
#include <QUrl>

CameraManager::CameraManager(QObject *parent)
    : QObject(parent), m_mediaPlayer(nullptr), m_videoWidget(nullptr),
      m_videoSink(nullptr), m_networkManager(nullptr),
      m_currentCameraType(CameraType::None) {
  m_networkManager = new QNetworkAccessManager(this);
  setupMediaPlayer();
}

CameraManager::~CameraManager() {
  stopStream();
  cleanupMediaPlayer();
}

void CameraManager::setupMediaPlayer() {
  m_mediaPlayer = new QMediaPlayer(this);
  connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this,
          &CameraManager::onMediaPlayerStateChanged);
  connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, this,
          &CameraManager::onMediaPlayerError);
}

void CameraManager::cleanupMediaPlayer() {
  if (m_mediaPlayer) {
    m_mediaPlayer->stop();
    m_mediaPlayer->deleteLater();
    m_mediaPlayer = nullptr;
  }
}

void CameraManager::setVideoOutput(QVideoWidget *videoWidget) {
  if (m_mediaPlayer && videoWidget) {
    m_mediaPlayer->setVideoOutput(videoWidget);
    m_videoSink = videoWidget->videoSink();
  }
}

void CameraManager::setVideoOutput(QVideoSink *videoSink) {
  m_videoSink = videoSink;
  if (m_mediaPlayer && m_videoSink) {
    m_mediaPlayer->setVideoOutput(m_videoSink);
  }
}

bool CameraManager::startRtspStream(const QString &url) {
  emit logMessage(
      QString("CameraManager::startRtspStream: Attempting to connect to %1")
          .arg(url));

  stopStream();

  if (!m_mediaPlayer) {
    setupMediaPlayer();
  }

  if (!m_mediaPlayer) {
    emit logMessage(
        "CameraManager::startRtspStream: Failed to create media player");
    emit streamError("Failed to initialize media player");
    return false;
  }

  QUrl streamUrl(url);
  if (!streamUrl.isValid()) {
    emit logMessage(
        QString("CameraManager::startRtspStream: Invalid URL: %1").arg(url));
    emit streamError(QString("Invalid RTSP URL: %1").arg(url));
    return false;
  }

  emit logMessage(
      QString("CameraManager::startRtspStream: Setting source to %1")
          .arg(streamUrl.toString()));

  // Set the source
  m_mediaPlayer->setSource(streamUrl);

  // Check if video output is set
  if (!m_videoWidget && !m_videoSink) {
    emit logMessage(
        "CameraManager::startRtspStream: Warning - No video output set");
  }

  // Start playing
  m_mediaPlayer->play();

  m_currentCameraType = CameraType::RTSP;
  m_currentStreamUrl = url;

  emit logMessage("CameraManager::startRtspStream: Play command sent, waiting "
                  "for state change...");

  return true;
}

bool CameraManager::startHttpStream(const QString &url) {
  emit logMessage(
      QString("CameraManager::startHttpStream: Attempting to connect to %1")
          .arg(url));

  stopStream();

  if (!m_mediaPlayer) {
    setupMediaPlayer();
  }

  if (!m_mediaPlayer) {
    emit logMessage(
        "CameraManager::startHttpStream: Failed to create media player");
    emit streamError("Failed to initialize media player");
    return false;
  }

  QUrl streamUrl(url);
  if (!streamUrl.isValid()) {
    emit logMessage(
        QString("CameraManager::startHttpStream: Invalid URL: %1").arg(url));
    emit streamError(QString("Invalid HTTP URL: %1").arg(url));
    return false;
  }

  emit logMessage(
      QString("CameraManager::startHttpStream: Setting source to %1")
          .arg(streamUrl.toString()));

  // Set the source
  m_mediaPlayer->setSource(streamUrl);

  // Check if video output is set
  if (!m_videoWidget && !m_videoSink) {
    emit logMessage(
        "CameraManager::startHttpStream: Warning - No video output set");
  }

  // Start playing
  m_mediaPlayer->play();

  m_currentCameraType = CameraType::HTTP;
  m_currentStreamUrl = url;

  emit logMessage("CameraManager::startHttpStream: Play command sent, waiting "
                  "for state change...");

  return true;
}

bool CameraManager::startIriunStream(const QString &deviceId) {
  stopStream();

  // Placeholder: Iriun stream implementation
  // Will connect to Iriun camera via discovered IP/port
  m_currentCameraType = CameraType::Iriun;
  m_currentStreamUrl = deviceId;

  emit streamError("Iriun stream not yet implemented");
  return false;
}

void CameraManager::stopStream() {
  if (m_mediaPlayer) {
    m_mediaPlayer->stop();
  }

  m_currentCameraType = CameraType::None;
  m_currentStreamUrl.clear();
  emit streamStopped();
}

bool CameraManager::isStreaming() const {
  if (m_mediaPlayer) {
    return m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState;
  }
  return false;
}

CameraType CameraManager::currentCameraType() const {
  return m_currentCameraType;
}

void CameraManager::onMediaPlayerStateChanged(
    QMediaPlayer::PlaybackState state) {
  emit logMessage(QString("CameraManager::onMediaPlayerStateChanged: State = "
                          "%1 (0=Stopped, 1=Playing, 2=Paused)")
                      .arg(state));
  emit logMessage(QString("  Current URL: %1").arg(m_currentStreamUrl));
  emit logMessage(QString("  Camera type: %1")
                      .arg(m_currentCameraType == CameraType::HTTP   ? "HTTP"
                           : m_currentCameraType == CameraType::RTSP ? "RTSP"
                                                                     : "None"));

  if (state == QMediaPlayer::PlayingState) {
    emit logMessage("CameraManager: Stream is now playing successfully");
    emit streamStarted();
    emit streamStatusChanged(true);
  } else if (state == QMediaPlayer::StoppedState) {
    emit logMessage("CameraManager: Stream stopped");
    emit streamStopped();
    emit streamStatusChanged(false);
  } else if (state == QMediaPlayer::PausedState) {
    emit logMessage("CameraManager: Stream paused");
  }
}

void CameraManager::onMediaPlayerError(QMediaPlayer::Error error,
                                       const QString &errorString) {
  QString errorMsg =
      QString("Media Player Error (code %1): %2").arg(error).arg(errorString);
  emit logMessage("CameraManager::onMediaPlayerError:");
  emit logMessage(QString("  Error code: %1").arg(error));
  emit logMessage(QString("  Error string: %1").arg(errorString));
  emit logMessage(QString("  Current stream URL: %1").arg(m_currentStreamUrl));
  emit logMessage(QString("  Camera type: %1")
                      .arg(m_currentCameraType == CameraType::HTTP   ? "HTTP"
                           : m_currentCameraType == CameraType::RTSP ? "RTSP"
                                                                     : "None"));
  emit logMessage(
      QString("  Media player state: %1")
          .arg(m_mediaPlayer ? m_mediaPlayer->playbackState() : -1));

  // Provide more helpful error messages
  QString userFriendlyError = errorString;
  QString protocolHint;

  if (m_currentCameraType == CameraType::HTTP) {
    protocolHint = "HTTP path (try /video, /mjpeg, /stream, or leave empty)";

    // Check for specific error types
    if (errorString.contains("format", Qt::CaseInsensitive) ||
        errorString.contains("codec", Qt::CaseInsensitive) ||
        errorString.contains("not supported", Qt::CaseInsensitive)) {
      userFriendlyError =
          QString("Camera stream format not supported by QMediaPlayer.\n\n"
                  "URL: %1\n\n"
                  "Many IP cameras use MJPEG format which QMediaPlayer may not "
                  "support.\n"
                  "Please try:\n"
                  "- Different stream path (e.g., /video, /mjpeg, /stream)\n"
                  "- Check camera settings for H.264/MPEG stream option\n"
                  "- Use RTSP protocol if available")
              .arg(m_currentStreamUrl);
    } else if (errorString.contains("Could not open", Qt::CaseInsensitive) ||
               errorString.contains("file", Qt::CaseInsensitive)) {
      // "Could not open file" often means format/codec not supported
      // Even if HTTP connection succeeds, QMediaPlayer may not support the stream format
      userFriendlyError =
          QString("QMediaPlayer cannot play this stream format.\n\n"
                  "URL: %1\n\n"
                  "The camera is reachable (connection successful), but QMediaPlayer "
                  "does not support the stream format (likely MJPEG).\n\n"
                  "Possible solutions:\n"
                  "- Try different stream path (e.g., /video, /mjpeg, /stream)\n"
                  "- Check camera settings for H.264/MPEG-4 stream option\n"
                  "- Use RTSP protocol if camera supports it\n"
                  "- The stream may require a different player/decoder")
              .arg(m_currentStreamUrl);
    } else if (errorString.contains("Cannot connect", Qt::CaseInsensitive)) {
      userFriendlyError =
          QString("Cannot connect to camera at %1. Please check:\n"
                  "- Camera IP and port are correct\n"
                  "- Camera is powered on and connected to network\n"
                  "- %2\n"
                  "- Username/password are correct (if required)\n"
                  "- Firewall is not blocking the connection")
              .arg(m_currentStreamUrl, protocolHint);
    }
  } else if (m_currentCameraType == CameraType::RTSP) {
    protocolHint = "RTSP path (try /h264, /live, or /stream)";
    userFriendlyError =
        QString("Cannot connect to camera at %1. Please check:\n"
                "- Camera IP and port are correct\n"
                "- Camera is powered on and connected to network\n"
                "- %2\n"
                "- Username/password are correct (if required)\n"
                "- Firewall is not blocking the connection")
            .arg(m_currentStreamUrl, protocolHint);
  } else {
    userFriendlyError = errorString;
  }

  emit streamError(userFriendlyError);
  emit streamStatusChanged(false);
}
