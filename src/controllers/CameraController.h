#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <QObject>
#include "camera/CameraManager.h"
#include "models/DeviceConfigModel.h"
#include "models/ConnectionStatusModel.h"

class CameraWidget;
class NetworkConfigModel;

/**
 * @brief Controller for camera operations
 * 
 * This controller manages camera connections by building URLs from NetworkConfigModel
 * and delegating to CameraManager. It handles state updates and error reporting.
 */
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
    
    /**
     * @brief Connect to camera using current configuration
     */
    void connectCamera();
    
    /**
     * @brief Disconnect from camera
     */
    void disconnectCamera();
    
    /**
     * @brief Start RTSP stream
     * @param url RTSP stream URL
     * @return true if successful
     */
    bool startRtspStream(const QString &url);
    
    /**
     * @brief Start HTTP stream
     * @param url HTTP stream URL
     * @return true if successful
     */
    bool startHttpStream(const QString &url);
    
    /**
     * @brief Start Iriun stream
     * @param deviceId Iriun device ID
     * @return true if successful
     */
    bool startIriunStream(const QString &deviceId);
    
    /**
     * @brief Stop current stream
     */
    void stopStream();
    
    /**
     * @brief Scan for Iriun cameras (placeholder)
     */
    void scanIriunCameras();

signals:
    /**
     * @brief Emitted when stream status changes
     * @param isActive true if streaming, false otherwise
     */
    void streamStatusChanged(bool isActive);
    
    /**
     * @brief Emitted when stream error occurs
     * @param error Error message
     */
    void streamError(const QString &error);
    
    /**
     * @brief Emitted for logging messages
     * @param message Log message
     */
    void logMessage(const QString &message);

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
    QTimer *m_connectTimer;  // Debounce timer to prevent duplicate connectCamera calls
    
    void setupConnections();
    QString buildStreamUrl() const;
    void scheduleConnectCamera();  // Debounced version
};

#endif // CAMERACONTROLLER_H
