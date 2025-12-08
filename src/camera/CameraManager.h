#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QVideoSink>
#include <QUrl>
#include <QLabel>
#include "models/DeviceConfigModel.h"

class MjpegStreamHandler;

/**
 * @brief Manages camera streams (HTTP MJPEG, RTSP, Iriun)
 * 
 * This class provides a unified interface for different camera stream types.
 * - HTTP MJPEG streams are handled by MjpegStreamHandler
 * - RTSP streams are handled by QMediaPlayer (if supported)
 * - Iriun streams are placeholder for future implementation
 */
class CameraManager : public QObject
{
    Q_OBJECT

public:
    explicit CameraManager(QObject *parent = nullptr);
    ~CameraManager();
    
    /**
     * @brief Set video output widget for RTSP streams
     * @param videoWidget QVideoWidget to display video
     */
    void setVideoOutput(QVideoWidget *videoWidget);
    
    /**
     * @brief Set video output sink for RTSP streams
     * @param videoSink QVideoSink to receive video frames
     */
    void setVideoOutput(QVideoSink *videoSink);
    
    /**
     * @brief Set QLabel for MJPEG frame display
     * @param label QLabel to display MJPEG frames
     */
    void setMjpegOutput(QLabel *label);
    
    /**
     * @brief Start RTSP stream
     * @param url RTSP stream URL
     * @return true if stream start was initiated successfully
     */
    bool startRtspStream(const QString &url);
    
    /**
     * @brief Start HTTP MJPEG stream
     * @param url HTTP stream URL
     * @param username Optional username for authentication
     * @param password Optional password for authentication
     * @return true if stream start was initiated successfully
     */
    bool startHttpStream(const QString &url, const QString &username = QString(), const QString &password = QString());
    
    /**
     * @brief Start Iriun stream (placeholder)
     * @param deviceId Iriun device ID
     * @return false (not implemented)
     */
    bool startIriunStream(const QString &deviceId);
    
    /**
     * @brief Stop current stream
     */
    void stopStream();
    
    /**
     * @brief Check if currently streaming
     * @return true if streaming
     */
    bool isStreaming() const;
    
    /**
     * @brief Get current camera type
     * @return Current CameraType
     */
    CameraType currentCameraType() const;

signals:
    /**
     * @brief Emitted when stream starts successfully
     */
    void streamStarted();
    
    /**
     * @brief Emitted when stream stops
     */
    void streamStopped();
    
    /**
     * @brief Emitted when stream error occurs
     * @param error Error message
     */
    void streamError(const QString &error);
    
    /**
     * @brief Emitted when stream status changes
     * @param isActive true if streaming, false otherwise
     */
    void streamStatusChanged(bool isActive);
    
    /**
     * @brief Emitted for logging messages
     * @param message Log message
     */
    void logMessage(const QString &message);

private slots:
    void onMediaPlayerStateChanged(QMediaPlayer::PlaybackState state);
    void onMediaPlayerError(QMediaPlayer::Error error, const QString &errorString);
    void onMjpegFrameReceived(const QImage &frame);
    void onMjpegError(const QString &error);
    void onMjpegConnected();
    void onMjpegDisconnected();

private:
    QMediaPlayer *m_mediaPlayer;
    QVideoWidget *m_videoWidget;
    QVideoSink *m_videoSink;
    MjpegStreamHandler *m_mjpegHandler;
    CameraType m_currentCameraType;
    QString m_currentStreamUrl;
    
    void setupMediaPlayer();
    void cleanupMediaPlayer();
    void stopAllStreams();
};

#endif // CAMERAMANAGER_H
