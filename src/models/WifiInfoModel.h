#ifndef WIFIINFOMODEL_H
#define WIFIINFOMODEL_H

#include <QObject>
#include <QString>
#include <QList>

struct WifiNetwork {
    QString ssid;
    QString password;
    int signalStrength;
    bool isConnected;
};

class WifiInfoModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<WifiNetwork> networks READ networks NOTIFY networksChanged)

public:
    explicit WifiInfoModel(QObject *parent = nullptr);
    
    QList<WifiNetwork> networks() const { return m_networks; }
    
    void refreshNetworks();
    void addNetwork(const WifiNetwork &network);
    void clearNetworks();

signals:
    void networksChanged();

private:
    QList<WifiNetwork> m_networks;
};

Q_DECLARE_METATYPE(WifiNetwork)
Q_DECLARE_METATYPE(QList<WifiNetwork>)

#endif // WIFIINFOMODEL_H

