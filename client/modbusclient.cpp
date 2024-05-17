#include "modbusclient.h"

ModbusClient::ModbusClient(QObject *parent)
    : QObject{parent}
{
    createClient();
}

ModbusClient::~ModbusClient()
{
    if(client_) {
        client_->disconnectDevice();
        delete client_;
    }
}

void ModbusClient::createClient()
{
    client_ = new QModbusTcpClient(this);
    connect(client_, &QModbusClient::errorOccurred,
            [this](QModbusDevice::Error) {
                emit uiMessage("Ошибка: " + client_->errorString());
            });

    if(!client_)
        emit uiMessage(tr("Не удалось создать Modbus клиент"));
    else
        connect(client_, &QModbusClient::stateChanged,
                this, &ModbusClient::onModbusStateChanged);
}

void ModbusClient::connectToServer(ServerConf conf)
{
    server_ = conf;
    if(!client_)
        return;

    if(client_->state() != QModbusDevice::ConnectedState) {
        client_->setConnectionParameter(QModbusDevice::NetworkAddressParameter, server_.host);
        client_->setConnectionParameter(QModbusDevice::NetworkPortParameter, server_.port);
        if(client_->connectDevice()) {
//            saveServerConf();
        } else {
            emit uiMessage("Подключение не удалось: " + client_->errorString());
        }
    } else {
        client_->disconnectDevice();
    }
}

void ModbusClient::onModbusStateChanged(int state)
{
    switch(state)
    {
        case QModbusDevice::UnconnectedState:
            emit stateChanged(false);
            emit uiMessage("Нет подключения к серверу");
            break;
        case QModbusDevice::ConnectedState:
            emit stateChanged(true);
            emit uiMessage("Подключено к серверу");
            break;
        case QModbusDevice::ConnectingState:
            emit uiMessage("Попытка подключения к серверу");
            break;
        case QModbusDevice::ClosingState:
            emit stateChanged(false);
            emit uiMessage("Подключение закрыто");
            break;
    }
}

void ModbusClient::read(const int &address, const uint &startRegister, const uint &registersCount)
{
    if(!client_)
        return;

    if(auto *reply = client_->sendReadRequest(
                QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startRegister, registersCount),
                address)) {
        if(!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &ModbusClient::onReadReady);
        else
            delete reply;
    } else {
        emit uiMessage("Ошибка при запросе: " + client_->errorString());
    }
}

void ModbusClient::onReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if(!reply)
        return;

    if(reply->error() == QModbusDevice::NoError) {
        unit_ = reply->result();
        saveServerConf(reply->serverAddress(), unit_);
        emit readyRead();
    } else if(reply->error() == QModbusDevice::ProtocolError) {
        emit uiMessage(tr("Ошибка при получении: %1").
                                    arg(getErrorString(reply->error())));
    } else {
        emit uiMessage(tr("Ошибка при получении: %1").
                                    arg(getErrorString(reply->error())));
    }

    reply->deleteLater();
}

QString ModbusClient::getErrorString(int error)
{
    switch(error)
    {
    case QModbusDevice::NoError:
        return "No errors have occurred.";
    case QModbusDevice::ReadError:
        return "An error occurred during a read operation.";
    case QModbusDevice::WriteError:
        return "An error occurred during a write operation.";
    case QModbusDevice::ConnectionError:
        return "An error occurred when attempting to open the backend.";
    case QModbusDevice::ConfigurationError:
        return "An error occurred when attempting to set a configuration parameter.";
    case QModbusDevice::TimeoutError:
        return "A timeout occurred during I/O. An I/O operation did not finish within a given time frame.";
    case QModbusDevice::ProtocolError:
        return "A Modbus specific protocol error occurred.";
    case QModbusDevice::ReplyAbortedError:
        return "The reply was aborted due to a disconnection of the device.";
    case QModbusDevice::UnknownError:
    default:
        return "An unknown error occurred.";
    }
}

void ModbusClient::loadServerConf()
{
    QFile file(S_CONF);
    if(!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Ошибка чтения файла" << file.errorString();
        file.close();
        emit uiMessage("Ошибка чтения файла конфигурации сервера: " + file.errorString());
        return;
    }

    QByteArray json = file.readAll();
    file.close();
    QJsonParseError jErr;
    QJsonDocument jDoc = QJsonDocument::fromJson(json, &jErr);
    if(jErr.error != QJsonParseError::NoError) {
        qCritical() << "Ошибка при чтении конфигурации" << jErr.errorString();
        emit uiMessage("Ошибка при чтении конфигурации сервера: " + jErr.errorString());
        return;
    }

    QJsonObject jObj = jDoc.object();
    qDebug() << jObj;
    server_.host = jObj.value(S_HOST).toString();
    server_.port = jObj.value(S_PORT).toInt();
    server_.address = jObj.value(S_ADDRESS).toInt();

    QJsonArray jArr = jObj.value(HOLDING).toArray();
    foreach(QJsonValue value, jArr)
        server_.registers.append(value.toInt());
}

void ModbusClient::saveServerConf(const int &serverAddress, const QModbusDataUnit &unit)
{
    QFile file(S_CONF);
    if(!file.open(QIODevice::WriteOnly)) {
        qCritical() << "Ошибка записи в файл" << file.errorString();
        file.close();
        emit uiMessage("Ошибка записи в файл конфигурации сервера: " + file.errorString());
        return;
    }

    QJsonDocument jDoc;
    QJsonObject jObj;
    jObj.insert(S_HOST, QJsonValue(server_.host));
    jObj.insert(S_PORT, QJsonValue((int)server_.port));
    jObj.insert(S_ADDRESS, QJsonValue(serverAddress));

    QJsonArray jArr;
    qDebug() << unit.values();
    for(uint i = 0; i < unit.valueCount(); ++i) {
        jArr.append(unit.value(i));
    }
    jObj.insert(HOLDING, jArr);

    jDoc.setObject(jObj);
    file.write(jDoc.toJson());
    file.close();
}

ServerConf ModbusClient::getServerConf()
{
    return server_;
}

void ModbusClient::getModbusData(int &startAddress, QVector<quint16> &values) const
{
    values = unit_.values();
    startAddress = unit_.startAddress();
}
