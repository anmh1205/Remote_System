#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

class TcpServer : public QObject
{
    Q_OBJECT

public:
    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer();
    
    bool start(quint16 port);
    void stop();
    bool isListening() const;
    quint16 serverPort() const;
    
    void sendToAllClients(const QByteArray &data);

signals:
    void clientConnected(QTcpSocket *socket);
    void clientDisconnected(QTcpSocket *socket);
    void dataReceived(QTcpSocket *socket, const QByteArray &data);
    void errorOccurred(const QString &error);

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onClientReadyRead();

private:
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;
};

#endif // TCPSERVER_H

