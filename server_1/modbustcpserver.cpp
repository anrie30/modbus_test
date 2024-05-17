#include "modbustcpserver.h"

ModbusTcpServer::ModbusTcpServer(QObject *parent)
    : QObject{parent}
{
    qDebug() << __func__;
    timer_.start();

    // Загрузка параметров сервера
    if(!load()) {
        deleteLater();
        return;
    }

    // Настройка сервера
    int socket = setModbus();
    if(socket <= 0) {
        deleteLater();
        return;
    }

    addNewReceiver(socket);
}

ModbusTcpServer::~ModbusTcpServer()
{
    while(threads_.count()) {
        threads_.last()->quit();
        threads_.last()->wait();
        threads_.last()->deleteLater();
        threads_.removeLast();
    }

    // Освобождение ресурсов
    modbus_mapping_free(context_.mb_mapping);
    modbus_close(context_.context);
    modbus_free(context_.context);
}

void ModbusTcpServer::addNewReceiver(int socket)
{
    ModbusReceiver *receiver = new ModbusReceiver(context_);
    QThread *thread = new QThread(this);
    receiver->moveToThread(thread);

    connect(thread, &QThread::finished, receiver, &QObject::deleteLater);
    connect(this, &ModbusTcpServer::waitForReadyRead, receiver, &ModbusReceiver::waitForReadyRead);
    connect(receiver, &ModbusReceiver::newConnection, this, &ModbusTcpServer::addNewReceiver);
    connect(receiver, &ModbusReceiver::readyRead, this, &ModbusTcpServer::processingQuery);
    connect(receiver, &ModbusReceiver::finished, [&](){
        qDebug() << __func__ << "Modbus finished";
        thread->deleteLater();
        threads_.removeAll(thread);
    });

    thread->start();
    threads_.append(thread);
    emit waitForReadyRead(socket);
}

int ModbusTcpServer::setModbus()
{
    int socket;

    // Контекст Modbus TCP
    context_.context = modbus_new_tcp(context_.host, context_.port);
    if(context_.context == nullptr) {
        qCritical() << "Не удалось инициализировать контекст Modbus";
        return 0;
    }

    // Таймаут для соединения и ответа
    modbus_set_response_timeout(context_.context, context_.timeoutSec, context_.timeoutUSec);

    // Карта данных Modbus
    context_.mb_mapping = modbus_mapping_new(10, 0, 10, 0);
    if(context_.mb_mapping == NULL) {
        qDebug() << __func__ << 3;
        qCritical() << "Не удалось создать карту данных Modbus";
        modbus_free(context_.context);
        return 0;
    }

    // Открытие Modbus TCP соединения
    socket = modbus_tcp_listen(context_.context, 1);
    if(socket > 0)
        qInfo() << "Modbus TCP сервер прослушивает порт";

    return socket;
}

void ModbusTcpServer::processingQuery(int rc, uint8_t *query)
{
    qDebug() << __func__ << rc;

    // Обработка запроса на чтение регистров
    if(query[7] == MODBUS_FC_READ_HOLDING_REGISTERS) {
        int addr =(query[8] << 8) | query[9];  // Адрес регистра
        int count =(query[10] << 8) | query[11];  // Количество регистров

        qDebug() << __func__ << "addr:" << addr << count;
        if(addr >= 0) {// && addr + count <= 10) {
            QVector<uint16_t> tab_registers;
            tab_registers.append(calculateSecondsCount());
            setRegisters(tab_registers);

            // Отправка регистров
            rc = modbus_reply(context_.context, query, count * 2, context_.mb_mapping);
            qDebug() << __func__ << "modbus_reply:" << rc;
        } else {
            // Отправка исключение "Illegal Data Address"
            qCritical() << __func__ << "Ошибка данных";
            rc = modbus_reply_exception(context_.context, query, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
        }

        if(rc < 0) {
            qCritical() << __func__ << "Ошибка! rc = -1";
//            break;
        }
    }
}

void ModbusTcpServer::setRegisters(QVector<uint16_t> tab_registers)
{
    // Заполнение карты данными из tab_registers
    for(int i = 0; i < tab_registers.count(); ++i) {
        context_.mb_mapping->tab_registers[i] = tab_registers[i];
    }
}

uint ModbusTcpServer::calculateSecondsCount()
{
    return timer_.elapsed()/1000;
}

bool ModbusTcpServer::load()
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
    context_.host = jObj.value(HOST).toString().toUtf8().data();
    context_.port = jObj.value(PORT).toInt();

    return true;
}

void ModbusTcpServer::save()
{
    QFile file(CONF);
    if(!file.open(QIODevice::WriteOnly)) {
        qCritical() << "Ошибка записи в файл" << file.errorString();
        file.close();
        return;
    }

    QJsonDocument jDoc;
    QJsonObject jObj;
    jObj.insert(HOST, QJsonValue(context_.host));
    jObj.insert(PORT, QJsonValue(context_.port));

    jDoc.setObject(jObj);
    file.write(jDoc.toJson());
    file.close();
}
