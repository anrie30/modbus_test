#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include <QObject>
#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

class QModbusClient;
class QModbusReply;

#define S_CONF "../conf/client.json"
#define S_HOST "host"
#define S_PORT "port"
#define S_ADDRESS "address"
#define HOLDING "holding_registers"

struct ServerConf
{
    QString host;
    uint port;
    uint address;
    QVector<quint16> registers;
};

class ModbusClient : public QObject
{
    Q_OBJECT

public:
    explicit ModbusClient(QObject *parent = nullptr);
    ~ModbusClient();
    void connectToServer(ServerConf conf);
    void read(const int &address, const uint &startRegister, const uint &registersCount);
    void getModbusData(int &startAddress, QVector<quint16> &values) const;
    void loadServerConf();
    void saveServerConf(const int &serverAddress, const QModbusDataUnit &unit);
    void setServerConf(const ServerConf &server);
    ServerConf getServerConf();

public slots:
    void onModbusStateChanged(int state);
    void onReadReady();

private:
    QModbusClient *client_ = nullptr;
    ServerConf server_;
    QModbusDataUnit unit_;

    void createClient();
    QString getErrorString(int error);

signals:
    void uiMessage(QString);
    void stateChanged(bool);
    void readyRead();
};

#endif // MODBUSCLIENT_H
