#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);

private:
    QList<QTcpSocket*> servers_;

    void connectToHost();

signals:

};

#endif // TCPCLIENT_H
