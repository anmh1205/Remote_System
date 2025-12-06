#ifndef NETWORKCONFIGMODEL_H
#define NETWORKCONFIGMODEL_H

#include <QObject>
#include <QString>

class NetworkConfigModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString localIp READ localIp NOTIFY localIpChanged)
    Q_PROPERTY(QString subnetMask READ subnetMask NOTIFY subnetMaskChanged)
    Q_PROPERTY(QString defaultGateway READ defaultGateway NOTIFY defaultGatewayChanged)
    Q_PROPERTY(QString dnsServer READ dnsServer NOTIFY dnsServerChanged)
    Q_PROPERTY(QString cameraIp READ cameraIp NOTIFY cameraIpChanged)
    Q_PROPERTY(quint16 cameraPort READ cameraPort NOTIFY cameraPortChanged)
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
    QString panTiltIp() const { return m_panTiltIp; }
    QString joystickIp() const { return m_joystickIp; }
    
    void setLocalIp(const QString &ip);
    void setSubnetMask(const QString &mask);
    void setDefaultGateway(const QString &gateway);
    void setDnsServer(const QString &dns);
    void setCameraIp(const QString &ip);
    void setCameraPort(quint16 port);
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
    void panTiltIpChanged(const QString &ip);
    void joystickIpChanged(const QString &ip);

private:
    QString m_localIp;
    QString m_subnetMask;
    QString m_defaultGateway;
    QString m_dnsServer;
    QString m_cameraIp;
    quint16 m_cameraPort;
    QString m_panTiltIp;
    QString m_joystickIp;
};

#endif // NETWORKCONFIGMODEL_H

