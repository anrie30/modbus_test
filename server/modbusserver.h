#ifndef ModbusServer_H
#define ModbusServer_H

//#include <modbus/modbus.h>
#include <QModbusTcpServer>
#include <QObject>
#include <QUrl>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define CONF "./server.json"
#define HOST "host"
#define PORT "port"
#define ADDRESS "address"
#define HOLDING "holding_registers"
#define FIRST "first_start"

class ModbusServer : public QObject
{
    Q_OBJECT

public:
    explicit ModbusServer(QObject *parent = nullptr);
    ~ModbusServer();
    void startConnection();

private:
//    uint16_t *tab_registers_;
//    modbus_mapping_t* mb_mapping_;
//    modbus_t *ctx_;
//    uint32_t timeoutSec_;
//    uint32_t timeoutUSec_;
    QModbusServer *server_ = nullptr;
    QModbusRequest *request_ = nullptr;
    QVector<quint16> registers_;
    QString host_;
    uint port_;
    uint serverAddress_ = 0;

    void setModbus();
    void save();
    void load();
//    void ModbusTcpServer();

private slots:
    void onStateChanged(int state);
    void handleDeviceError(QModbusDevice::Error newError);

signals:
    void quit(int exitCode);
};

#endif // ModbusServer_H
