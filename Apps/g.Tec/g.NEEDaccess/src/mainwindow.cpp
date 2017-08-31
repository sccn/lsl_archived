#include <qfiledialog.h>
#include <QtXml>
#include <qdebug.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, const QString config_file)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    load_config(config_file);
    connect(&m_thread, SIGNAL(devicesUpdated(std::vector<GDSThread::GDS_DeviceInfo>)), this, SLOT(update_device_lists(std::vector<GDSThread::GDS_DeviceInfo>)));
    connect(&m_thread, SIGNAL(devicesConnected(bool)), this, SLOT(devices_connected_result(bool)));
    connect(this, SIGNAL(devicesAdded(QStringList)), &m_thread, SLOT(devicesAddedToStreamList(QStringList)));
    connect(this, SIGNAL(startStreams()), &m_thread, SLOT(startStreamsRequested()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_scanPushButton_clicked()
{
    ui->scanPushButton->setText("Scanning...");
    ui->scanPushButton->setDisabled(true);
    m_thread.setGDSParams(
        ui->lineEdit_serverip->text(),
        ui->serverPortSpinBox->value(),
        ui->clientPortSpinBox->value());
}

void MainWindow::on_toStreamPushButton_clicked()
{
    QList<QListWidgetItem *> selectedItems = ui->availableListWidget->selectedItems();
    QStringList transferredList;
    for (int i = 0; i < selectedItems.count(); ++i)
    {
        int row_ix = ui->availableListWidget->row(selectedItems[i]);
        QListWidgetItem *takenItem = ui->availableListWidget->takeItem(row_ix);
        ui->streamingListWidget->addItem(takenItem);
        transferredList << takenItem->text();
    }
    emit devicesAdded(transferredList);
}

void MainWindow::on_goPushButton_clicked()
{
    qDebug() << "on_goPushButton_clicked";
    emit startStreams();
}

void MainWindow::update_device_lists(std::vector<GDSThread::GDS_DeviceInfo> deviceInfos)
{
    if (deviceInfos.size() > 0)
    {
        ui->availableListWidget->clear();
        ui->streamingListWidget->clear();
        QStringList newDeviceNames;
        for (auto it = deviceInfos.begin(); it != deviceInfos.end(); ++it) {
            ui->availableListWidget->addItem(it->name);
        }
    }
    ui->scanPushButton->setText("Scan");
    ui->scanPushButton->setDisabled(false);
}

void MainWindow::devices_connected_result(bool result)
{
    ui->goPushButton->setDisabled(!result);
    ui->toAvailablePushButton->setDisabled(!result);
    if (!result)
    {
        while (ui->streamingListWidget->count() > 0)
        {
            ui->availableListWidget->addItem(ui->streamingListWidget->takeItem(0));
        }
    }
}

void MainWindow::load_config(const QString filename)
{
    QFile* xmlFile = new QFile(filename);
    if (!xmlFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not load XML from file " << filename;
        return;
    }
    QXmlStreamReader* xmlReader = new QXmlStreamReader(xmlFile);
    while (!xmlReader->atEnd() && !xmlReader->hasError()) {
        // Read next element
        xmlReader->readNext();
        if (xmlReader->isStartElement() && xmlReader->name() != "settings")
        {
            QStringRef elname = xmlReader->name();
            if (elname == "todo-setting-xml-element")
            {
                // TODO: Do something with the setting.
            }
        }
    }
    if (xmlReader->hasError()) {
        qDebug() << "Config file parse error "
            << xmlReader->error()
            << ": "
            << xmlReader->errorString();
    }
    xmlReader->clear();
    xmlFile->close();
}