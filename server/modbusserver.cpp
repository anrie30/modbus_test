#include "modbusserver.h"

ModbusServer::ModbusServer(QObject *parent)
    : QObject(parent)
{
    load();
    qDebug() << __func__ << Qt::endl << registers_;
    setModbus();
}

ModbusServer::~ModbusServer()
{
    if(server_)
        server_->disconnectDevice();
    delete server_;
/*
    // Освобождение ресурсов
    delete[] tab_registers_;
    modbus_mapping_free(mb_mapping_);
    modbus_close(ctx_);
    modbus_free(ctx_);
    qDebug() << "Modbus TCP сервер остановлен.";
*/
}

void ModbusServer::setModbus()
{
    server_ = new QModbusTcpServer(this);
    QModbusDataUnit mdu(QModbusDataUnit::HoldingRegisters, 0, registers_);
    QModbusDataUnitMap mduMap;
    mduMap.insert(QModbusDataUnit::HoldingRegisters, mdu);
    server_->setMap(mduMap);

    connect(server_, &QModbusServer::stateChanged, this, &ModbusServer::onStateChanged);
    connect(server_, &QModbusServer::errorOccurred, this, &ModbusServer::handleDeviceError);
//    connect(server_->, &QModbusServer::)

//    request_ = new QModbusRequest()
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
        emit quit(1);
        return;
    }
    if(!port_) {
        qCritical() << "Не задан порт сервера";
        emit quit(1);
        return;
    }

    qDebug() << __func__ << host_ << port_ << serverAddress_;
    server_->setConnectionParameter(QModbusDevice::NetworkAddressParameter, host_);
    server_->setConnectionParameter(QModbusDevice::NetworkPortParameter, port_);
    server_->setServerAddress(serverAddress_);
    if(!server_->connectDevice()) {
        qCritical() << "Подключение не удалось:" << server_->errorString();
        emit quit(1);
    }
}

void ModbusServer::load()
{
//    QSettings set(CONF, QSettings::IniFormat);

//    if(set.value(HOST).isNull()) {
//        save();
//    }
//    host_ = set.value(HOST, "127.0.0.1").toString();
//    port_ = set.value(PORT, 1502).toUInt();
//    serverAddress_ = set.value(ADDRESS, 1).toUInt();

//    qDebug() << __func__ << set.value(HOLDING);
//    QJsonArray jArr = set.value(HOLDING).toJsonArray();
//    foreach(QJsonValue value, jArr)
//        registers_.append(value.toInt());

    QFile file(CONF);
    if(!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Ошибка чтения файла" << file.errorString();
        file.close();
        emit quit(1);
        return;
    }

    QByteArray json = file.readAll();
    file.close();
    QJsonParseError jErr;
    QJsonDocument jDoc = QJsonDocument::fromJson(json, &jErr);
    if(jErr.error != QJsonParseError::NoError) {
        qCritical() << "Ошибка при чтении конфигурации" << jErr.errorString();
        emit quit(1);
        return;
    }

    QJsonObject jObj = jDoc.object();
    qDebug() << jObj;
    host_ = jObj.value(HOST).toString();
    port_ = jObj.value(PORT).toInt();
    serverAddress_ = jObj.value(ADDRESS).toInt();

    QJsonArray jArr = jObj.value(HOLDING).toArray();
    foreach(QJsonValue value, jArr)
        registers_.append(value.toString().toInt(nullptr,16));
}

void ModbusServer::save()
{
//    QSettings set(CONF, QSettings::IniFormat);
//    set.setValue(HOST, "127.0.0.1");
//    set.setValue(PORT, 1502);
//    set.setValue(ADDRESS, 1);

    QFile file(CONF);
    if(!file.open(QIODevice::WriteOnly)) {
        qCritical() << "Ошибка записи в файл" << file.errorString();
        file.close();
        emit quit(1);
        return;
    }

    QJsonDocument jDoc;
    QJsonObject jObj;
    jObj.insert(HOST, QJsonValue(host_));
    jObj.insert(PORT, QJsonValue((int)port_));
    jObj.insert(ADDRESS, QJsonValue((int)serverAddress_));

    QJsonArray jArr;
    for(int i = 0; i < 10; ++i) {
        jArr.append(i);
    }
    jObj.insert(HOLDING, jArr);

    jDoc.setObject(jObj);
    file.write(jDoc.toJson());
    file.close();
}



/*
void ModbusServer::ModbusTcpServer()
{
    int socket;
    int rc;

    // Контекст Modbus TCP
    ctx_ = modbus_new_tcp(ip_.toStdString().c_str(), port_);

    if(ctx_ == NULL) {
        qDebug() << "Не удалось инициализировать контекст Modbus";
        emit quit(1);
    }

    // Таймаут для соединения и ответа
    modbus_set_response_timeout(ctx_, timeoutSec_, timeoutUSec_);

    // Хранилище регистров
    tab_registers_ = new uint16_t[10];
    for(int i = 0; i < 10; ++i) {
        tab_registers_[i] = i + 1;
    }

    // Карта данных Modbus
    mb_mapping_ = modbus_mapping_new(10, 0, 10, 0);
    if(mb_mapping_ == NULL) {
        qDebug() << "Не удалось создать карту данных Modbus";
        modbus_free(ctx_);
        emit quit(1);
    }

    // Заполнение карты данными из tab_registers
    for(int i = 0; i < 10; ++i) {
        mb_mapping_->tab_registers[i] = tab_registers_[i];
    }

    // Открытие Modbus TCP соединения
    socket = modbus_tcp_listen(ctx_, 1);
    int accept = modbus_tcp_accept(ctx_, &socket);
    if(accept > 0)
        qDebug() << "Modbus TCP сервер запущен.";
    else {
        qDebug() << "Ошибка при запуске сервера.";
        emit quit(1);
    }

    while(accept > 0) {
        // Прием запросов от клиента
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
        rc = modbus_receive(ctx_, query);
        if(rc == -1) {
            break;
        }

        // Проверка размера запроса
        if(rc > MODBUS_TCP_MAX_ADU_LENGTH) {
            qDebug() << "Принят запрос, превышающий максимальную длину";
            break;
        }

        // Обработка запроса на чтение регистров
        if(query[7] == MODBUS_FC_READ_HOLDING_REGISTERS) {
            int addr =(query[8] << 8) | query[9];  // Адрес регистра
            int count =(query[10] << 8) | query[11];  // Количество регистров
            if(addr >= 0 && addr + count <= 10) {
                // Отправка регистров
                rc = modbus_reply(ctx_, query, count * 2, mb_mapping_);
            } else {
                // Отправка исключение "Illegal Data Address"
                rc = modbus_reply_exception(ctx_, query, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
            }

            if(rc == -1) {
                break;
            }
        }
    }
}
*/
