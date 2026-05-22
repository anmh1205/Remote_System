#ifndef DEVICECONFIGMODEL_H
#define DEVICECONFIGMODEL_H

#include <QObject>
#include <QString>

enum class CameraType {
    None,
    RTSP,
    HTTP,
    Iriun
};

class DeviceConfigModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CameraType cameraType READ cameraType NOTIFY cameraTypeChanged)
    Q_PROPERTY(QString rtspUrl READ rtspUrl NOTIFY rtspUrlChanged)
    Q_PROPERTY(QString httpUrl READ httpUrl NOTIFY httpUrlChanged)
    Q_PROPERTY(QString iriunDeviceId READ iriunDeviceId NOTIFY iriunDeviceIdChanged)
    Q_PROPERTY(QString panTiltIp READ panTiltIp NOTIFY panTiltIpChanged)
    Q_PROPERTY(quint16 panTiltPort READ panTiltPort NOTIFY panTiltPortChanged)
    Q_PROPERTY(quint16 joystickPort READ joystickPort NOTIFY joystickPortChanged)

public:
    explicit DeviceConfigModel(QObject *parent = nullptr);
    
    CameraType cameraType() const { return m_cameraType; }
    QString rtspUrl() const { return m_rtspUrl; }
    QString httpUrl() const { return m_httpUrl; }
    QString iriunDeviceId() const { return m_iriunDeviceId; }
    QString panTiltIp() const { return m_panTiltIp; }
    quint16 panTiltPort() const { return m_panTiltPort; }
    quint16 joystickPort() const { return m_joystickPort; }
    
    void setCameraType(CameraType type);
    void setRtspUrl(const QString &url);
    void setHttpUrl(const QString &url);
    void setIriunDeviceId(const QString &deviceId);
    void setPanTiltIp(const QString &ip);
    void setPanTiltPort(quint16 port);
    void setJoystickPort(quint16 port);

signals:
    void cameraTypeChanged(CameraType type);
    void rtspUrlChanged(const QString &url);
    void httpUrlChanged(const QString &url);
    void iriunDeviceIdChanged(const QString &deviceId);
    void panTiltIpChanged(const QString &ip);
    void panTiltPortChanged(quint16 port);
    void joystickPortChanged(quint16 port);

private:
    CameraType m_cameraType;
    QString m_rtspUrl;
    QString m_httpUrl;
    QString m_iriunDeviceId;
    QString m_panTiltIp;
    quint16 m_panTiltPort;
    quint16 m_joystickPort;
};

Q_DECLARE_METATYPE(CameraType)

#endif // DEVICECONFIGMODEL_H

