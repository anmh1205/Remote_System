#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QVideoSink>
#include <QNetworkAccessManager>
#include <QUrl>
#include "models/DeviceConfigModel.h"

class CameraManager : public QObject
{
    Q_OBJECT

public:
    explicit CameraManager(QObject *parent = nullptr);
    ~CameraManager();
    
    void setVideoOutput(QVideoWidget *videoWidget);
    void setVideoOutput(QVideoSink *videoSink);
    
    bool startRtspStream(const QString &url);
    bool startHttpStream(const QString &url);
    bool startIriunStream(const QString &deviceId);
    void stopStream();
    
    bool isStreaming() const;
    CameraType currentCameraType() const;

signals:
    void streamStarted();
    void streamStopped();
    void streamError(const QString &error);
    void streamStatusChanged(bool isActive);

private slots:
    void onMediaPlayerStateChanged(QMediaPlayer::PlaybackState state);
    void onMediaPlayerError(QMediaPlayer::Error error, const QString &errorString);

private:
    QMediaPlayer *m_mediaPlayer;
    QVideoWidget *m_videoWidget;
    QVideoSink *m_videoSink;
    QNetworkAccessManager *m_networkManager;
    CameraType m_currentCameraType;
    QString m_currentStreamUrl;
    
    void setupMediaPlayer();
    void cleanupMediaPlayer();
};

#endif // CAMERAMANAGER_H

