#ifndef MODBUSRECEIVER_H
#define MODBUSRECEIVER_H

#include <QObject>
#include <QDebug>
#include "modbuscontext.h"

class ModbusReceiver : public QObject
{
    Q_OBJECT

public:
    explicit ModbusReceiver(const ModbusContext &context, QObject *parent = nullptr);
    void waitForReadyRead(int socket);

private:
    ModbusContext context_;
    volatile bool stopped_ = false;

signals:
    void newConnection(int);
    void readyRead(int, uint8_t*);
    void finished();

};

#endif // MODBUSRECEIVER_H
