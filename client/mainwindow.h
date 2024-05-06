#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusDataUnit>
#include <QModbusTcpClient>
//#include <QModbusRtuSerialMaster>
//#include <QStandardItemModel>
#include <QStatusBar>
//#include <QUrl>
#include <QSettings>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

QT_BEGIN_NAMESPACE

class QModbusClient;
class QModbusReply;

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

#define CONF "./server.ini"
#define SRV_HOST "host"
#define SRV_PORT "port"
#define SRV_ADDRESS "address"
#define HOLDING "holding_registers"
#define FIRST "first_start"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initActions();
    void setConnection();
    QModbusDataUnit readRequest() const;
    void load();
    void save();

private slots:
    void onConnectButtonClicked();
    void onModbusStateChanged(int state);

    void onReadButtonClicked();
    void onReadReady();

private:
    Ui::MainWindow *ui = nullptr;
    QModbusReply *lastRequest_ = nullptr;
    QModbusClient *client_ = nullptr;
};

#endif // MAINWINDOW_H
