#ifndef MODBUSTCPSERVER_H
#define MODBUSTCPSERVER_H

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QThread>
#include <QElapsedTimer>
#include <modbus/modbus.h>
#include "modbuscontext.h"
#include "modbusreceiver.h"

#define CONF "../conf/server.json"
#define HOST "host"
#define PORT "port"
#define HOLDING "holding_registers"

class ModbusTcpServer : public QObject
{
    Q_OBJECT

public:
    ModbusTcpServer(QObject *parent = nullptr);
    ~ModbusTcpServer();
    void setHost();
    void getHost();

private:
    QElapsedTimer timer_;
    ModbusContext context_;
    QList<QThread*> threads_;

    void addNewReceiver(int socket);
    int setModbus();
    void setRegisters(QVector<uint16_t> tab_registers);
    void processingQuery(int rc, uint8_t *query);
    uint calculateSecondsCount();
    bool load();
    void save();

signals:
    void waitForReadyRead(int);
    void finished();
};

#endif // MODBUSTCPSERVER_H
