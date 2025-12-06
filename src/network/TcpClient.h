#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient();
    
    void connectToHost(const QString &host, quint16 port);
    void disconnectFromHost();
    bool isConnected() const;
    
    void sendData(const QByteArray &data);
    
    void setAutoReconnect(bool enabled);
    void setReconnectInterval(int msec);

signals:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &error);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError error);
    void attemptReconnect();

private:
    QTcpSocket *m_socket;
    QString m_host;
    quint16 m_port;
    bool m_autoReconnect;
    QTimer *m_reconnectTimer;
};

#endif // TCPCLIENT_H

