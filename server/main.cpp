#include <QCoreApplication>
//#include <QLoggingCategory>
#include "modbusserver.h"

int exitCode = 0;
int execute = 0;

void quit(int code)
{
    qDebug() << __func__ << code;
    exitCode = code;
    if(execute > 0)
        qApp->exit(code);
    else
        execute = -1;
}

int main(int argc, char *argv[])
{
//    QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));
    QCoreApplication a(argc, argv);
    ModbusServer ms;
    QObject::connect(&ms, &ModbusServer::quit, &quit);
    ms.startConnection();

    if(execute >= 0) {
        execute = 1;
        exitCode = a.exec();
    }

    qDebug() << "Остановка" << argv[0];
    return exitCode;
}
