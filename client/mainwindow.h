#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>
#include <QSettings>
#include <QTimer>
#include "modbusclient.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

#define CONF     "../conf/client.ini"
#define SRV_CONF "../conf/client.json"
#define START_REG   "startRegister"
#define REG_NUM     "registersCount"
#define INTERVAL    "autoReadInterval"
#define GEOMETRY    "geometry"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui = nullptr;
    ModbusClient *modbusClient = nullptr;
    QTimer timer;

    void initActions();
    void initModbus();
    void load();
    void save();
    ulong elapsedSecs();

private slots:
    void onConnectButtonClicked();
    void onReadButtonClicked();
    void onModbusStateChanged(bool connected);
    void onModbusReadyRead();
};

#endif // MAINWINDOW_H
