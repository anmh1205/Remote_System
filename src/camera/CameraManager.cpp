#include "CameraManager.h"
#include "MjpegStreamHandler.h"
#include <QUrl>
#include <QLabel>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QDebug>

CameraManager::CameraManager(QObject *parent)
    : QObject(parent)
    , m_mediaPlayer(nullptr)
    , m_videoWidget(nullptr)
    , m_videoSink(nullptr)
    , m_mjpegHandler(nullptr)
    , m_currentCameraType(CameraType::None)
{
    m_mjpegHandler = new MjpegStreamHandler(this);
    
    // Connect MJPEG handler signals
    connect(m_mjpegHandler, &MjpegStreamHandler::frameReceived, 
            this, &CameraManager::onMjpegFrameReceived);
    connect(m_mjpegHandler, &MjpegStreamHandler::error, 
            this, &CameraManager::onMjpegError);
    connect(m_mjpegHandler, &MjpegStreamHandler::connected, 
            this, &CameraManager::onMjpegConnected);
    connect(m_mjpegHandler, &MjpegStreamHandler::disconnected, 
            this, &CameraManager::onMjpegDisconnected);
    connect(m_mjpegHandler, &MjpegStreamHandler::logMessage, 
            this, &CameraManager::logMessage);
    
    setupMediaPlayer();
}

CameraManager::~CameraManager()
{
    stopStream();
    cleanupMediaPlayer();
}

void CameraManager::setupMediaPlayer()
{
    m_mediaPlayer = new QMediaPlayer(this);
    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, 
            this, &CameraManager::onMediaPlayerStateChanged);
    connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, 
            this, &CameraManager::onMediaPlayerError);
    
    // Log available backends for debugging
    emit logMessage(QString("CameraManager: Available audio backends: %1")
                    .arg(QMediaDevices::defaultAudioOutput().description()));
}

void CameraManager::cleanupMediaPlayer()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        m_mediaPlayer->deleteLater();
        m_mediaPlayer = nullptr;
    }
}

void CameraManager::setVideoOutput(QVideoWidget *videoWidget)
{
    m_videoWidget = videoWidget;
    if (m_mediaPlayer && videoWidget) {
        m_mediaPlayer->setVideoOutput(videoWidget);
        m_videoSink = videoWidget->videoSink();
    }
}

void CameraManager::setVideoOutput(QVideoSink *videoSink)
{
    m_videoSink = videoSink;
    if (m_mediaPlayer && m_videoSink) {
        m_mediaPlayer->setVideoOutput(m_videoSink);
    }
}

void CameraManager::setMjpegOutput(QLabel *label)
{
    if (m_mjpegHandler) {
        m_mjpegHandler->setImageOutput(label);
    }
}

bool CameraManager::startRtspStream(const QString &url)
{
    emit logMessage(QString("CameraManager: Starting RTSP stream from %1").arg(url));
    
    stopAllStreams();
    
    if (!m_mediaPlayer) {
        setupMediaPlayer();
    }
    
    if (!m_mediaPlayer) {
        emit logMessage("CameraManager: Failed to create media player");
        emit streamError("Failed to initialize media player");
        return false;
    }
    
    QUrl streamUrl(url);
    if (!streamUrl.isValid()) {
        emit logMessage(QString("CameraManager: Invalid RTSP URL: %1").arg(url));
        emit streamError(QString("Invalid RTSP URL: %1").arg(url));
        return false;
    }
    
    // Set video output if available
    if (!m_videoWidget && !m_videoSink) {
        emit logMessage("CameraManager: Warning - No video output set for RTSP");
    }
    
    emit logMessage(QString("CameraManager: Setting RTSP source: %1").arg(streamUrl.toString()));
    
    // Set the source
    m_mediaPlayer->setSource(streamUrl);
    
    // Check if source was set successfully
    if (m_mediaPlayer->source().isEmpty()) {
        emit logMessage("CameraManager: Failed to set RTSP source");
        emit streamError("Failed to set RTSP source. QMediaPlayer may not support RTSP on this platform.");
        return false;
    }
    
    emit logMessage("CameraManager: Source set successfully, starting playback...");
    
    // Start playing
    m_mediaPlayer->play();
    
    m_currentCameraType = CameraType::RTSP;
    m_currentStreamUrl = url;
    
    emit logMessage("CameraManager: RTSP play command sent");
    return true;
}

bool CameraManager::startHttpStream(const QString &url, const QString &username, const QString &password)
{
    emit logMessage(QString("CameraManager: Starting HTTP MJPEG stream from %1").arg(url));
    
    stopAllStreams();
    
    if (!m_mjpegHandler) {
        emit logMessage("CameraManager: MJPEG handler not initialized");
        emit streamError("MJPEG handler not initialized");
        return false;
    }
    
    QUrl streamUrl(url);
    if (!streamUrl.isValid()) {
        emit logMessage(QString("CameraManager: Invalid HTTP URL: %1").arg(url));
        emit streamError(QString("Invalid HTTP URL: %1").arg(url));
        return false;
    }
    
    // Start MJPEG stream
    m_mjpegHandler->startStream(streamUrl, username, password);
    
    m_currentCameraType = CameraType::HTTP;
    m_currentStreamUrl = url;
    
    return true;
}

bool CameraManager::startIriunStream(const QString &deviceId)
{
    emit logMessage("CameraManager: Iriun stream not yet implemented");
    emit streamError("Iriun stream not yet implemented");
    return false;
}

void CameraManager::stopStream()
{
    stopAllStreams();
    emit streamStopped();
}

void CameraManager::stopAllStreams()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
    }
    
    if (m_mjpegHandler) {
        m_mjpegHandler->stopStream();
    }
    
    m_currentCameraType = CameraType::None;
    m_currentStreamUrl.clear();
}

bool CameraManager::isStreaming() const
{
    if (m_currentCameraType == CameraType::RTSP && m_mediaPlayer) {
        return m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState;
    } else if (m_currentCameraType == CameraType::HTTP && m_mjpegHandler) {
        return m_mjpegHandler->isStreaming();
    }
    return false;
}

CameraType CameraManager::currentCameraType() const
{
    return m_currentCameraType;
}

void CameraManager::onMediaPlayerStateChanged(QMediaPlayer::PlaybackState state)
{
    emit logMessage(QString("CameraManager: RTSP player state changed to %1").arg(state));
    
    if (state == QMediaPlayer::PlayingState) {
        emit logMessage("CameraManager: RTSP stream is now playing");
        emit streamStarted();
        emit streamStatusChanged(true);
    } else if (state == QMediaPlayer::StoppedState) {
        emit logMessage("CameraManager: RTSP stream stopped");
        emit streamStopped();
        emit streamStatusChanged(false);
    }
}

void CameraManager::onMediaPlayerError(QMediaPlayer::Error error, const QString &errorString)
{
    emit logMessage(QString("CameraManager: RTSP player error %1: %2").arg(error).arg(errorString));
    emit logMessage(QString("CameraManager: Error code: %1").arg(error));
    emit logMessage(QString("CameraManager: Current source: %1").arg(m_mediaPlayer ? m_mediaPlayer->source().toString() : "null"));
    
    QString userFriendlyError;
    
    // Provide more specific error messages
    if (errorString.contains("Could not open", Qt::CaseInsensitive) ||
        errorString.contains("file", Qt::CaseInsensitive)) {
        userFriendlyError = QString("RTSP stream cannot be opened.\n\n"
                                    "This error often means QMediaPlayer does not support RTSP on Windows.\n\n"
                                    "Possible solutions:\n"
                                    "1. Try using HTTP MJPEG stream instead (change protocol to HTTP)\n"
                                    "2. Install codec pack or VLC media player (provides RTSP support)\n"
                                    "3. Check if URL is correct: %1\n"
                                    "4. Verify camera supports RTSP and stream is active\n\n"
                                    "Note: QMediaPlayer RTSP support on Windows is limited.\n"
                                    "Consider using HTTP MJPEG stream for better compatibility.")
                                .arg(m_currentStreamUrl);
    } else {
        userFriendlyError = QString("RTSP stream error: %1\n\nPlease check:\n"
                                    "- Camera IP and port are correct\n"
                                    "- Camera is powered on and connected to network\n"
                                    "- RTSP path is correct\n"
                                    "- Username/password are correct (if required)\n"
                                    "- Firewall is not blocking the connection\n"
                                    "- QMediaPlayer supports RTSP on this platform")
                                .arg(errorString);
    }
    
    emit streamError(userFriendlyError);
    emit streamStatusChanged(false);
}

void CameraManager::onMjpegFrameReceived(const QImage &frame)
{
    // Frame is already displayed by MjpegStreamHandler if label is set
    // This signal can be used for additional processing if needed
    Q_UNUSED(frame);
}

void CameraManager::onMjpegError(const QString &error)
{
    emit logMessage(QString("CameraManager: MJPEG stream error: %1").arg(error));
    emit streamError(error);
    emit streamStatusChanged(false);
}

void CameraManager::onMjpegConnected()
{
    emit logMessage("CameraManager: MJPEG stream connected");
    emit streamStarted();
    emit streamStatusChanged(true);
}

void CameraManager::onMjpegDisconnected()
{
    emit logMessage("CameraManager: MJPEG stream disconnected");
    emit streamStopped();
    emit streamStatusChanged(false);
}
