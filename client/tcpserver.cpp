#include "tcpserver.h"

TcpServer::TcpServer(quint16 port, QObject *parent)
    : QObject{parent}
{
    if(server.listen(QHostAddress::Any, port)){
        qInfo() << "Ожидание подключения на порту" << port;
        connect(&server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
    }
    else
        qCritical() << server.errorString();
}

void TcpServer::setElapsTime(ulong secs)
{

}

void TcpServer::onNewConnection()
{
    qInfo() << "Новое подключение";
    QTcpSocket *socket = new QTcpSocket(this);
    clientList.append(socket);
    connect(socket, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead);
}

void TcpServer::onReadyRead()
{
    qDebug() << __func__;
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    QByteArray message = socket->readAll();
    qDebug() << __func__ << message;

//    QByteArray message;
    socket->write(message);
}

//void TcpServer::writeMessage(QTcpSocket *socket, QByteArray message)
//{
//    qDebug() << __func__;
//    QByteArray message;
//    socket->write(message);
//}
