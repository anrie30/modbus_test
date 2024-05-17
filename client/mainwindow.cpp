#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    load();
    initActions();
    initModbus();
}

MainWindow::~MainWindow()
{
    if(modbusClient)
        delete modbusClient;
    if(timer.isActive())
        timer.stop();
    delete ui;
}

void MainWindow::initActions()
{
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionExit->setEnabled(true);
    ui->buttonConnect->show();
    ui->buttonDisconnect->hide();
    ui->buttonAutoRead->show();
    ui->buttonAutoStop->hide();

    connect(ui->actionConnect, &QAction::triggered,
            this, &MainWindow::onConnectButtonClicked);
    connect(ui->actionDisconnect, &QAction::triggered,
            this, &MainWindow::onConnectButtonClicked);
    connect(ui->buttonConnect, &QPushButton::clicked,
            this, &MainWindow::onConnectButtonClicked);
    connect(ui->buttonDisconnect, &QPushButton::clicked,
            this, &MainWindow::onConnectButtonClicked);

    connect(ui->actionLoad, &QAction::triggered,
            this, &MainWindow::load);
    connect(ui->actionSave, &QAction::triggered,
            this, &MainWindow::save);

    connect(ui->readButton, &QPushButton::clicked,
            this, &MainWindow::onReadButtonClicked);
    connect(&timer, &QTimer::timeout, this, &MainWindow::onReadButtonClicked);

    // Запрос регистров по таймеру
    connect(ui->buttonAutoRead, &QPushButton::clicked, this, [&]()
    {
        timer.start(ui->spinInterval->value());
        if(timer.isActive()) {
            ui->buttonAutoStop->show();
            ui->buttonAutoRead->hide();
        }
    });

    // Остановка таймера запросов
    connect(ui->buttonAutoStop, &QPushButton::clicked, this, [&]()
    {
        timer.stop();
        if(!timer.isActive()) {
            ui->buttonAutoStop->hide();
            ui->buttonAutoRead->show();
        }
    });

    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);

}

void MainWindow::initModbus()
{
    modbusClient = new ModbusClient(this);
    connect(modbusClient, &ModbusClient::stateChanged,
            this, &MainWindow::onModbusStateChanged);
    connect(modbusClient, &ModbusClient::readyRead,
            this, &MainWindow::onModbusReadyRead);

    connect(modbusClient, &ModbusClient::uiMessage, this,
            [&](QString message) {
                statusBar()->showMessage(message, 5000);
            });
}

void MainWindow::onConnectButtonClicked()
{
    statusBar()->clearMessage();
    ServerConf server;
    server.host = ui->lineIP->text();
    server.port = ui->linePort->text().toUInt();
    server.address = ui->spinAddress->text().toInt();
    modbusClient->connectToServer(server);
}

void MainWindow::onReadButtonClicked()
{
    ui->listWidget->clear();
    statusBar()->clearMessage();
    quint16 numberOfEntries = ui->spinRegistersCount->value();
    modbusClient->read(ui->spinAddress->value(),
                       ui->spinStartRegister->value(),
                       numberOfEntries);
}

void MainWindow::onModbusStateChanged(bool connected)
{
    qDebug() << __func__ << connected;
    ui->actionConnect->setEnabled(!connected);
    ui->actionDisconnect->setEnabled(connected);

    if(connected) {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->buttonConnect->hide();
        ui->buttonDisconnect->show();
    } else {
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
        ui->buttonConnect->show();
        ui->buttonDisconnect->hide();
    }
}

void MainWindow::onModbusReadyRead()
{
    int startAddress;
    QVector<quint16> data;
    modbusClient->getModbusData(startAddress, data);

    for(int i = 0, total = int(data.count()); i < total; ++i) {
        const QString entry = tr("%1: %2").arg(startAddress + i, 5)
                                 .arg(QString::number(data[i], 16), 5);
        ui->listWidget->addItem(entry);
    }

}

void MainWindow::load()
{
    QSettings set(CONF, QSettings::IniFormat);
    restoreGeometry(set.value(GEOMETRY).toByteArray());

    ui->lineIP->setText(set.value(S_HOST, "127.0.0.1").toString());
    ui->linePort->setText(set.value(S_PORT, 1502).toString());
    ui->spinAddress->setValue(set.value(S_ADDRESS, 1).toUInt());
    ui->spinStartRegister->setValue(set.value(START_REG, 0).toUInt());
    ui->spinRegistersCount->setValue(set.value(REG_NUM, 1).toInt());
    ui->spinInterval->setValue(set.value(INTERVAL, 1000).toInt());
}

void MainWindow::save()
{
    QSettings set(CONF, QSettings::IniFormat);
    set.setValue(GEOMETRY, saveGeometry());

    set.setValue(S_HOST, ui->lineIP->text());
    set.setValue(S_PORT, ui->linePort->text());
    set.setValue(S_ADDRESS, ui->spinAddress->text());
    set.setValue(START_REG, ui->spinStartRegister->value());
    set.setValue(REG_NUM, ui->spinRegistersCount->value());
    set.setValue(INTERVAL, ui->spinInterval->value());
}
