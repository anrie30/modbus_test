#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initActions();
    setConnection();
}

MainWindow::~MainWindow()
{
    if(client_)
        client_->disconnectDevice();
    delete client_;

    delete ui;
}

void MainWindow::initActions()
{
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionExit->setEnabled(true);

    connect(ui->actionConnect, &QAction::triggered,
            this, &MainWindow::onConnectButtonClicked);
    connect(ui->actionDisconnect, &QAction::triggered,
            this, &MainWindow::onConnectButtonClicked);
    connect(ui->readButton, &QPushButton::clicked,
            this, &MainWindow::onReadButtonClicked);

    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
}

void MainWindow::setConnection()
{
    client_ = new QModbusTcpClient(this);
    if(ui->lineIP->text().isEmpty())
        ui->lineIP->setText(QLatin1String("127.0.0.1"));
    if(ui->linePort->text().isEmpty())
        ui->linePort->setText(QLatin1String("1502"));

    connect(client_, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
                statusBar()->showMessage(client_->errorString(), 5000);
            });

    if(!client_) {
        statusBar()->showMessage(tr("Не удалось создать Modbus клиент"), 5000);
    } else {
        connect(client_, &QModbusClient::stateChanged,
                this, &MainWindow::onModbusStateChanged);
    }
}

void MainWindow::onConnectButtonClicked()
{
    if(!client_)
        return;

    statusBar()->clearMessage();
    if(client_->state() != QModbusDevice::ConnectedState) {
        client_->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ui->lineIP->text());
        client_->setConnectionParameter(QModbusDevice::NetworkPortParameter, ui->linePort->text());
        if(client_->connectDevice()) {
            statusBar()->showMessage(tr("Подключено"), 5000);
            ui->actionConnect->setEnabled(false);
            ui->actionDisconnect->setEnabled(true);
        } else {
            statusBar()->showMessage(tr("Подключение не удалось: ") + client_->errorString(), 5000);
        }
    } else {
        client_->disconnectDevice();
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
    }
}

void MainWindow::onModbusStateChanged(int state)
{
    bool connected = (state != QModbusDevice::UnconnectedState);
    ui->actionConnect->setEnabled(!connected);
    ui->actionDisconnect->setEnabled(connected);

    if(state == QModbusDevice::UnconnectedState)
        ui->labelConnection->setText(tr("Разъединено"));
    else if(state == QModbusDevice::ConnectedState)
        ui->labelConnection->setText(tr("Подключено"));
}

void MainWindow::onReadButtonClicked()
{
    if(!client_)
        return;
    ui->listWidget->clear();
    statusBar()->clearMessage();

    if(auto *reply = client_->sendReadRequest(readRequest(), ui->spinAddress->value())) {
        if(!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::onReadReady);
        else
            delete reply;
    } else {
        statusBar()->showMessage(tr("Ошибка при запросе: ") + client_->errorString(), 5000);
    }
}

void MainWindow::onReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if(!reply)
        return;

    if(reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for(int i = 0, total = int(unit.valueCount()); i < total; ++i) {
            const QString entry = tr("%1: %2").arg(unit.startAddress() + i, 5)
                                     .arg(QString::number(unit.value(i), 16), 5);
            ui->listWidget->addItem(entry);
        }
    } else if(reply->error() == QModbusDevice::ProtocolError) {
        statusBar()->showMessage(tr("Ошибка при получении: %1 (Код ошибки Mobus: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        statusBar()->showMessage(tr("Ошибка при получении: %1 (Код ошибки: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }

    reply->deleteLater();
}

QModbusDataUnit MainWindow::readRequest() const
{
    qDebug() << __func__;
    int startAddress = ui->spinStartRegister->value();
    Q_ASSERT(startAddress >= 0 && startAddress < 10);

    quint16 numberOfEntries = qMin(ui->comboRegistersNumber->currentText().toUShort(), quint16(10 - startAddress));
    return QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
}

void MainWindow::load()
{
    QSettings set(CONF, QSettings::IniFormat);

    if(set.value(FIRST).toBool() == false) {
        save();
    }
    ui->lineIP->setText(set.value(SRV_HOST, "127.0.0.1").toString());
    ui->linePort->setText(set.value(SRV_PORT, 1502).toString());
    ui->spinAddress->setValue(set.value(SRV_ADDRESS, 1).toUInt());

//    QJsonArray jArr = set.value(HOLDING).toJsonArray();
//    foreach(QJsonValue value, jArr)
//        registers_.append(value.toInt());
//    QJsonObject jObj = set.value(HOLDING).toJsonObject();
//    foreach(QJsonValue value, jObj)
//        registers_.append(value.toInt());
}

void MainWindow::save()
{
    QSettings set(CONF, QSettings::IniFormat);
    set.setValue(FIRST, true);
    set.setValue(SRV_HOST, "127.0.0.1");
    set.setValue(SRV_PORT, 1502);
    set.setValue(SRV_ADDRESS, 1);

//    QJsonArray jArr;
//    for(int i = 0; i < 10; ++i) {
//        jArr.append(i);
//    }
//    set.setValue(HOLDING, jArr);
//    QJsonArray jObj;
//    for(int i = 0; i < 10; ++i) {
//        jObj.insert(i, i);
//    }
//    set.setValue(HOLDING, jObj);
}
