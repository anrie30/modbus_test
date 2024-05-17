#include "modbusreceiver.h"

ModbusReceiver::ModbusReceiver(const ModbusContext &context, QObject *parent)
    : QObject{parent}, context_(context)
{
    qDebug() << __func__;
}

void ModbusReceiver::waitForReadyRead(int socket)
{
    int rc;

    context_.accept = modbus_tcp_accept(context_.context, &socket);
    if(context_.accept <= 0) {
        qCritical() << "Ошибка при запуске сервера";
        emit finished();
        return;
    }

    qInfo() << "Принято подключение";
    emit newConnection(socket);

    while((context_.accept > 0) || (stopped_ == false)) {
        // Прием запросов от клиента
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
        rc = modbus_receive(context_.context, query);
        if(rc < 0) {
            qCritical() << "Ошибка! rc = -1";
//            emit finished();
//            break;
            continue;
        }

        // Проверка размера запроса
        if(rc > MODBUS_TCP_MAX_ADU_LENGTH) {
            qCritical() << "Принят запрос, превышающий максимальную длину";
//            emit finished();
//            break;
            continue;
        }

        emit readyRead(rc, query);
    }

    qDebug() << "Modbus TCP сервер прекратил прослушивание порта";
}
