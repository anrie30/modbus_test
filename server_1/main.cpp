#include <signal.h>
#include <QCoreApplication>
#include "modbustcpserver.h"

void sigHandler(int signal);

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    int exitCode = 0;

    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);
    signal(SIGABRT, sigHandler);
    signal(SIGSEGV, sigHandler);
    signal(SIGKILL, sigHandler);
    signal(SIGQUIT, sigHandler);

    ModbusTcpServer modbusServer;

    exitCode = app.exec();

    qInfo() << "Modbus TCP сервер остановлен.";
    return exitCode;
}

void sigHandler(int signal)
{
    qDebug() << __func__ << signal;
    if(signal == SIGABRT || signal == SIGSEGV) {
        qApp->exit(signal);
    }
    else
        qApp->exit(0);
}
