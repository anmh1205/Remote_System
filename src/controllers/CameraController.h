#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <QObject>
#include "camera/CameraManager.h"
#include "models/DeviceConfigModel.h"
#include "models/ConnectionStatusModel.h"

class CameraWidget;
class NetworkConfigModel;

class CameraController : public QObject
{
    Q_OBJECT

public:
    explicit CameraController(QObject *parent = nullptr);
    ~CameraController();
    
    void setCameraWidget(CameraWidget *widget);
    void setVideoWidget(QVideoWidget *videoWidget);
    void setDeviceConfigModel(DeviceConfigModel *model);
    void setConnectionStatusModel(ConnectionStatusModel *model);
    void setNetworkConfigModel(NetworkConfigModel *model);
    
    void connectCamera();
    void disconnectCamera();
    
    bool startRtspStream(const QString &url);
    bool startHttpStream(const QString &url);
    bool startIriunStream(const QString &deviceId);
    void stopStream();
    
    void scanIriunCameras();

signals:
    void streamStatusChanged(bool isActive);
    void streamError(const QString &error);
    void logMessage(const QString &message);  // For logging to MainControlView

private slots:
    void onStreamStarted();
    void onStreamStopped();
    void onStreamError(const QString &error);

private:
    CameraManager *m_cameraManager;
    CameraWidget *m_cameraWidget;
    DeviceConfigModel *m_deviceConfigModel;
    ConnectionStatusModel *m_connectionStatusModel;
    NetworkConfigModel *m_networkConfigModel;
    QTimer *m_connectTimer;  // For debouncing connectCamera() calls
    
    void setupConnections();
    QString buildStreamUrl() const;
    void scheduleConnectCamera();  // Debounced version of connectCamera
};

#endif // CAMERACONTROLLER_H

