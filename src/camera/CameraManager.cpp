#include "CameraManager.h"
#include <QDebug>
#include <QUrl>

CameraManager::CameraManager(QObject *parent)
    : QObject(parent)
    , m_mediaPlayer(nullptr)
    , m_videoWidget(nullptr)
    , m_videoSink(nullptr)
    , m_networkManager(nullptr)
    , m_currentCameraType(CameraType::None)
{
    m_networkManager = new QNetworkAccessManager(this);
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

bool CameraManager::startRtspStream(const QString &url)
{
    stopStream();
    
    if (!m_mediaPlayer) {
        setupMediaPlayer();
    }
    
    QUrl streamUrl(url);
    if (!streamUrl.isValid()) {
        emit streamError("Invalid RTSP URL");
        return false;
    }
    
    m_mediaPlayer->setSource(streamUrl);
    m_mediaPlayer->play();
    
    m_currentCameraType = CameraType::RTSP;
    m_currentStreamUrl = url;
    
    return true;
}

bool CameraManager::startHttpStream(const QString &url)
{
    stopStream();
    
    // Placeholder: HTTP stream implementation
    // Will use QNetworkAccessManager or QMediaPlayer depending on stream format
    m_currentCameraType = CameraType::HTTP;
    m_currentStreamUrl = url;
    
    emit streamError("HTTP stream not yet implemented");
    return false;
}

bool CameraManager::startIriunStream(const QString &deviceId)
{
    stopStream();
    
    // Placeholder: Iriun stream implementation
    // Will connect to Iriun camera via discovered IP/port
    m_currentCameraType = CameraType::Iriun;
    m_currentStreamUrl = deviceId;
    
    emit streamError("Iriun stream not yet implemented");
    return false;
}

void CameraManager::stopStream()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
    }
    
    m_currentCameraType = CameraType::None;
    m_currentStreamUrl.clear();
    emit streamStopped();
}

bool CameraManager::isStreaming() const
{
    if (m_mediaPlayer) {
        return m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState;
    }
    return false;
}

CameraType CameraManager::currentCameraType() const
{
    return m_currentCameraType;
}

void CameraManager::onMediaPlayerStateChanged(QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::PlayingState) {
        emit streamStarted();
        emit streamStatusChanged(true);
    } else if (state == QMediaPlayer::StoppedState) {
        emit streamStopped();
        emit streamStatusChanged(false);
    }
}

void CameraManager::onMediaPlayerError(QMediaPlayer::Error error, const QString &errorString)
{
    Q_UNUSED(error)
    emit streamError(errorString);
    emit streamStatusChanged(false);
}

