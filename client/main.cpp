#include <QApplication>
#include <QElapsedTimer>
//#include <QLoggingCategory>
#include "mainwindow.h"

QElapsedTimer elapsTimer;

ulong elapsedSecs()
{
    return elapsTimer.elapsed()/1000;
}

int main(int argc, char *argv[])
{
//    QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));
    elapsTimer.start();
//    connect()
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
