#ifndef ModbusServer_H
#define ModbusServer_H

#include <QModbusTcpServer>
#include <QObject>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QElapsedTimer>
#include <QTimer>

#define CONF "../conf/server.json"
#define HOST "host"
#define PORT "port"
#define ADDRESS "address"
#define HOLDING "holding_registers"

class ModbusServer : public QObject
{
    Q_OBJECT

public:
    explicit ModbusServer(QObject *parent = nullptr);
    ~ModbusServer();
    void startConnection();

private:
    QTimer timer_;
    QModbusTcpServer *server_ = nullptr;
    QModbusRequest *request_ = nullptr;
    QVector<quint16> registers_;
    QElapsedTimer eTimer_;
    QString host_;
    uint port_;
    uint serverAddress_ = 0;
    uint interval_ = 1000;

    void setModbus();
    void save();
    bool load();
    void setRegisters(QVector<uint16_t> tab_registers);

private slots:
    void onStateChanged(int state);
    void handleDeviceError(QModbusDevice::Error newError);

signals:
    void quit(int exitCode);
};

#endif // ModbusServer_H
