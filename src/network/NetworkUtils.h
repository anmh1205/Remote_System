#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H

#include <QObject>
#include <QString>
#include <QHostAddress>
#include <QList>

struct IriunCameraInfo {
    QString deviceId;
    QString ipAddress;
    quint16 port;
    QString name;
};

class NetworkUtils : public QObject
{
    Q_OBJECT

public:
    explicit NetworkUtils(QObject *parent = nullptr);
    
    static bool isValidIpAddress(const QString &ip);
    static bool isValidPort(quint16 port);
    static QList<QHostAddress> getLocalIpAddresses();
    static QString getLocalIpAddress();
    
    // Iriun camera discovery
    void startIriunDiscovery();
    void stopIriunDiscovery();
    QList<IriunCameraInfo> discoveredCameras() const;

signals:
    void iriunCameraDiscovered(const IriunCameraInfo &camera);

private slots:
    void onDiscoveryTimeout();

private:
    QList<IriunCameraInfo> m_discoveredCameras;
    // Placeholder for discovery mechanism
    // Will be implemented based on Iriun protocol
};

Q_DECLARE_METATYPE(IriunCameraInfo)

#endif // NETWORKUTILS_H

