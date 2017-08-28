#include <QFileDialog>
#include <QtXml>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, const QString config_file)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    load_config(config_file);
    connect(&m_thread, SIGNAL(openvrConnected(bool)), this, SLOT(update_connect_label(bool)));
    connect(&m_thread, SIGNAL(deviceListUpdated(QStringList)), this, SLOT(update_list_devices(QStringList)));
	connect(&m_thread, SIGNAL(outletsStarted(bool)), this, SLOT(update_stream_button(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load_config(const QString filename)
{
    QFile* xmlFile = new QFile(filename);
    if (!xmlFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not load XML from file " << filename;
        return;
    }
    QXmlStreamReader* xmlReader = new QXmlStreamReader(xmlFile);
    while(!xmlReader->atEnd() && !xmlReader->hasError()) {
        // Read next element
        xmlReader->readNext();
        if(xmlReader->isStartElement() && xmlReader->name() != "settings")
        {
            QStringRef elname = xmlReader->name();
            if (elname == "sampling-rate")
                ui->spinBox_sampling_rate->setValue(xmlReader->readElementText().toInt());
            if (elname == "origin-style")
            {
                ui->comboBox_origin->setCurrentIndex(xmlReader->readElementText().toInt());
            }
        }
    }
    if(xmlReader->hasError()) {
        qDebug() << "Config file parse error "
                 << xmlReader->error()
                 << ": "
                 << xmlReader->errorString();
    }
    xmlReader->clear();
    xmlFile->close();
}

void MainWindow::save_config(const QString filename)
{
    qDebug() << "save_config(" << filename << "); TODO: Write form contents to XML file.";
}

void MainWindow::on_actionLoad_Configuration_triggered()
{
    QString sel = QFileDialog::getOpenFileName(this,
                                               "Load Configuration File",
                                               "",//QDir::currentPath()
                                               "Configuration Files (*.cfg)");
    if (!sel.isEmpty())
    {
        load_config(sel);
    }
}

void MainWindow::on_actionSave_Configuration_triggered()
{
    QString sel = QFileDialog::getSaveFileName(this,"Save Configuration File","","Configuration Files (*.cfg)");
    if (!sel.isEmpty())
    {
        save_config(sel);
    }
}

void MainWindow::update_connect_label(bool status)
{
    if (status)
    {
        ui->label_conn_status->setText("Connected to OpenVR");
        ui->pushButton_stream->setText("Start Streams");
    }
    else
    {
        ui->label_conn_status->setText("Not Connected");
        ui->pushButton_stream->setText("No streams to start.");
    }
}

void MainWindow::update_list_devices(QStringList deviceList)
{
    ui->list_devices->clear();
    ui->list_devices->addItems(deviceList);
}

void MainWindow::update_stream_button(bool status)
{
	if (status)
	{
		ui->pushButton_stream->setText("Stop Streams");
	}
	else
	{
		ui->pushButton_stream->setText("Start Streams");
	}
}

void MainWindow::on_pushButton_scan_clicked()
{
    m_thread.initOpenVR(ui->spinBox_sampling_rate->value());
    ui->pushButton_scan->setText("Scanning...");
    ui->pushButton_scan->setDisabled(true);
}

void MainWindow::on_pushButton_stream_clicked()
{
	int originIndex = ui->comboBox_origin->currentIndex();
    QStringList devStringList;
    QList<QListWidgetItem *> lwi = ui->list_devices->selectedItems();
    for( int i=0; i<lwi.count(); ++i )
    {
        devStringList << lwi[i]->text();
    }
    m_thread.startStreams(devStringList, originIndex);
}
