#include <signal.h>
#include <QCoreApplication>
//#include <QLoggingCategory>
#include "modbusserver.h"

void sigHandler(int signal)
{
    qDebug() << __func__ << signal;
    if(signal == SIGABRT || signal == SIGSEGV) {
        qApp->exit(signal);
    }
    else
        qApp->exit(0);
}

int main(int argc, char *argv[])
{
    int exitCode = 0;

    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);
    signal(SIGABRT, sigHandler);
    signal(SIGSEGV, sigHandler);
    signal(SIGKILL, sigHandler);
    signal(SIGQUIT, sigHandler);

//    QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));
    QCoreApplication a(argc, argv);
    ModbusServer ms;
    ms.startConnection();

    exitCode = a.exec();

    qDebug() << "Остановка" << argv[0];
    return exitCode;
}
