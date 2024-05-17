#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(quint16 port, QObject *parent = nullptr);

public slots:
    void setElapsTime(ulong secs);

private:
    QTcpServer server;
    QList<QTcpSocket*> clientList;

//    void writeMessage();

private slots:
    void onNewConnection();
    void onReadyRead();

signals:
    void elapsTimeRequested();
};

#endif // TCPSERVER_H
