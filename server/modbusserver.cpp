#include "modbusserver.h"

ModbusServer::ModbusServer(QObject *parent)
    : QObject(parent)
{
    eTimer_.start();
    if(!load())
        return;

    for(int i = 0; i < 10; ++i)
        registers_.append(i);
    qDebug() << __func__ << Qt::endl << registers_;

    setModbus();
    connect(&timer_, &QTimer::timeout, [&](){
        registers_.clear();
        registers_.append(eTimer_.elapsed()/1000);
        QModbusDataUnit mdu(QModbusDataUnit::HoldingRegisters, 0, registers_);
        QModbusDataUnitMap mduMap;
        mduMap.insert(QModbusDataUnit::HoldingRegisters, mdu);
        server_->setMap(mduMap);
        qDebug() << registers_;
    });
    timer_.start(interval_);
}

ModbusServer::~ModbusServer()
{
    qDebug() << __func__;
    if(server_)
        server_->disconnectDevice();
    delete server_;
}

void ModbusServer::setModbus()
{
    server_ = new QModbusTcpServer(this);
    QModbusDataUnit mdu(QModbusDataUnit::HoldingRegisters, 0, registers_);
    QModbusDataUnitMap mduMap;
    mduMap.insert(QModbusDataUnit::HoldingRegisters, mdu);
    server_->setMap(mduMap);

    connect(server_, &QModbusTcpServer::stateChanged, this, &ModbusServer::onStateChanged);
    connect(server_, &QModbusTcpServer::errorOccurred, this, &ModbusServer::handleDeviceError);
}

void ModbusServer::handleDeviceError(QModbusDevice::Error newError)
{
    if(newError == QModbusDevice::NoError || !server_)
        return;
    qCritical() << __func__ << server_->errorString();
}

void ModbusServer::onStateChanged(int state)
{
    switch(state) {
        case QModbusDevice::UnconnectedState:
            qInfo() << "Разъединено";
            break;
        case QModbusDevice::ConnectedState:
            qInfo() << "Подключено";
            break;
        case QModbusDevice::ConnectingState:
            qDebug() << "Подключается";
            break;
        case QModbusDevice::ClosingState:
            qDebug() << "Закрывается";
            break;
        default:
            break;
    }
}

void ModbusServer::startConnection()
{
     if(host_.isEmpty()) {
        qCritical() << "Не задан ip адрес сервера";
        return;
    }
    if(!port_) {
        qCritical() << "Не задан порт сервера";
        return;
    }

    server_->setConnectionParameter(QModbusDevice::NetworkAddressParameter, host_);
    server_->setConnectionParameter(QModbusDevice::NetworkPortParameter, port_);
    server_->setServerAddress(serverAddress_);
    if(!server_->connectDevice()) {
        qCritical() << "Подключение не удалось:" << server_->errorString();
    }
}

bool ModbusServer::load()
{
    QFile file(CONF);
    if(!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Ошибка чтения файла" << file.errorString();
        file.close();
        return false;
    }

    QByteArray json = file.readAll();
    file.close();
    QJsonParseError jErr;
    QJsonDocument jDoc = QJsonDocument::fromJson(json, &jErr);
    if(jErr.error != QJsonParseError::NoError) {
        qCritical() << "Ошибка при чтении конфигурации" << jErr.errorString();
        return false;
    }

    QJsonObject jObj = jDoc.object();
    qDebug() << jObj;
    host_ = jObj.value(HOST).toString();
    port_ = jObj.value(PORT).toInt();
    serverAddress_ = jObj.value(ADDRESS).toInt();
    interval_ = jObj.value("interval").toInt();

    QJsonArray jArr = jObj.value(HOLDING).toArray();
    foreach(QJsonValue value, jArr)
        registers_.append(value.toInt());

    return true;
}

void ModbusServer::save()
{
    QFile file(CONF);
    if(!file.open(QIODevice::WriteOnly)) {
        qCritical() << "Ошибка записи в файл" << file.errorString();
        file.close();
        return;
    }

    QJsonDocument jDoc;
    QJsonObject jObj;
    jObj.insert(HOST, QJsonValue(host_));
    jObj.insert(PORT, QJsonValue((int)port_));
    jObj.insert(ADDRESS, QJsonValue((int)serverAddress_));
    jObj.insert("interval", QJsonValue((int)interval_));

    QJsonArray jArr;
    for(int i = 0; i < registers_.count(); ++i) {
        jArr.append(registers_.value(i));
    }

    jObj.insert(HOLDING, jArr);

    jDoc.setObject(jObj);
    file.write(jDoc.toJson());
    file.close();
}
