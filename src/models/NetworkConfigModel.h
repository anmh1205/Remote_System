#ifndef NETWORKCONFIGMODEL_H
#define NETWORKCONFIGMODEL_H

#include <QObject>
#include <QString>

enum class CameraProtocol {
    HTTP,
    RTSP
};

class NetworkConfigModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString localIp READ localIp NOTIFY localIpChanged)
    Q_PROPERTY(QString subnetMask READ subnetMask NOTIFY subnetMaskChanged)
    Q_PROPERTY(QString defaultGateway READ defaultGateway NOTIFY defaultGatewayChanged)
    Q_PROPERTY(QString dnsServer READ dnsServer NOTIFY dnsServerChanged)
    Q_PROPERTY(QString cameraIp READ cameraIp NOTIFY cameraIpChanged)
    Q_PROPERTY(quint16 cameraPort READ cameraPort NOTIFY cameraPortChanged)
    Q_PROPERTY(int cameraProtocol READ cameraProtocolInt NOTIFY cameraProtocolChanged)
    Q_PROPERTY(QString cameraRtspPath READ cameraRtspPath NOTIFY cameraRtspPathChanged)
    Q_PROPERTY(QString cameraUsername READ cameraUsername NOTIFY cameraUsernameChanged)
    Q_PROPERTY(QString cameraPassword READ cameraPassword NOTIFY cameraPasswordChanged)
    Q_PROPERTY(QString panTiltIp READ panTiltIp NOTIFY panTiltIpChanged)
    Q_PROPERTY(QString joystickIp READ joystickIp NOTIFY joystickIpChanged)

public:
    explicit NetworkConfigModel(QObject *parent = nullptr);
    
    QString localIp() const { return m_localIp; }
    QString subnetMask() const { return m_subnetMask; }
    QString defaultGateway() const { return m_defaultGateway; }
    QString dnsServer() const { return m_dnsServer; }
    QString cameraIp() const { return m_cameraIp; }
    quint16 cameraPort() const { return m_cameraPort; }
    CameraProtocol cameraProtocol() const { return m_cameraProtocol; }
    int cameraProtocolInt() const { return static_cast<int>(m_cameraProtocol); }  // For Q_PROPERTY
    QString cameraRtspPath() const { return m_cameraRtspPath; }
    QString cameraUsername() const { return m_cameraUsername; }
    QString cameraPassword() const { return m_cameraPassword; }
    QString panTiltIp() const { return m_panTiltIp; }
    QString joystickIp() const { return m_joystickIp; }
    
    void setLocalIp(const QString &ip);
    void setSubnetMask(const QString &mask);
    void setDefaultGateway(const QString &gateway);
    void setDnsServer(const QString &dns);
    void setCameraIp(const QString &ip);
    void setCameraPort(quint16 port);
    void setCameraProtocol(int protocol);  // 0 = HTTP, 1 = RTSP
    void setCameraRtspPath(const QString &path);
    void setCameraUsername(const QString &username);
    void setCameraPassword(const QString &password);
    void setPanTiltIp(const QString &ip);
    void setJoystickIp(const QString &ip);
    
    void updateLocalNetworkInfo();

signals:
    void localIpChanged(const QString &ip);
    void subnetMaskChanged(const QString &mask);
    void defaultGatewayChanged(const QString &gateway);
    void dnsServerChanged(const QString &dns);
    void cameraIpChanged(const QString &ip);
    void cameraPortChanged(quint16 port);
    void cameraProtocolChanged(int protocol);
    void cameraRtspPathChanged(const QString &path);
    void cameraUsernameChanged(const QString &username);
    void cameraPasswordChanged(const QString &password);
    void panTiltIpChanged(const QString &ip);
    void joystickIpChanged(const QString &ip);

private:
    QString m_localIp;
    QString m_subnetMask;
    QString m_defaultGateway;
    QString m_dnsServer;
    QString m_cameraIp;
    quint16 m_cameraPort;
    CameraProtocol m_cameraProtocol;
    QString m_cameraRtspPath;
    QString m_cameraUsername;
    QString m_cameraPassword;
    QString m_panTiltIp;
    QString m_joystickIp;
};

#endif // NETWORKCONFIGMODEL_H

